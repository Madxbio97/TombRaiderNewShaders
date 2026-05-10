#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.0
#define TR456_WATER_REFRACT_STRENGTH 1.0
#define TR456_WATER_REFLECT_STRENGTH 1.0
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_OPACITY 0.62
#define TR456_WATER_FORCE_REFLECTION 0.65
#define TR456_WATER_SCENE_REFLECTION 0.78
#define TR456_WATER_CAUSTICS_STRENGTH 1.10
#define TR456_WATER_RIPPLE_STRENGTH 0.65
#define TR456_WATER_RIPPLE_CENTER_X 0.50
#define TR456_WATER_RIPPLE_CENTER_Y 0.62
#define TR456_WATER_MIRROR_ROUGHNESS 1.0
#define TR456_WATER_REFLECTION_CONTRAST 1.45
#define TR456_WATER_COLOR_SATURATION 1.18
#define TR456_WATER_BRIGHTNESS 0.86
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#define TR456_WATER_FLOW_STRENGTH 1.0
#define TR456_WATER_FLOW_REFLECTION 1.0
#define TR456_WATER_FLOW_OPACITY 1.0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
in vec2 vTexCoord;
in vec3 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(0.2126,0.7152,0.0722)); }

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec3 captureColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.86,.96,1.04),.18);
 return clamp(c,vec3(0.0),vec3(3.0));
}

float lineMask(float x, float power){
 return pow(1.0-abs(fract(x)-.5)*2.0,power);
}

float caustic(vec2 p, float time){
 float a=lineMask(p.x*.72+p.y*.19+time*.090,12.0);
 float b=lineMask(p.x*-.31+p.y*.58-time*.066,10.0);
 return a*.65+b*.35;
}

vec3 wakeLayer(vec2 screen, float time){
 vec2 p=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.35,1.0);
 float d=length(p)+.0001;
 float trail=(1.0-smoothstep(.035,.36,abs(p.x)))*
   smoothstep(-.30,.02,p.y)*(1.0-smoothstep(.10,.72,p.y));
 float ring=sin(d*78.0-time*6.8)*exp(-d*4.4)*smoothstep(.025,.24,d);
 float wake=sin((p.y-time*.28)*66.0+sin(p.x*17.0))*trail*exp(-abs(p.x)*5.0);
 vec2 dir=normalize(p);
 vec2 flow=dir*ring*.028+vec2(p.x,-p.y)*wake*.014;
 float crest=sat(abs(ring)*.78+abs(wake)*.55);
 return vec3(flow*TR456_WATER_RIPPLE_STRENGTH,crest);
}

