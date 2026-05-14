#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.36
#define TR456_WATER_REFRACT_STRENGTH 1.62
#define TR456_WATER_REFLECT_STRENGTH 1.78
#define TR456_WATER_SSR_STRENGTH 1.08
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_TINT_STRENGTH 1.0
#define TR456_WATER_VOLUME_STRENGTH 1.0
#define TR456_WATER_OPACITY 0.62
#define TR456_WATER_FORCE_REFLECTION 1.0
#define TR456_WATER_SCENE_REFLECTION 1.12
#define TR456_WATER_CAUSTICS_STRENGTH 1.10
#define TR456_WATER_DEPTH_STRENGTH 1.0
#define TR456_WATER_RIPPLE_STRENGTH 0.65
#define TR456_WATER_RIPPLE_CENTER_X 0.50
#define TR456_WATER_RIPPLE_CENTER_Y 0.62
#define TR456_WATER_SURFACE_RELIEF 1.82
#define TR456_WATER_WAKE_STRENGTH 1.0
#define TR456_WATER_WAKE_WIDTH 0.42
#define TR456_WATER_WAKE_LENGTH 0.58
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.95
#define TR456_WATER_MICRO_RIPPLE 0.40
#define TR456_WATER_MICRO_SCALE 0.82
#define TR456_WATER_MIRROR_ROUGHNESS 1.16
#define TR456_WATER_SWELL_STRENGTH 1.05
#define TR456_WATER_SWELL_SCALE 0.80
#define TR456_WATER_WAKE_WAVE 1.0
#define TR456_WATER_EDGE_WAVE 0.75
#define TR456_WATER_EDGE_WIDTH 0.09
#define TR456_WATER_REFLECTION_CONTRAST 1.48
#define TR456_WATER_ROUGH_REFLECTION 1.03
#define TR456_WATER_FRESNEL_STRENGTH 1.24
#define TR456_WATER_BOTTOM_CAUSTICS 0.85
#define TR456_WATER_CONTACT_EDGE 0.72
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_WALL_STRETCH 0.84
#define TR456_WATER_COLOR_SATURATION 1.18
#define TR456_WATER_BRIGHTNESS 0.72
#define TR456_WATER_TEXTURE_STRENGTH 1.38
#define TR456_WATER_FBO_REFLECTION 0
#define TR456_WATER_SAFE_VOLUME 1.05
#endif
#ifndef TR456_WATER_BUMP_STRENGTH
#define TR456_WATER_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_BUMP_SCALE
#define TR456_WATER_BUMP_SCALE 1.0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform vec4 uModelMatrix[4];
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
in vec3 vPos;
in vec2 vTexCoord;
in vec3 vContactWave;
in vec3 vrgb0;
in vec3 vrgb1;
in vec3 vrgb2;
in vec3 vrgb3;
in vec3 vrgb4;
in vec3 vrgb5;
out vec4 fragColor;

#define TR_TOGGLE_SURFACE_CAUSTICS uTrWaterToggle1.w
#define TR_TOGGLE_SURFACE_FOAM uTrWaterToggle2.x
#define TR_TOGGLE_SURFACE_REFLECTION uTrWaterToggle2.y
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(.3333)); }

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec3 captureColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

float reflectionUvFade(vec2 uv){
 vec2 a=smoothstep(vec2(-.060),vec2(.120),uv);
 vec2 b=smoothstep(vec2(-.060),vec2(.120),1.0-uv);
 return a.x*a.y*b.x*b.y;
}

vec2 limitReflectionVec(vec2 v, float limit){
 float m=length(v);
 float safeLimit=max(limit,.0001);
 float scale=(safeLimit*(1.0-exp(-m/safeLimit)))/max(m,.0001);
 return v*scale;
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

vec3 stableCaptureColor(vec2 uv, vec2 fallback){
 return mix(captureColor(fallback),captureColor(uv),reflectionUvFade(uv));
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.84,.95,1.04),.16);
 return clamp(c,vec3(0.0),vec3(3.0));
}

vec3 colorGrade(vec3 c){
 float y=luma(c);
 c=mix(vec3(y),c,TR456_WATER_COLOR_SATURATION);
 return clamp(c*TR456_WATER_BRIGHTNESS,vec3(0.0),vec3(3.0));
}

