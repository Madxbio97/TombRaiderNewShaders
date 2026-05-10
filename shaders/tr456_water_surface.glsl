#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.0
#define TR456_WATER_REFRACT_STRENGTH 1.0
#define TR456_WATER_REFLECT_STRENGTH 1.0
#define TR456_WATER_SSR_STRENGTH 1.0
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_TINT_STRENGTH 1.0
#define TR456_WATER_OPACITY 0.62
#define TR456_WATER_FORCE_REFLECTION 0.65
#define TR456_WATER_SCENE_REFLECTION 0.78
#define TR456_WATER_CAUSTICS_STRENGTH 1.10
#define TR456_WATER_DEPTH_STRENGTH 1.0
#define TR456_WATER_RIPPLE_STRENGTH 0.65
#define TR456_WATER_RIPPLE_CENTER_X 0.50
#define TR456_WATER_RIPPLE_CENTER_Y 0.62
#define TR456_WATER_SURFACE_RELIEF 1.0
#define TR456_WATER_WAKE_STRENGTH 1.0
#define TR456_WATER_WAKE_WIDTH 0.42
#define TR456_WATER_WAKE_LENGTH 0.58
#define TR456_WATER_MICRO_RIPPLE 1.25
#define TR456_WATER_MICRO_SCALE 1.0
#define TR456_WATER_MIRROR_ROUGHNESS 1.0
#define TR456_WATER_SWELL_STRENGTH 0.85
#define TR456_WATER_SWELL_SCALE 1.0
#define TR456_WATER_WAKE_WAVE 1.0
#define TR456_WATER_EDGE_WAVE 0.75
#define TR456_WATER_EDGE_WIDTH 0.09
#define TR456_WATER_REFLECTION_CONTRAST 1.45
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#define TR456_WATER_FBO_REFLECTION 0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uColor;
in vec2 vTexCoord;
in vec3 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
in vec3 vWorldPos;
out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(.3333)); }

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec3 captureColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.84,.95,1.04),.16);
 return clamp(c,vec3(0.0),vec3(3.0));
}

float causticLine(vec2 p, float t, float scale, float speed){
 float a=fract(p.x*scale+p.y*(scale*.37)+t*speed);
 float b=fract(p.x*(-scale*.42)+p.y*(scale*.71)-t*speed*.73);
 float l=1.0-abs(a-.5)*2.0;
 float m=1.0-abs(b-.5)*2.0;
 return pow(sat(max(l,m)),13.0);
}

vec2 microRipple(vec2 p, float time){
 p*=max(TR456_WATER_MICRO_SCALE,.10);
 vec2 a=vec2(sin(p.x*37.0+p.y*16.0+time*2.10),
             cos(p.x*19.0-p.y*31.0-time*1.70));
 vec2 b=vec2(sin((p.x+p.y)*58.0+time*3.00),
             cos((p.x-p.y)*49.0-time*2.35));
 vec2 c=vec2(sin(p.x*91.0+p.y*13.0-time*1.35),
             sin(p.y*83.0-p.x*21.0+time*1.90));
 return (a*.010+b*.006+c*.0035)*TR456_WATER_MICRO_RIPPLE;
}

