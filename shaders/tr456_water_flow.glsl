#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.36
#define TR456_WATER_REFRACT_STRENGTH 1.62
#define TR456_WATER_REFRACTION_WAVE_STRENGTH 2.45
#define TR456_WATER_REFLECT_STRENGTH 1.78
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_TINT_STRENGTH 1.0
#define TR456_WATER_OPACITY 0.62
#define TR456_WATER_FORCE_REFLECTION 1.0
#define TR456_WATER_SCENE_REFLECTION 1.12
#define TR456_WATER_CAUSTICS_STRENGTH 1.10
#define TR456_WATER_DEPTH_STRENGTH 1.0
#define TR456_WATER_VOLUME_STRENGTH 1.0
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_RIPPLE_STRENGTH 0.65
#define TR456_WATER_RIPPLE_CENTER_X 0.50
#define TR456_WATER_RIPPLE_CENTER_Y 0.62
#define TR456_WATER_MIRROR_ROUGHNESS 1.16
#define TR456_WATER_REFLECTION_CONTRAST 1.48
#define TR456_WATER_FRESNEL_STRENGTH 1.24
#define TR456_WATER_COLOR_SATURATION 1.18
#define TR456_WATER_BRIGHTNESS 0.72
#define TR456_WATER_TEXTURE_STRENGTH 1.38
#define TR456_WATER_FLOW_STRENGTH 1.62
#define TR456_WATER_FLOW_REFLECTION 1.05
#define TR456_WATER_FLOW_OPACITY 0.42
#define TR456_WATER_FLOW_CHROMA 0.42
#define TR456_WATER_FLOW_CAUSTICS 1.05
#define TR456_WATER_FLOW_SPEED 2.05
#define TR456_WATER_FLOW_STREAK_FOAM 0.08
#define TR456_WATER_FLOW_LANE 1.22
#define TR456_WATER_FLOW_SWIRL 0.95
#define TR456_WATER_FLOW_SINGLE_LAYER 1.0
#define TR456_WATER_FLOW_AERATION 0.00
#define TR456_WATER_FLOW_GLINT 1.20
#define TR456_WATER_FLOW_REFRACTION_WARP 1.55
#define TR456_WATER_FLOW_SURFACE_DISTORTION 2.55
#define TR456_WATER_FLOW_CROSS_DISTORTION 2.35
#define TR456_WATER_FLOW_DIRECTION_SIGN 1.0
#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION 0.00
#define TR456_WATER_FLOW_BODY 0.34
#define TR456_WATER_FLOW_RIDGE 1.12
#define TR456_WATER_FLOW_EDGE_FOAM 0.32
#define TR456_WATER_FLOW_RIBBON 1.10
#define TR456_WATER_FLOW_EDDY_FOAM 0.38
#define TR456_WATER_FLOW_DEPTH_BODY 0.44
#define TR456_WATER_FLOW_SPECULAR_STREAK 1.05
#define TR456_WATER_FLOW_CROSS_WAVE 1.25
#define TR456_WATER_FLOW_BREAKUP 1.02
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.95
#define TR456_WATER_SAFE_VOLUME 1.05
#define TR456_WATER_TILE_SEAM_SOFTENING 1.0
#define TR456_WATER_TILE_SEAM_WIDTH 0.035
#endif
#ifndef TR456_WATER_FLOW_SECONDARY_MOTION
#define TR456_WATER_FLOW_SECONDARY_MOTION 0.0
#endif
#ifndef TR456_WATER_FLOW_SECONDARY_OPACITY
#define TR456_WATER_FLOW_SECONDARY_OPACITY 0.0
#endif
#ifndef TR456_WATER_FLOW_SECONDARY_REFLECTION
#define TR456_WATER_FLOW_SECONDARY_REFLECTION 0.16
#endif
#ifndef TR456_WATER_FLOW_AERATION
#define TR456_WATER_FLOW_AERATION 0.00
#endif
#ifndef TR456_WATER_FLOW_GLINT
#define TR456_WATER_FLOW_GLINT 1.20
#endif
#ifndef TR456_WATER_FLOW_REFRACTION_WARP
#define TR456_WATER_FLOW_REFRACTION_WARP 1.55
#endif
#ifndef TR456_WATER_FLOW_SURFACE_DISTORTION
#define TR456_WATER_FLOW_SURFACE_DISTORTION 2.55
#endif
#ifndef TR456_WATER_FLOW_CROSS_DISTORTION
#define TR456_WATER_FLOW_CROSS_DISTORTION 2.35
#endif
#ifndef TR456_WATER_FLOW_DIRECTION_SIGN
#define TR456_WATER_FLOW_DIRECTION_SIGN 1.0
#endif
#ifndef TR456_WATER_FLOW_ORIGINAL_DEFORMATION
#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION 0.00
#endif
#ifndef TR456_WATER_FLOW_BODY
#define TR456_WATER_FLOW_BODY 0.34
#endif
#ifndef TR456_WATER_FLOW_RIDGE
#define TR456_WATER_FLOW_RIDGE 1.12
#endif
#ifndef TR456_WATER_FLOW_EDGE_FOAM
#define TR456_WATER_FLOW_EDGE_FOAM 0.32
#endif
#ifndef TR456_WATER_FLOW_RIBBON
#define TR456_WATER_FLOW_RIBBON 1.10
#endif
#ifndef TR456_WATER_FLOW_EDDY_FOAM
#define TR456_WATER_FLOW_EDDY_FOAM 0.38
#endif
#ifndef TR456_WATER_FLOW_DEPTH_BODY
#define TR456_WATER_FLOW_DEPTH_BODY 0.44
#endif
#ifndef TR456_WATER_FLOW_SPECULAR_STREAK
#define TR456_WATER_FLOW_SPECULAR_STREAK 1.05
#endif
#ifndef TR456_WATER_FLOW_CROSS_WAVE
#define TR456_WATER_FLOW_CROSS_WAVE 1.25
#endif
#ifndef TR456_WATER_FLOW_BREAKUP
#define TR456_WATER_FLOW_BREAKUP 1.02
#endif
#ifndef TR456_WATER_CONTACT_WAVE_STRENGTH
#define TR456_WATER_CONTACT_WAVE_STRENGTH 1.08
#endif
#ifndef TR456_WATER_CONTACT_WAVE_RADIUS
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.18
#endif
#ifndef TR456_WATER_CONTACT_WAVE_SPEED
#define TR456_WATER_CONTACT_WAVE_SPEED 1.12
#endif
#ifndef TR456_WATER_CONTACT_COORD_MODE
#define TR456_WATER_CONTACT_COORD_MODE 1
#endif
#ifndef TR456_WATER_WAKE_STRENGTH
#define TR456_WATER_WAKE_STRENGTH 0.86
#endif
#ifndef TR456_WATER_WAKE_WIDTH
#define TR456_WATER_WAKE_WIDTH 0.46
#endif
#ifndef TR456_WATER_WAKE_LENGTH
#define TR456_WATER_WAKE_LENGTH 1.05
#endif
#ifndef TR456_WATER_WAKE_WAVE
#define TR456_WATER_WAKE_WAVE 0.92
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
#ifndef TR456_WATER_DETAIL_STRENGTH
#define TR456_WATER_DETAIL_STRENGTH 0.82
#endif
#ifndef TR456_WATER_DETAIL_SCALE
#define TR456_WATER_DETAIL_SCALE 1.0
#endif
#ifndef TR456_WATER_FLOW_DETAIL
#define TR456_WATER_FLOW_DETAIL 1.18
#endif
#ifndef TR456_WATER_FLOW_DETAIL_SCALE
#define TR456_WATER_FLOW_DETAIL_SCALE 1.0
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

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uContacts[16];
uniform vec4 uContactMotion[16];
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
uniform vec4 uTrWaterDrawInfo;
uniform vec4 uTrWaterMaterialProfile;
in vec2 vTexCoord;
in vec3 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
in vec3 vContactWave;
out vec4 fragColor;

#define TR_TOGGLE_FLOW_FOAM uTrWaterToggle0.x
#define TR_TOGGLE_FLOW_CHROMA uTrWaterToggle0.y
#define TR_TOGGLE_FLOW_CAUSTICS uTrWaterToggle0.z
#define TR_TOGGLE_FLOW_LANES uTrWaterToggle0.w
#define TR_TOGGLE_FLOW_WARP uTrWaterToggle1.x
#define TR_TOGGLE_FLOW_REFLECTION uTrWaterToggle1.y
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(0.2126,0.7152,0.0722)); }

float flowOriginalSprayBypass(){
 float drawCount=uTrWaterDrawInfo.z;
 float smallDraw=1.0-smoothstep(256.5,257.5,drawCount);
 float verticalParam=step(abs(uParams.x),.055)*step(1.05,abs(uParams.y));
 float sprayParam=step(.00045,abs(uParams.z))*
   (1.0-step(.0015,abs(uParams.w)));
 return smallDraw*verticalParam*sprayParam;
}

float flowCascadeSplashBypass(){
 float drawCount=uTrWaterDrawInfo.z;
 float countBand=step(2999.5,drawCount)*(1.0-step(20000.5,drawCount));
 float profileX=step(.42,uParams.x)*step(uParams.x,.58);
 float profileY=step(1.32,-uParams.y)*step(-uParams.y,1.68);
 float profileZ=1.0-step(.00025,abs(uParams.z));
 float profileW=step(8.5,uParams.w)*step(uParams.w,11.5);
 return countBand*profileX*profileY*profileZ*profileW;
}

float flowRockCascadeContactBypass(){
 float drawCount=uTrWaterDrawInfo.z;
 float countBand=step(899.5,drawCount)*(1.0-step(1500.5,drawCount));
 float profileX=1.0-step(.08,abs(uParams.x));
 float profileY=step(.55,-uParams.y)*step(-uParams.y,1.75);
 float fineFlow=abs(uParams.z);
 float fallAmp=abs(uParams.w);
 float basePass=step(.00045,fineFlow)*(1.0-step(.00160,fineFlow))*
   (1.0-step(.001,fallAmp));
 float detailPass=(1.0-step(.00025,fineFlow))*step(12.0,fallAmp)*
   (1.0-step(18.0,fallAmp));
 return countBand*profileX*profileY*max(basePass,detailPass);
}

float flowOriginalBypass(){
 float cpuProfile=step(.5,uTrWaterMaterialProfile.y);
 float shaderProfile=max(max(flowOriginalSprayBypass(),flowCascadeSplashBypass()),
   flowRockCascadeContactBypass());
 return max(cpuProfile,shaderProfile);
}

vec2 limitVec2(vec2 v, float maxLen){
 float l=length(v);
 return v*(maxLen/max(maxLen,l));
}

vec3 applyFlowBump(vec3 baseNormal, vec2 slope, float strength){
 float amount=clamp(strength,0.0,2.2);
 vec2 s=limitVec2(slope*amount,.62);
 return normalize(baseNormal+vec3(s.x,0.0,s.y));
}

float screenEdgeFade(vec2 uv){
 vec2 lo=smoothstep(vec2(.012),vec2(.070),uv);
 vec2 hi=smoothstep(vec2(.012),vec2(.070),1.0-uv);
 return lo.x*lo.y*hi.x*hi.y;
}

float reflectionEdgeFade(vec2 uv){
 vec2 lo=smoothstep(vec2(-.060),vec2(.120),uv);
 vec2 hi=smoothstep(vec2(-.060),vec2(.120),1.0-uv);
 return lo.x*lo.y*hi.x*hi.y;
}

vec3 debugIdColor(float id){
 vec3 seed=vec3(12.9898,78.233,37.719)*(id+1.0);
 return .22+.78*fract(sin(seed)*43758.5453);
}

vec3 debugTileColor(vec2 p, float layer){
 vec2 tile=floor(p);
 float id=tile.x*37.0+tile.y*19.0+floor(layer+.5)*101.0;
 vec2 f=fract(p);
 vec2 edge=min(f,1.0-f);
 float line=1.0-smoothstep(.006,.024,min(edge.x,edge.y));
 return mix(debugIdColor(id),vec3(1.0,.06,.02),line);
}

float tileSeamGuard(vec2 p){
 vec2 f=fract(p);
 vec2 edge=min(f,1.0-f);
 float d=min(edge.x,edge.y);
 float width=clamp(TR456_WATER_TILE_SEAM_WIDTH,.002,.08);
 float interior=smoothstep(width*.18,width,d);
 float guarded=mix(.08,1.0,interior);
 return mix(1.0,guarded,clamp(TR456_WATER_TILE_SEAM_SOFTENING,0.0,1.0));
}

