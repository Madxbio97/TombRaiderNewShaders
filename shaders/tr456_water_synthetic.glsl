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
#ifndef TR456_WATER_FLOW_ORIGINAL_DEFORMATION
#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION 0.85
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
#ifndef TR456_WATER_CONTACT_EDGE
#define TR456_WATER_CONTACT_EDGE 0.45
#endif
#ifndef TR456_WATER_FLOW_CONTACT_STRENGTH
#define TR456_WATER_FLOW_CONTACT_STRENGTH 1.0
#endif
#ifndef TR456_WATER_FLOW_CONTACT_NORMAL
#define TR456_WATER_FLOW_CONTACT_NORMAL 1.0
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
uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
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

out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float fastPow2(float x){ return x*x; }
float fastPow5(float x){ float x2=x*x; return x2*x2*x; }
float fastPow6(float x){ float x2=x*x; return x2*x2*x2; }
float fastPow9(float x){ float x2=x*x; float x4=x2*x2; return x4*x4*x; }
float fastPow13(float x){ float x2=x*x; float x4=x2*x2; float x8=x4*x4; return x8*x4*x; }
float fastPow18(float x){ float x2=x*x; float x4=x2*x2; float x8=x4*x4; float x16=x8*x8; return x16*x2; }
float fastPow48(float x){ float x2=x*x; float x4=x2*x2; float x8=x4*x4; float x16=x8*x8; float x32=x16*x16; return x32*x16; }
float fastPow58(float x){ float x2=x*x; float x4=x2*x2; float x8=x4*x4; float x16=x8*x8; float x32=x16*x16; return x32*x16*x8*x2; }
float luma(vec3 c){ return dot(c,vec3(.299,.587,.114)); }

#define TR_TOGGLE_FLOW_FOAM uTrWaterToggle0.x
#define TR_TOGGLE_FLOW_CHROMA uTrWaterToggle0.y
#define TR_TOGGLE_FLOW_LANES uTrWaterToggle0.w
#define TR_TOGGLE_FLOW_WARP uTrWaterToggle1.x
#define TR_TOGGLE_FLOW_REFLECTION uTrWaterToggle1.y
#define TR_TOGGLE_SURFACE_FOAM uTrWaterToggle2.x
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 float y=luma(c);
 c=mix(vec3(y),c,clamp(TR456_WATER_COLOR_SATURATION*.84,.55,1.08));
 c=mix(c,c*vec3(.86,.96,1.03),.12);
 return clamp(c*(.90+.10*TR456_WATER_BRIGHTNESS),vec3(0.0),vec3(2.1));
}

vec3 originalWaterGrade(vec3 c){
 float y=luma(c);
 c=mix(vec3(y)*vec3(.84,.92,.94),c,clamp(TR456_WATER_COLOR_SATURATION*.74,.50,1.04));
 return clamp(c*(.94+.08*TR456_WATER_BRIGHTNESS),vec3(0.0),vec3(1.55));
}

vec3 waterVolume(vec3 c, float depth, float ndv, vec3 tint){
 float path=sat(depth*(.58+.18*(1.0-ndv))*
   TR456_WATER_VOLUME_STRENGTH*TR456_WATER_DEPTH_ABSORPTION);
 vec3 absorbed=c*exp(-vec3(.48,.20,.10)*path);
 absorbed=mix(absorbed,vec3(luma(absorbed))*vec3(.82,.91,.93),path*.20);
 return mix(absorbed,tint,sat(path*.052));
}

float depthAwareOpacity(float opacity, float depth){
  return clamp(opacity,0.0,1.0);
}

float stableWaterBody(float opacity, float signal, float edge){
 return sat(opacity*.34+signal*.14+edge*.10);
}

vec3 holdWaterTint(vec3 c, vec3 tint, float amount){
 float y=luma(c);
 vec3 neutral=vec3(y)*vec3(.72,.93,1.00);
 vec3 cooled=c*vec3(.86,1.00,1.07)+tint*.48;
 return mix(c,mix(neutral,cooled,.68),sat(amount));
}

float stressFoamGate(float stress){
  return smoothstep(.16,.62,sat(stress));
}

float contactRadius(vec4 c){
 float encoded=abs(c.w);
 float encodedMode=step(49152.0,encoded);
 float nativeRadius=clamp(encoded*.025,90.0,340.0);
 float encodedRadius=clamp(floor(encoded*(1.0/512.0)),90.0,720.0);
 return mix(nativeRadius,encodedRadius,encodedMode);
}

vec3 baseWaterField(vec2 w, float t, vec2 primaryDir){
 vec2 a=normalize(primaryDir+vec2(.0001,.0003));
 vec2 b=vec2(-a.y,a.x);
 vec2 c=normalize(a*.38-b*.92);
 vec2 d=normalize(a+b);
 float p1=dot(w,a)*.017+t*1.10;
 float p2=dot(w,b)*.014-t*1.02;
 float p3=dot(w,c)*.026+t*1.56;
 float p4=dot(w,d)*.047-t*2.30;
 float px=p1*.58-p2*.46+t*.18;
 vec2 slope=a*cos(p1)*.018*.72+
            b*cos(p2)*.014*.56+
            c*cos(p3)*.028*.22+
            d*cos(p4)*.047*.070+
            normalize(a-b)*cos(px)*.010*.34;
 float ridge=fastPow9(sat(sin(p4)*.5+.5));
 float crossing=sin(px)*(.45+.55*sat(sin(p1)*sin(p2)*.5+.5));
 float h=sin(p1)*.62+sin(p2)*.56+sin(p3)*.18+ridge*.28+crossing*.20;
 return vec3(slope*60.0,h);
}

float reliefHeight(vec2 w, vec2 uv, float t, vec2 primaryDir){
 vec2 p=w*.0048+uv*2.2;
 vec2 a=normalize(primaryDir+vec2(.0001,.0003));
 vec2 b=vec2(-a.y,a.x);
 vec2 c=normalize(a*.34-b*.94);
 float r0=sin(dot(p,a)*6.20+t*.42+sin(dot(p,b)*1.70-t*.09)*.16);
 float r1=sin(dot(p,b)*9.50-t*.36+sin(dot(p,c)*2.10+t*.07)*.12);
 float r2=sin(dot(p,c)*14.0+t*.27);
 float lace=pow(sat(sin(dot(p,normalize(a+b))*12.0+t*.20)*.5+.5),2.8)-.42;
 float grain=sin((r0*.58+r1*.42)*2.40+r2*.10);
 return (r0*.105+r1*.060+r2*.032+lace*.035+grain*.020);
}

vec3 reliefField(vec2 w, vec2 uv, float t, vec2 primaryDir){
 float stepSize=22.0;
 float h=reliefHeight(w,uv,t,primaryDir);
 float hx=reliefHeight(w+vec2(stepSize,0.0),uv+vec2(.010,0.0),t,primaryDir)-h;
 float hy=reliefHeight(w+vec2(0.0,stepSize),uv+vec2(0.0,.010),t,primaryDir)-h;
 float strength=clamp(TR456_WATER_SURFACE_RELIEF*TR456_WATER_TEXTURE_STRENGTH,0.0,2.0)*.34;
 return vec3(vec2(hx,hy)*(6.2*strength),h*(.22*strength));
}

vec3 softMotionField(vec2 w, float t, vec2 primaryDir){
 vec2 a=normalize(primaryDir+vec2(.0001,.0003));
 vec2 b=vec2(-a.y,a.x);
 vec2 c=normalize(a*.72-b*.69);
 float s0=sin(dot(w,a)*.0105+t*.46);
 float s1=sin(dot(w,b)*.0150-t*.38+s0*.18);
 float s2=sin(dot(w,c)*.0270+t*.92+s1*.10);
 vec2 slope=(a*cos(dot(w,a)*.0105+t*.46)*.0105*.70+
             b*cos(dot(w,b)*.0150-t*.38+s0*.18)*.0150*.48+
             c*cos(dot(w,c)*.0270+t*.92+s1*.10)*.0270*.16);
 float softLine=fastPow6(sat(sin(dot(w,normalize(a+b))*.032+t*1.12+s0*.24)*.5+.5));
 float strength=clamp(TR456_WATER_SWELL_STRENGTH*.70+TR456_WATER_MICRO_RIPPLE*.48,0.0,1.8);
 return vec3(slope*(58.0*strength),softLine*strength);
}

float hash12(vec2 p){
 vec3 p3=fract(vec3(p.xyx)*.1031);
 p3+=dot(p3,p3.yzx+33.33);
 return fract((p3.x+p3.y)*p3.z);
}

float valueNoise(vec2 p){
 vec2 i=floor(p);
 vec2 f=fract(p);
 f=f*f*(3.0-2.0*f);
 float a=hash12(i);
 float b=hash12(i+vec2(1.0,0.0));
 float c=hash12(i+vec2(0.0,1.0));
 float d=hash12(i+vec2(1.0,1.0));
 return mix(mix(a,b,f.x),mix(c,d,f.x),f.y);
}

float lineMask(float x, float sharpness){
 return pow(sat(1.0-abs(fract(x)-.5)*2.0),sharpness);
}

float fbmNoise(vec2 p){
 float v=0.0;
 float a=.55;
 mat2 r=mat2(.80,-.60,.60,.80);
 for(int i=0;i<3;i++){
  v+=valueNoise(p)*a;
  p=r*p*2.07+vec2(17.31,9.17);
  a*=.55;
 }
 return v;
}

vec2 softLimitVec2(vec2 v, float limit){
 float m=length(v);
 float safeLimit=max(limit,.00001);
 float scale=(safeLimit*(1.0-exp(-m/safeLimit)))/max(m,.00001);
 return v*scale;
}

vec2 syntheticBumpSlope(vec2 slope, float amount, float limit){
 float a=clamp(amount,0.0,2.2);
 if(a<=.001) return vec2(0.0);
 return softLimitVec2(slope*max(TR456_WATER_BUMP_SCALE,.10),limit)*
   a;
}

vec2 syntheticStandingBump(vec2 baseSlope, vec2 reliefSlope,
                           vec2 aliveSlope, vec2 rippleSlope){
 float amount=TR456_WATER_BUMP_STRENGTH*TR456_WATER_SYNTHETIC_BUMP_STRENGTH;
 return syntheticBumpSlope(baseSlope*.34+reliefSlope*1.35+
   aliveSlope*.74+rippleSlope*.16,amount,.46);
}

float reflectionUvFade(vec2 uv){
 vec2 a=smoothstep(vec2(-.060),vec2(.120),uv);
 vec2 b=smoothstep(vec2(-.060),vec2(.120),1.0-uv);
 return a.x*a.y*b.x*b.y;
}

vec2 preciseReflectionUv(vec2 screen, vec3 normal, vec3 viewDir,
                         vec2 warp, float lift, float roughness){
 vec3 n=normalize(normal);
 vec3 v=normalize(viewDir);
 float ndv=sat(abs(dot(n,v)));
 float grazing=smoothstep(.14,.92,1.0-ndv);
 vec2 local=screen+vec2(warp.x,-warp.y*.35);
 vec2 mirror=vec2(screen.x,1.0-screen.y-lift);
 vec2 mirrorWarp=vec2(warp.x,-abs(warp.y))*(.64+.36*roughness);
 return mix(local,mirror+mirrorWarp,sat(.62+.38*grazing));
}

vec3 stableSceneColor(vec2 uv, vec2 fallback){
 vec3 a=texture(uTrWaterScene,clamp(fallback,vec2(.001),vec2(.999))).rgb;
 vec3 b=texture(uTrWaterScene,clamp(uv,vec2(.001),vec2(.999))).rgb;
 return mix(a,b,reflectionUvFade(uv));
}

vec4 flowPatchField(vec2 p, float travel, float speed){
 vec2 drift=vec2(-travel*(.030+speed*.006),travel*(.012+speed*.002));
 vec2 grid=p*vec2(2.65,4.75)+drift;
 vec2 base=floor(grid);
 vec2 local=fract(grid);
 float patchData=0.0;
 float rim=0.0;
 float toneSum=0.0;
 float weight=0.0;
 for(int ix=-1;ix<=1;ix++){
  for(int iy=-1;iy<=1;iy++){
   vec2 offs=vec2(float(ix),float(iy));
   vec2 id=base+offs;
   float rnd=hash12(id+vec2(11.7,3.4));
   vec2 center=vec2(hash12(id+vec2(2.7,4.1)),
     hash12(id+vec2(6.3,1.9)));
   vec2 d=local-(offs+center);
   d.x*=mix(.64,1.58,hash12(id+vec2(4.4,8.2)));
   d.y*=mix(1.36,.72,hash12(id+vec2(9.1,2.6)));
   float dist=length(d);
   float radius=mix(.18,.44,rnd);
   float blob=1.0-smoothstep(radius,radius+.22,dist);
   float shell=blob*smoothstep(radius*.36,radius*.88,dist);
   float life=.76+.24*sin(travel*(.035+speed*.006)+rnd*6.28318);
   float v=blob*life;
   float cellTone=hash12(id+vec2(12.5,9.1));
   patchData=max(patchData,v);
   rim=max(rim,shell*life);
   toneSum+=cellTone*v;
   weight+=v;
  }
 }
 float patchTone=weight>.001 ? toneSum/weight :
   valueNoise(p*vec2(5.0,7.0)+drift*.37);
 float grain=valueNoise(p*vec2(19.0,11.0)+vec2(-travel*.080,travel*.035));
 return vec4(sat(patchData),sat(rim),sat(patchTone),grain);
}

vec4 waterJunctionField(vec2 p, float travel, float speed){
 vec4 patchData=flowPatchField(p*0.82+vec2(.17,.41),travel*.82,speed);
 vec2 drift=vec2(-travel*(.026+speed*.004),travel*(.010+speed*.002));
 vec2 q=p+vec2(patchData.z-.5,patchData.w-.5)*(.034+.048*patchData.x);
 float broad=valueNoise(q*vec2(1.35,2.75)+drift*.32);
 float mid=valueNoise(q*vec2(4.8,8.6)+drift);
 float fine=valueNoise(q*vec2(13.5,18.0)+vec2(-travel*.18,travel*.045));
 float tongue=sin(q.x*8.2+q.y*.92-travel*(.58+speed*.10)+(mid-.5)*1.75)*.5+.5;
 tongue=pow(sat(tongue),2.15)*smoothstep(.24,.88,mid*.62+broad*.38);
 float lace=smoothstep(.48,.93,mid*.42+fine*.38+patchData.y*.36);
 float breakup=sat(broad*.46+mid*.30+patchData.x*.18+fine*.06);
 float foam=sat(lace*.58+tongue*.30+patchData.y*.42);
 return vec4(breakup,lace,tongue,foam);
}

