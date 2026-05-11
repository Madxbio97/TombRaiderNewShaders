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
#define TR456_WATER_VOLUME_STRENGTH 1.0
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
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.75
#define TR456_WATER_MICRO_RIPPLE 1.25
#define TR456_WATER_MICRO_SCALE 1.0
#define TR456_WATER_MIRROR_ROUGHNESS 1.0
#define TR456_WATER_SWELL_STRENGTH 0.85
#define TR456_WATER_SWELL_SCALE 1.0
#define TR456_WATER_WAKE_WAVE 1.0
#define TR456_WATER_EDGE_WAVE 0.75
#define TR456_WATER_EDGE_WIDTH 0.09
#define TR456_WATER_REFLECTION_CONTRAST 1.45
#define TR456_WATER_ROUGH_REFLECTION 0.85
#define TR456_WATER_FRESNEL_STRENGTH 1.10
#define TR456_WATER_BOTTOM_CAUSTICS 0.85
#define TR456_WATER_CONTACT_EDGE 0.72
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_WALL_STRETCH 0.84
#define TR456_WATER_COLOR_SATURATION 1.18
#define TR456_WATER_BRIGHTNESS 0.86
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#define TR456_WATER_FBO_REFLECTION 0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform vec4 uModelMatrix[4];
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

