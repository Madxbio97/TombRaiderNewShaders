#version 150
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.0
#endif
#ifndef TR456_WATER_FLOW_STRENGTH
#define TR456_WATER_FLOW_STRENGTH 1.0
#endif
#ifndef TR456_WATER_FLOW_VERTEX_STRENGTH
#define TR456_WATER_FLOW_VERTEX_STRENGTH 0.68
#endif
#ifndef TR456_WATER_FLOW_WAVE_STRENGTH
#define TR456_WATER_FLOW_WAVE_STRENGTH 1.18
#endif
#ifndef TR456_WATER_FLOW_SPEED
#define TR456_WATER_FLOW_SPEED 1.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_STRENGTH
#define TR456_WATER_CONTACT_WAVE_STRENGTH 0.70
#endif
#ifndef TR456_WATER_CONTACT_WAVE_RADIUS
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_SPEED
#define TR456_WATER_CONTACT_WAVE_SPEED 1.20
#endif
#ifndef TR456_WATER_CONTACT_VERTEX_STRENGTH
#define TR456_WATER_CONTACT_VERTEX_STRENGTH 0.35
#endif
#ifndef TR456_WATER_CONTACT_NORMAL_STRENGTH
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.75
#endif
#ifndef TR456_WATER_CONTACT_COORD_MODE
#define TR456_WATER_CONTACT_COORD_MODE 1
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

vec3 contactWaveField(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float radius=720.0*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)));
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
   float energy=active*(.18+.82*sat(abs(c.w)*(1.0/9000.0)));
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

 vec2 flowDir=uParams.xy;
 float flowLen=length(flowDir);
 flowDir=(flowLen>.0001) ? normalize(flowDir) : normalize(vec2(.85,.38));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 vec2 flowPos=vec2(dot(wp.xz,flowDir),dot(wp.xz,side));

 float weight=smoothstep(.04,.86,sat(coord.w/32767.0));
 float time=uModelMatrix[3].x*(.62+flowLen*.18)*clamp(TR456_WATER_FLOW_SPEED,0.20,3.50);
 float waveStrength=clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,1.6);
 float streamTime=time*(1.10+flowLen*.18);
 float lateralDrift=sin(flowPos.y*.00110+streamTime*.21)*.36+
   sin(flowPos.y*.00205-streamTime*.14)*.18;
 float phaseMain=flowPos.x*.00215+lateralDrift-streamTime*.92;
 float phaseLong=flowPos.x*.00110+sin(flowPos.y*.00072+streamTime*.11)*.28-streamTime*.54;
 float phaseFast=flowPos.x*.00375+sin(flowPos.y*.00155-streamTime*.17)*.18-streamTime*1.42;
 float phaseCross=flowPos.x*.00165+flowPos.y*.00042-streamTime*.72;
 float waveTrain=sin(phaseMain)*.52+sin(phaseMain*2.0+.45)*.13+
   sin(phaseLong)*.22+sin(phaseFast)*.17+sin(phaseCross)*.06;
 float crossRoll=sin(flowPos.y*.00086+streamTime*.19)*sin(phaseLong*.73)*.12;
 float lowNoise=texture(sNoise,vec3(wp.xz*.00028+flowDir*streamTime*.015,streamTime*.010)).x*2.0-1.0;
 float breath=waveTrain*(.78+.42*waveStrength)+crossRoll+lowNoise*.06;
 vec3 contact=contactWaveField(p.xyz,time);

 float distFade=1.0-smoothstep(4800.0,15800.0,length(p.xyz));
 vec3 viewDir=normalize(-p.xyz+normal.xyz*.0001);
 float angleFade=smoothstep(.04,.26,abs(dot(normal.xyz,viewDir)));
 float amp=clamp(abs(uParams.w),18.0,82.0)*.28*
   clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,1.15)*
   clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.35)*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,1.20)*
   distFade*angleFade*(.35+.65*weight);

 float contactAmp=32.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*
   distFade*angleFade*(.35+.65*weight);
 p.xyz+=normal.xyz*(breath*amp+contact.z*contactAmp);

 vec2 slope=flowDir*(cos(phaseMain)*.46+cos(phaseMain*2.0+.45)*.20+
   cos(phaseLong)*.18+cos(phaseFast)*.28+cos(phaseCross)*.08)+
   side*(cos(flowPos.y*.00110+streamTime*.21)*.10+
   cos(phaseLong*.73)*.08+lowNoise*.025);
 slope+=contact.xy*TR456_WATER_CONTACT_NORMAL_STRENGTH;
 float normalWave=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,1.0)*
   clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,1.25);
 vec3 bentNormal=normalize(normal.xyz+vec3(-slope.x*.12,0.0,-slope.y*.12)*
   normalWave*distFade);
 vNormal=normalize(mix(normal.xyz,bentNormal,.56));

 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 vPos=p.xyz;
 vContactWave=contact;
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
