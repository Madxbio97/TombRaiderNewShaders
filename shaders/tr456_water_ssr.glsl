#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.36
#define TR456_WATER_PIXEL_WAVE_STRENGTH 2.18
#define TR456_WATER_REFRACTION_WAVE_STRENGTH 2.45
#define TR456_WATER_DEEP_CAUSTICS_STRENGTH 1.0
#define TR456_WATER_VOLUME_STRENGTH 1.0
#define TR456_WATER_REFRACT_STRENGTH 1.62
#define TR456_WATER_REFLECT_STRENGTH 1.78
#define TR456_WATER_SSR_STRENGTH 1.08
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_TINT_STRENGTH 1.0
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
#define TR456_WATER_MICRO_RIPPLE 0.40
#define TR456_WATER_MICRO_SCALE 0.82
#define TR456_WATER_MIRROR_ROUGHNESS 1.16
#define TR456_WATER_SWELL_STRENGTH 1.05
#define TR456_WATER_SWELL_SCALE 0.80
#define TR456_WATER_WAKE_WAVE 1.0
#define TR456_WATER_EDGE_WAVE 0.75
#define TR456_WATER_EDGE_WIDTH 0.09
#define TR456_WATER_REFLECTION_CONTRAST 1.48
#define TR456_WATER_FRESNEL_STRENGTH 1.24
#define TR456_WATER_BOTTOM_CAUSTICS 0.85
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#define TR456_WATER_FBO_REFLECTION 0
#define TR456_WATER_SAFE_VOLUME 1.05
#endif
#ifndef TR456_WATER_BUMP_STRENGTH
#define TR456_WATER_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_BUMP_SCALE
#define TR456_WATER_BUMP_SCALE 1.0
#endif
#ifndef TR456_WATER_BUMP_ENABLED
#define TR456_WATER_BUMP_ENABLED 0
#endif
#ifndef TR456_WATER_SURFACE_CAUSTICS_ENABLED
#define TR456_WATER_SURFACE_CAUSTICS_ENABLED 1
#endif

uniform sampler2D uTrWaterScene;
uniform sampler2DArray sTex0;
uniform sampler2DArray sTex1;
uniform sampler2DArray sEnvMap;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uAmbient[6];
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vCamera;
out vec4 fragColor;

#define TR_TOGGLE_SURFACE_WARP uTrWaterToggle1.z
#define TR_TOGGLE_SURFACE_CAUSTICS uTrWaterToggle1.w
#define TR_TOGGLE_SURFACE_FOAM uTrWaterToggle2.x
#define TR_TOGGLE_SURFACE_REFLECTION uTrWaterToggle2.y
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }
float fastPow2(float x){ return x*x; }
float fastPow3(float x){ return x*x*x; }
float fastPow6(float x){ float x2=x*x; return x2*x2*x2; }
float fastPow78(float x){
 float x2=x*x;
 float x4=x2*x2;
 float x8=x4*x4;
 float x16=x8*x8;
 float x32=x16*x16;
 float x64=x32*x32;
 return x64*x8*x4*x2;
}

float luma(vec3 c){ return dot(c,vec3(.3333)); }

