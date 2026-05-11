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
out vec3 vPos;
out vec2 vTexCoord;
out vec3 vrgb0;
out vec3 vrgb1;
out vec3 vrgb2;
out vec3 vrgb3;
out vec3 vrgb4;
out vec3 vrgb5;
in vec3 aCoord;
in vec4 aTexCoord;
in vec4 aExtra;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;
in vec4 aFlags;

float sat(float x){ return clamp(x,0.0,1.0); }

void main(){
 vTexCoord=aTexCoord.xy*.125;
 vec4 rgb0_r5=pow(aExtra,vec4(2.2));
 vec4 rgb1_g5=pow(aNormal,vec4(2.2));
 vec4 rgb2_b5=pow(aLight,vec4(2.2));
 vec4 rgb3_w=aColor;
 vec4 rgb4_p=aFlags;
 rgb3_w.xyz=pow(rgb3_w.xyz,vec3(2.2));
 rgb4_p.xyz=pow(rgb4_p.xyz,vec3(2.2));
 vrgb0=rgb0_r5.xyz;
 vrgb1=rgb1_g5.xyz;
 vrgb2=rgb2_b5.xyz;
 vrgb3=rgb3_w.xyz;
 vrgb4=rgb4_p.xyz;
 vrgb5=vec3(rgb0_r5.w,rgb1_g5.w,rgb2_b5.w);

 vec4 p=vec4(dot(uModelMatrix[0],vec4(aCoord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(aCoord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(aCoord.xyz,1.0)),1.0);
 vec3 wp=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);

 float time=uModelMatrix[3].x;
 float strength=clamp(TR456_WATER_SURFACE_VERTEX_STRENGTH,0.0,1.25);
 float waveStrength=clamp(TR456_WATER_SURFACE_VERTEX_WAVE,0.0,1.7);
 vec2 pos=wp.xz;
 vec2 axisA=normalize(vec2(.90,.43));
 vec2 axisB=normalize(vec2(-.32,.95));
 vec2 axisC=normalize(vec2(.58,-.82));
 float phaseA=dot(pos,axisA)*.00125+sin(dot(pos,axisB)*.00045+time*.10)*.22+time*.38;
 float phaseB=dot(pos,axisB)*.00082+sin(dot(pos,axisA)*.00052-time*.08)*.18-time*.26;
 float phaseC=dot(pos,axisC)*.00170+sin(dot(pos,vec2(.72,-.69))*.00058+time*.11)*.12+time*.48;
 float lowNoise=texture(sNoise,vec3(pos*.00022+vec2(time*.010,-time*.006),time*.007)).x*2.0-1.0;
 float wave=sin(phaseA)*.50+sin(phaseA*2.0+.55)*.09+
   sin(phaseB)*.26+sin(phaseC)*.15+lowNoise*.045;
 float distFade=1.0-smoothstep(9000.0,26000.0,length(p.xyz));
 float amp=42.0*strength*waveStrength*clamp(TR456_WATER_SURFACE_WAVE,0.0,1.45)*distFade;
 p.y+=wave*amp;

 vPos=p.xyz;
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                              dot(uViewMatrix[1].xyz,p.xyz),
                              dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