vec2 waterBumpLimit(vec2 v, float limit){
 float m=length(v);
 float safeLimit=max(limit,.00001);
 float scale=(safeLimit*(1.0-exp(-m/safeLimit)))/max(m,.00001);
 return v*scale;
}

vec3 applyWaterBump(vec3 baseNormal, vec2 slope, float strength){
 float amount=clamp(strength,0.0,2.0);
 vec2 s=waterBumpLimit(slope*amount,.58);
 return normalize(baseNormal+vec3(-s.x,0.0,-s.y));
}

float causticLine(vec2 p, float time){
 float a=1.0-abs(fract(p.x*5.7+p.y*1.9+time*.055)-.5)*2.0;
 float b=1.0-abs(fract(p.x*-2.4+p.y*6.4-time*.047)-.5)*2.0;
 float c=1.0-abs(fract((p.x+p.y)*4.1+time*.034)-.5)*2.0;
 float d=1.0-abs(fract((p.x-p.y)*3.2-time*.026)-.5)*2.0;
 float line=pow(sat(max(max(a,b),max(c*.72,d*.58))),8.8);
 float broken=smoothstep(.42,.86,texture(sNoise,vec3(p*.70+vec2(.31,.17),time*.021)).x);
 float broad=smoothstep(.04,.50,line)*line;
 return broad*broken;
}

vec2 microRipple(vec2 p, float time){
 p*=max(TR456_WATER_MICRO_SCALE,.10);
 vec2 a=vec2(sin(p.x*37.0+p.y*16.0+time*1.25),
             cos(p.x*19.0-p.y*31.0-time*1.05));
 vec2 b=vec2(sin((p.x+p.y)*58.0+time*1.70),
             cos((p.x-p.y)*49.0-time*1.30));
 vec2 c=vec2(sin(p.x*91.0+p.y*13.0-time*.85),
             sin(p.y*83.0-p.x*21.0+time*1.15));
 return (a*.008+b*.0045+c*.0025)*TR456_WATER_MICRO_RIPPLE;
}

vec3 surfaceSwell(vec2 p, float time){
 p*=max(TR456_WATER_SWELL_SCALE,.10);
 float phaseA=dot(p,vec2(1.30,.42))*6.28318+time*.32;
 float phaseB=dot(p,vec2(-.55,1.12))*6.28318-time*.24;
 vec2 center=p-vec2(.35,-.25);
 float phaseC=length(center)*7.4-time*.38;
 float h=sin(phaseA)*.46+sin(phaseB)*.34+sin(phaseC)*.20;
 vec2 radial=normalize(center+vec2(.0001));
 vec2 grad=vec2(cos(phaseA)*1.30+cos(phaseB)*(-.55*.72),
                cos(phaseA)*.42+cos(phaseB)*(1.12*.72));
 grad+=radial*cos(phaseC)*.58;
 float crest=sat(abs(h)*.74+.10);
 return vec3(grad*.0065*TR456_WATER_SWELL_STRENGTH,crest*TR456_WATER_SWELL_STRENGTH);
}

vec3 edgeRippleLayer(vec2 p, vec2 screen, float time){
 float scale=mix(.70,1.35,sat(TR456_WATER_EDGE_WIDTH*9.0));
 p*=scale;
 float a=sin(dot(p,vec2(1.55,.38))*18.0-time*2.4);
 float b=sin(dot(p,vec2(-.42,1.18))*24.0+time*1.9);
 float c=sin((p.x+p.y)*37.0-time*3.1);
 vec2 grad=vec2(cos(dot(p,vec2(1.55,.38))*18.0-time*2.4)*1.55+
                cos((p.x+p.y)*37.0-time*3.1)*.45,
                cos(dot(p,vec2(-.42,1.18))*24.0+time*1.9)*1.18+
                cos((p.x+p.y)*37.0-time*3.1)*.45);
 float viewFade=smoothstep(.015,.070,screen.x)*smoothstep(.015,.070,screen.y)*
   smoothstep(.015,.070,1.0-screen.x)*smoothstep(.015,.070,1.0-screen.y);
 float crest=sat(abs(a)*.30+abs(b)*.23+abs(c)*.14-.18);
 return vec3(grad*.0020*TR456_WATER_EDGE_WAVE*viewFade,
   crest*.18*TR456_WATER_EDGE_WAVE*viewFade);
}

