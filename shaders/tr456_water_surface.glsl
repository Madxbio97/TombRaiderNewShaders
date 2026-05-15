#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.36
#define TR456_WATER_PIXEL_WAVE_STRENGTH 2.18
#define TR456_WATER_REFRACTION_WAVE_STRENGTH 2.45
#define TR456_WATER_DEEP_CAUSTICS_STRENGTH 1.0
#define TR456_WATER_VOLUME_STRENGTH 1.0
#define TR456_WATER_SHORELINE_STRENGTH 0.75
#define TR456_WATER_GAME_RIPPLE_STRENGTH 1.58
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
#define TR456_WATER_CONTACT_WAVE_STRENGTH 1.12
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.0
#define TR456_WATER_CONTACT_WAVE_SPEED 0.98
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.95
#define TR456_WATER_CONTACT_COORD_MODE 1
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
#define TR456_WATER_CONTACT_EDGE 0.72
#define TR456_WATER_CALM_MIRROR 0.72
#define TR456_WATER_RAIN_RIPPLE 1.06
#define TR456_WATER_WET_EDGE 0.84
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_TEXTURE_STRENGTH 1.38
#define TR456_WATER_FBO_REFLECTION 0
#define TR456_WATER_SAFE_VOLUME 1.05
#define TR456_WATER_TILE_SEAM_SOFTENING 1.0
#define TR456_WATER_TILE_SEAM_WIDTH 0.035
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
#ifndef TR456_WATER_BUMP_STRENGTH
#define TR456_WATER_BUMP_STRENGTH 0.0
#endif
#ifndef TR456_WATER_BUMP_SCALE
#define TR456_WATER_BUMP_SCALE 1.0
#endif
#ifndef TR456_WATER_BUMP_ENABLED
#define TR456_WATER_BUMP_ENABLED 0
#endif
#ifndef TR456_WATER_AUTHORED_REFLECTION_ENABLED
#define TR456_WATER_AUTHORED_REFLECTION_ENABLED 1
#endif
#ifndef TR456_WATER_SURFACE_CAUSTICS_ENABLED
#define TR456_WATER_SURFACE_CAUSTICS_ENABLED 1
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uContacts[16];
uniform vec4 uContactMotion[16];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uColor;
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterToggle2;
uniform vec4 uTrWaterDrawInfo;
in vec2 vTexCoord;
in vec3 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
in vec3 vWorldPos;
in vec3 vContactWave;
out vec4 fragColor;

#define TR_TOGGLE_SURFACE_WARP uTrWaterToggle1.z
#define TR_TOGGLE_SURFACE_CAUSTICS uTrWaterToggle1.w
#define TR_TOGGLE_SURFACE_FOAM uTrWaterToggle2.x
#define TR_TOGGLE_SURFACE_REFLECTION uTrWaterToggle2.y
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }
float fastPow2(float x){ return x*x; }
float fastPow3(float x){ return x*x*x; }
float fastPow5(float x){ float x2=x*x; return x2*x2*x; }
float fastPow8(float x){ float x2=x*x; float x4=x2*x2; return x4*x4; }
float luma(vec3 c){ return dot(c,vec3(.3333)); }