float hash12(vec2 p){
 vec3 p3=fract(vec3(p.xyx)*.1031);
 p3+=dot(p3,p3.yzx+33.33);
 return fract((p3.x+p3.y)*p3.z);
}

vec3 polygonFacetField(vec2 p, vec2 primaryDir, float time, float speedBias){
 float strength=clamp(TR456_WATER_POLYGONAL_STRENGTH,0.0,1.5);
 if(strength<=.001) return vec3(0.0);
 float scale=max(TR456_WATER_POLYGONAL_SCALE,120.0);
 vec2 flowDir=normalize(primaryDir+vec2(.0001,.0003));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float drift=clamp(TR456_WATER_POLYGONAL_FLOW,0.0,2.5)*speedBias;
 vec2 q=vec2(dot(p,flowDir),dot(p,side))/scale;
 vec2 id=floor(q);
 vec2 f=fract(q)-.5;
 float r0=hash12(id+vec2(1.7,9.2));
 float r1=hash12(id+vec2(8.4,2.1));
 float r2=hash12(id+vec2(3.6,6.8));
 vec2 axis=normalize(vec2(r0-.5,r1-.5)+vec2(.001));
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

float depthExtinctionCurve(float x){
 x=sat(x);
 return sat(x+x*x*(1.0-x)*.38);
}

vec3 flowVolumeAbsorption(vec3 c, float body){
 float path=sat(body*TR456_WATER_VOLUME_STRENGTH*TR456_WATER_DEPTH_ABSORPTION);
 float curve=depthExtinctionCurve(path);
 vec3 shallowTint=vec3(.014,.092,.104)*TR456_WATER_TINT_STRENGTH;
 vec3 deepTint=vec3(.004,.044,.054)*TR456_WATER_TINT_STRENGTH;
 vec3 absorbed=c*exp(-vec3(.58,.25,.13)*path);
 float y=luma(absorbed);
 absorbed=mix(absorbed,vec3(y)*vec3(.52,.78,.84),sat(curve*.10));
 absorbed=mix(absorbed,shallowTint,sat(path*.09)*TR456_WATER_FLOW_OPACITY);
 return mix(absorbed,deepTint,sat(curve*.075*TR456_WATER_FLOW_OPACITY));
}

float flowShoreFoamMask(vec2 p, float coverage, float depth, float stream, float time){
 float n0=texture(sNoise,vec3(p*.74+vec2(time*.016,-time*.009),time*.012)).x;
 float n1=texture(sNoise,vec3(p*1.46+vec2(-time*.010,time*.014),time*.017)).x;
 float shallow=smoothstep(.06,.34,1.0-coverage+depth*.22);
 float range=shallow+(n0-.5)*.22+(n1-.5)*.10;
 float lace=pow(sat(1.0-abs(fract(p.y*.72+p.x*.18+n0*.35+time*.026)-.5)*2.0),4.0);
 float breakUp=smoothstep(.22,.78,stream+n1*.25);
 return sat((smoothstep(.12,.62,range)*.60+lace*.40)*breakUp);
}

float flowBankFoamVolume(vec2 p, float coverage, float depth, float stream, float wave, float time){
 float contact=smoothstep(.045,.34,1.0-coverage+depth*.18);
 float n0=texture(sNoise,vec3(p*vec2(.54,1.38)+vec2(-time*.018,time*.010),time*.015)).x;
 float n1=texture(sNoise,vec3(p*vec2(1.45,.72)+vec2(time*.012,-time*.016),time*.023)).x;
 float n2=texture(sNoise,vec3(p*vec2(2.15,1.62)+vec2(time*.010,time*.006),time*.031)).x;
 float drag=p.x*(1.80+stream*.42)-time*(.050+stream*.085)+n0*.34;
 float lace=pow(sat(1.0-abs(fract(p.y*11.0+n0*2.2+
   sin(p.x*.85-time*.10)*.38)-.5)*2.0),8.5);
 float tongue=pow(sat(1.0-abs(fract(p.y*4.8+n1*1.7+drag*.24)-.5)*2.0),3.6)*
   smoothstep(.24,.86,n0+n2*.18);
 float torn=pow(sat(1.0-abs(fract(drag+n2*.40)-.5)*2.0),5.2)*
   smoothstep(.34,.92,stream+wave*.28+n1*.16);
 float pillow=pow(sat(n0*.62+n1*.30+n2*.18),1.65);
 float churn=smoothstep(.20,.86,stream*.58+wave*.38+n2*.20);
 float broken=smoothstep(.24,.84,n1+n2*.18);
 return sat(contact*(pillow*.36+lace*.28+tongue*.34+torn*.24+broken*.18)*churn);
}

vec4 flowOriginalMaterial(vec2 authoredUv, float layer, float intensity, float alphaBoost, float wetRock){
 vec4 originalBase=texture(sTex0_wrap,vec3(authoredUv,layer));
 if(intensity<=.001 && wetRock<=.001) {
  vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.85));
  vec3 col=originalBase.rgb*light;
  col=mix(uFogColor.rgb*originalBase.a,col,vFog);
  return vec4(col,originalBase.a);
 }
 float time=uModelMatrix[3].x*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0);
 float n0=texture(sNoise,vec3(authoredUv*vec2(2.2,9.5)+vec2(-time*.010,time*.030),time*.012)).x;
 float n1=texture(sNoise,vec3(authoredUv*vec2(8.5,3.0)+vec2(time*.022,-time*.006),time*.018)).x;
 float n2=texture(sNoise,vec3(authoredUv*vec2(15.0,13.0)+vec2(-time*.038,time*.020),time*.025)).x;
 float vertical=pow(sat(1.0-abs(fract(authoredUv.x*13.0+n0*.52-time*.030)-.5)*2.0),8.0);
 float fine=pow(sat(1.0-abs(fract(authoredUv.x*31.0+n1*.42-time*.065)-.5)*2.0),16.0);
 float plume=smoothstep(.34,.92,n0*.42+n1*.30+n2*.28+vertical*.24);
 float bloom=sat((vertical*.36+fine*.26+plume*.48+n2*.16)*intensity*TR_TOGGLE_FLOW_FOAM);
 vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.85+intensity*.35));
 float thread=sat(luma(originalBase.rgb)*1.45+originalBase.a*.50);
 float wetMask=sat(wetRock*smoothstep(.18,.86,thread+n0*.18)*(1.0-bloom*.38));
 float wetSpec=pow(sat(1.0-abs(fract(authoredUv.y*5.6+n1*.34+time*.018)-.5)*2.0),10.0)*
   smoothstep(.40,.94,n2+thread*.20)*wetMask;
 vec3 col=originalBase.rgb*light;
 vec3 foamColor=mix(vec3(.46,.63,.68),vec3(.88,.96,.98),sat(thread+bloom*.72));
 col=mix(col,foamColor,sat(bloom*(.12+.18*intensity)));
 col+=foamColor*bloom*(.020+.048*intensity);
 col=mix(col,col*vec3(.58,.74,.78)+vec3(.004,.016,.020),wetMask*.24);
 col+=vec3(.10,.16,.15)*wetSpec*(.025+.050*wetRock);
 col=mix(col,col*(1.0+intensity*.16)+vec3(.030,.026,.016)*thread*intensity,
   sat(intensity)*(1.0-bloom*.55));
 col=mix(uFogColor.rgb*originalBase.a,col,vFog);
 float alpha=clamp(originalBase.a*(1.0+alphaBoost*.12)+
   thread*.030*alphaBoost+bloom*.070*alphaBoost,0.0,1.0);
 return vec4(col,alpha);
}

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec3 captureColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

vec2 preciseReflectionUv(vec2 screen, vec3 normal, vec3 viewDir,
                         vec2 warp, float lift, float roughness){
 vec3 rn=normalize(normal);
 vec3 rv=normalize(viewDir);
 float ndv=sat(abs(dot(rn,rv)));
 float grazing=smoothstep(.14,.92,1.0-ndv);
 vec2 local=screen+vec2(warp.x,-warp.y*.35);
 vec2 mirror=vec2(screen.x,1.0-screen.y-lift);
 vec2 mirrorWarp=vec2(warp.x,-abs(warp.y))*(.64+.36*roughness);
 return mix(local,mirror+mirrorWarp,sat(.62+.38*grazing));
}

vec3 stableCaptureColor(vec2 uv, vec2 fallback){
 return mix(captureColor(fallback),captureColor(uv),reflectionEdgeFade(uv));
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.86,.96,1.04),.18);
 return clamp(c,vec3(0.0),vec3(3.0));
}

vec3 reflectionAutoBalance(vec3 c, vec3 waterTint, float activity){
 float y=luma(c);
 float dark=1.0-smoothstep(.035,.180,y);
 float hot=smoothstep(.70,1.45,y);
 vec3 lifted=mix(c,waterTint*.48+vec3(.020,.032,.038),dark*.38);
 vec3 compressed=lifted/(vec3(1.0)+lifted*(.18+.24*hot));
 vec3 balanced=mix(lifted,compressed,hot);
 balanced=mix(balanced,vec3(luma(balanced))*vec3(.82,.91,.96),
   sat(activity*.16+hot*.18));
 return clamp(balanced,vec3(0.0),vec3(2.35));
}

float lineMask(float x, float power){
 return pow(1.0-abs(fract(x)-.5)*2.0,power);
}

float caustic(vec2 p, float time){
 vec2 q=p;
 float n0=texture(sNoise,vec3(q*.72+vec2(.13,.41),time*.020)).x;
 float n1=texture(sNoise,vec3(q*1.36+vec2(.57,.08),time*.016)).x;
 float n2=texture(sNoise,vec3(q.yx*2.20+vec2(.24,.73),time*.013)).x;
 float veins=1.0-abs((n0*.52+n1*.34+n2*.14)-.53)*5.2;
 float split=1.0-abs(n1-n2)*4.7;
 float scatter=texture(sNoise,vec3(q*.42+vec2(.14,.27),time*.009)).x;
 float broken=smoothstep(.44,.86,texture(sNoise,vec3(q*2.65+vec2(.71,.19),time*.024)).x)*
   (.55+.45*smoothstep(.24,.82,scatter));
 float field=pow(sat(veins),2.20)*pow(sat(split),1.25);
 return smoothstep(.020,.50,field)*field*broken;
}

float currentStrands(vec2 p, float time){
 vec2 q=p;
 float lane=texture(sNoise,vec3(vec2(q.y*.62,q.x*.12)+vec2(.21,.37),time*.018)).x;
 float bend=sin(q.x*1.25+time*.42+lane*2.7)*.70;
 float lateral=sin(q.y*6.4+lane*3.8+bend);
 float strand=pow(sat(lateral*.5+.5),4.6);
 float broken=smoothstep(.34,.82,texture(sNoise,vec3(q*.92+vec2(.62,.14),time*.020)).x);
 float pulse=.46+.34*(sin(q.x*3.0-time*1.05+lane*2.3)*.5+.5);
 return strand*broken*pulse;
}

vec4 flowLaneField(vec2 p, float time){
 float noise=texture(sNoise,vec3(p*vec2(.36,1.18)+vec2(time*.030,0.0),time*.016)).x;
 float lanePhase=p.y*10.8+noise*3.4+sin(p.x*1.35+time*.22)*.58;
 float lane=pow(sat(sin(lanePhase)*.5+.5),4.8);
 float broken=smoothstep(.30,.84,texture(sNoise,vec3(p*vec2(1.35,.62)+vec2(time*.045,0.0),time*.020)).x);
 float pulse=.58+.34*(sin(p.x*3.1-time*.62+noise*2.4)*.5+.5);
 float mask=lane*broken*pulse*TR456_WATER_FLOW_LANE;
 vec2 bend=vec2(mask*(.020+.014*noise),cos(lanePhase)*mask*.006);
 return vec4(bend,mask,noise);
}

vec4 flowRibbonField(vec2 p, float time, float speed){
 float strength=clamp(TR456_WATER_FLOW_RIBBON,0.0,2.0);
 if(strength<=.001) return vec4(0.0);
 float n0=texture(sNoise,vec3(p*vec2(.32,1.05)+vec2(time*.026,0.0),time*.014)).x;
 float n1=texture(sNoise,vec3(p*vec2(1.20,.48)+vec2(-time*.018,time*.010),time*.020)).x;
 float sweep=time*(.10+speed*.08);
 float bend=sin(p.x*.92-sweep+n0*2.8)*.45+sin(p.x*2.10+sweep*.42)*.16;
 float phase=p.y*7.4+n0*2.6+bend;
 float broad=lineMask(phase*.52+p.x*.09,2.7);
 float narrow=lineMask(phase+n1*.42-p.x*.11,10.5);
 float runner=lineMask(p.x*2.65+n0*.35-time*(.10+speed*.12),4.2);
 float fine=lineMask(p.x*5.3+phase*.22+n1*.50-time*(.18+speed*.16),13.0);
 float broken=smoothstep(.28,.84,n1)*(.62+.28*runner+.10*fine);
 float crest=sat((broad*.38+narrow*.72+fine*.34)*broken)*strength;
 float trough=lineMask(phase*.73+1.12+n0*.25,3.1)*broken*strength;
 vec2 slope=vec2(crest*(.015+.010*n0),cos(phase)*crest*.010-trough*.006);
 return vec4(slope,crest,sat(max(trough,fine*.64*strength)));
}