vec3 playerWake(vec2 screen, float time){
 vec2 p=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.35,1.0);
 float d=length(p)+.0001;
 float width=max(TR456_WATER_WAKE_WIDTH,.05);
 float wakeLen=max(TR456_WATER_WAKE_LENGTH,.08);
 float ringA=sin(d*86.0-time*7.2)*exp(-d*4.6)*smoothstep(.020,.20,d);
 float ringB=sin(d*46.0-time*4.1+p.x*3.0)*exp(-d*2.5)*smoothstep(.070,.58,d);
 float trail=(1.0-smoothstep(width*.14,width,abs(p.x)))*
   smoothstep(-.34,.02,p.y)*(1.0-smoothstep(.08,wakeLen,p.y));
 float wake=sin((p.y-time*.30)*74.0+sin(p.x*18.0))*trail*exp(-abs(p.x)*5.4);
 float stepPhase=sin(time*6.3);
 vec2 footL=p-vec2(-.085,.018+stepPhase*.018);
 vec2 footR=p-vec2(.085,.018-stepPhase*.018);
  float foot=exp(-dot(footL,footL)*115.0)*(stepPhase*.5+.5)+
    exp(-dot(footR,footR)*115.0)*(.5-stepPhase*.5);
  float footWave=(sin(d*128.0-time*10.5)*exp(-d*7.2)*foot+
    sin((d-time*.42)*72.0)*trail*.56)*TR456_WATER_WAKE_WAVE;
  float rear=smoothstep(-.050,.085,p.y)*(1.0-smoothstep(wakeLen*.58,wakeLen*1.28,p.y));
  float armX=p.y*(.34+.22*smoothstep(.05,wakeLen,p.y));
  float armWidth=max(width*.070,.018)+p.y*.030;
  float left=exp(-pow((p.x+armX)/armWidth,2.0))*rear;
  float right=exp(-pow((p.x-armX)/armWidth,2.0))*rear;
  float stem=exp(-pow(p.x/max(width*.18,.040),2.0))*rear*
    (1.0-smoothstep(wakeLen*.22,wakeLen*.82,p.y));
  float yPhase=p.y*78.0+abs(p.x)*26.0-time*8.9;
  float yWave=sin(yPhase+sin(p.x*12.0)*.30)*(left+right)*TR456_WATER_WAKE_WAVE;
  float stemWave=sin(p.y*92.0-time*9.8)*stem*TR456_WATER_WAKE_WAVE;
  vec2 yFlow=vec2((right-left)*(.026+.018*abs(sin(yPhase))),
    -(left+right)*(.018+.014*abs(cos(yPhase)))-stem*.018)*
    (yWave*.72+stemWave*.45);
  vec2 dir=normalize(p);
  vec2 flow=dir*(ringA*.036+ringB*.018+foot*.026+footWave*.034)+
    vec2(p.x,-p.y)*(wake*.016)+yFlow;
  float crest=sat(abs(ringA)*.80+abs(ringB)*.42+abs(wake)*.55+
    foot*.95+abs(footWave)*.85+abs(yWave)*.72+abs(stemWave)*.45+
    (left+right+stem)*.22);
  return vec3(flow*TR456_WATER_RIPPLE_STRENGTH*TR456_WATER_WAKE_STRENGTH,crest*TR456_WATER_WAKE_STRENGTH);
}

vec3 safeVolumeLayer(vec2 p, float time){
 p*=.36;
 vec2 a=normalize(vec2(.88,.48));
 vec2 b=normalize(vec2(-.36,.93));
 vec2 c=normalize(vec2(.58,-.81));
 float pa=dot(p,a)*6.28318+sin(dot(p,b)*3.2+time*.10)*.18+time*.24;
 float pb=dot(p,b)*5.10+sin(dot(p,a)*2.7-time*.08)*.15-time*.18;
 float pc=dot(p,c)*7.20+sin(dot(p,vec2(.70,-.30))*2.2+time*.06)*.12+time*.31;
 float h=sin(pa)*.46+sin(pb)*.34+sin(pc)*.20+
   sin((pa+pb)*.50+time*.11)*.12;
 vec2 slope=a*cos(pa)*.42+b*cos(pb)*.30+c*cos(pc)*.18+
   normalize(a+b+vec2(.001))*cos((pa+pb)*.50+time*.11)*.08;
 float crest=sat(abs(h)*.58+pow(sat(abs(h)),3.0)*.30);
 return vec3(slope*.010,crest);
}

