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
 float wave=sat(length(grad)*2.65);
 vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;
 vec2 rp=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.45,1.0);
 float rd=length(rp)+.0001;
 float ring=sin(rd*64.0-t*5.8)*exp(-rd*5.2)*smoothstep(.035,.16,rd);
 vec2 ripple=normalize(rp)*ring*.030*TR456_WATER_RIPPLE_STRENGTH;
 vec2 warp=(grad*uParams.y*1.70+ripple)*(TR456_WATER_SURFACE_WAVE*TR456_WATER_REFRACT_STRENGTH);

 vec4 base=texture(sTex0_wrap,vec3(uv+warp,vLayer));
 vec3 r=texture(sTex0_wrap,vec3(uv+warp*(1.16+TR456_WATER_CHROMA_STRENGTH*.10)+.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb;
 vec3 bl=texture(sTex0_wrap,vec3(uv+warp*(.86-TR456_WATER_CHROMA_STRENGTH*.06)-.0011*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb;
 vec3 refr=mix(base.rgb,vec3(r.r,base.g,bl.b),.20*TR456_WATER_CHROMA_STRENGTH);

 float fres=.035+.72*pow(1.0-sat(dot(n,viewVec)),4.0);
 float ridge=smoothstep(.17,.54,wave)*(1.0-smoothstep(.48,.96,wave));
 float silk=pow(sat(1.0-abs((a*.52+b*.31+c*.17)-.53)*2.8),5.0);
 float flow=pow(1.0-abs(fract(tc.x*.28+tc.z*.19+a*.18+t*.030)-.5)*2.0,9.0);
 float film=pow(1.0-abs(fract((a+b*1.22+c+d*.18+t*.050)*2.15)-.5)*2.0,10.0);
 float foam=pow(ridge,1.45)*(0.40+0.55*flow)*.075*TR456_WATER_FOAM_STRENGTH;
 float glint=(silk*.014+film*.012+foam*.55)*TR456_WATER_GLINT_STRENGTH;
 float caustics=(causticLine(tc.xz,t,.42,.035)+causticLine(tc.zx+grad*.20,t,.67,-.026)*.55);
 caustics*=.060*TR456_WATER_CAUSTICS_STRENGTH*(.35+.65*vFog);

 vec3 shallow=vec3(.020,.185,.205);
 vec3 deep=vec3(.004,.045,.060);
 float depth=sat((1.0-vFog)*.85+wave*.18)*TR456_WATER_DEPTH_STRENGTH;
 vec3 tint=mix(deep,shallow,.62+.28*fres-depth*.28)*TR456_WATER_TINT_STRENGTH;
 vec3 light=clamp((vLight+vColor)*1.24,0.0,1.70);
 vec3 col=mix(refr,tint,(.14+.13*fres)*TR456_WATER_OPACITY)*light;
 col+=vec3(.42,.72,.78)*(glint+caustics);
 col+=vec3(.15,.28,.32)*pow(1.0-wave,6.0)*.018;

 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+vec2(warp.x,-abs(warp.y)*.32)*(.45+.65*fres);
 vec2 mirrorUv2=mirrorUv+vec2(grad.x,-grad.y)*(.012+.018*fres);
 vec3 sceneRefl=reflectionGrade(captureColor(mirrorUv)*.68+captureColor(mirrorUv2)*.32);
 float reflOk=smoothstep(.004,.040,luma(sceneRefl));
 float reflMask=clamp((.12+fres*.78+ridge*.10)*TR456_WATER_REFLECT_STRENGTH*
   max(TR456_WATER_FORCE_REFLECTION,.25),0.0,.82)*reflOk;
 col=mix(col,sceneRefl,reflMask*(.34+.22*vFog));

 col=mix(col,col*vec3(.62,.83,.90),sat(depth*.55));
 col=mix(uFogColor.rgb*base.a,col,vFog);
 float alpha=clamp((base.a*(.58+.10*fres)+foam*.22)*TR456_WATER_OPACITY,.05,.38);
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
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(.0,.85,1.0,1.0);
#else
 fragColor=vec4(col,alpha)*uColor;
#endif
}