float flowStandingJunctionBlend(float baseBlend, vec4 junction,
                                float shoreline, float cascadeMask){
 float breakup=clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 float local=(junction.x-.5)*.12+(junction.y-.5)*.075+
   (junction.z-.5)*.060;
 float cascadeAgitation=smoothstep(.08,.78,cascadeMask)*(.045+.045*junction.z);
 float thinCascade=1.0-smoothstep(.30,.86,uTrWaterSyntheticProfile.z);
 float poolSettle=smoothstep(.58,.94,baseBlend)*
   smoothstep(.04,.62,cascadeMask)*
   (.045+.060*junction.w+.050*thinCascade);
  float b=baseBlend+local*breakup*baseBlend*(1.0-baseBlend)*2.6+
   shoreline*.035*baseBlend+poolSettle-
   cascadeAgitation*(1.0-baseBlend)*.75;
 return sat(b);
}

float cascadeJunctionBlend(float cascadeMask, vec4 junction,
                           float standingBlend, float shoreline){
 float breakup=clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 float dissolve=(junction.x-.5)*.22+(junction.z-.5)*.20+
   (junction.y-.5)*.10+shoreline*.035-standingBlend*.025;
 float b=cascadeMask+dissolve*breakup;
 return smoothstep(.18,.86,b)*smoothstep(.015,.22,cascadeMask);
}

float junctionFoamMask(float cascadeBlend, vec4 junction,
                       float shoreline, float standingBlend){
 float seam=sat(1.0-abs(cascadeBlend-.52)*2.25);
 float fallLip=smoothstep(.62,1.0,cascadeBlend)*(.24+.42*junction.w);
 float calmLift=mix(1.0,.72,standingBlend);
 return sat((seam*(junction.w*.54+junction.y*.24+shoreline*.36)+
   fallLip*(junction.w*.42+junction.z*.26+shoreline*.18))*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM*calmLift);
}

float receivingPoolBlend(float cascadeMask, float cascadeBlend,
                         float standingBlend, vec4 junction,
                         float shoreline){
 float settled=smoothstep(.58,.96,standingBlend);
 float cascadeEdge=sat(1.0-abs(cascadeBlend-.42)*2.05);
 float cascadeReach=smoothstep(.035,.58,cascadeMask);
 float thinCascade=1.0-smoothstep(.30,.86,uTrWaterSyntheticProfile.z);
 float tongue=smoothstep(.24,.82,junction.z*.52+junction.x*.30+
   junction.w*.18);
 float lace=smoothstep(.34,.92,junction.y*.46+junction.w*.34+
   shoreline*.28);
 float softPool=smoothstep(.78,.98,standingBlend)*
   smoothstep(.62,.96,junction.x)*.14;
 float plungePool=smoothstep(.45,.98,cascadeMask)*settled*thinCascade*
   (.18+.30*junction.w+.12*shoreline);
 float triple=sat((cascadeReach*.54+cascadeEdge*.62+shoreline*.20)*
   (tongue*.72+lace*.34)*settled);
 return sat(triple*1.25+softPool+plungePool)*(1.0-cascadeBlend*.24);
}

float receivingPoolFoam(float poolBlend, float cascadeBlend,
                        vec4 junction, float shoreline){
 float seam=sat(1.0-abs(cascadeBlend-.42)*2.15);
 float plume=smoothstep(.10,.82,poolBlend)*(.55+.45*junction.w);
 return sat((poolBlend*.95+seam*.42+plume*.34)*(junction.w*.58+
   junction.y*.36+junction.z*.22+shoreline*.42)*
   TR456_WATER_FLOW_STREAK_FOAM*2.15*
   TR_TOGGLE_FLOW_FOAM);
}

float flowReceivingBranchMask(vec4 junction){
 float cascadeProfile=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
 float drawCount=max(uTrWaterDrawInfo.z,0.0);
 float drawIndex=max(uTrWaterDrawInfo.y,0.0);
 float patchSettle=smoothstep(.20,.86,
   junction.w*.55+junction.x*.25+junction.y*.20);
 float branchDraw=smoothstep(8.0,14.0,drawCount)*
   (1.0-smoothstep(24.0,32.0,drawCount));
 float branchIndex=1.0-smoothstep(2.45,3.10,drawIndex);
 float branchParams=(1.0-smoothstep(.015,.060,abs(uParams.x)))*
   smoothstep(1.15,1.30,abs(uParams.y))*
   (1.0-smoothstep(1.66,1.82,abs(uParams.y)));
 return sat(branchDraw*branchIndex*branchParams*cascadeProfile*
   (.82+.18*patchSettle));
}

float flowPoolReplacementMask(float poolBlend, float standingBlend,
                              vec4 junction){
 float horizontalSurface=smoothstep(.38,.78,abs(vSynNormal.y));
 float cascadeProfile=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
 float drawCount=max(uTrWaterDrawInfo.z,0.0);
 float hasCount=step(1.0,drawCount);
 float compactDraw=hasCount*(1.0-smoothstep(384.0,2048.0,drawCount));
 float patchSettle=smoothstep(.20,.86,
   junction.w*.55+junction.x*.25+junction.y*.20);
 float poolNeed=smoothstep(.26,.62,poolBlend)*
   smoothstep(.50,.86,standingBlend);
 float receivingBranch=flowReceivingBranchMask(junction);
 float settledPool=poolNeed*horizontalSurface*cascadeProfile*
   (.62+.38*compactDraw)*(.70+.30*patchSettle);
 return sat(max(settledPool,receivingBranch));
}

vec4 syntheticFlowPattern(vec2 p, float time, float speed, vec4 patchField){
 vec2 q=p;
 float adv=time*(.18+speed*.20);
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 q+=vec2(patchField.z-.5,patchField.w-.5)*(.026+.046*patchField.x);
 float n0=valueNoise(q*4.6+vec2(adv*.10,-adv*.055));
 float n1=valueNoise(q.yx*6.3+vec2(-adv*.080,adv*.095));
 float n2=valueNoise(q*12.5+vec2(adv*.13,adv*.040));
 float streamCoord=q.x*13.8+n0*.34-adv*(.62+speed*.20);
 float fineCoord=q.x*31.0+n1*.42-adv*(1.26+speed*.30);
 float stream=max(lineMask(streamCoord,9.5)*.28,
   lineMask(fineCoord,19.0)*.38);
 stream*=mix(1.0,mix(.70,1.22,patchField.z),patchField.x*.62);
 float sideDrift=sin(q.x*1.85-adv*.24+n0*2.0)*.20+
   sin(q.x*3.8+adv*.12+n1*1.5)*.075;
 float laneCoord=q.y*4.8+sideDrift+n1*.70;
 float laneBroad=smoothstep(.58,.92,
   valueNoise(vec2(laneCoord*.35,q.x*.55-adv*.20)));
 float laneThread=pow(sat(sin(laneCoord*3.6+n0*1.5)*.5+.5),4.2);
 float lane=laneBroad*laneThread*TR456_WATER_FLOW_LANE*TR_TOGGLE_FLOW_LANES;
 lane*=mix(1.0,mix(.70,1.16,patchField.w),patchField.x*.48);
 float ribbonCoord=q.x*6.8+n0*.44-adv*(.54+speed*.16)+sin(q.y*3.0+n1)*.12;
 float ribbonFine=lineMask(q.x*13.6+n1*.52-adv*(1.04+speed*.20)+
   sin(q.y*4.6+n0)*.14,15.0)*smoothstep(.42,.92,n2);
 float ribbon=(lineMask(ribbonCoord,6.4)*.72+ribbonFine*.42)*smoothstep(.36,.88,n2)*
   TR456_WATER_FLOW_RIBBON*TR_TOGGLE_FLOW_LANES;
 ribbon*=mix(1.0,mix(.72,1.24,patchField.z),patchField.x*.50);
 float eddy=smoothstep(.55,.90,
   valueNoise(q*1.7+vec2(adv*.04,-adv*.03)))*
   TR456_WATER_FLOW_SWIRL*TR_TOGGLE_FLOW_LANES;
 float chop=sin(q.x*72.0+q.y*2.6+n2*3.3-adv*(1.68+speed*.42));
 float streak=pow(sat(sin(fineCoord+n2*.58)*.5+.5),8.2)*smoothstep(.24,.90,n2);
 float crossCoord=q.y*16.5+sin(q.x*3.2-adv*.18+n1)*.22-
   adv*(.36+speed*.12);
 float crossFine=q.y*34.0+q.x*1.8+n0*.34-adv*(.70+speed*.18);
 float crossWave=(lineMask(crossCoord,7.5)*.26+
   lineMask(crossFine,16.0)*.18)*crossStrength*TR_TOGGLE_FLOW_LANES;
 float crest=sat(stream*.30+lane*.30+ribbon*.32+streak*.28+
   crossWave*.30+patchField.y*.18+patchField.x*.060+
   eddy*.14+abs(chop)*.11*TR456_WATER_FLOW_BREAKUP);
 float swirl=max(eddy,patchField.y*.25*TR456_WATER_FLOW_SWIRL*TR_TOGGLE_FLOW_LANES);
 float foam=(lineMask(streamCoord+n2*.18,10.0)*.18+lineMask(fineCoord+n0*.24,22.0)*.16+
   lane*.28+ribbon*.28+ribbonFine*.18+streak*.18+crossWave*.14+patchField.y*.12+
   swirl*.12+crest*crest*.14)*TR_TOGGLE_FLOW_FOAM;
 return vec4(crest,foam,swirl,stream);
}

vec3 syntheticFlowField(vec2 p, float time, float speed, vec4 patchField){
 vec2 q=p;
 q+=vec2(patchField.z-.5,patchField.w-.5)*(.020+.040*patchField.x);
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 float n0=valueNoise(q*4.2+vec2(time*.020,-time*.011))*2.0-1.0;
 float n1=valueNoise(q.yx*5.8+vec2(-time*.017,time*.019))*2.0-1.0;
 float adv=time*(.70+speed*.28);
 float main=sin(q.x*29.0+n0*1.8-adv*(.70+speed*.12));
 float longWave=sin(q.x*14.5+q.y*2.4+n1*1.2-adv*(.42+speed*.10));
 float sideTurb=sin(q.x*18.0+q.y*1.15+n1*1.7-adv*.48);
 float fast=sin(q.x*78.0+n0*2.5-adv*(1.34+speed*.28));
 float crossMain=sin(q.y*24.0+q.x*2.3+n0*1.4-adv*(.50+speed*.09));
 float crossFast=sin(q.y*54.0-q.x*1.6+n1*2.0-adv*(.88+speed*.18));
 float patchCue=patchField.x*.10+patchField.y*.12;
 float h=main*.20+longWave*.11+sideTurb*.08+fast*.12+
   (crossMain*.12+crossFast*.075)*crossStrength+
   patchCue;
 vec2 slope=vec2(
   cos(q.x*29.0+n0*1.8-adv*(.70+speed*.12))*.020+
   cos(q.x*14.5+q.y*2.4+n1*1.2-adv*(.42+speed*.10))*.008+
   cos(q.x*78.0+n0*2.5-adv*(1.34+speed*.28))*.006+
   cos(q.y*24.0+q.x*2.3+n0*1.4-adv*(.50+speed*.09))*.004*crossStrength,
   cos(q.x*18.0+q.y*1.15+n1*1.7-adv*.48)*.005+
   cos(q.x*14.5+q.y*2.4+n1*1.2-adv*(.42+speed*.10))*.006+
   (cos(q.y*24.0+q.x*2.3+n0*1.4-adv*(.50+speed*.09))*.017+
    cos(q.y*54.0-q.x*1.6+n1*2.0-adv*(.88+speed*.18))*.008)*crossStrength);
 float strength=clamp(TR456_WATER_FLOW_STRENGTH*TR456_WATER_FLOW_WAVE_STRENGTH,0.0,3.0);
 return vec3(slope*(44.0*strength),h*strength);
}