vec4 flowCrossWaveField(vec2 p, float time, float speed){
 float strength=clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0);
 if(strength<=.001) return vec4(0.0);
 float n0=texture(sNoise,vec3(p*vec2(.86,.52)+vec2(time*.018,-time*.010),time*.018)).x;
 float n1=texture(sNoise,vec3(p*vec2(.48,1.14)+vec2(-time*.012,time*.016),time*.021)).x;
 float flowPhase=p.x*10.6+n0*2.7-time*(.24+speed*.17);
 float crossPhase=p.y*12.2+n1*2.5+time*(.16+speed*.10);
 float slowPhase=(p.x*.62+p.y*.48)+sin(p.y*.72+n0*2.0)*.35-time*.10;
 float flowWave=sin(flowPhase);
 float crossWave=sin(crossPhase);
 float slow=sin(slowPhase);
 float flowCrest=pow(sat(flowWave*.5+.5),3.2);
 float crossCrest=pow(sat(crossWave*.5+.5),3.5);
 float broken=smoothstep(.24,.84,texture(sNoise,vec3(p*1.35+vec2(n0,n1),time*.026)).x);
 float crest=sat(flowCrest*.44+crossCrest*.50+abs(slow)*.10)*broken*strength;
 vec2 slope=vec2(cos(flowPhase)*(.020+.006*n0)+cos(slowPhase)*.006,
   cos(crossPhase)*(.021+.006*n1)+cos(slowPhase)*.005)*broken*strength;
 float chop=sat(abs(flowWave-crossWave)*.42+crest*.58)*broken*strength;
 return vec4(slope,crest,chop);
}

vec4 flowAnisotropicGlintField(vec2 p, float time, float speed,
  float lane, float ribbon, float stream, float edgeHint){
 float n0=texture(sNoise,vec3(p*vec2(.40,1.52)+
   vec2(time*.018,-time*.006),time*.020)).x;
 float n1=texture(sNoise,vec3(p*vec2(1.08,.46)+
   vec2(-time*.014,time*.009),time*.022)).x;
 float n2=texture(sNoise,vec3(p*vec2(2.10,.78)+
   vec2(time*.011,0.0),time*.025)).x;
 float runner=lineMask(p.x*2.55+n1*.44-time*(.11+speed*.10),4.2);
 float gate=smoothstep(.05,.72,stream*.48+lane*.50+ribbon*.48+
   edgeHint*.22+n2*.14);
 float crossPhase=p.y*17.5+n0*2.5+
   sin(p.x*1.10-time*(.11+speed*.04))*.32;
 float broadPhase=p.y*6.4+n1*1.8+sin(p.x*.58-time*.07)*.24;
 float needle=lineMask(crossPhase,18.0);
 float sheet=lineMask(broadPhase,5.4);
 float broken=smoothstep(.24,.80,n2)*(.45+.55*runner);
 float mask=sat((needle*.78+sheet*.40)*broken*gate*1.85);
 float thin=sat(needle*broken*gate*1.55);
 vec2 slope=vec2((runner-.5)*.006+
   sin(p.x*3.2-time*(.18+speed*.10)+n0*2.0)*.004,
   cos(crossPhase)*.007+cos(broadPhase)*.004)*mask;
 return vec4(slope,mask,thin);
}

float flowEdgeStreamerFoam(vec2 p, float time, float edgeHint,
  float stream, float lane, float ribbon){
 float edge=smoothstep(.06,.62,edgeHint);
 float n0=texture(sNoise,vec3(p*vec2(.58,1.26)+
   vec2(time*.012,-time*.008),time*.019)).x;
 float n1=texture(sNoise,vec3(p*vec2(1.45,.64)+
   vec2(-time*.018,time*.010),time*.023)).x;
 float thread=lineMask(p.y*13.5+n0*1.8+
   sin(p.x*.82-time*.11)*.26,10.5);
 float drag=lineMask(p.x*2.1+n1*.50-time*(.09+stream*.10),3.4);
 float lace=pow(sat(1.0-abs(fract(p.x*.42+p.y*1.10+
   n0*.32-time*.020)-.5)*2.0),3.4);
 float breakup=smoothstep(.26,.82,n1+stream*.20);
 float flowGate=smoothstep(.10,.92,stream+lane*.42+ribbon*.32);
 return sat(edge*(thread*.52+drag*.34+lace*.16)*breakup*
   (.38+.62*flowGate));
}

float flowEddyFoamField(vec2 p, float time, float edgeHint, float swirlBody, float stream){
 float strength=clamp(TR456_WATER_FLOW_EDDY_FOAM,0.0,2.0);
 if(strength<=.001) return 0.0;
 vec2 q=p*.58;
 vec2 id=floor(q);
 vec2 cellNoise=vec2(hash12(id+vec2(5.2,1.7)),hash12(id+vec2(2.9,8.4)));
 vec2 center=(id+cellNoise)/.58;
 vec2 d=p-center;
 float r=length(d)+.001;
 float angle=atan(d.y,d.x);
 float life=fract(time*.085+hash12(id+vec2(7.7,4.1)));
 float fade=smoothstep(.04,.22,life)*(1.0-smoothstep(.72,1.0,life));
 float ring=exp(-abs(r-.72)*3.1)*(1.0-smoothstep(1.65,2.45,r));
 float lace=lineMask(angle*.42+r*1.24-time*.22+cellNoise.x*2.3,5.8);
 float breakup=smoothstep(.30,.86,texture(sNoise,vec3(p*1.65+cellNoise,time*.026)).x);
 float edge=sat(edgeHint*.74+swirlBody*.56+stream*.16);
 return ring*lace*breakup*fade*edge*strength;
}

vec3 flowSwirlField(vec2 p, float time){
 vec2 q=p*.46;
 vec2 id=floor(q);
 vec2 center=(id+vec2(hash12(id+vec2(2.1,7.4)),hash12(id+vec2(6.2,1.8))))/.46;
 vec2 d=p-center;
 float r=length(d)+.001;
 float life=fract(time*.10+hash12(id+vec2(4.7,5.3)));
 float fade=smoothstep(.04,.18,life)*(1.0-smoothstep(.72,1.0,life));
 float body=exp(-r*r*.45)*fade*smoothstep(.25,.90,hash12(id+vec2(9.1,3.3)))*
   TR456_WATER_FLOW_SWIRL;
 vec2 tangent=vec2(-d.y,d.x)/r*body*.018;
 return vec3(tangent,body);
}

vec3 flowSafeVolumeLayer(vec2 p, float time){
 float wobble=sin(p.y*2.15+time*.08)*.18+sin(p.y*4.10-time*.06)*.08;
 float pa=p.x*5.90+wobble+time*.16;
 float pb=p.x*3.15+p.y*.86+sin(p.y*2.0+time*.05)*.16-time*.10;
 float pc=p.x*9.40-p.y*.42+time*.22;
 float h=sin(pa)*.48+sin(pb)*.32+sin(pc)*.16;
 vec2 slope=vec2(cos(pa)*.48+cos(pb)*.25+cos(pc)*.16,
   cos(p.y*2.15+time*.08)*.055+cos(pb)*.070-cos(pc)*.030);
 float crest=sat(abs(h)*.54+pow(sat(abs(h)),3.0)*.28);
 return vec3(slope*.012,crest);
}

vec3 flowBreathField(vec2 p, float time, float speed){
 vec2 q=p;
 float a=q.x*7.2+sin(q.y*1.6+time*.18)*.38+time*(.62+speed*.30);
 float b=q.x*12.8-q.y*1.25+sin(q.y*2.4-time*.10)*.30+time*(1.02+speed*.46);
 float c=q.x*4.6+q.y*.62+time*(.38+speed*.18);
 float h=sin(a)*.50+sin(b)*.30+sin(c)*.20;
 vec2 slope=vec2(cos(a)*.50+cos(b)*.30+cos(c)*.20,
   cos(a)*.11-cos(b)*.22+cos(c)*.12);
 float crest=smoothstep(.34,.92,abs(h))*(.72+.28*smoothstep(.72,1.0,abs(h)));
 return vec3(slope*.014,crest);
}

vec3 waterDetailLayer(vec2 p, float time, float flowBias){
 float strength=clamp(TR456_WATER_DETAIL_STRENGTH*TR456_WATER_FLOW_DETAIL,0.0,2.2);
 if(strength<=.001) return vec3(0.0);
 float scale=max(TR456_WATER_DETAIL_SCALE*TR456_WATER_FLOW_DETAIL_SCALE,.12);
 vec2 q=p/scale;
 float n0=texture(sNoise,vec3(q*.84+vec2(time*.038,-time*.014),time*.022)).x;
 float n1=texture(sNoise,vec3(q*1.72+vec2(-time*.021,time*.031),time*.027)).x;
 float n2=texture(sNoise,vec3(q.yx*2.85+vec2(.17,.43)+time*.016,time*.020)).x;
 float longA=q.x*6.10+q.y*.72+n0*2.1-time*(.72+.30*flowBias);
 float longB=q.x*10.6-q.y*1.12+n1*2.4-time*(1.06+.42*flowBias);
 float cross=q.y*12.8+q.x*.55+n2*2.0+time*(.62+.18*flowBias);
 float vein=lineMask(longA*.22+n1*.20,3.2);
 float fine=lineMask(longB*.18+n2*.26,7.8);
 float crossLine=lineMask(cross*.16+n0*.18,5.6);
 float broken=smoothstep(.22,.82,n0*.46+n1*.36+n2*.18);
 float mask=sat((vein*.42+fine*.40+crossLine*.28)*broken)*strength;
 vec2 slope=vec2(cos(longA)*.010+cos(longB)*.008,
   cos(cross)*.009+cos(longA*.72)*.005)*mask;
 return vec3(slope,mask);
}

// Shared flow state for Ctrl+F1..F6 effects. Keep all UV motion tied to
// flowPos/flowTravel so foam, caustics, glints, refraction, and reflection stay in phase.
struct FlowEffectState {
 vec2 continuousUv;
 vec2 breathing;
 vec2 totalWarp;
 vec2 chromaOffset;
 vec2 causticUv;
 vec3 detail;
 float lane;
 float ribbon;
 float swirl;
 float signal;
 float foam;
 float caustic;
 float glint;
 float sheen;
 float mercuryLine;
 float mercuryBroad;
 float mercury;
 float streamWave;
 float streamCrest;
 float streamSpec;
 float crestLine;
};

