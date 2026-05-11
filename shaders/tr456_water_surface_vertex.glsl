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
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

float sat(float x){ return clamp(x,0.0,1.0); }

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

 float horizontal=smoothstep(.42,.78,abs(normal.y));
 float distFade=1.0-smoothstep(5200.0,16800.0,length(p.xyz));
 vec3 viewDir=normalize(-p.xyz+normal.xyz*.0001);
 float angleFade=smoothstep(.04,.24,abs(dot(normal.xyz,viewDir)));
 float amp=22.0*vertexStrength*waveStrength*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,1.25)*horizontal*distFade*angleFade;
 float disp=wave*amp;
 p.xyz+=normal.xyz*disp;
 wp+=normal.xyz*disp;

 vec2 slope=axisA*(cos(phaseA)*.34+cos(phaseA*2.0+.65)*.12)+
   axisB*(cos(phaseB)*.24)+axisC*(cos(phaseC)*.20)+
   vec2(.71,-.70)*(lowNoise*.025);
 float normalWave=vertexStrength*waveStrength*horizontal*distFade;
 vec3 bentNormal=normalize(normal.xyz+vec3(-slope.x*.11,0.0,-slope.y*.11)*normalWave);
 vNormal=normalize(mix(normal.xyz,bentNormal,.52));

 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 vPos=p.xyz;
 vWorldPos=wp;
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
