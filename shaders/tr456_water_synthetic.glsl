#version 150

#ifndef TR456_WATER_SURFACE_RELIEF
#define TR456_WATER_SURFACE_RELIEF 1.0
#endif
#ifndef TR456_WATER_TEXTURE_STRENGTH
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#endif
#ifndef TR456_WATER_REFRACT_STRENGTH
#define TR456_WATER_REFRACT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_REFRACTION_WAVE_STRENGTH
#define TR456_WATER_REFRACTION_WAVE_STRENGTH 1.0
#endif
#ifndef TR456_WATER_REFLECT_STRENGTH
#define TR456_WATER_REFLECT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_GLINT_STRENGTH
#define TR456_WATER_GLINT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_SPARKLE_STRENGTH
#define TR456_WATER_SPARKLE_STRENGTH 0.0
#endif
#ifndef TR456_WATER_REFLECTION_CONTRAST
#define TR456_WATER_REFLECTION_CONTRAST 1.0
#endif
#ifndef TR456_WATER_COLOR_SATURATION
#define TR456_WATER_COLOR_SATURATION 1.0
#endif
#ifndef TR456_WATER_BRIGHTNESS
#define TR456_WATER_BRIGHTNESS 1.0
#endif
#ifndef TR456_WATER_VOLUME_STRENGTH
#define TR456_WATER_VOLUME_STRENGTH 1.0
#endif
#ifndef TR456_WATER_DEPTH_ABSORPTION
#define TR456_WATER_DEPTH_ABSORPTION 1.0
#endif
#ifndef TR456_WATER_DEPTH_STRENGTH
#define TR456_WATER_DEPTH_STRENGTH 1.0
#endif
#ifndef TR456_WATER_MICRO_RIPPLE
#define TR456_WATER_MICRO_RIPPLE 0.35
#endif
#ifndef TR456_WATER_SWELL_STRENGTH
#define TR456_WATER_SWELL_STRENGTH 0.45
#endif
#ifndef TR456_WATER_SHORELINE_STRENGTH
#define TR456_WATER_SHORELINE_STRENGTH 0.45
#endif
#ifndef TR456_WATER_WET_EDGE
#define TR456_WATER_WET_EDGE 0.45
#endif
#ifndef TR456_WATER_FOAM_STRENGTH
#define TR456_WATER_FOAM_STRENGTH 0.10
#endif
#ifndef TR456_WATER_SURFACE_CAUSTIC
#define TR456_WATER_SURFACE_CAUSTIC 0.0
#endif
#ifndef TR456_WATER_BLUE_STRIPE
#define TR456_WATER_BLUE_STRIPE 0.0
#endif
#ifndef TR456_WATER_STANDING_LIFE
#define TR456_WATER_STANDING_LIFE 0.0
#endif
#ifndef TR456_WATER_STANDING_MICRO_CHOP
#define TR456_WATER_STANDING_MICRO_CHOP 0.0
#endif
#ifndef TR456_WATER_STANDING_TENSION
#define TR456_WATER_STANDING_TENSION 0.0
#endif
#ifndef TR456_WATER_STANDING_DRIFT_SPEED
#define TR456_WATER_STANDING_DRIFT_SPEED 0.65
#endif
#ifndef TR456_WATER_RAIN_RIPPLE
#define TR456_WATER_RAIN_RIPPLE 0.0
#endif
#ifndef TR456_WATER_FLOW_SURFACE_TENSION
#define TR456_WATER_FLOW_SURFACE_TENSION 0.0
#endif
#ifndef TR456_WATER_FLOW_STANDING_BLEND
#define TR456_WATER_FLOW_STANDING_BLEND 0.0
#endif
#ifndef TR456_WATER_FLOW_WAVE_STRENGTH
#define TR456_WATER_FLOW_WAVE_STRENGTH 0.85
#endif
#ifndef TR456_WATER_FLOW_LONGITUDINAL_WAVE
#define TR456_WATER_FLOW_LONGITUDINAL_WAVE 1.0
#endif
#ifndef TR456_WATER_FLOW_TRANSVERSE_WAVE
#define TR456_WATER_FLOW_TRANSVERSE_WAVE 1.0
#endif
#ifndef TR456_WATER_FLOW_RELIEF_STRENGTH
#define TR456_WATER_FLOW_RELIEF_STRENGTH 1.0
#endif
#ifndef TR456_WATER_FLOW_REFRACTION_RELIEF
#define TR456_WATER_FLOW_REFRACTION_RELIEF 1.0
#endif
#ifndef TR456_WATER_FLOW_ORIGINAL_DEFORMATION
#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION 0.85
#endif
#ifndef TR456_WATER_FLOW_ORIGINAL_SYNC
#define TR456_WATER_FLOW_ORIGINAL_SYNC 1.0
#endif
#ifndef TR456_WATER_FLOW_CROSS_WAVE
#define TR456_WATER_FLOW_CROSS_WAVE 0.25
#endif
#ifndef TR456_WATER_FLOW_CROSS_DISTORTION
#define TR456_WATER_FLOW_CROSS_DISTORTION 1.0
#endif
#ifndef TR456_WATER_FLOW_VOLUME_WAVE
#define TR456_WATER_FLOW_VOLUME_WAVE 0.0
#endif
#ifndef TR456_WATER_FLOW_VOLUME_WAVE_SCALE
#define TR456_WATER_FLOW_VOLUME_WAVE_SCALE 1.0
#endif
#ifndef TR456_WATER_WAKE_STRENGTH
#define TR456_WATER_WAKE_STRENGTH 1.0
#endif
#ifndef TR456_WATER_WAKE_WIDTH
#define TR456_WATER_WAKE_WIDTH 0.50
#endif
#ifndef TR456_WATER_WAKE_LENGTH
#define TR456_WATER_WAKE_LENGTH 1.0
#endif
#ifndef TR456_WATER_WAKE_WAVE
#define TR456_WATER_WAKE_WAVE 1.0
#endif
#ifndef TR456_WATER_CONTACT_RIPPLE_DECAY
#define TR456_WATER_CONTACT_RIPPLE_DECAY 1.10
#endif
#ifndef TR456_WATER_CONTACT_WAKE_DIRECTIONAL
#define TR456_WATER_CONTACT_WAKE_DIRECTIONAL 0.72
#endif
#ifndef TR456_WATER_LARA_SPLASH_STRENGTH
#define TR456_WATER_LARA_SPLASH_STRENGTH 1.0
#endif
#ifndef TR456_WATER_CONTACT_EDGE
#define TR456_WATER_CONTACT_EDGE 0.45
#endif
#ifndef TR456_WATER_FLOW_CONTACT_STRENGTH
#define TR456_WATER_FLOW_CONTACT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_NORMAL
#define TR456_WATER_FLOW_CONTACT_NORMAL 1.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_RIPPLES
#define TR456_WATER_FLOW_CONTACT_RIPPLES 0.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_DISTORTION
#define TR456_WATER_FLOW_CONTACT_DISTORTION 1.0
#endif
#ifndef TR456_WATER_CONTACT_MAX_ACTIVE
#define TR456_WATER_CONTACT_MAX_ACTIVE 6
#endif
#ifndef TR456_WATER_EDGE_WAVE
#define TR456_WATER_EDGE_WAVE 0.55
#endif
#ifndef TR456_WATER_BUMP_STRENGTH
#define TR456_WATER_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_BUMP_SCALE
#define TR456_WATER_BUMP_SCALE 1.0
#endif
#ifndef TR456_WATER_FLOW_BUMP_STRENGTH
#define TR456_WATER_FLOW_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_SYNTHETIC_BUMP_STRENGTH
#define TR456_WATER_SYNTHETIC_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_SYNTHETIC_BUMP_ENABLED
#define TR456_WATER_SYNTHETIC_BUMP_ENABLED 0
#endif
#ifndef TR456_WATER_SYNTHETIC_REFLECTION_ENABLED
#define TR456_WATER_SYNTHETIC_REFLECTION_ENABLED 1
#endif
#ifndef TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED
#define TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED 1
#endif
#ifndef TR456_WATER_REFLECTION_QUALITY
#define TR456_WATER_REFLECTION_QUALITY 1
#endif
#ifndef TR456_WATER_REFLECTION_SHIMMER
#define TR456_WATER_REFLECTION_SHIMMER 0.18
#endif
#ifndef TR456_WATER_UNDERLAY_PATTERN_STRENGTH
#define TR456_WATER_UNDERLAY_PATTERN_STRENGTH 0.0
#endif
uniform sampler2D uTrWaterScene;
uniform sampler2D uTrWaterUnderlay;
uniform vec4 uTrWaterCaptureInfo;
uniform vec4 uTrWaterUnderlayInfo;
uniform vec4 uTrWaterSyntheticInfo;
uniform vec4 uTrWaterSyntheticMode;
uniform vec4 uTrWaterSyntheticProfile;
uniform vec4 uParams;
uniform vec4 uTrWaterDrawInfo;
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
uniform vec4 uContacts[16];
uniform vec4 uContactMotion[16];

in vec3 vSynPos;
in vec3 vSynWorldPos;
in vec3 vSynNormal;
in vec2 vSynUv;
in vec3 vSynLight;
in vec2 vSynFlowUv;
in vec2 vSynFlowDir;
in vec4 vSynFlowInfo;

out vec4 trshaderFragColor;

float trshaderSat(float trshaderX){ return clamp(trshaderX,0.0,1.0); }
float trshaderFastPow2(float trshaderX){ return trshaderX*trshaderX; }
float trshaderFastPow5(float trshaderX){ float trshaderX2=trshaderX*trshaderX; return trshaderX2*trshaderX2*trshaderX; }
float trshaderFastPow6(float trshaderX){ float trshaderX2=trshaderX*trshaderX; return trshaderX2*trshaderX2*trshaderX2; }
float trshaderFastPow9(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; return trshaderX4*trshaderX4*trshaderX; }
float trshaderFastPow13(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; float trshaderX8=trshaderX4*trshaderX4; return trshaderX8*trshaderX4*trshaderX; }
float trshaderFastPow18(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; float trshaderX8=trshaderX4*trshaderX4; float trshaderX16=trshaderX8*trshaderX8; return trshaderX16*trshaderX2; }
float trshaderFastPow48(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; float trshaderX8=trshaderX4*trshaderX4; float trshaderX16=trshaderX8*trshaderX8; float trshaderX32=trshaderX16*trshaderX16; return trshaderX32*trshaderX16; }
float trshaderFastPow58(float trshaderX){ float trshaderX2=trshaderX*trshaderX; float trshaderX4=trshaderX2*trshaderX2; float trshaderX8=trshaderX4*trshaderX4; float trshaderX16=trshaderX8*trshaderX8; float trshaderX32=trshaderX16*trshaderX16; return trshaderX32*trshaderX16*trshaderX8*trshaderX2; }
float trshaderLuma(vec3 trshaderC){ return dot(trshaderC,vec3(.299,.587,.114)); }

#define TR_TOGGLE_FLOW_FOAM uTrWaterToggle0.x
#define TR_TOGGLE_FLOW_CHROMA uTrWaterToggle0.y
#define TR_TOGGLE_FLOW_LANES uTrWaterToggle0.w
#define TR_TOGGLE_FLOW_WARP uTrWaterToggle1.x
#define TR_TOGGLE_FLOW_REFLECTION uTrWaterToggle1.y
#define TR_TOGGLE_SURFACE_FOAM uTrWaterToggle2.x
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