FlowEffectState buildFlowEffectState(
  vec2 flowPos,
  vec2 flowDir,
  vec2 side,
  float flowTravel,
  float flowDetailTime,
  float speed,
  vec2 textureSlope,
  float seamGuard) {
 FlowEffectState f;
 f.continuousUv=flowPos*.00072+vec2(-flowTravel*.018,flowDetailTime*.012);
 float streamCoord=f.continuousUv.x;
 float laneCoord=f.continuousUv.y;

 float nA=texture(sNoise,vec3(f.continuousUv*1.15,
   flowDetailTime*.010)).x*2.0-1.0;
 float nB=texture(sNoise,vec3(f.continuousUv.yx*1.35+vec2(.23,.41),
   flowDetailTime*.009)).x*2.0-1.0;
 f.breathing=flowDir*nA+side*nB;

 vec2 minimalWarp=(textureSlope*.016+f.breathing*.0042)*
   clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,1.6)*seamGuard*
   TR_TOGGLE_FLOW_WARP;
 float laneNoise=texture(sNoise,vec3(f.continuousUv*vec2(.70,1.35),
   flowDetailTime*.018)).x;
 float strandNoise=texture(sNoise,vec3(f.continuousUv.yx*vec2(1.25,.58)+
   vec2(.19,.41),flowDetailTime*.021)).x;

 float streamWaveA=sin(f.continuousUv.x*13.4+laneNoise*1.7-flowDetailTime*.42);
 float streamWaveB=sin(f.continuousUv.x*27.0+f.continuousUv.y*1.6+
   strandNoise*2.1-flowDetailTime*.86);
 float streamWaveC=sin(f.continuousUv.x*18.0+f.continuousUv.y*4.2-
   flowDetailTime*.58);
 f.streamWave=(streamWaveA*.52+streamWaveB*.30+streamWaveC*.18)*
   clamp(TR456_WATER_FLOW_CROSS_WAVE,0.0,2.0)*TR_TOGGLE_FLOW_LANES;
 f.streamCrest=smoothstep(.22,.82,abs(f.streamWave));
 f.streamSpec=lineMask(laneCoord*9.5+f.streamWave*.38+
   sin(streamCoord*1.15-flowDetailTime*.10)*.22,14.0)*f.streamCrest;

 f.lane=lineMask(f.continuousUv.y*8.0+laneNoise*.42+
   sin(f.continuousUv.x*1.7-flowDetailTime*.16)*.22,8.0)*
   smoothstep(.22,.86,strandNoise)*TR456_WATER_FLOW_LANE*
   TR_TOGGLE_FLOW_LANES;
 f.ribbon=lineMask(laneCoord*3.6+laneNoise*.28+
   sin(streamCoord*.95-flowDetailTime*.10)*.20,5.2)*
   smoothstep(.34,.88,f.lane+strandNoise*.42)*
   TR456_WATER_FLOW_RIBBON*TR_TOGGLE_FLOW_LANES;
 f.swirl=texture(sNoise,vec3(f.continuousUv*2.10+
   vec2(flowDetailTime*.010,-flowDetailTime*.007),flowDetailTime*.026)).x;
 f.swirl=smoothstep(.44,.88,f.swirl)*TR456_WATER_FLOW_SWIRL*
   TR_TOGGLE_FLOW_LANES;
 f.detail=waterDetailLayer(f.continuousUv*1.55,flowDetailTime,speed);
 f.signal=sat(f.lane*.62+f.ribbon*.48+f.swirl*.28+
   f.detail.z*.30+f.streamCrest*.34);

 f.mercuryBroad=pow(sat(sin(laneCoord*3.15+
   streamCoord*.28+laneNoise*1.2-flowDetailTime*.16)*.5+.5),1.85)*
   smoothstep(.08,.72,f.signal+f.ribbon*.55+f.detail.z*.46)*
   TR_TOGGLE_FLOW_LANES;
 f.sheen=lineMask(laneCoord*4.9+streamCoord*.38+
   strandNoise*.34-flowDetailTime*.18,4.6)*
   smoothstep(.08,.72,f.signal+f.ribbon*.55+f.detail.z*.44)*
   TR_TOGGLE_FLOW_LANES;
 f.mercuryLine=lineMask(laneCoord*10.8+laneNoise*.36+
   strandNoise*.28+sin(streamCoord*1.45-flowDetailTime*.16)*.18,9.5)*
   smoothstep(.08,.70,f.lane+f.ribbon+f.detail.z*.55)*
   TR_TOGGLE_FLOW_LANES;
 f.mercury=sat(f.mercuryBroad*.46+f.sheen*.70+
   f.mercuryLine*.62+f.ribbon*.30+f.detail.z*.28);

 float foamPower=TR456_WATER_FLOW_STREAK_FOAM+
   TR456_WATER_FLOW_EDGE_FOAM*.42+TR456_WATER_FLOW_EDDY_FOAM*.36;
 f.crestLine=lineMask(laneCoord*6.4+laneNoise*.30+
   sin(streamCoord*1.20-flowDetailTime*.18)*.18,10.0)*
   smoothstep(.20,.82,f.signal);
 f.foam=sat((pow(sat(f.lane),1.25)*.52+
   pow(sat(f.ribbon),1.35)*.60+f.swirl*.30+f.crestLine*.55+
   f.detail.z*.30+f.streamCrest*.18+
   smoothstep(.30,.84,abs(nA-nB))*.20)*foamPower*
   TR_TOGGLE_FLOW_FOAM);

 f.causticUv=flowPos*.00072+vec2(-flowTravel*.024,flowDetailTime*.004)+
   f.breathing*.016;
 f.caustic=caustic(f.causticUv,flowDetailTime)*
   TR456_WATER_FLOW_CAUSTICS*TR_TOGGLE_FLOW_CAUSTICS;
 f.glint=(lineMask(laneCoord*5.2+strandNoise*.35+
   sin(streamCoord*1.35-flowDetailTime*.12)*.16,7.5)*f.signal*.135+
   f.streamSpec*.090+
   f.mercuryLine*.170+f.sheen*.145+f.mercuryBroad*.105+
   f.foam*.085+f.crestLine*.045+f.streamCrest*.035)*
   TR456_WATER_FLOW_GLINT*TR456_WATER_GLINT_STRENGTH;

 vec2 featureWarp=(flowDir*(f.ribbon*.007+f.signal*.004+
   f.streamWave*.009+f.streamCrest*.004+f.mercury*.005+
   f.detail.x*.005)+
   side*((f.lane-f.swirl*.45)*.006+f.streamWave*.004+
   (f.sheen-f.mercuryLine*.35)*.004+f.detail.y*.005))*
   seamGuard*TR_TOGGLE_FLOW_LANES;
 f.totalWarp=limitVec2(minimalWarp+
   featureWarp*clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,1.9)*
   TR_TOGGLE_FLOW_WARP,.030);
 f.chromaOffset=limitVec2(f.totalWarp*3.2+f.breathing*.0024+
   flowDir*f.streamWave*.003,.017);
 return f;
}

vec3 wakeLayer(vec2 screen, float time){
 vec2 p=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.35,1.0);
 float d=length(p)+.0001;
 float width=max(TR456_WATER_WAKE_WIDTH,.05);
 float wakeLen=max(TR456_WATER_WAKE_LENGTH,.08);
 float trail=(1.0-smoothstep(width*.14,width,abs(p.x)))*
   smoothstep(-.30,.02,p.y)*(1.0-smoothstep(.10,wakeLen,p.y));
 float ring=sin(d*78.0-time*6.8)*exp(-d*4.4)*smoothstep(.025,.24,d);
 float wake=sin((p.y-time*.28)*66.0+sin(p.x*17.0))*trail*exp(-abs(p.x)*5.0);
 float rear=smoothstep(-.050,.085,p.y)*(1.0-smoothstep(wakeLen*.58,wakeLen*1.28,p.y));
 float armX=p.y*(.34+.22*smoothstep(.05,wakeLen,p.y));
 float armWidth=max(width*.070,.018)+p.y*.030;
 float left=exp(-pow((p.x+armX)/armWidth,2.0))*rear;
 float right=exp(-pow((p.x-armX)/armWidth,2.0))*rear;
 float stem=exp(-pow(p.x/max(width*.18,.040),2.0))*rear*
   (1.0-smoothstep(wakeLen*.22,wakeLen*.82,p.y));
 float yPhase=p.y*76.0+abs(p.x)*24.0-time*8.4;
 float yWave=sin(yPhase+sin(p.x*10.0)*.26)*(left+right)*TR456_WATER_WAKE_WAVE;
 float stemWave=sin(p.y*88.0-time*9.3)*stem*TR456_WATER_WAKE_WAVE;
 vec2 yFlow=vec2((right-left)*(.024+.016*abs(sin(yPhase))),
   -(left+right)*(.017+.012*abs(cos(yPhase)))-stem*.016)*
   (yWave*.70+stemWave*.42);
 vec2 dir=normalize(p);
 vec2 flow=dir*ring*.028+vec2(p.x,-p.y)*wake*.014+yFlow;
 float crest=sat(abs(ring)*.78+abs(wake)*.55+abs(yWave)*.70+
   abs(stemWave)*.42+(left+right+stem)*.20);
 return vec3(flow*TR456_WATER_RIPPLE_STRENGTH*TR456_WATER_WAKE_STRENGTH,
   crest*TR456_WATER_WAKE_STRENGTH);
}

float flowIsScreenContact(vec4 c){
 return (1.0-step(-.001,c.z))*step(-.001,c.x)*step(c.x,1.001)*
   step(-.001,c.y)*step(c.y,1.001);
}

float flowContactAge(vec4 c){
 if(flowIsScreenContact(c)>.5) return max(abs(c.w)-1.0,0.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 return max(packedValue-radiusPacked*512.0-1.0,0.0);
}

float flowContactRadius(vec4 c){
 if(flowIsScreenContact(c)>.5)
   return max(abs(c.z),14.0)*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 return mix(720.0,radiusPacked,step(96.0,radiusPacked))*
   clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 screenContactWake(vec2 screen, vec2 invViewport, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,3.0);
 vec2 slope=vec2(0.0);
 float crest=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)))*flowIsScreenContact(c);
   float radius=flowContactRadius(c);
   vec2 inv=max(invViewport,vec2(1.0/8192.0));
   vec2 p=((screen-c.xy)/inv)/max(radius,1.0);
   float d=length(p)+.001;
   vec2 dir=p/d;
   float age=clamp(flowContactAge(c),0.0,160.0);
   float fade=active*(1.0-smoothstep(106.0,160.0,age));
   float grow=smoothstep(0.0,118.0,age);
   float ringCenter=mix(.12,.68,grow);
   float ringWidth=mix(.055,.085,grow);
   float leadX=(d-ringCenter)/ringWidth;
   float longX=(d-(ringCenter+.20+.14*grow))/(ringWidth*(2.40+.70*grow));
   float lead=exp(-leadX*leadX);
   float longRing=exp(-longX*longX)*sin(d*34.0-age*.060);
   float trail=smoothstep(-.050,.095,p.y)*
     (1.0-smoothstep(.52,1.35,p.y));
   float armX=p.y*(.34+.18*grow);
   float armWidth=.045+p.y*.035;
   float left=exp(-pow((p.x+armX)/armWidth,2.0))*trail;
   float right=exp(-pow((p.x-armX)/armWidth,2.0))*trail;
   float yWave=sin(p.y*62.0+abs(p.x)*18.0-time*6.8)*(left+right)*
     TR456_WATER_WAKE_WAVE;
    slope+=dir*(lead*.055+longRing*.040)*fade;
    slope+=vec2((right-left)*.044,-(left+right)*.030)*yWave*fade;
    crest+=sat(lead*.48+abs(longRing)*.42+abs(yWave)*.70)*fade;
  }
  return vec3(slope*1.25*strength*TR456_WATER_RIPPLE_STRENGTH,
    crest*1.18*strength*TR456_WATER_RIPPLE_STRENGTH);
}

vec3 worldContactWake(vec3 pos, float time){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,3.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 vec2 slope=vec2(0.0);
 float crest=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)))*(1.0-flowIsScreenContact(c));
   float radius=flowContactRadius(c);
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
   float age=clamp(flowContactAge(c),0.0,160.0);
   float fade=active*(1.0-smoothstep(106.0,160.0,age));
   float grow=smoothstep(0.0,118.0,age);
   float vertical=1.0-smoothstep(radius*.24,radius*1.30,abs(pos.y-c.y));
   vec4 motion4=uContactMotion[i];
   vec2 motion=mix(motion4.xz,motion4.xy,useXY);
   float motionLen=length(motion);
   float motionEnergy=max(smoothstep(2.0,28.0,motionLen),
     smoothstep(.010,.075,motionLen/max(radius,1.0)));
   vec2 moveDir=(motionLen>.001) ? motion/motionLen : -dir;
   vec2 trailDir=-moveDir;
   vec2 sideDir=vec2(-trailDir.y,trailDir.x);
   float trailAlong=dot(delta,trailDir)/max(radius,1.0);
   float trailSide=dot(delta,sideDir)/max(radius,1.0);
   float tail=motionEnergy*smoothstep(.05,.20,trailAlong)*
     (1.0-smoothstep(1.40,2.90,trailAlong))*vertical*fade;
   float armX=trailAlong*(.32+.10*grow);
   float armWidth=.090+max(trailAlong,0.0)*.050;
   float left=exp(-pow((trailSide+armX)/armWidth,2.0))*tail;
   float right=exp(-pow((trailSide-armX)/armWidth,2.0))*tail;
   float stem=exp(-pow(trailSide/.14,2.0))*tail*
     (1.0-smoothstep(.30,1.20,trailAlong));
   float yWave=sin(trailAlong*50.0+abs(trailSide)*17.0-time*1.45-
     age*(.060+.020*speed))*(left+right)*TR456_WATER_WAKE_WAVE;
   float stemWave=sin(trailAlong*62.0-time*1.60-age*(.080+.024*speed))*
     stem*TR456_WATER_WAKE_WAVE;
   float tensionCenter=radius*(.32+.06*grow);
   float tensionWidth=max(radius*.070,18.0);
   float tensionX=(d-tensionCenter)/tensionWidth;
   float meniscus=exp(-tensionX*tensionX)*vertical*fade;
   float dMeniscus=(-2.0*tensionX/tensionWidth)*meniscus;
   slope+=dir*dMeniscus*.28*strength;
   slope+=(sideDir*(right-left)*.036-
     trailDir*(left+right+stem)*.024)*(yWave*.96+stemWave*.58)*
     strength*TR456_WATER_WAKE_STRENGTH;
   crest+=sat(abs(yWave)*.58+abs(stemWave)*.34+meniscus*.30)*
     strength*TR456_WATER_WAKE_STRENGTH;
 }
 return vec3(slope*1.35*TR456_WATER_RIPPLE_STRENGTH,
   crest*1.20*TR456_WATER_RIPPLE_STRENGTH);
}

