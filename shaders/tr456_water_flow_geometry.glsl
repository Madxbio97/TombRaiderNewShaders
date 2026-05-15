#version 150
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
#define TR456_WATER_MESH_SUBDIVISION 5
#endif
#ifndef TR456_WATER_CONTACT_MESH_STRENGTH
#define TR456_WATER_CONTACT_MESH_STRENGTH 0.78
#endif
#ifndef TR456_WATER_POLYGONAL_STRENGTH
#define TR456_WATER_POLYGONAL_STRENGTH 0.0
#endif
#ifndef TR456_WATER_POLYGONAL_SCALE
#define TR456_WATER_POLYGONAL_SCALE 620.0
#endif
#ifndef TR456_WATER_POLYGONAL_NORMAL
#define TR456_WATER_POLYGONAL_NORMAL 0.0
#endif
#ifndef TR456_WATER_POLYGONAL_FLOW
#define TR456_WATER_POLYGONAL_FLOW 0.0
#endif
#ifndef TR456_WATER_PHYSICS_MESH
#define TR456_WATER_PHYSICS_MESH 0.0
#endif
#ifndef TR456_WATER_PHYSICS_STRENGTH
#define TR456_WATER_PHYSICS_STRENGTH 0.88
#endif
#ifndef TR456_WATER_PHYSICS_SCALE
#define TR456_WATER_PHYSICS_SCALE 560.0
#endif
#ifndef TR456_WATER_PHYSICS_CONTACT
#define TR456_WATER_PHYSICS_CONTACT 1.25
#endif
#ifndef TR456_WATER_PHYSICS_RAIN
#define TR456_WATER_PHYSICS_RAIN 0.72
#endif
#ifndef TR456_WATER_PHYSICS_CHOP
#define TR456_WATER_PHYSICS_CHOP 0.42
#endif
#ifndef TR456_WATER_PHYSICS_NORMAL
#define TR456_WATER_PHYSICS_NORMAL 1.05
#endif
#ifndef TR456_WATER_SURFACE_WAVE
#define TR456_WATER_SURFACE_WAVE 1.36
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_STRENGTH
#define TR456_WATER_SURFACE_VERTEX_STRENGTH 0.82
#endif
#ifndef TR456_WATER_SURFACE_VERTEX_WAVE
#define TR456_WATER_SURFACE_VERTEX_WAVE 1.58
#endif
#ifndef TR456_WATER_SURFACE_RELIEF
#define TR456_WATER_SURFACE_RELIEF 1.82
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
#ifndef TR456_WATER_GAME_RIPPLE_STRENGTH
#define TR456_WATER_GAME_RIPPLE_STRENGTH 1.58
#endif
#ifndef TR456_WATER_RAIN_RIPPLE
#define TR456_WATER_RAIN_RIPPLE 1.06
#endif

layout(triangles) in;
layout(triangle_strip, max_vertices = 35) out;

uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uContacts[16];
uniform vec4 uTrWaterCaptureInfo;
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
uniform vec4 uTrWaterDrawInfo;
uniform vec4 uTrWaterMaterialProfile;

in vec2 gTexCoord[];
in vec3 gColor[];
in vec3 gLight[];
in float gLayer[];
in float gFog[];
in vec3 gNormal[];
in vec3 gPos[];
in vec3 gWorldPos[];
in vec3 gContactWave[];

out vec2 vTexCoord;
out vec3 vColor;
out vec3 vLight;
out float vLayer;
out float vFog;
out vec3 vNormal;
out vec3 vPos;
out vec3 vContactWave;

float flowOriginalBypass(){
 return step(.5,uTrWaterMaterialProfile.y);
}

float sat(float x){ return clamp(x,0.0,1.0); }

vec2 safeNormalize2(vec2 v, vec2 fallback){
 float d=dot(v,v);
 return (d>.00000001) ? v*inversesqrt(d) : fallback;
}

vec3 safeNormalize3(vec3 v, vec3 fallback){
 float d=dot(v,v);
 return (d>.00000001) ? v*inversesqrt(d) : fallback;
}

float interiorFade(vec3 b){
 float e=min(min(b.x,b.y),b.z);
 return smoothstep(.030,.165,e);
}

