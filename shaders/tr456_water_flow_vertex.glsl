#version 150
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.36
#endif
#ifndef TR456_WATER_FLOW_STRENGTH
#define TR456_WATER_FLOW_STRENGTH 1.30
#endif
#ifndef TR456_WATER_FLOW_VERTEX_STRENGTH
#define TR456_WATER_FLOW_VERTEX_STRENGTH 0.72
#endif
#ifndef TR456_WATER_FLOW_WAVE_STRENGTH
#define TR456_WATER_FLOW_WAVE_STRENGTH 1.22
#endif
#ifndef TR456_WATER_FLOW_SPEED
#define TR456_WATER_FLOW_SPEED 2.05
#endif
#ifndef TR456_WATER_FLOW_DIRECTION_SIGN
#define TR456_WATER_FLOW_DIRECTION_SIGN 1.0
#endif
#ifndef TR456_WATER_FLOW_ORIGINAL_DEFORMATION
#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION 0.85
#endif
#ifndef TR456_WATER_FLOW_SECONDARY_MOTION
#define TR456_WATER_FLOW_SECONDARY_MOTION 0.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_STRENGTH
#define TR456_WATER_CONTACT_WAVE_STRENGTH 1.12
#endif
#ifndef TR456_WATER_CONTACT_WAVE_RADIUS
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_SPEED
#define TR456_WATER_CONTACT_WAVE_SPEED 0.98
#endif
#ifndef TR456_WATER_CONTACT_VERTEX_STRENGTH
#define TR456_WATER_CONTACT_VERTEX_STRENGTH 0.46
#endif
#ifndef TR456_WATER_CONTACT_NORMAL_STRENGTH
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.95
#endif
#ifndef TR456_WATER_CONTACT_COORD_MODE
#define TR456_WATER_CONTACT_COORD_MODE 1
#endif
#ifndef TR456_WATER_MESH_SUBDIVISION
#define TR456_WATER_MESH_SUBDIVISION 0
#endif
uniform sampler3D sNoise;
uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform mat4 uShadowMatrix;
uniform vec4 uFogColor;
uniform vec4 uContacts[16];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uJoints[32 * 3];
uniform vec4 uLightPos[4];
uniform vec4 uLightCol[4];
uniform vec4 uAmbient[6];
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
uniform vec4 uTrWaterDrawInfo;
uniform vec4 uTrWaterMaterialProfile;
#if TR456_WATER_MESH_SUBDIVISION > 0
out vec2 gTexCoord;
out vec3 gColor;
out vec3 gLight;
out float gLayer;
out float gFog;
out vec3 gNormal;
out vec3 gPos;
out vec3 gWorldPos;
out vec3 gContactWave;
#endif
out vec2 vTexCoord;
out vec3 vColor;
out vec3 vLight;
out float vLayer;
out float vFog;
out vec3 vNormal;
out vec3 vPos;
out vec3 vContactWave;
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

float sat(float x){ return clamp(x,0.0,1.0); }

#define TR_TOGGLE_MESH_DISPLACEMENT uTrWaterToggle2.z
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float flowOriginalBypass(){
 return step(.5,uTrWaterMaterialProfile.y);
}

float isScreenContact(vec4 c){
 return (1.0-step(-.001,c.z))*step(-.001,c.x)*step(c.x,1.001)*
   step(-.001,c.y)*step(c.y,1.001);
}

