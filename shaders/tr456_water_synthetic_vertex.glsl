#version 150

#ifndef TR456_WATER_WAKE_WIDTH
#define TR456_WATER_WAKE_WIDTH 0.50
#endif
#ifndef TR456_WATER_WAKE_LENGTH
#define TR456_WATER_WAKE_LENGTH 1.0
#endif
#ifndef TR456_WATER_WAKE_STRENGTH
#define TR456_WATER_WAKE_STRENGTH 1.0
#endif
#ifndef TR456_WATER_WAKE_WAVE
#define TR456_WATER_WAKE_WAVE 1.0
#endif
#ifndef TR456_WATER_FLOW_SURFACE_TENSION
#define TR456_WATER_FLOW_SURFACE_TENSION 0.0
#endif
#ifndef TR456_WATER_FLOW_CROSS_WAVE
#define TR456_WATER_FLOW_CROSS_WAVE 0.25
#endif

uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uTrWaterSyntheticInfo;
uniform vec4 uTrWaterSyntheticMode;
uniform vec4 uTrWaterSyntheticProfile;
uniform vec4 uParams;
uniform vec4 uTrWaterDrawInfo;
uniform vec4 uContacts[16];
uniform vec4 uContactMotion[16];

in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

out vec3 vSynPos;
out vec3 vSynWorldPos;
out vec3 vSynNormal;
out vec2 vSynUv;
out vec3 vSynLight;
out vec2 vSynFlowUv;
out vec2 vSynFlowDir;
out vec4 vSynFlowInfo;

vec4 clipFromPos(vec3 p){
 return uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p),
                         dot(uViewMatrix[1].xyz,p),
                         dot(uViewMatrix[2].xyz,p),1.0);
}

float contactRadius(vec4 c){
 float encoded=abs(c.w);
 float encodedMode=step(49152.0,encoded);
 float nativeRadius=clamp(encoded*.025,90.0,340.0);
 float encodedRadius=clamp(floor(encoded*(1.0/512.0)),90.0,720.0);
 return mix(nativeRadius,encodedRadius,encodedMode);
}

float contactVertexLift(vec3 w, float t){
 float lift=0.0;
 float standingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 float liftStrength=mix(8.5,18.5,standingProfile);
 for(int i=0;i<16;i++){
  vec4 c=uContacts[i];
  float active=step(.001,dot(abs(c),vec4(1.0)));
  float radius=contactRadius(c);
  vec2 d=w.xz-c.xz;
  float dist=length(d)+.001;
  float vertical=1.0-smoothstep(96.0,520.0,abs(w.y-c.y));
  float age=mod(abs(c.w),512.0);
  float falloff=active*vertical*(1.0-smoothstep(radius*.10,radius*2.25,dist))*exp(-dist/(radius*1.08));
  float phase=dist*.052-t*4.10+age*.085+float(i)*.37;
  lift+=sin(phase)*falloff*liftStrength;
 }
 return clamp(lift,-19.0,28.0);
}

vec3 decodeSourceNormal(vec4 encodedNormal){
 vec3 decoded=encodedNormal.xyz-vec3(127.0);
 return length(decoded)>.001 ? normalize(decoded) : vec3(0.0,1.0,0.0);
}

vec2 gameFlowDir(vec2 flowVector){
 float flowLen=length(flowVector);
 float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 fallback=normalize(vec2(.92,.38));
 return ((flowLen>.000001) ? flowVector/flowLen : fallback)*flowSign;
}

float classifyCascadeFlow(float flowMode, vec3 sourceNormal){
 float horizontalFlow=smoothstep(.38,.72,abs(sourceNormal.y));
 float zeroAmp=1.0-step(.001,abs(uParams.w));
 float fineFlowParam=step(.00045,abs(uParams.z));
 return flowMode*clamp(max(1.0-horizontalFlow,zeroAmp*.85+fineFlowParam*.35),0.0,1.0);
}