vec3 trshaderReflectionGrade(vec3 trshaderC){
 trshaderC=max(trshaderC-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 float trshaderY=trshaderLuma(trshaderC);
 trshaderC=mix(vec3(trshaderY),trshaderC,clamp(TR456_WATER_COLOR_SATURATION*.84,.55,1.08));
 trshaderC=mix(trshaderC,trshaderC*vec3(.86,.96,1.03),.12);
 return clamp(trshaderC*(.90+.10*TR456_WATER_BRIGHTNESS),vec3(0.0),vec3(2.1));
}

vec3 trshaderOriginalWaterGrade(vec3 trshaderC){
 float trshaderY=trshaderLuma(trshaderC);
 trshaderC=mix(vec3(trshaderY)*vec3(.84,.92,.94),trshaderC,clamp(TR456_WATER_COLOR_SATURATION*.74,.50,1.04));
 return clamp(trshaderC*(.94+.08*TR456_WATER_BRIGHTNESS),vec3(0.0),vec3(1.55));
}

vec3 trshaderWaterVolume(vec3 trshaderC, float trshaderDepth, float trshaderNdv, vec3 trshaderTint){
 float trshaderPath=trshaderSat(trshaderDepth*(.58+.18*(1.0-trshaderNdv))*
   TR456_WATER_VOLUME_STRENGTH*TR456_WATER_DEPTH_ABSORPTION);
 vec3 trshaderAbsorbed=trshaderC*exp(-vec3(.48,.20,.10)*trshaderPath);
 trshaderAbsorbed=mix(trshaderAbsorbed,vec3(trshaderLuma(trshaderAbsorbed))*vec3(.82,.91,.93),trshaderPath*.20);
 return mix(trshaderAbsorbed,trshaderTint,trshaderSat(trshaderPath*.052));
}

float trshaderDepthAwareOpacity(float trshaderOpacity, float trshaderDepth){
  return clamp(trshaderOpacity,0.0,1.0);
}

float trshaderStableWaterBody(float trshaderOpacity, float trshaderSignal, float trshaderEdge){
 return trshaderSat(trshaderOpacity*.34+trshaderSignal*.14+trshaderEdge*.10);
}

vec3 trshaderHoldWaterTint(vec3 trshaderC, vec3 trshaderTint, float trshaderAmount){
 float trshaderY=trshaderLuma(trshaderC);
 vec3 trshaderNeutral=vec3(trshaderY)*vec3(.72,.93,1.00);
 vec3 trshaderCooled=trshaderC*vec3(.86,1.00,1.07)+trshaderTint*.48;
 return mix(trshaderC,mix(trshaderNeutral,trshaderCooled,.68),trshaderSat(trshaderAmount));
}

float trshaderStressFoamGate(float trshaderStress){
  return smoothstep(.16,.62,trshaderSat(trshaderStress));
}

float trshaderContactRadius(vec4 trshaderC){
 float trshaderEncoded=abs(trshaderC.w);
 float trshaderEncodedMode=step(49152.0,trshaderEncoded);
 float trshaderNativeRadius=clamp(trshaderEncoded*.025,90.0,340.0);
 float trshaderEncodedRadius=clamp(floor(trshaderEncoded*(1.0/512.0)),90.0,720.0);
 return mix(trshaderNativeRadius,trshaderEncodedRadius,trshaderEncodedMode);
}

vec3 trshaderBaseWaterField(vec2 trshaderW, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderA=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 vec2 trshaderB=vec2(-trshaderA.y,trshaderA.x);
 vec2 trshaderC=normalize(trshaderA*.38-trshaderB*.92);
 vec2 trshaderD=normalize(trshaderA+trshaderB);
 float trshaderP1=dot(trshaderW,trshaderA)*.017+trshaderT*1.10;
 float trshaderP2=dot(trshaderW,trshaderB)*.014-trshaderT*1.02;
 float trshaderP3=dot(trshaderW,trshaderC)*.026+trshaderT*1.56;
 float trshaderP4=dot(trshaderW,trshaderD)*.047-trshaderT*2.30;
 float trshaderPx=trshaderP1*.58-trshaderP2*.46+trshaderT*.18;
 vec2 trshaderSlope=trshaderA*cos(trshaderP1)*.018*.72+
            trshaderB*cos(trshaderP2)*.014*.56+
            trshaderC*cos(trshaderP3)*.028*.22+
            trshaderD*cos(trshaderP4)*.047*.070+
            normalize(trshaderA-trshaderB)*cos(trshaderPx)*.010*.34;
 float trshaderRidge=trshaderFastPow9(trshaderSat(sin(trshaderP4)*.5+.5));
 float trshaderCrossing=sin(trshaderPx)*(.45+.55*trshaderSat(sin(trshaderP1)*sin(trshaderP2)*.5+.5));
 float trshaderH=sin(trshaderP1)*.62+sin(trshaderP2)*.56+sin(trshaderP3)*.18+trshaderRidge*.28+trshaderCrossing*.20;
 return vec3(trshaderSlope*60.0,trshaderH);
}

float trshaderReliefHeight(vec2 trshaderW, vec2 trshaderUv, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderP=trshaderW*.0048+trshaderUv*2.2;
 vec2 trshaderA=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 vec2 trshaderB=vec2(-trshaderA.y,trshaderA.x);
 vec2 trshaderC=normalize(trshaderA*.34-trshaderB*.94);
 float trshaderR0=sin(dot(trshaderP,trshaderA)*6.20+trshaderT*.42+sin(dot(trshaderP,trshaderB)*1.70-trshaderT*.09)*.16);
 float trshaderR1=sin(dot(trshaderP,trshaderB)*9.50-trshaderT*.36+sin(dot(trshaderP,trshaderC)*2.10+trshaderT*.07)*.12);
 float trshaderR2=sin(dot(trshaderP,trshaderC)*14.0+trshaderT*.27);
 float trshaderLace=pow(trshaderSat(sin(dot(trshaderP,normalize(trshaderA+trshaderB))*12.0+trshaderT*.20)*.5+.5),2.8)-.42;
 float trshaderGrain=sin((trshaderR0*.58+trshaderR1*.42)*2.40+trshaderR2*.10);
 return (trshaderR0*.105+trshaderR1*.060+trshaderR2*.032+trshaderLace*.035+trshaderGrain*.020);
}

vec3 trshaderReliefField(vec2 trshaderW, vec2 trshaderUv, float trshaderT, vec2 trshaderPrimaryDir){
 float trshaderStepSize=22.0;
 float trshaderH=trshaderReliefHeight(trshaderW,trshaderUv,trshaderT,trshaderPrimaryDir);
 float trshaderHx=trshaderReliefHeight(trshaderW+vec2(trshaderStepSize,0.0),trshaderUv+vec2(.010,0.0),trshaderT,trshaderPrimaryDir)-trshaderH;
 float trshaderHy=trshaderReliefHeight(trshaderW+vec2(0.0,trshaderStepSize),trshaderUv+vec2(0.0,.010),trshaderT,trshaderPrimaryDir)-trshaderH;
 float trshaderStrength=clamp(TR456_WATER_SURFACE_RELIEF*TR456_WATER_TEXTURE_STRENGTH,0.0,2.45)*.36;
 return vec3(vec2(trshaderHx,trshaderHy)*(6.2*trshaderStrength),trshaderH*(.22*trshaderStrength));
}

vec3 trshaderSoftMotionField(vec2 trshaderW, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderA=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 vec2 trshaderB=vec2(-trshaderA.y,trshaderA.x);
 vec2 trshaderC=normalize(trshaderA*.72-trshaderB*.69);
 float trshaderS0=sin(dot(trshaderW,trshaderA)*.0105+trshaderT*.46);
 float trshaderS1=sin(dot(trshaderW,trshaderB)*.0150-trshaderT*.38+trshaderS0*.18);
 float trshaderS2=sin(dot(trshaderW,trshaderC)*.0270+trshaderT*.92+trshaderS1*.10);
 vec2 trshaderSlope=(trshaderA*cos(dot(trshaderW,trshaderA)*.0105+trshaderT*.46)*.0105*.70+
             trshaderB*cos(dot(trshaderW,trshaderB)*.0150-trshaderT*.38+trshaderS0*.18)*.0150*.48+
             trshaderC*cos(dot(trshaderW,trshaderC)*.0270+trshaderT*.92+trshaderS1*.10)*.0270*.16);
 float trshaderSoftLine=trshaderFastPow6(trshaderSat(sin(dot(trshaderW,normalize(trshaderA+trshaderB))*.032+trshaderT*1.12+trshaderS0*.24)*.5+.5));
 float trshaderStrength=clamp(TR456_WATER_SWELL_STRENGTH*.70+TR456_WATER_MICRO_RIPPLE*.48,0.0,1.8);
 return vec3(trshaderSlope*(58.0*trshaderStrength),trshaderSoftLine*trshaderStrength);
}

float trshaderHash12(vec2 trshaderP){
 vec3 trshaderP3=fract(vec3(trshaderP.xyx)*.1031);
 trshaderP3+=dot(trshaderP3,trshaderP3.yzx+33.33);
 return fract((trshaderP3.x+trshaderP3.y)*trshaderP3.z);
}

float trshaderValueNoise(vec2 trshaderP){
 vec2 trshaderI=floor(trshaderP);
 vec2 trshaderF=fract(trshaderP);
 trshaderF=trshaderF*trshaderF*(3.0-2.0*trshaderF);
 float trshaderA=trshaderHash12(trshaderI);
 float trshaderB=trshaderHash12(trshaderI+vec2(1.0,0.0));
 float trshaderC=trshaderHash12(trshaderI+vec2(0.0,1.0));
 float trshaderD=trshaderHash12(trshaderI+vec2(1.0,1.0));
 return mix(mix(trshaderA,trshaderB,trshaderF.x),mix(trshaderC,trshaderD,trshaderF.x),trshaderF.y);
}

float trshaderLineMask(float trshaderX, float trshaderSharpness){
 return pow(trshaderSat(1.0-abs(fract(trshaderX)-.5)*2.0),trshaderSharpness);
}

float trshaderFbmNoise(vec2 trshaderP){
 float trshaderV=0.0;
 float trshaderA=.55;
 mat2 trshaderR=mat2(.80,-.60,.60,.80);
 for(int trshaderI=0;trshaderI<3;trshaderI++){
  trshaderV+=trshaderValueNoise(trshaderP)*trshaderA;
  trshaderP=trshaderR*trshaderP*2.07+vec2(17.31,9.17);
  trshaderA*=.55;
 }
 return trshaderV;
}

vec2 trshaderSoftLimitVec2(vec2 trshaderV, float trshaderLimit){
 float trshaderM=length(trshaderV);
 float trshaderSafeLimit=max(trshaderLimit,.00001);
 float trshaderScale=(trshaderSafeLimit*(1.0-exp(-trshaderM/trshaderSafeLimit)))/max(trshaderM,.00001);
 return trshaderV*trshaderScale;
}

vec2 trshaderSyntheticBumpSlope(vec2 trshaderSlope, float trshaderAmount, float trshaderLimit){
 float trshaderA=clamp(trshaderAmount,0.0,2.2);
 if(trshaderA<=.001) return vec2(0.0);
 return trshaderSoftLimitVec2(trshaderSlope*max(TR456_WATER_BUMP_SCALE,.10),trshaderLimit)*
   trshaderA;
}

vec2 trshaderSyntheticStandingBump(vec2 trshaderBaseSlope, vec2 trshaderReliefSlope,
                           vec2 trshaderAliveSlope, vec2 trshaderRippleSlope){
 float trshaderAmount=TR456_WATER_BUMP_STRENGTH*TR456_WATER_SYNTHETIC_BUMP_STRENGTH;
 return trshaderSyntheticBumpSlope(trshaderBaseSlope*.34+trshaderReliefSlope*1.35+
   trshaderAliveSlope*.74+trshaderRippleSlope*.16,trshaderAmount,.46);
}

vec2 trshaderTextureMicroBump(vec2 trshaderW, float trshaderT,
                              vec2 trshaderPrimaryDir, float trshaderFlowProfile){
 vec2 trshaderA=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 vec2 trshaderB=vec2(-trshaderA.y,trshaderA.x);
 vec2 trshaderC=normalize(trshaderA*.36+trshaderB*.93);
 float trshaderFlow=trshaderSat(trshaderFlowProfile);
 float trshaderDrift=trshaderT*mix(.72,2.65,trshaderFlow);
 float trshaderGrain=trshaderValueNoise(trshaderW*.0062+
   vec2(trshaderDrift*.038,-trshaderDrift*.024));
 float trshaderP0=dot(trshaderW,trshaderA)*mix(.024,.044,trshaderFlow)+
   trshaderDrift*.62+trshaderGrain*.95;
 float trshaderP1=dot(trshaderW,trshaderB)*mix(.032,.060,trshaderFlow)-
   trshaderDrift*.48+trshaderGrain*.70;
 float trshaderP2=dot(trshaderW,trshaderC)*mix(.056,.088,trshaderFlow)+
   trshaderDrift*1.10;
 float trshaderR0=trshaderSat(sin(trshaderP0)*.5+.5);
 float trshaderR1=trshaderSat(sin(trshaderP1)*.5+.5);
 float trshaderFine=trshaderSat(sin(trshaderP2+trshaderR0*.62-trshaderR1*.38)*.5+.5);
 float trshaderRidge0=trshaderR0*trshaderR0*(3.0-2.0*trshaderR0);
 float trshaderRidge1=trshaderR1*trshaderR1*(3.0-2.0*trshaderR1);
 vec2 trshaderSlope=trshaderA*cos(trshaderP0)*(.050+.038*trshaderFlow)*
   (.58+.42*trshaderRidge0);
 trshaderSlope+=trshaderB*cos(trshaderP1)*(.040+.032*trshaderFlow)*
   (.62+.38*trshaderRidge1);
 trshaderSlope+=trshaderC*cos(trshaderP2+trshaderR0*.62-trshaderR1*.38)*
   (.025+.030*trshaderFlow)*(trshaderFine*.72+.28);
 trshaderSlope+=(trshaderA-trshaderB)*(trshaderGrain-.5)*(.026+.026*trshaderFlow);
 return trshaderSlope*(.70+.45*trshaderFlow);
}

float trshaderReflectionUvFade(vec2 trshaderUv){
 vec2 trshaderA=smoothstep(vec2(-.060),vec2(.120),trshaderUv);
 vec2 trshaderB=smoothstep(vec2(-.060),vec2(.120),1.0-trshaderUv);
 return trshaderA.x*trshaderA.y*trshaderB.x*trshaderB.y;
}

vec2 trshaderPreciseReflectionUv(vec2 trshaderScreen, vec3 trshaderNormal, vec3 trshaderViewDir,
                         vec2 trshaderWarp, float trshaderLift, float trshaderRoughness){
 vec3 trshaderN=normalize(trshaderNormal);
 vec3 trshaderV=normalize(trshaderViewDir);
 float trshaderNdv=trshaderSat(abs(dot(trshaderN,trshaderV)));
 float trshaderGrazing=smoothstep(.14,.92,1.0-trshaderNdv);
 vec2 trshaderLocal=trshaderScreen+vec2(trshaderWarp.x,-trshaderWarp.y*.35);
 vec2 trshaderMirror=vec2(trshaderScreen.x,1.0-trshaderScreen.y-trshaderLift);
 vec2 trshaderMirrorWarp=vec2(trshaderWarp.x,-abs(trshaderWarp.y))*(.64+.36*trshaderRoughness);
 return mix(trshaderLocal,trshaderMirror+trshaderMirrorWarp,trshaderSat(.62+.38*trshaderGrazing));
}

vec3 trshaderStableSceneColor(vec2 trshaderUv, vec2 trshaderFallback){
 float trshaderFade=trshaderReflectionUvFade(trshaderUv);
 if(trshaderFade>=.999)
  return texture(uTrWaterScene,clamp(trshaderUv,vec2(.001),vec2(.999))).rgb;
 if(trshaderFade<=.001)
  return texture(uTrWaterScene,clamp(trshaderFallback,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderA=texture(uTrWaterScene,clamp(trshaderFallback,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderB=texture(uTrWaterScene,clamp(trshaderUv,vec2(.001),vec2(.999))).rgb;
 return mix(trshaderA,trshaderB,trshaderFade);
}

vec4 trshaderUnderlayPattern(vec2 trshaderScreen, vec2 trshaderDir, vec2 trshaderSide){
 float trshaderEnabled=uTrWaterUnderlayInfo.z;
 float trshaderStrength=uTrWaterUnderlayInfo.w*TR456_WATER_UNDERLAY_PATTERN_STRENGTH;
 if(trshaderEnabled<=.001 || trshaderStrength<=.001) return vec4(0.0);
 vec2 trshaderUv=clamp(trshaderScreen,vec2(.001),vec2(.999));
 vec2 trshaderStep=max(uTrWaterUnderlayInfo.xy,vec2(1.0/8192.0));
 vec2 trshaderD=normalize(trshaderDir+vec2(.0001,.0003))*trshaderStep*vec2(7.0,7.0);
 vec2 trshaderS=normalize(trshaderSide+vec2(.0003,.0001))*trshaderStep*vec2(6.0,6.0);
 vec3 trshaderUnder=texture(uTrWaterUnderlay,trshaderUv).rgb;
 vec3 trshaderScene=texture(uTrWaterScene,trshaderUv).rgb;
 vec3 trshaderUnderA=texture(uTrWaterUnderlay,clamp(trshaderUv+trshaderD,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderUnderB=texture(uTrWaterUnderlay,clamp(trshaderUv-trshaderD,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderUnderC=texture(uTrWaterUnderlay,clamp(trshaderUv+trshaderS,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderUnderD=texture(uTrWaterUnderlay,clamp(trshaderUv-trshaderS,vec2(.001),vec2(.999))).rgb;
 float trshaderBase=trshaderLuma(trshaderUnder);
 float trshaderSceneBase=trshaderLuma(trshaderScene);
 float trshaderDelta=length(trshaderUnder-trshaderScene);
 float trshaderGradA=trshaderLuma(trshaderUnderA)-trshaderLuma(trshaderUnderB);
 float trshaderGradB=trshaderLuma(trshaderUnderC)-trshaderLuma(trshaderUnderD);
 float trshaderEdge=max(abs(trshaderGradA),abs(trshaderGradB));
 float trshaderFilm=trshaderSat((trshaderDelta*2.4+trshaderEdge*4.8+
   abs(trshaderBase-trshaderSceneBase)*1.8)*trshaderStrength);
 float trshaderBright=trshaderSat((trshaderBase-trshaderSceneBase+.08)*2.6*trshaderStrength);
 return vec4(trshaderFilm,trshaderGradA*trshaderStrength,
   trshaderGradB*trshaderStrength,trshaderBright);
}

vec4 trshaderFlowPatchField(vec2 trshaderP, float trshaderTravel, float trshaderSpeed){
 vec2 trshaderDrift=vec2(-trshaderTravel*(.030+trshaderSpeed*.006),trshaderTravel*(.012+trshaderSpeed*.002));
 vec2 trshaderGrid=trshaderP*vec2(2.65,4.75)+trshaderDrift;
 vec2 trshaderBase=floor(trshaderGrid);
 vec2 trshaderLocal=fract(trshaderGrid);
 float trshaderPatchData=0.0;
 float trshaderRim=0.0;
 float trshaderToneSum=0.0;
 float trshaderWeight=0.0;
 for(int trshaderIx=-1;trshaderIx<=1;trshaderIx++){
  for(int trshaderIy=-1;trshaderIy<=1;trshaderIy++){
   vec2 trshaderOffs=vec2(float(trshaderIx),float(trshaderIy));
   vec2 trshaderId=trshaderBase+trshaderOffs;
   float trshaderRnd=trshaderHash12(trshaderId+vec2(11.7,3.4));
   vec2 trshaderCenter=vec2(trshaderHash12(trshaderId+vec2(2.7,4.1)),
     trshaderHash12(trshaderId+vec2(6.3,1.9)));
   vec2 trshaderD=trshaderLocal-(trshaderOffs+trshaderCenter);
   trshaderD.x*=mix(.64,1.58,trshaderHash12(trshaderId+vec2(4.4,8.2)));
   trshaderD.y*=mix(1.36,.72,trshaderHash12(trshaderId+vec2(9.1,2.6)));
   float trshaderDist=length(trshaderD);
   float trshaderRadius=mix(.18,.44,trshaderRnd);
   float trshaderBlob=1.0-smoothstep(trshaderRadius,trshaderRadius+.22,trshaderDist);
   float trshaderShell=trshaderBlob*smoothstep(trshaderRadius*.36,trshaderRadius*.88,trshaderDist);
   float trshaderLife=.76+.24*sin(trshaderTravel*(.035+trshaderSpeed*.006)+trshaderRnd*6.28318);
   float trshaderV=trshaderBlob*trshaderLife;
   float trshaderCellTone=trshaderHash12(trshaderId+vec2(12.5,9.1));
   trshaderPatchData=max(trshaderPatchData,trshaderV);
   trshaderRim=max(trshaderRim,trshaderShell*trshaderLife);
   trshaderToneSum+=trshaderCellTone*trshaderV;
   trshaderWeight+=trshaderV;
  }
 }
 float trshaderPatchTone=trshaderWeight>.001 ? trshaderToneSum/trshaderWeight :
   trshaderValueNoise(trshaderP*vec2(5.0,7.0)+trshaderDrift*.37);
 float trshaderGrain=trshaderValueNoise(trshaderP*vec2(19.0,11.0)+vec2(-trshaderTravel*.080,trshaderTravel*.035));
 return vec4(trshaderSat(trshaderPatchData),trshaderSat(trshaderRim),trshaderSat(trshaderPatchTone),trshaderGrain);
}

vec4 trshaderWaterJunctionField(vec2 trshaderP, float trshaderTravel, float trshaderSpeed){
 vec4 trshaderPatchData=trshaderFlowPatchField(trshaderP*0.82+vec2(.17,.41),trshaderTravel*.82,trshaderSpeed);
 vec2 trshaderDrift=vec2(-trshaderTravel*(.026+trshaderSpeed*.004),trshaderTravel*(.010+trshaderSpeed*.002));
 vec2 trshaderQ=trshaderP+vec2(trshaderPatchData.z-.5,trshaderPatchData.w-.5)*(.034+.048*trshaderPatchData.x);
 float trshaderBroad=trshaderValueNoise(trshaderQ*vec2(1.35,2.75)+trshaderDrift*.32);
 float trshaderMid=trshaderValueNoise(trshaderQ*vec2(4.8,8.6)+trshaderDrift);
 float trshaderFine=trshaderValueNoise(trshaderQ*vec2(13.5,18.0)+vec2(-trshaderTravel*.18,trshaderTravel*.045));
 float trshaderTongue=sin(trshaderQ.x*8.2+trshaderQ.y*.92-trshaderTravel*(.58+trshaderSpeed*.10)+(trshaderMid-.5)*1.75)*.5+.5;
 trshaderTongue=pow(trshaderSat(trshaderTongue),2.15)*smoothstep(.24,.88,trshaderMid*.62+trshaderBroad*.38);
 float trshaderLace=smoothstep(.48,.93,trshaderMid*.42+trshaderFine*.38+trshaderPatchData.y*.36);
 float trshaderBreakup=trshaderSat(trshaderBroad*.46+trshaderMid*.30+trshaderPatchData.x*.18+trshaderFine*.06);
 float trshaderFoam=trshaderSat(trshaderLace*.58+trshaderTongue*.30+trshaderPatchData.y*.42);
 return vec4(trshaderBreakup,trshaderLace,trshaderTongue,trshaderFoam);
}

float trshaderFlowStandingJunctionBlend(float trshaderBaseBlend, vec4 trshaderJunction,
                                float trshaderShoreline, float trshaderCascadeMask){
 float trshaderBreakup=clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 float trshaderLocal=(trshaderJunction.x-.5)*.12+(trshaderJunction.y-.5)*.075+
   (trshaderJunction.z-.5)*.060;
 float trshaderCascadeAgitation=smoothstep(.08,.78,trshaderCascadeMask)*(.045+.045*trshaderJunction.z);
 float trshaderThinCascade=1.0-smoothstep(.30,.86,uTrWaterSyntheticProfile.z);
 float trshaderPoolSettle=smoothstep(.58,.94,trshaderBaseBlend)*
   smoothstep(.04,.62,trshaderCascadeMask)*
   (.045+.060*trshaderJunction.w+.050*trshaderThinCascade);
  float trshaderB=trshaderBaseBlend+trshaderLocal*trshaderBreakup*trshaderBaseBlend*(1.0-trshaderBaseBlend)*2.6+
   trshaderShoreline*.035*trshaderBaseBlend+trshaderPoolSettle-
   trshaderCascadeAgitation*(1.0-trshaderBaseBlend)*.75;
 return trshaderSat(trshaderB);
}

float trshaderCascadeJunctionBlend(float trshaderCascadeMask, vec4 trshaderJunction,
                           float trshaderStandingBlend, float trshaderShoreline){
 float trshaderBreakup=clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 float trshaderDissolve=(trshaderJunction.x-.5)*.22+(trshaderJunction.z-.5)*.20+
   (trshaderJunction.y-.5)*.10+trshaderShoreline*.035-trshaderStandingBlend*.025;
 float trshaderB=trshaderCascadeMask+trshaderDissolve*trshaderBreakup;
 return smoothstep(.18,.86,trshaderB)*smoothstep(.015,.22,trshaderCascadeMask);
}

float trshaderJunctionFoamMask(float trshaderCascadeBlend, vec4 trshaderJunction,
                       float trshaderShoreline, float trshaderStandingBlend){
 float trshaderSeam=trshaderSat(1.0-abs(trshaderCascadeBlend-.52)*2.25);
 float trshaderFallLip=smoothstep(.62,1.0,trshaderCascadeBlend)*(.24+.42*trshaderJunction.w);
 float trshaderCalmLift=mix(1.0,.72,trshaderStandingBlend);
 return trshaderSat((trshaderSeam*(trshaderJunction.w*.54+trshaderJunction.y*.24+trshaderShoreline*.36)+
   trshaderFallLip*(trshaderJunction.w*.42+trshaderJunction.z*.26+trshaderShoreline*.18))*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM*trshaderCalmLift);
}

float trshaderReceivingPoolBlend(float trshaderCascadeMask, float trshaderCascadeBlend,
                         float trshaderStandingBlend, vec4 trshaderJunction,
                         float trshaderShoreline){
 float trshaderSettled=smoothstep(.58,.96,trshaderStandingBlend);
 float trshaderCascadeEdge=trshaderSat(1.0-abs(trshaderCascadeBlend-.42)*2.05);
 float trshaderCascadeReach=smoothstep(.035,.58,trshaderCascadeMask);
 float trshaderThinCascade=1.0-smoothstep(.30,.86,uTrWaterSyntheticProfile.z);
 float trshaderTongue=smoothstep(.24,.82,trshaderJunction.z*.52+trshaderJunction.x*.30+
   trshaderJunction.w*.18);
 float trshaderLace=smoothstep(.34,.92,trshaderJunction.y*.46+trshaderJunction.w*.34+
   trshaderShoreline*.28);
 float trshaderSoftPool=smoothstep(.78,.98,trshaderStandingBlend)*
   smoothstep(.62,.96,trshaderJunction.x)*.14;
 float trshaderPlungePool=smoothstep(.45,.98,trshaderCascadeMask)*trshaderSettled*trshaderThinCascade*
   (.18+.30*trshaderJunction.w+.12*trshaderShoreline);
 float trshaderTriple=trshaderSat((trshaderCascadeReach*.54+trshaderCascadeEdge*.62+trshaderShoreline*.20)*
   (trshaderTongue*.72+trshaderLace*.34)*trshaderSettled);
 return trshaderSat(trshaderTriple*1.25+trshaderSoftPool+trshaderPlungePool)*(1.0-trshaderCascadeBlend*.24);
}

float trshaderReceivingPoolFoam(float trshaderPoolBlend, float trshaderCascadeBlend,
                        vec4 trshaderJunction, float trshaderShoreline){
 float trshaderSeam=trshaderSat(1.0-abs(trshaderCascadeBlend-.42)*2.15);
 float trshaderPlume=smoothstep(.10,.82,trshaderPoolBlend)*(.55+.45*trshaderJunction.w);
 return trshaderSat((trshaderPoolBlend*.95+trshaderSeam*.42+trshaderPlume*.34)*(trshaderJunction.w*.58+
   trshaderJunction.y*.36+trshaderJunction.z*.22+trshaderShoreline*.42)*
   TR456_WATER_FLOW_STREAK_FOAM*2.15*
   TR_TOGGLE_FLOW_FOAM);
}

float trshaderFlowReceivingBranchMask(vec4 trshaderJunction){
 float trshaderCascadeProfile=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
 float trshaderDrawCount=max(uTrWaterDrawInfo.z,0.0);
 float trshaderDrawIndex=max(uTrWaterDrawInfo.y,0.0);
 float trshaderPatchSettle=smoothstep(.20,.86,
   trshaderJunction.w*.55+trshaderJunction.x*.25+trshaderJunction.y*.20);
 float trshaderBranchDraw=smoothstep(8.0,14.0,trshaderDrawCount)*
   (1.0-smoothstep(24.0,32.0,trshaderDrawCount));
 float trshaderBranchIndex=1.0-smoothstep(2.45,3.10,trshaderDrawIndex);
 float trshaderBranchParams=(1.0-smoothstep(.015,.060,abs(uParams.x)))*
   smoothstep(1.15,1.30,abs(uParams.y))*
   (1.0-smoothstep(1.66,1.82,abs(uParams.y)));
 return trshaderSat(trshaderBranchDraw*trshaderBranchIndex*trshaderBranchParams*trshaderCascadeProfile*
   (.82+.18*trshaderPatchSettle));
}

float trshaderFlowPoolReplacementMask(float trshaderPoolBlend, float trshaderStandingBlend,
                              vec4 trshaderJunction){
 float trshaderHorizontalSurface=smoothstep(.38,.78,abs(vSynNormal.y));
 float trshaderCascadeProfile=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
 float trshaderDrawCount=max(uTrWaterDrawInfo.z,0.0);
 float trshaderHasCount=step(1.0,trshaderDrawCount);
 float trshaderCompactDraw=trshaderHasCount*(1.0-smoothstep(384.0,2048.0,trshaderDrawCount));
 float trshaderPatchSettle=smoothstep(.20,.86,
   trshaderJunction.w*.55+trshaderJunction.x*.25+trshaderJunction.y*.20);
 float trshaderPoolNeed=smoothstep(.26,.62,trshaderPoolBlend)*
   smoothstep(.50,.86,trshaderStandingBlend);
 float trshaderReceivingBranch=trshaderFlowReceivingBranchMask(trshaderJunction);
 float trshaderSettledPool=trshaderPoolNeed*trshaderHorizontalSurface*trshaderCascadeProfile*
   (.62+.38*trshaderCompactDraw)*(.70+.30*trshaderPatchSettle);
 return trshaderSat(max(trshaderSettledPool,trshaderReceivingBranch));
}

vec4 trshaderSyntheticFlowPattern(vec2 trshaderP, float trshaderTime, float trshaderSpeed, vec4 trshaderPatchField){
 vec2 trshaderQ=trshaderP;
  float trshaderAdv=trshaderTime*(.18+trshaderSpeed*.20);
  float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float trshaderLongWaveCtl=clamp(TR456_WATER_FLOW_LONGITUDINAL_WAVE,0.0,2.4);
  float trshaderTransWaveCtl=clamp(TR456_WATER_FLOW_TRANSVERSE_WAVE,0.0,2.4);
  trshaderQ+=vec2(trshaderPatchField.z-.5,trshaderPatchField.w-.5)*(.026+.046*trshaderPatchField.x);
 float trshaderN0=trshaderValueNoise(trshaderQ*4.6+vec2(trshaderAdv*.10,-trshaderAdv*.055));
 float trshaderN1=trshaderValueNoise(trshaderQ.yx*6.3+vec2(-trshaderAdv*.080,trshaderAdv*.095));
 float trshaderN2=trshaderValueNoise(trshaderQ*12.5+vec2(trshaderAdv*.13,trshaderAdv*.040));
 float trshaderStreamCoord=trshaderQ.x*13.8+trshaderN0*.34-trshaderAdv*(.62+trshaderSpeed*.20);
 float trshaderFineCoord=trshaderQ.x*31.0+trshaderN1*.42-trshaderAdv*(1.26+trshaderSpeed*.30);
  float trshaderStream=max(trshaderLineMask(trshaderStreamCoord,9.5)*.28,
    trshaderLineMask(trshaderFineCoord,19.0)*.38)*trshaderLongWaveCtl;
 trshaderStream*=mix(1.0,mix(.70,1.22,trshaderPatchField.z),trshaderPatchField.x*.62);
 float trshaderSideDrift=sin(trshaderQ.x*1.85-trshaderAdv*.24+trshaderN0*2.0)*.20+
   sin(trshaderQ.x*3.8+trshaderAdv*.12+trshaderN1*1.5)*.075;
 float trshaderLaneCoord=trshaderQ.y*4.8+trshaderSideDrift+trshaderN1*.70;
 float trshaderLaneBroad=smoothstep(.58,.92,
   trshaderValueNoise(vec2(trshaderLaneCoord*.35,trshaderQ.x*.55-trshaderAdv*.20)));
 float trshaderLaneThread=pow(trshaderSat(sin(trshaderLaneCoord*3.6+trshaderN0*1.5)*.5+.5),4.2);
 float trshaderLane=trshaderLaneBroad*trshaderLaneThread*TR456_WATER_FLOW_LANE*TR_TOGGLE_FLOW_LANES;
 trshaderLane*=mix(1.0,mix(.70,1.16,trshaderPatchField.w),trshaderPatchField.x*.48);
 float trshaderRibbonCoord=trshaderQ.x*6.8+trshaderN0*.44-trshaderAdv*(.54+trshaderSpeed*.16)+sin(trshaderQ.y*3.0+trshaderN1)*.12;
 float trshaderRibbonFine=trshaderLineMask(trshaderQ.x*13.6+trshaderN1*.52-trshaderAdv*(1.04+trshaderSpeed*.20)+
   sin(trshaderQ.y*4.6+trshaderN0)*.14,15.0)*smoothstep(.42,.92,trshaderN2);
  float trshaderRibbon=(trshaderLineMask(trshaderRibbonCoord,6.4)*.72+trshaderRibbonFine*.42)*smoothstep(.36,.88,trshaderN2)*
    TR456_WATER_FLOW_RIBBON*TR_TOGGLE_FLOW_LANES;
  trshaderRibbon*=trshaderLongWaveCtl;
  trshaderRibbon*=mix(1.0,mix(.72,1.24,trshaderPatchField.z),trshaderPatchField.x*.50);
 float trshaderEddy=smoothstep(.55,.90,
   trshaderValueNoise(trshaderQ*1.7+vec2(trshaderAdv*.04,-trshaderAdv*.03)))*
   TR456_WATER_FLOW_SWIRL*TR_TOGGLE_FLOW_LANES;
 float trshaderChop=sin(trshaderQ.x*72.0+trshaderQ.y*2.6+trshaderN2*3.3-trshaderAdv*(1.68+trshaderSpeed*.42));
 float trshaderStreak=pow(trshaderSat(sin(trshaderFineCoord+trshaderN2*.58)*.5+.5),8.2)*smoothstep(.24,.90,trshaderN2);
 float trshaderCrossCoord=trshaderQ.y*16.5+sin(trshaderQ.x*3.2-trshaderAdv*.18+trshaderN1)*.22-
   trshaderAdv*(.36+trshaderSpeed*.12);
  float trshaderCrossFine=trshaderQ.y*34.0+trshaderQ.x*1.8+trshaderN0*.34-trshaderAdv*(.70+trshaderSpeed*.18);
  float trshaderCrossWave=(trshaderLineMask(trshaderCrossCoord,7.5)*.26+
    trshaderLineMask(trshaderCrossFine,16.0)*.18)*trshaderCrossStrength*trshaderTransWaveCtl*TR_TOGGLE_FLOW_LANES;
 float trshaderLifePower=clamp(TR456_WATER_FLOW_DETAIL,0.0,2.0);
 float trshaderLifePulse=pow(trshaderSat(sin(trshaderQ.x*5.1+trshaderQ.y*1.7-
   trshaderAdv*(.32+trshaderSpeed*.06)+trshaderN0*1.4)*.5+.5),2.4)*
   smoothstep(.22,.86,trshaderN1);
  float trshaderNeedle=trshaderLineMask(trshaderQ.x*18.0+trshaderQ.y*.65+
    trshaderN2*.62-trshaderAdv*(1.02+trshaderSpeed*.22),24.0)*
    smoothstep(.34,.92,trshaderLifePulse+trshaderPatchField.y*.28)*
    trshaderLifePower*trshaderLongWaveCtl;
  float trshaderBrokenWake=trshaderLineMask(trshaderQ.x*9.2-trshaderQ.y*.48+
    trshaderN0*.46-trshaderAdv*(.74+trshaderSpeed*.18),11.0)*
    smoothstep(.42,.94,trshaderN2+trshaderPatchField.x*.22)*
    trshaderLifePower*trshaderLongWaveCtl;
 float trshaderCrest=trshaderSat(trshaderStream*.30+trshaderLane*.30+trshaderRibbon*.32+trshaderStreak*.28+
   trshaderCrossWave*.30+trshaderPatchField.y*.18+trshaderPatchField.x*.060+
    trshaderEddy*.14+trshaderNeedle*.18+trshaderBrokenWake*.12+
    abs(trshaderChop)*.11*TR456_WATER_FLOW_BREAKUP*trshaderLongWaveCtl);
 float trshaderSwirl=max(trshaderEddy,trshaderPatchField.y*.25*TR456_WATER_FLOW_SWIRL*TR_TOGGLE_FLOW_LANES);
 float trshaderFoam=(trshaderLineMask(trshaderStreamCoord+trshaderN2*.18,10.0)*.18+trshaderLineMask(trshaderFineCoord+trshaderN0*.24,22.0)*.16+
   trshaderLane*.28+trshaderRibbon*.28+trshaderRibbonFine*.18+trshaderStreak*.18+trshaderCrossWave*.14+trshaderPatchField.y*.12+
   trshaderSwirl*.12+trshaderNeedle*.18+trshaderBrokenWake*.10+
   trshaderCrest*trshaderCrest*.14)*TR_TOGGLE_FLOW_FOAM;
 return vec4(trshaderCrest,trshaderFoam,trshaderSwirl,trshaderStream);
}

vec3 trshaderSyntheticFlowField(vec2 trshaderP, float trshaderTime, float trshaderSpeed, vec4 trshaderPatchField){
 vec2 trshaderQ=trshaderP;
  trshaderQ+=vec2(trshaderPatchField.z-.5,trshaderPatchField.w-.5)*(.020+.040*trshaderPatchField.x);
  float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float trshaderLongWaveCtl=clamp(TR456_WATER_FLOW_LONGITUDINAL_WAVE,0.0,2.4);
  float trshaderTransWaveCtl=clamp(TR456_WATER_FLOW_TRANSVERSE_WAVE,0.0,2.4);
  float trshaderN0=trshaderValueNoise(trshaderQ*4.2+vec2(trshaderTime*.020,-trshaderTime*.011))*2.0-1.0;
 float trshaderN1=trshaderValueNoise(trshaderQ.yx*5.8+vec2(-trshaderTime*.017,trshaderTime*.019))*2.0-1.0;
 float trshaderAdv=trshaderTime*(.70+trshaderSpeed*.28);
 float trshaderMain=sin(trshaderQ.x*29.0+trshaderN0*1.8-trshaderAdv*(.70+trshaderSpeed*.12));
 float trshaderLongWave=sin(trshaderQ.x*14.5+trshaderQ.y*2.4+trshaderN1*1.2-trshaderAdv*(.42+trshaderSpeed*.10));
 float trshaderSideTurb=sin(trshaderQ.x*18.0+trshaderQ.y*1.15+trshaderN1*1.7-trshaderAdv*.48);
 float trshaderFast=sin(trshaderQ.x*78.0+trshaderN0*2.5-trshaderAdv*(1.34+trshaderSpeed*.28));
 float trshaderCrossMain=sin(trshaderQ.y*24.0+trshaderQ.x*2.3+trshaderN0*1.4-trshaderAdv*(.50+trshaderSpeed*.09));
 float trshaderCrossFast=sin(trshaderQ.y*54.0-trshaderQ.x*1.6+trshaderN1*2.0-trshaderAdv*(.88+trshaderSpeed*.18));
 float trshaderPatchCue=trshaderPatchField.x*.10+trshaderPatchField.y*.12;
  float trshaderH=(trshaderMain*.20+trshaderLongWave*.11+trshaderSideTurb*.08+trshaderFast*.12)*trshaderLongWaveCtl+
    (trshaderCrossMain*.12+trshaderCrossFast*.075)*trshaderCrossStrength*trshaderTransWaveCtl+
    trshaderPatchCue;
  vec2 trshaderSlope=vec2(
    (cos(trshaderQ.x*29.0+trshaderN0*1.8-trshaderAdv*(.70+trshaderSpeed*.12))*.020+
    cos(trshaderQ.x*14.5+trshaderQ.y*2.4+trshaderN1*1.2-trshaderAdv*(.42+trshaderSpeed*.10))*.008+
    cos(trshaderQ.x*78.0+trshaderN0*2.5-trshaderAdv*(1.34+trshaderSpeed*.28))*.006)*trshaderLongWaveCtl+
    cos(trshaderQ.y*24.0+trshaderQ.x*2.3+trshaderN0*1.4-trshaderAdv*(.50+trshaderSpeed*.09))*.004*trshaderCrossStrength*trshaderTransWaveCtl,
    (cos(trshaderQ.x*18.0+trshaderQ.y*1.15+trshaderN1*1.7-trshaderAdv*.48)*.005+
    cos(trshaderQ.x*14.5+trshaderQ.y*2.4+trshaderN1*1.2-trshaderAdv*(.42+trshaderSpeed*.10))*.006)*trshaderLongWaveCtl+
    (cos(trshaderQ.y*24.0+trshaderQ.x*2.3+trshaderN0*1.4-trshaderAdv*(.50+trshaderSpeed*.09))*.017+
     cos(trshaderQ.y*54.0-trshaderQ.x*1.6+trshaderN1*2.0-trshaderAdv*(.88+trshaderSpeed*.18))*.008)*trshaderCrossStrength*trshaderTransWaveCtl);
 float trshaderStrength=clamp(TR456_WATER_FLOW_STRENGTH*TR456_WATER_FLOW_WAVE_STRENGTH,0.0,3.0);
 return vec3(trshaderSlope*(44.0*trshaderStrength),trshaderH*trshaderStrength);
}

vec3 trshaderFlowMicroChopField(vec2 trshaderP, float trshaderTravel, float trshaderSpeed, vec4 trshaderPatchField){
 float trshaderDetailPower=clamp(TR456_WATER_FLOW_DETAIL*TR456_WATER_FLOW_BREAKUP,0.0,2.0);
  if(trshaderDetailPower<=.001) return vec3(0.0);
  float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float trshaderLongWaveCtl=clamp(TR456_WATER_FLOW_LONGITUDINAL_WAVE,0.0,2.4);
  float trshaderTransWaveCtl=clamp(TR456_WATER_FLOW_TRANSVERSE_WAVE,0.0,2.4);
  trshaderP+=vec2(trshaderPatchField.z-.5,trshaderPatchField.w-.5)*(.016+.030*trshaderPatchField.x);
 float trshaderN0=trshaderValueNoise(trshaderP*vec2(18.0,7.5)+vec2(-trshaderTravel*.35,trshaderTravel*.040));
 float trshaderN1=trshaderValueNoise(trshaderP*vec2(9.0,16.0)+vec2(-trshaderTravel*.22,-trshaderTravel*.025));
 float trshaderPhaseA=trshaderP.x*76.0+trshaderP.y*3.2+trshaderN0*4.0-trshaderTravel*(7.5+trshaderSpeed*1.6);
 float trshaderPhaseB=trshaderP.x*47.0-trshaderP.y*2.4+trshaderN1*3.2-trshaderTravel*(5.4+trshaderSpeed*1.1);
 float trshaderPhaseC=trshaderP.y*68.0+trshaderP.x*2.8+trshaderN0*2.4-trshaderTravel*(4.4+trshaderSpeed*.95);
 float trshaderGate=smoothstep(.38,.88,trshaderValueNoise(trshaderP*vec2(3.6,5.8)+vec2(-trshaderTravel*.15,trshaderTravel*.030)))*
   mix(.72,1.18,trshaderPatchField.w);
 float trshaderAmp=trshaderDetailPower*trshaderGate;
  vec2 trshaderSlope=vec2((cos(trshaderPhaseA)*.010+cos(trshaderPhaseB)*.006)*trshaderLongWaveCtl+
    cos(trshaderPhaseC)*.003*trshaderCrossStrength*trshaderTransWaveCtl,
    ((trshaderN0-trshaderN1)*.006+cos(trshaderPhaseA*.63+trshaderPhaseB*.18)*.003)*trshaderLongWaveCtl+
    cos(trshaderPhaseC)*.011*trshaderCrossStrength*trshaderTransWaveCtl)*trshaderAmp;
  float trshaderChop=trshaderSat((abs(sin(trshaderPhaseA))*.42+abs(sin(trshaderPhaseB))*.24+
    abs(sin(trshaderPhaseC))*.20*trshaderCrossStrength*trshaderTransWaveCtl+trshaderPatchField.y*.18)*trshaderAmp);
 return vec3(trshaderSlope,trshaderChop);
}

vec4 trshaderFlowRefractiveStreakField(vec2 trshaderP, float trshaderTravel, float trshaderSpeed, vec4 trshaderPatchField){
  float trshaderDetailPower=clamp(TR456_WATER_FLOW_DETAIL,0.0,1.8)*TR_TOGGLE_FLOW_WARP;
  if(trshaderDetailPower<=.001) return vec4(0.0);
 float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 trshaderP+=vec2(trshaderPatchField.z-.5,trshaderPatchField.w-.5)*(.014+.026*trshaderPatchField.x);
 float trshaderN0=trshaderValueNoise(trshaderP*vec2(3.0,6.4)+vec2(-trshaderTravel*.12,trshaderTravel*.025));
 float trshaderN1=trshaderValueNoise(trshaderP*vec2(7.2,2.1)+vec2(-trshaderTravel*.25,trshaderTravel*.018));
 float trshaderFine=trshaderLineMask(trshaderP.x*8.8+trshaderN0*.50-trshaderTravel*(1.18+trshaderSpeed*.22),12.0);
 float trshaderBroad=trshaderLineMask(trshaderP.x*3.6+trshaderN1*.42-trshaderTravel*(.70+trshaderSpeed*.15),4.2);
 float trshaderCrossMask=trshaderLineMask(trshaderP.y*5.8+trshaderP.x*.42+trshaderN0*.32-trshaderTravel*(.42+trshaderSpeed*.12),6.8);
 float trshaderGate=smoothstep(.34,.88,trshaderN1);
 float trshaderMask=trshaderSat((trshaderFine*.55+trshaderBroad*.32+trshaderCrossMask*.20*trshaderCrossStrength+
   trshaderPatchField.y*.16)*trshaderGate*trshaderDetailPower);
 vec2 trshaderSlope=vec2(trshaderMask*(.012+.006*trshaderN0)+trshaderCrossMask*.002*trshaderCrossStrength,
   ((trshaderN0-trshaderN1)*.006+trshaderCrossMask*.010*trshaderCrossStrength)*trshaderMask);
  return vec4(trshaderSlope,trshaderMask,(trshaderBroad+trshaderCrossMask*.34*trshaderCrossStrength)*trshaderGate);
}

vec4 trshaderFlowVolumeWaveField(vec2 trshaderP, float trshaderTravel, float trshaderSpeed, vec4 trshaderPatchField){
  float trshaderStrength=clamp(TR456_WATER_FLOW_VOLUME_WAVE,0.0,2.5);
  if(trshaderStrength<=.001) return vec4(0.0);
  float trshaderScale=max(TR456_WATER_FLOW_VOLUME_WAVE_SCALE,.25);
  vec2 trshaderQ=trshaderP/trshaderScale;
  float trshaderAdv=trshaderTravel*(.72+trshaderSpeed*.16);
  float trshaderN0=trshaderFbmNoise(trshaderQ*vec2(1.15,2.10)+vec2(-trshaderAdv*.045,trshaderAdv*.018));
  float trshaderN1=trshaderFbmNoise(trshaderQ*vec2(2.40,1.00)+vec2(-trshaderAdv*.072,-trshaderAdv*.011));
  float trshaderLane=sin(trshaderQ.y*2.10+trshaderN0*1.20+trshaderAdv*.18)*.20+
    sin(trshaderQ.y*4.60-trshaderN1*.80-trshaderAdv*.11)*.08;
  float trshaderPhase=trshaderQ.x*3.25+trshaderLane+trshaderN0*.65-trshaderAdv*.58;
  float trshaderPhase2=trshaderQ.x*6.70+trshaderQ.y*.75+trshaderN1*.75-trshaderAdv*1.05;
  float trshaderBroad=sin(trshaderPhase)*.5+.5;
  float trshaderShoulder=sin(trshaderPhase2)*.5+.5;
  float trshaderSwell=smoothstep(.22,.92,trshaderBroad)*(.72+.28*smoothstep(.35,.90,trshaderN1));
  float trshaderCrest=pow(trshaderSwell,2.2)*(.72+.28*trshaderShoulder);
  float trshaderTrough=pow(1.0-trshaderSwell,1.55)*(.55+.45*trshaderN0);
  vec2 trshaderSlope=vec2(
    (cos(trshaderPhase)*.050+cos(trshaderPhase2)*.022)*(.75+.25*trshaderN1),
    sin(trshaderPhase+trshaderQ.y*2.80)*.012+cos(trshaderQ.y*4.00+trshaderN0)*.009);
  float trshaderVolume=trshaderSat((trshaderCrest*.70+(trshaderBroad-.5)*.18+trshaderPatchField.x*.08)*trshaderStrength);
  float trshaderShadow=trshaderSat(trshaderTrough*.45*trshaderStrength);
  return vec4(trshaderSlope*trshaderStrength,trshaderVolume,trshaderShadow);
}

vec4 trshaderSmoothFlowDeformationField(vec3 trshaderW, vec2 trshaderFlowDir, vec2 trshaderFlowSide,
                                float trshaderTravel, float trshaderSpeed, float trshaderCalmMix){
  float trshaderStrength=clamp(TR456_WATER_FLOW_ORIGINAL_DEFORMATION,0.0,1.4)*
    clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,2.2);
  if(trshaderStrength<=.001) return vec4(0.0);
  vec2 trshaderP=vec2(dot(trshaderW.xz,trshaderFlowDir),dot(trshaderW.xz,trshaderFlowSide))*.00058;
  float trshaderAdv=trshaderTravel*(.26+trshaderSpeed*.060);
  float trshaderN0=trshaderFbmNoise(trshaderP*vec2(1.08,1.62)+vec2(-trshaderAdv*.055,trshaderAdv*.018));
  float trshaderN1=trshaderFbmNoise(trshaderP*vec2(2.25,.92)+vec2(-trshaderAdv*.086,-trshaderAdv*.012));
  float trshaderSideDrift=sin(trshaderP.y*2.10+trshaderN0*1.55+trshaderAdv*.24)*.24+
    sin(trshaderP.y*4.20-trshaderN1*1.20-trshaderAdv*.16)*.10;
  float trshaderPhaseA=trshaderP.x*5.10+trshaderSideDrift+trshaderN0*.92-trshaderAdv*.78;
  float trshaderPhaseB=trshaderP.x*2.55+trshaderP.y*.74+trshaderN1*.72-trshaderAdv*.44;
  float trshaderPhaseC=trshaderP.y*3.65-trshaderP.x*.42+trshaderN0*.66+trshaderAdv*.30;
  float trshaderPhaseD=trshaderP.x*8.40-trshaderP.y*.32+trshaderN1*.48-trshaderAdv*1.16;
  float trshaderWaveA=sin(trshaderPhaseA);
  float trshaderWaveB=sin(trshaderPhaseB);
  float trshaderWaveC=sin(trshaderPhaseC);
  float trshaderWaveD=sin(trshaderPhaseD);
  float trshaderCalmDamp=mix(1.0,.62,clamp(trshaderCalmMix,0.0,1.0));
  vec2 trshaderSlope=vec2(
    cos(trshaderPhaseA)*.052+cos(trshaderPhaseB)*.035+cos(trshaderPhaseD)*.015,
    cos(trshaderPhaseC)*.043+cos(trshaderPhaseB)*.018-cos(trshaderPhaseD)*.010)*
    trshaderStrength*trshaderCalmDamp;
  float trshaderSwell=trshaderSat((trshaderWaveA*.48+trshaderWaveB*.32+trshaderWaveC*.20)*.5+.5);
  float trshaderCrest=pow(smoothstep(.28,.92,trshaderSwell),1.35)*
    (.72+.28*smoothstep(.30,.88,trshaderN0));
  float trshaderTrough=pow(trshaderSat(1.0-trshaderSwell),1.45)*(.64+.36*trshaderN1);
  float trshaderBody=trshaderSat((trshaderCrest*.54+(trshaderWaveD*.5+.5)*.16+trshaderN0*.10)*trshaderStrength*trshaderCalmDamp);
  float trshaderShadow=trshaderSat(trshaderTrough*.36*trshaderStrength*trshaderCalmDamp);
  return vec4(trshaderSlope,trshaderBody,trshaderShadow);
}

vec4 trshaderStandingLifeField(vec3 trshaderW, float trshaderT,
                               vec2 trshaderPrimaryDir, vec2 trshaderSideDir){
 float trshaderLife=clamp(TR456_WATER_STANDING_LIFE,0.0,2.4);
 if(trshaderLife<=.001) return vec4(0.0);
 float trshaderMicro=clamp(TR456_WATER_STANDING_MICRO_CHOP,0.0,2.0);
 float trshaderTensionStrength=clamp(TR456_WATER_STANDING_TENSION,0.0,2.0);
 float trshaderDrift=trshaderT*clamp(TR456_WATER_STANDING_DRIFT_SPEED,0.05,3.5);
 float trshaderSpeed=.38+trshaderLife*.22;
 vec2 trshaderP=vec2(dot(trshaderW.xz,trshaderPrimaryDir),
   dot(trshaderW.xz,trshaderSideDir))*.00072;
 vec4 trshaderPatch=trshaderFlowPatchField(trshaderP*1.06+vec2(.19,.43),
   trshaderDrift*.76,trshaderSpeed);
 vec3 trshaderFlow=trshaderSyntheticFlowField(trshaderP*.86+vec2(.07,.13),
   trshaderDrift*.48,trshaderSpeed,trshaderPatch);
 vec3 trshaderChop=trshaderFlowMicroChopField(trshaderP*1.10+vec2(.31,.11),
   trshaderDrift*.56,trshaderSpeed,trshaderPatch);
 vec4 trshaderStreak=trshaderFlowRefractiveStreakField(
   trshaderP*.92+vec2(.17,.09),trshaderDrift*.50,trshaderSpeed,trshaderPatch);
 vec4 trshaderVolume=trshaderFlowVolumeWaveField(trshaderP*.74+vec2(.23,.31),
   trshaderDrift*.36,trshaderSpeed,trshaderPatch);
 float trshaderLineA=trshaderLineMask(trshaderP.x*4.90+trshaderPatch.z*.36-
   trshaderDrift*.155,5.6);
 float trshaderLineB=trshaderLineMask(trshaderP.x*10.8+trshaderP.y*.62+
   trshaderPatch.x*.42-trshaderDrift*.32,15.0);
 float trshaderGate=smoothstep(.22,.86,
   trshaderValueNoise(trshaderP*vec2(2.1,4.9)+
     vec2(-trshaderDrift*.060,trshaderDrift*.025)));
 float trshaderTensionFilm=trshaderSat((trshaderLineA*.50+trshaderLineB*.38+
   trshaderPatch.y*.24)*trshaderGate)*trshaderTensionStrength;
 vec2 trshaderFlowSlope=trshaderPrimaryDir*trshaderFlow.x+
   trshaderSideDir*trshaderFlow.y;
 vec2 trshaderChopSlope=trshaderPrimaryDir*trshaderChop.x+
   trshaderSideDir*trshaderChop.y;
 vec2 trshaderStreakSlope=trshaderPrimaryDir*trshaderStreak.x+
   trshaderSideDir*trshaderStreak.y;
 vec2 trshaderVolumeSlope=trshaderPrimaryDir*trshaderVolume.x+
   trshaderSideDir*trshaderVolume.y;
 vec2 trshaderSlope=(trshaderFlowSlope*.30+trshaderChopSlope*.80*trshaderMicro+
   trshaderStreakSlope*.58+trshaderVolumeSlope*.44+
   trshaderPrimaryDir*(trshaderPatch.w-.5)*.052+
   trshaderSideDir*((trshaderPatch.x-trshaderPatch.z)*.040+
     (trshaderLineB-.5)*trshaderTensionFilm*.050))*trshaderLife;
 float trshaderEnergy=trshaderSat(abs(trshaderFlow.z)*.32+
   trshaderChop.z*.42*trshaderMicro+trshaderStreak.z*.30+
   trshaderVolume.z*.28+trshaderPatch.x*.12+trshaderPatch.y*.24+
   trshaderTensionFilm*.38);
 return vec4(trshaderSlope,trshaderEnergy*trshaderLife,
   trshaderTensionFilm*trshaderLife);
}

vec3 trshaderContactField(vec3 trshaderW, float trshaderT){
 vec2 trshaderSlope=vec2(0.0);
 float trshaderCrest=0.0;
 float trshaderStandingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 float trshaderSlopeStrength=mix(1.26,3.20,trshaderStandingProfile);
 float trshaderCrestStrength=mix(.50,1.55,trshaderStandingProfile);
 float trshaderSplashStrength=clamp(TR456_WATER_LARA_SPLASH_STRENGTH,0.0,3.0);
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_MAX_ACTIVE;trshaderI++){
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderContactOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderContactOn<=.001) continue;
  float trshaderRadius=trshaderContactRadius(trshaderC);
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderDist=length(trshaderD)+.001;
  vec2 trshaderDir=trshaderD/trshaderDist;
  vec4 trshaderM=uContactMotion[trshaderI];
  vec2 trshaderMv=trshaderM.xz;
  float trshaderMotionSpeed=length(trshaderMv);
  vec2 trshaderMotionDir=trshaderMotionSpeed>.001 ? trshaderMv/trshaderMotionSpeed : trshaderDir;
  float trshaderMotionEnergy=trshaderSat(trshaderMotionSpeed*.11);
  float trshaderDirectional=mix(1.0,.76+.38*smoothstep(-.35,.90,dot(trshaderDir,trshaderMotionDir)),
    trshaderSat(TR456_WATER_CONTACT_WAKE_DIRECTIONAL)*trshaderMotionEnergy);
  float trshaderVertical=1.0-smoothstep(120.0,620.0,abs(trshaderW.y-trshaderC.y));
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderBreak=trshaderValueNoise(vec2(dot(trshaderD,vec2(.0021,.0008))+float(trshaderI)*1.7,
    dot(trshaderD,vec2(-.0009,.0024))+trshaderT*.042));
  float trshaderDecay=clamp(TR456_WATER_CONTACT_RIPPLE_DECAY,.55,2.40);
  float trshaderRange=trshaderRadius*mix(1.26,.92,trshaderSat(trshaderDecay-1.0));
  float trshaderAgeNorm=trshaderSat(trshaderAge/240.0);
  float trshaderFront=trshaderRadius*(.34+.22*trshaderBreak)+
    trshaderAge*(12.8+2.6*trshaderMotionEnergy)+trshaderT*38.0;
  float trshaderFrontWidth=58.0+trshaderRadius*.075+trshaderAge*1.55;
  float trshaderFrontX=(trshaderDist-trshaderFront)/max(trshaderFrontWidth,1.0);
  float trshaderTroughX=(trshaderDist-(trshaderFront-trshaderFrontWidth*1.35))/
    max(trshaderFrontWidth*1.75,1.0);
  float trshaderPacketCrestBase=trshaderSat(1.0-abs(trshaderFrontX));
  float trshaderPacketTroughBase=trshaderSat(1.0-abs(trshaderTroughX));
  float trshaderPacketCrest=trshaderPacketCrestBase*trshaderPacketCrestBase*
    (3.0-2.0*trshaderPacketCrestBase);
  float trshaderPacketTrough=trshaderPacketTroughBase*trshaderPacketTroughBase*
    (3.0-2.0*trshaderPacketTroughBase);
  float trshaderAgeFade=1.0-smoothstep(.70,1.0,trshaderAgeNorm);
  float trshaderTravelRing=(trshaderPacketCrest*.82-trshaderPacketTrough*.34)*trshaderAgeFade;
  float trshaderCrestGrad=-sign(trshaderFrontX)*6.0*trshaderPacketCrestBase*
    (1.0-trshaderPacketCrestBase)/max(trshaderFrontWidth,1.0);
  float trshaderTroughGrad=-sign(trshaderTroughX)*6.0*trshaderPacketTroughBase*
    (1.0-trshaderPacketTroughBase)/max(trshaderFrontWidth*1.75,1.0);
  float trshaderTravelSlope=(trshaderCrestGrad*.82-trshaderTroughGrad*.34)*
    trshaderAgeFade;
  float trshaderLongFade=trshaderSat(1.0-trshaderDist/(1500.0+trshaderRadius*4.8));
  trshaderLongFade=trshaderLongFade*trshaderLongFade*(3.0-2.0*trshaderLongFade)*
    (1.0-smoothstep(trshaderRadius*9.8,trshaderRadius*15.0,trshaderDist));
  float trshaderFalloff=trshaderContactOn*trshaderVertical*
    pow(1.0-smoothstep(trshaderRadius*.10,trshaderRadius*2.85,trshaderDist),trshaderDecay)*
    exp(-trshaderDist/max(trshaderRange,1.0))*trshaderDirectional*(.84+.16*trshaderBreak);
  float trshaderPhase=trshaderDist*(.044+.006*trshaderBreak)-trshaderT*(3.62+trshaderMotionEnergy*.48)+
    trshaderAge*.075+float(trshaderI)*.41+trshaderBreak*.70;
  float trshaderRing=sin(trshaderPhase);
  float trshaderRingSharp=trshaderSat(trshaderRing*.5+.5);
  trshaderRingSharp*=trshaderRingSharp;
  trshaderRingSharp*=trshaderRingSharp;
  vec2 trshaderWakeBias=trshaderMotionDir*trshaderMotionEnergy*.18*TR456_WATER_CONTACT_WAKE_DIRECTIONAL;
  trshaderSlope+=(trshaderDir+trshaderWakeBias)*cos(trshaderPhase)*trshaderFalloff*trshaderSlopeStrength*
    trshaderSplashStrength*.34;
  trshaderSlope+=trshaderDir*trshaderTravelSlope*trshaderLongFade*trshaderVertical*
    trshaderDirectional*trshaderSlopeStrength*trshaderSplashStrength*1.65;
  trshaderCrest+=trshaderRingSharp*trshaderFalloff*trshaderCrestStrength*trshaderSplashStrength*
    (.22+.06*trshaderBreak);
  trshaderCrest+=max(trshaderTravelRing,0.0)*trshaderLongFade*trshaderVertical*
    trshaderCrestStrength*trshaderSplashStrength*(.62+.30*trshaderMotionEnergy);
 }
 return vec3(trshaderSlope,trshaderCrest);
}

vec3 trshaderRainRippleField(vec2 trshaderP, float trshaderT){
 float trshaderStrength=clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.5)*
   clamp(TR456_WATER_SURFACE_RELIEF*.55+TR456_WATER_MICRO_RIPPLE*.35,0.0,2.0);
 vec2 trshaderSlope=vec2(0.0);
 float trshaderHeight=0.0;
 float trshaderCell=650.0;
 vec2 trshaderBase=floor(trshaderP/trshaderCell);
 for(int trshaderIx=-1;trshaderIx<=1;trshaderIx++){
  for(int trshaderIy=-1;trshaderIy<=1;trshaderIy++){
   vec2 trshaderId=trshaderBase+vec2(float(trshaderIx),float(trshaderIy));
   float trshaderRnd=trshaderHash12(trshaderId+vec2(7.1,3.7));
   vec2 trshaderCenter=(trshaderId+vec2(trshaderHash12(trshaderId+vec2(1.3,5.7)),
     trshaderHash12(trshaderId+vec2(8.2,2.4))))*trshaderCell;
   vec2 trshaderDelta=trshaderP-trshaderCenter;
   float trshaderD=length(trshaderDelta)+.001;
   vec2 trshaderDir=trshaderDelta/trshaderD;
   float trshaderAge=fract(trshaderT*.18+trshaderRnd);
   float trshaderDensity=smoothstep(.22,.96,trshaderRnd);
   float trshaderFade=smoothstep(.035,.14,trshaderAge)*(1.0-smoothstep(.74,1.0,trshaderAge))*trshaderDensity;
   float trshaderFront=mix(20.0,540.0,trshaderAge);
   float trshaderWidth=mix(22.0,68.0,trshaderAge);
   float trshaderCrestX=(trshaderD-trshaderFront)/trshaderWidth;
   float trshaderTroughX=(trshaderD-(trshaderFront-trshaderWidth*.82))/(trshaderWidth*1.55);
   float trshaderCrestRing=exp(-trshaderCrestX*trshaderCrestX);
   float trshaderTrough=exp(-trshaderTroughX*trshaderTroughX);
   float trshaderShell=1.0-smoothstep(600.0,900.0,trshaderD);
   float trshaderRing=(trshaderCrestRing*.58-trshaderTrough*.22)*trshaderShell;
   float trshaderDCrest=(-2.0*trshaderCrestX/trshaderWidth)*trshaderCrestRing;
   float trshaderDTrough=(-2.0*trshaderTroughX/(trshaderWidth*1.55))*trshaderTrough;
   float trshaderDRing=(trshaderDCrest*.58-trshaderDTrough*.22)*trshaderShell;
   trshaderHeight+=trshaderRing*trshaderFade;
   trshaderSlope+=trshaderDir*trshaderDRing*trshaderFade;
  }
 }
 return vec3(clamp(trshaderSlope*trshaderStrength*1.18,vec2(-.24),vec2(.24)),
   clamp(trshaderHeight*trshaderStrength,-1.0,1.0));
}

vec4 trshaderContactWakeField(vec3 trshaderW, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderSlope=vec2(0.0);
 float trshaderCrest=0.0;
 float trshaderFoam=0.0;
 vec2 trshaderFlowDir=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 float trshaderWakeWidth=clamp(TR456_WATER_WAKE_WIDTH,0.10,2.4);
 float trshaderWakeLength=clamp(TR456_WATER_WAKE_LENGTH,0.12,3.4);
 float trshaderWakeStrength=clamp(TR456_WATER_WAKE_STRENGTH,0.0,3.0)*
   clamp(uTrWaterSyntheticProfile.z,0.0,2.0)*
   clamp(TR456_WATER_LARA_SPLASH_STRENGTH,0.0,3.0);
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_MAX_ACTIVE;trshaderI++){
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderContactOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderContactOn<=.001) continue;
  vec4 trshaderM=uContactMotion[trshaderI];
  vec2 trshaderMv=trshaderM.xz;
  float trshaderSpeed=length(trshaderMv);
  vec2 trshaderMotionDir=trshaderSpeed>.001 ? trshaderMv/trshaderSpeed : trshaderFlowDir;
  float trshaderMotionEnergy=trshaderSat(trshaderSpeed*.13);
  vec2 trshaderWakeDir=normalize(mix(trshaderFlowDir,trshaderMotionDir,
    trshaderSat(TR456_WATER_CONTACT_WAKE_DIRECTIONAL)*trshaderMotionEnergy)+vec2(.0001,.0003));
  vec2 trshaderWakeSide=vec2(-trshaderWakeDir.y,trshaderWakeDir.x);
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderRadius=trshaderContactRadius(trshaderC);
  float trshaderVertical=1.0-smoothstep(120.0,680.0,abs(trshaderW.y-trshaderC.y));
  float trshaderAlong=dot(trshaderD,trshaderWakeDir)/max(trshaderRadius,1.0);
  float trshaderSide=dot(trshaderD,trshaderWakeSide)/max(trshaderRadius,1.0);
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderDecay=clamp(TR456_WATER_CONTACT_RIPPLE_DECAY,.55,2.40);
  float trshaderLengthWindow=1.0-smoothstep(.72*trshaderWakeLength,2.35*trshaderWakeLength,abs(trshaderAlong));
  float trshaderTrailWindow=mix(1.0,smoothstep(.38,-1.70,trshaderAlong),
    trshaderSat(TR456_WATER_CONTACT_WAKE_DIRECTIONAL)*trshaderMotionEnergy);
  float trshaderSideWidth=.135+.150*trshaderWakeWidth;
  float trshaderSideFalloff=trshaderSat(1.0-abs(trshaderSide)/max(trshaderSideWidth*1.65,.001));
  trshaderSideFalloff=trshaderSideFalloff*trshaderSideFalloff*(3.0-2.0*trshaderSideFalloff);
  float trshaderBroken=trshaderValueNoise(vec2(trshaderAlong*4.8+trshaderT*.045,trshaderSide*9.5+trshaderAge*.010));
  float trshaderPhase=trshaderAlong*(10.0+3.8*trshaderWakeLength)-trshaderT*(.76+trshaderMotionEnergy*.62)+
    trshaderAge*.030+trshaderBroken*.72;
  float trshaderAlongFade=trshaderSat(1.0-abs(trshaderAlong)*(.16+.18*trshaderDecay));
  trshaderAlongFade=trshaderAlongFade*trshaderAlongFade*(3.0-2.0*trshaderAlongFade);
  float trshaderStreak=trshaderContactOn*trshaderVertical*trshaderLengthWindow*trshaderTrailWindow*
    trshaderSideFalloff*(.44+.56*trshaderMotionEnergy)*(.62+.38*trshaderBroken)*
    trshaderAlongFade;
  float trshaderRidge=trshaderSat(sin(trshaderPhase)*.5+.5);
  trshaderSlope+=trshaderWakeDir*(sin(trshaderPhase)*trshaderStreak*.18+trshaderRidge*trshaderStreak*.060)+
    trshaderWakeSide*(-trshaderSide*trshaderStreak*(.16+.10*trshaderMotionEnergy));
  trshaderCrest+=trshaderStreak*trshaderRidge*(.18+.22*trshaderMotionEnergy);
  trshaderFoam+=trshaderStreak*(.16+.38*trshaderMotionEnergy)*TR456_WATER_CONTACT_EDGE;
 }
 return vec4(trshaderSlope*(.48*trshaderWakeStrength*TR456_WATER_WAKE_WAVE),
   trshaderCrest*trshaderWakeStrength*.52,trshaderFoam*trshaderWakeStrength*.82);
}

vec3 trshaderFlowContactDistortionField(vec3 trshaderW, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderSlope=vec2(0.0);
 float trshaderEnergy=0.0;
 float trshaderStrength=clamp(TR456_WATER_FLOW_CONTACT_DISTORTION,0.0,2.5)*
   clamp(TR456_WATER_LARA_SPLASH_STRENGTH,0.0,3.0);
 if(trshaderStrength<=.001) return vec3(0.0);
 vec2 trshaderFlowDir=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_MAX_ACTIVE;trshaderI++){
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderContactOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderContactOn<=.001) continue;
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderDist=length(trshaderD)+.001;
  vec2 trshaderDir=trshaderD/trshaderDist;
  float trshaderRadius=max(trshaderContactRadius(trshaderC),90.0);
  vec4 trshaderM=uContactMotion[trshaderI];
  vec2 trshaderMv=trshaderM.xz;
  float trshaderSpeed=length(trshaderMv);
  float trshaderMotionEnergy=trshaderSat(trshaderSpeed*.11);
  vec2 trshaderMotionDir=trshaderSpeed>.001 ? trshaderMv/trshaderSpeed : trshaderFlowDir;
  vec2 trshaderPushDir=normalize(mix(trshaderFlowDir,trshaderMotionDir,
    trshaderMotionEnergy*.62)+vec2(.0001,.0003));
  vec2 trshaderPushSide=vec2(-trshaderPushDir.y,trshaderPushDir.x);
  float trshaderAlong=dot(trshaderD,trshaderPushDir)/trshaderRadius;
  float trshaderSide=dot(trshaderD,trshaderPushSide)/trshaderRadius;
  float trshaderVertical=1.0-smoothstep(120.0,620.0,abs(trshaderW.y-trshaderC.y));
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderAgeFade=1.0-smoothstep(160.0,300.0,trshaderAge);
  float trshaderBody=1.0-smoothstep(.10,1.55,length(vec2(trshaderAlong*.72,trshaderSide*1.12)));
  float trshaderTrail=(1.0-smoothstep(-.10,2.20,trshaderAlong))*
    (1.0-smoothstep(2.8,5.0,abs(trshaderAlong)));
  float trshaderSideFalloff=1.0-smoothstep(.16,.92,abs(trshaderSide));
  float trshaderBreak=trshaderValueNoise(vec2(trshaderAlong*2.2+float(trshaderI)*1.3,
    trshaderSide*4.4+trshaderT*.035));
  float trshaderMask=trshaderContactOn*trshaderVertical*trshaderAgeFade*
    max(trshaderBody*.72,trshaderTrail*trshaderSideFalloff*.48)*
    (.82+.18*trshaderBreak);
  vec2 trshaderLocalSlope=(trshaderPushDir*(.12+.16*trshaderMotionEnergy)+
    trshaderDir*.055-trshaderPushSide*trshaderSide*.16)*trshaderMask;
  trshaderSlope+=trshaderLocalSlope;
  trshaderEnergy=max(trshaderEnergy,trshaderMask*(.58+.42*trshaderMotionEnergy));
 }
 return vec3(clamp(trshaderSlope*trshaderStrength,vec2(-.46),vec2(.46)),
   trshaderSat(trshaderEnergy*trshaderStrength));
}

vec3 trshaderWaterfallImpactWaveField(vec3 trshaderW, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderSlope=vec2(0.0);
 float trshaderCrest=0.0;
 vec2 trshaderFlowDir=normalize(trshaderPrimaryDir+vec2(.0001,.0003));
 vec2 trshaderSideDir=vec2(-trshaderFlowDir.y,trshaderFlowDir.x);
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_MAX_ACTIVE;trshaderI++){
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderContactOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderContactOn<=.001) continue;
  vec4 trshaderM=uContactMotion[trshaderI];
  float trshaderRadius=trshaderContactRadius(trshaderC);
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderDist=length(trshaderD)+.001;
  vec2 trshaderDir=trshaderD/trshaderDist;
  float trshaderVertical=1.0-smoothstep(140.0,860.0,abs(trshaderW.y-trshaderC.y));
  float trshaderStable=1.0-smoothstep(8.0,120.0,trshaderM.w);
  float trshaderSmallSource=(1.0-smoothstep(180.0,420.0,trshaderRadius))*
    smoothstep(72.0,104.0,trshaderRadius);
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderReach=1780.0+trshaderRadius*5.4;
  float trshaderFade=exp(-trshaderDist/(760.0+trshaderRadius*3.8))*
    (1.0-smoothstep(trshaderReach,trshaderReach*1.34,trshaderDist));
  float trshaderSource=trshaderContactOn*trshaderVertical*trshaderSmallSource*(.62+.38*trshaderStable);
  float trshaderBroken=trshaderValueNoise(vec2(dot(trshaderD,trshaderFlowDir)*.0022+float(trshaderI)*1.7,
    dot(trshaderD,trshaderSideDir)*.0028+trshaderT*.050));
  float trshaderPhase=trshaderDist*.0255-trshaderT*2.32+trshaderAge*.032+trshaderBroken*.72+float(trshaderI)*.43;
  float trshaderPhase2=trshaderDist*.0165-trshaderT*1.55+trshaderAge*.020+trshaderBroken*.50;
  float trshaderRing=sin(trshaderPhase)*.68+sin(trshaderPhase2)*.32;
  float trshaderRidge=pow(trshaderSat(trshaderRing*.5+.5),2.6);
  float trshaderDirectional=.78+.22*smoothstep(-.20,.82,dot(trshaderDir,trshaderFlowDir));
  float trshaderAmp=trshaderSource*trshaderFade*trshaderDirectional*(.72+.28*trshaderBroken);
  trshaderSlope+=trshaderDir*(cos(trshaderPhase)*.076+cos(trshaderPhase2)*.034)*trshaderAmp+
    trshaderSideDir*sin(trshaderPhase2+trshaderBroken)*trshaderAmp*.010;
  trshaderCrest+=trshaderRidge*trshaderAmp;
 }
 return vec3(clamp(trshaderSlope,vec2(-.18),vec2(.18)),trshaderSat(trshaderCrest));
}

float trshaderShorelineEdgeField(vec2 trshaderScreen, float trshaderT, vec2 trshaderPrimaryDir){
 vec2 trshaderInv=max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 trshaderFlowScreen=normalize(vec2(trshaderPrimaryDir.x,-trshaderPrimaryDir.y)+vec2(.0001,.0003));
 vec2 trshaderSideScreen=vec2(-trshaderFlowScreen.y,trshaderFlowScreen.x);
 vec2 trshaderA=trshaderFlowScreen*trshaderInv*vec2(9.0,9.0);
 vec2 trshaderB=trshaderSideScreen*trshaderInv*vec2(7.0,7.0);
 vec3 trshaderC0=texture(uTrWaterScene,clamp(trshaderScreen,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderC1=texture(uTrWaterScene,clamp(trshaderScreen+trshaderA,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderC2=texture(uTrWaterScene,clamp(trshaderScreen-trshaderA,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderC3=texture(uTrWaterScene,clamp(trshaderScreen+trshaderB,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderC4=texture(uTrWaterScene,clamp(trshaderScreen-trshaderB,vec2(.001),vec2(.999))).rgb;
 float trshaderL0=trshaderLuma(trshaderC0);
 float trshaderContrast=max(abs(trshaderLuma(trshaderC1)-trshaderLuma(trshaderC2)),abs(trshaderLuma(trshaderC3)-trshaderLuma(trshaderC4)));
 trshaderContrast=max(trshaderContrast,max(abs(trshaderLuma(trshaderC1)-trshaderL0),abs(trshaderLuma(trshaderC3)-trshaderL0))*.72);
 float trshaderEdge=smoothstep(.030,.155,trshaderContrast);
 float trshaderGrain=trshaderValueNoise(trshaderScreen*vec2(720.0,420.0)+vec2(trshaderT*.030,-trshaderT*.017));
 float trshaderPulse=sin(dot(trshaderScreen,trshaderFlowScreen)*900.0+trshaderT*.82+trshaderGrain*1.8)*.5+.5;
 return trshaderEdge*(.58+.42*trshaderGrain)*(.76+.24*trshaderPulse);
}

struct TrshaderSyntheticFrame {
 vec2 trshaderScreen;
 float trshaderTime;
 vec3 trshaderBaseField;
 vec3 trshaderContacts;
 vec3 trshaderFlowContacts;
 vec3 trshaderRainRipples;
 vec4 trshaderContactWake;
 vec3 trshaderWaterfallWaves;
 float trshaderShoreline;
 vec3 trshaderRelief;
 vec3 trshaderAlive;
 vec4 trshaderStandingLife;
 vec2 trshaderSlope;
 vec3 trshaderNormal;
 vec3 trshaderViewDir;
 float trshaderNdv;
 float trshaderFresnel;
};

TrshaderSyntheticFrame trshaderBuildSyntheticFrame(vec2 trshaderScreen, float trshaderT){
  TrshaderSyntheticFrame trshaderF;
  vec2 trshaderPrimaryDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
 trshaderF.trshaderScreen=trshaderScreen;
 trshaderF.trshaderTime=trshaderT;
 trshaderF.trshaderBaseField=trshaderBaseWaterField(vSynWorldPos.xz,trshaderT,trshaderPrimaryDir);
 float trshaderStandingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 float trshaderFlowProfile=1.0-trshaderStandingProfile;
 float trshaderFlowRippleGate=step(.5,TR456_WATER_FLOW_CONTACT_RIPPLES);
 float trshaderContactRippleProfile=max(trshaderStandingProfile,trshaderFlowProfile*trshaderFlowRippleGate);
 trshaderF.trshaderContacts=vec3(0.0);
 trshaderF.trshaderContactWake=vec4(0.0);
 if(trshaderContactRippleProfile>.001) {
  trshaderF.trshaderContacts=trshaderContactField(vSynWorldPos,trshaderT)*trshaderContactRippleProfile;
  trshaderF.trshaderContactWake=trshaderContactWakeField(vSynWorldPos,trshaderT,trshaderPrimaryDir)*
    trshaderContactRippleProfile;
 }
 trshaderF.trshaderFlowContacts=vec3(0.0);
 if(trshaderFlowProfile>.001) {
  trshaderF.trshaderFlowContacts=trshaderFlowContactDistortionField(vSynWorldPos,
    trshaderT,trshaderPrimaryDir)*trshaderFlowProfile;
 }
 trshaderF.trshaderRainRipples=vec3(0.0);
 if(trshaderStandingProfile>.001 && TR456_WATER_RAIN_RIPPLE>.001)
  trshaderF.trshaderRainRipples=trshaderRainRippleField(vSynWorldPos.xz,trshaderT)*
    trshaderStandingProfile;
 trshaderF.trshaderWaterfallWaves=vec3(0.0);
 if(trshaderStandingProfile>.001) {
  trshaderF.trshaderWaterfallWaves=trshaderWaterfallImpactWaveField(vSynWorldPos,trshaderT,trshaderPrimaryDir)*
    trshaderStandingProfile;
 }
 float trshaderShorelineProfile=clamp(uTrWaterSyntheticProfile.y,0.0,2.0);
 trshaderF.trshaderShoreline=trshaderShorelineProfile>.001 ?
   trshaderShorelineEdgeField(trshaderScreen,trshaderT,trshaderPrimaryDir)*
   trshaderShorelineProfile : 0.0;
 trshaderF.trshaderRelief=trshaderReliefField(vSynWorldPos.xz,vSynUv,trshaderT,trshaderPrimaryDir);
 trshaderF.trshaderAlive=trshaderSoftMotionField(vSynWorldPos.xz,trshaderT,trshaderPrimaryDir);
 vec2 trshaderSideDir=vec2(-trshaderPrimaryDir.y,trshaderPrimaryDir.x);
 trshaderF.trshaderStandingLife=vec4(0.0);
 if(trshaderStandingProfile>.001)
  trshaderF.trshaderStandingLife=trshaderStandingLifeField(vSynWorldPos,
    trshaderT,trshaderPrimaryDir,trshaderSideDir)*trshaderStandingProfile;
 float trshaderReliefSlopeStrength=mix(.52,.66,trshaderStandingProfile);
 trshaderF.trshaderSlope=trshaderF.trshaderBaseField.xy*.78+trshaderF.trshaderContacts.xy*1.65+trshaderF.trshaderRainRipples.xy*1.80+
   trshaderF.trshaderContactWake.xy*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderF.trshaderFlowContacts.xy*.42*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderF.trshaderWaterfallWaves.xy*1.42*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderF.trshaderRelief.xy*trshaderReliefSlopeStrength+
   trshaderF.trshaderAlive.xy*.48+trshaderF.trshaderStandingLife.xy*.72;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
 trshaderF.trshaderSlope+=trshaderSyntheticStandingBump(trshaderF.trshaderBaseField.xy,trshaderF.trshaderRelief.xy,trshaderF.trshaderAlive.xy,
   trshaderF.trshaderRainRipples.xy+trshaderF.trshaderContactWake.xy*.35*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderF.trshaderWaterfallWaves.xy*.22*TR_TOGGLE_CONTACT_RIPPLES)*trshaderStandingProfile;
 trshaderF.trshaderSlope+=trshaderSyntheticBumpSlope(
   trshaderTextureMicroBump(vSynWorldPos.xz,trshaderT,trshaderPrimaryDir,trshaderFlowProfile),
   TR456_WATER_BUMP_STRENGTH*TR456_WATER_SYNTHETIC_BUMP_STRENGTH*
   mix(.74,.44,trshaderFlowProfile),.34);
#endif
 trshaderF.trshaderNormal=normalize(vec3(-trshaderF.trshaderSlope.x,1.0,-trshaderF.trshaderSlope.y));
 trshaderF.trshaderViewDir=normalize(-vSynPos+vSynNormal*.001);
 trshaderF.trshaderNdv=trshaderSat(abs(dot(trshaderF.trshaderNormal,trshaderF.trshaderViewDir)));
 trshaderF.trshaderFresnel=pow(1.0-trshaderF.trshaderNdv,2.55);
 return trshaderF;
}

TrshaderSyntheticFrame trshaderStandingPoolReplacementFrame(TrshaderSyntheticFrame trshaderF){
 TrshaderSyntheticFrame trshaderP=trshaderF;
 trshaderP.trshaderRainRipples=trshaderRainRippleField(vSynWorldPos.xz,trshaderF.trshaderTime);
 float trshaderWakeLift=1.0/max(clamp(uTrWaterSyntheticProfile.z,.18,2.0),.18);
  trshaderP.trshaderContactWake=trshaderF.trshaderContactWake*clamp(trshaderWakeLift,.80,3.40);
 trshaderP.trshaderWaterfallWaves=trshaderF.trshaderWaterfallWaves;
  trshaderP.trshaderSlope=trshaderP.trshaderBaseField.xy*.78+trshaderP.trshaderContacts.xy*1.65+trshaderP.trshaderRainRipples.xy*1.80+
   trshaderP.trshaderContactWake.xy*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderP.trshaderWaterfallWaves.xy*1.42*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderP.trshaderRelief.xy*.66+trshaderP.trshaderAlive.xy*.48+
   trshaderP.trshaderStandingLife.xy*.72;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
 trshaderP.trshaderSlope+=trshaderSyntheticStandingBump(trshaderP.trshaderBaseField.xy,trshaderP.trshaderRelief.xy,trshaderP.trshaderAlive.xy,
   trshaderP.trshaderRainRipples.xy+trshaderP.trshaderContactWake.xy*.35*TR_TOGGLE_CONTACT_RIPPLES+
   trshaderP.trshaderWaterfallWaves.xy*.22*TR_TOGGLE_CONTACT_RIPPLES);
 trshaderP.trshaderSlope+=trshaderSyntheticBumpSlope(
   trshaderTextureMicroBump(vSynWorldPos.xz,trshaderF.trshaderTime,
     normalize(vSynFlowDir+vec2(.0001,.0003)),0.0),
   TR456_WATER_BUMP_STRENGTH*TR456_WATER_SYNTHETIC_BUMP_STRENGTH*.76,.34);
#endif
 trshaderP.trshaderNormal=normalize(vec3(-trshaderP.trshaderSlope.x,1.0,-trshaderP.trshaderSlope.y));
 trshaderP.trshaderNdv=trshaderSat(abs(dot(trshaderP.trshaderNormal,trshaderP.trshaderViewDir)));
 trshaderP.trshaderFresnel=pow(1.0-trshaderP.trshaderNdv,2.55);
 return trshaderP;
}

vec4 trshaderRenderCascadeFlow(TrshaderSyntheticFrame trshaderF, vec2 trshaderFlowDir, vec2 trshaderFlowSide,
                       vec2 trshaderFlowScreenDir, vec2 trshaderFlowScreenSide,
                       float trshaderFlowSpeed, float trshaderGameTravel,
                       float trshaderPoolBlend, vec4 trshaderJunction){
 vec2 trshaderTexFlow=length(uParams.xy)>.000001 ? normalize(uParams.xy) : vec2(0.0,-1.0);
 vec2 trshaderTexSide=vec2(-trshaderTexFlow.y,trshaderTexFlow.x);
 vec2 trshaderSourceFlowUv=vec2(dot(vSynUv,trshaderTexFlow),dot(vSynUv,trshaderTexSide));
 float trshaderWorldDown=-vSynWorldPos.y*.00105+dot(vSynWorldPos.xz,trshaderFlowDir)*.00010;
 float trshaderWorldSide=dot(vSynWorldPos.xz,trshaderFlowSide)*.00072+
   trshaderValueNoise(vSynWorldPos.xz*.0011)*.18;
 vec2 trshaderFallUv=vec2(trshaderWorldDown,trshaderWorldSide);
 float trshaderFallTime=-trshaderGameTravel*(1.55+trshaderFlowSpeed*.24);
 float trshaderSourceDrift=sin(trshaderSourceFlowUv.x*6.28318+trshaderSourceFlowUv.y*.37)*.055;
 float trshaderN0=trshaderValueNoise(trshaderFallUv*vec2(2.6,8.5)+vec2(-trshaderFallTime*.18+trshaderSourceDrift,trshaderFallTime*.035));
 float trshaderN1=trshaderValueNoise(trshaderFallUv*vec2(6.2,3.4)+vec2(-trshaderFallTime*.30+trshaderSourceDrift*.6,-trshaderFallTime*.018));
 float trshaderN2=trshaderValueNoise(trshaderFallUv*vec2(12.0,14.0)+vec2(-trshaderFallTime*.48,trshaderFallTime*.052));
 float trshaderSideVeil=smoothstep(.36,.86,
   trshaderValueNoise(vec2(trshaderFallUv.y*3.4+trshaderN0*.62,trshaderFallUv.x*.34-trshaderFallTime*.22)));
 float trshaderFineVeil=smoothstep(.52,.94,
   trshaderValueNoise(vec2(trshaderFallUv.y*9.0+trshaderN1*.82,trshaderFallUv.x*.56-trshaderFallTime*.38)));
 float trshaderDroplet=smoothstep(.78,.97,trshaderN2)*
   smoothstep(.22,.88,trshaderValueNoise(trshaderFallUv*vec2(4.0,7.5)+vec2(-trshaderFallTime*.72,trshaderFallTime*.11)));
 float trshaderCurtain=trshaderSideVeil*(.52+.48*trshaderN0);
 float trshaderThread=trshaderFineVeil*(.34+.66*trshaderN1);
 float trshaderFoamPower=clamp(TR456_WATER_FLOW_STREAK_FOAM,0.0,2.0)*
   TR_TOGGLE_FLOW_FOAM;
 float trshaderMist=smoothstep(.36,.88,trshaderN0*.46+trshaderN1*.34+trshaderN2*.20)*
   trshaderFoamPower;
 trshaderMist=trshaderSat(trshaderMist*2.05);
 float trshaderFoamVeil=smoothstep(.24,.82,trshaderCurtain*.32+trshaderThread*.38+
   trshaderSideVeil*.20+trshaderFineVeil*.22+trshaderN2*.12)*trshaderFoamPower;
 float trshaderImpactPlume=smoothstep(.34,.92,trshaderThread*.38+trshaderDroplet*.34+
   trshaderMist*.34+trshaderN1*.18)*trshaderFoamPower;
 float trshaderPlume=smoothstep(.24,.86,trshaderCurtain*.30+trshaderThread*.40+
   trshaderDroplet*.26+trshaderMist*.46+trshaderImpactPlume*.30);
 float trshaderVeil=smoothstep(.14,.78,trshaderN0*.28+trshaderN1*.40+trshaderN2*.32);
 float trshaderSheetNoise=trshaderValueNoise(vec2(trshaderFallUv.y*2.2+trshaderN0*.54,
   trshaderFallUv.x*.40-trshaderFallTime*.16));
 float trshaderSheetBreak=trshaderValueNoise(trshaderFallUv*vec2(1.55,5.2)+
   vec2(-trshaderFallTime*.11,trshaderFallTime*.028));
 float trshaderSheetLayer=smoothstep(.18,.78,trshaderCurtain*.38+trshaderVeil*.26+
   trshaderSheetNoise*.20+trshaderSheetBreak*.12+trshaderN0*.10);
 float trshaderStrandBroad=trshaderLineMask(trshaderFallUv.y*4.4+trshaderFallUv.x*.34+
   trshaderN0*.56-trshaderFallTime*.22,4.6)*
   smoothstep(.20,.86,trshaderSideVeil+trshaderN1*.28);
 float trshaderStrandFine=trshaderLineMask(trshaderFallUv.y*12.8-trshaderFallUv.x*.20+
   trshaderN1*.46-trshaderFallTime*.46,13.5)*
   smoothstep(.28,.94,trshaderFineVeil+trshaderN2*.30);
 float trshaderStrandTear=trshaderLineMask(trshaderFallUv.y*20.0+trshaderFallUv.x*.16+
   trshaderN2*.30-trshaderFallTime*.72,22.0)*smoothstep(.42,.98,trshaderN2);
 float trshaderStrandLayer=trshaderSat((trshaderStrandBroad*.52+trshaderStrandFine*.42+
   trshaderStrandTear*.26+trshaderThread*.20)*(.50+.50*trshaderSheetLayer));
 float trshaderSurfaceFoam=smoothstep(.16,.74,trshaderCurtain*.28+trshaderThread*.26+
   trshaderVeil*.18+trshaderSheetNoise*.25+trshaderSheetBreak*.18+trshaderN0*.12+
   trshaderStrandLayer*.22)*trshaderFoamPower;
 trshaderSurfaceFoam=trshaderSat(trshaderSurfaceFoam*(1.08+.42*smoothstep(.16,.90,
   trshaderCurtain+trshaderThread+trshaderMist+trshaderImpactPlume)));
 float trshaderImpactNoise=trshaderValueNoise(vec2(trshaderFallUv.y*6.5+trshaderN1*.72,
   trshaderFallUv.x*1.3-trshaderFallTime*.52));
 float trshaderImpactPulse=trshaderLineMask(trshaderFallUv.y*8.4+trshaderN2*.62-trshaderFallTime*.64,5.0);
 float trshaderImpactBoil=smoothstep(.04,.66,trshaderPoolBlend)*
   smoothstep(.28,.92,trshaderImpactNoise*.40+trshaderImpactPulse*.24+
   trshaderDroplet*.26+trshaderMist*.28+trshaderSurfaceFoam*.22)*
   (.72+.28*trshaderJunction.w)*trshaderFoamPower;
 float trshaderVolumeFoam=trshaderSat(trshaderMist*.56+trshaderFoamVeil*.44+trshaderImpactPlume*.58+
   trshaderSurfaceFoam*.58+trshaderStrandLayer*.28*trshaderFoamPower+trshaderImpactBoil*.78+
   trshaderDroplet*.18+trshaderPlume*.20);
 float trshaderBloomFoam=trshaderSat((trshaderImpactBoil*.68+trshaderImpactPlume*.48+
   trshaderSurfaceFoam*.38+trshaderMist*.28+trshaderPlume*.20)*TR_TOGGLE_FLOW_FOAM);
 float trshaderAerated=trshaderSat(trshaderCurtain*.36+trshaderThread*.36+trshaderDroplet*.24+
   trshaderMist*.82+trshaderFoamVeil*.40+trshaderImpactPlume*.46+trshaderSurfaceFoam*.48+
   trshaderStrandLayer*.18+trshaderImpactBoil*.60+trshaderPlume*.24);
 vec2 trshaderFallWarp=vec2(0.0);
 vec3 trshaderSceneFall=trshaderOriginalWaterGrade(texture(uTrWaterScene,
   clamp(trshaderF.trshaderScreen+trshaderFallWarp,vec2(.001),vec2(.999))).rgb);
 vec3 trshaderCascadeNormal=normalize(vSynNormal);
 float trshaderFallFres=pow(1.0-trshaderSat(abs(dot(trshaderCascadeNormal,trshaderF.trshaderViewDir))),1.85);
 float trshaderFallFresQuiet=trshaderFallFres*.24;
 vec3 trshaderCoolBody=trshaderSceneFall*vec3(.56,.84,1.18)+vec3(.028,.096,.154);
 vec3 trshaderFoamColor=mix(vec3(.54,.72,.78),vec3(.86,.96,1.00),
   trshaderSat(trshaderAerated*1.06+trshaderSurfaceFoam*.34+trshaderFallFresQuiet*.14));
 vec3 trshaderCol=mix(trshaderSceneFall,trshaderCoolBody,trshaderSat(.30+trshaderAerated*.44+trshaderMist*.24+
   trshaderPlume*.12+trshaderVolumeFoam*.12+trshaderSurfaceFoam*.08));
 vec3 trshaderSheetColor=trshaderSceneFall*vec3(.68,.88,1.08)+vec3(.016,.056,.092);
 trshaderCol=mix(trshaderCol,trshaderSheetColor,trshaderSat(trshaderSheetLayer*.20+trshaderStrandLayer*.12));
 float trshaderInnerShadow=trshaderSat((1.0-trshaderSheetBreak)*trshaderSheetLayer*(1.0-trshaderSurfaceFoam*.42));
 trshaderCol*=mix(vec3(1.0),vec3(.88,.95,1.02),trshaderInnerShadow*.18);
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderSat((trshaderMist*.74+trshaderThread*.30+trshaderDroplet*.22+
   trshaderCurtain*.10+trshaderPlume*.30+trshaderFoamVeil*.34+trshaderImpactPlume*.36+
   trshaderSurfaceFoam*.48+trshaderStrandLayer*.24*trshaderFoamPower+trshaderImpactBoil*.62)*
   TR_TOGGLE_FLOW_FOAM));
 trshaderCol=mix(trshaderCol,trshaderCol*vec3(.92,1.02,1.08)+trshaderFoamColor*.18,
   trshaderSat((trshaderPlume*.30+trshaderVeil*.14+trshaderMist*.22+trshaderVolumeFoam*.20+
   trshaderSurfaceFoam*.26+trshaderStrandLayer*.12*trshaderFoamPower+trshaderImpactBoil*.36)*
   TR_TOGGLE_FLOW_FOAM));
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderBloomFoam*.16);
 trshaderCol+=trshaderFoamColor*(trshaderMist*.092+trshaderPlume*.068+trshaderVolumeFoam*.078+
   trshaderImpactPlume*.052+trshaderSurfaceFoam*.070+trshaderImpactBoil*.105+
   trshaderBloomFoam*.090)*
   TR_TOGGLE_FLOW_FOAM;
 float trshaderFallSpark=smoothstep(.91,.985,
   trshaderValueNoise(trshaderFallUv*vec2(9.5,16.0)+vec2(-trshaderFallTime*.66,trshaderFallTime*.14)))*
   smoothstep(.58,.96,trshaderThread+trshaderDroplet*.34)*
   step(.94,trshaderHash12(floor(trshaderFallUv*vec2(3.0,7.0)+vec2(-trshaderFallTime*.18,0.0))));
 trshaderCol+=vec3(.12,.22,.28)*(trshaderFallSpark*.052+trshaderFallFresQuiet*.006)*
  TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
 vec3 trshaderLight=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.22)),.18);
 trshaderCol*=trshaderLight;
 trshaderCol=(trshaderCol-.5)*1.035+.5;
 float trshaderCascadeAlpha=clamp(.55+trshaderMist*.10+trshaderSurfaceFoam*.08+
  trshaderImpactBoil*.10+trshaderBloomFoam*.08+trshaderPlume*.04,.48,.78);
 return vec4(clamp(trshaderCol,0.0,1.0),trshaderCascadeAlpha);
}

struct TrshaderFlowCore {
 vec2 trshaderFlowUv;
 vec4 trshaderFlowAnimPatch;
 vec4 trshaderPatchField;
 vec4 trshaderPattern;
 vec3 trshaderFlowField;
 vec3 trshaderMicroChop;
 vec4 trshaderRefrStreak;
};

TrshaderFlowCore trshaderBuildFlowCore(float trshaderFlowTime,
                                       float trshaderGameTravel,
                                       float trshaderFlowSpeed){
 TrshaderFlowCore trshaderCore;
  trshaderCore.trshaderFlowUv=vSynFlowUv;
  float trshaderOriginalSync=clamp(TR456_WATER_FLOW_ORIGINAL_SYNC,0.0,1.0);
  float trshaderAnimTravel=mix(
    trshaderFlowTime*(.18+trshaderFlowSpeed*.20),
    trshaderGameTravel,trshaderOriginalSync);
  trshaderCore.trshaderFlowAnimPatch=trshaderFlowPatchField(
    trshaderCore.trshaderFlowUv,trshaderAnimTravel,trshaderFlowSpeed);
 trshaderCore.trshaderPatchField=trshaderFlowPatchField(
   trshaderCore.trshaderFlowUv,trshaderGameTravel,trshaderFlowSpeed);
 trshaderCore.trshaderPattern=trshaderSyntheticFlowPattern(
   trshaderCore.trshaderFlowUv,trshaderFlowTime,trshaderFlowSpeed,
   trshaderCore.trshaderFlowAnimPatch);
 trshaderCore.trshaderFlowField=trshaderSyntheticFlowField(
   trshaderCore.trshaderFlowUv,trshaderFlowTime,trshaderFlowSpeed,
   trshaderCore.trshaderFlowAnimPatch);
 trshaderCore.trshaderMicroChop=trshaderFlowMicroChopField(
   trshaderCore.trshaderFlowUv,trshaderGameTravel,trshaderFlowSpeed,
   trshaderCore.trshaderPatchField);
 trshaderCore.trshaderRefrStreak=trshaderFlowRefractiveStreakField(
   trshaderCore.trshaderFlowUv,trshaderGameTravel,trshaderFlowSpeed,
   trshaderCore.trshaderPatchField);
 return trshaderCore;
}

vec4 trshaderRenderSurfaceFlow(TrshaderSyntheticFrame trshaderF, vec2 trshaderFlowDir, vec2 trshaderFlowSide,
                       vec2 trshaderFlowScreenDir, vec2 trshaderFlowScreenSide,
                       float trshaderFlowSpeed, float trshaderDuplicatePass,
                       float trshaderFlowTime, float trshaderGameTravel,
                       float trshaderSettledWarp,
                       TrshaderFlowCore trshaderCore){
 float trshaderPassOpacity=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.0),trshaderDuplicatePass);
 vec2 trshaderFlowUv=trshaderCore.trshaderFlowUv;
 vec4 trshaderPatchField=trshaderCore.trshaderPatchField;
 vec4 trshaderPattern=trshaderCore.trshaderPattern;
 vec3 trshaderFlowField=trshaderCore.trshaderFlowField;
 vec3 trshaderMicroChop=trshaderCore.trshaderMicroChop;
 vec4 trshaderRefrStreak=trshaderCore.trshaderRefrStreak;
  vec4 trshaderVolumeWave=trshaderFlowVolumeWaveField(trshaderFlowUv,trshaderGameTravel,trshaderFlowSpeed,trshaderPatchField);
  vec4 trshaderSmoothDeform=trshaderSmoothFlowDeformationField(vSynWorldPos,trshaderFlowDir,trshaderFlowSide,
    trshaderGameTravel,trshaderFlowSpeed,0.0);
  float trshaderReliefCtl=clamp(TR456_WATER_FLOW_RELIEF_STRENGTH,0.0,2.4);
  float trshaderRefractionReliefCtl=clamp(TR456_WATER_FLOW_REFRACTION_RELIEF,0.0,2.8);
  vec3 trshaderEdgeA=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowScreenSide*.0065,
    vec2(.001),vec2(.999))).rgb;
 vec3 trshaderEdgeB=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen-trshaderFlowScreenSide*.0065,
   vec2(.001),vec2(.999))).rgb;
  float trshaderEdgeContrast=smoothstep(.035,.22,abs(trshaderLuma(trshaderEdgeA)-trshaderLuma(trshaderEdgeB)));
  float trshaderEdgeNoise=trshaderValueNoise(trshaderFlowUv*vec2(7.8,13.0)+vec2(-trshaderGameTravel*.42,trshaderGameTravel*.050));
  float trshaderProfileFoam=clamp(uTrWaterSyntheticProfile.y,0.0,2.0);
  float trshaderEdgeWaveStrength=mix(.75,1.25,clamp(TR456_WATER_EDGE_WAVE,0.0,1.0));
  float trshaderShoreFlow=trshaderSat(trshaderF.trshaderShoreline*TR456_WATER_SHORELINE_STRENGTH*
    trshaderProfileFoam*(.66+.34*trshaderPattern.w));
  float trshaderBankTongue=trshaderLineMask(trshaderFlowUv.y*5.2+trshaderEdgeNoise*1.4-
     trshaderGameTravel*(.28+trshaderFlowSpeed*.10),4.0)*
    smoothstep(.18,.82,trshaderShoreFlow+trshaderEdgeContrast*.42+trshaderPattern.w*.22)*
    trshaderEdgeWaveStrength;
  float trshaderBankLace=trshaderLineMask(trshaderFlowUv.y*14.0+trshaderPattern.z*.45+
     sin(trshaderFlowUv.x*1.5-trshaderGameTravel*.20)*.28,10.0)*
    smoothstep(.32,.90,trshaderEdgeNoise+trshaderShoreFlow*.30)*
    trshaderEdgeWaveStrength;
  float trshaderShorelineBand=trshaderSat(trshaderShoreFlow*.90+trshaderEdgeContrast*.42+
    trshaderBankTongue*.26+trshaderBankLace*.14);
  float trshaderEdgeMicroGate=trshaderShorelineBand*TR456_WATER_FLOW_EDGE_FOAM*
    TR_TOGGLE_FLOW_FOAM;
  float trshaderEdgeMicroA=0.0;
  float trshaderEdgeMicroB=0.0;
  float trshaderEdgeMicroWave=0.0;
  float trshaderEdgeMicroFoam=0.0;
  if(trshaderEdgeMicroGate>.001) {
   float trshaderEdgeMicroNoise=trshaderValueNoise(trshaderFlowUv*vec2(28.0,52.0)+
     vec2(trshaderGameTravel*.55,-trshaderGameTravel*.18));
   trshaderEdgeMicroA=sin(trshaderFlowUv.x*70.0+trshaderFlowUv.y*9.5-
     trshaderGameTravel*(7.40+trshaderFlowSpeed*.42)+trshaderEdgeMicroNoise*2.4);
   trshaderEdgeMicroB=sin(trshaderFlowUv.y*92.0-trshaderFlowUv.x*6.0-
     trshaderGameTravel*(9.60+trshaderFlowSpeed*.55)+trshaderPattern.z*1.8);
   trshaderEdgeMicroWave=(trshaderEdgeMicroA*.55+trshaderEdgeMicroB*.45)*
     trshaderEdgeMicroGate*(.50+.50*trshaderEdgeNoise);
   trshaderEdgeMicroFoam=trshaderLineMask(trshaderFlowUv.y*38.0+trshaderEdgeMicroNoise*1.6-
     trshaderGameTravel*(2.40+trshaderFlowSpeed*.20),18.0)*trshaderEdgeMicroGate;
  }
  float trshaderEdgeTurb=(trshaderEdgeContrast*smoothstep(.42,.90,trshaderEdgeNoise)*
    TR456_WATER_FLOW_EDGE_FOAM+trshaderShoreFlow*.44+
    (trshaderBankTongue*.42+trshaderBankLace*.22)*TR456_WATER_FLOW_EDGE_FOAM)*
    TR_TOGGLE_FLOW_FOAM;
  trshaderEdgeTurb=trshaderSat(trshaderEdgeTurb+(abs(trshaderEdgeMicroWave)*.24+trshaderEdgeMicroFoam*.16)*
    TR_TOGGLE_FLOW_FOAM);
  float trshaderSettleMask=smoothstep(.18,.92,trshaderSettledWarp);
   trshaderEdgeTurb*=mix(1.0,.42,trshaderSettleMask);
   trshaderBankTongue*=mix(1.0,.55,trshaderSettleMask);
   trshaderBankLace*=mix(1.0,.55,trshaderSettleMask);
   trshaderEdgeMicroWave*=mix(1.0,.35,trshaderSettleMask);
   trshaderEdgeMicroFoam*=mix(1.0,.45,trshaderSettleMask);
   trshaderEdgeMicroGate*=mix(1.0,.50,trshaderSettleMask);
   vec2 trshaderFlowSlopeWorld=trshaderFlowDir*trshaderFlowField.x+trshaderFlowSide*trshaderFlowField.y;
  vec2 trshaderVolumeSlopeWorld=(trshaderFlowDir*trshaderVolumeWave.x+trshaderFlowSide*trshaderVolumeWave.y)*trshaderReliefCtl;
  trshaderSmoothDeform*=mix(1.0,.58,trshaderSettleMask);
  vec2 trshaderSmoothDeformWorld=(trshaderFlowDir*trshaderSmoothDeform.x+trshaderFlowSide*trshaderSmoothDeform.y)*trshaderReliefCtl;
  float trshaderBreathPulse=sin(trshaderFlowUv.x*12.5-trshaderGameTravel*3.35+trshaderPattern.z*1.35)*.5+.5;
 float trshaderBreathFine=sin(trshaderFlowUv.x*25.0+trshaderFlowUv.y*2.1-trshaderGameTravel*5.10+trshaderEdgeNoise*.80)*.5+.5;
 float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 float trshaderLongWaveCtl=clamp(TR456_WATER_FLOW_LONGITUDINAL_WAVE,0.0,2.4);
 float trshaderTransWaveCtl=clamp(TR456_WATER_FLOW_TRANSVERSE_WAVE,0.0,2.4);
  float trshaderCrossPulse=sin(trshaderFlowUv.y*17.0+trshaderFlowUv.x*1.6-trshaderGameTravel*(1.95+trshaderFlowSpeed*.18)+
    trshaderEdgeNoise*.55)*.5+.5;
  float trshaderCrossFine=sin(trshaderFlowUv.y*34.0-trshaderFlowUv.x*1.1-trshaderGameTravel*(3.20+trshaderFlowSpeed*.26))*.5+.5;
 float trshaderCrossFine3=trshaderCrossFine*trshaderCrossFine*trshaderCrossFine;
 float trshaderCrossDistortion=clamp(TR456_WATER_FLOW_CROSS_DISTORTION,0.0,3.0);
 float trshaderCrossShearA=sin(trshaderFlowUv.y*41.0+trshaderFlowUv.x*3.8-
   trshaderGameTravel*(2.25+trshaderFlowSpeed*.22)+trshaderPattern.x*1.2);
 float trshaderCrossShearB=sin(trshaderFlowUv.y*73.0-trshaderFlowUv.x*5.4+
   trshaderGameTravel*(1.85+trshaderFlowSpeed*.17)+trshaderEdgeNoise*2.0);
  float trshaderCrossCounter=sin(trshaderFlowUv.x*18.0-trshaderFlowUv.y*21.0+
    trshaderGameTravel*(3.10+trshaderFlowSpeed*.18)+trshaderPattern.z*1.6);
  float trshaderCrossShear=(trshaderCrossShearA*.55+trshaderCrossShearB*.45)*
    trshaderCrossStrength*trshaderCrossDistortion*trshaderTransWaveCtl;
  float trshaderCrossRefract=(trshaderCrossShear*.72+trshaderCrossCounter*.28*
    trshaderCrossStrength*trshaderCrossDistortion*trshaderTransWaveCtl);
 float trshaderBreathFine3=trshaderBreathFine*trshaderBreathFine*trshaderBreathFine;
 float trshaderSurfaceBreath=.72+.28*pow(trshaderBreathPulse,1.55);
 float trshaderSurfacePulse=(pow(trshaderBreathPulse,1.8)*.58+trshaderBreathFine3*.28)*
   (.42+.58*trshaderPattern.w);
 float trshaderTensionStrength=clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0);
 float trshaderTensionGate=smoothstep(.28,.86,
   trshaderValueNoise(trshaderFlowUv*vec2(2.2,5.6)+vec2(-trshaderGameTravel*.055,trshaderGameTravel*.021)));
 float trshaderTensionLineA=trshaderLineMask(trshaderFlowUv.x*5.8+trshaderEdgeNoise*.42-
   trshaderGameTravel*(.30+trshaderFlowSpeed*.08),6.0);
  float trshaderTensionLineB=trshaderLineMask(trshaderFlowUv.x*11.6+trshaderFlowUv.y*.82+trshaderPattern.z*.45-
    trshaderGameTravel*(.52+trshaderFlowSpeed*.14),18.0);
  float trshaderTensionFilm=trshaderSat((trshaderTensionLineA*.52+trshaderTensionLineB*.42)*trshaderTensionGate*
    (.62+.38*trshaderPattern.w))*trshaderTensionStrength;
 float trshaderFlowContactStrength=clamp(TR456_WATER_FLOW_CONTACT_STRENGTH,0.0,3.0)*
    TR_TOGGLE_CONTACT_RIPPLES;
  float trshaderFlowContactNormal=clamp(TR456_WATER_FLOW_CONTACT_NORMAL,0.0,3.0);
  float trshaderFlowRippleMix=clamp(TR456_WATER_FLOW_CONTACT_RIPPLES,0.0,1.0);
  vec2 trshaderFlowContactSlope=(trshaderF.trshaderFlowContacts.xy+
    (trshaderF.trshaderContacts.xy*1.35+trshaderF.trshaderContactWake.xy*.72)*trshaderFlowRippleMix)*
    trshaderFlowContactStrength;
  float trshaderFlowContactEnergy=trshaderSat((trshaderF.trshaderFlowContacts.z*.70+
    (trshaderF.trshaderContacts.z*.44+trshaderF.trshaderContactWake.z*.72+
    trshaderF.trshaderContactWake.w*.58)*trshaderFlowRippleMix)*trshaderFlowContactStrength);
  vec2 trshaderFlowSlope=trshaderFlowSlopeWorld*.54*trshaderSurfaceBreath+
    trshaderVolumeSlopeWorld*1.08+
    trshaderSmoothDeformWorld*.86+
    trshaderFlowDir*((trshaderPattern.w*.24+trshaderPattern.y*.16+trshaderSurfacePulse*.10+
      trshaderTensionFilm*.18+(pow(trshaderCrossPulse,2.2)-.34)*.052*trshaderCrossStrength+
      trshaderMicroChop.x*.72+trshaderRefrStreak.x*.90+trshaderEdgeTurb*.030+
      trshaderEdgeMicroWave*.055+trshaderEdgeMicroFoam*.018)*trshaderLongWaveCtl*trshaderReliefCtl)+
    trshaderFlowSide*(((trshaderPattern.x-trshaderPattern.z)*.075+
      sin(trshaderFlowUv.x*9.0+trshaderFlowUv.y*1.3-trshaderGameTravel*3.20)*.026+
      (pow(trshaderCrossPulse,1.7)-.36)*.145*trshaderCrossStrength+
      (trshaderCrossFine3-.24)*.062*trshaderCrossStrength+
      trshaderCrossShear*.016+trshaderCrossRefract*.010+
      (trshaderTensionLineB-.5)*trshaderTensionFilm*.040+
      trshaderMicroChop.y*.64+trshaderRefrStreak.y*.85+(trshaderEdgeNoise-.5)*trshaderEdgeTurb*.020+
      (trshaderEdgeMicroA-trshaderEdgeMicroB)*trshaderEdgeMicroGate*.016)*trshaderTransWaveCtl*trshaderReliefCtl)+
    trshaderFlowContactSlope+
    trshaderF.trshaderRelief.xy*.18+trshaderF.trshaderAlive.xy*.12;
  vec2 trshaderFlowNormalSlope=trshaderFlowSlope+trshaderFlowContactSlope*.42*trshaderFlowContactNormal;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
  float trshaderFlowBumpAmount=TR456_WATER_BUMP_STRENGTH*
    TR456_WATER_SYNTHETIC_BUMP_STRENGTH*TR456_WATER_FLOW_BUMP_STRENGTH;
  trshaderFlowNormalSlope+=trshaderSyntheticBumpSlope(
    trshaderFlowDir*(trshaderMicroChop.x*1.20+trshaderRefrStreak.x*1.05+trshaderPattern.w*.090+
      trshaderSurfacePulse*.070+trshaderTensionFilm*.10+trshaderEdgeMicroWave*.14+
      trshaderEdgeMicroFoam*.050+trshaderSmoothDeform.x*.42+trshaderVolumeWave.x*.32)+
   trshaderFlowSide*(trshaderMicroChop.y*1.08+trshaderRefrStreak.y*1.00+
      (trshaderPattern.x-trshaderPattern.z)*.080+
      (pow(trshaderCrossPulse,1.7)-.36)*.070*trshaderCrossStrength+
      trshaderCrossShear*.036+
      trshaderSmoothDeform.y*.38+trshaderVolumeWave.y*.28+
      (trshaderEdgeMicroA-trshaderEdgeMicroB)*trshaderEdgeMicroGate*.045)+
    trshaderTextureMicroBump(vSynWorldPos.xz,trshaderF.trshaderTime,
      trshaderFlowDir,1.0)*1.18,
    trshaderFlowBumpAmount*mix(1.0,1.26,trshaderSat(trshaderReliefCtl-1.0)),.50);