vec3 cubeApprox(vec3 r){
 vec3 q=r*r;
 vec3 p=step(vec3(0.0),r);
 return q.x*mix(vrgb1,vrgb0,p.x)+q.y*mix(vrgb3,vrgb2,p.y)+q.z*mix(vrgb5,vrgb4,p.z);
}

void main(){
 vec2 uv=vTexCoord.xy;
 vec2 d=vec2(1.0/192.0,0.0);
 float t=uModelMatrix[3].x*.082;
 float h0=texture(sNoise,vec3(uv*.50,t)).x;
 float h1=texture(sNoise,vec3(uv*1.13+vec2(.09,.21),t*.63)).x;
 float h2=texture(sNoise,vec3(uv*2.05+vec2(.31,.04),t*.38)).x;
 float hx=texture(sNoise,vec3(uv*.50+d.xy,t)).x-h0;
 float hy=texture(sNoise,vec3(uv*.50+d.yx,t)).x-h0;
 vec2 grad=vec2(hx,hy)+vec2(h1-h0,h2-h1)*.18;
 vec2 screen=gl_FragCoord.xy*captureInvViewport();
 vec3 wake=vec3(0.0);
 vec3 contactWave=vContactWave*TR_TOGGLE_CONTACT_RIPPLES;
 float contactHeight=abs(contactWave.z);
 float contactCrest=smoothstep(.010,.105,contactHeight)*TR_TOGGLE_CONTACT_RIPPLES;
 float contactTension=smoothstep(.018,.155,contactHeight)*
   (1.0-smoothstep(.20,.42,contactHeight))*TR_TOGGLE_CONTACT_RIPPLES;
  vec2 micro=microRipple(uv*2.25+vec2(t*.012,-t*.008),t);
 vec3 swell=surfaceSwell(uv*1.10,t);
 vec3 safeVolume=safeVolumeLayer(uv,t)*TR456_WATER_SAFE_VOLUME;
 vec3 edgeRip=edgeRippleLayer(uv*1.45,screen,t);
 wake.xy+=contactWave.xy*1.24;
 wake.z+=contactHeight*1.38+contactTension*.20;
 grad+=micro*(.40*TR456_WATER_SURFACE_RELIEF);
 grad+=swell.xy*(.95*TR456_WATER_SURFACE_RELIEF);
 grad+=safeVolume.xy*(1.20*TR456_WATER_SURFACE_RELIEF);
 grad+=edgeRip.xy*(.78*TR456_WATER_SURFACE_RELIEF);
 grad+=contactWave.xy*(3.35*TR456_WATER_CONTACT_NORMAL_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=normalize(contactWave.xy+vec2(.0001))*
   (contactTension*.010*TR456_WATER_CONTACT_NORMAL_STRENGTH);
 grad+=wake.xy*(1.86*TR456_WATER_SURFACE_RELIEF);
 vec3 n=normalize(vec3(-grad.x*7.6*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF,1.0,-grad.y*7.6*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF));
 vec2 bumpSlope=(grad*1.38+micro*7.0+swell.xy*3.0+
   safeVolume.xy*3.2+edgeRip.xy*2.6+contactWave.xy*2.0)*
   max(TR456_WATER_BUMP_SCALE,.10);
 n=applyWaterBump(n,bumpSlope,
   TR456_WATER_BUMP_STRENGTH*.56*TR456_WATER_SURFACE_RELIEF);
 float bumpEnergy=sat(length(bumpSlope)*2.55*TR456_WATER_BUMP_STRENGTH);
 if(!gl_FrontFacing)n=-n;

 vec3 vv=normalize(-vPos);
 float ndv=sat(dot(n,vv));
 vec3 rr=normalize(reflect(-vv,n));
 vec3 refSharp=cubeApprox(rr);
 vec3 refWide=cubeApprox(normalize(rr+vec3((grad.x+micro.x*.32+swell.x*.98+edgeRip.x*.78)*.58,.08,(grad.y+micro.y*.32+swell.y*.98+edgeRip.y*.78)*.58)));
 vec3 ref=mix(refSharp,refWide,sat(length(grad)*7.4+length(micro)*8.0+swell.z*.18+edgeRip.z*.22));

 float fresBase=.028+.88*pow(1.0-ndv,4.2);
 float F=sat(mix(fresBase,sqrt(fresBase),.16)*TR456_WATER_FRESNEL_STRENGTH);
 float edge=smoothstep(.10,.64,F);
 float topView=1.0-edge;
 float microEnergy=sat(length(micro)*32.0+bumpEnergy*.36);
 float energy=abs(grad.x)+abs(grad.y)+wake.z*.083+contactHeight*.052+
   contactCrest*.022+swell.z*.026+
   safeVolume.z*.030+edgeRip.z*.030+microEnergy*.018;
 float ridge=smoothstep(.026,.090,energy)*(1.0-smoothstep(.092,.20,energy));
 ridge=max(ridge,max(max(max(wake.z*.40,contactHeight*.30+contactCrest*.08),swell.z*.10),edgeRip.z*.14));
 float streak=pow(1.0-abs(fract((screen.x+h0*.18+h1*.10+t*.055)*4.0)-.5)*2.0,9.0);
 streak*=smoothstep(.38,.90,h1)*(.08+.68*edge)*clamp(TR456_WATER_TEXTURE_STRENGTH,.75,1.65);
 float glint=pow(max(h1*.70+h2*.25-.60,0.0),7.0)*.052+wake.z*.044+
   contactHeight*.040+contactCrest*.022+
   swell.z*.008+safeVolume.z*.009+edgeRip.z*.008+microEnergy*.004;

 vec3 horizon=vec3(.012,.046,.058);
 vec3 cold=vec3(.22,.34,.38);
 vec3 col=mix(horizon,ref*(2.05+edge*.82)+cold*streak*.38,.68)*F*TR456_WATER_REFLECT_STRENGTH;
 col+=cold*(ridge*.030+glint)*TR456_WATER_GLINT_STRENGTH;
 col+=vec3(.16,.30,.34)*(contactCrest*.022+contactTension*.026)*
   TR456_WATER_GLINT_STRENGTH;
 col+=cold*(streak*.030+ridge*.028)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 float alpha=(.060+.160*edge+.012*ridge+contactHeight*.036+
   contactCrest*.026+contactTension*.018)*TR456_WATER_REFLECT_STRENGTH*
   TR456_WATER_OPACITY;

 float waveEnergy=sat(length(grad)*7.0+wake.z*.74+contactHeight*.66+
   contactCrest*.30+swell.z*.20+edgeRip.z*.24+microEnergy*.10);
 float rough=clamp((.32+waveEnergy*.68+topView*.20-edge*.07)*TR456_WATER_ROUGH_REFLECTION*TR456_WATER_MIRROR_ROUGHNESS,.12,.98);
 vec2 rippleOffset=vec2(grad.x,-grad.y)*(.032+.080*rough+.052*edge)+
   vec2(wake.x,-wake.y)*(.34+.44*edge)+vec2(swell.x,-swell.y)*(.70+.42*rough)+
   vec2(safeVolume.x,-safeVolume.y)*(.72+.40*rough)+
   vec2(edgeRip.x,-edgeRip.y)*(.66+.42*rough)+vec2(micro.x,-micro.y)*(.24+.24*rough);
vec2 mirrorWarp=vec2(grad.x+micro.x*.38+swell.x*.94+edgeRip.x*.70,
   -abs(grad.y+micro.y*.28+swell.y*.94+edgeRip.y*.70))*
   (.058+.108*edge+.064*rough)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 bumpMirror=waterBumpLimit(bumpSlope*(.006+.012*edge)*
   TR456_WATER_BUMP_STRENGTH,.032);
 mirrorWarp+=vec2(bumpMirror.x,-abs(bumpMirror.y));
 vec3 reflectNormal=normalize(n+vec3(mirrorWarp.x*4.5,0.0,
   -mirrorWarp.y*4.5));
 vec2 mirrorUv=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorWarp,0.0,rough);
 vec2 mirrorUv2=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorWarp+rippleOffset*(.80+rough*.96)+vec2(rr.x,-rr.z)*(.018+.044*edge),
   .012+.018*edge,rough*.92);
 vec2 mirrorUv3=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorWarp-rippleOffset*(.64+rough*.66)+vec2(-rr.z,-rr.x)*(.012+.030*rough),
   0.0,rough*.76);
 vec2 mirrorUv4=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorWarp+vec2(grad.y+micro.y*.32+swell.y*.94+edgeRip.y*.70,
   grad.x+micro.x*.32+swell.x*.94+edgeRip.x*.70)*(.024+.050*rough),
   .008,rough*.68);
 vec2 wallStretch=vec2(0.0,(-.045-.090*edge)*TR456_WATER_WALL_STRETCH);
 float reflectValid=mix(.58,1.0,min(reflectionUvFade(mirrorUv),
   reflectionUvFade(mirrorUv2)));
 vec3 refrScene=reflectionGrade(stableCaptureColor(screen+rippleOffset*.32,screen));
 float causticA=causticLine(screen*vec2(1.15,.82)+grad*.16,t);
 float causticB=causticLine(screen.yx*vec2(.74,1.36)-grad*.10+vec2(.17,.09),t*.83);
 float bottomMask=smoothstep(.12,.76,topView)*(1.0-smoothstep(.84,1.0,topView)*.25);
 float bottomLight=(causticA*.65+causticB*.35)*TR456_WATER_BOTTOM_CAUSTICS*
   TR456_WATER_CAUSTICS_STRENGTH*TR_TOGGLE_SURFACE_CAUSTICS*bottomMask;
 refrScene+=vec3(.28,.36,.32)*bottomLight*.10;