vec3 surfaceSwell(vec2 p, float time){
 p*=max(TR456_WATER_SWELL_SCALE,.10);
 float phaseA=dot(p,vec2(1.30,.42))*6.28318+time*.50;
 float phaseB=dot(p,vec2(-.55,1.12))*6.28318-time*.37;
 vec2 center=p-vec2(.35,-.25);
 float phaseC=length(center)*7.4-time*.60;
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
 float a=sin(dot(p,vec2(1.55,.38))*18.0-time*3.7);
 float b=sin(dot(p,vec2(-.42,1.18))*24.0+time*2.9);
 float c=sin((p.x+p.y)*37.0-time*5.1);
 vec2 grad=vec2(cos(dot(p,vec2(1.55,.38))*18.0-time*3.7)*1.55+
                cos((p.x+p.y)*37.0-time*5.1)*.45,
                cos(dot(p,vec2(-.42,1.18))*24.0+time*2.9)*1.18+
                cos((p.x+p.y)*37.0-time*5.1)*.45);
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
 vec2 dir=normalize(p);
 vec2 flow=dir*(ringA*.036+ringB*.018+foot*.026+footWave*.034)+vec2(p.x,-p.y)*(wake*.016);
 float crest=sat(abs(ringA)*.80+abs(ringB)*.42+abs(wake)*.55+foot*.95+abs(footWave)*.85);
 return vec3(flow*TR456_WATER_RIPPLE_STRENGTH*TR456_WATER_WAKE_STRENGTH,crest*TR456_WATER_WAKE_STRENGTH);
}