#endif
 vec3 trshaderFlowNormal=normalize(vec3(-trshaderFlowNormalSlope.x,1.0,-trshaderFlowNormalSlope.y));
 float trshaderFlowNdv=trshaderSat(abs(dot(trshaderFlowNormal,trshaderF.trshaderViewDir)));
 float trshaderFlowFres=pow(1.0-trshaderFlowNdv,2.35)*TR456_WATER_FRESNEL_STRENGTH;
 float trshaderFlowSignal=trshaderSat(trshaderPattern.x*.42+trshaderPattern.y*.54+trshaderPattern.z*.34+
    trshaderPattern.w*.24+abs(trshaderFlowField.z)*.34+trshaderMicroChop.z*.24+
      trshaderRefrStreak.z*.16+trshaderVolumeWave.z*.36*trshaderReliefCtl+trshaderVolumeWave.w*.14*trshaderReliefCtl+
      trshaderEdgeTurb*.18+trshaderTensionFilm*.20+
      abs(trshaderEdgeMicroWave)*.20+trshaderEdgeMicroFoam*.16+
      trshaderSmoothDeform.z*.26*trshaderReliefCtl+trshaderSmoothDeform.w*.12*trshaderReliefCtl+
      (trshaderCrossPulse*trshaderCrossPulse*.18+trshaderCrossFine3*.10)*trshaderCrossStrength+
      abs(trshaderCrossShear)*.030+abs(trshaderCrossRefract)*.018+
     trshaderFlowContactEnergy*.18+
     trshaderPatchField.x*.075+trshaderPatchField.y*.16);
  trshaderFlowSignal*=mix(1.0,.58,trshaderSettleMask);

  vec2 trshaderLongPull=trshaderFlowScreenDir*(trshaderPattern.w*.017+trshaderPattern.y*.014+trshaderFlowSignal*.010+
    trshaderSurfacePulse*.018+trshaderRefrStreak.z*.012+trshaderMicroChop.z*.007+
    trshaderVolumeWave.z*.018+trshaderTensionFilm*.014+trshaderSmoothDeform.z*.012+
    trshaderSmoothDeform.x*.004)*trshaderLongWaveCtl;
  vec2 trshaderCrossTear=trshaderFlowScreenSide*((trshaderPattern.x-trshaderPattern.z)*.0075+trshaderFlowSlope.y*.0042+
    (trshaderTensionLineB-.5)*trshaderTensionFilm*.012+
    (trshaderCrossPulse-.5)*.015*trshaderCrossStrength+trshaderCrossRefract*.0065)*trshaderTransWaveCtl;
  vec2 trshaderCrossWeave=trshaderFlowScreenSide*(trshaderCrossShear*.0048+trshaderCrossCounter*
    trshaderCrossStrength*trshaderCrossDistortion*.0022)+trshaderFlowScreenDir*(trshaderCrossCounter*
    trshaderCrossStrength*trshaderCrossDistortion*.0018*trshaderLongWaveCtl);
  trshaderCrossWeave*=trshaderTransWaveCtl;
  vec2 trshaderReliefPull=trshaderFlowScreenDir*(trshaderVolumeWave.x*.026+trshaderSmoothDeform.x*.018+
    trshaderFlowField.x*.0045+trshaderMicroChop.x*.0055)*trshaderLongWaveCtl+
    trshaderFlowScreenSide*(trshaderVolumeWave.y*.022+trshaderSmoothDeform.y*.016+
    trshaderFlowField.y*.0040+trshaderMicroChop.y*.0050)*trshaderTransWaveCtl;
  trshaderReliefPull*=trshaderReliefCtl*trshaderRefractionReliefCtl;
  vec2 trshaderContactPull=trshaderFlowScreenDir*(trshaderFlowContactSlope.x*.0065+
    trshaderFlowContactEnergy*.010)+trshaderFlowScreenSide*(trshaderFlowContactSlope.y*.0048);
  vec2 trshaderEdgePull=trshaderFlowScreenDir*(trshaderEdgeTurb*.020+trshaderEdgeMicroWave*.010+
    trshaderEdgeMicroFoam*.006)+trshaderFlowScreenSide*(((trshaderEdgeNoise-.5)*trshaderEdgeTurb*.016)+
    (trshaderEdgeMicroA-trshaderEdgeMicroB)*trshaderEdgeMicroGate*.004);
  vec2 trshaderFlowWarp=(trshaderFlowScreenDir*(trshaderFlowSlope.x*.0052+trshaderRefrStreak.x*.50+trshaderMicroChop.x*.16)*trshaderLongWaveCtl+
    trshaderFlowScreenSide*(trshaderRefrStreak.y*.40+trshaderMicroChop.y*.14+trshaderSmoothDeform.y*.020)*trshaderTransWaveCtl+
    trshaderLongPull*.82+trshaderCrossTear*.62+trshaderCrossWeave*.72+
    trshaderReliefPull*.66+trshaderContactPull*.76+trshaderEdgePull*.72)*
   clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,2.2)*
   clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,3.2)*
   TR_TOGGLE_FLOW_WARP*mix(1.0,.10,trshaderSettleMask);
 trshaderFlowWarp=trshaderSoftLimitVec2(trshaderFlowWarp,.078);
 vec2 trshaderChromaWarp=trshaderFlowWarp*(.32+.22*TR456_WATER_FLOW_CHROMA);
 vec3 trshaderScene0=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp,vec2(.001),vec2(.999))).rgb;
 float trshaderFlowChroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA*
   TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
 vec3 trshaderRefractedSource=trshaderScene0;
 if(trshaderFlowChroma>.001) {
   vec3 trshaderSceneR=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp+trshaderChromaWarp+trshaderFlowScreenSide*.00055,
     vec2(.001),vec2(.999))).rgb;
   vec3 trshaderSceneB=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp-trshaderChromaWarp-trshaderFlowScreenDir*.00055,
     vec2(.001),vec2(.999))).rgb;
   trshaderRefractedSource=mix(trshaderScene0,vec3(trshaderSceneR.r,trshaderScene0.g,trshaderSceneB.b),.36*trshaderFlowChroma);
 }
 vec3 trshaderRefracted=trshaderOriginalWaterGrade(trshaderRefractedSource);

 float trshaderOpacityProfile=clamp(uTrWaterSyntheticProfile.z,0.05,2.0);
 float trshaderOpacity=clamp(uTrWaterSyntheticInfo.x*TR456_WATER_FLOW_OPACITY*
   trshaderPassOpacity*trshaderOpacityProfile,.020,.76);
 float trshaderTintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
  float trshaderFlowReflectStrength=TR456_WATER_FLOW_REFLECTION;
  float trshaderReflectAmt=clamp(uTrWaterSyntheticInfo.z*trshaderFlowReflectStrength*
    TR456_WATER_REFLECT_STRENGTH*.26*clamp(uTrWaterSyntheticProfile.w,.05,1.0),0.0,1.6);
  float trshaderDepthCue=0.0;
   float trshaderSceneFlowDepth=trshaderSat(((1.0-trshaderLuma(trshaderRefracted))*.34+trshaderDepthCue*.24+
     trshaderFlowSignal*.075+trshaderOpacity*.030)*TR456_WATER_DEPTH_STRENGTH*
     (.62+.38*TR456_WATER_FLOW_DEPTH_BODY));
   float trshaderMaterialFlowBody=trshaderStableWaterBody(trshaderOpacity,
     trshaderFlowSignal*.78+trshaderPattern.w*.22+trshaderMicroChop.z*.18+trshaderTensionFilm*.16,
     trshaderEdgeTurb*.30);
  float trshaderFlowDepth=max(trshaderSceneFlowDepth,trshaderMaterialFlowBody*.26);
  float trshaderDepthBody=smoothstep(.16,.82,trshaderFlowDepth);
  float trshaderDepthOpacity=trshaderDepthAwareOpacity(trshaderOpacity,trshaderFlowDepth);
   vec3 trshaderFlowTint=mix(vec3(.018,.086,.078),vec3(.006,.036,.042),
     trshaderSat(trshaderFlowDepth+trshaderDepthCue*.30))*trshaderTintStrength;
   trshaderRefracted=trshaderWaterVolume(trshaderRefracted,
    max(trshaderSceneFlowDepth,trshaderMaterialFlowBody*.16)*(.62+.38*trshaderDepthBody),trshaderFlowNdv,
    trshaderFlowTint*mix(.55,1.0,trshaderDepthBody));
 vec3 trshaderReflected=trshaderFlowTint;
 float trshaderReflMask=0.0;