#define TR_TOGGLE_MESH_DISPLACEMENT uTrWaterToggle2.z
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float hash12(vec2 p){
 vec3 p3=fract(vec3(p.xyx)*.1031);
 p3+=dot(p3,p3.yzx+33.33);
 return fract((p3.x+p3.y)*p3.z);
}

vec3 polygonFacetWave(vec2 p, vec2 primaryDir, float time, float speedBias){
 float strength=clamp(TR456_WATER_POLYGONAL_STRENGTH,0.0,1.5);
 if(strength<=.001) return vec3(0.0);
 float scale=max(TR456_WATER_POLYGONAL_SCALE,120.0);
 vec2 flowDir=safeNormalize2(primaryDir+vec2(.0001,.0003),vec2(.92,.38));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float drift=clamp(TR456_WATER_POLYGONAL_FLOW,0.0,2.5)*speedBias;
 vec2 q=vec2(dot(p,flowDir),dot(p,side))/scale;

 vec2 id=floor(q);
 vec2 f=fract(q)-.5;
 float r0=hash12(id+vec2(1.7,9.2));
 float r1=hash12(id+vec2(8.4,2.1));
 float r2=hash12(id+vec2(3.6,6.8));
 vec2 axis=safeNormalize2(vec2(r0-.5,r1-.5)+vec2(.001),flowDir);
 float stream=dot(q,vec2(.91,.23))-time*(.20+.58*drift);
 float phase=stream+time*(.18+r2*.30)*max(drift,.18)+r0*6.28318;
 float plane=dot(f,axis)*2.0;
 float pulse=sin(phase)*.16+sin(phase*1.7+r1*3.2)*.055;
 float h=(plane*.24+pulse+(r1-.5)*.16)*strength;
 vec2 localSlope=axis*(.26+sin(phase+r2)*.06)*strength;
 localSlope+=vec2(sin(phase*.73+r0),cos(phase*.91+r1))*.075*strength;
 vec2 slope=flowDir*localSlope.x+side*localSlope.y;
 return vec3(slope,h);
}

float isScreenContact(vec4 c);
float contactAge(vec4 c);
float contactRadius(vec4 c);
vec2 captureInvViewport();
vec2 screenFromClip(vec4 clip);

vec3 physicsTravelWave(vec2 p, vec2 primaryDir, float time){
 float scale=max(TR456_WATER_PHYSICS_SCALE,160.0);
 vec2 flowDir=safeNormalize2(primaryDir+vec2(.0001,.0003),vec2(.92,.38));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 vec2 fp=vec2(dot(p,flowDir),dot(p,side))/scale;
 float chop=clamp(TR456_WATER_PHYSICS_CHOP,0.0,2.0);
 float pa=fp.x*6.28318-time*.58;
 float pb=fp.y*6.28318+time*.43;
 float pc=(fp.x*.72+fp.y*.48)*6.28318-time*.31;
 float h=(sin(pa)*.26+sin(pb)*.18+sin(pc)*.14)*chop;
 vec2 slope=(flowDir*cos(pa)*.080+side*cos(pb)*.070+
   safeNormalize2(flowDir*.72+side*.48,flowDir)*cos(pc)*.050)*chop;
 return vec3(slope,h);
}