float calcFresnel(float ndv, float f0){
 float x=1.0-sat(ndv);
 float x2=x*x;
 return f0+(1.0-f0)*x2*x2*x;
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

float depthExtinctionCurve(float x){
 x=sat(x);
 return sat(x+x*x*(1.0-x)*.42);
}

vec3 waterVolumeAbsorption(vec3 c, float body, float ndv){
 float path=sat(body*TR456_WATER_VOLUME_STRENGTH*TR456_WATER_DEPTH_ABSORPTION*
   (.92+.26*(1.0-ndv)));
 float curve=depthExtinctionCurve(path);
 float horizontal=sat(body*1.12);
 vec3 shoreTint=vec3(.018,.120,.136)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 vec3 surfaceTint=vec3(.006,.082,.100)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 vec3 deepTint=vec3(.002,.038,.050)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 vec3 waterTint=mix(shoreTint,surfaceTint,smoothstep(.10,.56,horizontal));
 waterTint=mix(waterTint,deepTint,smoothstep(.46,.98,horizontal));
 vec3 absorbed=c*exp(-vec3(.70,.30,.14)*path);
 float y=luma(absorbed);
 absorbed=mix(absorbed,vec3(y)*vec3(.63,.84,.91),sat(curve*.32));
 absorbed=mix(absorbed,waterTint,sat(path*.22+horizontal*.10)*TR456_WATER_OPACITY);
 return mix(absorbed,deepTint,sat(curve*.14*TR456_WATER_OPACITY));
}

vec4 pixelWaveField(vec2 p, float time){
 p*=.86;
 vec2 a=normalize(vec2(.86,.51));
 vec2 b=normalize(vec2(-.42,.91));
 vec2 c=normalize(vec2(.19,.98));
 float phA=dot(p,a)*6.45+sin(dot(p,b)*1.08+time*.18)*.34+time*.67;
 float phB=dot(p,b)*4.10+sin(dot(p,a)*.74-time*.13)*.22-time*.39;
 float phC=dot(p,c)*8.70+sin(dot(p,vec2(.72,-.69))*1.22+time*.11)*.15+time*.82;
 float h=sin(phA)*.50+sin(phA*2.0+.60)*.11+sin(phB)*.25+sin(phC)*.14;
 vec2 grad=a*(cos(phA)*.50+cos(phA*2.0+.60)*.22)+
   b*(cos(phB)*.25)+c*(cos(phC)*.14);
 float band=pow(sat(1.0-abs(fract(phA*.159+sin(phB)*.040)-.5)*2.0),4.6);
 float crest=sat(abs(h)*.70+band*.34);
 return vec4(grad*.0170*TR456_WATER_PIXEL_WAVE_STRENGTH,
   crest*TR456_WATER_PIXEL_WAVE_STRENGTH,h);
}

float deepCausticField(vec2 p, float time, float depthMask, vec2 waveBend){
 p+=waveBend*.26+vec2(time*.020,-time*.013);
 float n0=texture(sTex1,vec3(p*.82+vec2(.19,.41),0)).x;
 float n1=texture(sTex1,vec3(p*1.46+vec2(.59,.07),0)).y;
 float n2=texture(sTex1,vec3(p.yx*2.10+vec2(.26,.77),0)).z;
 float web=1.0-abs((n0*.50+n1*.33+n2*.17)-.54)*4.8;
 float vein=1.0-abs(n0-n2)*3.7;
 float scatter=texture(sTex1,vec3(p*.40+vec2(.13,.29),0)).x;
 float broken=smoothstep(.40,.86,texture(sTex1,vec3(p*2.65+vec2(.71,.23),0)).x)*
   (.55+.45*smoothstep(.22,.82,scatter));
 float soft=smoothstep(.18,.86,depthMask)*(1.0-smoothstep(.82,1.0,depthMask)*.42);
 float lines=pow(sat(web),2.20)*pow(sat(vein),1.18);
 lines=smoothstep(.015,.50,lines)*lines;
 return lines*broken*soft*
   TR456_WATER_DEEP_CAUSTICS_STRENGTH*TR456_WATER_BOTTOM_CAUSTICS;
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
  float left=exp(-fastPow2((p.x+armX)/armWidth))*rear;
  float right=exp(-fastPow2((p.x-armX)/armWidth))*rear;
  float stem=exp(-fastPow2(p.x/max(width*.18,.040)))*rear*
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
 p*=.38;
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
 float crest=sat(abs(h)*.58+fastPow3(sat(abs(h)))*.30);
 return vec3(slope*.010,crest);
}

float screenFade(vec2 uv){
 vec2 a=smoothstep(vec2(.015),vec2(.080),uv);
 vec2 b=smoothstep(vec2(.015),vec2(.080),vec2(1.0)-uv);
 return a.x*a.y*b.x*b.y;
}

float screenShoreFoamMask(vec2 screen, float edge, float depth, float time){
 float n0=texture(sTex1,vec3(screen*vec2(7.0,4.0)+vec2(time*.020,-time*.012),0)).x;
 float n1=texture(sTex1,vec3(screen*vec2(13.0,8.0)+vec2(-time*.014,time*.018),0)).y;
 float inner=smoothstep(.08,.34,edge+n0*.16);
 float shallow=1.0-smoothstep(.48,.96,depth+(n1-.5)*.14);
 float lace=pow(sat(1.0-abs(fract(screen.x*9.0+screen.y*5.0+n0*.40+time*.030)-.5)*2.0),3.6);
 return sat((inner*.62+lace*.38)*shallow*screenFade(screen));
}

vec3 sceneColor(vec2 uv){
 return texture(sTex0,vec3(clamp(uv,vec2(0.0),vec2(1.0)),0)).rgb;
}

vec3 reflectColor(vec2 uv){
 return texture(sEnvMap,vec3(clamp(uv,vec2(0.0),vec2(1.0)),0)).rgb;
}

vec3 fboColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

float reflectionUvFade(vec2 uv){
 vec2 a=smoothstep(vec2(-.060),vec2(.120),uv);
 vec2 b=smoothstep(vec2(-.060),vec2(.120),vec2(1.0)-uv);
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

vec3 stableSceneColor(vec2 uv, vec2 fallback){
 return mix(sceneColor(fallback),sceneColor(uv),reflectionUvFade(uv));
}

vec3 stableFboColor(vec2 uv, vec2 fallback){
 return mix(fboColor(fallback),fboColor(uv),reflectionUvFade(uv));
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.84,.95,1.04),.16);
 return clamp(c,vec3(0.0),vec3(3.0));
}