void main(){
 vec2 uv=vTexCoord;
 vec3 n=normalize(vNormal);
 vec3 viewVec=normalize(-vPos);
 float time=uModelMatrix[3].x;
 vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;
 float flowTime=time*clamp(TR456_WATER_FLOW_SPEED,0.20,35.0);
 float duplicatePass=step(0.5,uTrWaterDrawInfo.w);
 float passMotion=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,1.0),duplicatePass);
 float passOpacity=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.0),duplicatePass);
 float passReflection=mix(1.0,clamp(TR456_WATER_FLOW_SECONDARY_REFLECTION,0.0,1.0),duplicatePass);
 flowTime*=passMotion;
 vec2 flowVector=uParams.xy;
 float flowLen=length(flowVector);
 float authoredFlowPresent=step(.000001,flowLen);
 float flowActive=1.0;
 float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 flowDir=((flowLen>.000001) ? flowVector/flowLen : normalize(vec2(.92,.38)))*flowSign;
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float speed=max(flowLen,0.22);
 float rapid=sat((TR456_WATER_FLOW_SPEED-1.0)*.34+
   TR456_WATER_FLOW_RIDGE*.10+TR456_WATER_FLOW_STREAK_FOAM*.08+
   TR456_WATER_FLOW_BREAKUP*.08);
 float flowTravel=flowTime*(.55+speed*.35)*(1.0+rapid*.26)*flowActive;
 float flowDetailTime=flowTime*(.16+rapid*.035);
 vec2 authoredUv=uv;
 vec3 worldPos=vPos+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vec2 flowPos=vec2(dot(worldPos.xz,flowDir),dot(worldPos.xz,side));
 vec3 facet=polygonFacetField(worldPos.xz,flowDir,flowDetailTime,1.0)*passMotion;
 vec2 facetLocal=vec2(dot(facet.xy,flowDir),dot(facet.xy,side));
 float facetEnergy=sat(abs(facet.z)*.95+length(facet.xy)*2.4);
 vec2 stableUv=flowPos*.00072+vec2(-flowTravel*.018,flowDetailTime*.012);
 vec2 flowScreenDir=normalize(vec2(flowDir.x,-flowDir.y)+vec2(.0001));
 vec2 flowScreenSide=vec2(-flowScreenDir.y,flowScreenDir.x);
 float flowBake=clamp(TR456_WATER_FLOW_SINGLE_LAYER,0.0,1.0);
 float sampleLayer=mix(vLayer,0.0,flowBake);
 float seamGuard=tileSeamGuard(uv);
 float seamFade=1.0-seamGuard;

 if(flowOriginalBypass()>.5) {
   fragColor=flowOriginalMaterial(authoredUv,vLayer,.0,.0,.0);
   return;
 }

#if TR456_WATER_DEBUG_MODE == 0
 float flowOnlyToggles=max(
   max(max(uTrWaterToggle0.x,uTrWaterToggle0.y),
       max(uTrWaterToggle0.z,uTrWaterToggle0.w)),
   max(TR_TOGGLE_FLOW_WARP,TR_TOGGLE_FLOW_REFLECTION));
 float nonFlowToggles=max(uTrWaterToggle2.z,uTrWaterToggle2.w);
 float minimalFlowMode=flowOnlyToggles*(1.0-step(.5,nonFlowToggles));
 if(minimalFlowMode>.5) {
   vec4 originalBase=texture(sTex0_wrap,vec3(authoredUv,sampleLayer));
   vec2 du=vec2(.0025,0.0);
   vec2 dv=vec2(0.0,.0025);
   float hL=luma(texture(sTex0_wrap,vec3(authoredUv-du,sampleLayer)).rgb);
   float hR=luma(texture(sTex0_wrap,vec3(authoredUv+du,sampleLayer)).rgb);
   float hD=luma(texture(sTex0_wrap,vec3(authoredUv-dv,sampleLayer)).rgb);
   float hU=luma(texture(sTex0_wrap,vec3(authoredUv+dv,sampleLayer)).rgb);
    vec2 textureSlope=vec2(hR-hL,hU-hD);
    FlowEffectState fx=buildFlowEffectState(flowPos,flowDir,side,
      flowTravel,flowDetailTime,speed,textureSlope,seamGuard);
    float flowBumpAmount=clamp(TR456_WATER_BUMP_STRENGTH*
      TR456_WATER_FLOW_BUMP_STRENGTH,0.0,2.2);
    vec2 fxBumpSlope=limitVec2(
      flowScreenDir*(fx.detail.x*15.0+fx.ribbon*.052+
        fx.streamWave*.030+fx.streamCrest*.016+fx.mercuryLine*.020)+
      flowScreenSide*(fx.detail.y*15.0+(fx.lane-fx.swirl*.45)*.040+
        fx.streamWave*.018+(fx.sheen-fx.mercuryLine*.35)*.018),
      .38)*max(TR456_WATER_BUMP_SCALE,.10)*flowBumpAmount;
    n=applyFlowBump(n,fxBumpSlope,.78);
    float fxBumpEnergy=sat(length(fxBumpSlope)*2.80);
    vec4 warpedBase=texture(sTex0_wrap,vec3(authoredUv+fx.totalWarp,sampleLayer));
    float flowChroma=clamp(max(TR456_WATER_CHROMA_STRENGTH,.22)*
      TR456_WATER_FLOW_CHROMA*2.20*TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
    vec3 chromaTex=vec3(
      texture(sTex0_wrap,vec3(authoredUv+fx.chromaOffset,sampleLayer)).r,
      warpedBase.g,
      texture(sTex0_wrap,vec3(authoredUv-fx.chromaOffset,sampleLayer)).b);
    float ndv=sat(dot(n,viewVec));
    float grazing=smoothstep(.18,.72,1.0-ndv);
    float fres=sat(.04+pow(1.0-ndv,4.0)*.70);
    vec3 tex=mix(originalBase.rgb,warpedBase.rgb,.44);
    tex=mix(tex,chromaTex,.34*flowChroma);
    vec3 mercuryTint=mix(vec3(.075,.205,.195),vec3(.275,.300,.220),
      sat(fx.mercuryLine*.72+fx.sheen*.45+fx.mercuryBroad*.40+grazing*.28));
    tex=mix(tex,tex+mercuryTint*.30,fx.mercury*.72);
    tex=mix(tex,tex*vec3(.88,.95,.92)+vec3(.008,.026,.026),
      sat(fx.signal*.13+fx.mercury*.08+fx.caustic*.045));
    vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.65));
    vec3 col=tex*light;
    vec2 screenWarp=limitVec2(
      flowScreenDir*(fx.totalWarp.x*1.85+fx.ribbon*.008+fx.streamWave*.013+
        fx.streamCrest*.007+fx.detail.x*.008)+
      flowScreenSide*(fx.totalWarp.y*1.95+(fx.lane-fx.swirl*.45)*.007+
        fx.streamWave*.005+fx.detail.y*.008)+
      fxBumpSlope*(.008+.006*grazing),
      .038)*TR_TOGGLE_FLOW_WARP;
    vec2 refrUv=screen+vec2(screenWarp.x,-screenWarp.y);
    vec2 refrUv2=screen+vec2(screenWarp.x*2.05,-screenWarp.y*2.05);
    vec3 sceneRefract=mix(captureColor(refrUv),captureColor(refrUv2),.45);
    float refrEdge=mix(.52,1.0,min(reflectionEdgeFade(refrUv),
      reflectionEdgeFade(refrUv2)));
    float refrMask=clamp((.10+.24*fx.signal+.12*fx.caustic+
      .08*grazing+.04*fxBumpEnergy)*refrEdge*
      TR_TOGGLE_FLOW_WARP,0.0,.38);
    col=mix(col,sceneRefract,refrMask);
    vec3 stableFlowTint=mix(vec3(.018,.072,.082),vec3(.026,.095,.108),
      sat(fx.signal*.65+grazing*.20+fxBumpEnergy*.20));
    float flowTintHold=sat(originalBase.a*
      (.045+.075*fx.signal+.030*grazing+.025*fxBumpEnergy));
    col=mix(col,col*vec3(.84,1.00,1.08)+stableFlowTint*light*.34,flowTintHold);
    float ribbonFoam=sat(pow(sat(fx.ribbon),1.35)*TR456_WATER_FLOW_STREAK_FOAM*
      TR_TOGGLE_FLOW_FOAM);
    float tensionPatch=sat((fx.mercuryBroad*.40+fx.sheen*.24+fx.detail.z*.18)*
      TR456_WATER_FLOW_SURFACE_TENSION);
    col=mix(col,col*vec3(.88,1.01,1.07)+stableFlowTint*light*.25,
      tensionPatch*.08);
    col=mix(col,vec3(.60,.75,.76),ribbonFoam*.08);
    col+=vec3(.10,.18,.20)*ribbonFoam*.030;
    col+=vec3(.040,.105,.096)*(fx.signal+fx.mercury*.38)*
      TR456_WATER_FLOW_STRENGTH;
    col+=vec3(.13,.28,.22)*fx.caustic*.18;
    col+=vec3(.86,.82,.50)*(fx.glint+fxBumpEnergy*.010);
    col+=vec3(.150,.255,.215)*fx.mercury*
      TR456_WATER_FLOW_GLINT*TR456_WATER_GLINT_STRENGTH;
    col+=vec3(.235,.255,.185)*(fx.mercuryBroad*.18+fx.sheen*.14+
      fx.mercuryLine*.16)*TR456_WATER_FLOW_GLINT*TR456_WATER_GLINT_STRENGTH;
    col+=vec3(.24,.25,.18)*fx.foam*.34*TR456_WATER_FOAM_STRENGTH;
    col+=vec3(.025,.060,.058)*(fx.detail.z+fx.streamCrest*.45)*
      TR456_WATER_FLOW_STRENGTH;
    col=mix(col,col*vec3(.80,.88,.84),sat(fx.ribbon*.10+fx.swirl*.08));
    float flowReflectionActive=0.0;
    if(flowReflectionActive>.004) {
     vec2 reflFlowUv=flowPos*.00072+vec2(-flowTravel*.014,flowDetailTime*.010);
     float rn0=texture(sNoise,vec3(reflFlowUv,flowDetailTime*.008)).x*2.0-1.0;
     float rn1=texture(sNoise,vec3(reflFlowUv.yx*1.41+vec2(.19,.41),
       flowDetailTime*.011)).x*2.0-1.0;
     vec2 continuousReflect=limitVec2((flowScreenDir*(rn0+fx.streamWave*.65+
       fx.mercury*.36)+flowScreenSide*(rn1+fx.streamCrest*.28+
       (fx.sheen-fx.mercuryLine*.35)*.28))*
       (.0058+.0105*fres)*TR_TOGGLE_FLOW_REFLECTION,.024);
     float waterDown=smoothstep(.28,.96,screen.y);
     float localLift=(.030+.055*grazing+.030*waterDown+.018*fres)*
       TR_TOGGLE_FLOW_REFLECTION;
     vec2 liftedUv=screen+vec2(0.0,-localLift);
     vec2 mirrorWarp=vec2(continuousReflect.x,-abs(continuousReflect.y))*
       (.62+.48*fres);
     vec2 mirrorWarp2=mirrorWarp*.35+
       vec2(flowScreenSide.x,-abs(flowScreenSide.y))*(.003+.010*fres)*rn1;
     vec3 reflectNormal=normalize(n+vec3(continuousReflect.x*16.0,0.0,
       continuousReflect.y*16.0));
     vec2 mirrorBase=preciseReflectionUv(screen,reflectNormal,viewVec,
       vec2(0.0),localLift*(.84+.28*grazing),.20+.42*fres);
     vec2 mirrorUv=preciseReflectionUv(screen,reflectNormal,viewVec,
       mirrorWarp,localLift,.54+.46*fres);
     vec2 mirrorUv2=preciseReflectionUv(screen,reflectNormal,viewVec,
       mirrorWarp2,localLift*(1.20+.20*grazing),.42+.38*fres);
     vec2 px=captureInvViewport();
     vec2 roughSide=flowScreenSide*px*(12.0+24.0*fres);
     vec2 roughDir=flowScreenDir*px*(8.0+18.0*fres);
     vec3 reflTight=stableCaptureColor(mirrorUv,screen)*.38+
       stableCaptureColor(mirrorUv2,screen)*.22+
       stableCaptureColor(mirrorUv+roughSide,screen)*.14+
       stableCaptureColor(mirrorUv-roughSide,screen)*.14+
       stableCaptureColor(mirrorUv2+roughDir,screen)*.06+
       stableCaptureColor(mirrorUv2-roughDir,screen)*.06;
     vec3 reflWide=stableCaptureColor(mirrorUv+roughSide*1.65,screen)*.22+
       stableCaptureColor(mirrorUv-roughSide*1.65,screen)*.22+
       stableCaptureColor(mirrorUv2+roughDir*1.85,screen)*.18+
       stableCaptureColor(mirrorUv2-roughDir*1.85,screen)*.18+
       stableCaptureColor(mirrorBase,screen)*.20;
     float reflectionTileDetail=length(reflTight-reflWide);
     vec3 sceneRefl=reflectionGrade(mix(reflTight,reflWide,.60+.26*(1.0-grazing)));
     sceneRefl=mix(sceneRefl,reflectionGrade(stableCaptureColor(liftedUv,screen)),.34);
     sceneRefl=mix(sceneRefl,reflectionGrade(stableCaptureColor(
       screen+vec2(0.0,-localLift*(1.45+.30*grazing)),screen)),.12*grazing);
     sceneRefl=mix(sceneRefl,vec3(luma(sceneRefl))*vec3(.82,.90,.88),.10*(1.0-grazing));
     sceneRefl=mix(sceneRefl,sceneRefl+mercuryTint*.16,fx.mercury*(.18+.34*grazing));
     sceneRefl=reflectionAutoBalance(sceneRefl,mercuryTint,
       sat(fx.signal*.24+fx.foam*.20+fx.mercury*.18));
     float detailFade=mix(.48,1.0,1.0-smoothstep(.10,.34,reflectionTileDetail));
     float reflectContinuity=mix(.86,1.0,seamGuard);
     float reflEdge=mix(.55,1.0,min(min(reflectionEdgeFade(mirrorBase),
       reflectionEdgeFade(mirrorUv)),reflectionEdgeFade(mirrorUv2)));
     float reflOk=mix(.60,1.0,smoothstep(.004,.045,luma(sceneRefl)))*
       reflEdge;
     float reflMask=clamp((.075+.215*fres+fx.mercury*.045)*
       flowReflectionActive*reflOk*detailFade*reflectContinuity*
       (.22+.78*grazing),0.0,.34);
     col=mix(col,sceneRefl,reflMask);
    }
    col=mix(uFogColor.rgb*originalBase.a,col,vFog);
    float flowAlpha=clamp(originalBase.a+
      (fx.foam*.035+fx.signal*.012+fx.caustic*.006)*originalBase.a,
      .0,.68);
    fragColor=vec4(col,flowAlpha);
   return;
 }