vec3 physicsContactWave(vec3 worldPos, vec4 clip){
 float strength=clamp(TR456_WATER_PHYSICS_CONTACT,0.0,2.5);
 if(strength<=.001) return vec3(0.0);
 vec2 screen=screenFromClip(clip);
 vec2 inv=max(captureInvViewport(),vec2(1.0/8192.0));
 vec2 slope=vec2(0.0);
 float height=0.0;

 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float contactOn=step(.001,dot(abs(c),vec4(1.0)));
   float screenContact=isScreenContact(c);
   float radius=contactRadius(c);
   vec2 delta;
   float vertical;

   if(screenContact>.5) {
     delta=(screen-c.xy)/inv;
     vertical=1.0;
   } else {
     vec2 deltaXZ=worldPos.xz-c.xz;
     vec2 deltaXY=worldPos.xy-c.xy;
     float dXZ=length(deltaXZ);
     float dXY=length(deltaXY);
     float autoXY=step(dXY,dXZ);
     float mode=float(TR456_WATER_CONTACT_COORD_MODE);
     float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
     delta=mix(deltaXZ,deltaXY,useXY);
     vertical=1.0-smoothstep(radius*.18,radius*1.35,abs(worldPos.y-c.y));
   }

   float d=length(delta)+.001;
   vec2 dir=delta/d;
   float age=clamp(contactAge(c),0.0,260.0);
   float grow=smoothstep(0.0,170.0,age);
   float fade=contactOn*smoothstep(1.0,10.0,age)*(1.0-smoothstep(170.0,260.0,age))*vertical;
   float front=mix(max(62.0,radius*(.50+.20*grow)+age*(1.08+.38*TR456_WATER_CONTACT_WAVE_SPEED)),
     max(14.0,radius*(.34+.14*grow)+age*(.21+.10*TR456_WATER_CONTACT_WAVE_SPEED)),screenContact);
   float width=mix(max(58.0,radius*(.105+.045*grow)),
     max(16.0,radius*(.060+.020*grow)),screenContact);
   float x=(d-front)/width;
   float env=exp(-x*x);
   float phase=(d-front)*(.036+.004*TR456_WATER_CONTACT_WAVE_SPEED);
   float main=sin(phase)*env;
   float trough=sin(phase*1.72+1.6)*env*.38;
   float wake=sin((d-front*.42)*.024-age*.026)*exp(-d/(radius*1.95))*
     smoothstep(.20,.80,grow);
   float shell=(1.0-smoothstep(radius*3.4,radius*4.8,d));
   float h=(main*.54+trough*.22+wake*.18)*shell*fade;
   float dEnv=(-2.0*x/width)*env;
   float dMain=cos(phase)*(.036+.004*TR456_WATER_CONTACT_WAVE_SPEED)*env+
     sin(phase)*dEnv;
   float dTrough=cos(phase*1.72+1.6)*(.062+.007*TR456_WATER_CONTACT_WAVE_SPEED)*env*.38+
     sin(phase*1.72+1.6)*dEnv*.38;
   float dWake=(cos((d-front*.42)*.024-age*.026)*.024-wake/(radius*1.95))*
     smoothstep(.20,.80,grow);
   slope+=dir*(dMain*.54+dTrough*.22+dWake*.18)*shell*fade;
   height+=h;
 }

 return vec3(clamp(slope*.62*strength,vec2(-.20),vec2(.20)),
   clamp(height*.82*strength,-1.0,1.0));
}

vec3 physicsRainWave(vec2 p, float time){
 float strength=clamp(TR456_WATER_PHYSICS_RAIN,0.0,2.5)*clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.5);
 if(strength<=.001) return vec3(0.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 float cell=430.0;
 vec2 base=floor(p/cell);
 for(int ix=-1;ix<=1;ix++){
   for(int iy=-1;iy<=1;iy++){
     vec2 id=base+vec2(float(ix),float(iy));
     float rnd=hash12(id+vec2(11.2,4.6));
     vec2 center=(id+vec2(hash12(id+vec2(1.3,5.7)),
       hash12(id+vec2(8.2,2.4))))*cell;
     vec2 delta=p-center;
     float d=length(delta)+.001;
     vec2 dir=delta/d;
     float age=fract(time*.32+rnd);
     float density=smoothstep(.26,.94,rnd);
     float fade=smoothstep(.035,.12,age)*(1.0-smoothstep(.66,1.0,age))*density;
     float front=mix(18.0,380.0,age);
     float width=mix(18.0,54.0,age);
     float x=(d-front)/width;
     float env=exp(-x*x);
     float ring=sin((d-front)*.058)*env*(1.0-smoothstep(520.0,780.0,d));
     float dRing=(cos((d-front)*.058)*.058+sin((d-front)*.058)*(-2.0*x/width))*env;
     height+=ring*fade*.42;
     slope+=dir*dRing*fade*.34;
   }
 }
 return vec3(clamp(slope*strength,vec2(-.20),vec2(.20)),
   clamp(height*strength,-1.0,1.0));
}