#if TR456_WATER_REFLECTION_QUALITY <= 0
 vec3 mirrorSharp=reflectionGrade(stableCaptureColor(mirrorUv,screen)*.70+
   stableCaptureColor(mirrorUv2,screen)*.30);
 vec3 mirrorSoft=mirrorSharp;
#elif TR456_WATER_REFLECTION_QUALITY == 1
 vec3 mirrorTall=reflectionGrade(stableCaptureColor(mirrorUv+wallStretch,screen)*.62+
   stableCaptureColor(mirrorUv+wallStretch*1.55,screen)*.38);
 vec3 mirrorSharp=reflectionGrade(stableCaptureColor(mirrorUv,screen)*.58+
   stableCaptureColor(mirrorUv2,screen)*.30+
   stableCaptureColor(mirrorUv3,screen)*.12)*.88+mirrorTall*.12;
 vec3 mirrorSoft=reflectionGrade(stableCaptureColor(mirrorUv2,screen)*.46+
   stableCaptureColor(mirrorUv3,screen)*.34+
   stableCaptureColor(mirrorUv4,screen)*.20);
#else
 vec3 mirrorTall=reflectionGrade(stableCaptureColor(mirrorUv+wallStretch,screen)*.50+
   stableCaptureColor(mirrorUv+wallStretch*1.85,screen)*.30+
   stableCaptureColor(mirrorUv+wallStretch*.35,screen)*.20);
 vec3 mirrorSharp=reflectionGrade(stableCaptureColor(mirrorUv,screen)*.47+
   stableCaptureColor(mirrorUv2,screen)*.26+
   stableCaptureColor(mirrorUv3,screen)*.12)*.85+mirrorTall*.15;
 vec3 mirrorSoft=reflectionGrade(stableCaptureColor(mirrorUv2,screen)*.34+
   stableCaptureColor(mirrorUv3,screen)*.28+
   stableCaptureColor(mirrorUv4,screen)*.20+
   stableCaptureColor(mirrorUv-rippleOffset*.95,screen)*.12+
   stableCaptureColor(mirrorUv+wallStretch*1.25,screen)*.06);
