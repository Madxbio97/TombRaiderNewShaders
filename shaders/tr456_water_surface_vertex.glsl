#version 150
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.0
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_STRENGTH
#define TR456_WATER_SURFACE_VERTEX_STRENGTH 0.46
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_WAVE
#define TR456_WATER_SURFACE_VERTEX_WAVE 1.05
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
out vec3 vWorldPos;
out vec3 vContactWave;
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

float sat(float x){ return clamp(x,0.0,1.0); }

float isScreenContact(vec4 c){
 return 1.0-step(-.001,c.z);
}

float contactAge(vec4 c){
 if(isScreenContact(c)>.5) return max(abs(c.w)-1.0,0.0);
 float packed=abs(c.w);
 float r=floor(packed*(1.0/512.0));
 return max(packed-r*512.0-1.0,0.0);
}

float contactRadius(vec4 c){
 float packed=abs(c.w);
 float r=floor(packed*(1.0/512.0));
 float fallback=720.0;
 return mix(fallback,r,step(96.0,r))*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 contactWaveField(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 float life=138.0;
 vec2 slope=vec2(0.0);
 float height=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)))*(1.0-isScreenContact(c));
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
   float fade=active*(1.0-smoothstep(life*.72,life,age));
   float vertical=1.0-smoothstep(radius*.22,radius*1.24,abs(pos.y-c.y));
   float grow=smoothstep(0.0,life*.72,age);
   float front=max(54.0,radius*(.54+.18*grow)+age*(1.8+speed*1.6));
   float width=max(42.0,radius*(.070+.025*grow));
   float crestX=(d-front)/width;
   float troughX=(d-(front-width*.92))/(width*1.44);
   float crest=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
   float shell=(1.0-smoothstep(radius*2.70,radius*3.90,d))*vertical;
   float phase=(d-front)*(.030+.004*speed);
   float fine=sin(phase)*crest*.18+sin(phase*1.82+1.35)*crest*.07;
   float ring=(crest*.70-trough*.30+fine)*shell;
   float dCrest=(-2.0*crestX/width)*crest;
   float dTrough=(-2.0*troughX/(width*1.44))*trough;
   float dFine=cos(phase)*(.030+.004*speed)*crest*.18+
     cos(phase*1.82+1.35)*(.055+.007*speed)*crest*.07;
   float dRing=(dCrest*.70-dTrough*.30+dFine)*shell;
   float source=exp(-d/(radius*.34))*(1.0-smoothstep(1.0,30.0,age))*vertical;
   float pressure=source*.36;
   float dSource=-source/(radius*.34);
   float energy=fade*(.88+.12*sin(float(i)*1.37));
   height+=ring*energy;
   height+=pressure*energy;
   slope+=dir*(dRing+dSource*.38)*energy;
 }
 return vec3(slope*1.55*strength,height*strength);
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
 float vertexStrength=clamp(TR456_WATER_SURFACE_VERTEX_STRENGTH,0.0,1.0);
 float waveStrength=clamp(TR456_WATER_SURFACE_VERTEX_WAVE,0.0,1.4);
 vec2 pos=wp.xz;
 vec2 axisA=normalize(vec2(.86,.50));
 vec2 axisB=normalize(vec2(-.38,.93));
 vec2 axisC=normalize(vec2(.18,.98));
 float phaseA=dot(pos,axisA)*.00105+sin(dot(pos,axisB)*.00055+time*.10)*.28+time*.34;
 float phaseB=dot(pos,axisB)*.00078+sin(dot(pos,axisA)*.00046-time*.07)*.20-time*.23;
 float phaseC=dot(pos,axisC)*.00155+sin(dot(pos,vec2(.71,-.70))*.00062+time*.13)*.12+time*.42;
 float lowNoise=texture(sNoise,vec3(pos*.00024+vec2(time*.010,-time*.006),time*.008)).x*2.0-1.0;
 float wave=sin(phaseA)*.46+sin(phaseA*2.0+.65)*.08+
   sin(phaseB)*.30+sin(phaseC)*.18+lowNoise*.05;
 vec3 contact=contactWaveField(wp,time);

 float horizontal=smoothstep(.42,.78,abs(normal.y));
 float distFade=1.0-smoothstep(5200.0,16800.0,length(p.xyz));
 vec3 viewDir=normalize(-p.xyz+normal.xyz*.0001);
 float angleFade=smoothstep(.04,.24,abs(dot(normal.xyz,viewDir)));
 float amp=22.0*vertexStrength*waveStrength*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,1.25)*horizontal*distFade*angleFade;
 float disp=wave*amp;
 float contactAmp=34.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*
   horizontal*distFade*angleFade;
 disp+=contact.z*contactAmp;
 p.xyz+=normal.xyz*disp;
 wp+=normal.xyz*disp;

 vec2 slope=axisA*(cos(phaseA)*.34+cos(phaseA*2.0+.65)*.12)+
   axisB*(cos(phaseB)*.24)+axisC*(cos(phaseC)*.20)+
   vec2(.71,-.70)*(lowNoise*.025);
 slope+=contact.xy*TR456_WATER_CONTACT_NORMAL_STRENGTH;
 float normalWave=vertexStrength*waveStrength*horizontal*distFade;
 vec3 bentNormal=normalize(normal.xyz+vec3(-slope.x*.11,0.0,-slope.y*.11)*normalWave);
 vNormal=normalize(mix(normal.xyz,bentNormal,.52));

 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 vPos=p.xyz;
 vWorldPos=wp;
 vContactWave=contact;
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