vec3 physicsMeshWave(vec3 worldPos, vec4 clip, vec2 primaryDir, float time){
 float enabled=step(.001,TR456_WATER_PHYSICS_MESH);
 float strength=clamp(TR456_WATER_PHYSICS_STRENGTH,0.0,2.0)*enabled;
 if(strength<=.001) return vec3(0.0);
 vec3 travel=physicsTravelWave(worldPos.xz,primaryDir,time);
 vec3 contact=physicsContactWave(worldPos,clip)*TR_TOGGLE_CONTACT_RIPPLES;
 vec3 rain=physicsRainWave(worldPos.xz,time);
 vec3 field=travel+contact+rain;
 field.xy=clamp(field.xy,vec2(-.26),vec2(.26));
 field.z=clamp(field.z,-1.2,1.2);
 return field*strength;
}

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
 if(isScreenContact(c)>.5)
   return max(abs(c.z),14.0)*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 float fallback=720.0;
 return mix(fallback,radiusPacked,step(96.0,radiusPacked))*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec4 clipFromPos(vec3 p){
 return uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p),
                         dot(uViewMatrix[1].xyz,p),
                         dot(uViewMatrix[2].xyz,p),1.0);
}

vec2 screenFromClip(vec4 clip){
 float w=(abs(clip.w)<.00001) ? ((clip.w<0.0) ? -.00001 : .00001) : clip.w;
 return clip.xy/w*.5+.5;
}

vec3 contactMeshWave(vec3 worldPos, vec4 clip){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 float life=160.0;
 vec2 screen=screenFromClip(clip);
 vec2 inv=max(captureInvViewport(),vec2(1.0/8192.0));
 vec2 slope=vec2(0.0);
 float height=0.0;

 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float contactOn=step(.001,dot(abs(c),vec4(1.0)));
   float screenContact=isScreenContact(c);
   float radius=contactRadius(c);
   vec2 delta;
   float vertical;

   if(screenContact>.5) {
     delta=(screen-c.xy)/inv;
     vertical=1.0;
   } else {
      vec2 deltaXZ=worldPos.xz-c.xz;
      vec2 deltaXY=worldPos.xy-c.xy;
     float dXZ=length(deltaXZ);
     float dXY=length(deltaXY);
     float autoXY=step(dXY,dXZ);
     float mode=float(TR456_WATER_CONTACT_COORD_MODE);
     float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
     delta=mix(deltaXZ,deltaXY,useXY);
      vertical=1.0-smoothstep(radius*.20,radius*1.16,abs(worldPos.y-c.y));
   }

   float d=length(delta)+.001;
   vec2 dir=delta/d;
    float age=clamp(contactAge(c),0.0,life);
     float fade=contactOn*smoothstep(2.0,12.0,age)*(1.0-smoothstep(life*.62,life,age));
    float grow=smoothstep(0.0,life*.74,age);
     float front=mix(max(70.0,radius*(.54+.16*grow)+age*(1.20+speed*.92)),
       max(15.0,radius*(.46+.12*grow)+age*(.24+speed*.16)),screenContact);
     float width=mix(max(72.0,radius*(.130+.044*grow)),
       max(18.0,radius*(.078+.026*grow)),screenContact);
   float crestX=(d-front)/width;
    float troughX=(d-(front-width*.94))/(width*1.54);
    float crest=exp(-crestX*crestX);
    float trough=exp(-troughX*troughX);
    float trailX=(d-(front-width*2.20))/(width*1.90);
    float trail=exp(-trailX*trailX)*smoothstep(.16,.72,grow);
    float shell=(1.0-smoothstep(radius*3.05,radius*4.35,d))*vertical;
    float phase=(d-front)*(.026+.003*speed);
     float ripple=sin(phase)*crest*.060+sin(phase*1.58+1.25)*crest*.025;
     float rim=(crest*.54-trough*.18+trail*.20+ripple)*shell;
    float dCrest=(-2.0*crestX/width)*crest;
    float dTrough=(-2.0*troughX/(width*1.54))*trough;
    float dTrail=(-2.0*trailX/(width*1.90))*trail;
    float dFine=cos(phase)*(.026+.003*speed)*crest*.10+
      cos(phase*1.58+1.25)*(.041+.005*speed)*crest*.04;
     float dWave=(dCrest*.54-dTrough*.18+dTrail*.20+dFine)*shell;
     float source=exp(-d/(radius*.48))*(1.0-smoothstep(1.0,34.0,age))*vertical;
     float dSource=-source/(radius*.48);
    float energy=fade;
     height+=(rim+source*.12)*energy*mix(.62,.70,screenContact);
     slope+=dir*(dWave*mix(.70,.92,screenContact)+dSource*.10)*energy;
  }

  slope=clamp(slope,vec2(-.075),vec2(.075));
  return vec3(slope*.82*strength,clamp(height,-.62,.62)*strength);
}