#endif
 vec3 mirrorScene=mix(mirrorSharp,mirrorSoft,rough*.62);
 vec3 sceneRefl=mix(refrScene,mirrorScene,clamp(.40+edge*.46+F*.36,0.0,.98));
 float reflOk=mix(.72,1.0,smoothstep(.002,.030,luma(sceneRefl)))*
   reflectValid;
 float sceneMask=0.0;
 vec3 waterBody=vec3(.012,.060,.076)*TR456_WATER_TINT_STRENGTH;
 vec3 waterDeep=vec3(.006,.032,.046)*TR456_WATER_TINT_STRENGTH;
 vec3 waterTint=vec3(.82,.91,.93);
 float floorLum=luma(refrScene);
 float depthCue=clamp((1.0-floorLum)*.70+topView*.20+waveEnergy*.12,0.0,1.0);
 depthCue*=TR456_WATER_VOLUME_STRENGTH*TR456_WATER_DEPTH_ABSORPTION*TR456_WATER_DEPTH_STRENGTH;
 float bodyMask=clamp(.10+.09*TR456_WATER_OPACITY+.09*(1.0-ndv)+depthCue*.16+
   max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0)*.045,0.0,.36);
 vec3 sceneWater=mix(refrScene*waterTint,sceneRefl,clamp(.48+edge*.42,0.0,.94));
 vec3 absorbedBody=mix(waterBody,waterDeep,clamp(depthCue*.85,0.0,1.0));
 sceneWater*=exp(-vec3(.54,.24,.12)*depthCue);
 sceneWater=mix(sceneWater,absorbedBody,bodyMask*.72);
 col=mix(col*.84+absorbedBody*.16,sceneWater,sceneMask*.84);
 vec2 texEdge=min(uv,1.0-uv);
 float uvInside=step(0.0,uv.x)*step(0.0,uv.y)*step(uv.x,1.0)*step(uv.y,1.0);
 float uvEdge=(1.0-smoothstep(.018,.095,min(texEdge.x,texEdge.y)))*uvInside;