#endif

 float noiseA=texture(sNoise,vec3(stableUv*1.18,flowDetailTime*.030)).x;
 float noiseB=texture(sNoise,vec3(stableUv.yx*1.72+vec2(.17,.31),flowDetailTime*.024)).x;
 float noiseC=texture(sNoise,vec3(stableUv*2.55+vec2(0.0,flowDetailTime*.008),flowDetailTime*.040)).x;
 float strands=currentStrands(stableUv,flowDetailTime);
 float stream=max(lineMask(stableUv.x*5.2+noiseA*.26-flowTravel*(.06+speed*.10+rapid*.08),7.5)*.30,strands*.58);
 float currentA=sin(stableUv.x*14.5+noiseA*2.1-flowTravel*(.20+speed*.25));
 float currentB=sin(stableUv.x*7.2+stableUv.y*2.0+noiseB*1.6-flowTravel*(.16+speed*.20));
 float current=currentA*.56+currentB*.44;
 float cross=sin(stableUv.y*18.0+noiseB*3.4+flowDetailTime*(.75+speed*.42))*0.5+0.5;
 float chop=sin(stableUv.x*32.0+noiseC*3.8-flowTravel*(.44+speed*.55+rapid*.32));
 float small=sin(stableUv.y*38.0+stableUv.x*12.0-flowTravel*(.28+rapid*.18));
 float lateralA=sin(stableUv.y*22.0+noiseB*2.2-flowTravel*(.32+speed*.22));
 float lateralB=sin(stableUv.x*5.0-stableUv.y*7.0+noiseC*3.0-flowTravel*(.58+speed*.30));
 float wave=sat(abs(current)*(.34+rapid*.08)+abs(chop)*(.17+rapid*.10)+
   abs(small)*(.045+rapid*.030)+stream*(.24+rapid*.08)+
   strands*.18+facetEnergy*(.22+rapid*.08));
 float streakLane=lineMask(stableUv.y*10.5+noiseB*.20+sin(stableUv.x*1.35-flowTravel*.10)*.10,14.0);
 float streakBreak=smoothstep(.50,.86,texture(sNoise,vec3(stableUv*vec2(1.65,.72)+
   vec2(0.0,flowDetailTime*.014),flowDetailTime*.018)).x);
 float streakPulse=lineMask(stableUv.x*2.15+noiseA*.30-flowTravel*(.08+speed*.10),5.0);
 float streakFoam=streakLane*streakBreak*(.36+.64*streakPulse)*
   smoothstep(.26,.82,stream+strands*.45+wave*.22)*
   TR456_WATER_FLOW_STREAK_FOAM*.42;
 vec4 flowLane=flowLaneField(stableUv,flowDetailTime);
 vec4 ribbon=flowRibbonField(stableUv,flowDetailTime,speed);
 vec4 crossWaveField=flowCrossWaveField(stableUv,flowDetailTime,speed);
 vec3 swirl=flowSwirlField(stableUv,flowDetailTime);
 vec3 safeVolume=flowSafeVolumeLayer(stableUv,flowDetailTime)*
   TR456_WATER_SAFE_VOLUME*TR456_WATER_FLOW_STRENGTH;
 vec3 flowBreath=flowBreathField(stableUv,flowDetailTime,speed)*
   TR456_WATER_FLOW_STRENGTH;
 vec2 detailPos=flowPos*.00075+vec2(flowTravel*.020,flowDetailTime*.012);
 vec3 detailTex=waterDetailLayer(detailPos,flowDetailTime,speed)*passMotion;
  flowLane*=TR_TOGGLE_FLOW_LANES;
 ribbon*=TR_TOGGLE_FLOW_LANES;
 crossWaveField*=TR_TOGGLE_FLOW_LANES;
 swirl*=TR_TOGGLE_FLOW_LANES;
 stream=max(stream,max(flowLane.z*.46,ribbon.z*.52));
 wave=sat(wave+flowLane.z*.34+ribbon.z*.28+swirl.z*.22+
   crossWaveField.z*(.34+rapid*.18)+safeVolume.z*.38+
   flowBreath.z*(.54+rapid*.18)+detailTex.z*(.42+rapid*.16));
 streakFoam+=flowLane.z*.0045*TR456_WATER_FLOW_STREAK_FOAM+swirl.z*.004+
   flowBreath.z*.004+ribbon.z*.0042*TR456_WATER_FLOW_STREAK_FOAM+
   crossWaveField.z*.0028*TR456_WATER_FLOW_STREAK_FOAM+
   rapid*(flowBreath.z*.004+crossWaveField.w*.0035+ribbon.z*.003);
 streakFoam*=TR_TOGGLE_FLOW_FOAM;
 stream*=passMotion;
 current*=passMotion;
 chop*=passMotion;
 small*=passMotion;
 cross=mix(0.5,cross,passMotion);
 wave*=passMotion;
 streakFoam*=passMotion;
 flowLane*=passMotion;
 ribbon*=passMotion;
 crossWaveField*=passMotion;
 swirl*=passMotion;
 safeVolume*=passMotion;
 flowBreath*=passMotion;

 vec3 wake=vec3(0.0);
  vec3 spriteWake=screenContactWake(screen,invViewport,flowTime)*
    TR_TOGGLE_CONTACT_RIPPLES;
  vec3 worldWake=worldContactWake(worldPos,flowTime)*
    TR_TOGGLE_CONTACT_RIPPLES;
  vec3 contactWave=vContactWave*TR_TOGGLE_CONTACT_RIPPLES;
  wake*=passMotion;
  spriteWake*=passMotion;
  worldWake*=passMotion;
  contactWave*=passMotion;
  wake+=spriteWake;
  wake+=worldWake;
  contactWave.xy*=.42;
  contactWave.z*=.58;
  float contactHeight=smoothstep(.018,.22,abs(contactWave.z))*abs(contactWave.z);
 float spriteHeight=abs(spriteWake.z)+abs(worldWake.z);
 float surfaceMotion=sat(spriteHeight*.90+contactHeight*.85);
 wake.z+=contactHeight*.55;
 wave=sat(wave+contactHeight*.30+spriteHeight*.36+surfaceMotion*.08);
 vec2 wakeFlow=vec2(dot(wake.xy,flowScreenDir),dot(wake.xy,flowScreenSide));
 vec2 originalRipple=vec2(currentA*.015+currentB*.012+chop*.004+stream*.010+strands*.014,
    (noiseA-noiseB)*.011+(cross-.5)*.008)+wakeFlow*.62;
 originalRipple+=contactWave.xy*(.46*TR456_WATER_CONTACT_NORMAL_STRENGTH);
 vec2 shapedRipple=originalRipple;
 shapedRipple+=vec2(lateralB*.006,lateralA*.014+lateralB*.010)*
  clamp(TR456_WATER_FLOW_CROSS_DISTORTION,0.0,3.0);
