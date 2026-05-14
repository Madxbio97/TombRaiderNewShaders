#version 150
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.36
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_STRENGTH
#define TR456_WATER_SURFACE_VERTEX_STRENGTH 0.72
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_WAVE
#define TR456_WATER_SURFACE_VERTEX_WAVE 1.48
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
uniform vec4 uContactMotion[16];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uTrWaterToggle2;
uniform vec4 uJoints[32 * 3];
uniform vec4 uLightPos[4];
uniform vec4 uLightCol[4];
uniform vec4 uAmbient[6];
out vec3 vPos;
out vec2 vTexCoord;
out vec3 vContactWave;
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

#define TR_TOGGLE_MESH_DISPLACEMENT uTrWaterToggle2.z
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }

float reflectContactRadius(vec4 c){
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 float nativeRadius=clamp(packedValue*.025,96.0,320.0);
 float repackedRadius=mix(720.0,radiusPacked,
   step(96.0,radiusPacked)*step(radiusPacked,680.0));
 float repacked=step(49152.0,packedValue);
 return mix(nativeRadius,repackedRadius,repacked)*
   clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 contactWaveField(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,3.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)));
   float radius=reflectContactRadius(c);
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
   float vertical=1.0-smoothstep(radius*.10,radius*1.05,abs(pos.y-c.y));
   float falloff=(1.0-smoothstep(radius*.15,radius*2.75,d))*exp(-d/(radius*1.18))*vertical;
   float energy=active*(.18+.82*sat(abs(c.w)*(1.0/9000.0)));
   float phase=d*(.035+.004*speed)-time*(1.15+.22*speed)+float(i)*.417;
   float ring=sin(phase)*falloff;
   float ripple=sin(phase*1.78+1.15)*falloff*.42;
   float dRing=(cos(phase)*(.035+.004*speed)+
     cos(phase*1.78+1.15)*(.026+.004*speed))*falloff;
   vec4 motion4=uContactMotion[i];
   vec2 motion=mix(motion4.xz,motion4.xy,useXY);
   float motionLen=length(motion);
   float motionEnergy=smoothstep(.06,9.0,motionLen);
   vec2 moveDir=(motionLen>.001) ? motion/motionLen : -dir;
   vec2 trailDir=-moveDir;
   vec2 sideDir=vec2(-trailDir.y,trailDir.x);
   float trailAlong=dot(delta,trailDir)/max(radius,1.0);
   float trailSide=dot(delta,sideDir)/max(radius,1.0);
   float trail=motionEnergy*smoothstep(.05,.20,trailAlong)*
     (1.0-smoothstep(1.35,2.75,trailAlong))*vertical;
   float armX=trailAlong*.34;
   float armWidth=.095+max(trailAlong,0.0)*.050;
   float left=exp(-pow((trailSide+armX)/armWidth,2.0))*trail;
   float right=exp(-pow((trailSide-armX)/armWidth,2.0))*trail;
   float stem=exp(-pow(trailSide/.14,2.0))*trail*
     (1.0-smoothstep(.28,1.10,trailAlong));
   float yWave=sin(trailAlong*56.0+abs(trailSide)*20.0-time*3.3)*
     (left+right)*TR456_WATER_WAKE_WAVE;
   float stemWave=sin(trailAlong*70.0-time*3.6)*stem*
     TR456_WATER_WAKE_WAVE;
   float tensionX=(d-radius*.34)/max(radius*.080,18.0);
   float meniscus=exp(-tensionX*tensionX)*falloff;
   float dMeniscus=(-2.0*tensionX/max(radius*.080,18.0))*meniscus;
   height+=(ring+ripple+meniscus*.34+yWave*.42+stemWave*.26)*energy;
   slope+=(dir*(dRing+dMeniscus*.34)+
     (sideDir*(right-left)*.062-trailDir*(left+right+stem)*.040)*
       (yWave*1.18+stemWave*.74))*energy;
 }
 return vec3(slope*2.75*strength,height*1.92*strength);
}

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
 vec3 contact=contactWaveField(p.xyz,time)*TR_TOGGLE_CONTACT_RIPPLES;
 vec3 viewP=vec3(dot(uViewMatrix[0].xyz,p.xyz),
                 dot(uViewMatrix[1].xyz,p.xyz),
                 dot(uViewMatrix[2].xyz,p.xyz));
 float distFade=1.0-smoothstep(6800.0,32000.0,length(viewP));
 float amp=42.0*strength*waveStrength*clamp(TR456_WATER_SURFACE_WAVE,0.0,1.45)*distFade;
 float contactAmp=82.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*distFade;
 float vertexDisp=clamp((wave*amp*.16+contact.z*contactAmp)*
   TR_TOGGLE_MESH_DISPLACEMENT,-42.0,42.0);
 p.y+=vertexDisp;

 vPos=p.xyz;
 vContactWave=contact;
  gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),
                               dot(uViewMatrix[1].xyz,p.xyz),
                               dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