void main(){
 vec2 uv=vTexCoord;
 vec3 n=normalize(vNormal);
 vec3 viewVec=normalize(-vPos);
 float time=uModelMatrix[3].x;
 vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;

 vec2 flowDir=uParams.xy;
 float flowLen=length(flowDir);
 flowDir=(flowLen>.0001) ? normalize(flowDir) : normalize(vec2(.85,.38));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float speed=max(flowLen,0.05);
 vec2 stableUv=screen*vec2(2.60,1.45)+flowDir*time*(.070+speed*.018)+side*sin(time*.21)*.035;

 float noiseA=texture(sNoise,vec3(stableUv*1.30,time*.045)).x;
 float noiseB=texture(sNoise,vec3(stableUv.yx*2.10+vec2(.17,.31),time*.032)).x;
 float noiseC=texture(sNoise,vec3(stableUv*3.70+flowDir*time*.035,time*.070)).x;
 float stream=lineMask(dot(stableUv,flowDir)*8.0+noiseA*.45+time*(.20+speed*.55),9.0);
 float cross=sin(dot(stableUv,side)*34.0+noiseB*5.2+time*(1.7+speed))*0.5+0.5;
 float chop=sin(dot(stableUv,flowDir)*82.0+noiseC*6.0+time*(4.0+speed*4.5));
 float small=sin(dot(stableUv,side)*91.0+dot(stableUv,flowDir)*23.0-time*5.4);
 float wave=sat(abs(chop)*.52+abs(small)*.18+stream*.42);

 vec3 wake=wakeLayer(screen,time);
 vec2 ripple=(flowDir*(chop*.010+stream*.014)+side*((noiseA-noiseB)*.020+(cross-.5)*.010)+wake.xy*.72);
 ripple*=TR456_WATER_FLOW_STRENGTH*TR456_WATER_SURFACE_WAVE*TR456_WATER_REFRACT_STRENGTH;

 vec2 texUv=stableUv+ripple*.34;
 vec4 meshBase=texture(sTex0_wrap,vec3(uv+ripple,vLayer));
 vec4 worldBase=texture(sTex0_wrap,vec3(texUv,vLayer));
 vec4 base=mix(meshBase,worldBase,.20);
 vec3 r=mix(
   texture(sTex0_wrap,vec3(uv+ripple*(1.18+TR456_WATER_CHROMA_STRENGTH*.10)+side*.0012*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv+ripple*(.18+TR456_WATER_CHROMA_STRENGTH*.08)+side*.0012*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .16);
 vec3 b=mix(
   texture(sTex0_wrap,vec3(uv+ripple*.82-side*.0012*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv-ripple*(.14+TR456_WATER_CHROMA_STRENGTH*.05)-side*.0012*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .16);
 vec3 tex=mix(base.rgb,vec3(r.r,base.g,b.b),.20*TR456_WATER_CHROMA_STRENGTH);
 tex=mix(vec3(luma(tex)),tex,clamp(TR456_WATER_TEXTURE_STRENGTH,.60,1.75));

 vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.85));
 vec3 waterTint=mix(vec3(.020,.105,.120),vec3(.050,.205,.225),sat(vFog*.75+stream*.25));
 float fres=(.045+.72*pow(1.0-sat(dot(n,viewVec)),4.0))*TR456_WATER_REFLECT_STRENGTH;
 vec3 col=mix(tex,waterTint,.16+.12*fres)*light*1.72;

 float foam=(pow(stream,1.35)*.040+pow(wave,2.2)*.035+wake.z*.060)*TR456_WATER_FOAM_STRENGTH*TR456_WATER_FLOW_STRENGTH;
 float glint=(lineMask(noiseA+noiseB*.7+time*.11,13.0)*.015+foam*.55+wave*.010)*TR456_WATER_GLINT_STRENGTH;
 float caust=caustic(stableUv+ripple*1.4,time)*.034*TR456_WATER_CAUSTICS_STRENGTH;
 col+=vec3(.34,.66,.70)*(glint+caust);
 col+=vec3(.08,.16,.17)*(stream*.50+wave*.32)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);

 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+
   vec2(ripple.x+wake.x*.45,-abs(ripple.y+wake.y*.40))*(.65+.85*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv2=mirrorUv+vec2(side.x,-abs(side.y))*(.004+.018*fres)*(noiseA-noiseB);
 vec3 sceneRefl=reflectionGrade(captureColor(mirrorUv)*.72+captureColor(mirrorUv2)*.28);
 float reflOk=smoothstep(.004,.045,luma(sceneRefl));
 float reflMask=clamp((.11+fres*.62+stream*.10+wave*.06)*TR456_WATER_REFLECT_STRENGTH*
   TR456_WATER_FLOW_REFLECTION*max(TR456_WATER_FORCE_REFLECTION,.25),0.0,.78)*reflOk;
 col=mix(col,sceneRefl,reflMask*(.28+.22*vFog)*TR456_WATER_SCENE_REFLECTION);

 col=mix(vec3(luma(col)),col,TR456_WATER_COLOR_SATURATION);
 col*=TR456_WATER_BRIGHTNESS;
 col=mix(uFogColor.rgb*base.a,col,vFog);
 float alpha=clamp((base.a*(.78+.14*fres)+foam*.18+reflMask*.08)*
   TR456_WATER_OPACITY*TR456_WATER_FLOW_OPACITY,.035,.82);

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
 fragColor=vec4(wake.z,stream,wave,1.0);
#else
 fragColor=vec4(col,alpha);
#endif
}
