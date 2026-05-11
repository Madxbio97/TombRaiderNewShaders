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
#define TR456_WATER_FLOW_CHROMA 0.35
#define TR456_WATER_FLOW_CAUSTICS 1.0
#define TR456_WATER_FLOW_SPEED 1.0
#define TR456_WATER_FLOW_STREAK_FOAM 0.75
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uViewMatrix[4];
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
 vec2 q=p+vec2(time*.026,-time*.014);
 float n0=texture(sNoise,vec3(q*.72+vec2(.13,.41),time*.020)).x;
 float n1=texture(sNoise,vec3(q*1.36+vec2(.57,.08),time*.016)).x;
 float n2=texture(sNoise,vec3(q.yx*2.20+vec2(.24,.73),time*.013)).x;
 float veins=1.0-abs((n0*.52+n1*.34+n2*.14)-.53)*5.2;
 float split=1.0-abs(n1-n2)*4.7;
 float broken=smoothstep(.38,.78,texture(sNoise,vec3(q*2.65+vec2(.71,.19),time*.024)).x);
 return pow(sat(veins),3.2)*pow(sat(split),1.7)*broken;
}

float currentStrands(vec2 p, float time){
 vec2 q=p+vec2(time*.12,0.0);
 float lane=texture(sNoise,vec3(vec2(q.y*.62,q.x*.12)+vec2(.21,.37),time*.018)).x;
 float bend=sin(q.x*1.25+time*.42+lane*2.7)*.70;
 float lateral=sin(q.y*6.4+lane*3.8+bend);
 float strand=pow(sat(lateral*.5+.5),4.6);
 float broken=smoothstep(.34,.82,texture(sNoise,vec3(q*.92+vec2(.62,.14),time*.020)).x);
 float pulse=.46+.34*(sin(q.x*3.0-time*1.05+lane*2.3)*.5+.5);
 return strand*broken*pulse;
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
 float flowTime=time*clamp(TR456_WATER_FLOW_SPEED,0.20,3.50);

 vec2 flowDir=uParams.xy;
 float flowLen=length(flowDir);
 flowDir=(flowLen>.0001) ? normalize(flowDir) : normalize(vec2(.85,.38));
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float speed=max(flowLen,0.05);
 vec3 worldPos=vPos+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vec2 flowPos=vec2(dot(worldPos.xz,flowDir),dot(worldPos.xz,side));
 vec2 stableUv=vec2(flowPos.x*.0046,flowPos.y*.0038)+
   vec2(flowTime*(.050+speed*.020),sin(flowTime*.12)*.014);

 float noiseA=texture(sNoise,vec3(stableUv*1.18,flowTime*.030)).x;
 float noiseB=texture(sNoise,vec3(stableUv.yx*1.72+vec2(.17,.31),flowTime*.024)).x;
 float noiseC=texture(sNoise,vec3(stableUv*2.55+flowDir*flowTime*.020,flowTime*.040)).x;
 float strands=currentStrands(stableUv,flowTime);
 float stream=max(lineMask(stableUv.x*5.2+noiseA*.26+flowTime*(.08+speed*.28),8.5)*.26,strands*.58);
 float currentA=sin(stableUv.x*14.5+noiseA*2.1+flowTime*(.86+speed*1.30));
 float currentB=sin(stableUv.x*7.2+stableUv.y*2.0+noiseB*1.6-flowTime*(.56+speed*.84));
 float current=currentA*.56+currentB*.44;
 float cross=sin(stableUv.y*18.0+noiseB*3.4+flowTime*(.75+speed*.42))*0.5+0.5;
 float chop=sin(stableUv.x*32.0+noiseC*3.8+flowTime*(1.95+speed*2.35));
 float small=sin(stableUv.y*38.0+stableUv.x*12.0-flowTime*2.0);
 float wave=sat(abs(current)*.34+abs(chop)*.17+abs(small)*.045+stream*.24+strands*.18);
 float streakLane=lineMask(stableUv.y*10.5+noiseB*.20+sin(stableUv.x*1.35+flowTime*.22)*.10,18.0);
 float streakBreak=smoothstep(.50,.86,texture(sNoise,vec3(stableUv*vec2(1.65,.72)+
   vec2(flowTime*.055,0.0),flowTime*.018)).x);
 float streakPulse=lineMask(stableUv.x*2.15+noiseA*.30-flowTime*(.18+speed*.25),5.0);
 float streakFoam=streakLane*streakBreak*(.36+.64*streakPulse)*
   smoothstep(.18,.74,stream+strands*.45+wave*.22)*TR456_WATER_FLOW_STREAK_FOAM;

 vec3 wake=wakeLayer(screen,flowTime);
 vec2 ripple=vec2(currentA*.015+currentB*.012+chop*.004+stream*.010+strands*.014,
   (noiseA-noiseB)*.011+(cross-.5)*.008)+wake.xy*.42;
 ripple*=TR456_WATER_FLOW_STRENGTH*TR456_WATER_SURFACE_WAVE*TR456_WATER_REFRACT_STRENGTH;

 vec2 texUv=stableUv+ripple*.24+vec2(current*.006,0.0);
 vec4 meshBase=texture(sTex0_wrap,vec3(uv+ripple,vLayer));
 vec4 worldBase=texture(sTex0_wrap,vec3(texUv,vLayer));
 vec4 base=mix(meshBase,worldBase,.54);
 base.a=mix(meshBase.a,worldBase.a,.32);
 float flowChroma=clamp(TR456_WATER_CHROMA_STRENGTH*TR456_WATER_FLOW_CHROMA,0.0,1.0);
 vec3 r=mix(
   texture(sTex0_wrap,vec3(texUv+ripple*(.20+flowChroma*.04)+vec2(0.0,.0005)*flowChroma,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv+ripple*(.08+flowChroma*.04)+vec2(.0005,0.0)*flowChroma,vLayer)).rgb,
   .35);
 vec3 b=mix(
   texture(sTex0_wrap,vec3(texUv-ripple*(.16+flowChroma*.03)-vec2(0.0,.0005)*flowChroma,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv-ripple*(.07+flowChroma*.03)-vec2(.0005,0.0)*flowChroma,vLayer)).rgb,
   .35);
 vec3 tex=mix(base.rgb,vec3(r.r,base.g,b.b),.11*flowChroma);
 tex=mix(vec3(luma(tex)),tex,clamp(TR456_WATER_TEXTURE_STRENGTH*.72,.50,1.08));

 vec3 light=clamp(vLight+vColor,vec3(0.0),vec3(1.85));
 vec3 waterTint=mix(vec3(.012,.060,.074),vec3(.026,.108,.124),sat(vFog*.68+stream*.14));
 float fres=(.045+.72*pow(1.0-sat(dot(n,viewVec)),4.0))*TR456_WATER_REFLECT_STRENGTH;
 vec3 col=mix(tex,waterTint,.08+.07*fres)*light*1.42;

 float foam=(pow(stream,1.65)*.014+pow(wave,2.3)*.010+strands*.006+
   streakFoam*.011+wake.z*.036)*TR456_WATER_FOAM_STRENGTH*TR456_WATER_FLOW_STRENGTH;
 float glint=(lineMask(noiseA+noiseB*.7+flowTime*.075,13.0)*.003+
   foam*.20+wave*.003+streakFoam*.004)*TR456_WATER_GLINT_STRENGTH;
 float caust=caustic(stableUv+vec2(flowTime*.030,0.0)+ripple*.46,flowTime)*.014*TR456_WATER_CAUSTICS_STRENGTH*TR456_WATER_FLOW_CAUSTICS;
 col+=vec3(.10,.14,.11)*caust+vec3(.14,.28,.31)*glint;
 col+=vec3(.015,.045,.050)*(strands*.28+stream*.22+wave*.14+abs(current)*.16);
 col+=vec3(.035,.070,.072)*streakFoam*.28;
 col+=vec3(.045,.080,.085)*(stream*.22+wave*.14+strands*.18+abs(current)*.14)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);

 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+
   vec2(ripple.x+wake.x*.34,-abs(ripple.y+wake.y*.30))*(.50+.58*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv2=mirrorUv+vec2(side.x,-abs(side.y))*(.004+.018*fres)*(noiseA-noiseB);
 vec3 sceneRefl=reflectionGrade(captureColor(mirrorUv)*.72+captureColor(mirrorUv2)*.28);
 float reflOk=smoothstep(.004,.045,luma(sceneRefl));
 float reflMask=clamp((.08+fres*.46+stream*.07+wave*.04)*TR456_WATER_REFLECT_STRENGTH*
   TR456_WATER_FLOW_REFLECTION*max(TR456_WATER_FORCE_REFLECTION,.25),0.0,.58)*reflOk;
 col=mix(col,sceneRefl,reflMask*(.24+.16*vFog)*TR456_WATER_SCENE_REFLECTION);

 col=mix(vec3(luma(col)),col,TR456_WATER_COLOR_SATURATION);
 col*=TR456_WATER_BRIGHTNESS;
 col=mix(uFogColor.rgb*base.a,col,vFog);
 float alpha=clamp((base.a*(.66+.10*fres)+foam*.05+streakFoam*.008+reflMask*.05)*
   TR456_WATER_OPACITY*TR456_WATER_FLOW_OPACITY,.025,.62);

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
 fragColor=vec4(streakFoam,strands,wave,1.0);
#else
 fragColor=vec4(col,alpha);
#endif
}