vec3 flowMicroChopField(vec2 p, float travel, float speed, vec4 patchField){
 float detailPower=clamp(TR456_WATER_FLOW_DETAIL*TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 if(detailPower<=.001) return vec3(0.0);
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 p+=vec2(patchField.z-.5,patchField.w-.5)*(.016+.030*patchField.x);
 float n0=valueNoise(p*vec2(18.0,7.5)+vec2(-travel*.35,travel*.040));
 float n1=valueNoise(p*vec2(9.0,16.0)+vec2(-travel*.22,-travel*.025));
 float phaseA=p.x*76.0+p.y*3.2+n0*4.0-travel*(7.5+speed*1.6);
 float phaseB=p.x*47.0-p.y*2.4+n1*3.2-travel*(5.4+speed*1.1);
 float phaseC=p.y*68.0+p.x*2.8+n0*2.4-travel*(4.4+speed*.95);
 float gate=smoothstep(.38,.88,valueNoise(p*vec2(3.6,5.8)+vec2(-travel*.15,travel*.030)))*
   mix(.72,1.18,patchField.w);
 float amp=detailPower*gate;
 vec2 slope=vec2(cos(phaseA)*.010+cos(phaseB)*.006+
   cos(phaseC)*.003*crossStrength,
   (n0-n1)*.006+cos(phaseA*.63+phaseB*.18)*.003+
   cos(phaseC)*.011*crossStrength)*amp;
 float chop=sat((abs(sin(phaseA))*.42+abs(sin(phaseB))*.24+
   abs(sin(phaseC))*.20*crossStrength+patchField.y*.18)*amp);
 return vec3(slope,chop);
}

vec4 flowRefractiveStreakField(vec2 p, float travel, float speed, vec4 patchField){
  float detailPower=clamp(TR456_WATER_FLOW_DETAIL,0.0,1.8)*TR_TOGGLE_FLOW_WARP;
  if(detailPower<=.001) return vec4(0.0);
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 p+=vec2(patchField.z-.5,patchField.w-.5)*(.014+.026*patchField.x);
 float n0=valueNoise(p*vec2(3.0,6.4)+vec2(-travel*.12,travel*.025));
 float n1=valueNoise(p*vec2(7.2,2.1)+vec2(-travel*.25,travel*.018));
 float fine=lineMask(p.x*8.8+n0*.50-travel*(1.18+speed*.22),12.0);
 float broad=lineMask(p.x*3.6+n1*.42-travel*(.70+speed*.15),4.2);
 float crossMask=lineMask(p.y*5.8+p.x*.42+n0*.32-travel*(.42+speed*.12),6.8);
 float gate=smoothstep(.34,.88,n1);
 float mask=sat((fine*.55+broad*.32+crossMask*.20*crossStrength+
   patchField.y*.16)*gate*detailPower);
 vec2 slope=vec2(mask*(.012+.006*n0)+crossMask*.002*crossStrength,
   ((n0-n1)*.006+crossMask*.010*crossStrength)*mask);
  return vec4(slope,mask,(broad+crossMask*.34*crossStrength)*gate);
}

vec4 flowVolumeWaveField(vec2 p, float travel, float speed, vec4 patchField){
  float strength=clamp(TR456_WATER_FLOW_VOLUME_WAVE,0.0,2.5);
  if(strength<=.001) return vec4(0.0);
  float scale=max(TR456_WATER_FLOW_VOLUME_WAVE_SCALE,.25);
  vec2 q=p/scale;
  float adv=travel*(.72+speed*.16);
  float n0=fbmNoise(q*vec2(1.15,2.10)+vec2(-adv*.045,adv*.018));
  float n1=fbmNoise(q*vec2(2.40,1.00)+vec2(-adv*.072,-adv*.011));
  float lane=sin(q.y*2.10+n0*1.20+adv*.18)*.20+
    sin(q.y*4.60-n1*.80-adv*.11)*.08;
  float phase=q.x*3.25+lane+n0*.65-adv*.58;
  float phase2=q.x*6.70+q.y*.75+n1*.75-adv*1.05;
  float broad=sin(phase)*.5+.5;
  float shoulder=sin(phase2)*.5+.5;
  float swell=smoothstep(.22,.92,broad)*(.72+.28*smoothstep(.35,.90,n1));
  float crest=pow(swell,2.2)*(.72+.28*shoulder);
  float trough=pow(1.0-swell,1.55)*(.55+.45*n0);
  vec2 slope=vec2(
    (cos(phase)*.050+cos(phase2)*.022)*(.75+.25*n1),
    sin(phase+q.y*2.80)*.012+cos(q.y*4.00+n0)*.009);
  float volume=sat((crest*.70+(broad-.5)*.18+patchField.x*.08)*strength);
  float shadow=sat(trough*.45*strength);
  return vec4(slope*strength,volume,shadow);
}

vec4 smoothFlowDeformationField(vec3 w, vec2 flowDir, vec2 flowSide,
                                float travel, float speed, float calmMix){
  float strength=clamp(TR456_WATER_FLOW_ORIGINAL_DEFORMATION,0.0,1.4)*
    clamp(TR456_WATER_FLOW_WAVE_STRENGTH,0.0,2.2);
  if(strength<=.001) return vec4(0.0);
  vec2 p=vec2(dot(w.xz,flowDir),dot(w.xz,flowSide))*.00058;
  float adv=travel*(.26+speed*.060);
  float n0=fbmNoise(p*vec2(1.08,1.62)+vec2(-adv*.055,adv*.018));
  float n1=fbmNoise(p*vec2(2.25,.92)+vec2(-adv*.086,-adv*.012));
  float sideDrift=sin(p.y*2.10+n0*1.55+adv*.24)*.24+
    sin(p.y*4.20-n1*1.20-adv*.16)*.10;
  float phaseA=p.x*5.10+sideDrift+n0*.92-adv*.78;
  float phaseB=p.x*2.55+p.y*.74+n1*.72-adv*.44;
  float phaseC=p.y*3.65-p.x*.42+n0*.66+adv*.30;
  float phaseD=p.x*8.40-p.y*.32+n1*.48-adv*1.16;
  float waveA=sin(phaseA);
  float waveB=sin(phaseB);
  float waveC=sin(phaseC);
  float waveD=sin(phaseD);
  float calmDamp=mix(1.0,.62,clamp(calmMix,0.0,1.0));
  vec2 slope=vec2(
    cos(phaseA)*.052+cos(phaseB)*.035+cos(phaseD)*.015,
    cos(phaseC)*.043+cos(phaseB)*.018-cos(phaseD)*.010)*
    strength*calmDamp;
  float swell=sat((waveA*.48+waveB*.32+waveC*.20)*.5+.5);
  float crest=pow(smoothstep(.28,.92,swell),1.35)*
    (.72+.28*smoothstep(.30,.88,n0));
  float trough=pow(sat(1.0-swell),1.45)*(.64+.36*n1);
  float body=sat((crest*.54+(waveD*.5+.5)*.16+n0*.10)*strength*calmDamp);
  float shadow=sat(trough*.36*strength*calmDamp);
  return vec4(slope,body,shadow);
}

vec3 contactField(vec3 w, float t){
 vec2 slope=vec2(0.0);
 float crest=0.0;
 float standingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 float slopeStrength=mix(1.26,3.20,standingProfile);
 float crestStrength=mix(.50,1.55,standingProfile);
 for(int i=0;i<16;i++){
  vec4 c=uContacts[i];
  float contactOn=step(.001,dot(abs(c),vec4(1.0)));
  if(contactOn<=.001) continue;
  float radius=contactRadius(c);
  vec2 d=w.xz-c.xz;
  float dist=length(d)+.001;
  vec2 dir=d/dist;
  float vertical=1.0-smoothstep(120.0,620.0,abs(w.y-c.y));
  float age=mod(abs(c.w),512.0);
  float falloff=contactOn*vertical*(1.0-smoothstep(radius*.10,radius*2.85,dist))*exp(-dist/(radius*1.12));
  float phase=dist*.047-t*3.85+age*.075+float(i)*.41;
  float ring=sin(phase);
  float ringSharp=sat(ring*.5+.5);
  ringSharp*=ringSharp;
  ringSharp*=ringSharp;
  slope+=dir*cos(phase)*falloff*slopeStrength;
  crest+=ringSharp*falloff*crestStrength;
 }
 return vec3(slope,crest);
}

vec3 rainRippleField(vec2 p, float t){
 float strength=clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.5)*
   clamp(TR456_WATER_SURFACE_RELIEF*.55+TR456_WATER_MICRO_RIPPLE*.35,0.0,2.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 float cell=650.0;
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
   float age=fract(t*.18+rnd);
   float density=smoothstep(.22,.96,rnd);
   float fade=smoothstep(.035,.14,age)*(1.0-smoothstep(.74,1.0,age))*density;
   float front=mix(20.0,540.0,age);
   float width=mix(22.0,68.0,age);
   float crestX=(d-front)/width;
   float troughX=(d-(front-width*.82))/(width*1.55);
   float crestRing=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
   float shell=1.0-smoothstep(600.0,900.0,d);
   float ring=(crestRing*.58-trough*.22)*shell;
   float dCrest=(-2.0*crestX/width)*crestRing;
   float dTrough=(-2.0*troughX/(width*1.55))*trough;
   float dRing=(dCrest*.58-dTrough*.22)*shell;
   height+=ring*fade;
   slope+=dir*dRing*fade;
  }
 }
 return vec3(clamp(slope*strength*1.18,vec2(-.24),vec2(.24)),
   clamp(height*strength,-1.0,1.0));
}

vec4 contactWakeField(vec3 w, float t, vec2 primaryDir){
 vec2 slope=vec2(0.0);
 float crest=0.0;
 float foam=0.0;
 vec2 flowDir=normalize(primaryDir+vec2(.0001,.0003));
 vec2 sideDir=vec2(-flowDir.y,flowDir.x);
 float wakeWidth=clamp(TR456_WATER_WAKE_WIDTH,0.10,2.4);
 float wakeLength=clamp(TR456_WATER_WAKE_LENGTH,0.12,3.4);
 float wakeStrength=clamp(TR456_WATER_WAKE_STRENGTH,0.0,3.0)*
   clamp(uTrWaterSyntheticProfile.z,0.0,2.0);
 for(int i=0;i<16;i++){
  vec4 c=uContacts[i];
  float contactOn=step(.001,dot(abs(c),vec4(1.0)));
  if(contactOn<=.001) continue;
  vec4 m=uContactMotion[i];
  vec2 mv=m.xz;
  float speed=length(mv);
  vec2 d=w.xz-c.xz;
  float radius=contactRadius(c);
  float vertical=1.0-smoothstep(120.0,680.0,abs(w.y-c.y));
  float along=dot(d,flowDir)/max(radius,1.0);
  float side=dot(d,sideDir)/max(radius,1.0);
  float age=mod(abs(c.w),512.0);
  float motionEnergy=sat(speed*.13);
  float lengthWindow=1.0-smoothstep(.72*wakeLength,2.35*wakeLength,abs(along));
  float sideWidth=.135+.150*wakeWidth;
  float sideFalloff=exp(-fastPow2(side/max(sideWidth,.001)));
  float broken=valueNoise(vec2(along*4.8+t*.045,side*9.5+age*.010));
  float phase=along*(10.0+3.8*wakeLength)-t*(.76+motionEnergy*.62)+
    age*.030+broken*.72;
  float streak=contactOn*vertical*lengthWindow*sideFalloff*
    (.44+.56*motionEnergy)*(.62+.38*broken);
  float ridge=sat(sin(phase)*.5+.5);
  slope+=flowDir*(sin(phase)*streak*.18+ridge*streak*.060)+
    sideDir*(-side*streak*(.16+.10*motionEnergy));
  crest+=streak*ridge*(.18+.22*motionEnergy);
  foam+=streak*(.16+.38*motionEnergy)*TR456_WATER_CONTACT_EDGE;
 }
 return vec4(slope*(.48*wakeStrength*TR456_WATER_WAKE_WAVE),
   crest*wakeStrength*.52,foam*wakeStrength*.82);
}

vec3 waterfallImpactWaveField(vec3 w, float t, vec2 primaryDir){
 vec2 slope=vec2(0.0);
 float crest=0.0;
 vec2 flowDir=normalize(primaryDir+vec2(.0001,.0003));
 vec2 sideDir=vec2(-flowDir.y,flowDir.x);
 for(int i=0;i<16;i++){
  vec4 c=uContacts[i];
  float contactOn=step(.001,dot(abs(c),vec4(1.0)));
  if(contactOn<=.001) continue;
  vec4 m=uContactMotion[i];
  float radius=contactRadius(c);
  vec2 d=w.xz-c.xz;
  float dist=length(d)+.001;
  vec2 dir=d/dist;
  float vertical=1.0-smoothstep(140.0,860.0,abs(w.y-c.y));
  float stable=1.0-smoothstep(8.0,120.0,m.w);
  float smallSource=(1.0-smoothstep(180.0,420.0,radius))*
    smoothstep(72.0,104.0,radius);
  float age=mod(abs(c.w),512.0);
  float reach=1780.0+radius*5.4;
  float fade=exp(-dist/(760.0+radius*3.8))*
    (1.0-smoothstep(reach,reach*1.34,dist));
  float source=contactOn*vertical*smallSource*(.62+.38*stable);
  float broken=valueNoise(vec2(dot(d,flowDir)*.0022+float(i)*1.7,
    dot(d,sideDir)*.0028+t*.050));
  float phase=dist*.0255-t*2.32+age*.032+broken*.72+float(i)*.43;
  float phase2=dist*.0165-t*1.55+age*.020+broken*.50;
  float ring=sin(phase)*.68+sin(phase2)*.32;
  float ridge=pow(sat(ring*.5+.5),2.6);
  float directional=.78+.22*smoothstep(-.20,.82,dot(dir,flowDir));
  float amp=source*fade*directional*(.72+.28*broken);
  slope+=dir*(cos(phase)*.076+cos(phase2)*.034)*amp+
    sideDir*sin(phase2+broken)*amp*.010;
  crest+=ridge*amp;
 }
 return vec3(clamp(slope,vec2(-.18),vec2(.18)),sat(crest));
}

float shorelineEdgeField(vec2 screen, float t, vec2 primaryDir){
 vec2 inv=max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 flowScreen=normalize(vec2(primaryDir.x,-primaryDir.y)+vec2(.0001,.0003));
 vec2 sideScreen=vec2(-flowScreen.y,flowScreen.x);
 vec2 a=flowScreen*inv*vec2(9.0,9.0);
 vec2 b=sideScreen*inv*vec2(7.0,7.0);
 vec3 c0=texture(uTrWaterScene,clamp(screen,vec2(.001),vec2(.999))).rgb;
 vec3 c1=texture(uTrWaterScene,clamp(screen+a,vec2(.001),vec2(.999))).rgb;
 vec3 c2=texture(uTrWaterScene,clamp(screen-a,vec2(.001),vec2(.999))).rgb;
 vec3 c3=texture(uTrWaterScene,clamp(screen+b,vec2(.001),vec2(.999))).rgb;
 vec3 c4=texture(uTrWaterScene,clamp(screen-b,vec2(.001),vec2(.999))).rgb;
 float l0=luma(c0);
 float contrast=max(abs(luma(c1)-luma(c2)),abs(luma(c3)-luma(c4)));
 contrast=max(contrast,max(abs(luma(c1)-l0),abs(luma(c3)-l0))*.72);
 float edge=smoothstep(.030,.155,contrast);
 float grain=valueNoise(screen*vec2(720.0,420.0)+vec2(t*.030,-t*.017));
 float pulse=sin(dot(screen,flowScreen)*900.0+t*.82+grain*1.8)*.5+.5;
 return edge*(.58+.42*grain)*(.76+.24*pulse);
}

struct SyntheticFrame {
 vec2 screen;
 float time;
 vec3 baseField;
 vec3 contacts;
 vec3 rainRipples;
 vec4 contactWake;
 vec3 waterfallWaves;
 float shoreline;
 vec3 relief;
 vec3 alive;
 vec2 slope;
 vec3 normal;
 vec3 viewDir;
 float ndv;
 float fresnel;
};