void main(){
 vec4 coord=vec4(aCoord);
 vec3 sourceNormal=decodeSourceNormal(aNormal);
 vec2 rawUv=vec2(aLight.w,aColor.w);
 vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);
 vec3 wp0=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 float t=uTrWaterSyntheticInfo.w;
 float flowMode=clamp(uTrWaterSyntheticMode.x,0.0,1.0);
 float duplicatePass=step(0.5,uTrWaterSyntheticMode.w);
 vec2 flowVector=uParams.xy;
 float flowLen=length(flowVector);
 vec2 flowDir=gameFlowDir(flowVector);
 vec2 flowSide=vec2(-flowDir.y,flowDir.x);
 float speed=max(flowLen,.22);
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 float cascadeMask=0.0;
 float surfaceFlowMask=1.0-cascadeMask;
 float cascadeStillMask=1.0-smoothstep(.12,.58,cascadeMask);
 float flowTime=t*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0)*(.92+speed*.26);
 float passMotion=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,1.0),duplicatePass);
 flowTime*=passMotion;
 vec2 flowPos=vec2(dot(wp0.xz,flowDir),dot(wp0.xz,flowSide));
 vec2 flowWavePos=flowPos*.0084;
 float streamTime=flowTime*.32;
 float lane=sin(flowWavePos.y*1.75-streamTime*.21)*.38+
   sin(flowWavePos.y*3.20+streamTime*.14)*.20;
 float phaseMain=flowWavePos.x*3.75+lane*.80-streamTime*.34;
 float phaseLong=flowWavePos.x*2.15+sin(flowWavePos.y*1.45-streamTime*.15)*.22-streamTime*.18;
 float phaseFast=flowWavePos.x*7.60+sin(flowWavePos.y*3.35+streamTime*.19)*.16-streamTime*.48;
 float phaseNeedle=flowWavePos.x*15.4+sin(flowWavePos.y*7.20+streamTime*.36)*.18-streamTime*1.15;
 float phaseBreath=flowWavePos.x*2.55+sin(flowWavePos.y*1.80+streamTime*.23)*.30-streamTime*.26;
 float phaseChatter=flowWavePos.x*26.0+sin(flowWavePos.y*9.4-streamTime*.28)*.14-streamTime*1.75;
 float phaseCross=flowWavePos.y*4.40+sin(flowWavePos.x*1.70-streamTime*.16)*.18-streamTime*.30;
 float phaseCrossFast=flowWavePos.y*11.8-flowWavePos.x*.70-streamTime*.74;
 float flowTrain=sin(phaseMain)*.34+sin(phaseMain*2.0+.45)*.10+
   sin(phaseLong)*.13+sin(phaseFast)*.17+sin(phaseNeedle)*.12+
   sin(phaseBreath)*.22+sin(phaseChatter)*.070+
   (sin(phaseCross)*.16+sin(phaseCrossFast)*.075)*crossStrength;
 float ridge=pow(clamp(sin(flowWavePos.x*12.8-streamTime*.96+lane)*.5+.5,0.0,1.0),5.0);
 float crossRidge=pow(clamp(sin(flowWavePos.y*9.8+flowWavePos.x*.55-streamTime*.68)*.5+.5,0.0,1.0),5.0);
 float streak=pow(clamp(sin(flowWavePos.x*23.0+lane*1.5-streamTime*1.55)*.5+.5,0.0,1.0),9.0);
 float breathEnvelope=.76+.24*(sin(phaseBreath*.72+streamTime*.18)*.5+.5);
 float flowLift=(flowTrain*(1.02+.62*TR456_WATER_FLOW_WAVE_STRENGTH)+
   (ridge-.38)*.28+(crossRidge-.36)*.20*crossStrength)*
   12.5*breathEnvelope*clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.15)*
   clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.70);
 flowLift+=(streak-.22)*2.9*clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.70);
 flowLift+=(streak-.18)*2.4*clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0)*
   clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.15);
 vec2 diagA=flowDir;
 vec2 diagB=flowSide;
 vec2 diagC=normalize(flowDir*.38-flowSide*.92);
 float low=sin(dot(wp0.xz,diagA)*.0100+t*.92);
 float crossRoll=sin(dot(wp0.xz,diagB)*.0128-t*.80);
 float fine=sin(dot(wp0.xz,diagC)*.027+t*1.55);
 float crossing=sin(dot(wp0.xz,normalize(diagA+diagB))*.017+low*.20-crossRoll*.14+t*.32);
 float breath=sin(dot(wp0.xz,normalize(flowDir*.44+flowSide*.90))*.0088+t*.48+crossRoll*.10);
 float calmLift=low*3.2+crossRoll*2.4+crossing*1.15+fine*1.15+breath*1.65;
 p.y+=(mix(calmLift,flowLift*surfaceFlowMask,flowMode)+
   contactVertexLift(wp0,t))*cascadeStillMask;
 vSynPos=p.xyz;
 vSynWorldPos=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vSynNormal=normalize(mix(vec3(0.0,1.0,0.0),sourceNormal,cascadeMask));
 vSynUv=mix(rawUv,rawUv+flowVector*t,flowMode);
 vSynLight=clamp(pow(aLight.xyz,vec3(2.2))+pow(aColor.xyz,vec3(2.2))*.45,0.0,1.8);
 vSynFlowUv=flowPos*.00072;
 vSynFlowDir=flowDir;
 vSynFlowInfo=vec4(flowMode,cascadeMask,duplicatePass,speed);
 gl_Position=clipFromPos(p.xyz);
}
