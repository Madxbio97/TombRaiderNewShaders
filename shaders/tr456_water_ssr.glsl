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
uniform sampler2DArray sTex0;
uniform sampler2DArray sTex1;
uniform sampler2DArray sEnvMap;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uAmbient[6];
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vCamera;
out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }

float luma(vec3 c){ return dot(c,vec3(.3333)); }

float calcFresnel(float ndv, float f0){
 return f0+(1.0-f0)*pow(1.0-sat(ndv),5.0);
}

float causticPattern(vec2 uv, float time){
 float a=1.0-abs(fract(uv.x*8.0+uv.y*2.7+time*.18)-.5)*2.0;
 float b=1.0-abs(fract(uv.x*-3.6+uv.y*7.1-time*.13)-.5)*2.0;
 float c=1.0-abs(fract((uv.x+uv.y)*5.2+time*.09)-.5)*2.0;
 return pow(sat(max(max(a,b),c)),14.0);
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

float screenFade(vec2 uv){
 vec2 a=smoothstep(vec2(.015),vec2(.080),uv);
 vec2 b=smoothstep(vec2(.015),vec2(.080),vec2(1.0)-uv);
 return a.x*a.y*b.x*b.y;
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

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.84,.95,1.04),.16);
 return clamp(c,vec3(0.0),vec3(3.0));
}