SyntheticFrame buildSyntheticFrame(vec2 screen, float t){
  SyntheticFrame f;
  vec2 primaryDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
  f.screen=screen;
 f.time=t;
 f.baseField=baseWaterField(vSynWorldPos.xz,t,primaryDir);
 f.contacts=contactField(vSynWorldPos,t);
 float standingProfile=1.0-smoothstep(2.35,2.95,uTrWaterSyntheticProfile.x);
 f.rainRipples=rainRippleField(vSynWorldPos.xz,t)*standingProfile;
 f.contactWake=contactWakeField(vSynWorldPos,t,primaryDir);
 f.waterfallWaves=vec3(0.0);
 if(standingProfile>.001) {
  f.waterfallWaves=waterfallImpactWaveField(vSynWorldPos,t,primaryDir)*
    standingProfile;
 }
 f.shoreline=shorelineEdgeField(screen,t,primaryDir)*
   clamp(uTrWaterSyntheticProfile.y,0.0,2.0);
 f.relief=reliefField(vSynWorldPos.xz,vSynUv,t,primaryDir);
 f.alive=softMotionField(vSynWorldPos.xz,t,primaryDir);
 f.slope=f.baseField.xy*.78+f.contacts.xy*1.65+f.rainRipples.xy*1.80+
   f.contactWake.xy*TR_TOGGLE_CONTACT_RIPPLES+
   f.waterfallWaves.xy*1.42*TR_TOGGLE_CONTACT_RIPPLES+
   f.relief.xy*.52+f.alive.xy*.48;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
 f.slope+=syntheticStandingBump(f.baseField.xy,f.relief.xy,f.alive.xy,
   f.rainRipples.xy+f.contactWake.xy*.35*TR_TOGGLE_CONTACT_RIPPLES+
   f.waterfallWaves.xy*.22*TR_TOGGLE_CONTACT_RIPPLES)*standingProfile;
#endif
 f.normal=normalize(vec3(-f.slope.x,1.0,-f.slope.y));
 f.viewDir=normalize(-vSynPos+vSynNormal*.001);
 f.ndv=sat(abs(dot(f.normal,f.viewDir)));
 f.fresnel=pow(1.0-f.ndv,2.55);
 return f;
}

SyntheticFrame standingPoolReplacementFrame(SyntheticFrame f){
 SyntheticFrame p=f;
 p.rainRipples=rainRippleField(vSynWorldPos.xz,f.time);
 float wakeLift=1.0/max(clamp(uTrWaterSyntheticProfile.z,.18,2.0),.18);
 p.contactWake=f.contactWake*clamp(wakeLift,.80,3.40);
 p.waterfallWaves=f.waterfallWaves;
 p.slope=p.baseField.xy*.78+p.contacts.xy*1.65+p.rainRipples.xy*1.80+
   p.contactWake.xy*TR_TOGGLE_CONTACT_RIPPLES+
   p.waterfallWaves.xy*1.42*TR_TOGGLE_CONTACT_RIPPLES+
   p.relief.xy*.52+p.alive.xy*.48;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
 p.slope+=syntheticStandingBump(p.baseField.xy,p.relief.xy,p.alive.xy,
   p.rainRipples.xy+p.contactWake.xy*.35*TR_TOGGLE_CONTACT_RIPPLES+
   p.waterfallWaves.xy*.22*TR_TOGGLE_CONTACT_RIPPLES);
#endif
 p.normal=normalize(vec3(-p.slope.x,1.0,-p.slope.y));
 p.ndv=sat(abs(dot(p.normal,p.viewDir)));
 p.fresnel=pow(1.0-p.ndv,2.55);
 return p;
}

vec4 renderCascadeFlow(SyntheticFrame f, vec2 flowDir, vec2 flowSide,
                       vec2 flowScreenDir, vec2 flowScreenSide,
                       float flowSpeed, float gameTravel,
                       float poolBlend, vec4 junction){
 vec2 texFlow=length(uParams.xy)>.000001 ? normalize(uParams.xy) : vec2(0.0,-1.0);
 vec2 texSide=vec2(-texFlow.y,texFlow.x);
 vec2 sourceFlowUv=vec2(dot(vSynUv,texFlow),dot(vSynUv,texSide));
 float worldDown=-vSynWorldPos.y*.00105+dot(vSynWorldPos.xz,flowDir)*.00010;
 float worldSide=dot(vSynWorldPos.xz,flowSide)*.00072+
   valueNoise(vSynWorldPos.xz*.0011)*.18;
 vec2 fallUv=vec2(worldDown,worldSide);
 float fallTime=-gameTravel*(1.55+flowSpeed*.24);
 float sourceDrift=sin(sourceFlowUv.x*6.28318+sourceFlowUv.y*.37)*.055;
 float n0=valueNoise(fallUv*vec2(2.6,8.5)+vec2(-fallTime*.18+sourceDrift,fallTime*.035));
 float n1=valueNoise(fallUv*vec2(6.2,3.4)+vec2(-fallTime*.30+sourceDrift*.6,-fallTime*.018));
 float n2=valueNoise(fallUv*vec2(12.0,14.0)+vec2(-fallTime*.48,fallTime*.052));
 float sideVeil=smoothstep(.36,.86,
   valueNoise(vec2(fallUv.y*3.4+n0*.62,fallUv.x*.34-fallTime*.22)));
 float fineVeil=smoothstep(.52,.94,
   valueNoise(vec2(fallUv.y*9.0+n1*.82,fallUv.x*.56-fallTime*.38)));
 float droplet=smoothstep(.78,.97,n2)*
   smoothstep(.22,.88,valueNoise(fallUv*vec2(4.0,7.5)+vec2(-fallTime*.72,fallTime*.11)));
 float curtain=sideVeil*(.52+.48*n0);
 float thread=fineVeil*(.34+.66*n1);
 float foamPower=clamp(TR456_WATER_FLOW_STREAK_FOAM,0.0,2.0)*
   TR_TOGGLE_FLOW_FOAM;
 float mist=smoothstep(.36,.88,n0*.46+n1*.34+n2*.20)*
   foamPower;
 mist=sat(mist*2.05);
 float foamVeil=smoothstep(.24,.82,curtain*.32+thread*.38+
   sideVeil*.20+fineVeil*.22+n2*.12)*foamPower;
 float impactPlume=smoothstep(.34,.92,thread*.38+droplet*.34+
   mist*.34+n1*.18)*foamPower;
 float plume=smoothstep(.24,.86,curtain*.30+thread*.40+
   droplet*.26+mist*.46+impactPlume*.30);
 float veil=smoothstep(.14,.78,n0*.28+n1*.40+n2*.32);
 float sheetNoise=valueNoise(vec2(fallUv.y*2.2+n0*.54,
   fallUv.x*.40-fallTime*.16));
 float sheetBreak=valueNoise(fallUv*vec2(1.55,5.2)+
   vec2(-fallTime*.11,fallTime*.028));
 float sheetLayer=smoothstep(.18,.78,curtain*.38+veil*.26+
   sheetNoise*.20+sheetBreak*.12+n0*.10);
 float strandBroad=lineMask(fallUv.y*4.4+fallUv.x*.34+
   n0*.56-fallTime*.22,4.6)*
   smoothstep(.20,.86,sideVeil+n1*.28);
 float strandFine=lineMask(fallUv.y*12.8-fallUv.x*.20+
   n1*.46-fallTime*.46,13.5)*
   smoothstep(.28,.94,fineVeil+n2*.30);
 float strandTear=lineMask(fallUv.y*20.0+fallUv.x*.16+
   n2*.30-fallTime*.72,22.0)*smoothstep(.42,.98,n2);
 float strandLayer=sat((strandBroad*.52+strandFine*.42+
   strandTear*.26+thread*.20)*(.50+.50*sheetLayer));
 float surfaceFoam=smoothstep(.16,.74,curtain*.28+thread*.26+
   veil*.18+sheetNoise*.25+sheetBreak*.18+n0*.12+
   strandLayer*.22)*foamPower;
 surfaceFoam=sat(surfaceFoam*(1.08+.42*smoothstep(.16,.90,
   curtain+thread+mist+impactPlume)));
 float impactNoise=valueNoise(vec2(fallUv.y*6.5+n1*.72,
   fallUv.x*1.3-fallTime*.52));
 float impactPulse=lineMask(fallUv.y*8.4+n2*.62-fallTime*.64,5.0);
 float impactBoil=smoothstep(.04,.66,poolBlend)*
   smoothstep(.28,.92,impactNoise*.40+impactPulse*.24+
   droplet*.26+mist*.28+surfaceFoam*.22)*
   (.72+.28*junction.w)*foamPower;
 float volumeFoam=sat(mist*.56+foamVeil*.44+impactPlume*.58+
   surfaceFoam*.58+strandLayer*.28*foamPower+impactBoil*.78+
   droplet*.18+plume*.20);
 float bloomFoam=sat((impactBoil*.68+impactPlume*.48+
   surfaceFoam*.38+mist*.28+plume*.20)*TR_TOGGLE_FLOW_FOAM);
 float aerated=sat(curtain*.36+thread*.36+droplet*.24+
   mist*.82+foamVeil*.40+impactPlume*.46+surfaceFoam*.48+
   strandLayer*.18+impactBoil*.60+plume*.24);
 vec2 fallWarp=vec2(0.0);
 vec3 sceneFall=originalWaterGrade(texture(uTrWaterScene,
   clamp(f.screen+fallWarp,vec2(.001),vec2(.999))).rgb);
 vec3 cascadeNormal=normalize(vSynNormal);
 float fallFres=pow(1.0-sat(abs(dot(cascadeNormal,f.viewDir))),1.85);
 float fallFresQuiet=fallFres*.24;
 vec3 coolBody=sceneFall*vec3(.56,.84,1.18)+vec3(.028,.096,.154);
 vec3 foamColor=mix(vec3(.54,.72,.78),vec3(.86,.96,1.00),
   sat(aerated*1.06+surfaceFoam*.34+fallFresQuiet*.14));
 vec3 col=mix(sceneFall,coolBody,sat(.30+aerated*.44+mist*.24+
   plume*.12+volumeFoam*.12+surfaceFoam*.08));
 vec3 sheetColor=sceneFall*vec3(.68,.88,1.08)+vec3(.016,.056,.092);
 col=mix(col,sheetColor,sat(sheetLayer*.20+strandLayer*.12));
 float innerShadow=sat((1.0-sheetBreak)*sheetLayer*(1.0-surfaceFoam*.42));
 col*=mix(vec3(1.0),vec3(.88,.95,1.02),innerShadow*.18);
 col=mix(col,foamColor,sat((mist*.74+thread*.30+droplet*.22+
   curtain*.10+plume*.30+foamVeil*.34+impactPlume*.36+
   surfaceFoam*.48+strandLayer*.24*foamPower+impactBoil*.62)*
   TR_TOGGLE_FLOW_FOAM));
 col=mix(col,col*vec3(.92,1.02,1.08)+foamColor*.18,
   sat((plume*.30+veil*.14+mist*.22+volumeFoam*.20+
   surfaceFoam*.26+strandLayer*.12*foamPower+impactBoil*.36)*
   TR_TOGGLE_FLOW_FOAM));
 col=mix(col,foamColor,bloomFoam*.16);
 col+=foamColor*(mist*.092+plume*.068+volumeFoam*.078+
   impactPlume*.052+surfaceFoam*.070+impactBoil*.105+
   bloomFoam*.090)*
   TR_TOGGLE_FLOW_FOAM;
 float fallSpark=smoothstep(.91,.985,
   valueNoise(fallUv*vec2(9.5,16.0)+vec2(-fallTime*.66,fallTime*.14)))*
   smoothstep(.58,.96,thread+droplet*.34)*
   step(.94,hash12(floor(fallUv*vec2(3.0,7.0)+vec2(-fallTime*.18,0.0))));
 col+=vec3(.12,.22,.28)*(fallSpark*.052+fallFresQuiet*.006)*
  TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
 vec3 light=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.22)),.18);
 col*=light;
 col=(col-.5)*1.035+.5;
 float cascadeAlpha=clamp(.55+mist*.10+surfaceFoam*.08+
   impactBoil*.10+bloomFoam*.08+plume*.04,.48,.78);
 return vec4(clamp(col,0.0,1.0),cascadeAlpha);
}