#if TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED
 float trshaderReflectActive=trshaderReflectAmt;
 if(trshaderReflectActive>.001) {
  vec2 trshaderReflectWarp=trshaderFlowScreenDir*(trshaderFlowSlope.x*.005+trshaderPattern.x*.006+trshaderPattern.y*.004)+
    trshaderFlowScreenSide*(-abs(trshaderFlowSlope.y)*.003+trshaderPattern.z*.005+trshaderFlowFres*.008);
  trshaderReflectWarp=clamp(trshaderReflectWarp,vec2(-.075),vec2(.075));
  vec3 trshaderSceneRefl=trshaderStableSceneColor(trshaderF.trshaderScreen+trshaderReflectWarp,trshaderF.trshaderScreen)*.46+
    trshaderStableSceneColor(trshaderF.trshaderScreen+trshaderReflectWarp*.55+vec2(0.0,.034+trshaderFlowFres*.040),
      trshaderF.trshaderScreen)*.34+
    trshaderStableSceneColor(trshaderF.trshaderScreen-trshaderReflectWarp*.82+trshaderFlowScreenSide*.018,
      trshaderF.trshaderScreen)*.20;
#if TR456_WATER_REFLECTION_QUALITY > 1
  vec3 trshaderReflNormal=normalize(trshaderFlowNormal+vec3(trshaderReflectWarp.x*18.0,0.0,
    -trshaderReflectWarp.y*18.0));
  vec2 trshaderMirrorUv0=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderReflNormal,trshaderF.trshaderViewDir,
    trshaderReflectWarp*.72,0.0,.45+.45*trshaderFlowFres);
  vec2 trshaderMirrorUv1=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderReflNormal,trshaderF.trshaderViewDir,
    trshaderReflectWarp*.30+vec2(0.0,.040+trshaderFlowFres*.055),
    .010+.018*trshaderFlowFres,.38+.36*trshaderFlowFres);
  vec2 trshaderMirrorUv2=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderReflNormal,trshaderF.trshaderViewDir,
    -trshaderReflectWarp*.92-trshaderFlowScreenSide*.020,0.0,.36+.32*trshaderFlowFres);
  vec3 trshaderMirrorRefl=trshaderStableSceneColor(trshaderMirrorUv0,trshaderF.trshaderScreen)*.55+
    trshaderStableSceneColor(trshaderMirrorUv1,trshaderF.trshaderScreen)*.30+
    trshaderStableSceneColor(trshaderMirrorUv2,trshaderF.trshaderScreen)*.15;
  float trshaderRibbonMirror=trshaderSat((trshaderPattern.w*.36+trshaderRefrStreak.z*.24+
    trshaderMicroChop.z*.14+trshaderTensionFilm*.18+trshaderSmoothDeform.z*.12)*
    (0.55+trshaderFlowFres*.45));
  vec2 trshaderRibbonUv=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderReflNormal,
    trshaderF.trshaderViewDir,
    trshaderReflectWarp*1.25+trshaderFlowScreenDir*(trshaderRefrStreak.x*.18)+
    trshaderFlowScreenSide*(trshaderRefrStreak.y*.16),.004+.010*trshaderRibbonMirror,
    .52+.28*trshaderFlowFres);
  vec3 trshaderRibbonRefl=trshaderStableSceneColor(trshaderRibbonUv,trshaderF.trshaderScreen);
  trshaderMirrorRefl=mix(trshaderMirrorRefl,trshaderRibbonRefl,
    trshaderRibbonMirror*.22*TR_TOGGLE_FLOW_REFLECTION);
  trshaderReflected=trshaderReflectionGrade(mix(trshaderSceneRefl,trshaderMirrorRefl,
    trshaderSat(.26+trshaderFlowFres*.58+trshaderFlowDepth*.16+trshaderRibbonMirror*.10)));
  trshaderReflMask=trshaderSat((.012+trshaderFlowFres*.070+trshaderFlowSignal*.012+
    trshaderFlowDepth*.006+trshaderRibbonMirror*.018)*
    trshaderReflectActive*(1.0-trshaderPattern.y*.22)*
    mix(.60,1.0,trshaderReflectionUvFade(trshaderMirrorUv0)));