vec2 gameSurfaceDir(){
 vec2 fallback=normalize(vec2(.86,.50));
 float d=dot(uParams.xy,uParams.xy);
 return (d>.000001) ? normalize(uParams.xy) : fallback;
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

float openLaraDrop(float nr){
 float x=max(0.0,1.0-nr);
 return 0.5-cos(x*3.141592653589793)*0.5;
}

float openLaraDropSlope(float nr, float r){
 float x=max(0.0,1.0-nr);
 return -0.5*3.141592653589793*sin(x*3.141592653589793)/max(r,1.0);
}

float tileSeamGuard(vec2 p){
 vec2 f=fract(p);
 vec2 edge=min(f,1.0-f);
 float d=min(edge.x,edge.y);
 float width=clamp(TR456_WATER_TILE_SEAM_WIDTH,.002,.08);
 float interior=smoothstep(width*.18,width,d);
 float guarded=mix(.82,1.0,interior);
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
 return sat(x+x*x*(1.0-x)*.42);
}

vec3 waterVolumeAbsorption(vec3 c, float body, float ndv){
 float path=sat(body*TR456_WATER_DEPTH_ABSORPTION*(.88+.24*(1.0-ndv)));
 float curve=depthExtinctionCurve(path);
 float horizontal=sat(body*1.18);
 vec3 shoreTint=vec3(.020,.118,.126)*TR456_WATER_TINT_STRENGTH;
 vec3 surfaceTint=vec3(.006,.076,.092)*TR456_WATER_TINT_STRENGTH;
 vec3 deepTint=vec3(.002,.034,.044)*TR456_WATER_TINT_STRENGTH;
 vec3 waterTint=mix(shoreTint,surfaceTint,smoothstep(.10,.55,horizontal));
 waterTint=mix(waterTint,deepTint,smoothstep(.48,.98,horizontal));
 vec3 absorbed=c*exp(-vec3(.66,.28,.13)*path);
 float y=luma(absorbed);
 absorbed=mix(absorbed,vec3(y)*vec3(.64,.85,.91),sat(curve*.30));
 absorbed=mix(absorbed,waterTint,sat(path*.20+horizontal*.10)*TR456_WATER_OPACITY);
 return mix(absorbed,deepTint,sat(curve*.12*TR456_WATER_OPACITY));
}

float shoreFoamMask(vec2 p, float shore, float depthHint, float time){
 float n0=texture(sNoise,vec3(p*.56+vec2(time*.018,-time*.012),time*.010)).x;
 float n1=texture(sNoise,vec3(p*1.18+vec2(-time*.011,time*.017),time*.016)).x;
 float ranges=shore+(n0-.5)*.24+(n1-.5)*.12;
 float inner=smoothstep(.16,.58,ranges);
 float outer=1.0-smoothstep(.50,.96,depthHint+(n1-.5)*.16);
 float lace=pow(sat(1.0-abs(fract(dot(p,vec2(.37,.19))+n0*.38+time*.030)-.5)*2.0),3.4);
 return sat((inner*.62+lace*.38)*outer*shore);
}

vec3 softShoreLapping(vec2 p, vec2 dir, float shore, float time){
 vec2 flowDir=normalize(dir+vec2(.0001,.0003));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 vec2 q=vec2(dot(p,flowDir),dot(p,side));
 float n0=texture(sNoise,vec3(q*.018+vec2(time*.018,-time*.012),time*.010)).x;
 float n1=texture(sNoise,vec3(q*.041+vec2(-time*.010,time*.017),time*.014)).x;
 float phase=q.x*.030+sin(q.y*.016+n0*1.7+time*.20)*.32-time*.56;
 float backwash=q.x*.018+sin(q.y*.024+n1*1.4-time*.17)*.22+time*.34;
 float lap=pow(sat(sin(phase)*.5+.5),2.35)*shore;
 float returnLap=pow(sat(sin(backwash)*.5+.5),3.4)*shore*.42;
 float lace=pow(sat(1.0-abs(fract(q.y*.018+n1*.35+time*.040)-.5)*2.0),4.2);
 float soft=(lap+returnLap)*(.58+.42*lace)*(1.0-smoothstep(.72,1.0,shore));
 vec2 slope=flowDir*(cos(phase)*lap*.0072-cos(backwash)*returnLap*.0038)+
   side*(n0-n1)*soft*.0038;
 return vec3(slope,soft);
}

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

vec4 pixelWaveField(vec2 p, float time){
 p*=.92;
 vec2 a=normalize(vec2(.88,.47));
 vec2 b=normalize(vec2(-.36,.93));
 vec2 c=normalize(vec2(.22,.98));
 float bendA=sin(dot(p,b)*1.22+time*.42)*.34;
 float bendB=sin(dot(p,a)*.86-time*.28)*.26;
 float phA=dot(p,a)*6.10+bendA+time*.92;
 float phB=dot(p,b)*4.25+bendB-time*.68;
 float phC=dot(p,c)*8.40+sin(dot(p,vec2(.71,-.70))*1.15+time*.24)*.18+time*1.08;
 float h=sin(phA)*.50+sin(phA*2.0+.55)*.10+sin(phB)*.27+sin(phC)*.13;
 vec2 grad=a*(cos(phA)*.50+cos(phA*2.0+.55)*.20)+
   b*(cos(phB)*.27)+c*(cos(phC)*.13);
 float crest=sat(abs(h)*.72+.18);
 float longBand=pow(sat(1.0-abs(fract(phA*.159+sin(phB)*.045)-.5)*2.0),4.8);
 return vec4(grad*.0240*TR456_WATER_PIXEL_WAVE_STRENGTH,
   sat(crest*.58+longBand*.46)*TR456_WATER_PIXEL_WAVE_STRENGTH,h);
}

float deepCausticField(vec2 p, float time, float depthMask, vec2 waveBend){
 p+=waveBend*.18+vec2(time*.018,-time*.011);
 float n0=texture(sNoise,vec3(p*.92+vec2(.17,.43),time*.017)).x;
 float n1=texture(sNoise,vec3(p*1.71+vec2(.53,.11),time*.013)).x;
 float n2=texture(sNoise,vec3(p.yx*2.38+vec2(.29,.71),time*.011)).x;
 float web=1.0-abs((n0*.50+n1*.34+n2*.16)-.54)*4.7;
 float vein=1.0-abs(n0-n2)*3.8;
 float scatter=texture(sNoise,vec3(p*.44+vec2(.11,.29),time*.007)).x;
 float broken=smoothstep(.42,.86,texture(sNoise,vec3(p*2.85+vec2(.67,.23),time*.019)).x)*
   (.55+.45*smoothstep(.22,.82,scatter));
 float soft=smoothstep(.24,.86,depthMask)*(1.0-smoothstep(.82,1.0,depthMask)*.42);
 float lines=pow(sat(web),2.25)*pow(sat(vein),1.20);
 lines=smoothstep(.015,.52,lines)*lines;
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
 float phaseA=dot(p,vec2(1.30,.42))*6.28318+time*.54;
 float phaseB=dot(p,vec2(-.55,1.12))*6.28318-time*.42;
 vec2 center=p-vec2(.35,-.25);
 float phaseC=length(center)*7.4-time*.62;
 float h=sin(phaseA)*.46+sin(phaseB)*.34+sin(phaseC)*.20;
 vec2 radial=normalize(center+vec2(.0001));
 vec2 grad=vec2(cos(phaseA)*1.30+cos(phaseB)*(-.55*.72),
                cos(phaseA)*.42+cos(phaseB)*(1.12*.72));
 grad+=radial*cos(phaseC)*.58;
 float crest=sat(abs(h)*.74+.10);
 return vec3(grad*.0092*TR456_WATER_SWELL_STRENGTH,crest*TR456_WATER_SWELL_STRENGTH);
}

float calmMirrorMask(vec2 p, float time){
 p*=.18;
 float broad=texture(sNoise,vec3(p*.28+vec2(time*.006,-time*.004),time*.004)).x;
 float cell=texture(sNoise,vec3(p*.78+vec2(-time*.003,time*.005),time*.003)).x;
 float ribbon=pow(sat(1.0-abs(fract(dot(p,vec2(.74,.31))+broad*.18)-.5)*2.0),3.2);
 float calm=smoothstep(.48,.82,broad*.58+cell*.34+ribbon*.08);
 return calm*TR456_WATER_CALM_MIRROR;
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

vec3 contactWaveFieldPixel(vec3 pos, vec2 screen, vec2 invViewport){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,3.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 float life=160.0;
 vec2 slope=vec2(0.0);
 float crest=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float contactOn=step(.001,dot(abs(c),vec4(1.0)));
   if(contactOn<=.001) continue;
   float radius=contactRadius(c);
   float screenContact=isScreenContact(c);
   vec2 delta;
   float vertical;
   if(screenContact>.5) {
     vec2 inv=max(invViewport,vec2(1.0/8192.0));
     delta=(screen-c.xy)/inv;
     vertical=1.0;
   } else {
     vec2 deltaXZ=pos.xz-c.xz;
     vec2 deltaXY=pos.xy-c.xy;
     float dXZ=length(deltaXZ);
     float dXY=length(deltaXY);
     float autoXY=step(dXY,dXZ);
     float mode=float(TR456_WATER_CONTACT_COORD_MODE);
     float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
     delta=mix(deltaXZ,deltaXY,useXY);
     vertical=1.0-smoothstep(radius*.22,radius*1.24,abs(pos.y-c.y));
   }
   float d=length(delta)+.001;
   vec2 dir=delta/d;
   float age=clamp(contactAge(c),0.0,life);
    float fade=contactOn*(1.0-smoothstep(life*.62,life,age));
    float grow=smoothstep(0.0,life*.74,age);

    if(screenContact>.5) {
      float r=max(radius,1.0);
      float nr=d/r;
      float drop=openLaraDrop(nr);
      float dDrop=openLaraDropSlope(nr,r);
      float ringCenter=mix(.12,.62,grow);
      float ringWidth=mix(.044,.064,grow);
      float leadX=(nr-ringCenter)/ringWidth;
      float troughX=(nr-(ringCenter-ringWidth*1.18))/(ringWidth*1.55);
      float lipX=(nr-.72)/.050;
      float longCenter=ringCenter+.18+.16*grow;
      float longWidth=ringWidth*(2.20+.80*grow);
      float longX=(nr-longCenter)/longWidth;
      float lead=exp(-leadX*leadX);
      float trough=exp(-troughX*troughX);
      float lip=exp(-lipX*lipX)*smoothstep(.18,.80,grow);
      float longRing=exp(-longX*longX)*sin(nr*36.0-age*(.050+.018*speed));
      vec2 p=delta/r;
      float rear=smoothstep(-.050,.095,p.y)*(1.0-smoothstep(.52,1.35,p.y))*
        smoothstep(.08,.62,grow);
      float armX=p.y*(.34+.18*grow);
      float armWidth=.045+max(p.y,0.0)*.035;
      float left=exp(-fastPow2((p.x+armX)/armWidth))*rear;
      float right=exp(-fastPow2((p.x-armX)/armWidth))*rear;
      float stem=exp(-fastPow2(p.x/.075))*rear*(1.0-smoothstep(.18,.82,p.y));
      float yPhase=p.y*64.0+abs(p.x)*18.0-age*(.10+.032*speed);
      float yWave=sin(yPhase+sin(p.x*10.0)*.22)*(left+right)*TR456_WATER_WAKE_WAVE;
      float stemWave=sin(p.y*78.0-age*(.12+.035*speed))*stem*TR456_WATER_WAKE_WAVE;
      vec2 ySlope=vec2((right-left)*(.030+.010*abs(sin(yPhase))),
        -(left+right)*(.020+.010*abs(cos(yPhase)))-stem*.018)*
        (yWave*.75+stemWave*.44);
      float center=drop*(1.0-smoothstep(1.0,34.0,age));
      float spriteMask=(1.0-smoothstep(.78,.92,nr))*smoothstep(.018,.075,nr);
      float osc=cos(age*(.18+.035*speed));
      float h=(lead*.50-trough*.22+lip*.10)*spriteMask+
        longRing*.22*smoothstep(.16,.88,grow)+
        yWave*.18+stemWave*.10+center*.14*osc;
      float dLead=(-2.0*leadX/(ringWidth*r))*lead;
      float dTrough=(-2.0*troughX/(ringWidth*1.55*r))*trough;
      float dLip=(-2.0*lipX/(.050*r))*lip;
      float dLong=((-2.0*longX/(longWidth*r))*sin(nr*36.0-age*(.050+.018*speed))+
        cos(nr*36.0-age*(.050+.018*speed))*(36.0/r))*exp(-longX*longX);
      float ringSlope=(dLead*.50-dTrough*.22+dLip*.10)*spriteMask+
        dLong*.22*smoothstep(.16,.88,grow)+dDrop*.14*osc;
      slope+=(dir*ringSlope*max(r*.040,1.0)+ySlope)*fade;
      crest+=sat(abs(h)*.90+lead*.12+lip*.10+abs(longRing)*.18+
        abs(yWave)*.32+abs(stemWave)*.18)*fade;
      continue;
    }

    float front=mix(max(64.0,radius*(.50+.15*grow)+age*(1.35+speed*1.20)),
      max(11.0,radius*(.42+.10*grow)+age*(.30+speed*.22)),screenContact);
    float width=mix(max(58.0,radius*(.095+.034*grow)),
      max(10.5,radius*(.055+.018*grow)),screenContact);
   float crestX=(d-front)/width;
    float troughX=(d-(front-width*.94))/(width*1.52);
   float crestRing=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
    float trailX=(d-(front-width*2.15))/(width*1.85);
    float trail=exp(-trailX*trailX)*smoothstep(.16,.72,grow);
    float longX=(d-(front+width*1.55))/(width*(2.65+.70*grow));
    float longRing=exp(-longX*longX)*sin((d-front)*(.032+.004*speed)-age*.045);
    float shell=(1.0-smoothstep(radius*3.05,radius*4.35,d))*vertical;
    float phase=(d-front)*(.026+.003*speed);
    float rim=(crestRing*.74-trough*.28+trail*.30+
      longRing*.14*smoothstep(.12,.82,grow)+
      sin(phase)*crestRing*.10+sin(phase*1.58+1.25)*crestRing*.04)*shell;
    float dCrest=(-2.0*crestX/width)*crestRing;
    float dTrough=(-2.0*troughX/(width*1.52))*trough;
    float dTrail=(-2.0*trailX/(width*1.85))*trail;
    float dLong=((-2.0*longX/(width*(2.65+.70*grow)))*
      sin((d-front)*(.032+.004*speed)-age*.045)+
      cos((d-front)*(.032+.004*speed)-age*.045)*(.032+.004*speed))*
      exp(-longX*longX);
    float dFine=cos(phase)*(.026+.003*speed)*crestRing*.10+
      cos(phase*1.58+1.25)*(.041+.005*speed)*crestRing*.04;
    float dWave=(dCrest*.74-dTrough*.28+dTrail*.30+dFine+
      dLong*.42*smoothstep(.12,.82,grow))*shell;
    float source=exp(-d/(radius*.36))*(1.0-smoothstep(1.0,50.0,age))*vertical;
    float waterline=source*(.36+.24*sin(d*.062-age*.32+
      texture(sNoise,vec3(pos.xz*.0018,float(i)*.13)).x*2.4));
    float dSource=-source/(radius*.36);
    vec2 deltaXZ=pos.xz-c.xz;
    vec2 deltaXY=pos.xy-c.xy;
    float dXZ=length(deltaXZ);
    float dXY=length(deltaXY);
    float autoXY=step(dXY,dXZ);
    float mode=float(TR456_WATER_CONTACT_COORD_MODE);
    float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
    vec4 motion4=uContactMotion[i];
    vec2 motion=mix(motion4.xz,motion4.xy,useXY);
    float motionLen=length(motion);
    float motionEnergy=max(smoothstep(2.0,26.0,motionLen),
      smoothstep(.010,.070,motionLen/max(radius,1.0)));
    vec2 moveDir=(motionLen>.001) ? motion/motionLen : -dir;
    vec2 trailDir=-moveDir;
    vec2 sideDir=vec2(-trailDir.y,trailDir.x);
    float trailAlong=dot(delta,trailDir)/max(radius,1.0);
    float trailSide=dot(delta,sideDir)/max(radius,1.0);
    float trailMask=motionEnergy*smoothstep(.05,.20,trailAlong)*
      (1.0-smoothstep(1.45,2.85,trailAlong))*vertical;
    float armX=trailAlong*(.30+.10*grow);
    float armWidth=.085+max(trailAlong,0.0)*.045;
    float leftWake=exp(-fastPow2((trailSide+armX)/armWidth))*trailMask;
    float rightWake=exp(-fastPow2((trailSide-armX)/armWidth))*trailMask;
    float centerWake=exp(-fastPow2(trailSide/.13))*trailMask*
      (1.0-smoothstep(.28,1.15,trailAlong));
    float yWake=sin(trailAlong*48.0+abs(trailSide)*16.0-age*(.070+.022*speed))*
      (leftWake+rightWake)*TR456_WATER_WAKE_WAVE;
    float stemWake=sin(trailAlong*58.0-age*(.090+.028*speed))*centerWake*
      TR456_WATER_WAKE_WAVE;
    vec2 wakeSlope=(sideDir*(rightWake-leftWake)*.030-
      trailDir*(leftWake+rightWake+centerWake)*.020)*
      (yWake*.72+stemWake*.40)*TR456_WATER_WAKE_STRENGTH;
    float tensionCenter=radius*(.32+.06*grow);
    float tensionWidth=max(radius*.070,18.0);
    float tensionX=(d-tensionCenter)/tensionWidth;
    float meniscus=exp(-tensionX*tensionX)*shell;
    float dMeniscus=(-2.0*tensionX/tensionWidth)*meniscus;
    dWave+=dMeniscus*.20;
    rim+=meniscus*.14;
    float energy=fade;
    slope+=dir*(dWave*mix(1.00,1.55,screenContact)+dSource*.26+waterline*.010)*energy;
    slope+=wakeSlope*energy;
    crest+=sat(abs(rim)*.78+abs(longRing)*.34+source*.18+abs(waterline)*.16+
      abs(yWake)*.28+abs(stemWake)*.14+meniscus*.18)*energy;
  }
  return vec3(slope*1.62*strength,crest*1.35*strength);
}

vec4 authoredRippleField(vec2 uv, float layer){
 vec2 sx=vec2(.0034,0.0);
 vec2 sy=vec2(0.0,.0034);
 float cc=luma(texture(sTex0_wrap,vec3(uv,layer)).rgb);
 float ll=luma(texture(sTex0_wrap,vec3(uv-sx,layer)).rgb);
 float rr=luma(texture(sTex0_wrap,vec3(uv+sx,layer)).rgb);
 float dd=luma(texture(sTex0_wrap,vec3(uv-sy,layer)).rgb);
 float uu=luma(texture(sTex0_wrap,vec3(uv+sy,layer)).rgb);
 vec2 g=vec2(rr-ll,uu-dd);
 float local=abs(cc-(ll+rr+dd+uu)*.25);
 float ring=smoothstep(.010,.075,local+length(g)*.46);
 float thin=smoothstep(.020,.115,length(g))*(1.0-smoothstep(.18,.50,abs(cc-.50)));
 float mask=sat(ring*.70+thin*.45);
 return vec4(g*mask,mask,cc);
}

vec3 safeVolumeLayer(vec2 p, float time){
 p*=.22;
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

float surfaceLineMask(float x, float power){
 return pow(1.0-abs(fract(x)-.5)*2.0,power);
}

vec3 waterDetailLayer(vec2 p, float time){
 float strength=clamp(TR456_WATER_DETAIL_STRENGTH,0.0,2.0);
 if(strength<=.001) return vec3(0.0);
 float scale=max(TR456_WATER_DETAIL_SCALE,.12);
 vec2 q=p/scale;
 float n0=texture(sNoise,vec3(q*.76+vec2(time*.022,-time*.014),time*.018)).x;
 float n1=texture(sNoise,vec3(q*1.54+vec2(-time*.016,time*.021),time*.021)).x;
 float n2=texture(sNoise,vec3(q.yx*2.42+vec2(.31,.09),time*.014)).x;
 float a=q.x*5.4+q.y*.62+n0*2.0+time*.24;
 float b=q.x*9.8-q.y*.88+n1*2.4-time*.34;
 float c=q.y*11.6+q.x*.48+n2*2.0+time*.20;
 float silk=surfaceLineMask(a*.20+n1*.22,4.0);
 float rip=surfaceLineMask(b*.18+n2*.20,8.4);
 float cross=surfaceLineMask(c*.16+n0*.18,6.2);
 float broken=smoothstep(.20,.82,n0*.48+n1*.34+n2*.18);
 float mask=sat((silk*.42+rip*.38+cross*.24)*broken)*strength;
 vec2 slope=vec2(cos(a)*.008+cos(b)*.006,
   cos(c)*.008+cos(a*.64)*.004)*mask;
 return vec3(slope,mask);
}

vec2 waterBumpLimit(vec2 v, float limit){
 float m=length(v);
 float safeLimit=max(limit,.00001);
 float scale=(safeLimit*(1.0-exp(-m/safeLimit)))/max(m,.00001);
 return v*scale;
}

vec3 applyWaterBump(vec3 baseNormal, vec2 slope, float strength){
 float amount=clamp(strength,0.0,2.0);
 vec2 s=waterBumpLimit(slope*amount,.60);
 return normalize(baseNormal+vec3(-s.x,0.0,-s.y));
}

void main(){
 vec2 uv=vTexCoord;
 vec3 n=normalize(vNormal);
 vec3 viewVec=normalize(-vPos);
 float ndv=sat(dot(n,viewVec));
 float t=uModelMatrix[3].x*uParams.z;
 vec3 tc=vWorldPos.xyz*(uParams.x/1024.0);

 float a=texture(sNoise,tc*.58+n*t*.16).x;
 float b=texture(sNoise,tc*1.07-n*t*.12+vec3(.17,.07,.31)).x;
 float c=texture(sNoise,tc*1.91+n*t*.08+vec3(.41,.13,0.0)).x;
 float d=texture(sNoise,tc*2.80+vec3(-t*.045,t*.025,t*.012)).x;
 vec2 grad=vec2(a-b,c-a)+vec2(d-c,b-d)*.24;
 vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;
 vec3 wake=vec3(0.0);
 vec3 contactPixel=contactWaveFieldPixel(vWorldPos,screen,invViewport)*
   TR_TOGGLE_CONTACT_RIPPLES;
 vec3 contactWave=contactPixel;
 float contactHeight=abs(contactWave.z);
 vec2 stableUv=tc.xz*.72+vec2(t*.024,-t*.016);
 vec2 micro=microRipple(tc.xz*.58+vec2(t*.018,-t*.013),t);
 vec3 swell=surfaceSwell(tc.xz*.28,t);
 vec3 edgeRip=edgeRippleLayer(tc.xz*.30,screen,t);
 vec4 pixelWave=pixelWaveField(tc.xz*.42+vec2(t*.006,-t*.004),t);
 vec3 safeVolume=safeVolumeLayer(tc.xz,t)*TR456_WATER_SAFE_VOLUME;
 vec3 detailTex=waterDetailLayer(tc.xz*.18+vec2(t*.012,-t*.008),t);
 vec3 facet=polygonFacetField(vWorldPos.xz,gameSurfaceDir(),t,.36);
 float facetEnergy=sat(abs(facet.z)*.95+length(facet.xy)*2.2);
 vec4 authoredRing=authoredRippleField(uv,vLayer)*TR_TOGGLE_CONTACT_RIPPLES;
 float surfaceMotion=sat(contactHeight*1.05);
 float calmMirror=calmMirrorMask(tc.xz,t);
 float seamGuard=mix(tileSeamGuard(uv),1.0,.92);
 float seamFade=sat((1.0-seamGuard)*.25);
 wake.xy+=authoredRing.xy*.155*TR456_WATER_GAME_RIPPLE_STRENGTH;
 wake.z+=authoredRing.z*1.10*TR456_WATER_GAME_RIPPLE_STRENGTH;
  wake.xy+=contactWave.xy*.42;
  wake.z+=contactHeight*.70;
 grad+=micro*(.52*TR456_WATER_SURFACE_RELIEF);
 grad+=swell.xy*(1.25*TR456_WATER_SURFACE_RELIEF);
 grad+=safeVolume.xy*(1.55*TR456_WATER_SURFACE_RELIEF);
 grad+=detailTex.xy*(1.45*TR456_WATER_SURFACE_RELIEF);
 grad+=facet.xy*(1.05*TR456_WATER_SURFACE_RELIEF*
   clamp(TR456_WATER_POLYGONAL_NORMAL,0.0,2.0));
 grad+=edgeRip.xy*(.72*TR456_WATER_SURFACE_RELIEF);
 grad+=pixelWave.xy*(3.10*TR456_WATER_SURFACE_RELIEF);
 grad+=authoredRing.xy*(1.85*TR456_WATER_GAME_RIPPLE_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=contactWave.xy*(1.65*TR456_WATER_CONTACT_NORMAL_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=wake.xy*(1.22*TR456_WATER_SURFACE_RELIEF);
 calmMirror*=1.0-sat(wake.z*.42+abs(contactWave.z)*.50+authoredRing.z*.36+edgeRip.z*.44);
 grad*=mix(1.0,.72,calmMirror);
 vec2 bumpSlope=vec2(0.0);
 float bumpEnergy=0.0;
#if TR456_WATER_BUMP_ENABLED
 float bumpAmount=clamp(TR456_WATER_BUMP_STRENGTH,0.0,2.0);
 bumpSlope=(grad*2.05+detailTex.xy*16.0+micro*7.5+
   swell.xy*3.2+safeVolume.xy*3.6+edgeRip.xy*2.7+pixelWave.xy*2.4+
   contactWave.xy*2.1+authoredRing.xy*2.3)*max(TR456_WATER_BUMP_SCALE,.10);
 bumpSlope*=mix(1.0,.56,calmMirror)*bumpAmount;
 n=applyWaterBump(n,bumpSlope,.74*TR456_WATER_SURFACE_RELIEF);
 ndv=sat(dot(n,viewVec));
 bumpEnergy=sat(length(bumpSlope)*2.85);
#endif
 float wave=sat((length(grad)*2.80+wake.z*.54+contactHeight*.48+surfaceMotion*.10+
   authoredRing.z*.36+swell.z*.22+
   safeVolume.z*.44+detailTex.z*.34+facetEnergy*.36+edgeRip.z*.22+
   pixelWave.z*.60+length(micro)*3.0+bumpEnergy*.28)*TR456_WATER_SURFACE_RELIEF);
 vec2 warp=(grad*uParams.y*1.92+wake.xy*.92+contactWave.xy*.58+micro*.22+swell.xy*.82+
   edgeRip.xy*.50+pixelWave.xy*2.05)*(TR456_WATER_SURFACE_WAVE*
   TR456_WATER_SURFACE_RELIEF*TR456_WATER_REFRACT_STRENGTH*
   TR456_WATER_REFRACTION_WAVE_STRENGTH*TR_TOGGLE_SURFACE_WARP);
 warp+=waterBumpLimit(bumpSlope*.0045,.018)*
   TR456_WATER_REFRACT_STRENGTH*TR_TOGGLE_SURFACE_WARP;
 warp*=mix(1.0,.82,calmMirror);
 warp*=seamGuard;

 vec2 seamWarp=warp*(1.02+.16*seamGuard);
 vec2 texUv=stableUv+seamWarp*.58;
 vec4 meshBase=texture(sTex0_wrap,vec3(uv+seamWarp,vLayer));
 vec4 worldBase=texture(sTex0_wrap,vec3(texUv,vLayer));
 vec4 base=mix(meshBase,worldBase,.98);
 base=mix(base,worldBase,seamFade*.85);
 float waterCoverage=clamp(mix(.78,worldBase.a,.12),.60,.95);
 base.a=waterCoverage;
 vec3 r=mix(
   texture(sTex0_wrap,vec3(texUv+seamWarp*(.24+TR456_WATER_CHROMA_STRENGTH*.06)+.0007*TR456_WATER_CHROMA_STRENGTH*seamGuard,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv+seamWarp*(.10+TR456_WATER_CHROMA_STRENGTH*.04)+.0003*TR456_WATER_CHROMA_STRENGTH*seamGuard,vLayer)).rgb,
   .34);
 vec3 bl=mix(
   texture(sTex0_wrap,vec3(texUv-seamWarp*(.20+TR456_WATER_CHROMA_STRENGTH*.04)-.0007*TR456_WATER_CHROMA_STRENGTH*seamGuard,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv-seamWarp*(.09+TR456_WATER_CHROMA_STRENGTH*.03)-.0003*TR456_WATER_CHROMA_STRENGTH*seamGuard,vLayer)).rgb,
   .34);
 vec3 refr=mix(base.rgb,vec3(r.r,base.g,bl.b),.20*TR456_WATER_CHROMA_STRENGTH);
 refr=mix(vec3(luma(refr)),refr,clamp(TR456_WATER_TEXTURE_STRENGTH,.60,1.75));
 float waveShade=clamp(pixelWave.w*.50+.50,0.0,1.0);
 float waveBand=smoothstep(.20,.82,pixelWave.z);
 float depthHint=sat((1.0-vFog)*.72+(1.0-waterCoverage)*.18+wave*.16);
 float materialEdge=sat((1.0-waterCoverage)*.18+authoredRing.z*.52+edgeRip.z*.30);
 float shoreline=sat(smoothstep(.26,.68,materialEdge+(1.0-waterCoverage)*.12)*
   (1.0-smoothstep(.52,.96,depthHint))*TR456_WATER_SHORELINE_STRENGTH*
   TR456_WATER_CONTACT_EDGE);
 float waterlinePattern=sin(dot(tc.xz,vec2(13.7,5.4))+t*2.15)+
   sin(dot(tc.xz,vec2(-7.1,15.8))-t*1.72)*.55+
   (texture(sNoise,vec3(tc.xz*.92+vec2(t*.020,-t*.012),t*.010)).x-.5)*1.15;
 float waterlineRipple=shoreline*smoothstep(.18,1.18,abs(waterlinePattern))*
   TR456_WATER_EDGE_WAVE*(.62+.38*vFog);
 float shoreFoam=shoreFoamMask(tc.xz*.34,shoreline,depthHint,t)*
   TR_TOGGLE_SURFACE_FOAM;
 float wetEdge=shoreline*TR456_WATER_WET_EDGE*(.70+.30*smoothstep(.12,.78,depthHint));
 float shoreRipple=shoreline*
   pow(sat(1.0-abs(fract(dot(tc.xz,vec2(.018,.031))+t*.070)-.5)*2.0),5.6)*
   TR456_WATER_EDGE_WAVE;
 vec3 shoreLap=vec3(0.0);
 if(TR_TOGGLE_SURFACE_FOAM*shoreline>.001) {
   shoreLap=softShoreLapping(tc.xz,gameSurfaceDir(),shoreline,t)*
     TR_TOGGLE_SURFACE_FOAM;
 }
 float shoreLapFoam=shoreLap.z*(.45+.55*shoreline);
 wave=sat(wave+waterlineRipple*.24+shoreFoam*.09+shoreRipple*.22+
   shoreLap.z*.18);
 float volume=sat(smoothstep(.14,.92,depthHint)*TR456_WATER_VOLUME_STRENGTH);
 refr*=1.0+(waveShade-.5)*.32*clamp(TR456_WATER_PIXEL_WAVE_STRENGTH,0.0,3.0);
 refr+=vec3(.040,.100,.106)*waveBand*(.45+.55*vFog);
 refr+=vec3(.026,.070,.076)*(safeVolume.z*.70+swell.z*.42)*(.32+.68*vFog);
 refr+=vec3(.018,.060,.068)*detailTex.z*(.32+.68*vFog);
 float volumeDepth=sat(volume*.80+depthHint*.24+(1.0-vFog)*.10);
 refr=waterVolumeAbsorption(refr,volumeDepth,ndv);

 float invNdv=1.0-ndv;
 float fres=sat((.035+.72*(invNdv*invNdv)*(invNdv*invNdv))*TR456_WATER_FRESNEL_STRENGTH);
 float ridge=smoothstep(.17,.54,wave)*(1.0-smoothstep(.48,.96,wave));
 float silk=fastPow5(sat(1.0-abs((a*.52+b*.31+c*.17)-.53)*2.8));
 float flowLine=1.0-abs(fract(tc.x*.28+tc.z*.19+a*.18+t*.018)-.5)*2.0;
 float flowLine2=flowLine*flowLine;
 float flow=flowLine2*flowLine2*flowLine2*flowLine2*flowLine;
 float filmLine=1.0-abs(fract((a+b*1.22+c+d*.18+t*.030)*2.15)-.5)*2.0;
 float filmLine2=filmLine*filmLine;
 float filmLine4=filmLine2*filmLine2;
 float film=filmLine4*filmLine4*filmLine2;
 float microEnergy=sat(length(micro)*58.0);
 float windRipple=sat(waveBand*.30+microEnergy*.18+film*.16+swell.z*.18+
   edgeRip.z*.16+detailTex.z*.18+shoreline*.20+shoreLap.z*.18);
 float calmMirrorBoost=calmMirror*(1.0-smoothstep(.16,.62,windRipple))*
   (1.0-shoreline*.30);
 float foam=(pow(ridge,1.45)*(0.34+0.46*flow)*.060+wake.z*.026+contactHeight*.014+
   swell.z*.010+
   safeVolume.z*.014+detailTex.z*.018+facetEnergy*.010+edgeRip.z*.014+microEnergy*.004+shoreline*.010+
   waterlineRipple*.028+shoreFoam*.024+shoreRipple*.014+shoreLapFoam*.018+
   authoredRing.z*.020)*TR456_WATER_FOAM_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 float glint=(silk*.012+film*.009+foam*.36+wake.z*.008+contactHeight*.004+
   swell.z*.008+
   safeVolume.z*.008+detailTex.z*.013+facetEnergy*.007+edgeRip.z*.007+microEnergy*.004+shoreline*.004+
   waterlineRipple*.010+shoreFoam*.006+shoreRipple*.005+shoreLapFoam*.006+
   bumpEnergy*.010)*
   TR456_WATER_GLINT_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 float crestGlint=fastPow8(sat(1.0-abs(fract(dot(tc.xz,vec2(.026,.012))+
   wave*.18+t*.078)-.5)*2.0))*smoothstep(.30,.86,wave)*
   (.35+.65*fres)*TR456_WATER_GLINT_STRENGTH*TR_TOGGLE_SURFACE_FOAM;
 glint+=crestGlint*.046;
 float caustics=0.0;
#if TR456_WATER_SURFACE_CAUSTICS_ENABLED
 float causticSurfaceReject=1.0-smoothstep(.26,.86,
   wave+shoreFoam*.34+shoreLap.z*.30+waterlineRipple*.18);
 float bottomCue=sat(depthHint*.82+volumeDepth*.28+(1.0-fres)*.12);
 float causticDepth=smoothstep(.34,.86,bottomCue)*
   (1.0-smoothstep(.18,.72,fres))*
   causticSurfaceReject*(1.0-sat(shoreline*.78));
 if(TR456_WATER_CAUSTICS_STRENGTH*TR_TOGGLE_SURFACE_CAUSTICS*causticDepth>.001) {
   caustics=deepCausticField(tc.xz*.48+seamWarp*.18,t,depthHint,grad+pixelWave.xy);
   caustics=caustics*(.036+.064*causticDepth)*causticDepth*
     TR456_WATER_CAUSTICS_STRENGTH*TR_TOGGLE_SURFACE_CAUSTICS;
 }
#endif

 vec3 shallow=vec3(.020,.185,.205);
 vec3 deep=vec3(.004,.045,.060);
 float depth=sat((1.0-vFog)*.85+wave*.18)*TR456_WATER_DEPTH_STRENGTH;
 vec3 tint=mix(deep,shallow,.62+.28*fres-depth*.28)*TR456_WATER_TINT_STRENGTH;
  vec3 light=clamp((vLight+vColor)*1.24,0.0,1.70);
  refr+=vec3(.16,.32,.27)*caustics*(.50+.50*depthHint);
  vec3 col=mix(refr,tint,(.14+.13*fres)*TR456_WATER_OPACITY)*light;
  col=mix(col,(deep*.92+vec3(.000,.016,.022))*light,sat(volume*.08*TR456_WATER_DEPTH_ABSORPTION));
 col=mix(col,col*vec3(.70,.88,.88)+vec3(.006,.030,.026),sat(shoreline*.26));
 col+=vec3(.34,.56,.60)*glint;
 col+=vec3(.030,.090,.095)*waterlineRipple*(.35+.65*fres);
 col+=vec3(.060,.125,.125)*shoreFoam*(.30+.70*fres)*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.050,.110,.120)*shoreRipple*(.28+.72*fres)*TR456_WATER_FOAM_STRENGTH;
 col+=vec3(.046,.105,.112)*shoreLapFoam*(.22+.42*fres);
 col+=vec3(.050,.105,.105)*wetEdge*(.42+.58*fres);
 col+=vec3(.040,.105,.112)*waveBand*(.28+.72*fres);
 col+=vec3(.065,.150,.160)*sat(swell.z*.50+pixelWave.z*.34+safeVolume.z*.32)*
   (.32+.68*fres);
 col+=vec3(.030,.060,.066)*facetEnergy*(.22+.78*fres);
 col+=vec3(.035,.070,.075)*(flow*.55+film*.45+ridge*.60+detailTex.z*.72)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 float calmPocket=1.0-wave;
 float calmPocket2=calmPocket*calmPocket;
 col+=vec3(.15,.28,.32)*calmPocket2*calmPocket2*calmPocket2*.018;

 vec3 sceneRefl=vec3(0.0);
 float reflMask=0.0;
#if TR456_WATER_AUTHORED_REFLECTION_ENABLED || TR456_WATER_DEBUG_MODE == 5 || TR456_WATER_DEBUG_MODE == 7
vec2 mirrorWarp=vec2(seamWarp.x+micro.x*.46+swell.x*1.05+
   facet.x*.90+edgeRip.x*.72,
   -abs(seamWarp.y+micro.y*.36+swell.y*1.05+
   facet.y*.90+edgeRip.y*.72)*.42)*
   (.62+.86*fres)*TR456_WATER_MIRROR_ROUGHNESS*mix(.84,1.0,seamGuard);
 vec2 bumpMirror=waterBumpLimit(bumpSlope*(.010+.014*fres),.034);
 mirrorWarp+=vec2(bumpMirror.x,-abs(bumpMirror.y))*(.72+.28*fres);
 vec2 mirrorWarp2=mirrorWarp+
   vec2(grad.x+micro.x*.34+swell.x*1.12+facet.x*.72+edgeRip.x*.82,
   -grad.y-micro.y*.34-swell.y*1.12-facet.y*.72-edgeRip.y*.82)*
   (.020+.040*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec3 reflectNormal=normalize(n+vec3(mirrorWarp.x*5.0,0.0,
   -mirrorWarp.y*5.0));
 float reflectRough=TR456_WATER_MIRROR_ROUGHNESS*(.38+.62*fres+
   windRipple*.24);
 vec2 mirrorUv=preciseReflectionUv(screen,reflectNormal,viewVec,
   mirrorWarp,0.0,reflectRough);
 vec2 mirrorUv2=preciseReflectionUv(screen,reflectNormal,viewVec,
   mirrorWarp2,.010+.018*fres,reflectRough*.86);
 vec2 mirrorBase=preciseReflectionUv(screen,n,viewVec,vec2(0.0),0.0,.18);
 vec2 calmMirrorUv=mix(mirrorBase,mirrorUv,mix(.34,.68,windRipple));
 float reflectValid=mix(.58,1.0,min(reflectionUvFade(mirrorUv),
   reflectionUvFade(mirrorUv2)));
 vec3 roughRefl=reflectionGrade(stableCaptureColor(mirrorUv,screen)*.62+
   stableCaptureColor(mirrorUv2,screen)*.38);
 vec3 calmRefl=reflectionGrade(stableCaptureColor(calmMirrorUv,screen)*.82+
   stableCaptureColor(mirrorUv,screen)*.18);
 sceneRefl=mix(roughRefl,calmRefl,calmMirrorBoost*.62);
 sceneRefl=reflectionAutoBalance(sceneRefl,tint,
   sat(windRipple*.45+shoreline*.25+foam*.18));
 float reflOk=mix(.62,1.0,smoothstep(.004,.040,luma(sceneRefl)))*reflectValid;
#endif

 col=mix(col,col*vec3(.58,.80,.88),sat((depth*.36+volume*.24)*TR456_WATER_DEPTH_ABSORPTION));
 col=mix(uFogColor.rgb*waterCoverage,col,vFog);
 float alpha=clamp((waterCoverage*(.68+.10*fres)+foam*.08+waveBand*.030+
   shoreline*.016+wetEdge*.014+waterlineRipple*.024+detailTex.z*.010+
   shoreFoam*.018*TR456_WATER_FOAM_STRENGTH+shoreLapFoam*.014+authoredRing.z*.010+
   volume*.045+depth*.018)*TR456_WATER_OPACITY,.185,.62);
 alpha=max(alpha,reflMask*.20);

#if TR456_WATER_DEBUG_MODE == 1
 fragColor=vec4(vec3(wave),1.0);
#elif TR456_WATER_DEBUG_MODE == 2
 fragColor=vec4(vec3(fres),1.0);
#elif TR456_WATER_DEBUG_MODE == 3
 fragColor=vec4(vec3(foam*6.0),1.0);
#elif TR456_WATER_DEBUG_MODE == 5
 fragColor=vec4(sceneRefl,1.0);
#elif TR456_WATER_DEBUG_MODE == 6
 fragColor=vec4(reflectionGrade(captureColor(screen)),1.0);
#elif TR456_WATER_DEBUG_MODE == 7
 fragColor=vec4(vec3(reflMask),1.0);
#elif TR456_WATER_DEBUG_MODE == 9
 fragColor=vec4(authoredRing.z,shoreline,waterlineRipple,1.0);
#elif TR456_WATER_DEBUG_MODE == 10
 fragColor=vec4(abs(contactWave.z)*2.0,abs(contactWave.x)*12.0,abs(contactWave.y)*12.0,1.0);
#elif TR456_WATER_DEBUG_MODE == 11
 fragColor=vec4(vec3(alpha),1.0);
#elif TR456_WATER_DEBUG_MODE == 12
 fragColor=vec4(base.a,depthHint,materialEdge,1.0);
#elif TR456_WATER_DEBUG_MODE == 13
 fragColor=vec4(abs(meshBase.a-worldBase.a)*4.0,
   length(meshBase.rgb-worldBase.rgb)*2.0,shoreline,1.0);
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
#elif TR456_WATER_DEBUG_MODE == 23
 fragColor=vec4(detailTex.z,wave,glint*5.0,1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(.0,.85,1.0,1.0);
#else
 fragColor=vec4(col,alpha)*uColor;
#endif
}
