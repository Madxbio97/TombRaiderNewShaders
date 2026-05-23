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
#ifndef TR456_WATER_FLOW_ORIGINAL_SYNC
#define TR456_WATER_FLOW_ORIGINAL_SYNC 1.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_STRENGTH
#define TR456_WATER_FLOW_CONTACT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_RIPPLES
#define TR456_WATER_FLOW_CONTACT_RIPPLES 0.0
#endif
#ifndef TR456_WATER_CONTACT_MAX_ACTIVE
#define TR456_WATER_CONTACT_MAX_ACTIVE 6
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

float trshaderFastPow5(float trshaderX){ float trshaderX2=trshaderX*trshaderX; return trshaderX2*trshaderX2*trshaderX; }
float trshaderFastPow9(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; return trshaderX4*trshaderX4*trshaderX; }

vec4 trshaderClipFromPos(vec3 trshaderP){
 return uProjMatrix*vec4(dot(uViewMatrix[0].xyz,trshaderP),
                         dot(uViewMatrix[1].xyz,trshaderP),
                         dot(uViewMatrix[2].xyz,trshaderP),1.0);
}

float trshaderContactRadius(vec4 trshaderC){
 float trshaderEncoded=abs(trshaderC.w);
 float trshaderEncodedMode=step(49152.0,trshaderEncoded);
 float trshaderNativeRadius=clamp(trshaderEncoded*.025,90.0,340.0);
 float trshaderEncodedRadius=clamp(floor(trshaderEncoded*(1.0/512.0)),90.0,720.0);
 return mix(trshaderNativeRadius,trshaderEncodedRadius,trshaderEncodedMode);
}

float trshaderContactVertexLift(vec3 trshaderW, float trshaderT){
 float trshaderLift=0.0;
 float trshaderStandingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 float trshaderFlowProfile=1.0-trshaderStandingProfile;
 float trshaderRippleGate=max(trshaderStandingProfile,
   trshaderFlowProfile*step(.5,TR456_WATER_FLOW_CONTACT_RIPPLES));
 if(trshaderRippleGate<=.001) return 0.0;
 float trshaderLiftStrength=mix(8.5,18.5,trshaderStandingProfile)*
   mix(1.0,clamp(TR456_WATER_FLOW_CONTACT_STRENGTH,0.0,2.4),trshaderFlowProfile)*
   trshaderRippleGate;
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_MAX_ACTIVE;trshaderI++){
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderContactOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderContactOn<=.001) continue;
  float trshaderRadius=trshaderContactRadius(trshaderC);
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderDist=length(trshaderD)+.001;
  float trshaderVertical=1.0-smoothstep(96.0,520.0,abs(trshaderW.y-trshaderC.y));
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderFalloff=trshaderContactOn*trshaderVertical*(1.0-smoothstep(trshaderRadius*.10,trshaderRadius*2.25,trshaderDist))*exp(-trshaderDist/(trshaderRadius*1.08));
  float trshaderPhase=trshaderDist*.052-trshaderT*4.10+trshaderAge*.085+float(trshaderI)*.37;
  trshaderLift+=sin(trshaderPhase)*trshaderFalloff*trshaderLiftStrength;
 }
 return clamp(trshaderLift,-19.0,28.0);
}

vec3 trshaderDecodeSourceNormal(vec4 trshaderEncodedNormal){
 vec3 trshaderDecoded=trshaderEncodedNormal.xyz-vec3(127.0);
 return length(trshaderDecoded)>.001 ? normalize(trshaderDecoded) : vec3(0.0,1.0,0.0);
}