vec4 renderSurfaceFlow(SyntheticFrame f, vec2 flowDir, vec2 flowSide,
                       vec2 flowScreenDir, vec2 flowScreenSide,
                       float flowSpeed, float duplicatePass,
                       float flowTime, float gameTravel,
                       float settledWarp){
 float passOpacity=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.0),duplicatePass);
 float passReflection=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_REFLECTION,0.0,1.0),duplicatePass);
 vec2 flowUv=vSynFlowUv;
 vec4 flowAnimPatch=flowPatchField(flowUv,flowTime*(.18+flowSpeed*.20),flowSpeed);
 vec4 patchField=flowPatchField(flowUv,gameTravel,flowSpeed);
 vec4 pattern=syntheticFlowPattern(flowUv,flowTime,flowSpeed,flowAnimPatch);
  vec3 flowField=syntheticFlowField(flowUv,flowTime,flowSpeed,flowAnimPatch);
 vec3 microChop=flowMicroChopField(flowUv,gameTravel,flowSpeed,patchField);
 vec4 refrStreak=flowRefractiveStreakField(flowUv,gameTravel,flowSpeed,patchField);
 vec4 volumeWave=flowVolumeWaveField(flowUv,gameTravel,flowSpeed,patchField);
 vec4 smoothDeform=smoothFlowDeformationField(vSynWorldPos,flowDir,flowSide,
   gameTravel,flowSpeed,0.0);
  vec3 edgeA=texture(uTrWaterScene,clamp(f.screen+flowScreenSide*.0065,
    vec2(.001),vec2(.999))).rgb;
 vec3 edgeB=texture(uTrWaterScene,clamp(f.screen-flowScreenSide*.0065,
   vec2(.001),vec2(.999))).rgb;
  float edgeContrast=smoothstep(.035,.22,abs(luma(edgeA)-luma(edgeB)));
  float edgeNoise=valueNoise(flowUv*vec2(7.8,13.0)+vec2(-gameTravel*.42,gameTravel*.050));
  float profileFoam=clamp(uTrWaterSyntheticProfile.y,0.0,2.0);
  float shoreFlow=sat(f.shoreline*TR456_WATER_SHORELINE_STRENGTH*
    profileFoam*(.66+.34*pattern.w));
  float bankTongue=lineMask(flowUv.y*5.2+edgeNoise*1.4-
    gameTravel*(.28+flowSpeed*.10),4.0)*
    smoothstep(.18,.82,shoreFlow+edgeContrast*.42+pattern.w*.22);
  float bankLace=lineMask(flowUv.y*14.0+pattern.z*.45+
    sin(flowUv.x*1.5-gameTravel*.20)*.28,10.0)*
    smoothstep(.32,.90,edgeNoise+shoreFlow*.30);
  float shorelineBand=sat(shoreFlow*.90+edgeContrast*.42+
    bankTongue*.26+bankLace*.14);
  float edgeMicroGate=shorelineBand*TR456_WATER_FLOW_EDGE_FOAM*
    TR_TOGGLE_FLOW_FOAM;
  float edgeMicroNoise=valueNoise(flowUv*vec2(28.0,52.0)+
    vec2(gameTravel*.55,-gameTravel*.18));
  float edgeMicroA=sin(flowUv.x*70.0+flowUv.y*9.5-
    gameTravel*(7.40+flowSpeed*.42)+edgeMicroNoise*2.4);
  float edgeMicroB=sin(flowUv.y*92.0-flowUv.x*6.0-
    gameTravel*(9.60+flowSpeed*.55)+pattern.z*1.8);
  float edgeMicroWave=(edgeMicroA*.55+edgeMicroB*.45)*
    edgeMicroGate*(.50+.50*edgeNoise);
  float edgeMicroFoam=lineMask(flowUv.y*38.0+edgeMicroNoise*1.6-
    gameTravel*(2.40+flowSpeed*.20),18.0)*edgeMicroGate;
  float edgeTurb=(edgeContrast*smoothstep(.42,.90,edgeNoise)*
    TR456_WATER_FLOW_EDGE_FOAM+shoreFlow*.44+
    (bankTongue*.42+bankLace*.22)*TR456_WATER_FLOW_EDGE_FOAM)*
    TR_TOGGLE_FLOW_FOAM;
  edgeTurb=sat(edgeTurb+(abs(edgeMicroWave)*.24+edgeMicroFoam*.16)*
    TR_TOGGLE_FLOW_FOAM);
  float settleMask=smoothstep(.18,.92,settledWarp);
   edgeTurb*=mix(1.0,.42,settleMask);
   bankTongue*=mix(1.0,.55,settleMask);
   bankLace*=mix(1.0,.55,settleMask);
   edgeMicroWave*=mix(1.0,.35,settleMask);
   edgeMicroFoam*=mix(1.0,.45,settleMask);
   edgeMicroGate*=mix(1.0,.50,settleMask);
   vec2 flowSlopeWorld=flowDir*flowField.x+flowSide*flowField.y;
  vec2 volumeSlopeWorld=flowDir*volumeWave.x+flowSide*volumeWave.y;
  smoothDeform*=mix(1.0,.58,settleMask);
  vec2 smoothDeformWorld=flowDir*smoothDeform.x+flowSide*smoothDeform.y;
  float breathPulse=sin(flowUv.x*12.5-gameTravel*3.35+pattern.z*1.35)*.5+.5;
 float breathFine=sin(flowUv.x*25.0+flowUv.y*2.1-gameTravel*5.10+edgeNoise*.80)*.5+.5;
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float crossPulse=sin(flowUv.y*17.0+flowUv.x*1.6-gameTravel*(1.95+flowSpeed*.18)+
    edgeNoise*.55)*.5+.5;
  float crossFine=sin(flowUv.y*34.0-flowUv.x*1.1-gameTravel*(3.20+flowSpeed*.26))*.5+.5;
 float crossFine3=crossFine*crossFine*crossFine;
 float crossDistortion=clamp(TR456_WATER_FLOW_CROSS_DISTORTION,0.0,3.0);
 float crossShearA=sin(flowUv.y*41.0+flowUv.x*3.8-
   gameTravel*(2.25+flowSpeed*.22)+pattern.x*1.2);
 float crossShearB=sin(flowUv.y*73.0-flowUv.x*5.4+
   gameTravel*(1.85+flowSpeed*.17)+edgeNoise*2.0);
 float crossCounter=sin(flowUv.x*18.0-flowUv.y*21.0+
   gameTravel*(3.10+flowSpeed*.18)+pattern.z*1.6);
 float crossShear=(crossShearA*.55+crossShearB*.45)*
   crossStrength*crossDistortion;
 float crossRefract=(crossShear*.72+crossCounter*.28*
   crossStrength*crossDistortion);
 float breathFine3=breathFine*breathFine*breathFine;
 float surfaceBreath=.72+.28*pow(breathPulse,1.55);
 float surfacePulse=(pow(breathPulse,1.8)*.58+breathFine3*.28)*
   (.42+.58*pattern.w);
 float tensionStrength=clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0);
 float tensionGate=smoothstep(.28,.86,
   valueNoise(flowUv*vec2(2.2,5.6)+vec2(-gameTravel*.055,gameTravel*.021)));
 float tensionLineA=lineMask(flowUv.x*5.8+edgeNoise*.42-
   gameTravel*(.30+flowSpeed*.08),6.0);
  float tensionLineB=lineMask(flowUv.x*11.6+flowUv.y*.82+pattern.z*.45-
    gameTravel*(.52+flowSpeed*.14),18.0);
  float tensionFilm=sat((tensionLineA*.52+tensionLineB*.42)*tensionGate*
    (.62+.38*pattern.w))*tensionStrength;
  float flowContactStrength=clamp(TR456_WATER_FLOW_CONTACT_STRENGTH,0.0,3.0)*
    TR_TOGGLE_CONTACT_RIPPLES;
  float flowContactNormal=clamp(TR456_WATER_FLOW_CONTACT_NORMAL,0.0,3.0);
  vec2 flowContactSlope=(f.contacts.xy*1.35+f.contactWake.xy*.72)*
    flowContactStrength;
  float flowContactEnergy=sat((f.contacts.z*.44+f.contactWake.z*.72+
    f.contactWake.w*.58)*flowContactStrength);
  vec2 flowSlope=flowSlopeWorld*.54*surfaceBreath+
    volumeSlopeWorld*.92+
    smoothDeformWorld*.72+
    flowDir*(pattern.w*.24+pattern.y*.16+surfacePulse*.10+
      tensionFilm*.18+(pow(crossPulse,2.2)-.34)*.052*crossStrength+
      microChop.x*.72+refrStreak.x*.90+edgeTurb*.030+
      edgeMicroWave*.055+edgeMicroFoam*.018)+
    flowSide*((pattern.x-pattern.z)*.075+
      sin(flowUv.x*9.0+flowUv.y*1.3-gameTravel*3.20)*.026+
      (pow(crossPulse,1.7)-.36)*.145*crossStrength+
      (crossFine3-.24)*.062*crossStrength+
      crossShear*.016+crossRefract*.010+
      (tensionLineB-.5)*tensionFilm*.040+
      microChop.y*.64+refrStreak.y*.85+(edgeNoise-.5)*edgeTurb*.020+
      (edgeMicroA-edgeMicroB)*edgeMicroGate*.016)+
    flowContactSlope+
    f.relief.xy*.18+f.alive.xy*.12;
  vec2 flowNormalSlope=flowSlope+flowContactSlope*.42*flowContactNormal;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
  float flowBumpAmount=TR456_WATER_BUMP_STRENGTH*
    TR456_WATER_SYNTHETIC_BUMP_STRENGTH*TR456_WATER_FLOW_BUMP_STRENGTH;
  flowNormalSlope+=syntheticBumpSlope(
    flowDir*(microChop.x*1.20+refrStreak.x*1.05+pattern.w*.090+
      surfacePulse*.070+tensionFilm*.10+edgeMicroWave*.14+
      edgeMicroFoam*.050+smoothDeform.x*.34)+
   flowSide*(microChop.y*1.08+refrStreak.y*1.00+
      (pattern.x-pattern.z)*.080+
      (pow(crossPulse,1.7)-.36)*.070*crossStrength+
      crossShear*.036+
      smoothDeform.y*.30+
      (edgeMicroA-edgeMicroB)*edgeMicroGate*.045),
    flowBumpAmount,.44);
#endif
 vec3 flowNormal=normalize(vec3(-flowNormalSlope.x,1.0,-flowNormalSlope.y));
 float flowNdv=sat(abs(dot(flowNormal,f.viewDir)));
 float flowFres=pow(1.0-flowNdv,2.35)*TR456_WATER_FRESNEL_STRENGTH;
 float flowSignal=sat(pattern.x*.42+pattern.y*.54+pattern.z*.34+
    pattern.w*.24+abs(flowField.z)*.34+microChop.z*.24+
      refrStreak.z*.16+volumeWave.z*.30+volumeWave.w*.10+
      edgeTurb*.18+tensionFilm*.20+
      abs(edgeMicroWave)*.20+edgeMicroFoam*.16+
      smoothDeform.z*.20+smoothDeform.w*.08+
      (crossPulse*crossPulse*.18+crossFine3*.10)*crossStrength+
      abs(crossShear)*.030+abs(crossRefract)*.018+
     flowContactEnergy*.18+
     patchField.x*.075+patchField.y*.16);
  flowSignal*=mix(1.0,.58,settleMask);

  vec2 longPull=flowScreenDir*(pattern.w*.017+pattern.y*.014+flowSignal*.010+
    surfacePulse*.018+refrStreak.z*.012+microChop.z*.007+
    volumeWave.z*.018+tensionFilm*.014+smoothDeform.z*.012+
    smoothDeform.x*.004);
  vec2 crossTear=flowScreenSide*((pattern.x-pattern.z)*.0075+flowSlope.y*.0042+
    (tensionLineB-.5)*tensionFilm*.012+
    (crossPulse-.5)*.015*crossStrength+crossRefract*.0065);
  vec2 crossWeave=flowScreenSide*(crossShear*.0048+crossCounter*
    crossStrength*crossDistortion*.0022)+flowScreenDir*(crossCounter*
    crossStrength*crossDistortion*.0018);
  vec2 contactPull=flowScreenDir*(flowContactSlope.x*.0065+
    flowContactEnergy*.010)+flowScreenSide*(flowContactSlope.y*.0048);
  vec2 edgePull=flowScreenDir*(edgeTurb*.020+edgeMicroWave*.010+
    edgeMicroFoam*.006)+flowScreenSide*(((edgeNoise-.5)*edgeTurb*.016)+
    (edgeMicroA-edgeMicroB)*edgeMicroGate*.004);
  vec2 flowWarp=(flowScreenDir*(flowSlope.x*.0052+refrStreak.x*.50+microChop.x*.16)+
    flowScreenSide*(refrStreak.y*.40+microChop.y*.14+smoothDeform.y*.020)+
    longPull*.82+crossTear*.62+crossWeave*.72+
    contactPull*.76+edgePull*.72)*
   clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,2.2)*
   clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,3.2)*
   TR_TOGGLE_FLOW_WARP*mix(1.0,.10,settleMask);
 flowWarp=softLimitVec2(flowWarp,.062);
 vec2 chromaWarp=flowWarp*(.32+.22*TR456_WATER_FLOW_CHROMA);
 vec3 scene0=texture(uTrWaterScene,clamp(f.screen+flowWarp,vec2(.001),vec2(.999))).rgb;
 float flowChroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA*
   TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
 vec3 refractedSource=scene0;
 if(flowChroma>.001) {
   vec3 sceneR=texture(uTrWaterScene,clamp(f.screen+flowWarp+chromaWarp+flowScreenSide*.00055,
     vec2(.001),vec2(.999))).rgb;
   vec3 sceneB=texture(uTrWaterScene,clamp(f.screen+flowWarp-chromaWarp-flowScreenDir*.00055,
     vec2(.001),vec2(.999))).rgb;
   refractedSource=mix(scene0,vec3(sceneR.r,scene0.g,sceneB.b),.36*flowChroma);
 }
 vec3 refracted=originalWaterGrade(refractedSource);

 float opacityProfile=clamp(uTrWaterSyntheticProfile.z,0.05,2.0);
 float opacity=clamp(uTrWaterSyntheticInfo.x*TR456_WATER_FLOW_OPACITY*
   passOpacity*opacityProfile,.020,.76);
 float tintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
  float flowReflectStrength=TR456_WATER_FLOW_REFLECTION;
  float reflectAmt=clamp(uTrWaterSyntheticInfo.z*flowReflectStrength*
    TR456_WATER_REFLECT_STRENGTH*.26*clamp(uTrWaterSyntheticProfile.w,.05,1.0),0.0,1.6);
  float depthCue=0.0;
   float sceneFlowDepth=sat(((1.0-luma(refracted))*.34+depthCue*.24+
     flowSignal*.075+opacity*.030)*TR456_WATER_DEPTH_STRENGTH*
     (.62+.38*TR456_WATER_FLOW_DEPTH_BODY));
   float materialFlowBody=stableWaterBody(opacity,
     flowSignal*.78+pattern.w*.22+microChop.z*.18+tensionFilm*.16,
     edgeTurb*.30);
  float flowDepth=max(sceneFlowDepth,materialFlowBody*.26);
  float depthBody=smoothstep(.16,.82,flowDepth);
  float depthOpacity=depthAwareOpacity(opacity,flowDepth);
   vec3 flowTint=mix(vec3(.018,.086,.078),vec3(.006,.036,.042),
     sat(flowDepth+depthCue*.30))*tintStrength;
   refracted=waterVolume(refracted,
    max(sceneFlowDepth,materialFlowBody*.16)*(.62+.38*depthBody),flowNdv,
    flowTint*mix(.55,1.0,depthBody));
 vec3 reflected=flowTint;
 float reflMask=0.0;