#else
  trshaderReflected=trshaderReflectionGrade(trshaderSceneRefl);
  trshaderReflMask=trshaderSat((.010+trshaderFlowFres*.060+
    trshaderFlowSignal*.010+trshaderFlowDepth*.005)*
    trshaderReflectActive*(1.0-trshaderPattern.y*.22)*
    mix(.60,1.0,trshaderReflectionUvFade(trshaderF.trshaderScreen+
    trshaderReflectWarp)));
#endif
 }
#endif

 float trshaderPatternX2=trshaderPattern.x*trshaderPattern.x;
 float trshaderFoamStress=trshaderSat(trshaderEdgeTurb*.95+trshaderShoreFlow*.95+trshaderF.trshaderContactWake.w*.80+
     trshaderMicroChop.z*.38+trshaderRefrStreak.z*.22+trshaderPatternX2*.24+
     trshaderTensionFilm*.18+trshaderFlowContactEnergy*.28+
     abs(trshaderEdgeMicroWave)*.32+trshaderEdgeMicroFoam*.20+
     trshaderPatchField.y*.62+trshaderBankTongue*.38+trshaderBankLace*.22+
     abs(trshaderFlowSlope.y)*.018);
 float trshaderFoamGate=trshaderStressFoamGate(trshaderFoamStress);
 float trshaderAeration=trshaderSat(trshaderPattern.y*.62+trshaderPattern.z*.20+trshaderEdgeTurb*.42+
   trshaderFlowSignal*.28*TR456_WATER_FLOW_AERATION)*mix(.30,1.0,trshaderFoamGate);
  float trshaderFoamMask=trshaderSat((trshaderPattern.y*.22+trshaderPatternX2*.16+
     trshaderEdgeTurb*.70+trshaderF.trshaderContactWake.w*.48+trshaderFlowContactEnergy*.10+trshaderShoreFlow*.78+
     trshaderBankTongue*.34+trshaderBankLace*.18+trshaderMicroChop.z*.10+trshaderRefrStreak.z*.06+
     trshaderEdgeMicroFoam*.22+abs(trshaderEdgeMicroWave)*.12)*
     TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*trshaderFoamGate;
   float trshaderDirectionalFoam=trshaderSat((pow(trshaderPattern.w,1.35)*.34+trshaderPattern.y*.30+
     trshaderBankTongue*.18+trshaderBankLace*.10+trshaderRefrStreak.z*.10+
     trshaderEdgeMicroFoam*.14)*
   smoothstep(.16,.86,trshaderFlowSignal+trshaderShoreFlow*.28)*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*trshaderFoamGate;
 float trshaderTensionPatch=trshaderSat((trshaderTensionFilm*.62+trshaderPatchField.x*.18+
   trshaderMicroChop.z*.12+trshaderPattern.w*.10)*TR456_WATER_FLOW_SURFACE_TENSION*
   (1.0-trshaderFoamMask*.36));
 trshaderFoamMask=trshaderSat(trshaderFoamMask+trshaderDirectionalFoam*.42);
 float trshaderBodyMask=trshaderSat((trshaderFlowDepth*.45+trshaderFlowSignal*.24+trshaderPattern.w*.12)*
   TR456_WATER_FLOW_BODY);
 float trshaderRidgeMask=trshaderSat((trshaderPattern.x*.62+abs(trshaderFlowField.z)*.28+trshaderFlowSignal*.24)*
   TR456_WATER_FLOW_RIDGE);
 float trshaderGlint=trshaderFastPow48(trshaderSat(dot(trshaderFlowNormal,normalize(vec3(-.25,.93,.27)))))*
   (.18+trshaderFlowFres*.60)*(trshaderFlowSignal*.55+trshaderPattern.w*.25+.12)*
   TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
 float trshaderSparkPhase=trshaderFlowUv.x*12.4+trshaderValueNoise(trshaderFlowUv*5.5)*.42-
   trshaderGameTravel*(1.42+trshaderFlowSpeed*.32);
 float trshaderSparkLife=fract(trshaderSparkPhase*.45);
 float trshaderSparkGate=step(.88,trshaderHash12(floor(vec2(trshaderSparkPhase*.45,trshaderFlowUv.y*5.5))))*
   smoothstep(.05,.32,trshaderSparkLife)*(1.0-smoothstep(.52,.96,trshaderSparkLife));
 float trshaderFlowSpark=trshaderLineMask(trshaderSparkPhase,24.0)*
   smoothstep(.45,.92,trshaderPattern.w+trshaderPattern.y*.55+trshaderPattern.x*.35)*
   smoothstep(.08,.82,1.0-trshaderFlowDepth*.55)*trshaderSparkGate;
  float trshaderStreakGlint=(trshaderLineMask(trshaderFlowUv.x*4.35+trshaderValueNoise(trshaderFlowUv*3.2)*.24-
    trshaderGameTravel*(.56+trshaderFlowSpeed*.18),15.0)*smoothstep(.26,.88,trshaderFlowSignal)+
    trshaderFlowSpark*.72+trshaderRefrStreak.z*.12)*
    TR456_WATER_FLOW_SPECULAR_STREAK*TR456_WATER_GLINT_STRENGTH*
    TR456_WATER_FLOW_GLINT;
  float trshaderTensionGlint=pow(trshaderSat(trshaderTensionFilm),1.7)*(.16+trshaderFlowFres*.20)*
   TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;

  vec3 trshaderFlowBody=mix(trshaderRefracted,trshaderFlowTint+vec3(.004,.014,.014),
    max(trshaderDepthOpacity*.040+trshaderBodyMask*.026,
      trshaderMaterialFlowBody*(.11+.05*trshaderFlowFres)));
  trshaderFlowBody=trshaderHoldWaterTint(trshaderFlowBody,trshaderFlowTint,.10+trshaderMaterialFlowBody*.18);
  trshaderFlowBody=mix(trshaderFlowBody,trshaderFlowBody*vec3(.82,.98,1.12)+vec3(.000,.010,.024),
    trshaderSat(trshaderFlowSignal*.12+trshaderFlowDepth*.08));
 trshaderFlowBody+=vec3(.004,.028,.035)*(trshaderFlowSignal*.70+trshaderSurfacePulse*.20)*
   TR456_WATER_FLOW_STRENGTH*mix(.60,1.0,trshaderDepthBody);
  trshaderFlowBody=mix(trshaderFlowBody,
   trshaderFlowBody*mix(vec3(.94,1.02,1.02),vec3(1.05,.98,.92),trshaderPatchField.z),
   trshaderPatchField.x*.10);
  trshaderFlowBody=mix(trshaderFlowBody,trshaderFlowBody*vec3(.88,1.01,1.08)+trshaderFlowTint*.16,
    trshaderTensionPatch*.16);
   trshaderFlowBody=mix(trshaderFlowBody,trshaderFlowBody*vec3(.86,.97,1.06)+trshaderFlowTint*.14,
     trshaderVolumeWave.w*.10);
   trshaderFlowBody+=vec3(.018,.052,.058)*trshaderVolumeWave.z*(.30+.70*(1.0-trshaderFlowFres));
   trshaderFlowBody+=vec3(.006,.030,.038)*trshaderSmoothDeform.z*(.34+.66*(1.0-trshaderFlowFres));
   trshaderFlowBody=mix(trshaderFlowBody,trshaderFlowBody*vec3(.92,.98,1.04),
     trshaderSmoothDeform.w*.10);
   trshaderFlowBody+=vec3(.003,.018,.022)*trshaderRidgeMask;
 vec3 trshaderFoamColor=mix(vec3(.44,.60,.64),vec3(.72,.88,.94),trshaderSat(trshaderFlowDepth*.35+trshaderFlowFres*.45));
 vec3 trshaderCol=mix(trshaderFlowBody,trshaderReflected,trshaderReflMask*.20);
  trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderFoamMask*.52+trshaderAeration*.18+trshaderEdgeTurb*.20+
    trshaderEdgeMicroFoam*.055);
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderSat(trshaderBankTongue*.16+trshaderBankLace*.08)*TR456_WATER_FLOW_EDGE_FOAM);
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderDirectionalFoam*.20);
  trshaderCol+=trshaderFoamColor*trshaderDirectionalFoam*.045+
    vec3(.020,.055,.064)*trshaderTensionPatch*(.35+.65*trshaderFlowFres);
  trshaderCol+=vec3(.008,.030,.036)*abs(trshaderEdgeMicroWave)*(.45+.55*(1.0-trshaderFlowFres));
 trshaderCol+=vec3(.34,.56,.70)*(trshaderGlint*.70+trshaderStreakGlint*.95)+
    vec3(.10,.24,.34)*(trshaderMicroChop.z*.10+trshaderRefrStreak.z*.18+trshaderEdgeTurb*.20+
      trshaderBankTongue*.10)+
   vec3(.10,.22,.20)*trshaderTensionGlint;
 vec3 trshaderLight=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.68),vec3(1.24)),.24);
 trshaderCol*=trshaderLight;
 trshaderCol=(trshaderCol-.5)*1.055+.5;
 float trshaderFlowCompositeAlpha=clamp(.37+trshaderOpacity*.32+trshaderFlowDepth*.06+
   trshaderFlowSignal*.055+trshaderFoamMask*.14+trshaderDirectionalFoam*.08+trshaderAeration*.055+
   trshaderEdgeTurb*.07+trshaderEdgeMicroFoam*.04+trshaderReflMask*.08,.33,.64);
 trshaderFlowCompositeAlpha*=mix(1.0,.88,trshaderDuplicatePass);
 trshaderFlowCompositeAlpha=mix(trshaderFlowCompositeAlpha,.45,trshaderSettleMask*.35);
 return vec4(clamp(trshaderCol,0.0,1.0),trshaderFlowCompositeAlpha);
}