vec3 breathMeshWave(vec2 p, float time, vec2 primaryDir){
 float strength=clamp(TR456_WATER_SURFACE_VERTEX_STRENGTH,0.0,1.5)*
   clamp(TR456_WATER_SURFACE_VERTEX_WAVE,0.0,2.0)*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,2.5)*
   clamp(TR456_WATER_SURFACE_RELIEF,0.0,2.5);
 vec2 q=p*.0010;
 vec2 a=safeNormalize2(primaryDir,safeNormalize2(vec2(.92,.38),vec2(1.0,0.0)));
 vec2 b=vec2(-a.y,a.x);
 vec2 c=safeNormalize2(a*.58-b*.82,a);
 float phaseA=dot(q,a)*6.28318+sin(dot(q,b)*3.9+time*.13)*.28+time*.42;
 float phaseB=dot(q,b)*5.20+sin(dot(q,a)*3.4-time*.10)*.22-time*.31;
 float phaseC=dot(q,c)*8.35+time*.54+sin(dot(q,safeNormalize2(a*.70-b*.42,a))*2.6)*.16;
 float h=sin(phaseA)*.40+sin(phaseB)*.30+sin(phaseC)*.20+
   sin((phaseA+phaseB)*.52+time*.18)*.14;
 vec2 slope=a*cos(phaseA)*.34+b*cos(phaseB)*.26+c*cos(phaseC)*.22+
   normalize(a+b+vec2(.001))*cos((phaseA+phaseB)*.52+time*.18)*.10;
 return vec3(slope*.12*strength,h*.58*strength);
}

vec3 flowMeshWave(vec2 p, float time){
 vec2 flowVector=uParams.xy;
 float flowLen=length(flowVector);
 float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 flowDir=((flowLen>.0001) ? flowVector/flowLen : normalize(vec2(.85,.38)))*flowSign;
 vec2 side=vec2(-flowDir.y,flowDir.x);
 vec2 fp=vec2(dot(p,flowDir),dot(p,side));
 float speed=max(flowLen,0.05);
 float t=time*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0)*(1.36+speed*.28);
 float motion=t;
 float waveStrength=clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,2.0);
 float strength=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.15)*
   clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.75)*
   clamp(TR456_WATER_SURFACE_WAVE,0.0,2.5);

 float drift=sin(fp.y*.00105-motion*.27)*.38+sin(fp.y*.00200+motion*.18)*.20;
 float phaseA=fp.x*.00215+drift-motion*1.34;
 float phaseB=fp.x*.00110+sin(fp.y*.00072-motion*.15)*.32-motion*.86;
 float phaseC=fp.x*.00375+sin(fp.y*.00155+motion*.22)*.22-motion*1.96;
 float phaseD=fp.x*.00165+fp.y*.00042-motion*1.08;
 float breathA=sin(fp.x*.00046+sin(fp.y*.00040-motion*.10)*.36-motion*.22);
 float breathB=sin(fp.x*.00028-fp.y*.00020-motion*.16);
 float originalDeform=clamp(TR456_WATER_FLOW_ORIGINAL_DEFORMATION,0.0,1.0);
 float h=(sin(phaseA)*.52+sin(phaseA*2.0+.45)*.12+
   sin(phaseB)*.27+sin(phaseC)*.19+sin(phaseD)*.08)*
   waveStrength;
 h+=originalDeform*(breathA*.40+breathB*.30);
 vec2 localSlope=vec2(
   (cos(phaseA)*.52+cos(phaseA*2.0+.45)*.24+
   cos(phaseB)*.27+cos(phaseC)*.32+cos(phaseD)*.08)*waveStrength+
   originalDeform*(cos(breathA)*.20+cos(breathB)*.14),
   (cos(fp.y*.00105-motion*.27)*.13+cos(phaseB*.73)*.10)*waveStrength);
 vec2 slope=flowDir*localSlope.x+side*localSlope.y;
 float crest=smoothstep(.36,.92,abs(h))*waveStrength;
 h=mix(h,h+sign(h)*crest*.16,.45*waveStrength);
 return vec3(slope*.24*strength,h*1.12*strength);
}