#if TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED
 float reflectActive=reflectAmt;
 if(reflectActive>.001) {
  vec2 reflectWarp=flowScreenDir*(flowSlope.x*.005+pattern.x*.006+pattern.y*.004)+
    flowScreenSide*(-abs(flowSlope.y)*.003+pattern.z*.005+flowFres*.008);
  reflectWarp=clamp(reflectWarp,vec2(-.075),vec2(.075));
  vec3 reflNormal=normalize(flowNormal+vec3(reflectWarp.x*18.0,0.0,
    -reflectWarp.y*18.0));
  vec3 sceneRefl=stableSceneColor(f.screen+reflectWarp,f.screen)*.46+
    stableSceneColor(f.screen+reflectWarp*.55+vec2(0.0,.034+flowFres*.040),
      f.screen)*.34+
    stableSceneColor(f.screen-reflectWarp*.82+flowScreenSide*.018,
      f.screen)*.20;
  vec2 mirrorUv0=preciseReflectionUv(f.screen,reflNormal,f.viewDir,
    reflectWarp*.72,0.0,.45+.45*flowFres);
  vec2 mirrorUv1=preciseReflectionUv(f.screen,reflNormal,f.viewDir,
    reflectWarp*.30+vec2(0.0,.040+flowFres*.055),
    .010+.018*flowFres,.38+.36*flowFres);
  vec2 mirrorUv2=preciseReflectionUv(f.screen,reflNormal,f.viewDir,
    -reflectWarp*.92-flowScreenSide*.020,0.0,.36+.32*flowFres);
  vec3 mirrorRefl=stableSceneColor(mirrorUv0,f.screen)*.55+
    stableSceneColor(mirrorUv1,f.screen)*.30+
    stableSceneColor(mirrorUv2,f.screen)*.15;
  reflected=reflectionGrade(mix(sceneRefl,mirrorRefl,
    sat(.24+flowFres*.54+flowDepth*.18)));
  reflMask=sat((.006+flowFres*.045+flowSignal*.007+flowDepth*.004)*
    reflectActive*(1.0-pattern.y*.22)*
    mix(.60,1.0,reflectionUvFade(mirrorUv0)));
 }
#endif

 float patternX2=pattern.x*pattern.x;
 float foamStress=sat(edgeTurb*.95+shoreFlow*.95+f.contactWake.w*.80+
     microChop.z*.38+refrStreak.z*.22+patternX2*.24+
     tensionFilm*.18+flowContactEnergy*.28+
     abs(edgeMicroWave)*.32+edgeMicroFoam*.20+
     patchField.y*.62+bankTongue*.38+bankLace*.22+
     abs(flowSlope.y)*.018);
 float foamGate=stressFoamGate(foamStress);
 float aeration=sat(pattern.y*.62+pattern.z*.20+edgeTurb*.42+
   flowSignal*.28*TR456_WATER_FLOW_AERATION)*mix(.30,1.0,foamGate);
  float foamMask=sat((pattern.y*.22+patternX2*.16+
     edgeTurb*.70+f.contactWake.w*.48+flowContactEnergy*.10+shoreFlow*.78+
     bankTongue*.34+bankLace*.18+microChop.z*.10+refrStreak.z*.06+
     edgeMicroFoam*.22+abs(edgeMicroWave)*.12)*
     TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*foamGate;
   float directionalFoam=sat((pow(pattern.w,1.35)*.34+pattern.y*.30+
     bankTongue*.18+bankLace*.10+refrStreak.z*.10+
     edgeMicroFoam*.14)*
   smoothstep(.16,.86,flowSignal+shoreFlow*.28)*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*foamGate;
 float tensionPatch=sat((tensionFilm*.62+patchField.x*.18+
   microChop.z*.12+pattern.w*.10)*TR456_WATER_FLOW_SURFACE_TENSION*
   (1.0-foamMask*.36));
 foamMask=sat(foamMask+directionalFoam*.42);
 float bodyMask=sat((flowDepth*.45+flowSignal*.24+pattern.w*.12)*
   TR456_WATER_FLOW_BODY);
 float ridgeMask=sat((pattern.x*.62+abs(flowField.z)*.28+flowSignal*.24)*
   TR456_WATER_FLOW_RIDGE);
 float glint=fastPow48(sat(dot(flowNormal,normalize(vec3(-.25,.93,.27)))))*
   (.18+flowFres*.60)*(flowSignal*.55+pattern.w*.25+.12)*
   TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
 float sparkPhase=flowUv.x*12.4+valueNoise(flowUv*5.5)*.42-
   gameTravel*(1.42+flowSpeed*.32);
 float sparkLife=fract(sparkPhase*.45);
 float sparkGate=step(.88,hash12(floor(vec2(sparkPhase*.45,flowUv.y*5.5))))*
   smoothstep(.05,.32,sparkLife)*(1.0-smoothstep(.52,.96,sparkLife));
 float flowSpark=lineMask(sparkPhase,24.0)*
   smoothstep(.45,.92,pattern.w+pattern.y*.55+pattern.x*.35)*
   smoothstep(.08,.82,1.0-flowDepth*.55)*sparkGate;
  float streakGlint=(lineMask(flowUv.x*4.35+valueNoise(flowUv*3.2)*.24-
    gameTravel*(.56+flowSpeed*.18),15.0)*smoothstep(.26,.88,flowSignal)+
    flowSpark*.72+refrStreak.z*.12)*
    TR456_WATER_FLOW_SPECULAR_STREAK*TR456_WATER_GLINT_STRENGTH*
    TR456_WATER_FLOW_GLINT;
  float tensionGlint=pow(sat(tensionFilm),1.7)*(.16+flowFres*.20)*
   TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;

  vec3 flowBody=mix(refracted,flowTint+vec3(.004,.014,.014),
    max(depthOpacity*.040+bodyMask*.026,
      materialFlowBody*(.11+.05*flowFres)));
  flowBody=holdWaterTint(flowBody,flowTint,.10+materialFlowBody*.18);
  flowBody=mix(flowBody,flowBody*vec3(.82,.98,1.12)+vec3(.000,.010,.024),
    sat(flowSignal*.12+flowDepth*.08));
 flowBody+=vec3(.004,.028,.035)*(flowSignal*.70+surfacePulse*.20)*
   TR456_WATER_FLOW_STRENGTH*mix(.60,1.0,depthBody);
  flowBody=mix(flowBody,
   flowBody*mix(vec3(.94,1.02,1.02),vec3(1.05,.98,.92),patchField.z),
   patchField.x*.10);
  flowBody=mix(flowBody,flowBody*vec3(.88,1.01,1.08)+flowTint*.16,
    tensionPatch*.16);
   flowBody=mix(flowBody,flowBody*vec3(.86,.97,1.06)+flowTint*.14,
     volumeWave.w*.10);
   flowBody+=vec3(.018,.052,.058)*volumeWave.z*(.30+.70*(1.0-flowFres));
   flowBody+=vec3(.006,.030,.038)*smoothDeform.z*(.34+.66*(1.0-flowFres));
   flowBody=mix(flowBody,flowBody*vec3(.92,.98,1.04),
     smoothDeform.w*.10);
   flowBody+=vec3(.003,.018,.022)*ridgeMask;
 vec3 foamColor=mix(vec3(.44,.60,.64),vec3(.72,.88,.94),sat(flowDepth*.35+flowFres*.45));
 vec3 col=mix(flowBody,reflected,reflMask*.075);
  col=mix(col,foamColor,foamMask*.52+aeration*.18+edgeTurb*.20+
    edgeMicroFoam*.055);
 col=mix(col,foamColor,sat(bankTongue*.16+bankLace*.08)*TR456_WATER_FLOW_EDGE_FOAM);
 col=mix(col,foamColor,directionalFoam*.20);
  col+=foamColor*directionalFoam*.045+
    vec3(.020,.055,.064)*tensionPatch*(.35+.65*flowFres);
  col+=vec3(.008,.030,.036)*abs(edgeMicroWave)*(.45+.55*(1.0-flowFres));
 col+=vec3(.34,.56,.70)*(glint*.70+streakGlint*.95)+
    vec3(.10,.24,.34)*(microChop.z*.10+refrStreak.z*.18+edgeTurb*.20+
      bankTongue*.10)+
   vec3(.10,.22,.20)*tensionGlint;
 vec3 light=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.68),vec3(1.24)),.24);
 col*=light;
 col=(col-.5)*1.055+.5;
 float flowCompositeAlpha=clamp(.37+opacity*.32+flowDepth*.06+
   flowSignal*.055+foamMask*.14+directionalFoam*.08+aeration*.055+
   edgeTurb*.07+edgeMicroFoam*.04+reflMask*.08,.33,.64);
 flowCompositeAlpha*=mix(1.0,.88,duplicatePass);
 flowCompositeAlpha=mix(flowCompositeAlpha,.45,settleMask*.35);
 return vec4(clamp(col,0.0,1.0),flowCompositeAlpha);
}

vec4 renderCalmFlowSurface(SyntheticFrame f, vec2 flowDir, vec2 flowSide,
                           vec2 flowScreenDir, vec2 flowScreenSide,
                           float flowSpeed, float duplicatePass,
                           float flowTime, float gameTravel,
                           float settledWarp){
 float passOpacity=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.0),duplicatePass);
 float passReflection=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_REFLECTION,0.0,1.0),duplicatePass);
 vec2 flowUv=vSynFlowUv;
 vec4 flowAnimPatch=flowPatchField(flowUv,flowTime*(.18+flowSpeed*.20),flowSpeed);
 vec4 patchField=flowPatchField(flowUv,gameTravel,flowSpeed);
 vec4 pattern=syntheticFlowPattern(flowUv,flowTime,flowSpeed,flowAnimPatch);
  vec3 flowField=syntheticFlowField(flowUv,flowTime,flowSpeed,flowAnimPatch);
  vec3 microChop=flowMicroChopField(flowUv,gameTravel,flowSpeed,patchField);
  vec4 refrStreak=flowRefractiveStreakField(flowUv,gameTravel,flowSpeed,patchField);
  vec4 volumeWave=flowVolumeWaveField(flowUv,gameTravel*.86,flowSpeed,patchField);
  vec4 smoothDeform=smoothFlowDeformationField(vSynWorldPos,flowDir,flowSide,
    gameTravel*.86,flowSpeed,.65);
   float settleMask=smoothstep(.18,.92,settledWarp);
 float tensionStrength=clamp(TR456_WATER_FLOW_SURFACE_TENSION,0.0,2.0);
 float tensionGate=smoothstep(.26,.84,
   valueNoise(flowUv*vec2(2.0,5.0)+vec2(-gameTravel*.045,gameTravel*.018)));
 float tensionLineA=lineMask(flowUv.x*4.8+pattern.z*.38-
   gameTravel*(.24+flowSpeed*.070),5.2);
 float tensionLineB=lineMask(flowUv.x*10.4+flowUv.y*.70+pattern.x*.36-
   gameTravel*(.44+flowSpeed*.11),16.0);
 float tensionFilm=sat((tensionLineA*.58+tensionLineB*.36)*tensionGate*
   (.68+.32*pattern.w))*tensionStrength;
 float calmBreath=sin(flowUv.x*11.0+flowUv.y*.90-gameTravel*2.75+pattern.z*1.1)*.5+.5;
 float fineBreath=sin(flowUv.x*24.0+flowUv.y*1.7-gameTravel*4.65)*.5+.5;
 float crossStrength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
  float calmCross=sin(flowUv.y*15.0+flowUv.x*1.2-gameTravel*(1.55+flowSpeed*.14)+
    pattern.x*.7)*.5+.5;
  float calmCrossFine=sin(flowUv.y*31.0-flowUv.x*.85-gameTravel*(2.85+flowSpeed*.22))*.5+.5;
  float crossDistortion=clamp(TR456_WATER_FLOW_CROSS_DISTORTION,0.0,3.0);
  float calmCrossShearA=sin(flowUv.y*36.0+flowUv.x*2.9-
    gameTravel*(1.85+flowSpeed*.18)+pattern.y*1.1);
  float calmCrossShearB=sin(flowUv.y*62.0-flowUv.x*4.4+
    gameTravel*(1.42+flowSpeed*.14)+pattern.z*1.5);
  float calmCrossCounter=sin(flowUv.x*15.0-flowUv.y*18.5+
    gameTravel*(2.55+flowSpeed*.16)+pattern.x*1.3);
  float calmCrossShear=(calmCrossShearA*.58+calmCrossShearB*.42)*
    crossStrength*crossDistortion;
  float calmCrossRefract=(calmCrossShear*.70+calmCrossCounter*.30*
    crossStrength*crossDistortion);
   float fineBreath3=fineBreath*fineBreath*fineBreath;
   float calmCross2=calmCross*calmCross;
   float calmCrossFine3=calmCrossFine*calmCrossFine*calmCrossFine;
   float breathBand=.76+.24*pow(calmBreath,1.6);
   vec2 flowSlopeWorld=flowDir*flowField.x+flowSide*flowField.y;
   vec2 volumeSlopeWorld=flowDir*volumeWave.x+flowSide*volumeWave.y;
   smoothDeform*=mix(1.0,.62,settleMask);
   vec2 smoothDeformWorld=flowDir*smoothDeform.x+flowSide*smoothDeform.y;
   float flowContactStrength=clamp(TR456_WATER_FLOW_CONTACT_STRENGTH,0.0,3.0)*
     TR_TOGGLE_CONTACT_RIPPLES;
   float flowContactNormal=clamp(TR456_WATER_FLOW_CONTACT_NORMAL,0.0,3.0);
   vec2 calmContactSlope=(f.contacts.xy*.45+f.contactWake.xy*.42)*
     flowContactStrength;
   float calmContactEnergy=sat((f.contacts.z*.30+f.contactWake.z*.58+
     f.contactWake.w*.44)*flowContactStrength);
   vec2 calmSlope=f.baseField.xy*.48+f.relief.xy*.26+f.alive.xy*.34+
     flowSlopeWorld*.42*breathBand+
     volumeSlopeWorld*.58+
     smoothDeformWorld*.54+
     flowDir*(pattern.w*.10+microChop.x*.62+refrStreak.x*.60+
      tensionFilm*.20+fineBreath3*.055+
     (calmCross2-.34)*.038*crossStrength)+
   flowSide*((pattern.x-pattern.z)*.030+microChop.y*.48+refrStreak.y*.46+
     (pow(calmCross,1.7)-.35)*.120*crossStrength+
      (calmCrossFine3-.25)*.052*crossStrength+
      calmCrossShear*.012+calmCrossRefract*.008+
      (tensionLineB-.5)*tensionFilm*.055)+
    calmContactSlope;
  vec2 calmNormalSlope=calmSlope+calmContactSlope*.34*flowContactNormal;