vec4 trshaderRenderCalmFlowSurface(TrshaderSyntheticFrame trshaderF, vec2 trshaderFlowDir, vec2 trshaderFlowSide,
                           vec2 trshaderFlowScreenDir, vec2 trshaderFlowScreenSide,
                           float trshaderFlowSpeed, float trshaderDuplicatePass,
                           float trshaderFlowTime, float trshaderGameTravel,
                           float trshaderSettledWarp,
                           TrshaderFlowCore trshaderCore){
 float trshaderPassOpacity=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.0),trshaderDuplicatePass);
 vec2 trshaderFlowUv=trshaderCore.trshaderFlowUv;
 vec4 trshaderPatchField=trshaderCore.trshaderPatchField;
 vec4 trshaderPattern=trshaderCore.trshaderPattern;
 vec3 trshaderFlowField=trshaderCore.trshaderFlowField;
 vec3 trshaderMicroChop=trshaderCore.trshaderMicroChop;
 vec4 trshaderRefrStreak=trshaderCore.trshaderRefrStreak;
  vec4 trshaderVolumeWave=trshaderFlowVolumeWaveField(trshaderFlowUv,trshaderGameTravel*.86,trshaderFlowSpeed,trshaderPatchField);
  vec4 trshaderSmoothDeform=trshaderSmoothFlowDeformationField(vSynWorldPos,trshaderFlowDir,trshaderFlowSide,
    trshaderGameTravel*.86,trshaderFlowSpeed,.65);
  float trshaderReliefCtl=clamp(TR456_WATER_FLOW_RELIEF_STRENGTH,0.0,2.4);
  float trshaderRefractionReliefCtl=clamp(TR456_WATER_FLOW_REFRACTION_RELIEF,0.0,2.8);
   float trshaderSettleMask=smoothstep(.18,.92,trshaderSettledWarp);
 float trshaderTensionStrength=clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0);
 float trshaderTensionGate=smoothstep(.26,.84,
   trshaderValueNoise(trshaderFlowUv*vec2(2.0,5.0)+vec2(-trshaderGameTravel*.045,trshaderGameTravel*.018)));
 float trshaderTensionLineA=trshaderLineMask(trshaderFlowUv.x*4.8+trshaderPattern.z*.38-
   trshaderGameTravel*(.24+trshaderFlowSpeed*.070),5.2);
 float trshaderTensionLineB=trshaderLineMask(trshaderFlowUv.x*10.4+trshaderFlowUv.y*.70+trshaderPattern.x*.36-
   trshaderGameTravel*(.44+trshaderFlowSpeed*.11),16.0);
 float trshaderTensionFilm=trshaderSat((trshaderTensionLineA*.58+trshaderTensionLineB*.36)*trshaderTensionGate*
   (.68+.32*trshaderPattern.w))*trshaderTensionStrength;
  float trshaderCalmBreath=sin(trshaderFlowUv.x*11.0+trshaderFlowUv.y*.90-trshaderGameTravel*2.75+trshaderPattern.z*1.1)*.5+.5;
  float trshaderFineBreath=sin(trshaderFlowUv.x*24.0+trshaderFlowUv.y*1.7-trshaderGameTravel*4.65)*.5+.5;
  float trshaderCrossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float trshaderLongWaveCtl=clamp(TR456_WATER_FLOW_LONGITUDINAL_WAVE,0.0,2.4);
  float trshaderTransWaveCtl=clamp(TR456_WATER_FLOW_TRANSVERSE_WAVE,0.0,2.4);
   float trshaderCalmCross=sin(trshaderFlowUv.y*15.0+trshaderFlowUv.x*1.2-trshaderGameTravel*(1.55+trshaderFlowSpeed*.14)+
    trshaderPattern.x*.7)*.5+.5;
  float trshaderCalmCrossFine=sin(trshaderFlowUv.y*31.0-trshaderFlowUv.x*.85-trshaderGameTravel*(2.85+trshaderFlowSpeed*.22))*.5+.5;
  float trshaderCrossDistortion=clamp(TR456_WATER_FLOW_CROSS_DISTORTION,0.0,3.0);
  float trshaderCalmCrossShearA=sin(trshaderFlowUv.y*36.0+trshaderFlowUv.x*2.9-
    trshaderGameTravel*(1.85+trshaderFlowSpeed*.18)+trshaderPattern.y*1.1);
  float trshaderCalmCrossShearB=sin(trshaderFlowUv.y*62.0-trshaderFlowUv.x*4.4+
    trshaderGameTravel*(1.42+trshaderFlowSpeed*.14)+trshaderPattern.z*1.5);
  float trshaderCalmCrossCounter=sin(trshaderFlowUv.x*15.0-trshaderFlowUv.y*18.5+
    trshaderGameTravel*(2.55+trshaderFlowSpeed*.16)+trshaderPattern.x*1.3);
  float trshaderCalmCrossShear=(trshaderCalmCrossShearA*.58+trshaderCalmCrossShearB*.42)*
    trshaderCrossStrength*trshaderCrossDistortion*trshaderTransWaveCtl;
  float trshaderCalmCrossRefract=(trshaderCalmCrossShear*.70+trshaderCalmCrossCounter*.30*
    trshaderCrossStrength*trshaderCrossDistortion*trshaderTransWaveCtl);
   float trshaderFineBreath3=trshaderFineBreath*trshaderFineBreath*trshaderFineBreath;
   float trshaderCalmCross2=trshaderCalmCross*trshaderCalmCross;
   float trshaderCalmCrossFine3=trshaderCalmCrossFine*trshaderCalmCrossFine*trshaderCalmCrossFine;
   float trshaderBreathBand=.76+.24*pow(trshaderCalmBreath,1.6);
   vec2 trshaderFlowSlopeWorld=trshaderFlowDir*trshaderFlowField.x+trshaderFlowSide*trshaderFlowField.y;
   vec2 trshaderVolumeSlopeWorld=(trshaderFlowDir*trshaderVolumeWave.x+trshaderFlowSide*trshaderVolumeWave.y)*trshaderReliefCtl;
   trshaderSmoothDeform*=mix(1.0,.62,trshaderSettleMask);
   vec2 trshaderSmoothDeformWorld=(trshaderFlowDir*trshaderSmoothDeform.x+trshaderFlowSide*trshaderSmoothDeform.y)*trshaderReliefCtl;
   float trshaderFlowContactStrength=clamp(TR456_WATER_FLOW_CONTACT_STRENGTH,0.0,3.0)*
     TR_TOGGLE_CONTACT_RIPPLES;
   float trshaderFlowContactNormal=clamp(TR456_WATER_FLOW_CONTACT_NORMAL,0.0,3.0);
   float trshaderFlowRippleMix=clamp(TR456_WATER_FLOW_CONTACT_RIPPLES,0.0,1.0);
   vec2 trshaderCalmContactSlope=(trshaderF.trshaderFlowContacts.xy*.72+
     (trshaderF.trshaderContacts.xy*.45+trshaderF.trshaderContactWake.xy*.42)*trshaderFlowRippleMix)*
     trshaderFlowContactStrength;
   float trshaderCalmContactEnergy=trshaderSat((trshaderF.trshaderFlowContacts.z*.62+
     (trshaderF.trshaderContacts.z*.30+trshaderF.trshaderContactWake.z*.58+
     trshaderF.trshaderContactWake.w*.44)*trshaderFlowRippleMix)*trshaderFlowContactStrength);
   vec2 trshaderCalmSlope=trshaderF.trshaderBaseField.xy*.48+trshaderF.trshaderRelief.xy*.26+trshaderF.trshaderAlive.xy*.34+
      trshaderFlowSlopeWorld*.42*trshaderBreathBand+
      trshaderVolumeSlopeWorld*.72+
      trshaderSmoothDeformWorld*.66+
      trshaderFlowDir*((trshaderPattern.w*.10+trshaderMicroChop.x*.62+trshaderRefrStreak.x*.60+
       trshaderTensionFilm*.20+trshaderFineBreath3*.055+
      (trshaderCalmCross2-.34)*.038*trshaderCrossStrength)*trshaderLongWaveCtl*trshaderReliefCtl)+
    trshaderFlowSide*(((trshaderPattern.x-trshaderPattern.z)*.030+trshaderMicroChop.y*.48+trshaderRefrStreak.y*.46+
      (pow(trshaderCalmCross,1.7)-.35)*.120*trshaderCrossStrength+
       (trshaderCalmCrossFine3-.25)*.052*trshaderCrossStrength+
       trshaderCalmCrossShear*.012+trshaderCalmCrossRefract*.008+
       (trshaderTensionLineB-.5)*trshaderTensionFilm*.055)*trshaderTransWaveCtl*trshaderReliefCtl)+
    trshaderCalmContactSlope;
  vec2 trshaderCalmNormalSlope=trshaderCalmSlope+trshaderCalmContactSlope*.34*trshaderFlowContactNormal;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
  float trshaderCalmBumpAmount=TR456_WATER_BUMP_STRENGTH*
    TR456_WATER_SYNTHETIC_BUMP_STRENGTH*TR456_WATER_FLOW_BUMP_STRENGTH;
  trshaderCalmNormalSlope+=trshaderSyntheticBumpSlope(
   trshaderFlowDir*(trshaderMicroChop.x*1.05+trshaderRefrStreak.x*.95+trshaderPattern.w*.070+
      trshaderTensionFilm*.085+trshaderFineBreath3*.030+trshaderSmoothDeform.x*.34+trshaderVolumeWave.x*.25)+
   trshaderFlowSide*(trshaderMicroChop.y*.95+trshaderRefrStreak.y*.88+
      (trshaderPattern.x-trshaderPattern.z)*.050+
      (pow(trshaderCalmCross,1.7)-.35)*.055*trshaderCrossStrength+
       trshaderCalmCrossShear*.026+trshaderSmoothDeform.y*.30+trshaderVolumeWave.y*.22)+
   trshaderTextureMicroBump(vSynWorldPos.xz,trshaderF.trshaderTime,
     trshaderFlowDir,.65)*.82,
    trshaderCalmBumpAmount*.82*mix(1.0,1.22,trshaderSat(trshaderReliefCtl-1.0)),.44);