void main(){
 vec2 uv=vTexCoord;
 vec3 n=normalize(vNormal);
 vec3 viewVec=normalize(-vPos);
 float t=uModelMatrix[3].x*uParams.z;
 vec3 tc=vWorldPos.xyz*(uParams.x/1024.0);

 float a=texture(sNoise,tc*.58+n*t*.25).x;
 float b=texture(sNoise,tc*1.07-n*t*.18+vec3(.17,.07,.31)).x;
 float c=texture(sNoise,tc*1.91+n*t*.12+vec3(.41,.13,0.0)).x;
 float d=texture(sNoise,tc*2.80+vec3(-t*.085,t*.045,t*.02)).x;
 vec2 grad=vec2(a-b,c-a)+vec2(d-c,b-d)*.24;
  vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;
 vec3 wake=playerWake(screen,t);
 vec2 stableUv=tc.xz*.72+vec2(t*.020,-t*.014);
 vec2 micro=microRipple(stableUv+screen*.55,t);
 vec3 swell=surfaceSwell(tc.xz*.32+screen*.14,t);
 vec3 edgeRip=edgeRippleLayer(tc.xz*.34+screen*.42,screen,t);
 grad+=micro*(.55*TR456_WATER_SURFACE_RELIEF);
 grad+=swell.xy*(1.10*TR456_WATER_SURFACE_RELIEF);
 grad+=edgeRip.xy*(1.05*TR456_WATER_SURFACE_RELIEF);
 grad+=wake.xy*(1.05*TR456_WATER_SURFACE_RELIEF);
 float wave=sat((length(grad)*2.55+wake.z*.42+swell.z*.22+edgeRip.z*.28+length(micro)*4.2)*TR456_WATER_SURFACE_RELIEF);
 vec2 warp=(grad*uParams.y*1.35+wake.xy*.62+micro*.22+swell.xy*.58+edgeRip.xy*.54)*(TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF*TR456_WATER_REFRACT_STRENGTH);

 vec2 texUv=stableUv+warp*.36;
 vec4 meshBase=texture(sTex0_wrap,vec3(uv+warp,vLayer));
 vec4 worldBase=texture(sTex0_wrap,vec3(texUv,vLayer));
 vec4 base=mix(meshBase,worldBase,.22);
 vec3 r=mix(
   texture(sTex0_wrap,vec3(uv+warp*(1.16+TR456_WATER_CHROMA_STRENGTH*.10)+.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv+warp*(.16+TR456_WATER_CHROMA_STRENGTH*.08)+.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .18);
 vec3 bl=mix(
   texture(sTex0_wrap,vec3(uv+warp*(.86-TR456_WATER_CHROMA_STRENGTH*.06)-.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv-warp*(.14+TR456_WATER_CHROMA_STRENGTH*.05)-.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .18);
 vec3 refr=mix(base.rgb,vec3(r.r,base.g,bl.b),.20*TR456_WATER_CHROMA_STRENGTH);
 refr=mix(vec3(luma(refr)),refr,clamp(TR456_WATER_TEXTURE_STRENGTH,.60,1.75));

 float fres=.035+.72*pow(1.0-sat(dot(n,viewVec)),4.0);
 float ridge=smoothstep(.17,.54,wave)*(1.0-smoothstep(.48,.96,wave));
 float silk=pow(sat(1.0-abs((a*.52+b*.31+c*.17)-.53)*2.8),5.0);
 float flow=pow(1.0-abs(fract(tc.x*.28+tc.z*.19+a*.18+t*.030)-.5)*2.0,9.0);
 float film=pow(1.0-abs(fract((a+b*1.22+c+d*.18+t*.050)*2.15)-.5)*2.0,10.0);
 float microEnergy=sat(length(micro)*58.0);
 float foam=(pow(ridge,1.45)*(0.40+0.55*flow)*.075+wake.z*.044+swell.z*.012+edgeRip.z*.030+microEnergy*.006)*TR456_WATER_FOAM_STRENGTH;
 float glint=(silk*.014+film*.012+foam*.55+wake.z*.015+swell.z*.010+edgeRip.z*.016+microEnergy*.007)*TR456_WATER_GLINT_STRENGTH;
 float caustics=(causticLine(tc.xz,t,.42,.035)+causticLine(tc.zx+grad*.20,t,.67,-.026)*.55);
 caustics=(caustics*.060*(.35+.65*vFog)+wake.z*.018+edgeRip.z*.010)*TR456_WATER_CAUSTICS_STRENGTH;

 vec3 shallow=vec3(.020,.185,.205);
 vec3 deep=vec3(.004,.045,.060);
 float depth=sat((1.0-vFog)*.85+wave*.18)*TR456_WATER_DEPTH_STRENGTH;
 vec3 tint=mix(deep,shallow,.62+.28*fres-depth*.28)*TR456_WATER_TINT_STRENGTH;
 vec3 light=clamp((vLight+vColor)*1.24,0.0,1.70);
 vec3 col=mix(refr,tint,(.14+.13*fres)*TR456_WATER_OPACITY)*light;
 col+=vec3(.42,.72,.78)*(glint+caustics);
 col+=vec3(.035,.070,.075)*(flow*.55+film*.45+ridge*.60)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 col+=vec3(.15,.28,.32)*pow(1.0-wave,6.0)*.018;

 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+vec2(warp.x+micro.x*.55+swell.x*.85+edgeRip.x*.85,-abs(warp.y+micro.y*.45+swell.y*.85+edgeRip.y*.85)*.38)*(.55+.85*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv2=mirrorUv+vec2(grad.x+micro.x*.35+swell.x+edgeRip.x,-grad.y-micro.y*.35-swell.y-edgeRip.y)*(.018+.030*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec3 sceneRefl=reflectionGrade(captureColor(mirrorUv)*.68+captureColor(mirrorUv2)*.32);
 float reflOk=smoothstep(.004,.040,luma(sceneRefl));
 float reflMask=clamp((.12+fres*.78+ridge*.10)*TR456_WATER_REFLECT_STRENGTH*
   max(TR456_WATER_FORCE_REFLECTION,.25),0.0,.82)*reflOk;
 col=mix(col,sceneRefl,reflMask*(.34+.22*vFog));

 col=mix(col,col*vec3(.62,.83,.90),sat(depth*.55));
 col=mix(uFogColor.rgb*base.a,col,vFog);
 float alpha=clamp((base.a*(.58+.10*fres)+foam*.22)*TR456_WATER_OPACITY,.05,.38);
 alpha=max(alpha,reflMask*.20+(wake.z*.10+edgeRip.z*.055)*TR456_WATER_OPACITY);

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
 fragColor=vec4(wake.z,swell.z,edgeRip.z,1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(.0,.85,1.0,1.0);
#else
 fragColor=vec4(col,alpha)*uColor;
#endif
}