vec3 contactWaveField(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float radius=720.0*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
    float contactOn=step(.001,dot(abs(c),vec4(1.0)))*(1.0-isScreenContact(c));
   vec2 deltaXZ=pos.xz-c.xz;
   vec2 deltaXY=pos.xy-c.xy;
   float dXZ=length(deltaXZ);
   float dXY=length(deltaXY);
   float autoXY=step(dXY,dXZ);
   float mode=float(TR456_WATER_CONTACT_COORD_MODE);
   float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
   vec2 delta=mix(deltaXZ,deltaXY,useXY);
   float d=length(delta)+.001;
   vec2 dir=delta/d;
   float vertical=1.0-smoothstep(radius*.08,radius*.78,abs(pos.y-c.y));
   float falloff=(1.0-smoothstep(radius*.20,radius*2.55,d))*exp(-d/(radius*1.35))*vertical;
   float energy=contactOn*(.18+.82*sat(abs(c.w)*(1.0/9000.0)));
   float phase=d*.030+float(i)*.417;
   float ring=sin(phase)*falloff;
   float ripple=sin(phase*1.72+1.15)*falloff*.34;
   float dRing=(cos(phase)*.030+cos(phase*1.72+1.15)*.017)*falloff;
   height+=(ring+ripple)*energy;
   slope+=dir*dRing*energy;
 }
 return vec3(slope*1.55*strength,height*strength);
}