#endif
 vec3 trshaderCalmNormal=normalize(vec3(-trshaderCalmNormalSlope.x,1.0,-trshaderCalmNormalSlope.y));
 float trshaderCalmNdv=trshaderSat(abs(dot(trshaderCalmNormal,trshaderF.trshaderViewDir)));
 float trshaderCalmFres=pow(1.0-trshaderCalmNdv,2.50)*TR456_WATER_FRESNEL_STRENGTH;
 float trshaderFlowSignal=trshaderSat(trshaderPattern.x*.26+trshaderPattern.w*.18+abs(trshaderFlowField.z)*.25+
     trshaderMicroChop.z*.28+trshaderRefrStreak.z*.18+trshaderVolumeWave.z*.30*trshaderReliefCtl+
     trshaderVolumeWave.w*.11*trshaderReliefCtl+trshaderTensionFilm*.30+trshaderFineBreath3*.12+
     (trshaderCalmCross2*.18+trshaderCalmCrossFine3*.10)*trshaderCrossStrength+
     trshaderSmoothDeform.z*.22*trshaderReliefCtl+trshaderSmoothDeform.w*.09*trshaderReliefCtl+
     abs(trshaderCalmCrossShear)*.022+abs(trshaderCalmCrossRefract)*.014+
     trshaderCalmContactEnergy*.16+
     trshaderPatchField.x*.070+trshaderPatchField.y*.14);
  trshaderFlowSignal*=mix(1.0,.62,trshaderSettleMask);

  vec2 trshaderLongPull=trshaderFlowScreenDir*(trshaderPattern.w*.018+trshaderFlowSignal*.014+
    trshaderRefrStreak.z*.014+trshaderVolumeWave.z*.014+trshaderTensionFilm*.020+
    trshaderCalmBreath*trshaderCalmBreath*.010+trshaderSmoothDeform.z*.010+
    trshaderSmoothDeform.x*.0035)*trshaderLongWaveCtl;
  vec2 trshaderCrossPull=trshaderFlowScreenSide*((trshaderPattern.x-trshaderPattern.z)*.0045+
    trshaderCalmSlope.y*.0038+(trshaderTensionLineB-.5)*trshaderTensionFilm*.014+
    (trshaderCalmCross-.5)*.013*trshaderCrossStrength+trshaderCalmCrossRefract*.0048)*trshaderTransWaveCtl;
  vec2 trshaderCrossWeave=trshaderFlowScreenSide*(trshaderCalmCrossShear*.0038+
    trshaderCalmCrossCounter*trshaderCrossStrength*trshaderCrossDistortion*.0018)+
    trshaderFlowScreenDir*(trshaderCalmCrossCounter*trshaderCrossStrength*trshaderCrossDistortion*.0015*trshaderLongWaveCtl);
  trshaderCrossWeave*=trshaderTransWaveCtl;
  vec2 trshaderReliefPull=trshaderFlowScreenDir*(trshaderVolumeWave.x*.022+trshaderSmoothDeform.x*.016+
    trshaderFlowField.x*.0038+trshaderMicroChop.x*.0045)*trshaderLongWaveCtl+
    trshaderFlowScreenSide*(trshaderVolumeWave.y*.018+trshaderSmoothDeform.y*.014+
    trshaderFlowField.y*.0034+trshaderMicroChop.y*.0042)*trshaderTransWaveCtl;
  trshaderReliefPull*=trshaderReliefCtl*trshaderRefractionReliefCtl;
  vec2 trshaderContactPull=trshaderFlowScreenDir*(trshaderCalmContactSlope.x*.0060+
    trshaderCalmContactEnergy*.0085)+trshaderFlowScreenSide*(trshaderCalmContactSlope.y*.0042);
  vec2 trshaderFlowWarp=(trshaderFlowScreenDir*(trshaderCalmSlope.x*.0058+trshaderRefrStreak.x*.54+
      trshaderMicroChop.x*.18)*trshaderLongWaveCtl+trshaderFlowScreenSide*(trshaderRefrStreak.y*.44+
      trshaderMicroChop.y*.16+trshaderSmoothDeform.y*.016)*trshaderTransWaveCtl+
      trshaderLongPull*.82+trshaderCrossPull*.64+trshaderCrossWeave*.68+
      trshaderReliefPull*.60+trshaderContactPull*.72)*
   clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,2.6)*
   clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,3.6)*
   TR_TOGGLE_FLOW_WARP*mix(1.0,.12,trshaderSettleMask);
 trshaderFlowWarp=trshaderSoftLimitVec2(trshaderFlowWarp,.082);
 vec2 trshaderChromaWarp=trshaderFlowWarp*(.25+.16*TR456_WATER_FLOW_CHROMA);
 vec3 trshaderSceneA=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp,vec2(.001),vec2(.999))).rgb;
 float trshaderChroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA*
   TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
 vec3 trshaderCalmRefractedSource=trshaderSceneA;
 if(trshaderChroma>.001) {
   vec3 trshaderSceneR=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp+trshaderChromaWarp,
     vec2(.001),vec2(.999))).rgb;
   vec3 trshaderSceneB=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderFlowWarp-trshaderChromaWarp,
     vec2(.001),vec2(.999))).rgb;
   trshaderCalmRefractedSource=mix(trshaderSceneA,vec3(trshaderSceneR.r,trshaderSceneA.g,trshaderSceneB.b),.24*trshaderChroma);
 }
 vec3 trshaderRefracted=trshaderOriginalWaterGrade(trshaderCalmRefractedSource);

 float trshaderOpacity=clamp(uTrWaterSyntheticInfo.x*TR456_WATER_FLOW_OPACITY*
   (.78+trshaderFlowSignal*.12)*trshaderPassOpacity,.010,.58);
 float trshaderTintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
 float trshaderDepthCue=0.0;
   float trshaderSceneFloorDepth=trshaderSat(((1.0-trshaderLuma(trshaderRefracted))*.24+trshaderDepthCue*.18+
     trshaderFlowSignal*.045+trshaderOpacity*.020)*TR456_WATER_DEPTH_STRENGTH*
     (.42+.34*TR456_WATER_FLOW_DEPTH_BODY));
   float trshaderMaterialFloorBody=trshaderStableWaterBody(trshaderOpacity,
     trshaderFlowSignal*.72+trshaderPattern.w*.20+trshaderMicroChop.z*.18+trshaderTensionFilm*.20,
     trshaderF.trshaderShoreline*.24);
  float trshaderFloorDepth=max(trshaderSceneFloorDepth,trshaderMaterialFloorBody*.24);
  float trshaderDepthBody=smoothstep(.16,.82,trshaderFloorDepth);
  float trshaderDepthOpacity=trshaderDepthAwareOpacity(trshaderOpacity,trshaderFloorDepth);
   vec3 trshaderTint=mix(vec3(.018,.130,.116),vec3(.004,.038,.044),
     trshaderSat(trshaderFloorDepth+trshaderDepthCue*.28))*trshaderTintStrength;
   trshaderRefracted=trshaderWaterVolume(trshaderRefracted,
    max(trshaderSceneFloorDepth,trshaderMaterialFloorBody*.14)*(.44+.44*trshaderDepthBody),trshaderCalmNdv,
    trshaderTint*mix(.50,1.0,trshaderDepthBody));

 float trshaderReflectAmt=clamp(uTrWaterSyntheticInfo.z*TR456_WATER_FLOW_REFLECTION*
   TR456_WATER_REFLECT_STRENGTH*.16*clamp(uTrWaterSyntheticProfile.w,.05,1.0),
   0.0,1.0);
 vec3 trshaderReflected=trshaderTint;
 float trshaderReflectionMask=0.0;
#if TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED
 float trshaderReflectActive=trshaderReflectAmt;
 if(trshaderReflectActive>.001) {
  vec2 trshaderReflectWarp=vec2(-trshaderCalmSlope.x*.006+trshaderCalmNormal.x*.008,
    .035+trshaderCalmFres*.052-trshaderCalmSlope.y*.004);
  vec3 trshaderSceneRefl=trshaderStableSceneColor(trshaderF.trshaderScreen+trshaderReflectWarp,trshaderF.trshaderScreen)*.58+
    trshaderStableSceneColor(trshaderF.trshaderScreen+trshaderReflectWarp*.42+vec2(0.0,.026),
      trshaderF.trshaderScreen)*.42;
#if TR456_WATER_REFLECTION_QUALITY > 1
  vec2 trshaderMirrorUv0=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderCalmNormal,trshaderF.trshaderViewDir,
    trshaderReflectWarp*.55,0.0,.36+.42*trshaderCalmFres);
  vec2 trshaderMirrorUv1=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderCalmNormal,trshaderF.trshaderViewDir,
    trshaderReflectWarp*.24+vec2(0.0,.030),.008+.016*trshaderCalmFres,
    .28+.32*trshaderCalmFres);
  vec3 trshaderMirrorRefl=trshaderStableSceneColor(trshaderMirrorUv0,trshaderF.trshaderScreen)*.62+
    trshaderStableSceneColor(trshaderMirrorUv1,trshaderF.trshaderScreen)*.38;
  float trshaderFilmMirror=trshaderSat((trshaderTensionFilm*.34+trshaderRefrStreak.z*.24+
    trshaderMicroChop.z*.16+trshaderVolumeWave.z*.16+trshaderFlowSignal*.12)*
    (.52+trshaderCalmFres*.48));
  vec2 trshaderFilmUv=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderCalmNormal,
    trshaderF.trshaderViewDir,
    trshaderReflectWarp*.80+trshaderFlowScreenDir*(trshaderRefrStreak.x*.12)+
    trshaderFlowScreenSide*(trshaderRefrStreak.y*.12),.004+.008*trshaderFilmMirror,
    .46+.30*trshaderCalmFres);
  trshaderMirrorRefl=mix(trshaderMirrorRefl,
    trshaderStableSceneColor(trshaderFilmUv,trshaderF.trshaderScreen),
    trshaderFilmMirror*.20*TR_TOGGLE_FLOW_REFLECTION);
  trshaderReflected=trshaderReflectionGrade(mix(trshaderSceneRefl,trshaderMirrorRefl,
    trshaderSat(.18+trshaderCalmFres*.44+trshaderFloorDepth*.10+trshaderFilmMirror*.08)));
  trshaderReflectionMask=trshaderSat((.026+trshaderCalmFres*.145+trshaderFloorDepth*.020+
    trshaderFilmMirror*.014)*
    trshaderReflectActive*mix(.60,1.0,trshaderReflectionUvFade(trshaderMirrorUv0)));
#else
  trshaderReflected=trshaderReflectionGrade(trshaderSceneRefl);
  trshaderReflectionMask=trshaderSat((.022+trshaderCalmFres*.118+
    trshaderFloorDepth*.016)*trshaderReflectActive*
    mix(.60,1.0,trshaderReflectionUvFade(trshaderF.trshaderScreen+
    trshaderReflectWarp)));
#endif
 }
#endif

 float trshaderShoreEdge=trshaderSat(trshaderF.trshaderShoreline*TR456_WATER_SHORELINE_STRENGTH*
   (.50+.50*(1.0-trshaderFloorDepth))*TR_TOGGLE_SURFACE_FOAM);
  float trshaderCalmStress=trshaderSat(trshaderShoreEdge*.92+trshaderF.trshaderContactWake.w*.55+trshaderCalmContactEnergy*.22+
    trshaderMicroChop.z*.22+
    trshaderRefrStreak.z*.16+trshaderTensionFilm*.26+trshaderPatchField.y*.58+trshaderFlowSignal*.12);
  float trshaderFoamGate=trshaderStressFoamGate(trshaderCalmStress);
  float trshaderFoamMask=trshaderSat((trshaderShoreEdge*.42+trshaderF.trshaderContactWake.w*.18+trshaderCalmContactEnergy*.08+
    trshaderPattern.y*.045)*
    TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*trshaderFoamGate;
  float trshaderDirectionalFoam=trshaderSat((pow(trshaderPattern.w,1.35)*.22+trshaderPattern.y*.16+
    trshaderShoreEdge*.18+trshaderRefrStreak.z*.08)*
   smoothstep(.12,.78,trshaderFlowSignal+trshaderShoreEdge*.30)*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*trshaderFoamGate;
 float trshaderTensionPatch=trshaderSat((trshaderTensionFilm*.55+trshaderPatchField.y*.24+
   trshaderFlowSignal*.12)*TR456_WATER_FLOW_SURFACE_TENSION*
   (1.0-trshaderFoamMask*.30));
 trshaderFoamMask=trshaderSat(trshaderFoamMask+trshaderDirectionalFoam*.24);
  float trshaderFilmGlint=pow(trshaderSat(trshaderTensionFilm),1.55)*(.10+trshaderCalmFres*.18)*
    TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
   vec3 trshaderWaterBase=trshaderRefracted*mix(1.015,.94,trshaderDepthOpacity)+
     trshaderTint*(.018+.050*trshaderDepthOpacity)*mix(.55,1.0,trshaderDepthBody);
   trshaderWaterBase=trshaderHoldWaterTint(trshaderWaterBase,trshaderTint,.11+trshaderMaterialFloorBody*.17);
   trshaderWaterBase=mix(trshaderWaterBase,trshaderWaterBase*vec3(.90,1.01,1.07)+trshaderTint*.14,
     trshaderTensionPatch*.12);
   trshaderWaterBase=mix(trshaderWaterBase,trshaderWaterBase*vec3(.88,.98,1.05)+trshaderTint*.12,
     trshaderVolumeWave.w*.075);
   trshaderWaterBase+=vec3(.014,.042,.046)*trshaderVolumeWave.z*(.28+.62*(1.0-trshaderCalmFres));
   trshaderWaterBase+=vec3(.004,.022,.028)*trshaderSmoothDeform.z*(.30+.60*(1.0-trshaderCalmFres));
   trshaderWaterBase=mix(trshaderWaterBase,trshaderWaterBase*vec3(.94,.99,1.035),
     trshaderSmoothDeform.w*.075);
  vec3 trshaderWaterBody=mix(trshaderWaterBase,trshaderReflected*.86+trshaderTint*.040,trshaderReflectionMask*.20);
 trshaderWaterBody=mix(trshaderWaterBody,
   trshaderWaterBody*mix(vec3(.95,1.02,1.02),vec3(1.04,.99,.93),trshaderPatchField.z),
   trshaderPatchField.x*.085);
 vec3 trshaderFoamColor=mix(vec3(.42,.58,.58),vec3(.70,.86,.86),
   trshaderSat(trshaderFloorDepth*.30+trshaderCalmFres*.42+trshaderShoreEdge*.50));
  vec3 trshaderCol=trshaderWaterBody+
    vec3(.012,.052,.052)*(trshaderFlowSignal*.30+trshaderTensionFilm*.16)+
    vec3(.08,.20,.18)*trshaderFilmGlint+
    vec3(.03,.08,.09)*(trshaderMicroChop.z*.08+trshaderRefrStreak.z*.13);
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderFoamMask*.28);
 trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderDirectionalFoam*.14);
 trshaderCol+=trshaderFoamColor*trshaderDirectionalFoam*.030+
   vec3(.016,.044,.052)*trshaderTensionPatch*(.28+.52*trshaderCalmFres);
 vec3 trshaderLight=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.20)),.24);
 trshaderCol*=trshaderLight;
 trshaderCol=(trshaderCol-.5)*1.035+.5;
 float trshaderCalmCompositeAlpha=clamp(.32+trshaderOpacity*.30+trshaderFlowSignal*.070+
   trshaderShoreEdge*.06+trshaderFoamMask*.10+trshaderDirectionalFoam*.05+trshaderTensionPatch*.05+
   trshaderReflectionMask*.06,.28,.55);
 trshaderCalmCompositeAlpha=mix(trshaderCalmCompositeAlpha,.42,trshaderSettleMask*.40);
 trshaderCalmCompositeAlpha*=mix(1.0,.90,trshaderDuplicatePass);
 return vec4(clamp(trshaderCol,0.0,1.0),trshaderCalmCompositeAlpha);
}

vec4 trshaderRenderStandingWater(TrshaderSyntheticFrame trshaderF){
  vec2 trshaderPrimaryDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
  vec2 trshaderSideDir=vec2(-trshaderPrimaryDir.y,trshaderPrimaryDir.x);
  vec2 trshaderCrossDir=normalize(trshaderPrimaryDir*.56+trshaderSideDir*.83);
  vec2 trshaderScreenDir=normalize(vec2(trshaderPrimaryDir.x,-trshaderPrimaryDir.y)+vec2(.0001,.0003));
  vec2 trshaderScreenSide=vec2(-trshaderScreenDir.y,trshaderScreenDir.x);
  vec4 trshaderUnderlay=trshaderUnderlayPattern(trshaderF.trshaderScreen,
    trshaderScreenDir,trshaderScreenSide);
  float trshaderRefractAmt=clamp(TR456_WATER_REFRACT_STRENGTH*
    TR456_WATER_REFRACTION_WAVE_STRENGTH,0.55,3.25);
 float trshaderViewWarp=1.22;
 vec2 trshaderWarp=(trshaderF.trshaderSlope*.0114+trshaderF.trshaderNormal.xz*.0078+
   trshaderF.trshaderRelief.xy*.00325+trshaderF.trshaderAlive.xy*.00125+
   trshaderF.trshaderStandingLife.xy*.0038)*trshaderRefractAmt*trshaderViewWarp;
 trshaderWarp+=trshaderScreenDir*(trshaderUnderlay.y*.006+trshaderUnderlay.x*.0018)+
   trshaderScreenSide*(trshaderUnderlay.z*.005);
 vec3 trshaderSceneA=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen+trshaderWarp,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderSceneB=texture(uTrWaterScene,clamp(trshaderF.trshaderScreen-trshaderWarp*.72,vec2(.001),vec2(.999))).rgb;
 vec3 trshaderRefracted=trshaderOriginalWaterGrade(mix(trshaderSceneA,trshaderSceneB,.42));

 float trshaderOpacity=clamp(uTrWaterSyntheticInfo.x,0.0,1.0);
 float trshaderTintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
  float trshaderReflectAmt=clamp(uTrWaterSyntheticInfo.z*TR456_WATER_REFLECT_STRENGTH*
    clamp(uTrWaterSyntheticProfile.w,.05,1.2),0.0,2.0);
 float trshaderDepthCue=0.0;
 vec3 trshaderShallow=vec3(.020,.155,.170);
 vec3 trshaderDeep=vec3(.006,.046,.060);
  vec3 trshaderTint=mix(trshaderShallow,trshaderDeep,trshaderDepthCue)*trshaderTintStrength;
   float trshaderFloorDepth=trshaderSat(((1.0-trshaderLuma(trshaderRefracted))*.30+trshaderDepthCue*.24+trshaderOpacity*.10)*
     TR456_WATER_DEPTH_STRENGTH);
  float trshaderDepthBody=smoothstep(.16,.82,trshaderFloorDepth);
  float trshaderDepthOpacity=trshaderDepthAwareOpacity(trshaderOpacity,trshaderFloorDepth);
  float trshaderBaseMurk=trshaderSat(.18+trshaderOpacity*.42+trshaderFloorDepth*.38+
    (1.0-trshaderF.trshaderFresnel)*.16);
  vec3 trshaderMurkTint=mix(vec3(.025,.046,.045),vec3(.080,.106,.094),
    trshaderSat(trshaderFloorDepth*.85+trshaderOpacity*.35))*trshaderTintStrength;
  trshaderRefracted=mix(trshaderRefracted,trshaderRefracted*vec3(.74,.86,.84)+trshaderMurkTint,
    trshaderBaseMurk*.28);
   trshaderRefracted=trshaderWaterVolume(trshaderRefracted,trshaderFloorDepth*(.55+.45*trshaderDepthBody),trshaderF.trshaderNdv,
     trshaderTint*mix(.52,1.0,trshaderDepthBody));
  float trshaderShoreEdge=trshaderSat(trshaderF.trshaderShoreline*TR456_WATER_SHORELINE_STRENGTH*
    (.58+.42*(1.0-trshaderFloorDepth))*TR_TOGGLE_SURFACE_FOAM);
  float trshaderEdgeWaveStrength=mix(.75,1.25,clamp(TR456_WATER_EDGE_WAVE,0.0,1.0));
  float trshaderShorePulse=pow(trshaderSat(sin(dot(vSynWorldPos.xz,trshaderPrimaryDir)*.020+
    trshaderF.trshaderTime*1.18+trshaderF.trshaderShoreline*2.1)*.5+.5),2.2);
  float trshaderShoreReturn=pow(trshaderSat(sin(dot(vSynWorldPos.xz,trshaderPrimaryDir)*.010-
    sin(dot(vSynWorldPos.xz,trshaderSideDir)*.022-trshaderF.trshaderTime*.16)*.24+
    trshaderF.trshaderTime*.33)*.5+.5),3.2);
  float trshaderShoreLap=pow(trshaderSat(sin(dot(vSynWorldPos.xz,trshaderPrimaryDir)*.014+
    sin(dot(vSynWorldPos.xz,trshaderSideDir)*.018+trshaderF.trshaderTime*.22)*.32-
    trshaderF.trshaderTime*.54)*.5+.5),2.6)*trshaderShoreEdge*(.52+.42*trshaderShorePulse)+
    trshaderShoreReturn*trshaderShoreEdge*.18;
  trshaderShoreLap*=trshaderEdgeWaveStrength;
  float trshaderWakeFoam=trshaderSat(trshaderF.trshaderContactWake.w*TR456_WATER_FOAM_STRENGTH*
    TR456_WATER_CONTACT_EDGE*TR_TOGGLE_CONTACT_RIPPLES);

 vec3 trshaderReflected=trshaderTint;
 float trshaderReflectionMask=0.0;
#if TR456_WATER_SYNTHETIC_REFLECTION_ENABLED
 vec2 trshaderReflectionWarp=vec2(-trshaderF.trshaderSlope.x*.0075+trshaderF.trshaderNormal.x*.010,
                          .060+trshaderF.trshaderFresnel*.100-trshaderF.trshaderSlope.y*.006);
 trshaderReflectionWarp+=trshaderScreenDir*(trshaderUnderlay.y*.008+trshaderUnderlay.w*.002)+
   trshaderScreenSide*(trshaderUnderlay.z*.006);
 float trshaderShimmerAmt=clamp(TR456_WATER_REFLECTION_SHIMMER,0.0,1.0);
 vec2 trshaderShimmer=vec2(
   sin(dot(vSynWorldPos.xz,trshaderPrimaryDir)*.031+trshaderF.trshaderTime*1.18+
     trshaderF.trshaderAlive.z*1.7+trshaderF.trshaderStandingLife.z*.8),
   sin(dot(vSynWorldPos.xz,trshaderSideDir)*.026-trshaderF.trshaderTime*.92+
     trshaderF.trshaderRelief.z*.9+trshaderF.trshaderStandingLife.w*.7));
 trshaderShimmer+=vec2(trshaderValueNoise(vSynWorldPos.xz*.0018+
     vec2(trshaderF.trshaderTime*.018,-trshaderF.trshaderTime*.012))-.5,
   trshaderValueNoise(vSynWorldPos.xz*.0022+
     vec2(-trshaderF.trshaderTime*.010,trshaderF.trshaderTime*.016))-.5)*1.6;
 trshaderShimmer*=trshaderShimmerAmt*(.0018+.0038*trshaderF.trshaderFresnel)*
   (1.0-trshaderShoreEdge*.18);
 trshaderReflectionWarp+=trshaderShimmer;
 vec2 trshaderReflectUv0=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   trshaderReflectionWarp,0.0,.34+.34*trshaderF.trshaderFresnel);
 vec2 trshaderReflectUv1=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   vec2(trshaderReflectionWarp.x*.55,trshaderReflectionWarp.y*1.70+.035),
   .010+.014*trshaderF.trshaderFresnel,.30+.30*trshaderF.trshaderFresnel);
 vec2 trshaderReflectUv2=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   vec2(trshaderReflectionWarp.x*1.45,trshaderReflectionWarp.y*.72),
   0.0,.28+.26*trshaderF.trshaderFresnel);
 trshaderReflected=trshaderStableSceneColor(trshaderReflectUv0,trshaderF.trshaderScreen)*.52+
                trshaderStableSceneColor(trshaderReflectUv1,trshaderF.trshaderScreen)*.28+
                trshaderStableSceneColor(trshaderReflectUv2,trshaderF.trshaderScreen)*.20;