vec3 rainMeshWave(vec2 p, float time){
 float strength=clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.5)*
   clamp(TR456_WATER_SURFACE_RELIEF,0.0,2.5);
 vec2 slope=vec2(0.0);
 float height=0.0;
 float cell=680.0;
 vec2 base=floor(p/cell);
 for(int ix=-1;ix<=1;ix++){
   for(int iy=-1;iy<=1;iy++){
     vec2 id=base+vec2(float(ix),float(iy));
     float rnd=hash12(id+vec2(7.1,3.7));
     vec2 center=(id+vec2(hash12(id+vec2(1.3,5.7)),
       hash12(id+vec2(8.2,2.4))))*cell;
     vec2 delta=p-center;
     float d=length(delta)+.001;
     vec2 dir=delta/d;
     float age=fract(time*.18+rnd);
     float density=smoothstep(.18,.95,rnd);
     float fade=smoothstep(.035,.14,age)*(1.0-smoothstep(.74,1.0,age))*density;
     float front=mix(22.0,560.0,age);
     float width=mix(24.0,72.0,age);
     float crestX=(d-front)/width;
     float troughX=(d-(front-width*.82))/(width*1.55);
     float crest=exp(-crestX*crestX);
     float trough=exp(-troughX*troughX);
     float shell=1.0-smoothstep(620.0,920.0,d);
     float ring=(crest*.58-trough*.22)*shell;
     float dCrest=(-2.0*crestX/width)*crest;
     float dTrough=(-2.0*troughX/(width*1.55))*trough;
     float dRing=(dCrest*.58-dTrough*.22)*shell;
     height+=ring*fade;
     slope+=dir*dRing*fade;
   }
 }
 return vec3(clamp(slope*strength,vec2(-.20),vec2(.20)),
   clamp(height*strength,-1.0,1.0));
}