#if TR456_WATER_SYNTHETIC_BUMP_ENABLED
  float calmBumpAmount=TR456_WATER_BUMP_STRENGTH*
    TR456_WATER_SYNTHETIC_BUMP_STRENGTH*TR456_WATER_FLOW_BUMP_STRENGTH;
  calmNormalSlope+=syntheticBumpSlope(
   flowDir*(microChop.x*1.05+refrStreak.x*.95+pattern.w*.070+
     tensionFilm*.085+fineBreath3*.030+smoothDeform.x*.26)+
   flowSide*(microChop.y*.95+refrStreak.y*.88+
      (pattern.x-pattern.z)*.050+
      (pow(calmCross,1.7)-.35)*.055*crossStrength+
      calmCrossShear*.026+smoothDeform.y*.22),
   calmBumpAmount*.82,.38);
#endif
 vec3 calmNormal=normalize(vec3(-calmNormalSlope.x,1.0,-calmNormalSlope.y));
 float calmNdv=sat(abs(dot(calmNormal,f.viewDir)));
 float calmFres=pow(1.0-calmNdv,2.50)*TR456_WATER_FRESNEL_STRENGTH;
 float flowSignal=sat(pattern.x*.26+pattern.w*.18+abs(flowField.z)*.25+
     microChop.z*.28+refrStreak.z*.18+volumeWave.z*.24+
     volumeWave.w*.08+tensionFilm*.30+fineBreath3*.12+
     (calmCross2*.18+calmCrossFine3*.10)*crossStrength+
     smoothDeform.z*.16+smoothDeform.w*.06+
     abs(calmCrossShear)*.022+abs(calmCrossRefract)*.014+
     calmContactEnergy*.16+
     patchField.x*.070+patchField.y*.14);
  flowSignal*=mix(1.0,.62,settleMask);

  vec2 longPull=flowScreenDir*(pattern.w*.018+flowSignal*.014+
    refrStreak.z*.014+volumeWave.z*.014+tensionFilm*.020+
    calmBreath*calmBreath*.010+smoothDeform.z*.010+
    smoothDeform.x*.0035);
  vec2 crossPull=flowScreenSide*((pattern.x-pattern.z)*.0045+
    calmSlope.y*.0038+(tensionLineB-.5)*tensionFilm*.014+
    (calmCross-.5)*.013*crossStrength+calmCrossRefract*.0048);
  vec2 crossWeave=flowScreenSide*(calmCrossShear*.0038+
    calmCrossCounter*crossStrength*crossDistortion*.0018)+
    flowScreenDir*(calmCrossCounter*crossStrength*crossDistortion*.0015);
  vec2 contactPull=flowScreenDir*(calmContactSlope.x*.0060+
    calmContactEnergy*.0085)+flowScreenSide*(calmContactSlope.y*.0042);
  vec2 flowWarp=(flowScreenDir*(calmSlope.x*.0058+refrStreak.x*.54+
      microChop.x*.18)+flowScreenSide*(refrStreak.y*.44+
      microChop.y*.16+smoothDeform.y*.016)+
     longPull*.82+crossPull*.64+crossWeave*.68+contactPull*.72)*
   clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,2.6)*
   clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,3.6)*
   TR_TOGGLE_FLOW_WARP*mix(1.0,.12,settleMask);
 flowWarp=softLimitVec2(flowWarp,.068);
 vec2 chromaWarp=flowWarp*(.25+.16*TR456_WATER_FLOW_CHROMA);
 vec3 sceneA=texture(uTrWaterScene,clamp(f.screen+flowWarp,vec2(.001),vec2(.999))).rgb;
 float chroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA*
   TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
 vec3 calmRefractedSource=sceneA;
 if(chroma>.001) {
   vec3 sceneR=texture(uTrWaterScene,clamp(f.screen+flowWarp+chromaWarp,
     vec2(.001),vec2(.999))).rgb;
   vec3 sceneB=texture(uTrWaterScene,clamp(f.screen+flowWarp-chromaWarp,
     vec2(.001),vec2(.999))).rgb;
   calmRefractedSource=mix(sceneA,vec3(sceneR.r,sceneA.g,sceneB.b),.24*chroma);
 }
 vec3 refracted=originalWaterGrade(calmRefractedSource);

 float opacity=clamp(uTrWaterSyntheticInfo.x*TR456_WATER_FLOW_OPACITY*
   (.78+flowSignal*.12)*passOpacity,.010,.58);
 float tintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
 float depthCue=0.0;
   float sceneFloorDepth=sat(((1.0-luma(refracted))*.24+depthCue*.18+
     flowSignal*.045+opacity*.020)*TR456_WATER_DEPTH_STRENGTH*
     (.42+.34*TR456_WATER_FLOW_DEPTH_BODY));
   float materialFloorBody=stableWaterBody(opacity,
     flowSignal*.72+pattern.w*.20+microChop.z*.18+tensionFilm*.20,
     f.shoreline*.24);
  float floorDepth=max(sceneFloorDepth,materialFloorBody*.24);
  float depthBody=smoothstep(.16,.82,floorDepth);
  float depthOpacity=depthAwareOpacity(opacity,floorDepth);
   vec3 tint=mix(vec3(.018,.130,.116),vec3(.004,.038,.044),
     sat(floorDepth+depthCue*.28))*tintStrength;
   refracted=waterVolume(refracted,
    max(sceneFloorDepth,materialFloorBody*.14)*(.44+.44*depthBody),calmNdv,
    tint*mix(.50,1.0,depthBody));

 float reflectAmt=clamp(uTrWaterSyntheticInfo.z*TR456_WATER_FLOW_REFLECTION*
   TR456_WATER_REFLECT_STRENGTH*.16*clamp(uTrWaterSyntheticProfile.w,.05,1.0),
   0.0,1.0);
 vec3 reflected=tint;
 float reflectionMask=0.0;
#if TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED
 float reflectActive=reflectAmt;
 if(reflectActive>.001) {
  vec2 reflectWarp=vec2(-calmSlope.x*.006+calmNormal.x*.008,
    .035+calmFres*.052-calmSlope.y*.004);
  vec3 sceneRefl=stableSceneColor(f.screen+reflectWarp,f.screen)*.58+
    stableSceneColor(f.screen+reflectWarp*.42+vec2(0.0,.026),
      f.screen)*.42;
  vec2 mirrorUv0=preciseReflectionUv(f.screen,calmNormal,f.viewDir,
    reflectWarp*.55,0.0,.36+.42*calmFres);
  vec2 mirrorUv1=preciseReflectionUv(f.screen,calmNormal,f.viewDir,
    reflectWarp*.24+vec2(0.0,.030),.008+.016*calmFres,
    .28+.32*calmFres);
  vec3 mirrorRefl=stableSceneColor(mirrorUv0,f.screen)*.62+
    stableSceneColor(mirrorUv1,f.screen)*.38;
  reflected=reflectionGrade(mix(sceneRefl,mirrorRefl,
    sat(.14+calmFres*.38+floorDepth*.10)));
  reflectionMask=sat((.020+calmFres*.12+floorDepth*.018)*
    reflectActive*mix(.60,1.0,reflectionUvFade(mirrorUv0)));
 }
#endif

 float shoreEdge=sat(f.shoreline*TR456_WATER_SHORELINE_STRENGTH*
   (.50+.50*(1.0-floorDepth))*TR_TOGGLE_SURFACE_FOAM);
  float calmStress=sat(shoreEdge*.92+f.contactWake.w*.55+calmContactEnergy*.22+
    microChop.z*.22+
    refrStreak.z*.16+tensionFilm*.26+patchField.y*.58+flowSignal*.12);
  float foamGate=stressFoamGate(calmStress);
  float foamMask=sat((shoreEdge*.42+f.contactWake.w*.18+calmContactEnergy*.08+
    pattern.y*.045)*
    TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*foamGate;
  float directionalFoam=sat((pow(pattern.w,1.35)*.22+pattern.y*.16+
    shoreEdge*.18+refrStreak.z*.08)*
   smoothstep(.12,.78,flowSignal+shoreEdge*.30)*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*foamGate;
 float tensionPatch=sat((tensionFilm*.55+patchField.y*.24+
   flowSignal*.12)*TR456_WATER_FLOW_SURFACE_TENSION*
   (1.0-foamMask*.30));
 foamMask=sat(foamMask+directionalFoam*.24);
  float filmGlint=pow(sat(tensionFilm),1.55)*(.10+calmFres*.18)*
    TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT;
   vec3 waterBase=refracted*mix(1.015,.94,depthOpacity)+
     tint*(.018+.050*depthOpacity)*mix(.55,1.0,depthBody);
   waterBase=holdWaterTint(waterBase,tint,.11+materialFloorBody*.17);
   waterBase=mix(waterBase,waterBase*vec3(.90,1.01,1.07)+tint*.14,
     tensionPatch*.12);
   waterBase=mix(waterBase,waterBase*vec3(.88,.98,1.05)+tint*.12,
     volumeWave.w*.075);
   waterBase+=vec3(.014,.042,.046)*volumeWave.z*(.28+.62*(1.0-calmFres));
   waterBase+=vec3(.004,.022,.028)*smoothDeform.z*(.30+.60*(1.0-calmFres));
   waterBase=mix(waterBase,waterBase*vec3(.94,.99,1.035),
     smoothDeform.w*.075);
  vec3 waterBody=mix(waterBase,reflected*.82+tint*.045,reflectionMask*.10);
 waterBody=mix(waterBody,
   waterBody*mix(vec3(.95,1.02,1.02),vec3(1.04,.99,.93),patchField.z),
   patchField.x*.085);
 vec3 foamColor=mix(vec3(.42,.58,.58),vec3(.70,.86,.86),
   sat(floorDepth*.30+calmFres*.42+shoreEdge*.50));
  vec3 col=waterBody+
    vec3(.012,.052,.052)*(flowSignal*.30+tensionFilm*.16)+
    vec3(.08,.20,.18)*filmGlint+
    vec3(.03,.08,.09)*(microChop.z*.08+refrStreak.z*.13);
 col=mix(col,foamColor,foamMask*.28);
 col=mix(col,foamColor,directionalFoam*.14);
 col+=foamColor*directionalFoam*.030+
   vec3(.016,.044,.052)*tensionPatch*(.28+.52*calmFres);
 vec3 light=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.20)),.24);
 col*=light;
 col=(col-.5)*1.035+.5;
 float calmCompositeAlpha=clamp(.32+opacity*.30+flowSignal*.070+
   shoreEdge*.06+foamMask*.10+directionalFoam*.05+tensionPatch*.05+
   reflectionMask*.06,.28,.55);
 calmCompositeAlpha=mix(calmCompositeAlpha,.42,settleMask*.40);
 calmCompositeAlpha*=mix(1.0,.90,duplicatePass);
 return vec4(clamp(col,0.0,1.0),calmCompositeAlpha);
}

vec4 renderStandingWater(SyntheticFrame f){
  vec2 primaryDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
  vec2 sideDir=vec2(-primaryDir.y,primaryDir.x);
  vec2 crossDir=normalize(primaryDir*.56+sideDir*.83);
  float refractAmt=clamp(TR456_WATER_REFRACT_STRENGTH*
    TR456_WATER_REFRACTION_WAVE_STRENGTH,0.55,2.55);
 float viewWarp=1.10;
 vec2 warp=(f.slope*.0096+f.normal.xz*.0065+
   f.relief.xy*.00225+f.alive.xy*.00112)*refractAmt*viewWarp;
 vec3 sceneA=texture(uTrWaterScene,clamp(f.screen+warp,vec2(.001),vec2(.999))).rgb;
 vec3 sceneB=texture(uTrWaterScene,clamp(f.screen-warp*.72,vec2(.001),vec2(.999))).rgb;
 vec3 refracted=originalWaterGrade(mix(sceneA,sceneB,.42));

 float opacity=clamp(uTrWaterSyntheticInfo.x,0.0,1.0);
 float tintStrength=clamp(uTrWaterSyntheticInfo.y,0.0,2.0);
  float reflectAmt=clamp(uTrWaterSyntheticInfo.z*TR456_WATER_REFLECT_STRENGTH*
    clamp(uTrWaterSyntheticProfile.w,.05,1.2),0.0,2.0);
 float depthCue=0.0;
 vec3 shallow=vec3(.020,.155,.170);
 vec3 deep=vec3(.006,.046,.060);
  vec3 tint=mix(shallow,deep,depthCue)*tintStrength;
   float floorDepth=sat(((1.0-luma(refracted))*.30+depthCue*.24+opacity*.10)*
     TR456_WATER_DEPTH_STRENGTH);
  float depthBody=smoothstep(.16,.82,floorDepth);
  float depthOpacity=depthAwareOpacity(opacity,floorDepth);
  float baseMurk=sat(.18+opacity*.42+floorDepth*.38+
    (1.0-f.fresnel)*.16);
  vec3 murkTint=mix(vec3(.025,.046,.045),vec3(.080,.106,.094),
    sat(floorDepth*.85+opacity*.35))*tintStrength;
  refracted=mix(refracted,refracted*vec3(.74,.86,.84)+murkTint,
    baseMurk*.28);
   refracted=waterVolume(refracted,floorDepth*(.55+.45*depthBody),f.ndv,
     tint*mix(.52,1.0,depthBody));
  float shoreEdge=sat(f.shoreline*TR456_WATER_SHORELINE_STRENGTH*
    (.58+.42*(1.0-floorDepth))*TR_TOGGLE_SURFACE_FOAM);
  float shorePulse=pow(sat(sin(dot(vSynWorldPos.xz,primaryDir)*.020+
    f.time*1.18+f.shoreline*2.1)*.5+.5),2.2);
  float shoreReturn=pow(sat(sin(dot(vSynWorldPos.xz,primaryDir)*.010-
    sin(dot(vSynWorldPos.xz,sideDir)*.022-f.time*.16)*.24+
    f.time*.33)*.5+.5),3.2);
  float shoreLap=pow(sat(sin(dot(vSynWorldPos.xz,primaryDir)*.014+
    sin(dot(vSynWorldPos.xz,sideDir)*.018+f.time*.22)*.32-
    f.time*.54)*.5+.5),2.6)*shoreEdge*(.52+.42*shorePulse)+
    shoreReturn*shoreEdge*.18;
  float wakeFoam=sat(f.contactWake.w*TR456_WATER_FOAM_STRENGTH*
    TR456_WATER_CONTACT_EDGE*TR_TOGGLE_CONTACT_RIPPLES);

 vec3 reflected=tint;
#if TR456_WATER_SYNTHETIC_REFLECTION_ENABLED
 vec2 reflectionWarp=vec2(-f.slope.x*.0075+f.normal.x*.010,
                          .060+f.fresnel*.100-f.slope.y*.006);
 vec2 reflectUv0=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   reflectionWarp,0.0,.34+.34*f.fresnel);
 vec2 reflectUv1=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   vec2(reflectionWarp.x*.55,reflectionWarp.y*1.70+.035),
   .010+.014*f.fresnel,.30+.30*f.fresnel);
 vec2 reflectUv2=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   vec2(reflectionWarp.x*1.45,reflectionWarp.y*.72),
   0.0,.28+.26*f.fresnel);
 reflected=stableSceneColor(reflectUv0,f.screen)*.52+
                stableSceneColor(reflectUv1,f.screen)*.28+
                stableSceneColor(reflectUv2,f.screen)*.20;
 vec2 mirrorWarp=vec2(f.slope.x*.011+f.normal.x*.013,
                      -abs(f.slope.y)*.006+f.normal.z*.008);
 vec2 mirrorUv0=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   mirrorWarp,0.0,.40+.42*f.fresnel);
 vec2 mirrorUv1=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   mirrorWarp*.55+vec2(f.slope.y*.003,.035+f.fresnel*.055),
   .010+.018*f.fresnel,.34+.34*f.fresnel);
 vec2 mirrorUv2=preciseReflectionUv(f.screen,f.normal,f.viewDir,
   mirrorWarp*1.42+vec2(-f.slope.y*.002,-.030),
   0.0,.30+.28*f.fresnel);
 vec3 mirrorRef=stableSceneColor(mirrorUv0,f.screen)*.55+
                stableSceneColor(mirrorUv1,f.screen)*.30+
                stableSceneColor(mirrorUv2,f.screen)*.15;
 float mirrorMask=sat(.30+f.fresnel*.58+floorDepth*.16);
 reflected=reflectionGrade(mix(reflected,mirrorRef,mirrorMask));
