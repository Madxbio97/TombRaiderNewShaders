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
#define TR456_WATER_ROUGH_REFLECTION 0.85
#define TR456_WATER_FRESNEL_STRENGTH 1.10
#define TR456_WATER_BOTTOM_CAUSTICS 0.85
#define TR456_WATER_CONTACT_EDGE 0.72
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_WALL_STRETCH 0.84
#define TR456_WATER_COLOR_SATURATION 1.18
#define TR456_WATER_BRIGHTNESS 0.86
#define TR456_WATER_FBO_REFLECTION 0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform vec4 uModelMatrix[4];
in vec3 vPos;
in vec2 vTexCoord;
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
 return pow(sat(max(max(a,b),max(c*.72,d*.58))),13.5);
}

vec3 cubeApprox(vec3 r){
 vec3 q=r*r;
 vec3 p=step(vec3(0.0),r);
 return q.x*mix(vrgb1,vrgb0,p.x)+q.y*mix(vrgb3,vrgb2,p.y)+q.z*mix(vrgb5,vrgb4,p.z);
}

void main(){
 vec2 uv=vTexCoord.xy;
 vec2 d=vec2(1.0/192.0,0.0);
 float t=uModelMatrix[3].x*.096;
 float h0=texture(sNoise,vec3(uv*.50,t)).x;
 float h1=texture(sNoise,vec3(uv*1.13+vec2(.09,.21),t*.63)).x;
 float h2=texture(sNoise,vec3(uv*2.05+vec2(.31,.04),t*.38)).x;
 float hx=texture(sNoise,vec3(uv*.50+d.xy,t)).x-h0;
 float hy=texture(sNoise,vec3(uv*.50+d.yx,t)).x-h0;
 vec2 grad=vec2(hx,hy)+vec2(h1-h0,h2-h1)*.18;
 vec3 n=normalize(vec3(-grad.x*6.8*TR456_WATER_SURFACE_WAVE,1.0,-grad.y*6.8*TR456_WATER_SURFACE_WAVE));
 if(!gl_FrontFacing)n=-n;

 vec3 vv=normalize(-vPos);
 float ndv=sat(dot(n,vv));
 vec3 rr=normalize(reflect(-vv,n));
 vec3 refSharp=cubeApprox(rr);
 vec3 refWide=cubeApprox(normalize(rr+vec3(grad.x*.45,.08,grad.y*.45)));
 vec3 ref=mix(refSharp,refWide,sat(length(grad)*8.0));

 float fresBase=.028+.88*pow(1.0-ndv,4.2);
 float F=sat(mix(fresBase,sqrt(fresBase),.16)*TR456_WATER_FRESNEL_STRENGTH);
 float edge=smoothstep(.10,.64,F);
 float topView=1.0-edge;
 float energy=abs(grad.x)+abs(grad.y);
 float ridge=smoothstep(.020,.074,energy)*(1.0-smoothstep(.080,.18,energy));
 float streak=pow(1.0-abs(fract((uv.x+h0*.18+h1*.10+t*.055)*4.0)-.5)*2.0,9.0);
 streak*=smoothstep(.38,.90,h1)*(.08+.68*edge);
 float glint=pow(max(h1*.70+h2*.25-.60,0.0),7.0)*.075;

 vec3 horizon=vec3(.012,.046,.058);
 vec3 cold=vec3(.22,.34,.38);
 vec3 col=mix(horizon,ref*(1.85+edge*.66)+cold*streak*.34,.62)*F*TR456_WATER_REFLECT_STRENGTH;
 col+=cold*(ridge*.040+glint)*TR456_WATER_GLINT_STRENGTH;
 float alpha=(.060+.18*edge+.020*ridge)*TR456_WATER_REFLECT_STRENGTH*TR456_WATER_OPACITY;

 vec2 screen=gl_FragCoord.xy*captureInvViewport();
 float waveEnergy=sat(length(grad)*9.0);
 float rough=clamp((.22+waveEnergy*.56+topView*.14-edge*.10)*TR456_WATER_ROUGH_REFLECTION,.06,.82);
 vec2 rippleOffset=vec2(grad.x,-grad.y)*(.022+.064*rough+.040*edge);
 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+vec2(grad.x,-abs(grad.y))*(.040+.092*edge+.045*rough);
 vec2 mirrorUv2=mirrorUv+rippleOffset*(.64+rough*.72)+vec2(rr.x,-rr.z)*(.012+.030*edge);
 vec2 mirrorUv3=mirrorUv-rippleOffset*(.52+rough*.45)+vec2(-rr.z,-rr.x)*(.008+.020*rough);
 vec2 mirrorUv4=mirrorUv+vec2(grad.y,grad.x)*(.018+.042*rough);
 vec2 wallStretch=vec2(0.0,(-.045-.090*edge)*TR456_WATER_WALL_STRETCH);
 vec3 refrScene=reflectionGrade(captureColor(screen+rippleOffset*.32));
 float causticA=causticLine(uv*vec2(1.15,.82)+grad*.16,t);
 float causticB=causticLine(uv.yx*vec2(.74,1.36)-grad*.10+vec2(.17,.09),t*.83);
 float bottomLight=(causticA*.70+causticB*.30)*TR456_WATER_BOTTOM_CAUSTICS*TR456_WATER_CAUSTICS_STRENGTH;
 bottomLight*=smoothstep(.08,.82,topView)*(.60+.40*smoothstep(.012,.060,energy));
 refrScene+=vec3(.42,.50,.42)*bottomLight*.16;
 vec3 mirrorTall=reflectionGrade(captureColor(mirrorUv+wallStretch)*.50+
   captureColor(mirrorUv+wallStretch*1.85)*.30+captureColor(mirrorUv+wallStretch*.35)*.20);
 vec3 mirrorSharp=reflectionGrade(captureColor(mirrorUv)*.47+captureColor(mirrorUv2)*.26+
   captureColor(mirrorUv3)*.12)*.85+mirrorTall*.15;
 vec3 mirrorSoft=reflectionGrade(captureColor(mirrorUv2)*.34+captureColor(mirrorUv3)*.28+
   captureColor(mirrorUv4)*.20+captureColor(mirrorUv-rippleOffset*.95)*.12+
   captureColor(mirrorUv+wallStretch*1.25)*.06);
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
 depthCue*=TR456_WATER_DEPTH_ABSORPTION*TR456_WATER_DEPTH_STRENGTH;
 float bodyMask=clamp(.10+.09*TR456_WATER_OPACITY+.09*(1.0-ndv)+depthCue*.16,0.0,.38);
 vec3 sceneWater=mix(refrScene*waterTint,sceneRefl,clamp(.48+edge*.42,0.0,.94));
 vec3 absorbedBody=mix(waterBody,waterDeep,clamp(depthCue*.85,0.0,1.0));
 sceneWater=mix(sceneWater,absorbedBody,bodyMask);
 col=mix(col*.84+absorbedBody*.16,sceneWater,sceneMask*.84);
 vec2 texEdge=min(uv,1.0-uv);
 float uvInside=step(0.0,uv.x)*step(0.0,uv.y)*step(uv.x,1.0)*step(uv.y,1.0);
 float uvEdge=(1.0-smoothstep(.018,.095,min(texEdge.x,texEdge.y)))*uvInside;
 vec2 px=captureInvViewport();
 float lumC=luma(captureColor(screen));
 float lumDx=abs(luma(captureColor(screen+vec2(px.x*2.0,0.0)))-lumC);
 float lumDy=abs(luma(captureColor(screen+vec2(0.0,px.y*2.0)))-lumC);
 float sceneEdge=smoothstep(.045,.18,lumDx+lumDy);
 float contact=clamp(max(uvEdge*.20,sceneEdge*.55)*(1.0-edge*.32)*TR456_WATER_CONTACT_EDGE,0.0,1.0);
 col=mix(col,absorbedBody*.76,contact*.28);
 col+=vec3(.07,.12,.11)*contact*.035;
 col+=vec3(.30,.36,.30)*bottomLight*.045;
 col+=cold*(ridge*.046+streak*.026+glint*.62)*TR456_WATER_GLINT_STRENGTH;
 alpha=max(alpha,.18+sceneMask*.22+edge*.08+bodyMask*.10+contact*.09);
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
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(1.0,.82,.05,1.0);
#else
 fragColor=vec4(col,clamp(alpha,.16,.54));
#endif
}