vec2 trshaderGameFlowDir(vec2 trshaderFlowVector){
 float trshaderFlowLen=length(trshaderFlowVector);
 float trshaderFlowSign=mix(1.0,-1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 trshaderFallback=normalize(vec2(.92,.38));
 return ((trshaderFlowLen>.000001) ? trshaderFlowVector/trshaderFlowLen : trshaderFallback)*trshaderFlowSign;
}

float trshaderClassifyCascadeFlow(float trshaderFlowMode, vec3 trshaderSourceNormal){
 float trshaderHorizontalFlow=smoothstep(.38,.72,abs(trshaderSourceNormal.y));
 float trshaderZeroAmp=1.0-step(.001,abs(uParams.w));
 float trshaderFineFlowParam=step(.00045,abs(uParams.z));
 return trshaderFlowMode*clamp(max(1.0-trshaderHorizontalFlow,trshaderZeroAmp*.85+trshaderFineFlowParam*.35),0.0,1.0);
}

void main(){
 vec4 trshaderCoord=vec4(aCoord);
 vec3 trshaderSourceNormal=trshaderDecodeSourceNormal(aNormal);
 vec2 trshaderRawUv=vec2(aLight.w,aColor.w);
 vec4 trshaderP=vec4(dot(uModelMatrix[0],vec4(trshaderCoord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(trshaderCoord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(trshaderCoord.xyz,1.0)),1.0);
 vec3 trshaderWp0=trshaderP.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 float trshaderT=uTrWaterSyntheticInfo.w;
 float trshaderFlowMode=clamp(uTrWaterSyntheticMode.x,0.0,1.0);
 float trshaderDuplicatePass=step(0.5,uTrWaterSyntheticMode.w);
 vec2 trshaderFlowVector=uParams.xy;
 float trshaderFlowLen=length(trshaderFlowVector);
 vec2 trshaderFlowDir=trshaderGameFlowDir(trshaderFlowVector);
 vec2 trshaderFlowSide=vec2(-trshaderFlowDir.y,trshaderFlowDir.x);
 float trshaderSpeed=max(trshaderFlowLen,.22);
 float trshaderCascadeMask=0.0;
 float trshaderSurfaceFlowMask=1.0-trshaderCascadeMask;
 float trshaderCascadeStillMask=1.0-smoothstep(.12,.58,trshaderCascadeMask);
 float trshaderOriginalSync=clamp(TR456_WATER_FLOW_ORIGINAL_SYNC,0.0,1.0);
 float trshaderFlowVertexStrength=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,2.15);
 float trshaderDecorTime=trshaderT*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0)*(.92+trshaderSpeed*.26);
 float trshaderOriginalTravel=trshaderT*trshaderSpeed;
 float trshaderFlowTime=mix(trshaderDecorTime,trshaderOriginalTravel,trshaderOriginalSync);
  float trshaderPassMotion=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,1.0),trshaderDuplicatePass);
  trshaderFlowTime*=mix(trshaderPassMotion,1.0,trshaderOriginalSync);
 vec2 trshaderFlowPos=vec2(dot(trshaderWp0.xz,trshaderFlowDir),dot(trshaderWp0.xz,trshaderFlowSide));
 float trshaderFlowLift=0.0;
 if(trshaderFlowMode>.001) {
  float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  vec2 trshaderFlowWavePos=trshaderFlowPos*.0084;
  float trshaderStreamTime=trshaderFlowTime*.32;
  float trshaderLane=sin(trshaderFlowWavePos.y*1.75-trshaderStreamTime*.21)*.38+
    sin(trshaderFlowWavePos.y*3.20+trshaderStreamTime*.14)*.20;
  float trshaderPhaseMain=trshaderFlowWavePos.x*3.75+trshaderLane*.80-trshaderStreamTime*.34;
  float trshaderPhaseLong=trshaderFlowWavePos.x*2.15+sin(trshaderFlowWavePos.y*1.45-trshaderStreamTime*.15)*.22-trshaderStreamTime*.18;
  float trshaderPhaseFast=trshaderFlowWavePos.x*7.60+sin(trshaderFlowWavePos.y*3.35+trshaderStreamTime*.19)*.16-trshaderStreamTime*.48;
  float trshaderPhaseNeedle=trshaderFlowWavePos.x*15.4+sin(trshaderFlowWavePos.y*7.20+trshaderStreamTime*.36)*.18-trshaderStreamTime*1.15;
  float trshaderPhaseBreath=trshaderFlowWavePos.x*2.55+sin(trshaderFlowWavePos.y*1.80+trshaderStreamTime*.23)*.30-trshaderStreamTime*.26;
  float trshaderPhaseChatter=trshaderFlowWavePos.x*26.0+sin(trshaderFlowWavePos.y*9.4-trshaderStreamTime*.28)*.14-trshaderStreamTime*1.75;
  float trshaderPhaseCross=trshaderFlowWavePos.y*4.40+sin(trshaderFlowWavePos.x*1.70-trshaderStreamTime*.16)*.18-trshaderStreamTime*.30;
  float trshaderPhaseCrossFast=trshaderFlowWavePos.y*11.8-trshaderFlowWavePos.x*.70-trshaderStreamTime*.74;
  float trshaderFlowTrain=sin(trshaderPhaseMain)*.34+sin(trshaderPhaseMain*2.0+.45)*.10+
    sin(trshaderPhaseLong)*.13+sin(trshaderPhaseFast)*.17+sin(trshaderPhaseNeedle)*.12+
    sin(trshaderPhaseBreath)*.22+sin(trshaderPhaseChatter)*.070+
    (sin(trshaderPhaseCross)*.16+sin(trshaderPhaseCrossFast)*.075)*trshaderCrossStrength;
  float trshaderRidge=trshaderFastPow5(clamp(sin(trshaderFlowWavePos.x*12.8-trshaderStreamTime*.96+trshaderLane)*.5+.5,0.0,1.0));
  float trshaderCrossRidge=trshaderFastPow5(clamp(sin(trshaderFlowWavePos.y*9.8+trshaderFlowWavePos.x*.55-trshaderStreamTime*.68)*.5+.5,0.0,1.0));
  float trshaderStreak=trshaderFastPow9(clamp(sin(trshaderFlowWavePos.x*23.0+trshaderLane*1.5-trshaderStreamTime*1.55)*.5+.5,0.0,1.0));
  float trshaderBreathEnvelope=.76+.24*(sin(trshaderPhaseBreath*.72+trshaderStreamTime*.18)*.5+.5);
  trshaderFlowLift=(trshaderFlowTrain*(1.02+.62*TR456_WATER_FLOW_WAVE_STRENGTH)+
    (trshaderRidge-.38)*.28+(trshaderCrossRidge-.36)*.20*trshaderCrossStrength)*
    12.5*trshaderBreathEnvelope*trshaderFlowVertexStrength*
    clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.70);
  trshaderFlowLift+=(trshaderStreak-.22)*2.9*
    clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.70)*trshaderFlowVertexStrength;
  trshaderFlowLift+=(trshaderStreak-.18)*2.4*clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0)*
    trshaderFlowVertexStrength;
 }
 float trshaderCalmLift=0.0;
 if(trshaderFlowMode<.999) {
  vec2 trshaderDiagA=trshaderFlowDir;
  vec2 trshaderDiagB=trshaderFlowSide;
  vec2 trshaderDiagC=normalize(trshaderFlowDir*.38-trshaderFlowSide*.92);
  float trshaderLow=sin(dot(trshaderWp0.xz,trshaderDiagA)*.0100+trshaderT*.92);
  float trshaderCrossRoll=sin(dot(trshaderWp0.xz,trshaderDiagB)*.0128-trshaderT*.80);
  float trshaderFine=sin(dot(trshaderWp0.xz,trshaderDiagC)*.027+trshaderT*1.55);
  float trshaderCrossing=sin(dot(trshaderWp0.xz,normalize(trshaderDiagA+trshaderDiagB))*.017+trshaderLow*.20-trshaderCrossRoll*.14+trshaderT*.32);
  float trshaderBreath=sin(dot(trshaderWp0.xz,normalize(trshaderFlowDir*.44+trshaderFlowSide*.90))*.0088+trshaderT*.48+trshaderCrossRoll*.10);
  trshaderCalmLift=trshaderLow*3.2+trshaderCrossRoll*2.4+trshaderCrossing*1.15+trshaderFine*1.15+trshaderBreath*1.65;
 }
 float trshaderVertexLift=mix(trshaderCalmLift,trshaderFlowLift*trshaderSurfaceFlowMask,trshaderFlowMode)+
    trshaderContactVertexLift(trshaderWp0,trshaderT);
 trshaderP.y+=trshaderVertexLift*trshaderCascadeStillMask;
 vSynPos=trshaderP.xyz;
 vSynWorldPos=trshaderP.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vSynNormal=normalize(mix(vec3(0.0,1.0,0.0),trshaderSourceNormal,trshaderCascadeMask));
 vSynUv=mix(trshaderRawUv,trshaderRawUv+trshaderFlowVector*trshaderT,trshaderFlowMode);
 vSynLight=clamp(pow(aLight.xyz,vec3(2.2))+pow(aColor.xyz,vec3(2.2))*.45,0.0,1.8);
 vSynFlowUv=trshaderFlowPos*.00072;
 vSynFlowDir=trshaderFlowDir;
 vSynFlowInfo=vec4(trshaderFlowMode,trshaderCascadeMask,trshaderDuplicatePass,trshaderSpeed);
 gl_Position=trshaderClipFromPos(trshaderP.xyz);
}