shapedRipple+=safeVolume.xy*1.18;
shapedRipple+=flowBreath.xy*2.10;
shapedRipple+=detailTex.xy*2.20;
shapedRipple+=flowLane.xy*1.25+ribbon.xy*1.45+
   crossWaveField.xy*1.65+swirl.xy*1.10;
 shapedRipple+=facetLocal*.030*clamp(TR456_WATER_POLYGONAL_NORMAL,0.0,2.0);
 shapedRipple+=contactWave.xy*(.38*TR456_WATER_CONTACT_NORMAL_STRENGTH);
 float originalDeform=clamp(TR456_WATER_FLOW_ORIGINAL_DEFORMATION,0.0,1.0);
 vec2 ripple=mix(shapedRipple,originalRipple,originalDeform);
 vec2 surfaceRippleBase=mix(shapedRipple,originalRipple*.88,originalDeform*.58);
 ripple*=TR456_WATER_FLOW_STRENGTH*TR456_WATER_SURFACE_WAVE*
   TR456_WATER_REFRACT_STRENGTH*TR456_WATER_REFRACTION_WAVE_STRENGTH;
 surfaceRippleBase*=TR456_WATER_FLOW_STRENGTH*TR456_WATER_SURFACE_WAVE*
   TR456_WATER_REFRACT_STRENGTH*TR456_WATER_REFRACTION_WAVE_STRENGTH;
 ripple*=passMotion;
 surfaceRippleBase*=passMotion;
 ripple*=seamGuard;
 surfaceRippleBase*=seamGuard;

  vec2 refractRipple=limitVec2(
    ripple*clamp(TR456_WATER_FLOW_REFRACTION_WARP,0.0,1.8)*TR_TOGGLE_FLOW_WARP,
    .092);
  vec2 surfaceRipple=limitVec2(
    surfaceRippleBase*clamp(TR456_WATER_FLOW_SURFACE_DISTORTION,0.0,3.0),
    .138);
  vec2 seamRipple=refractRipple*seamGuard;
  vec2 flowOffset=flowDir*seamRipple.x+side*seamRipple.y;
  vec2 texUv=authoredUv+flowOffset*.22+
    vec2(current*.004*seamGuard,(cross-.5)*.003*seamGuard);
  vec4 meshBase=texture(sTex0_wrap,vec3(authoredUv,sampleLayer));
  vec4 worldBase=texture(sTex0_wrap,vec3(texUv,sampleLayer));
  vec4 base=mix(worldBase,meshBase,.94+.06*seamFade);
 float breakupNoise=texture(sNoise,vec3(stableUv*vec2(.74,1.18)+
   vec2(flowDetailTime*.012,-flowDetailTime*.009),flowDetailTime*.017)).x;
 float breakupDetail=texture(sNoise,vec3(stableUv*vec2(2.20,.92)+
   vec2(-flowDetailTime*.016,flowDetailTime*.011),flowDetailTime*.029)).x;
 float sheetBreak=sat((abs(breakupNoise-breakupDetail)*1.35+
   crossWaveField.w*.34+ribbon.w*.20+swirl.z*.18)*
   clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0));
  float sourceDetail=sat(luma(base.rgb)*1.45+.08*wave+.04*stream+
    sheetBreak*.10);
  vec3 bakedBase=mix(vec3(.040,.052,.047),
    vec3(sourceDetail)*vec3(.46,.49,.43)+vec3(.025,.030,.027),.52);
  base.rgb=mix(base.rgb,bakedBase,flowBake*(.18+.06*(1.0-sheetBreak)));
   float waterCoverage=clamp(mix(.80,meshBase.a,.12)-sheetBreak*.045*seamGuard+
     crossWaveField.z*.012*seamGuard,.58,.96);
 base.a=waterCoverage;
 float flowChroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA*
   TR_TOGGLE_FLOW_CHROMA,0.0,1.0);
  vec2 chromaUv=authoredUv+flowOffset*.16;
  vec3 r=mix(
    texture(sTex0_wrap,vec3(chromaUv+seamRipple*(.18+flowChroma*.04)+vec2(0.0,.0004)*flowChroma*seamGuard,sampleLayer)).rgb,
    texture(sTex0_wrap,vec3(chromaUv+seamRipple*(.08+flowChroma*.03)+vec2(.0004,0.0)*flowChroma*seamGuard,sampleLayer)).rgb,
    .35);
  vec3 b=mix(
    texture(sTex0_wrap,vec3(chromaUv-seamRipple*(.14+flowChroma*.04)-vec2(0.0,.0004)*flowChroma*seamGuard,sampleLayer)).rgb,
    texture(sTex0_wrap,vec3(chromaUv-seamRipple*(.06+flowChroma*.03)-vec2(.0004,0.0)*flowChroma*seamGuard,sampleLayer)).rgb,
    .35);
 vec3 tex=mix(base.rgb,vec3(r.r,base.g,b.b),.22*flowChroma);
  vec3 syncedTex=mix(vec3(luma(tex))*vec3(.50,.78,.84),tex,.95);
  tex=mix(syncedTex,tex,mix(clamp(TR456_WATER_TEXTURE_STRENGTH*.62,.45,.84),.90,flowBake));
  tex=mix(vec3(luma(tex)),tex,clamp(TR456_WATER_TEXTURE_STRENGTH*.95,.72,1.0));
 tex=mix(tex,tex*vec3(.70,.78,.75)+vec3(.010,.026,.024),sheetBreak*.24);
 float flowDepth=sat(((1.0-vFog)*.46+(1.0-waterCoverage)*.16+
    stream*.08+wave*.10+sheetBreak*.10)*
    TR456_WATER_DEPTH_STRENGTH);
tex=flowVolumeAbsorption(tex,flowDepth);
tex+=vec3(.020,.060,.066)*(safeVolume.z*.70+flowBreath.z*.80)*(.35+.65*vFog);
tex+=vec3(.018,.065,.072)*detailTex.z*(.32+.68*vFog);
 float bankContact=smoothstep(.055,.34,1.0-waterCoverage+flowDepth*.14);
 float shoreFoam=0.0;
 if(TR_TOGGLE_FLOW_FOAM>.001) {
   shoreFoam=flowShoreFoamMask(stableUv,waterCoverage,flowDepth,stream,flowDetailTime)*.46;
 }
 float bankFoamVolume=0.0;
 if(TR_TOGGLE_FLOW_FOAM*TR456_WATER_FLOW_EDGE_FOAM>.001 && bankContact>.001) {
   bankFoamVolume=flowBankFoamVolume(stableUv,waterCoverage,flowDepth,
     stream,wave,flowDetailTime)*TR_TOGGLE_FLOW_FOAM*passMotion;
 }
 float edgeHint=sat(shoreFoam*.82+bankContact*.58+bankFoamVolume*.46+
   swirl.z*.42+ribbon.w*.22);
 float edgeCalm=sat(edgeHint*.70+bankContact*.52+bankFoamVolume*.32);
 float coreMask=1.0-smoothstep(.22,.82,edgeCalm);
 float coreRush=rapid*coreMask*TR_TOGGLE_FLOW_LANES*passMotion;
 stream*=mix(.78,1.0+coreRush*.18,coreMask);
 wave=sat(wave*mix(.84,1.0+coreRush*.22,coreMask));
 streakFoam*=mix(.70,1.0+coreRush*.36,coreMask);
 flowLane*=mix(.82,1.0+coreRush*.16,coreMask);
 ribbon*=mix(.84,1.0+coreRush*.20,coreMask);
 crossWaveField*=mix(.78,1.0+coreRush*.25,coreMask);
 float eddyFoam=flowEddyFoamField(stableUv,flowDetailTime,edgeHint,swirl.z,stream)*
   TR_TOGGLE_FLOW_FOAM*passMotion;
 float edgeStreamFoam=flowEdgeStreamerFoam(stableUv,flowDetailTime,edgeHint,
   stream,flowLane.z,ribbon.z)*TR_TOGGLE_FLOW_FOAM*passMotion;
 vec4 anisoGlint=flowAnisotropicGlintField(stableUv,flowDetailTime,speed,
   flowLane.z,ribbon.z,stream,edgeHint)*passMotion;
 float submergedCausticMask=smoothstep(.10,.70,flowDepth)*
   (1.0-smoothstep(.74,1.0,streakFoam+edgeStreamFoam*.55));
 vec2 causticUv=stableUv+vec2(-flowTravel*(.032+speed*.010),sin(flowDetailTime*.20)*.010)+
   surfaceRipple*.16;
 float caust=0.0;
 if(TR456_WATER_CAUSTICS_STRENGTH*TR456_WATER_FLOW_CAUSTICS*
    TR_TOGGLE_FLOW_CAUSTICS*submergedCausticMask>.001) {
   caust=caustic(causticUv,flowDetailTime)*.065*
     TR456_WATER_CAUSTICS_STRENGTH*TR456_WATER_FLOW_CAUSTICS*
     TR_TOGGLE_FLOW_CAUSTICS*submergedCausticMask;
 }
 tex+=vec3(.18,.38,.32)*caust;
 float bubbles=(lineMask(stableUv.x*9.0+noiseC*.34,14.0)*streakLane*
   smoothstep(.22,.78,stream+flowLane.z+ribbon.z*.32+crossWaveField.z*.26)+
   swirl.z*.58+eddyFoam*.72+edgeStreamFoam*.42+crossWaveField.w*.20)*
   TR456_WATER_FLOW_SWIRL*
   TR_TOGGLE_FLOW_FOAM*.25;
 float crestMask=smoothstep(.58,.96,wave+flowBreath.z*.24+flowLane.z*.18+
   ribbon.z*.24+crossWaveField.z*.22);
float flowSignal=sat(flowLane.z*.55+ribbon.z*.48+stream*.28+
  crossWaveField.z*.38+flowBreath.z*.50+detailTex.z*.46+strands*.22+
  anisoGlint.z*.30+edgeStreamFoam*.22+coreRush*.20);
 float shallowBreak=smoothstep(.12,.48,flowDepth)*(1.0-smoothstep(.68,1.0,flowDepth));
 float depthFoam=sat((shallowBreak*.56+bankContact*.34)*
   (flowSignal*.44+wave*.24+sheetBreak*.22+coreRush*.28+edgeHint*.12))*
   TR_TOGGLE_FLOW_FOAM;
 float aeration=sat((pow(stream,1.35)*.16+streakFoam*.72+bubbles*.34+
   shoreFoam*.72+bankFoamVolume*.44+eddyFoam*.58+
   edgeStreamFoam*.52+depthFoam*.55+crestMask*.24+
   flowBreath.z*.20+flowLane.z*.14+ribbon.z*.18+
   crossWaveField.z*.12+anisoGlint.z*.08)*
   TR456_WATER_FLOW_AERATION);
 float retroSheet=sat(stream*.34+flowSignal*.38+pow(wave,1.55)*.22+
   abs(current)*.16+ribbon.z*.12+edgeStreamFoam*.08+
   anisoGlint.z*.10+depthFoam*.06+sheetBreak*.10);
 float broadStreak=lineMask(stableUv.x*2.05+noiseA*.18-flowTravel*(.06+speed*.10),3.0)*
   smoothstep(.12,.82,stream+flowLane.z*.38);
 float thinStreak=lineMask(stableUv.x*7.6+noiseB*.34-flowTravel*(.18+speed*.18),11.0)*
   smoothstep(.30,.90,wave+strands*.36);
 float directionalFoamRibbon=sat((pow(sat(ribbon.z),1.45)*.72+
   thinStreak*.20+broadStreak*.12+anisoGlint.w*.18)*
   smoothstep(.18,.88,flowSignal+stream*.30+coreRush*.16)*
   TR456_WATER_FLOW_STREAK_FOAM*TR_TOGGLE_FLOW_FOAM)*(1.0-aeration*.32);
 float tensionPatch=sat((ribbon.w*.38+sheetBreak*.22+flowBreath.z*.16+
   detailTex.z*.12)*TR456_WATER_FLOW_SURFACE_TENSION*
   (1.0-directionalFoamRibbon*.45));
float bodyDepth=sat((flowDepth*.62+retroSheet*.24+(1.0-vFog)*.16+
  ribbon.w*.20+crossWaveField.w*.18+sheetBreak*.22+detailTex.z*.18+
  flowSignal*.18+depthFoam*.14)*
   TR456_WATER_FLOW_DEPTH_BODY);
 float bodyMask=sat((flowDepth*.52+retroSheet*.34+(1.0-vFog)*.18+
   stream*.10+flowSignal*.12+bodyDepth*.26)*TR456_WATER_FLOW_BODY);
