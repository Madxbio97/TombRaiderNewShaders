#version 150
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.36
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_STRENGTH
#define TR456_WATER_SURFACE_VERTEX_STRENGTH 0.82
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_WAVE
#define TR456_WATER_SURFACE_VERTEX_WAVE 1.58
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
out vec3 vWorldPos;
out vec3 vContactWave;
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

float sat(float x){ return clamp(x,0.0,1.0); }

vec2 gameSurfaceDir(){
 vec2 fallback=normalize(vec2(.86,.50));
 float d=dot(uParams.xy,uParams.xy);
 return (d>.000001) ? normalize(uParams.xy) : fallback;
}

#define TR_TOGGLE_MESH_DISPLACEMENT uTrWaterToggle2.z
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float isScreenContact(vec4 c){
 return (1.0-step(-.001,c.z))*step(-.001,c.x)*step(c.x,1.001)*
   step(-.001,c.y)*step(c.y,1.001);
}

float contactAge(vec4 c){
 if(isScreenContact(c)>.5) return max(abs(c.w)-1.0,0.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 return max(packedValue-radiusPacked*512.0-1.0,0.0);
}

float contactRadius(vec4 c){
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 float fallback=720.0;
 return mix(fallback,radiusPacked,step(96.0,radiusPacked))*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 contactWaveField(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 float life=160.0;
 vec2 slope=vec2(0.0);
 float height=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float contactOn=step(.001,dot(abs(c),vec4(1.0)))*(1.0-isScreenContact(c));
   if(contactOn<=.001) continue;
   float radius=contactRadius(c);
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
   float age=clamp(contactAge(c),0.0,life);
    float fade=contactOn*(1.0-smoothstep(life*.62,life,age));
    float vertical=1.0-smoothstep(radius*.24,radius*1.32,abs(pos.y-c.y));
    float grow=smoothstep(0.0,life*.74,age);
    float front=max(64.0,radius*(.52+.14*grow)+age*(1.35+speed*1.20));
    float width=max(60.0,radius*(.100+.036*grow));
   float crestX=(d-front)/width;
    float troughX=(d-(front-width*.92))/(width*1.58);
   float crest=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
    float shell=(1.0-smoothstep(radius*3.05,radius*4.30,d))*vertical;
    float phase=(d-front)*(.024+.003*speed);
    float fine=sin(phase)*crest*.10+sin(phase*1.58+1.35)*crest*.04;
    float ring=(crest*.62-trough*.24+fine)*shell;
    float dCrest=(-2.0*crestX/width)*crest;
    float dTrough=(-2.0*troughX/(width*1.58))*trough;
    float dFine=cos(phase)*(.024+.003*speed)*crest*.10+
      cos(phase*1.58+1.35)*(.038+.005*speed)*crest*.04;
    float dRing=(dCrest*.62-dTrough*.24+dFine)*shell;
    float source=exp(-d/(radius*.36))*(1.0-smoothstep(1.0,46.0,age))*vertical;
    float pressure=source*.26;
    float dSource=-source/(radius*.36);
    float energy=fade;
    height+=ring*energy;
    height+=pressure*energy;
    slope+=dir*(dRing+dSource*.26)*energy;
  }
  return vec3(slope*1.20*strength,height*strength);
}

void main(){
 vec4 coord=vec4(aCoord);
 vec4 normal=vec4(aNormal);
 vec4 light=aLight;
 vec4 color=aColor;

 vec2 uv=vec2(light.w,color.w);
 vTexCoord=uv;

 normal.xyz=normalize(normal.xyz-127.0);
 vLight=pow(light.xyz,vec3(2.2));
 vColor=pow(color.xyz,vec3(2.2));
 vLayer=normal.w;

 vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);
 vec3 wp=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);

 float time=uModelMatrix[3].x;
 float vertexStrength=clamp(TR456_WATER_SURFACE_VERTEX_STRENGTH,0.0,1.25);
 float waveStrength=clamp(TR456_WATER_SURFACE_VERTEX_WAVE,0.0,1.75);
 vec2 pos=wp.xz;
 vec2 axisA=gameSurfaceDir();
 vec2 axisB=vec2(-axisA.y,axisA.x);
 vec2 axisC=normalize(axisA*.38+axisB*.92);
 float phaseA=dot(pos,axisA)*.00105+sin(dot(pos,axisB)*.00055+time*.10)*.28+time*.34;
 float phaseB=dot(pos,axisB)*.00078+sin(dot(pos,axisA)*.00046-time*.07)*.20-time*.23;
 float phaseC=dot(pos,axisC)*.00155+sin(dot(pos,normalize(axisA*.71-axisB*.70))*.00062+time*.13)*.12+time*.42;
 float lowNoise=texture(sNoise,vec3(pos*.00024+vec2(time*.010,-time*.006),time*.008)).x*2.0-1.0;
 float wave=sin(phaseA)*.46+sin(phaseA*2.0+.65)*.08+
   sin(phaseB)*.30+sin(phaseC)*.18+lowNoise*.05;
 vec3 contact=contactWaveField(wp,time)*TR_TOGGLE_CONTACT_RIPPLES;

 float horizontal=smoothstep(.42,.78,abs(normal.y));
 float distFade=1.0-smoothstep(5200.0,16800.0,length(p.xyz));
 vec3 viewDir=normalize(-p.xyz+normal.xyz*.0001);
 float angleFade=smoothstep(.04,.24,abs(dot(normal.xyz,viewDir)));
 float amp=24.0*vertexStrength*waveStrength*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,1.45)*horizontal*distFade*angleFade;
 float disp=wave*amp;
  float contactAmp=34.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*
    horizontal*distFade*angleFade;
  disp+=contact.z*contactAmp;
#if TR456_WATER_MESH_SUBDIVISION > 0
  float vertexDisp=0.0;
#else
  float vertexDisp=0.0;
#endif
  p.xyz+=normal.xyz*vertexDisp;
  wp+=normal.xyz*vertexDisp;

 vec2 slope=axisA*(cos(phaseA)*.34+cos(phaseA*2.0+.65)*.12)+
   axisB*(cos(phaseB)*.24)+axisC*(cos(phaseC)*.20)+
   normalize(axisA*.71-axisB*.70)*(lowNoise*.025);
 slope+=contact.xy*TR456_WATER_CONTACT_NORMAL_STRENGTH;
 float normalWave=vertexStrength*waveStrength*horizontal*distFade*
   1.0;
 vec3 bentNormal=normalize(normal.xyz+vec3(-slope.x*.14,0.0,-slope.y*.14)*normalWave);
 vNormal=normalize(mix(normal.xyz,bentNormal,.62));

 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 vPos=p.xyz;
 vWorldPos=wp;
 vContactWave=contact;
#if TR456_WATER_MESH_SUBDIVISION > 0
 gTexCoord=vTexCoord;
 gColor=vColor;
 gLight=vLight;
 gLayer=vLayer;
 gFog=vFog;
 gNormal=normal.xyz;
 gPos=vPos;
 gWorldPos=vWorldPos;
 gContactWave=vec3(0.0);
#endif
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