void main(){
 float time=uModelMatrix[3].x;
 vec2 flowA=vec2(time*.030,-time*.021);
 vec2 flowB=vec2(-time*.018,time*.026);
 vec2 flowC=vec2(time*.007,time*.011);
 vec3 e0=texture(sTex1,vec3(vTexCoord+flowA,0)).xyz*2.0-1.0;
 vec3 e1=texture(sTex1,vec3(vTexCoord*1.73+flowB,0)).xyz*2.0-1.0;
 vec3 e2=texture(sTex1,vec3(vTexCoord*3.10+flowC,0)).xyz*2.0-1.0;
 float h=luma(e0);
 float hx=luma(texture(sTex1,vec3(vTexCoord+flowA+vec2(.004,0.0),0)).xyz*2.0-1.0)-h;
 float hy=luma(texture(sTex1,vec3(vTexCoord+flowA+vec2(0.0,.004),0)).xyz*2.0-1.0)-h;
 vec3 heightNormal=normalize(vec3(-hx*24.0,1.0,-hy*24.0));
 vec3 n=normalize(vNormal*.42+heightNormal*.42+(e0*.38+e1*.18+e2*.065)*TR456_WATER_SURFACE_WAVE*TR456_WATER_SURFACE_RELIEF);
 if(!gl_FrontFacing)n=-n;

 vec2 px=uAmbient[2].xy;
 vec2 screen=gl_FragCoord.xy*px;
 vec3 wake=playerWake(screen,time);
 vec2 ripple=wake.xy;
 vec2 micro=microRipple(screen*2.2+vec2(time*.020,-time*.014),time);
 vec3 swell=surfaceSwell(screen*.80+vec2(time*.012,time*.009),time);
 vec3 edgeRip=edgeRippleLayer(screen*1.8+vTexCoord*.10,screen,time);
 float maskC=texture(sTex0,vec3(clamp(screen,vec2(0.0),vec2(1.0)),0)).w;
 float maskL=texture(sTex0,vec3(clamp(screen-vec2(px.x*2.0,0.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskR=texture(sTex0,vec3(clamp(screen+vec2(px.x*2.0,0.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskD=texture(sTex0,vec3(clamp(screen-vec2(0.0,px.y*2.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskU=texture(sTex0,vec3(clamp(screen+vec2(0.0,px.y*2.0),vec2(0.0),vec2(1.0)),0)).w;
 float maskEdge=smoothstep(.035,.20,abs(maskR-maskC)+abs(maskL-maskC)+abs(maskU-maskC)+abs(maskD-maskC));
 vec2 maskDir=normalize(vec2(maskR-maskL,maskU-maskD)+vec2(.0001));
 edgeRip.xy+=maskDir*maskEdge*.006*TR456_WATER_EDGE_WAVE;
 edgeRip.z=max(edgeRip.z,maskEdge*.42*TR456_WATER_EDGE_WAVE);
 float microEnergy=sat(length(micro)*42.0);
 n=normalize(n+vec3((ripple.x*.78+micro.x*.70+swell.x*1.35+edgeRip.x*1.28)*6.2,0.0,(-ripple.y*.78-micro.y*.70-swell.y*1.35-edgeRip.y*1.28)*6.2)*TR456_WATER_SURFACE_RELIEF);
 vec2 bend=(n.xy*46.0*TR456_WATER_SURFACE_RELIEF+(ripple*.72+micro*.58+swell.xy*1.12+edgeRip.xy*.92)*620.0)*TR456_WATER_REFRACT_STRENGTH;
 bend.y=-bend.y;
 vec2 uvRefract=screen-bend*px;
 float notWater=1.0-texture(sTex0,vec3(clamp(uvRefract,vec2(0.0),vec2(1.0)),0)).w;
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
 fres=mix(.08+sqrt(fres)*.16,sqrt(fres),hasRefl)*typeWater;

 vec2 ray=normalize(vec2(n.x+micro.x*.85+swell.x*1.35+edgeRip.x*1.25,-n.y-micro.y*.85-swell.y*1.35-edgeRip.y*1.25)+vec2(.001))*px*(100.0*TR456_WATER_SSR_STRENGTH*TR456_WATER_MIRROR_ROUGHNESS);
 vec2 uvReflect=screen+bend*px*(.55+.78*fres)*TR456_WATER_SSR_STRENGTH*TR456_WATER_MIRROR_ROUGHNESS;
 float fade=screenFade(uvReflect);
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
 vec2 mirrorOffset=vec2(bend.x+micro.x*105.0+swell.x*180.0+edgeRip.x*150.0,-abs(bend.y+micro.y*92.0+swell.y*165.0+edgeRip.y*150.0)*.42)*px*(.48+.70*fres)*TR456_WATER_SSR_STRENGTH*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+mirrorOffset;
 vec2 mirrorUv2=vec2(screen.x+ray.x*.38,1.0-screen.y+ray.y*.16)+mirrorOffset*.55;
#if TR456_WATER_REFLECTION_QUALITY <= 0
 vec3 flipRefl=reflectionGrade(sceneColor(mirrorUv));
#else
 vec3 flipRefl=reflectionGrade(sceneColor(mirrorUv)*.66+sceneColor(mirrorUv2)*.34);
#endif
 vec2 noFlipUv=screen+vec2(bend.x+micro.x*78.0+swell.x*130.0+edgeRip.x*110.0,-bend.y-micro.y*78.0-swell.y*130.0-edgeRip.y*110.0)*px*(.22+.40*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec3 localRefl=reflectionGrade(sceneColor(noFlipUv));
 float grazing=smoothstep(.18,.72,1.0-ndv);
 vec3 sceneRefl=mix(localRefl,flipRefl,clamp(.48+.42*grazing,0.0,1.0));
 vec3 fboRefl=vec3(0.0);
#if TR456_WATER_FBO_REFLECTION == 1
#if TR456_WATER_REFLECTION_QUALITY <= 0
 fboRefl=reflectionGrade(fboColor(noFlipUv));
#elif TR456_WATER_REFLECTION_QUALITY == 1
 vec3 fboFlip=fboColor(mirrorUv)*.68+fboColor(mirrorUv+ray*.38)*.32;
 vec3 fboFlat=fboColor(noFlipUv);
 fboRefl=reflectionGrade(mix(fboFlat,fboFlip,clamp(.62+.26*grazing,0.0,1.0)));
#else
 vec3 fboFlip=fboColor(mirrorUv)*.56+fboColor(mirrorUv+ray*.45)*.28+fboColor(mirrorUv-ray*.22)*.16;
 vec3 fboFlat=fboColor(noFlipUv);
 fboRefl=reflectionGrade(mix(fboFlat,fboFlip,clamp(.62+.26*grazing,0.0,1.0)));
#endif
 float fboOk=smoothstep(.015,.075,luma(fboRefl));
 sceneRefl=mix(sceneRefl,fboRefl,.86*fboOk*TR456_WATER_SCENE_REFLECTION);
#endif
 sceneRefl=mix(localRefl,sceneRefl,fade*TR456_WATER_SCENE_REFLECTION);
 vec3 refl=mix(sceneRefl,envRefl,clamp(authoredBlend*.12,0.0,.24));
 refl=mix(refl,refl*vec3(.82,.94,1.06),.16);

 float spec=pow(max(dot(lightReflect,vv),0.0),78.0)*.85;
 float sparkle=pow(max(e0.x*e1.y+e2.z*.28,0.0),6.0)*.075+wake.z*.026+swell.z*.014+edgeRip.z*.016+microEnergy*.008;
 float crest=max(smoothstep(.42,.88,e0.z*.52+e1.z*.28+e2.z*.12),max(max(max(wake.z*.56,swell.z*.24),edgeRip.z*.34),microEnergy*.12));
 float foam=smoothstep(.24,.92,notWater)*(crest*.085+wake.z*.036+swell.z*.014+edgeRip.z*.026+microEnergy*.006)*TR456_WATER_FOAM_STRENGTH;
 float caustic=causticPattern(screen+e0.xy*.025+flowA*.35,time);
 caustic=(caustic*(.018+.060*smoothstep(.15,1.0,notWater))+wake.z*.014+swell.z*.006+edgeRip.z*.006+microEnergy*.003)*TR456_WATER_CAUSTICS_STRENGTH;
 vec3 tint=vec3(.010,.130,.165)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 float depth=sat(notWater*.80+(1.0-ndv)*.35)*TR456_WATER_DEPTH_STRENGTH;
 refr=mix(refr,tint,(.10+.18*depth+.08*fres)*typeWater*TR456_WATER_OPACITY);

 vec3 highlight=uAmbient[1].rgb*(spec*.92+sparkle*TR456_WATER_GLINT_STRENGTH)+vec3(.42,.76,.82)*(foam+caustic);
 vec3 sceneBase=sceneColor(screen);
 vec3 waterBase=mix(sceneBase,refr,clamp(TR456_WATER_OPACITY*.78,0.0,1.0));
 waterBase=mix(waterBase,waterBase*vec3(.62,.84,.93),sat(depth*.42));
 waterBase+=vec3(.020,.048,.054)*(crest+sparkle*.65)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 float reflectAmount=clamp((.24+fres*1.75)*reflStrength*hasRefl,0.0,.96);
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