vec3 colorGrade(vec3 c){
 float y=luma(c);
 c=mix(vec3(y),c,TR456_WATER_COLOR_SATURATION);
 return clamp(c*TR456_WATER_BRIGHTNESS,vec3(0.0),vec3(3.0));
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
 vec2 dir=normalize(p);
 vec2 flow=dir*(ringA*.036+ringB*.018+foot*.026+footWave*.034)+vec2(p.x,-p.y)*(wake*.016);
 float crest=sat(abs(ringA)*.80+abs(ringB)*.42+abs(wake)*.55+foot*.95+abs(footWave)*.85);
 return vec3(flow*TR456_WATER_RIPPLE_STRENGTH*TR456_WATER_WAKE_STRENGTH,crest*TR456_WATER_WAKE_STRENGTH);
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
 vec3 wake=playerWake(screen,t);
 vec3 contactWave=vContactWave;
 vec2 micro=microRipple(uv*2.25+vec2(t*.012,-t*.008),t);
 vec3 swell=surfaceSwell(uv*1.10,t);
 vec3 edgeRip=edgeRippleLayer(uv*1.45,screen,t);
 wake.xy+=contactWave.xy*.28;
 wake.z+=abs(contactWave.z)*.42;
 grad+=micro*(.40*TR456_WATER_SURFACE_RELIEF);
 grad+=swell.xy*(.95*TR456_WATER_SURFACE_RELIEF);
 grad+=edgeRip.xy*(.78*TR456_WATER_SURFACE_RELIEF);
 grad+=contactWave.xy*(1.25*TR456_WATER_CONTACT_NORMAL_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=wake.xy*(1.05*TR456_WATER_SURFACE_RELIEF);
 vec3 n=normalize(vec3(-grad.x*6.8*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF,1.0,-grad.y*6.8*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF));
 if(!gl_FrontFacing)n=-n;

 vec3 vv=normalize(-vPos);
 float ndv=sat(dot(n,vv));
 vec3 rr=normalize(reflect(-vv,n));
 vec3 refSharp=cubeApprox(rr);
 vec3 refWide=cubeApprox(normalize(rr+vec3((grad.x+micro.x*.28+swell.x*.86+edgeRip.x*.70)*.48,.08,(grad.y+micro.y*.28+swell.y*.86+edgeRip.y*.70)*.48)));
 vec3 ref=mix(refSharp,refWide,sat(length(grad)*7.4+length(micro)*8.0+swell.z*.18+edgeRip.z*.22));

 float fresBase=.028+.88*pow(1.0-ndv,4.2);
 float F=sat(mix(fresBase,sqrt(fresBase),.16)*TR456_WATER_FRESNEL_STRENGTH);
 float edge=smoothstep(.10,.64,F);
 float topView=1.0-edge;
 float microEnergy=sat(length(micro)*32.0);
 float energy=abs(grad.x)+abs(grad.y)+wake.z*.065+swell.z*.026+edgeRip.z*.030+microEnergy*.018;
 float ridge=smoothstep(.026,.090,energy)*(1.0-smoothstep(.092,.20,energy));
 ridge=max(ridge,max(max(wake.z*.30,swell.z*.10),edgeRip.z*.14));
 float streak=pow(1.0-abs(fract((screen.x+h0*.18+h1*.10+t*.055)*4.0)-.5)*2.0,9.0);
 streak*=smoothstep(.38,.90,h1)*(.08+.68*edge)*clamp(TR456_WATER_TEXTURE_STRENGTH,.75,1.65);
 float glint=pow(max(h1*.70+h2*.25-.60,0.0),7.0)*.052+wake.z*.020+swell.z*.008+edgeRip.z*.008+microEnergy*.004;

 vec3 horizon=vec3(.012,.046,.058);
 vec3 cold=vec3(.22,.34,.38);
 vec3 col=mix(horizon,ref*(1.85+edge*.66)+cold*streak*.34,.62)*F*TR456_WATER_REFLECT_STRENGTH;
 col+=cold*(ridge*.026+glint)*TR456_WATER_GLINT_STRENGTH;
 col+=cold*(streak*.030+ridge*.028)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 float alpha=(.052+.135*edge+.008*ridge)*TR456_WATER_REFLECT_STRENGTH*TR456_WATER_OPACITY;

 float waveEnergy=sat(length(grad)*7.0+wake.z*.42+swell.z*.20+edgeRip.z*.24+microEnergy*.10);
 float rough=clamp((.30+waveEnergy*.62+topView*.18-edge*.08)*TR456_WATER_ROUGH_REFLECTION*TR456_WATER_MIRROR_ROUGHNESS,.10,.94);
 vec2 rippleOffset=vec2(grad.x,-grad.y)*(.026+.068*rough+.044*edge)+vec2(wake.x,-wake.y)*(.30+.40*edge)+vec2(swell.x,-swell.y)*(.58+.34*rough)+vec2(edgeRip.x,-edgeRip.y)*(.58+.36*rough)+vec2(micro.x,-micro.y)*(.20+.20*rough);
 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+vec2(grad.x+micro.x*.34+swell.x*.82+edgeRip.x*.62,-abs(grad.y+micro.y*.24+swell.y*.82+edgeRip.y*.62))*(.048+.094*edge+.052*rough)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv2=mirrorUv+rippleOffset*(.72+rough*.86)+vec2(rr.x,-rr.z)*(.014+.036*edge);
 vec2 mirrorUv3=mirrorUv-rippleOffset*(.58+rough*.58)+vec2(-rr.z,-rr.x)*(.010+.024*rough);
 vec2 mirrorUv4=mirrorUv+vec2(grad.y+micro.y*.28+swell.y*.82+edgeRip.y*.62,grad.x+micro.x*.28+swell.x*.82+edgeRip.x*.62)*(.020+.042*rough);
 vec2 wallStretch=vec2(0.0,(-.045-.090*edge)*TR456_WATER_WALL_STRETCH);
 vec3 refrScene=reflectionGrade(captureColor(screen+rippleOffset*.32));
 float causticA=causticLine(screen*vec2(1.15,.82)+grad*.16,t);
 float causticB=causticLine(screen.yx*vec2(.74,1.36)-grad*.10+vec2(.17,.09),t*.83);
 float bottomMask=smoothstep(.12,.76,topView)*(1.0-smoothstep(.84,1.0,topView)*.25);
 float bottomLight=(causticA*.65+causticB*.35)*TR456_WATER_BOTTOM_CAUSTICS*
   TR456_WATER_CAUSTICS_STRENGTH*bottomMask;
 refrScene+=vec3(.28,.36,.32)*bottomLight*.10;
#if TR456_WATER_REFLECTION_QUALITY <= 0
 vec3 mirrorSharp=reflectionGrade(captureColor(mirrorUv)*.70+captureColor(mirrorUv2)*.30);
 vec3 mirrorSoft=mirrorSharp;
#elif TR456_WATER_REFLECTION_QUALITY == 1
 vec3 mirrorTall=reflectionGrade(captureColor(mirrorUv+wallStretch)*.62+
   captureColor(mirrorUv+wallStretch*1.55)*.38);
 vec3 mirrorSharp=reflectionGrade(captureColor(mirrorUv)*.58+captureColor(mirrorUv2)*.30+
   captureColor(mirrorUv3)*.12)*.88+mirrorTall*.12;
 vec3 mirrorSoft=reflectionGrade(captureColor(mirrorUv2)*.46+captureColor(mirrorUv3)*.34+
   captureColor(mirrorUv4)*.20);
#else
 vec3 mirrorTall=reflectionGrade(captureColor(mirrorUv+wallStretch)*.50+
   captureColor(mirrorUv+wallStretch*1.85)*.30+captureColor(mirrorUv+wallStretch*.35)*.20);
 vec3 mirrorSharp=reflectionGrade(captureColor(mirrorUv)*.47+captureColor(mirrorUv2)*.26+
   captureColor(mirrorUv3)*.12)*.85+mirrorTall*.15;
 vec3 mirrorSoft=reflectionGrade(captureColor(mirrorUv2)*.34+captureColor(mirrorUv3)*.28+
   captureColor(mirrorUv4)*.20+captureColor(mirrorUv-rippleOffset*.95)*.12+
   captureColor(mirrorUv+wallStretch*1.25)*.06);
#endif
 vec3 mirrorScene=mix(mirrorSharp,mirrorSoft,rough*.62);
 vec3 sceneRefl=mix(refrScene,mirrorScene,clamp(.40+edge*.46+F*.36,0.0,.98));
 float reflOk=mix(.68,1.0,smoothstep(.002,.030,luma(sceneRefl)));
 float sceneMask=clamp((.42+edge*.44+F*.40)*TR456_WATER_REFLECT_STRENGTH*
   max(TR456_WATER_FORCE_REFLECTION,.56),0.0,.97)*reflOk;
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
 col+=cold*(ridge*.028+streak*.018+glint*.42+edgeRip.z*.006)*TR456_WATER_GLINT_STRENGTH;
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
 fragColor=vec4(col,clamp(alpha,.16,.54));
#endif
}