#endif

  vec2 w=vSynWorldPos.xz;
  float ridgeA=fastPow13(sat(sin(dot(w,primaryDir)*.043+f.time*2.05)*.5+.5));
 float ridgeB=fastPow18(sat(sin(dot(w,sideDir)*.055-f.time*2.62)*.5+.5));
  float ridgeCross=sqrt(ridgeA*ridgeB);
  ridgeCross*=sat(sin(dot(w,crossDir)*.032+f.time*.72)*.5+.65);
 float rippleMemory=pow(sat(f.rainRipples.z+f.contacts.z*.40+
   f.contactWake.z*.30+f.waterfallWaves.z*.18),1.25);
 float crest=sat(f.baseField.z*.28+.30+f.alive.z*.18)+
   f.contacts.z*.95+f.rainRipples.z*.70+f.contactWake.z*.42+
   f.waterfallWaves.z*.62+rippleMemory*.18;
  float reliefGrain=sat(abs(f.relief.z)*.66+f.alive.z*.22);
  float tensionA=fastPow5(sat(1.0-abs(fract(dot(w,primaryDir)*.018+
    sin(dot(w,sideDir)*.015+f.time*.21)*.18+f.time*.032)-.5)*2.0));
  float tensionB=fastPow5(sat(1.0-abs(fract(dot(w,crossDir)*.024+
    f.alive.z*.18-f.time*.026)-.5)*2.0));
  float tensionFilm=sat((tensionA*.52+tensionB*.34+ridgeCross*.16)*
    (.55+.45*(1.0-f.fresnel))*(.78+.22*reliefGrain));
  float glint=(ridgeA*.070+ridgeB*.056+ridgeCross*.150+reliefGrain*.052+
    f.alive.z*.115+f.rainRipples.z*.052+rippleMemory*.030+
    f.waterfallWaves.z*.082+tensionFilm*.135+
    fastPow58(sat(dot(f.normal,normalize(vec3(-.28,.92,.26)))))*.20)*
    (0.22+f.fresnel*.54)*reflectAmt*TR456_WATER_GLINT_STRENGTH;
 float contactLight=pow(sat(f.contacts.z+f.contactWake.z*.56),1.7)*
   .045*TR456_WATER_GLINT_STRENGTH;

  vec3 waterBase=refracted*mix(1.025,.90,depthOpacity)+
    tint*(.036+.094*depthOpacity)*mix(.55,1.0,depthBody);
  waterBase=mix(waterBase,
    waterBase*vec3(.82,.92,.90)+murkTint*.34+vec3(.006,.012,.011),
    baseMurk*.34);
  waterBase=mix(waterBase,waterBase*vec3(.94,1.03,1.04)+tint*.10,
    tensionFilm*.16);
 float reflectionMask=0.0;
 vec3 waterBody=mix(waterBase,reflected*.96+tint*.028,reflectionMask*.50);
  vec3 rim=vec3(.12,.22,.24)*f.fresnel*(.08+.22*reflectAmt);
  vec3 foamColor=mix(vec3(.38,.56,.58),vec3(.70,.88,.90),
    sat(f.fresnel+floorDepth*.35+shoreEdge*.60));
  vec3 sparkle=vec3(.34,.55,.58)*(glint+contactLight);
 vec3 light=mix(vec3(1.0),clamp(sqrt(max(vSynLight,vec3(0.0))),vec3(.70),vec3(1.22)),.26);
 vec3 shade=(vec3(.90,.98,1.00)+vec3(0.0,.050,.058)*crest+
   vec3(.018,.024,.022)*reliefGrain+vec3(.018,.026,.024)*f.alive.z)*light;
 vec3 col=(waterBody+rim+sparkle)*shade;
  float shoreFoam=sat((shoreEdge*(.38+.48*shorePulse)+shoreLap*.30+
    wakeFoam*.72+
    f.contactWake.z*.030)*TR456_WATER_FOAM_STRENGTH);
  col=mix(col,foamColor,shoreFoam*.42);
  col+=foamColor*(wakeFoam*.20+shoreEdge*.030*TR456_WATER_WET_EDGE+
    shoreLap*.040);
  col+=vec3(.024,.058,.066)*rippleMemory*(.22+.34*(1.0-f.fresnel));
  col+=vec3(.035,.070,.066)*tensionFilm*(.18+.32*(1.0-f.fresnel));
  float reliefVein=pow(sat(abs(f.relief.z)*1.90+f.alive.z*.34+
    ridgeCross*.22),1.25);
  col+=vec3(.034,.076,.084)*reliefVein*(.30+.56*(1.0-f.fresnel));
  float mistLine=fastPow5(sat(1.0-abs(fract(dot(w,vec2(.016,.011))+f.time*.022)-.5)*2.0));
  float haze=sat(floorDepth*.24+(1.0-f.fresnel)*.075+
    mistLine*.010+opacity*.085+baseMurk*.13);
  vec3 hazeColor=mix(vec3(.065,.078,.076),vec3(.125,.150,.142),sat(floorDepth+f.fresnel*.35))*tintStrength;
  col=mix(col,col*vec3(.90,.95,.94)+hazeColor,haze*.34);
  col+=hazeColor*(mistLine*.010+f.contacts.z*.004);
 float contrast=1.045;
 col=(col-.5)*contrast+.5;
 col=clamp(col,0.0,1.0);
 return vec4(col,1.0);
}

void main(){
 vec2 inv=max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 screen=gl_FragCoord.xy*inv;
 float t=uTrWaterSyntheticInfo.w;
 SyntheticFrame f=buildSyntheticFrame(screen,t);

 if(sat(vSynFlowInfo.x)>.5){
  vec2 flowDir=length(vSynFlowDir)>.0001 ? normalize(vSynFlowDir) : normalize(vec2(.92,.38));
  vec2 flowSide=vec2(-flowDir.y,flowDir.x);
  vec2 flowScreenDir=normalize(vec2(flowDir.x,-flowDir.y)+vec2(.0001,.0003));
  vec2 flowScreenSide=vec2(-flowScreenDir.y,flowScreenDir.x);
  float flowSpeed=max(vSynFlowInfo.w,.22);
  float duplicatePass=sat(vSynFlowInfo.z);
  float flowTime=t*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0)*(.98+flowSpeed*.30);
  float gameTravel=flowTime*(.18+flowSpeed*.20);
  float cascadeMask=0.0;
   vec4 junction=waterJunctionField(vSynFlowUv,gameTravel,flowSpeed);
   float standingBlend=flowStandingJunctionBlend(
     clamp(TR456_WATER_FLOW_STANDING_BLEND,0.0,1.0),
     junction,f.shoreline,cascadeMask);
   float branchReplacement=flowReceivingBranchMask(junction);
   float cascadeBlend=cascadeJunctionBlend(cascadeMask,junction,
     standingBlend,f.shoreline);
   float poolBlend=receivingPoolBlend(cascadeMask,cascadeBlend,
     standingBlend,junction,f.shoreline);
   float poolReplacement=flowPoolReplacementMask(poolBlend,standingBlend,junction);
   float cascadeSurface=1.0-smoothstep(.32,.78,uTrWaterSyntheticProfile.z);
  float horizontalSurface=smoothstep(.38,.78,abs(vSynNormal.y));
  float settledWarp=sat(poolBlend*.92+poolReplacement+
    standingBlend*cascadeMask*cascadeSurface*horizontalSurface*.55);
  vec4 flowColor=renderSurfaceFlow(f,flowDir,flowSide,flowScreenDir,flowScreenSide,
    flowSpeed,duplicatePass,flowTime,gameTravel,settledWarp);
  if(standingBlend>.001) {
   vec4 calmColor=renderCalmFlowSurface(f,flowDir,flowSide,flowScreenDir,
     flowScreenSide,flowSpeed,duplicatePass,flowTime,gameTravel,settledWarp);
   flowColor=mix(flowColor,calmColor,standingBlend);
  }
  float junctionFoam=0.0;
  vec3 foamColor=mix(vec3(.46,.64,.68),vec3(.82,.94,.98),
    sat(junction.w+f.fresnel*.35+poolBlend*.25+poolReplacement*.18));
  if(cascadeBlend>.002) {
   vec4 cascadeColor=renderCascadeFlow(f,flowDir,flowSide,flowScreenDir,flowScreenSide,
     flowSpeed,gameTravel,poolBlend,junction);
   flowColor=mix(flowColor,cascadeColor,cascadeBlend);
   junctionFoam=max(junctionFoam,junctionFoamMask(cascadeBlend,junction,
     f.shoreline,standingBlend));
  }
  if(poolBlend>.002) {
   vec3 poolScene=originalWaterGrade(texture(uTrWaterScene,
     clamp(f.screen,vec2(.001),vec2(.999))).rgb);
    vec3 poolSoft=poolScene*vec3(.92,1.01,1.06)+vec3(.000,.010,.018);
    float settleTint=smoothstep(.06,.82,poolBlend);
    flowColor.rgb=mix(flowColor.rgb,poolScene,poolBlend*.84);
    flowColor.rgb=mix(flowColor.rgb,poolSoft,settleTint*.22);
   junctionFoam=max(junctionFoam,
     receivingPoolFoam(poolBlend,cascadeBlend,junction,f.shoreline));
  }
  if(poolReplacement>.002) {
   SyntheticFrame poolFrame=standingPoolReplacementFrame(f);
   vec4 poolRipple=renderStandingWater(poolFrame);
   vec3 poolScene=originalWaterGrade(texture(uTrWaterScene,
     clamp(f.screen,vec2(.001),vec2(.999))).rgb);
   float branchMix=smoothstep(.05,.82,branchReplacement);
   float rippleWeight=mix(.54,.72,smoothstep(.18,.86,poolReplacement));
   poolRipple.rgb=mix(poolScene,poolRipple.rgb,rippleWeight);
   poolRipple.rgb=mix(poolRipple.rgb,
     poolRipple.rgb*vec3(.92,1.02,1.07)+vec3(.000,.006,.012),
     poolReplacement*.16);
   float rippleRidge=pow(sat(sin(dot(vSynWorldPos.xz,flowDir)*.047+
     t*1.86+junction.x*1.4)*.5+.5),12.0);
   float crossRidge=pow(sat(sin(dot(vSynWorldPos.xz,flowSide)*.058-
     t*2.32+junction.y*1.1)*.5+.5),16.0);
   float rippleEnergy=sat(poolFrame.rainRipples.z*.62+
     poolFrame.contactWake.z*.44+poolFrame.contacts.z*.24+
     poolFrame.alive.z*.10+rippleRidge*.13+crossRidge*.10);
   vec3 sceneRipple=poolScene*vec3(1.025,1.035,1.035)+
     vec3(.004,.006,.007);
   sceneRipple+=vec3(.040,.070,.076)*(rippleEnergy*.42+
     poolFrame.fresnel*.12);
   sceneRipple=mix(poolScene,sceneRipple,.46+.34*branchReplacement);
   poolRipple.rgb=mix(poolRipple.rgb,sceneRipple,branchMix);
   flowColor=mix(flowColor,poolRipple,smoothstep(.05,.82,poolReplacement));
   junctionFoam=max(junctionFoam,
     receivingPoolFoam(max(poolBlend,poolReplacement),cascadeBlend,
       junction,f.shoreline)*(.88+.42*poolReplacement)*
       (1.0-branchMix*.55));
  }
  if(junctionFoam>.001) {
   flowColor.rgb=mix(flowColor.rgb,foamColor,junctionFoam*.48);
   flowColor.rgb+=foamColor*(junctionFoam*.085);
  }
  flowColor.a=clamp(flowColor.a,.26,.78);
  flowColor.a=mix(flowColor.a,.74,sat(junctionFoam*.30+cascadeBlend*.18));
  flowColor.a=mix(flowColor.a,.58,smoothstep(.05,.82,poolReplacement));
  flowColor.a=mix(flowColor.a,.46,poolBlend*.22);
  fragColor=flowColor;
  return;
 }

 fragColor=renderStandingWater(f);
}