void main(){
 vec4 coord=vec4(aCoord);
 vec4 normal=vec4(aNormal);
 vec4 light=aLight;
 vec4 color=aColor;

 vec2 uv=vec2(light.w,color.w);
 uv+=uParams.xy*uModelMatrix[3].x;
 vTexCoord=uv;

 normal.xyz=normalize(normal.xyz-127.0);
 vLight=pow(light.xyz,vec3(2.2));
 vColor=pow(color.xyz,vec3(2.2));
 vLayer=normal.w;

 vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);
 vec3 wp=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);

 if(flowOriginalBypass()>.5) {
  vNormal=normal.xyz;
  vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
  vPos=p.xyz;
  vContactWave=vec3(0.0);
#if TR456_WATER_MESH_SUBDIVISION > 0
  gTexCoord=vTexCoord;
  gColor=vColor;
  gLight=vLight;
  gLayer=vLayer;
  gFog=vFog;
  gNormal=normal.xyz;
  gPos=vPos;
  gWorldPos=wp;
  gContactWave=vec3(0.0);
#endif
  gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                               dot(uViewMatrix[1].xyz,p.xyz),
                               dot(uViewMatrix[2].xyz,p.xyz),p.w);
  return;
 }

 vec2 flowVector=uParams.xy;
 float flowLen=length(flowVector);
 float flowActive=1.0;
 float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 flowDir=((flowLen>.000001) ? flowVector/flowLen : normalize(vec2(.92,.38)))*flowSign;
 vec2 side=vec2(-flowDir.y,flowDir.x);
 vec2 flowPos=vec2(dot(uv,flowDir),dot(uv,side));
 vec2 worldFlowPos=vec2(dot(wp.xz,flowDir),dot(wp.xz,side));

 float weight=smoothstep(.04,.86,sat(coord.w/32767.0));
 float time=uModelMatrix[3].x*(.62+flowLen*.18)*
   clamp(TR456_WATER_FLOW_SPEED,0.20,35.0);
 float duplicatePass=step(0.5,uTrWaterDrawInfo.w);
 float passMotion=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,1.0),duplicatePass);
 time*=passMotion;
 float waveStrength=clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,1.8);
 float streamTime=time*.22;
 float motionTime=streamTime;
 vec2 wavePos=worldFlowPos*.0049;
 float lateralDrift=sin(wavePos.y*1.75-motionTime*.21)*.38+
   sin(wavePos.y*3.20+motionTime*.14)*.20;
 float phaseMain=wavePos.x*2.35+lateralDrift-motionTime*.24;
 float phaseLong=wavePos.x*1.18+sin(wavePos.y*.92-motionTime*.11)*.30-motionTime*.12;
 float phaseFast=wavePos.x*4.45+sin(wavePos.y*2.20+motionTime*.17)*.20-motionTime*.32;
 float phaseCross=wavePos.x*1.70+wavePos.y*.68-motionTime*.18;
 float waveTrain=sin(phaseMain)*.52+sin(phaseMain*2.0+.45)*.13+
   sin(phaseLong)*.25+sin(phaseFast)*.22+sin(phaseCross)*.08;
 float crossRoll=sin(worldFlowPos.y*.00086+streamTime*.19)*sin(phaseLong*.73)*.14;
 float lowNoise=texture(sNoise,vec3(worldFlowPos*.00095-
   flowDir*motionTime*.020,streamTime*.010)).x*2.0-1.0;
 float worldNoise=texture(sNoise,vec3(worldFlowPos*.00072-
   flowDir*motionTime*.018,streamTime*.010)).x*2.0-1.0;
 float geoPhaseA=worldFlowPos.x*.0058+sin(worldFlowPos.y*.0038+streamTime*.15)*.34-
   motionTime*.34;
 float geoPhaseB=worldFlowPos.x*.0029-worldFlowPos.y*.0017-motionTime*.20;
 float geoBreath=sin(geoPhaseA)*.98+sin(geoPhaseB)*.50+worldNoise*.14;
 float breath=(waveTrain*(.92+.58*waveStrength)+crossRoll)*waveStrength+
   lowNoise*.06;
 breath*=passMotion;
 vec3 contact=contactWaveField(wp,time)*TR_TOGGLE_CONTACT_RIPPLES*passMotion;

 float distFade=1.0-smoothstep(4800.0,15800.0,length(p.xyz));
 vec3 viewDir=normalize(-p.xyz+normal.xyz*.0001);
 float angleFade=mix(.58,1.0,smoothstep(.02,.20,abs(dot(normal.xyz,viewDir))));
  float amp=clamp(abs(uParams.w),18.0,128.0)*.68*
    clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.15)*
    clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.70)*
    clamp(TR456_WATER_SURFACE_WAVE,0.0,1.55)*
    distFade*angleFade*(.35+.65*weight);

  float contactAmp=32.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*
    distFade*angleFade*(.35+.65*weight);
  float originalDeform=clamp(TR456_WATER_FLOW_ORIGINAL_DEFORMATION,0.0,1.0);
  float disp=mix(breath,geoBreath,.96*originalDeform)*amp+contact.z*contactAmp;
  float vertexDisp=clamp(disp,-52.0,52.0);
  p.xyz+=normal.xyz*vertexDisp;

 vec2 slope=flowDir*(cos(phaseMain)*.46+cos(phaseMain*2.0+.45)*.20+
   cos(phaseLong)*.18+cos(phaseFast)*.28+cos(phaseCross)*.08)+
   side*(cos(flowPos.y*8.5+streamTime*.21)*.10+
   cos(phaseLong*.73)*.08+lowNoise*.025);
 slope+=contact.xy*TR456_WATER_CONTACT_NORMAL_STRENGTH;
  float normalWave=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.0)*
    clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,2.0)*passMotion;
  float breathNormal=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.0)*
    originalDeform*passMotion;
  vec2 breathSlope=flowDir*(cos(geoPhaseA)*.16+cos(geoPhaseB)*.08)+
    side*cos(worldFlowPos.y*.0038+streamTime*.15)*.06;
  vec3 bentNormal=normalize(normal.xyz+
    vec3(-(slope.x*.23*normalWave+breathSlope.x*.060*breathNormal),0.0,
         -(slope.y*.23*normalWave+breathSlope.y*.060*breathNormal))*distFade);
  vNormal=normalize(mix(normal.xyz,bentNormal,.66));

 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 vPos=p.xyz;
 vContactWave=contact;
#if TR456_WATER_MESH_SUBDIVISION > 0
 gTexCoord=vTexCoord;
 gColor=vColor;
 gLight=vLight;
 gLayer=vLayer;
 gFog=vFog;
 gNormal=normal.xyz;
 gPos=vPos;
 gWorldPos=wp;
 gContactWave=vec3(0.0);
#endif
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