#if TR456_WATER_REFLECTION_QUALITY <= 0
 float sceneEdge=0.0;
#else
 vec2 px=captureInvViewport();
 float lumC=luma(captureColor(screen));
 float lumDx=abs(luma(captureColor(screen+vec2(px.x*2.0,0.0)))-lumC);
 float lumDy=abs(luma(captureColor(screen+vec2(0.0,px.y*2.0)))-lumC);
 float sceneEdge=smoothstep(.045,.18,lumDx+lumDy);
#endif
 float contact=clamp(max(max(uvEdge*.20,sceneEdge*.55),edgeRip.z*.42)*(1.0-edge*.32)*TR456_WATER_CONTACT_EDGE,0.0,1.0);
 col=mix(col,absorbedBody*.76,contact*.28);
 col+=vec3(.07,.12,.11)*contact*.035;
 col+=cold*(ridge*.030+streak*.018+glint*.46+edgeRip.z*.006+
   contactCrest*.018)*
    TR456_WATER_GLINT_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 alpha=max(alpha,.14+sceneMask*.18+edge*.055+bodyMask*.07+contact*.055);
 col=colorGrade(col);

#if TR456_WATER_DEBUG_MODE == 1
 fragColor=vec4(abs(n),1.0);
#elif TR456_WATER_DEBUG_MODE == 2
 fragColor=vec4(vec3(F),1.0);
#elif TR456_WATER_DEBUG_MODE == 5
 fragColor=vec4(sceneRefl,1.0);
#elif TR456_WATER_DEBUG_MODE == 6
 fragColor=vec4(reflectionGrade(captureColor(screen)),1.0);
#elif TR456_WATER_DEBUG_MODE == 7
 fragColor=vec4(vec3(sceneMask),1.0);
#elif TR456_WATER_DEBUG_MODE == 9
 fragColor=vec4(wake.z,swell.z,edgeRip.z,1.0);
#elif TR456_WATER_DEBUG_MODE == 10
 fragColor=vec4(abs(contactWave.z)*2.0,abs(contactWave.x)*12.0,abs(contactWave.y)*12.0,1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(1.0,.82,.05,1.0);
#else
 float surfaceFxVisible=max(TR_TOGGLE_SURFACE_REFLECTION,
   max(TR_TOGGLE_SURFACE_CAUSTICS,TR_TOGGLE_SURFACE_FOAM)*.38);
 fragColor=vec4(col,clamp(alpha,.16,.54)*surfaceFxVisible);
#endif
}
