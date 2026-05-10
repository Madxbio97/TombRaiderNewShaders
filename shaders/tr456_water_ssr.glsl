#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
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
#define TR456_WATER_REFLECTION_CONTRAST 1.45
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

vec2 playerRipple(vec2 screen, float time){
 vec2 p=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.45,1.0);
 float d=length(p)+.0001;
 float ring=sin(d*72.0-time*6.2)*exp(-d*5.0)*smoothstep(.030,.18,d);
 float wake=(1.0-smoothstep(.02,.28,abs(p.x)))*(1.0-smoothstep(-.10,.42,p.y))*smoothstep(-.32,.08,p.y);
 wake*=sin((p.y+time*.22)*54.0)*exp(-abs(p.x)*11.0);
 return normalize(p)*(ring*.040+wake*.018)*TR456_WATER_RIPPLE_STRENGTH;
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
 vec3 n=normalize(vNormal*.42+heightNormal*.42+(e0*.38+e1*.18+e2*.065)*TR456_WATER_SURFACE_WAVE);
 if(!gl_FrontFacing)n=-n;

 vec2 px=uAmbient[2].xy;
 vec2 screen=gl_FragCoord.xy*px;
 vec2 ripple=playerRipple(screen,time);
 n=normalize(n+vec3(ripple.x*7.0,0.0,-ripple.y*7.0));
 vec2 bend=(n.xy*54.0+ripple*1150.0)*TR456_WATER_REFRACT_STRENGTH;
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

 vec2 ray=normalize(vec2(n.x,-n.y)+vec2(.001))*px*(96.0*TR456_WATER_SSR_STRENGTH);
 vec2 uvReflect=screen+bend*px*(.45+.65*fres)*TR456_WATER_SSR_STRENGTH;
 float fade=screenFade(uvReflect);
 vec3 refl0=reflectColor(uvReflect);
 vec3 refl1=reflectColor(uvReflect+ray*.34);
 vec3 refl2=reflectColor(uvReflect+ray*.78);
 vec3 refl3=reflectColor(uvReflect-ray*.22);
 vec3 envRefl=reflectionGrade(refl0*.42+refl1*.27+refl2*.18+refl3*.13);
 vec2 mirrorOffset=vec2(bend.x,-abs(bend.y)*.35)*px*(.42+.58*fres)*TR456_WATER_SSR_STRENGTH;
 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+mirrorOffset;
 vec2 mirrorUv2=vec2(screen.x+ray.x*.38,1.0-screen.y+ray.y*.16)+mirrorOffset*.55;
 vec3 flipRefl=reflectionGrade(sceneColor(mirrorUv)*.66+sceneColor(mirrorUv2)*.34);
 vec2 noFlipUv=screen+vec2(bend.x,-bend.y)*px*(.18+.32*fres);
 vec3 localRefl=reflectionGrade(sceneColor(noFlipUv));
 float grazing=smoothstep(.18,.72,1.0-ndv);
 vec3 sceneRefl=mix(localRefl,flipRefl,clamp(.48+.42*grazing,0.0,1.0));
 vec3 fboRefl=vec3(0.0);
#if TR456_WATER_FBO_REFLECTION == 1
 vec3 fboFlip=fboColor(mirrorUv)*.56+fboColor(mirrorUv+ray*.45)*.28+fboColor(mirrorUv-ray*.22)*.16;
 vec3 fboFlat=fboColor(noFlipUv);
 fboRefl=reflectionGrade(mix(fboFlat,fboFlip,clamp(.62+.26*grazing,0.0,1.0)));
 float fboOk=smoothstep(.015,.075,luma(fboRefl));
 sceneRefl=mix(sceneRefl,fboRefl,.86*fboOk*TR456_WATER_SCENE_REFLECTION);
#endif
 sceneRefl=mix(localRefl,sceneRefl,fade*TR456_WATER_SCENE_REFLECTION);
 vec3 refl=mix(sceneRefl,envRefl,clamp(authoredBlend*.12,0.0,.24));
 refl=mix(refl,refl*vec3(.82,.94,1.06),.16);

 float spec=pow(max(dot(lightReflect,vv),0.0),78.0)*.85;
 float sparkle=pow(max(e0.x*e1.y+e2.z*.28,0.0),6.0)*.075;
 float crest=smoothstep(.42,.88,e0.z*.52+e1.z*.28+e2.z*.12);
 float foam=smoothstep(.24,.92,notWater)*crest*.085*TR456_WATER_FOAM_STRENGTH;
 float caustic=causticPattern(vTexCoord+e0.xy*.025+flowA*.35,time);
 caustic*=TR456_WATER_CAUSTICS_STRENGTH*(.018+.060*smoothstep(.15,1.0,notWater));
 vec3 tint=vec3(.010,.130,.165)*uAmbient[0].rgb*TR456_WATER_TINT_STRENGTH;
 float depth=sat(notWater*.80+(1.0-ndv)*.35)*TR456_WATER_DEPTH_STRENGTH;
 refr=mix(refr,tint,(.10+.18*depth+.08*fres)*typeWater*TR456_WATER_OPACITY);

 vec3 highlight=uAmbient[1].rgb*(spec*.92+sparkle*TR456_WATER_GLINT_STRENGTH)+vec3(.42,.76,.82)*(foam+caustic);
 vec3 sceneBase=sceneColor(screen);
 vec3 waterBase=mix(sceneBase,refr,clamp(TR456_WATER_OPACITY*.72,0.0,1.0));
 waterBase=mix(waterBase,waterBase*vec3(.62,.84,.93),sat(depth*.42));
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
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(1.0,.0,1.0,1.0);
#else
 fragColor=vec4(col,1.0);
#endif
}