void emitFlowVertex(vec3 b){
  vec3 pos=gPos[0]*b.x+gPos[1]*b.y+gPos[2]*b.z;
  vec3 worldPos=gWorldPos[0]*b.x+gWorldPos[1]*b.y+gWorldPos[2]*b.z;
  vec3 normal=safeNormalize3(gNormal[0]*b.x+gNormal[1]*b.y+gNormal[2]*b.z,
    safeNormalize3(gNormal[0]+gNormal[1]+gNormal[2],vec3(0.0,1.0,0.0)));
  if(flowOriginalBypass()>.5) {
   vTexCoord=gTexCoord[0]*b.x+gTexCoord[1]*b.y+gTexCoord[2]*b.z;
   vColor=gColor[0]*b.x+gColor[1]*b.y+gColor[2]*b.z;
   vLight=gLight[0]*b.x+gLight[1]*b.y+gLight[2]*b.z;
   vLayer=gLayer[0]*b.x+gLayer[1]*b.y+gLayer[2]*b.z;
   vFog=clamp(exp(-((length(pos)/15000.0)*(length(pos)/15000.0))),0.0,1.0);
   vNormal=normal;
   vPos=pos;
   vContactWave=gContactWave[0]*b.x+gContactWave[1]*b.y+gContactWave[2]*b.z;
   gl_Position=clipFromPos(pos);
   EmitVertex();
   return;
  }
  float edgeMask=interiorFade(b);
  float time=uModelMatrix[3].x;
  float duplicatePass=step(0.5,uTrWaterDrawInfo.w);
  float passMotion=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,1.0),duplicatePass);
  time*=passMotion;
  vec4 baseClip=clipFromPos(pos);
  vec2 flowVector=uParams.xy;
  float flowLen=length(flowVector);
  float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
  vec2 flowDir=((flowLen>.0001) ? flowVector/flowLen : safeNormalize2(vec2(.85,.38),vec2(.92,.38)))*flowSign;
  vec3 contact=contactMeshWave(worldPos,baseClip)*TR_TOGGLE_CONTACT_RIPPLES*passMotion;
  vec3 flow=flowMeshWave(worldPos.xz,time)*passMotion;
  vec3 breath=breathMeshWave(worldPos.xz,time*.82,flowDir)*passMotion;
  vec3 rain=rainMeshWave(worldPos.xz,time)*passMotion;
  vec3 facet=polygonFacetWave(worldPos.xz,flowDir,time,1.0)*passMotion;
  vec3 physics=physicsMeshWave(worldPos,baseClip,flowDir,time)*passMotion;
 float flowMeshActive=max(TR_TOGGLE_MESH_DISPLACEMENT,
   step(0.001,TR456_WATER_FLOW_VERTEX_STRENGTH));
 vec3 viewDir=normalize(-pos+normal*.0001);
 float distFade=1.0-smoothstep(4800.0,15800.0,length(pos));
 float angleFade=mix(.60,1.0,smoothstep(.02,.20,abs(dot(normal,viewDir))));
 float horizontal=smoothstep(.18,.72,abs(normal.y));
 float upSign=mix(-1.0,1.0,step(0.0,normal.y));
 vec3 heightDir=safeNormalize3(mix(normal,vec3(0.0,upSign,0.0),horizontal),vec3(0.0,upSign,0.0));
  float contactAmp=28.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.2)*
     clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*distFade*angleFade;
  float flowAmp=96.0*clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*
    distFade*angleFade*horizontal;
  float breathAmp=16.0*clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*
    distFade*angleFade*horizontal;
  float rainAmp=5.0*clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*
    distFade*angleFade*horizontal;
  float facetAmp=30.0*clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*
    distFade*angleFade*horizontal;
  float physicsAmp=54.0*clamp(TR456_WATER_CONTACT_MESH_STRENGTH,0.0,2.5)*
    distFade*angleFade*horizontal;
  float disp=(contact.z*contactAmp*TR_TOGGLE_MESH_DISPLACEMENT+
    (flow.z*flowAmp+breath.z*breathAmp)*flowMeshActive+
    (rain.z*rainAmp+facet.z*facetAmp+physics.z*physicsAmp)*
      TR_TOGGLE_MESH_DISPLACEMENT)*edgeMask;
  pos+=heightDir*disp;

 vTexCoord=gTexCoord[0]*b.x+gTexCoord[1]*b.y+gTexCoord[2]*b.z;
 vColor=gColor[0]*b.x+gColor[1]*b.y+gColor[2]*b.z;
 vLight=gLight[0]*b.x+gLight[1]*b.y+gLight[2]*b.z;
 vLayer=gLayer[0]*b.x+gLayer[1]*b.y+gLayer[2]*b.z;
 vFog=clamp(exp(-((length(pos)/15000.0)*(length(pos)/15000.0))),0.0,1.0);
  vec2 meshSlope=contact.xy*(.030*clamp(TR456_WATER_CONTACT_NORMAL_STRENGTH,0.0,2.0))+
    flow.xy*.120+breath.xy*.016+rain.xy*.004+
    facet.xy*.180*clamp(TR456_WATER_POLYGONAL_NORMAL,0.0,2.0)+
    physics.xy*.125*clamp(TR456_WATER_PHYSICS_NORMAL,0.0,2.0);
 meshSlope*=edgeMask*flowMeshActive;
  vec3 heightNormal=safeNormalize3(vec3(-meshSlope.x,1.0,-meshSlope.y),vec3(0.0,1.0,0.0));
  vNormal=safeNormalize3(mix(normal,heightNormal,distFade*horizontal),normal);
  vPos=pos;
  vContactWave=gContactWave[0]*b.x+gContactWave[1]*b.y+gContactWave[2]*b.z+
    contact*.08*edgeMask+physics*.055*edgeMask;
 gl_Position=clipFromPos(pos);
 EmitVertex();
}

void main(){
 int n=TR456_WATER_MESH_SUBDIVISION;
 if(n<1) n=1;
 if(n>5) n=5;
 float fn=float(n);

 for(int row=0;row<5;row++){
   if(row>=n) continue;
   int width=n-row;
   for(int col=0;col<5;col++){
     if(col>=width) continue;
     vec3 lower=vec3(float(n-row-col-1),float(col),float(row+1))/fn;
     vec3 upper=vec3(float(n-row-col),float(col),float(row))/fn;
     emitFlowVertex(lower);
     emitFlowVertex(upper);
   }
   emitFlowVertex(vec3(0.0,float(width),float(row))/fn);
   EndPrimitive();
 }
}