#if TR456_WATER_REFLECTION_QUALITY > 1
 vec2 trshaderMirrorWarp=vec2(trshaderF.trshaderSlope.x*.011+trshaderF.trshaderNormal.x*.013,
                      -abs(trshaderF.trshaderSlope.y)*.006+trshaderF.trshaderNormal.z*.008);
 trshaderMirrorWarp+=trshaderScreenDir*(trshaderUnderlay.y*.010+trshaderUnderlay.x*.002)+
   trshaderScreenSide*(trshaderUnderlay.z*.008);
 trshaderMirrorWarp+=trshaderShimmer*(1.15+.35*trshaderF.trshaderFresnel);
 vec2 trshaderMirrorUv0=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   trshaderMirrorWarp,0.0,.40+.42*trshaderF.trshaderFresnel);
 vec2 trshaderMirrorUv1=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   trshaderMirrorWarp*.55+vec2(trshaderF.trshaderSlope.y*.003,.035+trshaderF.trshaderFresnel*.055),
   .010+.018*trshaderF.trshaderFresnel,.34+.34*trshaderF.trshaderFresnel);
 vec2 trshaderMirrorUv2=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,trshaderF.trshaderViewDir,
   trshaderMirrorWarp*1.42+vec2(-trshaderF.trshaderSlope.y*.002,-.030),
   0.0,.30+.28*trshaderF.trshaderFresnel);
 vec3 trshaderMirrorRef=trshaderStableSceneColor(trshaderMirrorUv0,trshaderF.trshaderScreen)*.55+
                trshaderStableSceneColor(trshaderMirrorUv1,trshaderF.trshaderScreen)*.30+
                trshaderStableSceneColor(trshaderMirrorUv2,trshaderF.trshaderScreen)*.15;
 float trshaderSheetMirror=trshaderSat((trshaderF.trshaderAlive.z*.24+
   abs(trshaderF.trshaderRelief.z)*.18+trshaderF.trshaderRainRipples.z*.16+
   trshaderF.trshaderWaterfallWaves.z*.20+trshaderF.trshaderStandingLife.z*.24+
   trshaderF.trshaderStandingLife.w*.16+trshaderUnderlay.x*.22+
   trshaderUnderlay.w*.12)*(0.45+trshaderF.trshaderFresnel*.55));
 vec2 trshaderSheetUv=trshaderPreciseReflectionUv(trshaderF.trshaderScreen,trshaderF.trshaderNormal,
   trshaderF.trshaderViewDir,
   trshaderMirrorWarp*1.12+trshaderPrimaryDir*(trshaderF.trshaderAlive.x*.006)+
   trshaderSideDir*(trshaderF.trshaderRelief.y*.004)+
   trshaderF.trshaderStandingLife.xy*.004,
   .006+.012*trshaderSheetMirror,.48+.28*trshaderF.trshaderFresnel);
 trshaderMirrorRef=mix(trshaderMirrorRef,
   trshaderStableSceneColor(trshaderSheetUv,trshaderF.trshaderScreen),
   trshaderSheetMirror*.18);
 float trshaderMirrorMask=trshaderSat(.30+trshaderF.trshaderFresnel*.58+trshaderFloorDepth*.16);
 trshaderReflected=trshaderReflectionGrade(mix(trshaderReflected,trshaderMirrorRef,
   trshaderSat(trshaderMirrorMask+trshaderSheetMirror*.08)));
 trshaderReflectionMask=trshaderSat((.052+trshaderF.trshaderFresnel*.27+
   trshaderFloorDepth*.036+trshaderSheetMirror*.012+trshaderUnderlay.x*.018)*
   trshaderReflectAmt*mix(.64,1.0,trshaderMirrorMask)*
   mix(.58,1.0,trshaderReflectionUvFade(trshaderMirrorUv0)));
#else
 trshaderReflected=trshaderReflectionGrade(trshaderReflected);
 trshaderReflectionMask=trshaderSat((.044+trshaderF.trshaderFresnel*.220+
   trshaderFloorDepth*.030+trshaderUnderlay.x*.014)*
   trshaderReflectAmt*mix(.58,1.0,trshaderReflectionUvFade(trshaderReflectUv0)));
#endif
#endif

  vec2 trshaderW=vSynWorldPos.xz;
  float trshaderRidgeA=trshaderFastPow13(trshaderSat(sin(dot(trshaderW,trshaderPrimaryDir)*.043+trshaderF.trshaderTime*2.05)*.5+.5));
 float trshaderRidgeB=trshaderFastPow18(trshaderSat(sin(dot(trshaderW,trshaderSideDir)*.055-trshaderF.trshaderTime*2.62)*.5+.5));
  float trshaderRidgeCross=sqrt(trshaderRidgeA*trshaderRidgeB);
  trshaderRidgeCross*=trshaderSat(sin(dot(trshaderW,trshaderCrossDir)*.032+trshaderF.trshaderTime*.72)*.5+.65);
 float trshaderRippleMemory=pow(trshaderSat(trshaderF.trshaderRainRipples.z+trshaderF.trshaderContacts.z*.40+
   trshaderF.trshaderContactWake.z*.30+trshaderF.trshaderWaterfallWaves.z*.18+
   trshaderF.trshaderStandingLife.z*.26),1.25);
 float trshaderCrest=trshaderSat(trshaderF.trshaderBaseField.z*.28+.30+trshaderF.trshaderAlive.z*.18)+
   trshaderF.trshaderContacts.z*.95+trshaderF.trshaderRainRipples.z*.70+trshaderF.trshaderContactWake.z*.42+
   trshaderF.trshaderWaterfallWaves.z*.62+trshaderF.trshaderStandingLife.z*.46+
   trshaderRippleMemory*.18;
  float trshaderReliefGrain=trshaderSat(abs(trshaderF.trshaderRelief.z)*.66+trshaderF.trshaderAlive.z*.22);
  float trshaderTensionA=trshaderFastPow5(trshaderSat(1.0-abs(fract(dot(trshaderW,trshaderPrimaryDir)*.018+
    sin(dot(trshaderW,trshaderSideDir)*.015+trshaderF.trshaderTime*.21)*.18+trshaderF.trshaderTime*.032)-.5)*2.0));
  float trshaderTensionB=trshaderFastPow5(trshaderSat(1.0-abs(fract(dot(trshaderW,trshaderCrossDir)*.024+
    trshaderF.trshaderAlive.z*.18-trshaderF.trshaderTime*.026)-.5)*2.0));
  float trshaderTensionFilm=trshaderSat((trshaderTensionA*.52+trshaderTensionB*.34+
    trshaderRidgeCross*.16+trshaderF.trshaderStandingLife.w*.40)*
    (.55+.45*(1.0-trshaderF.trshaderFresnel))*(.78+.22*trshaderReliefGrain));
  trshaderTensionFilm*=clamp(TR456_WATER_SURFACE_CAUSTIC,0.0,2.0);
  trshaderTensionFilm=trshaderSat(trshaderTensionFilm+trshaderUnderlay.x*.20+trshaderUnderlay.w*.08);
 float trshaderGlint=(trshaderRidgeA*.070+trshaderRidgeB*.056+trshaderRidgeCross*.150+trshaderReliefGrain*.052+
    trshaderF.trshaderAlive.z*.115+trshaderF.trshaderRainRipples.z*.052+trshaderRippleMemory*.030+
    trshaderF.trshaderWaterfallWaves.z*.082+trshaderF.trshaderStandingLife.z*.080+
    trshaderTensionFilm*.135+
    trshaderUnderlay.x*.070+trshaderUnderlay.w*.045+
    trshaderFastPow58(trshaderSat(dot(trshaderF.trshaderNormal,normalize(vec3(-.28,.92,.26)))))*.20)*
    (0.22+trshaderF.trshaderFresnel*.54)*trshaderReflectAmt*TR456_WATER_GLINT_STRENGTH*
    clamp(TR456_WATER_SURFACE_CAUSTIC,0.0,2.0);
 float trshaderPinA=trshaderFastPow48(trshaderSat(sin(dot(trshaderW,trshaderPrimaryDir)*.083+
   trshaderF.trshaderTime*3.14+trshaderF.trshaderRelief.z*1.7)*.5+.5));
 float trshaderPinB=trshaderFastPow18(trshaderSat(sin(dot(trshaderW,trshaderSideDir)*.071-
   trshaderF.trshaderTime*2.67+trshaderF.trshaderAlive.z*2.1)*.5+.5));
 float trshaderSparkleMask=trshaderPinA*trshaderPinB*
   trshaderSat(.20+trshaderF.trshaderFresnel*.86+trshaderReflectAmt*.20)*
   (0.42+0.58*trshaderReliefGrain);
 trshaderGlint+=trshaderSparkleMask*TR456_WATER_SPARKLE_STRENGTH*
   TR456_WATER_GLINT_STRENGTH*(.18+.36*trshaderReflectAmt);
 float trshaderContactLight=pow(trshaderSat(trshaderF.trshaderContacts.z+trshaderF.trshaderContactWake.z*.56),1.7)*
   .045*TR456_WATER_GLINT_STRENGTH*clamp(TR456_WATER_SURFACE_CAUSTIC,0.0,2.0);

  vec3 trshaderWaterBase=trshaderRefracted*mix(1.025,.90,trshaderDepthOpacity)+
    trshaderTint*(.036+.094*trshaderDepthOpacity)*mix(.55,1.0,trshaderDepthBody);
  trshaderWaterBase=mix(trshaderWaterBase,
    trshaderWaterBase*vec3(.82,.92,.90)+trshaderMurkTint*.34+vec3(.006,.012,.011),
    trshaderBaseMurk*.34);
  trshaderWaterBase=mix(trshaderWaterBase,trshaderWaterBase*vec3(.94,1.03,1.04)+trshaderTint*.10,
    trshaderTensionFilm*.16);
  trshaderWaterBase=mix(trshaderWaterBase,
    trshaderWaterBase*vec3(.90,.98,1.02)+trshaderTint*.10,
    trshaderUnderlay.x*.12);
 vec3 trshaderWaterBody=mix(trshaderWaterBase,trshaderReflected*.98+trshaderTint*.030,trshaderReflectionMask*.62);
  vec3 trshaderRim=vec3(.12,.22,.24)*trshaderF.trshaderFresnel*(.08+.22*trshaderReflectAmt);
  vec3 trshaderFoamColor=mix(vec3(.38,.56,.58),vec3(.70,.88,.90),
    trshaderSat(trshaderF.trshaderFresnel+trshaderFloorDepth*.35+trshaderShoreEdge*.60));
  vec3 trshaderSparkle=vec3(.34,.55,.58)*(trshaderGlint+trshaderContactLight);
 vec3 trshaderLight=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.22)),.26);
 vec3 trshaderShade=(vec3(.90,.98,1.00)+vec3(0.0,.050,.058)*trshaderCrest+
   vec3(.018,.024,.022)*trshaderReliefGrain+vec3(.018,.026,.024)*trshaderF.trshaderAlive.z)*trshaderLight;
 vec3 trshaderCol=(trshaderWaterBody+trshaderRim+trshaderSparkle)*trshaderShade;
  float trshaderShoreFoam=trshaderSat((trshaderShoreEdge*(.38+.48*trshaderShorePulse)+trshaderShoreLap*.30+
    trshaderWakeFoam*.72+
    trshaderF.trshaderContactWake.z*.030)*TR456_WATER_FOAM_STRENGTH);
  trshaderCol=mix(trshaderCol,trshaderFoamColor,trshaderShoreFoam*.42);
  trshaderCol+=trshaderFoamColor*(trshaderWakeFoam*.20+trshaderShoreEdge*.030*TR456_WATER_WET_EDGE+
    trshaderShoreLap*.040);
 trshaderCol+=vec3(.024,.058,.066)*trshaderRippleMemory*(.22+.34*(1.0-trshaderF.trshaderFresnel));
  trshaderCol+=vec3(.035,.070,.066)*trshaderTensionFilm*(.18+.32*(1.0-trshaderF.trshaderFresnel));
  trshaderCol+=vec3(.020,.060,.065)*trshaderF.trshaderStandingLife.z*
    (.18+.38*(1.0-trshaderF.trshaderFresnel));
  float trshaderReliefVein=pow(trshaderSat(abs(trshaderF.trshaderRelief.z)*1.90+trshaderF.trshaderAlive.z*.34+
    trshaderRidgeCross*.22),1.25)*clamp(TR456_WATER_BLUE_STRIPE,0.0,2.0);
  trshaderCol+=vec3(.034,.076,.084)*trshaderReliefVein*(.30+.56*(1.0-trshaderF.trshaderFresnel));
  float trshaderMistLine=trshaderFastPow5(trshaderSat(1.0-abs(fract(dot(trshaderW,vec2(.016,.011))+trshaderF.trshaderTime*.022)-.5)*2.0));
  float trshaderHaze=trshaderSat(trshaderFloorDepth*.24+(1.0-trshaderF.trshaderFresnel)*.075+
    trshaderMistLine*.010+trshaderOpacity*.085+trshaderBaseMurk*.13);
  vec3 trshaderHazeColor=mix(vec3(.065,.078,.076),vec3(.125,.150,.142),trshaderSat(trshaderFloorDepth+trshaderF.trshaderFresnel*.35))*trshaderTintStrength;
  trshaderCol=mix(trshaderCol,trshaderCol*vec3(.90,.95,.94)+trshaderHazeColor,trshaderHaze*.34);
  trshaderCol+=trshaderHazeColor*(trshaderMistLine*.010+trshaderF.trshaderContacts.z*.004);
 float trshaderContrast=1.045;
 trshaderCol=(trshaderCol-.5)*trshaderContrast+.5;
 trshaderCol=clamp(trshaderCol,0.0,1.0);
 float trshaderStandingCompositeAlpha=clamp(.24+trshaderOpacity*.50+
   trshaderShoreFoam*.10+trshaderWakeFoam*.06+trshaderGlint*.10+trshaderContactLight*.12+
   trshaderReflectionMask*.08+trshaderHaze*.05+trshaderTensionFilm*.035+
   trshaderF.trshaderStandingLife.z*.025,.30,.68);
 trshaderStandingCompositeAlpha=mix(trshaderStandingCompositeAlpha,.42,trshaderBaseMurk*.18);
 return vec4(trshaderCol,trshaderStandingCompositeAlpha);
}

void main(){
 vec2 trshaderInv=max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 trshaderScreen=gl_FragCoord.xy*trshaderInv;
 float trshaderT=uTrWaterSyntheticInfo.w;
 TrshaderSyntheticFrame trshaderF=trshaderBuildSyntheticFrame(trshaderScreen,trshaderT);

 if(trshaderSat(vSynFlowInfo.x)>.5){
  vec2 trshaderFlowDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
  vec2 trshaderFlowSide=vec2(-trshaderFlowDir.y,trshaderFlowDir.x);
  vec2 trshaderFlowScreenDir=normalize(vec2(trshaderFlowDir.x,-trshaderFlowDir.y)+vec2(.0001,.0003));
  vec2 trshaderFlowScreenSide=vec2(-trshaderFlowScreenDir.y,trshaderFlowScreenDir.x);
  float trshaderFlowSpeed=max(vSynFlowInfo.w,.22);
  float trshaderDuplicatePass=trshaderSat(vSynFlowInfo.z);
  float trshaderOriginalSync=clamp(TR456_WATER_FLOW_ORIGINAL_SYNC,0.0,1.0);
  float trshaderDecorTime=trshaderT*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0)*(.98+trshaderFlowSpeed*.30);
  float trshaderOriginalTravel=trshaderT*trshaderFlowSpeed;
  float trshaderFlowTime=mix(trshaderDecorTime,trshaderOriginalTravel,trshaderOriginalSync);
  float trshaderGameTravel=mix(
    trshaderDecorTime*(.18+trshaderFlowSpeed*.20),
    trshaderOriginalTravel,trshaderOriginalSync);
  float trshaderCascadeMask=0.0;
   vec4 trshaderJunction=trshaderWaterJunctionField(vSynFlowUv,trshaderGameTravel,trshaderFlowSpeed);
   float trshaderStandingBlend=trshaderFlowStandingJunctionBlend(
     clamp(TR456_WATER_FLOW_STANDING_BLEND,0.0,1.0),
     trshaderJunction,trshaderF.trshaderShoreline,trshaderCascadeMask);
   float trshaderBranchReplacement=trshaderFlowReceivingBranchMask(trshaderJunction);
   float trshaderCascadeBlend=trshaderCascadeJunctionBlend(trshaderCascadeMask,trshaderJunction,
     trshaderStandingBlend,trshaderF.trshaderShoreline);
   float trshaderPoolBlend=trshaderReceivingPoolBlend(trshaderCascadeMask,trshaderCascadeBlend,
     trshaderStandingBlend,trshaderJunction,trshaderF.trshaderShoreline);
   float trshaderPoolReplacement=trshaderFlowPoolReplacementMask(trshaderPoolBlend,trshaderStandingBlend,trshaderJunction);
   float trshaderCascadeSurface=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
  float trshaderHorizontalSurface=smoothstep(.38,.78,abs(vSynNormal.y));
  float trshaderSettledWarp=trshaderSat(trshaderPoolBlend*.92+trshaderPoolReplacement+
    trshaderStandingBlend*trshaderCascadeMask*trshaderCascadeSurface*trshaderHorizontalSurface*.55);
  TrshaderFlowCore trshaderFlowCore=trshaderBuildFlowCore(trshaderFlowTime,
    trshaderGameTravel,trshaderFlowSpeed);
  vec4 trshaderFlowColor=trshaderRenderSurfaceFlow(trshaderF,trshaderFlowDir,trshaderFlowSide,trshaderFlowScreenDir,trshaderFlowScreenSide,
    trshaderFlowSpeed,trshaderDuplicatePass,trshaderFlowTime,trshaderGameTravel,trshaderSettledWarp,
    trshaderFlowCore);
  if(trshaderStandingBlend>.001) {
   vec4 trshaderCalmColor=trshaderRenderCalmFlowSurface(trshaderF,trshaderFlowDir,trshaderFlowSide,trshaderFlowScreenDir,
     trshaderFlowScreenSide,trshaderFlowSpeed,trshaderDuplicatePass,trshaderFlowTime,trshaderGameTravel,
     trshaderSettledWarp,trshaderFlowCore);
   trshaderFlowColor=mix(trshaderFlowColor,trshaderCalmColor,trshaderStandingBlend);
  }
  float trshaderJunctionFoam=0.0;
  vec3 trshaderFoamColor=mix(vec3(.46,.64,.68),vec3(.82,.94,.98),
    trshaderSat(trshaderJunction.w+trshaderF.trshaderFresnel*.35+trshaderPoolBlend*.25+trshaderPoolReplacement*.18));
  if(trshaderCascadeBlend>.002) {
   vec4 trshaderCascadeColor=trshaderRenderCascadeFlow(trshaderF,trshaderFlowDir,trshaderFlowSide,trshaderFlowScreenDir,trshaderFlowScreenSide,
     trshaderFlowSpeed,trshaderGameTravel,trshaderPoolBlend,trshaderJunction);
   trshaderFlowColor=mix(trshaderFlowColor,trshaderCascadeColor,trshaderCascadeBlend);
   trshaderJunctionFoam=max(trshaderJunctionFoam,trshaderJunctionFoamMask(trshaderCascadeBlend,trshaderJunction,
     trshaderF.trshaderShoreline,trshaderStandingBlend));
  }
  if(trshaderPoolBlend>.002) {
   vec3 trshaderPoolScene=trshaderOriginalWaterGrade(texture(uTrWaterScene,
     clamp(trshaderF.trshaderScreen,vec2(.001),vec2(.999))).rgb);
    vec3 trshaderPoolSoft=trshaderPoolScene*vec3(.92,1.01,1.06)+vec3(.000,.010,.018);
    float trshaderSettleTint=smoothstep(.06,.82,trshaderPoolBlend);
    trshaderFlowColor.rgb=mix(trshaderFlowColor.rgb,trshaderPoolScene,trshaderPoolBlend*.84);
    trshaderFlowColor.rgb=mix(trshaderFlowColor.rgb,trshaderPoolSoft,trshaderSettleTint*.22);
   trshaderJunctionFoam=max(trshaderJunctionFoam,
     trshaderReceivingPoolFoam(trshaderPoolBlend,trshaderCascadeBlend,trshaderJunction,trshaderF.trshaderShoreline));
  }
  if(trshaderPoolReplacement>.002) {
   TrshaderSyntheticFrame trshaderPoolFrame=trshaderStandingPoolReplacementFrame(trshaderF);
   vec4 trshaderPoolRipple=trshaderRenderStandingWater(trshaderPoolFrame);
   vec3 trshaderPoolScene=trshaderOriginalWaterGrade(texture(uTrWaterScene,
     clamp(trshaderF.trshaderScreen,vec2(.001),vec2(.999))).rgb);
   float trshaderBranchMix=smoothstep(.05,.82,trshaderBranchReplacement);
   float trshaderRippleWeight=mix(.54,.72,smoothstep(.18,.86,trshaderPoolReplacement));
   trshaderPoolRipple.rgb=mix(trshaderPoolScene,trshaderPoolRipple.rgb,trshaderRippleWeight);
   trshaderPoolRipple.rgb=mix(trshaderPoolRipple.rgb,
     trshaderPoolRipple.rgb*vec3(.92,1.02,1.07)+vec3(.000,.006,.012),
     trshaderPoolReplacement*.16);
   float trshaderRippleRidge=pow(trshaderSat(sin(dot(vSynWorldPos.xz,trshaderFlowDir)*.047+
     trshaderT*1.86+trshaderJunction.x*1.4)*.5+.5),12.0);
   float trshaderCrossRidge=pow(trshaderSat(sin(dot(vSynWorldPos.xz,trshaderFlowSide)*.058-
     trshaderT*2.32+trshaderJunction.y*1.1)*.5+.5),16.0);
   float trshaderRippleEnergy=trshaderSat(trshaderPoolFrame.trshaderRainRipples.z*.62+
     trshaderPoolFrame.trshaderContactWake.z*.44+trshaderPoolFrame.trshaderContacts.z*.24+
     trshaderPoolFrame.trshaderAlive.z*.10+trshaderRippleRidge*.13+trshaderCrossRidge*.10);
   vec3 trshaderSceneRipple=trshaderPoolScene*vec3(1.025,1.035,1.035)+
     vec3(.004,.006,.007);
   trshaderSceneRipple+=vec3(.040,.070,.076)*(trshaderRippleEnergy*.42+
     trshaderPoolFrame.trshaderFresnel*.12);
   trshaderSceneRipple=mix(trshaderPoolScene,trshaderSceneRipple,.46+.34*trshaderBranchReplacement);
   trshaderPoolRipple.rgb=mix(trshaderPoolRipple.rgb,trshaderSceneRipple,trshaderBranchMix);
   trshaderFlowColor=mix(trshaderFlowColor,trshaderPoolRipple,smoothstep(.05,.82,trshaderPoolReplacement));
   trshaderJunctionFoam=max(trshaderJunctionFoam,
     trshaderReceivingPoolFoam(max(trshaderPoolBlend,trshaderPoolReplacement),trshaderCascadeBlend,
       trshaderJunction,trshaderF.trshaderShoreline)*(.88+.42*trshaderPoolReplacement)*
       (1.0-trshaderBranchMix*.55));
  }
  if(trshaderJunctionFoam>.001) {
   trshaderFlowColor.rgb=mix(trshaderFlowColor.rgb,trshaderFoamColor,trshaderJunctionFoam*.48);
   trshaderFlowColor.rgb+=trshaderFoamColor*(trshaderJunctionFoam*.085);
  }
  trshaderFlowColor.a=clamp(trshaderFlowColor.a,.26,.78);
  trshaderFlowColor.a=mix(trshaderFlowColor.a,.74,trshaderSat(trshaderJunctionFoam*.30+trshaderCascadeBlend*.18));
  trshaderFlowColor.a=mix(trshaderFlowColor.a,.58,smoothstep(.05,.82,trshaderPoolReplacement));
  trshaderFlowColor.a=mix(trshaderFlowColor.a,.46,trshaderPoolBlend*.22);
  trshaderFragColor=trshaderFlowColor;
  return;
 }

 trshaderFragColor=trshaderRenderStandingWater(trshaderF);
}