float ridgeMask=sat((broadStreak*.72+thinStreak*.48+crestMask*.24+
  wave*.16+flowSignal*.12+ribbon.z*.22+crossWaveField.z*.20+
  facetEnergy*.34+detailTex.z*.42+anisoGlint.z*.28+depthFoam*.24)*
  TR456_WATER_FLOW_RIDGE);
 float edgeFoamMask=sat((shoreFoam*.90+bubbles*.20+streakFoam*.32+
   bankFoamVolume*.70+eddyFoam*.52+edgeStreamFoam*.68+
   bankContact*.35+depthFoam*.32)*
   TR456_WATER_FLOW_EDGE_FOAM)*
   TR_TOGGLE_FLOW_FOAM;
 float bankFoamBody=sat(bankFoamVolume*(.74+TR456_WATER_FLOW_EDGE_FOAM*.72)+
   shoreFoam*.34)*TR456_WATER_FLOW_EDGE_FOAM;
 float bankFoamTongue=sat(bankFoamBody*(.55+.45*stream)+edgeStreamFoam*.18);
 vec3 bodyTint=mix(vec3(.018,.030,.044),vec3(.040,.066,.092),sat(vFog*.45+retroSheet*.34));
  tex=mix(tex,vec3(.034,.046,.060),sat(retroSheet*.30+flowDepth*.12)*.08);
  tex=mix(tex,vec3(luma(tex))*vec3(.72,.84,.76),sat(retroSheet*.04));
  tex=mix(tex,bodyTint,sat(bodyMask*.18));
  tex=mix(tex,tex*vec3(.66,.74,.70)+bodyTint*.44,sat(bodyDepth*.20));
  tex=mix(tex,vec3(.105,.142,.145),bankFoamBody*.13);

 vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.85));
 vec3 waterTint=mix(vec3(.012,.030,.050),vec3(.034,.070,.108),sat(vFog*.52+stream*.12));
 float flowBumpAmount=clamp(TR456_WATER_BUMP_STRENGTH*
   TR456_WATER_FLOW_BUMP_STRENGTH,0.0,2.2);
 vec2 flowBumpSlope=limitVec2(
   surfaceRipple*.78+safeVolume.xy*5.0+flowBreath.xy*6.2+
   detailTex.xy*16.0+flowLane.xy*2.8+ribbon.xy*3.2+
   crossWaveField.xy*3.5+swirl.xy*2.2+wakeFlow*.34+
   facetLocal*.035*clamp(TR456_WATER_POLYGONAL_NORMAL,0.0,2.0),
   .56)*max(TR456_WATER_BUMP_SCALE,.10)*flowBumpAmount;
 n=applyFlowBump(n,flowBumpSlope,
   (.64+.28*coreMask)*TR456_WATER_SURFACE_RELIEF);
 float flowBumpEnergy=sat(length(flowBumpSlope)*2.70);
 float fres=sat((.045+.72*pow(1.0-sat(dot(n,viewVec)),4.0))*TR456_WATER_FRESNEL_STRENGTH)*
   TR456_WATER_REFLECT_STRENGTH;
 fres*=mix(1.08,.78,aeration);
   vec3 col=mix(tex,waterTint,.045+.050*fres+.016*retroSheet)*light*1.14;
   col=mix(col,vec3(luma(col))*vec3(.72,.84,.76),sat(retroSheet*.035+aeration*.045));
   col=mix(col,col*vec3(.66,.76,.70)+bodyTint*.54,bodyMask*.18);
   col=mix(col,col*vec3(.62,.70,.66)+bodyTint*.42,bodyDepth*.16);
   col=mix(col,col*vec3(.72,.82,.76)+bodyTint*.24,sheetBreak*.10);
   float stableFlowTint=sat(TR456_WATER_FLOW_OPACITY*
     (.055+.050*stream+.035*wave+.030*flowBumpEnergy+.025*bodyMask));
   col=mix(col,col*vec3(.82,.98,1.08)+waterTint*light*.28,stableFlowTint);
 float foam=(pow(stream,1.70)*.005+pow(wave,2.35)*.010+
  streakFoam*.004+bubbles*.004+wake.z*.022+contactHeight*.008+
  safeVolume.z*.004+flowBreath.z*.005+crestMask*.014+
  shoreFoam*.004+bankFoamVolume*.018+edgeFoamMask*.010+eddyFoam*.014+edgeStreamFoam*.010+
  depthFoam*.016+aeration*.004+ribbon.z*.0035+crossWaveField.z*.0042+
   directionalFoamRibbon*.010+tensionPatch*.002+
   facetEnergy*.005+anisoGlint.z*.0030)*
   TR456_WATER_FOAM_STRENGTH*TR456_WATER_FLOW_STRENGTH;
  float glint=(broadStreak*.015+thinStreak*.009+
    lineMask(noiseA+noiseB*.7+flowDetailTime*.075,13.0)*.0034+
    foam*.080+wave*.0050+contactHeight*.0025+safeVolume.z*.0025+
    flowBreath.z*.0045+detailTex.z*.012+streakFoam*.0006+shoreFoam*.0004+
    ribbon.z*.0085+crossWaveField.z*.0075+edgeStreamFoam*.0012+
    anisoGlint.z*.036+anisoGlint.w*.022+flowBumpEnergy*.010)*
    TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT*(1.0-aeration*.18);
  float specStreak=(pow(sat(ribbon.z),2.20)*.027+
    lineMask(stableUv.x*4.35+noiseA*.24-flowTravel*(.10+speed*.16),15.0)*
    smoothstep(.38,.92,ribbon.z+thinStreak*.32)*.010+
    anisoGlint.z*.066+anisoGlint.w*.038+flowBumpEnergy*.014)*
   clamp(TR456_WATER_FLOW_SPECULAR_STREAK,0.0,2.0)*
   TR456_WATER_GLINT_STRENGTH*TR456_WATER_FLOW_GLINT*
   (.35+.65*fres)*(1.0-aeration*.22);
 col+=vec3(.18,.25,.31)*glint;
 col+=vec3(.22,.30,.36)*specStreak;
 col+=vec3(.06,.13,.18)*anisoGlint.z*
   clamp(TR456_WATER_FLOW_SPECULAR_STREAK,0.0,2.0)*TR456_WATER_GLINT_STRENGTH;
 col+=vec3(.09,.15,.18)*edgeStreamFoam*.026*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.12,.18,.19)*depthFoam*.022*TR456_WATER_FOAM_STRENGTH;
 col-=vec3(.030,.034,.030)*ridgeMask*(.32+.38*bodyMask);
 col+=vec3(.12,.20,.24)*ridgeMask*(.026+.024*vFog);
 col+=vec3(.030,.034,.030)*(strands*.22+stream*.20+wave*.12+abs(current)*.14);
 col=mix(col,col*vec3(.72,.86,.98),flowSignal*.08*(.45+.55*vFog));
 col+=vec3(.024,.044,.062)*pow(flowSignal,1.35)*(.35+.65*vFog)*
   TR456_WATER_FLOW_STRENGTH;
col+=vec3(.045,.078,.095)*(broadStreak*.030+thinStreak*.018+
  ribbon.z*.034+crossWaveField.z*.030+flowBreath.z*.035+
  detailTex.z*.040);
 col=mix(col,col*vec3(.88,1.01,1.07)+waterTint*light*.18,
   tensionPatch*.10);
 col=mix(col,vec3(.66,.80,.82),directionalFoamRibbon*.10);
 col+=vec3(.12,.20,.22)*directionalFoamRibbon*.035+
   vec3(.026,.055,.062)*tensionPatch*(.16+.20*vFog);
col+=vec3(.10,.15,.16)*edgeFoamMask*.026*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.15,.22,.23)*bankFoamBody*(.044+.030*vFog);
 col=mix(col,vec3(.58,.72,.72),bankFoamTongue*.070);
 col+=vec3(.08,.13,.15)*eddyFoam*.022*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.035,.060,.070)*shoreFoam*.016*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.060,.095,.100)*depthFoam*.018*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.032,.052,.060)*bubbles*.022;
 col+=vec3(.09,.15,.18)*pow(sat(wave),2.20)*.016;
 col+=vec3(.030,.052,.066)*(stream*.20+wave*.12+strands*.16+abs(current)*.12+
    detailTex.z*.24)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);

 vec3 sceneRefl=waterTint;
 float reflMask=0.0;
 float flowReflectionActive=0.0;
 if(flowReflectionActive>.004) {
  vec2 reflectFlow=surfaceRipple*1.52+safeVolume.xy*.70+flowBreath.xy*1.28+
    detailTex.xy*1.10+
    flowLane.xy*.66+ribbon.xy*.80+crossWaveField.xy*.92+swirl.xy*.58+
    flowBumpSlope*.52+facetLocal*.95*clamp(TR456_WATER_POLYGONAL_NORMAL,0.0,2.0);
  reflectFlow=limitVec2(reflectFlow,.092);
  vec2 flowScreenOffset=limitVec2(flowScreenDir*reflectFlow.x+flowScreenSide*reflectFlow.y,.072);
  vec2 mirrorWarp=vec2(flowScreenOffset.x,-abs(flowScreenOffset.y))*
    (.58+.72*fres)*TR456_WATER_MIRROR_ROUGHNESS*mix(.84,1.0,seamGuard);
  vec3 reflectNormal=normalize(n+vec3(reflectFlow.x*1.8,0.0,
    reflectFlow.y*1.8));
  vec2 mirrorUv=preciseReflectionUv(screen,reflectNormal,viewVec,
    mirrorWarp,0.0,.44+.56*fres);
  vec2 mirrorUv2=preciseReflectionUv(screen,reflectNormal,viewVec,
    mirrorWarp+vec2(flowScreenSide.x,-abs(flowScreenSide.y))*
    (.005+.024*fres)*(noiseA-noiseB+flowLane.z*.35),
    .006+.016*fres,.34+.48*fres);
  sceneRefl=reflectionGrade(stableCaptureColor(mirrorUv,screen)*.72+
    stableCaptureColor(mirrorUv2,screen)*.28);
  sceneRefl=mix(sceneRefl,vec3(luma(sceneRefl))*vec3(.82,.88,.80),sat(flowSignal*.22+aeration*.18));
  sceneRefl=mix(sceneRefl,waterTint,aeration*.05);
  sceneRefl=reflectionAutoBalance(sceneRefl,waterTint,
    sat(flowSignal*.22+aeration*.18+edgeCalm*.16+coreRush*.18));
  float reflEdge=mix(.55,1.0,min(reflectionEdgeFade(mirrorUv),
    reflectionEdgeFade(mirrorUv2)));
  float reflOk=mix(.60,1.0,smoothstep(.004,.045,luma(sceneRefl)))*
    reflEdge;
  reflMask=clamp((.120+fres*.72+stream*.105+wave*.095+retroSheet*.060)*
    flowReflectionActive,0.0,.78)*reflOk*
    (1.0-aeration*.20)*(1.0-edgeCalm*.18);
  col=mix(col,sceneRefl,reflMask*(.42+.28*vFog)*TR456_WATER_SCENE_REFLECTION);
 }

 vec3 flowGrade=max((col-vec3(.040))*1.16+vec3(.004,.016,.038),vec3(0.0));
 flowGrade=flowGrade*vec3(.82,.96,1.20);
 col=mix(col,flowGrade,.42);
 col=mix(vec3(luma(col)),col,TR456_WATER_COLOR_SATURATION);
 col*=TR456_WATER_BRIGHTNESS;
 col=mix(uFogColor.rgb*waterCoverage,col,vFog);
  float alpha=clamp((waterCoverage*(.64+.12*fres)+bodyMask*.045+ridgeMask*.018+
    edgeFoamMask*.038+retroSheet*.035+aeration*.030+foam*.025+streakFoam*.001+
    bankFoamBody*.026+shoreFoam*.003*TR456_WATER_FOAM_STRENGTH+eddyFoam*.008+
    edgeStreamFoam*.006+depthFoam*.010+anisoGlint.z*.002+
    directionalFoamRibbon*.018+tensionPatch*.006+
    reflMask*.065+flowDepth*.024+bodyDepth*.014-
     sheetBreak*.018)*
    TR456_WATER_OPACITY*TR456_WATER_FLOW_OPACITY,.135,.62);
 alpha*=passOpacity;

#if TR456_WATER_DEBUG_MODE == 1
 fragColor=vec4(vec3(wave),1.0);
#elif TR456_WATER_DEBUG_MODE == 5
 fragColor=vec4(sceneRefl,1.0);
#elif TR456_WATER_DEBUG_MODE == 6
 fragColor=vec4(reflectionGrade(captureColor(screen)),1.0);
#elif TR456_WATER_DEBUG_MODE == 7
 fragColor=vec4(vec3(reflMask),1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(.12,1.0,.22,1.0);
#elif TR456_WATER_DEBUG_MODE == 9
 fragColor=vec4(streakFoam+edgeStreamFoam,anisoGlint.z,wave,1.0);
#elif TR456_WATER_DEBUG_MODE == 10
 fragColor=vec4(abs(contactWave.z)*2.0,abs(contactWave.x)*12.0,abs(contactWave.y)*12.0,1.0);
#elif TR456_WATER_DEBUG_MODE == 11
 fragColor=vec4(vec3(alpha),1.0);
#elif TR456_WATER_DEBUG_MODE == 12
 fragColor=vec4(base.a,flowDepth,wave,1.0);
#elif TR456_WATER_DEBUG_MODE == 13
 fragColor=vec4(abs(meshBase.a-worldBase.a)*4.0,
   length(meshBase.rgb-worldBase.rgb)*2.0,streakFoam,1.0);
#elif TR456_WATER_DEBUG_MODE == 14
 fragColor=vec4(debugIdColor(uTrWaterDrawInfo.y),1.0);
#elif TR456_WATER_DEBUG_MODE == 15
 fragColor=vec4(debugTileColor(uv,vLayer),1.0);
#elif TR456_WATER_DEBUG_MODE == 16
 fragColor=vec4(clamp(vLight+vColor,vec3(0.0),vec3(1.0)),1.0);
#elif TR456_WATER_DEBUG_MODE == 17
 fragColor=vec4(meshBase.rgb,1.0);
#elif TR456_WATER_DEBUG_MODE == 18
 fragColor=vec4(worldBase.rgb,1.0);
#elif TR456_WATER_DEBUG_MODE == 19
 fragColor=vec4(meshBase.a,worldBase.a,base.a,1.0);
#elif TR456_WATER_DEBUG_MODE == 20
 fragColor=vec4(col,1.0);
#elif TR456_WATER_DEBUG_MODE == 21
 fragColor=vec4(vec3(seamGuard),1.0);
#elif TR456_WATER_DEBUG_MODE == 22
 fragColor=vec4(authoredFlowPresent,smoothstep(0.0,.010,flowLen),
   fract(abs(flowTravel)*.05),1.0);
#elif TR456_WATER_DEBUG_MODE == 23
 fragColor=vec4(detailTex.z,flowSignal,wave,1.0);
#else
 if(duplicatePass>.5 && passOpacity<=.001)
   discard;
 fragColor=vec4(col,alpha);
#endif
}