void main(){
 float time=uModelMatrix[3].x;
 vec2 flowA=vec2(time*.020,-time*.014);
 vec2 flowB=vec2(-time*.012,time*.017);
 vec2 flowC=vec2(time*.005,time*.008);
 vec3 e0=texture(sTex1,vec3(vTexCoord+flowA,0)).xyz*2.0-1.0;
 vec3 e1=texture(sTex1,vec3(vTexCoord*1.73+flowB,0)).xyz*2.0-1.0;
 vec3 e2=texture(sTex1,vec3(vTexCoord*3.10+flowC,0)).xyz*2.0-1.0;
 float h=luma(e0);
 float hx=luma(texture(sTex1,vec3(vTexCoord+flowA+vec2(.004,0.0),0)).xyz*2.0-1.0)-h;
 float hy=luma(texture(sTex1,vec3(vTexCoord+flowA+vec2(0.0,.004),0)).xyz*2.0-1.0)-h;
 vec3 heightNormal=normalize(vec3(-hx*18.0,1.0,-hy*18.0));
 vec3 n=normalize(vNormal*.48+heightNormal*.34+(e0*.30+e1*.14+e2*.045)*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF);
 if(!gl_FrontFacing)n=-n;

 vec2 px=uAmbient[2].xy;
 vec2 screen=gl_FragCoord.xy*px;
 vec3 wake=vec3(0.0);
 float contactHeight=wake.z;
 vec2 ripple=wake.xy*1.24;
 vec2 micro=microRipple(vTexCoord*1.70+vec2(time*.010,-time*.007),time);
 vec3 swell=surfaceSwell(vTexCoord*.64,time);
 vec3 safeVolume=safeVolumeLayer(vTexCoord,time)*TR456_WATER_SAFE_VOLUME;
 vec3 edgeRip=edgeRippleLayer(vTexCoord*1.35,screen,time);
 vec4 pixelWave=pixelWaveField(vTexCoord*.72+vec2(time*.006,-time*.004),time);
 float maskC=texture(sTex0,vec3(clamp(screen,vec2(0.0),vec2(1.0)),0)).w;
 float maskL=texture(sTex0,vec3(clamp(screen-vec2(px.x*2.0,0.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskR=texture(sTex0,vec3(clamp(screen+vec2(px.x*2.0,0.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskD=texture(sTex0,vec3(clamp(screen-vec2(0.0,px.y*2.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskU=texture(sTex0,vec3(clamp(screen+vec2(0.0,px.y*2.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskEdge=smoothstep(.035,.20,abs(maskR-maskC)+abs(maskL-maskC)+abs(maskU-maskC)+abs(maskD-maskC));
 vec2 maskDir=normalize(vec2(maskR-maskL,maskU-maskD)+vec2(.0001));
 edgeRip.xy+=maskDir*maskEdge*.006*TR456_WATER_EDGE_WAVE;
 edgeRip.z=max(edgeRip.z,maskEdge*.42*TR456_WATER_EDGE_WAVE);
 float microEnergy=sat(length(micro)*30.0);
 vec2 waveSlope=vec2(ripple.x*.96+micro.x*.44+swell.x*1.06+safeVolume.x*1.25+edgeRip.x*.90+
   pixelWave.x*1.90,ripple.y*.96+micro.y*.44+swell.y*1.06+safeVolume.y*1.25+edgeRip.y*.90+
   pixelWave.y*1.90);
 n=normalize(n+vec3(waveSlope.x*8.4,0.0,-waveSlope.y*8.4)*TR456_WATER_SURFACE_RELIEF);
 vec2 bumpSlope=vec2(0.0);
 float bumpEnergy=0.0;
#if TR456_WATER_BUMP_ENABLED
 bumpSlope=(waveSlope*1.32+micro*6.4+swell.xy*3.0+
   safeVolume.xy*3.1+edgeRip.xy*2.6+pixelWave.xy*2.0)*
   max(TR456_WATER_BUMP_SCALE,.10);
 n=applyWaterBump(n,bumpSlope,
   TR456_WATER_BUMP_STRENGTH*.62*TR456_WATER_SURFACE_RELIEF);
 bumpEnergy=sat(length(bumpSlope)*2.40*TR456_WATER_BUMP_STRENGTH);
#endif
 vec2 normalBend=vec2(n.x,-n.z);
 vec2 bend=(normalBend*74.0*TR456_WATER_SURFACE_RELIEF+
   (ripple*.86+micro*.36+swell.xy*.92+safeVolume.xy*1.10+edgeRip.xy*.72+pixelWave.xy*1.55)*860.0)*
   TR456_WATER_REFRACT_STRENGTH*TR456_WATER_REFRACTION_WAVE_STRENGTH*
   TR_TOGGLE_SURFACE_WARP;
 bend.y=-bend.y;
 vec2 uvRefract=screen-bend*px;
 float refractWaterMask=texture(sTex0,vec3(clamp(uvRefract,vec2(0.0),vec2(1.0)),0)).w;
 float notWater=1.0-smoothstep(.18,.82,refractWaterMask);
 uvRefract+=bend*px*notWater;

 vec2 chroma=normalize(bend+vec2(.001))*px*(1.15*TR456_WATER_CHROMA_STRENGTH);
 vec3 refr;
 refr.r=sceneColor(uvRefract+chroma).r;
 refr.g=sceneColor(uvRefract).g;
 refr.b=sceneColor(uvRefract-chroma).b;
 refr*=uAmbient[0].rgb;
 refr=mix(vec3(luma(refr)),refr,clamp(TR456_WATER_TEXTURE_STRENGTH,.65,1.70));

 vec3 vv=normalize(vCamera);
 vec3 lightDir=normalize((-uViewMatrix[2].xyz)*vec3(1.0,1.0,-1.0));
 vec3 lightReflect=reflect(-lightDir,n);
 float forced=clamp(TR456_WATER_FORCE_REFLECTION,0.0,1.0);
 float typeWater=max(uAmbient[5].x,forced);
 float hasRefl=max(uAmbient[5].y,forced);
 float reflStrength=max(clamp(uAmbient[5].z,0.0,1.0),forced)*TR456_WATER_REFLECT_STRENGTH;
 float authoredBlend=max(clamp(uAmbient[5].w,0.0,1.0),forced);
 float ndv=sat(dot(n,vv));
 float fres=calcFresnel(ndv,.10);
 fres=sat(mix(.08+sqrt(fres)*.16,sqrt(fres),hasRefl)*typeWater*TR456_WATER_FRESNEL_STRENGTH);

 vec2 ray=normalize(vec2(n.x+micro.x*.48+swell.x*.94+edgeRip.x*.82+pixelWave.x*.82,
   -n.z-micro.y*.48-swell.y*.94-edgeRip.y*.82-pixelWave.y*.82)+vec2(.001))*
   px*(108.0*TR456_WATER_SSR_STRENGTH*TR456_WATER_MIRROR_ROUGHNESS);
 vec2 uvReflect=screen+bend*px*(.62+.92*fres)*TR456_WATER_SSR_STRENGTH*TR456_WATER_MIRROR_ROUGHNESS;
 float fade=mix(.50,1.0,reflectionUvFade(uvReflect));
 vec3 refl0=reflectColor(uvReflect);
#if TR456_WATER_REFLECTION_QUALITY <= 0
 vec3 envRefl=reflectionGrade(refl0);
#elif TR456_WATER_REFLECTION_QUALITY == 1
 vec3 refl1=reflectColor(uvReflect+ray*.46);
 vec3 envRefl=reflectionGrade(refl0*.68+refl1*.32);
#else
 vec3 refl1=reflectColor(uvReflect+ray*.34);
 vec3 refl2=reflectColor(uvReflect+ray*.78);
 vec3 refl3=reflectColor(uvReflect-ray*.22);
 vec3 envRefl=reflectionGrade(refl0*.42+refl1*.27+refl2*.18+refl3*.13);
#endif
 vec2 mirrorOffset=vec2(bend.x+micro.x*62.0+swell.x*138.0+edgeRip.x*98.0,
   -abs(bend.y+micro.y*54.0+swell.y*126.0+edgeRip.y*98.0)*.42)*
   px*(.46+.66*fres)*TR456_WATER_SSR_STRENGTH*
   TR456_WATER_MIRROR_ROUGHNESS;
 vec3 reflectNormal=normalize(n+vec3(mirrorOffset.x*18.0,0.0,
   -mirrorOffset.y*18.0));
 vec2 mirrorUv=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorOffset,0.0,.42+.48*fres);
 vec2 mirrorUv2=preciseReflectionUv(screen,reflectNormal,vv,
   mirrorOffset*.55+ray*.24,.010+.012*fres,.34+.40*fres);
#if TR456_WATER_REFLECTION_QUALITY <= 0
 vec3 flipRefl=reflectionGrade(stableSceneColor(mirrorUv,screen));
#else
 vec3 flipRefl=reflectionGrade(stableSceneColor(mirrorUv,screen)*.66+
   stableSceneColor(mirrorUv2,screen)*.34);
#endif
 vec2 noFlipWarp=vec2(bend.x+micro.x*48.0+swell.x*104.0+edgeRip.x*76.0,
   -bend.y-micro.y*48.0-swell.y*104.0-edgeRip.y*76.0)*
   px*(.22+.40*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 noFlipUv=preciseReflectionUv(screen,n,vv,noFlipWarp,0.0,.20+.32*fres);
 vec3 localRefl=reflectionGrade(stableSceneColor(noFlipUv,screen));
 float grazing=smoothstep(.18,.72,1.0-ndv);
 vec3 sceneRefl=mix(localRefl,flipRefl,clamp(.48+.42*grazing,0.0,1.0));
 vec3 fboRefl=vec3(0.0);
#if TR456_WATER_FBO_REFLECTION == 1
#if TR456_WATER_REFLECTION_QUALITY <= 0
 fboRefl=reflectionGrade(stableFboColor(noFlipUv,screen));
#elif TR456_WATER_REFLECTION_QUALITY == 1
 vec3 fboFlip=stableFboColor(mirrorUv,screen)*.68+
   stableFboColor(mirrorUv+ray*.38,screen)*.32;
 vec3 fboFlat=stableFboColor(noFlipUv,screen);
 fboRefl=reflectionGrade(mix(fboFlat,fboFlip,clamp(.62+.26*grazing,0.0,1.0)));
#else
 vec3 fboFlip=stableFboColor(mirrorUv,screen)*.56+
   stableFboColor(mirrorUv+ray*.45,screen)*.28+
   stableFboColor(mirrorUv-ray*.22,screen)*.16;
 vec3 fboFlat=stableFboColor(noFlipUv,screen);
 fboRefl=reflectionGrade(mix(fboFlat,fboFlip,clamp(.62+.26*grazing,0.0,1.0)));
#endif
 float fboOk=smoothstep(.015,.075,luma(fboRefl));
 sceneRefl=mix(sceneRefl,fboRefl,.86*fboOk*TR456_WATER_SCENE_REFLECTION);
#endif
 sceneRefl=mix(localRefl,sceneRefl,fade*TR456_WATER_SCENE_REFLECTION);
 vec3 refl=mix(sceneRefl,envRefl,clamp(authoredBlend*.12,0.0,.24));
 refl=mix(refl,refl*vec3(.82,.94,1.06),.16);

 float spec=fastPow78(max(dot(lightReflect,vv),0.0))*.85;
 float sparkle=fastPow6(max(e0.x*e1.y+e2.z*.28,0.0))*.075+contactHeight*.034+
   swell.z*.014+safeVolume.z*.014+edgeRip.z*.016+pixelWave.z*.010+
   microEnergy*.008+bumpEnergy*.014;
 float crest=max(smoothstep(.42,.88,e0.z*.52+e1.z*.28+e2.z*.12),
   max(max(max(max(max(contactHeight*.70,swell.z*.24),safeVolume.z*.30),edgeRip.z*.34),pixelWave.z*.40),microEnergy*.12+bumpEnergy*.10));
 float foam=smoothstep(.24,.92,notWater)*(crest*.085+contactHeight*.048+swell.z*.014+safeVolume.z*.012+edgeRip.z*.026+microEnergy*.006)*
   TR456_WATER_FOAM_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 float deepMask=smoothstep(.12,.92,notWater)*screenFade(screen);
 float shoreFoam=screenShoreFoamMask(screen,maskEdge,notWater,time)*
   TR456_WATER_FOAM_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 foam+=shoreFoam*.080;
 float caustic=0.0;
#if TR456_WATER_SURFACE_CAUSTICS_ENABLED
 caustic=deepCausticField(uvRefract+pixelWave.xy*.040,time,deepMask,bend*px);
 caustic=caustic*(.026+.096*deepMask)*deepMask*TR456_WATER_CAUSTICS_STRENGTH*
   TR_TOGGLE_SURFACE_CAUSTICS;
#endif
 vec3 tint=vec3(.010,.130,.165)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 float depth=sat(notWater*.80+(1.0-ndv)*.35)*TR456_WATER_DEPTH_STRENGTH;
 float volumeDepth=sat(notWater*.82+deepMask*.34+(1.0-ndv)*.16)*TR456_WATER_DEPTH_STRENGTH;
  refr=waterVolumeAbsorption(refr,volumeDepth,ndv);
  refr=mix(refr,tint,(.070+.15*depth+.045*fres)*typeWater*TR456_WATER_OPACITY);

  refr+=vec3(.16,.34,.28)*caustic*(.55+.45*deepMask);
 vec3 highlight=uAmbient[1].rgb*(spec*.92+sparkle*TR456_WATER_GLINT_STRENGTH)*
   TR_TOGGLE_SURFACE_FOAM+
   vec3(.42,.76,.82)*foam+vec3(.18,.32,.34)*shoreFoam;
 vec3 sceneBase=sceneColor(screen);
 vec3 waterBase=mix(sceneBase,refr,clamp(TR456_WATER_OPACITY*.92,0.0,1.0));
 waterBase=mix(waterBase,waterBase*vec3(.58,.80,.90),
   sat((depth*.30+volumeDepth*.18)*TR456_WATER_DEPTH_ABSORPTION));
 waterBase+=vec3(.020,.048,.054)*(crest+sparkle*.65+shoreFoam*.36)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 float reflectAmount=0.0;
 vec3 col=mix(waterBase,refl,reflectAmount);
 col+=highlight*(.50+.50*(1.0-reflectAmount));

#if TR456_WATER_DEBUG_MODE == 1
 fragColor=vec4(abs(n),1.0);
#elif TR456_WATER_DEBUG_MODE == 2
 fragColor=vec4(vec3(fres),1.0);
#elif TR456_WATER_DEBUG_MODE == 3
 fragColor=vec4(vec3(fade),1.0);
#elif TR456_WATER_DEBUG_MODE == 4
 fragColor=vec4(vec3(notWater),1.0);
#elif TR456_WATER_DEBUG_MODE == 5
 fragColor=vec4(sceneRefl,1.0);
#elif TR456_WATER_DEBUG_MODE == 6
 fragColor=vec4(reflectionGrade(fboColor(screen)),1.0);
#elif TR456_WATER_DEBUG_MODE == 7
 fragColor=vec4(vec3(reflectAmount),1.0);
#elif TR456_WATER_DEBUG_MODE == 9
 fragColor=vec4(wake.z,swell.z,edgeRip.z,1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(1.0,.0,1.0,1.0);
#else
 fragColor=vec4(col,1.0);
#endif
}
