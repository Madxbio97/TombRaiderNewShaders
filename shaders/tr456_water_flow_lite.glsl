#version 150
uniform sampler2DArray sTex0_wrap;
uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform vec4 uTrWaterSyntheticInfo;
uniform vec4 uTrWaterSyntheticMode;
uniform vec4 uTrWaterSyntheticProfile;
uniform vec4 uTrWaterToggle0;
uniform vec4 uTrWaterToggle1;
uniform vec4 uTrWaterFlowFx0;
uniform vec4 uTrWaterFlowFx1;
uniform vec4 uTrWaterFlowFx2;
uniform vec4 uTrWaterFlowFx3;
in vec2 vFlowUv;
in vec2 vOrigUv;
in vec2 vFlowDir;
in vec3 vWorldPos;
in vec3 vLight;
in float vLayer;
in float vFog;
in float vSpeed;
in float vViewTop;
out vec4 trshaderFragColor;
float sat(float x){return clamp(x,0.0,1.0);}
vec2 softLimitVec2(vec2 v,float limit){float m=length(v);float l=max(limit,0.00001);float s=(l*(1.0-exp(-m/l)))/max(m,0.00001);return v*s;}
float locHash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453123);}
float locNoise(vec2 p){vec2 i=floor(p);vec2 f=fract(p);f=f*f*(3.0-2.0*f);float a=locHash(i);float b=locHash(i+vec2(1.0,0.0));float c=locHash(i+vec2(0.0,1.0));float d=locHash(i+vec2(1.0,1.0));return mix(mix(a,b,f.x),mix(c,d,f.x),f.y);}
vec4 flowLocale(vec2 w){float s=clamp(TR456_WATER_FLOW_LOCATION_VARIATION,0.0,1.5);if(s<=0.001)return vec4(1.0,1.0,1.0,0.0);float b=clamp(s,0.0,1.0);vec2 q=w/12288.0;float n0=locNoise(q);float n1=locNoise(q+vec2(17.3,41.9));float n2=locNoise(q*0.73+vec2(6.8,2.4));float n3=locNoise(q*1.21+vec2(23.2,9.7));return vec4(mix(1.0,mix(0.88,1.16,n0),b),mix(1.0,mix(0.82,1.25,n1),b),mix(1.0,mix(0.78,1.30,n2),b),(n3-0.5)*2.0*s);}
void main(){
 vec2 flow=length(vFlowDir)>0.00001?normalize(vFlowDir):vec2(0.0,-1.0);
 vec2 side=vec2(-flow.y,flow.x);
 vec4 locale=flowLocale(vWorldPos.xz);
 float localeDetail=locale.y;
 float localeReflect=locale.z;
 float localeTint=locale.w;
 float localeAmount=clamp(TR456_WATER_FLOW_LOCATION_VARIATION,0.0,1.0);
 float speedMul=clamp(TR456_WATER_FLOW_SPEED,0.25,3.0);
 float secondaryMotion=clamp(TR456_WATER_FLOW_SECONDARY_MOTION,0.0,2.0);
 float secondaryOpacity=clamp(TR456_WATER_FLOW_SECONDARY_OPACITY,0.0,1.5);
 float secondaryReflection=clamp(TR456_WATER_FLOW_SECONDARY_REFLECTION,0.0,2.0);
 float breakupGain=clamp(TR456_WATER_FLOW_BREAKUP,0.0,2.0);
 float specStreak=clamp(uTrWaterFlowFx1.y,0.0,2.0);
 float bumpGain=clamp(TR456_WATER_FLOW_BUMP_STRENGTH,0.0,2.0);
 float bumpScale=clamp(TR456_WATER_BUMP_SCALE,0.25,2.0);
 float texGain=clamp(TR456_WATER_TEXTURE_STRENGTH,0.0,2.0);
 float chromaGain=clamp(TR456_WATER_FLOW_CHROMA,0.0,0.35);
 float sceneScale=clamp(uTrWaterSyntheticMode.z,0.0,1.0);
 float underwaterDamp=mix(0.42,1.0,sceneScale);
 float detailNear=smoothstep(0.24,0.78,vFog);
 float detailMid=mix(0.32,1.0,detailNear);
 float detailFine=mix(0.06,1.0,detailNear);
 float detailWarp=mix(0.46,1.0,detailNear);
 float detailSpec=mix(0.48,1.0,detailNear);
 float t=uTrWaterSyntheticInfo.w*(1.08+vSpeed*0.18)*locale.x*speedMul;
 float travel=t*(2.1875+vSpeed*0.3125);
 vec2 tileEdge=vec2(0.0);
 float seamWidth=clamp(uTrWaterFlowFx3.y,0.006,0.20);
 float seamMask=0.0;
 vec2 uv=vFlowUv*vec2(1.18+localeTint*0.045,0.92-localeTint*0.035);
 float along=uv.x;
 float across=uv.y;
 vec2 flowAxis=vec2(1.0,0.0);
 vec2 sideAxis=vec2(0.0,1.0);
 float patternWarpA=sin(along*2.11+across*1.37-travel*0.17);
 float patternWarpB=sin(along*4.23-across*2.71-travel*0.31+patternWarpA*0.95);
 float patternWarpC=sin(along*7.90+across*5.20-travel*0.53+patternWarpB*0.55);
 float a=along+patternWarpA*0.095+patternWarpB*0.052+patternWarpC*0.022;
 float b=across+patternWarpB*0.070-patternWarpA*0.035+sin(along*1.35-travel*0.12)*0.040;
 float breakup=sat((abs(patternWarpA)*0.25+abs(patternWarpB)*0.20+abs(patternWarpC)*0.10)*(0.72+breakupGain*0.55));
 float driftBreakA=sin(dot(uv,vec2(2.6,-1.4))-travel*0.17);
 float driftBreakB=sin(dot(uv,vec2(-3.8,2.2))-travel*0.23+driftBreakA*0.65);
 vec2 texDrift=(flowAxis*(-travel*0.024+driftBreakA*0.010*secondaryMotion+driftBreakB*0.006*secondaryMotion)+sideAxis*(sin(dot(uv,vec2(0.8,1.7))-travel*0.68+driftBreakB*0.75)*0.014*secondaryMotion+driftBreakB*0.004*secondaryMotion))*detailMid;
 vec2 flowTuv=vec2(a*0.30+b*0.035,b*0.34+a*0.018)+texDrift;
 vec2 flowTuv2=vec2(a*0.47-b*0.055,b*0.23+a*0.040)+flowAxis*(-travel*0.010)+sideAxis*(patternWarpB*0.018);
 tileEdge=abs(fract(flowTuv)-vec2(0.5))*2.0;
 seamMask=max(smoothstep(1.0-seamWidth,1.0,tileEdge.x),smoothstep(1.0-seamWidth,1.0,tileEdge.y))*uTrWaterFlowFx3.x;
 seamMask=sat(seamMask)*0.18;
 vec4 tex0=texture(sTex0_wrap,vec3(flowTuv,vLayer));
 vec4 tex1=texture(sTex0_wrap,vec3(flowTuv+flowAxis*0.046+sideAxis*0.018,vLayer));
 vec4 tex2=texture(sTex0_wrap,vec3(flowTuv2-flowAxis*0.035+sideAxis*0.024,vLayer));
 vec4 tex3=texture(sTex0_wrap,vec3(flowTuv2+flowAxis*0.020-sideAxis*0.031,vLayer));
 vec4 tex=mix(tex0,(tex0+tex1+tex2+tex3)*0.25,0.32+seamMask*0.24);
 vec4 orig0=texture(sTex0_wrap,vec3(vOrigUv,vLayer));
 vec4 orig1=texture(sTex0_wrap,vec3(vOrigUv+vec2(0.014,0.009),vLayer));
 vec4 orig2=texture(sTex0_wrap,vec3(vOrigUv+vec2(-0.011,0.016),vLayer));
 vec4 orig3=texture(sTex0_wrap,vec3(vOrigUv+vec2(0.018,-0.013),vLayer));
 vec4 authoredDirect=mix(orig0,(orig0+orig1+orig2+orig3)*0.25,0.10);
 if(uTrWaterSyntheticMode.x<0.5) {
  tex=vec4(0.055,0.210,0.235,0.68);
  orig0=tex;
  orig1=tex;
  orig2=tex;
  orig3=tex;
  authoredDirect=tex;
 }
#if TR456_WATER_FLOW_DEBUG_MODE == 1
 trshaderFragColor=vec4(0.00,0.92,1.00,0.88);
 return;
#endif
 float authoredDirectLum=dot(authoredDirect.rgb,vec3(0.28,0.52,0.20));
 float authoredDirectLuma0=dot(orig0.rgb,vec3(0.28,0.52,0.20));
 float authoredDirectLuma1=dot(orig1.rgb,vec3(0.28,0.52,0.20));
 float authoredDirectLuma2=dot(orig2.rgb,vec3(0.28,0.52,0.20));
 float authoredDirectLuma3=dot(orig3.rgb,vec3(0.28,0.52,0.20));
 float authoredDirectLocalLum=(authoredDirectLuma0+authoredDirectLuma1+
   authoredDirectLuma2+authoredDirectLuma3)*0.25;
 float authoredDirectRange=max(authoredDirect.r,max(authoredDirect.g,authoredDirect.b))-
   min(authoredDirect.r,min(authoredDirect.g,authoredDirect.b));
 float texLuma=dot(tex.rgb,vec3(0.28,0.52,0.20));
 float texLuma0=dot(tex0.rgb,vec3(0.28,0.52,0.20));
 float texLuma1=dot(tex1.rgb,vec3(0.28,0.52,0.20));
 float texLuma2=dot(tex2.rgb,vec3(0.28,0.52,0.20));
 float texLuma3=dot(tex3.rgb,vec3(0.28,0.52,0.20));
 float texLocalLuma=(texLuma0+texLuma1+texLuma2+texLuma3)*0.25;
 float texRange=max(tex.r,max(tex.g,tex.b))-min(tex.r,min(tex.g,tex.b));
 float authoredActive=sat(uTrWaterSyntheticMode.x)*uTrWaterToggle1.x*(1.0-seamMask*0.42)*sceneScale;
 float authoredRelief=clamp((texLuma-texLocalLuma)*3.10+(texLuma0-texLuma2)*0.78+
   (texLuma1-texLuma3)*0.58,-1.0,1.0)*texGain*detailFine*authoredActive;
 float authoredReliefAbs=abs(authoredRelief);
 float authoredMask=sat((texRange*0.34+authoredDirectRange*0.52+
   authoredReliefAbs*0.48+authoredDirect.a*0.030+tex.a*0.018)*
   mix(0.48,0.96,sceneScale)*authoredActive);
 float swell=sin(a*3.60+sin(b*1.42-travel*0.28+patternWarpA*0.50)*0.95-travel*1.05);
 float runA=sin(a*8.20+b*0.75-travel*2.15+patternWarpB*0.55);
 float runB=sin(a*13.4-b*1.28-travel*3.05+patternWarpC*0.45);
 float crossToggle=uTrWaterToggle1.z*(0.24+secondaryMotion*0.08);
 float cross=sin(a*0.86+b*7.40-travel*0.95+patternWarpA*0.35)*uTrWaterFlowFx0.w*crossToggle;
 float ripple=sin(a*25.0+b*6.8-travel*6.20+patternWarpC*0.80)*sin(a*2.6+b*18.0-travel*4.10+patternWarpB*0.50)*uTrWaterFlowFx2.w*uTrWaterToggle1.x*(0.70+secondaryMotion*0.45)*detailFine;
 float chop=sin(a*17.0-b*4.4-travel*4.70+patternWarpA*0.55)*sin(a*1.1+b*9.4-travel*2.40+patternWarpC*0.45)*uTrWaterToggle1.x*(0.70+secondaryMotion*0.40)*detailFine;
 float trembleA=sin(a*42.0+b*14.0-travel*8.60+patternWarpB*1.20)*0.58+sin(a*55.0-b*31.0-travel*10.20+patternWarpC*0.90)*0.42;
 float trembleB=sin(a*38.0-b*19.0-travel*8.15+patternWarpA*1.05)*0.62+sin(a*52.0+b*27.0-travel*9.45+patternWarpB*0.70)*0.38;
 float tremble=(trembleA*0.68+trembleB*0.60)*uTrWaterFlowFx2.w*uTrWaterToggle1.x*(0.64+secondaryMotion*0.55)*detailFine;
 float trembleCross=(sin(a*6.2+b*49.0-travel*9.35+patternWarpC*0.65)*0.62+sin(a*45.0-b*16.0-travel*8.85+patternWarpA*0.75)*0.54)*uTrWaterFlowFx2.w*uTrWaterToggle1.x*(0.64+secondaryMotion*0.55)*detailFine;
 float microDomain=patternWarpA*1.15+patternWarpB*0.72+patternWarpC*0.38;
 float microA=sin(a*71.0+b*19.0-travel*12.8+microDomain*1.65);
 float microB=sin(a*29.0-b*67.0-travel*10.9-microDomain*1.35);
 float microC=sin(a*43.0+b*53.0-travel*14.2+patternWarpB*1.20-patternWarpA*0.70);
 float microRelief=(microA*0.30+microA*microB*0.34+microB*microC*0.25+microC*0.11)*uTrWaterFlowFx2.w*uTrWaterToggle1.x*localeDetail*(0.86+secondaryMotion*0.54)*bumpGain*underwaterDamp*detailFine;
 float microReliefAbs=abs(microRelief);
 float trembleAbs=abs(tremble);
 float breathA=sin(a*1.95+sin(b*1.15-travel*0.20+patternWarpA*0.30)*0.80-travel*0.36);
 float breathB=sin(a*1.20+b*2.05+sin(a*0.70-b*1.10-travel*0.16+patternWarpB*0.25)*0.55-travel*0.28);
 float breath=(breathA*0.70+breathB*0.48)*crossToggle;
 float breathAbs=abs(breath);
 float microWaveDomain=microDomain+breath*0.34+cross*0.22+patternWarpC*0.45;
 float microWave=sin(a*96.0-b*37.0-travel*15.6+microWaveDomain*1.10)*sin(a*18.0+b*83.0-travel*12.4-patternWarpC*0.80);
 microWave=(microWave*0.72+sin(a*117.0+b*46.0-travel*17.2-microWaveDomain*0.90)*0.28)*uTrWaterFlowFx2.w*uTrWaterToggle1.w*(0.86+secondaryMotion*0.54)*bumpGain*underwaterDamp*detailFine;
 float microWaveAbs=abs(microWave);
 float reliefA=sin(a*34.0+b*0.80-travel*7.4+patternWarpB*0.85+microDomain*0.42);
 float reliefB=sin(a*58.0-b*1.60-travel*10.6+patternWarpC*0.70-microDomain*0.34);
 float reliefCross=sin(a*14.0+b*45.0-travel*8.8+patternWarpA*0.80+reliefA*0.20);
 float reliefNeedle=sin(a*104.0+b*24.0-travel*15.8+microWaveDomain*0.88+reliefB*0.18);
 float flowRelief=(reliefA*0.34+reliefA*reliefB*0.30+reliefCross*0.22+
   reliefNeedle*0.14)*uTrWaterFlowFx2.w*uTrWaterToggle1.w*
   localeDetail*(0.78+secondaryMotion*0.42)*bumpGain*underwaterDamp*detailFine;
 float flowReliefAbs=abs(flowRelief);
 float shearA=sin(b*3.8+a*0.34-travel*0.42+patternWarpB*0.76);
 float shearB=sin(b*8.6-a*0.58-travel*0.74+patternWarpC*0.62+shearA*0.55);
 float currentShear=(shearA*0.54+shearB*0.32+shearA*shearB*0.22)*uTrWaterFlowFx2.w*uTrWaterToggle0.y*localeDetail*(0.74+breakupGain*0.35)*detailMid;
 float shearAbs=abs(currentShear);
 float refractionStreak=smoothstep(0.34,0.92,(sin(a*18.5+b*0.55-travel*5.8+currentShear*1.8+patternWarpA)*0.5+0.5)*(0.58+0.42*sat(sin(a*6.8-b*1.1-travel*2.4+patternWarpC)*0.5+0.5)));
 refractionStreak*=uTrWaterToggle0.y*detailFine;
 float capFilm=smoothstep(0.34,0.88,(abs(runA*0.50+cross*0.34+ripple*0.24+chop*0.18)+trembleAbs*0.12+microReliefAbs*0.18+microWaveAbs*0.13+flowReliefAbs*0.14+authoredReliefAbs*0.10+shearAbs*0.08+refractionStreak*0.07+breathAbs*0.08)*(0.78+breakupGain*0.42))*detailMid;
 float crest=smoothstep(0.44,0.94,swell*0.44+runA*0.30+runB*0.20+cross*0.24+ripple*0.14+chop*0.10+tremble*0.09+microRelief*0.11+microWave*0.07+flowRelief*0.08+authoredRelief*0.055+currentShear*0.06+breath*0.13+0.48);
 float flowBreak=smoothstep(0.28,0.88,(abs(runA-runB)*0.42+abs(cross)*0.30+capFilm*0.24+abs(chop)*0.12+trembleAbs*0.09+microReliefAbs*0.12+microWaveAbs*0.10+flowReliefAbs*0.13+authoredReliefAbs*0.12+shearAbs*0.08+breathAbs*0.06)*(0.78+breakupGain*0.46));
 float wave=swell*0.42+runA*0.26+cross*0.23+runB*0.11+ripple*0.09+chop*0.06+tremble*0.060+microRelief*0.052+microWave*0.042+flowRelief*0.060+authoredRelief*0.050+currentShear*0.045+breath*0.205;
 float textureGrain=sat((texRange*0.86+authoredMask*0.32+tex.a*0.050)*texGain*mix(0.48,0.96,sceneScale));
 float streamLine=sat(sin(a*22.0+b*1.55-travel*5.15+patternWarpB*1.10)*0.5+0.5);
 float streamEnvelope=sat(sin(b*6.2-travel*1.34+patternWarpC*0.80)*0.5+0.5);
 float streamJet=smoothstep(0.42,0.98,streamLine)*(0.24+0.42*streamEnvelope);
 streamJet*=0.50+0.24*sat(sin(a*3.7+b*2.4-travel*0.48+patternWarpA)*0.5+0.5);
 float tension=sat((capFilm*0.34+flowBreak*0.18+textureGrain*0.13+authoredMask*0.10)*uTrWaterFlowFx0.z*crossToggle);
 float ridge=smoothstep(0.34,0.90,abs(wave)*1.02+crest*0.28+tension*0.22+textureGrain*0.12+authoredReliefAbs*0.20+trembleAbs*0.08+microReliefAbs*0.22+microWaveAbs*0.18+flowReliefAbs*0.26+breathAbs*0.26)*uTrWaterFlowFx2.z*localeDetail;
 float mistWarp=sin(a*3.1-b*1.7-travel*0.37+patternWarpA*0.60)*0.22+sin(a*2.3+b*2.9-travel*0.29+patternWarpB*0.45)*0.18+breath*0.52;
 float mistA=sin(a*5.7+sin(b*2.1-travel*0.31+patternWarpC*0.55)*0.90+mistWarp-travel*1.18);
 float mistB=sin(a*11.3+b*0.9+mistWarp*1.7+patternWarpA*0.60-travel*2.04);
 float mistC=sin(a*16.5-b*3.8+sin(a*4.2+b*5.1-travel*0.41+patternWarpB*0.50)*0.55-travel*2.70);
 float flowMist=smoothstep(0.30,0.84,(mistA*0.44+mistB*0.31+mistC*0.25)*0.5+0.5)*mix(0.94,1.28,sat((localeDetail-0.82)/0.43));
 flowMist*=smoothstep(0.06,0.72,abs(cross)*0.22+capFilm*0.34+ridge*0.22+streamJet*0.16+trembleAbs*0.10+microWaveAbs*0.06+flowReliefAbs*0.08+authoredMask*0.10+breakup*0.16+textureGrain*0.10)*crossToggle;
 flowMist=sat(flowMist*(1.10+textureGrain*0.30+sceneScale*0.14));
 float reliefShade=(wave*0.066+ridge*0.120+tension*0.052+flowMist*0.052+microRelief*0.072+microReliefAbs*0.030+microWave*0.052+microWaveAbs*0.022+flowRelief*0.080+flowReliefAbs*0.035+authoredRelief*0.070+authoredReliefAbs*0.022+currentShear*0.044+refractionStreak*0.024+breath*0.112+breathAbs*0.038+tremble*0.040-abs(cross)*0.016)*(0.82+bumpGain*0.28);
 float foam=smoothstep(0.72,0.995,flowBreak*0.30+ridge*0.22+crest*0.18+abs(chop)*0.06)*0.055*uTrWaterFlowFx1.z*uTrWaterToggle0.w*uTrWaterSyntheticProfile.y;
 float specBreakA=sat(sin(a*18.0-b*8.4-travel*4.7+microDomain*0.80+currentShear)*0.5+0.5);
 float specBreakB=sat(sin(a*7.2+b*15.0-travel*5.6-patternWarpB*0.62+microWave*0.45)*0.5+0.5);
 float specScatter=sat(sin(a*2.9-b*2.1-travel*0.72+patternWarpC*0.35)*0.5+0.5);
 float specularBreakup=smoothstep(0.34,0.96,(specBreakA*0.28+specBreakB*0.22+specScatter*0.12+microWaveAbs*0.10+flowReliefAbs*0.10+authoredReliefAbs*0.055+authoredMask*0.035+shearAbs*0.10+ridge*0.09)*(0.64+specStreak*0.48+breakupGain*0.18))*detailSpec;
 float glintBase=sat(streamJet*0.16+tension*0.26+ridge*0.22+crest*0.10+max(wave,0.0)*0.08+trembleAbs*0.07+microReliefAbs*0.10+microWaveAbs*0.09+flowReliefAbs*0.11+authoredReliefAbs*0.055+refractionStreak*0.05+abs(chop)*0.035);
 float glint=pow(sat(glintBase*(0.72+specularBreakup*0.34)),18.0)*uTrWaterFlowFx1.x*uTrWaterToggle1.y*localeReflect*(0.72+specStreak*0.14)*detailSpec;
 vec2 screen=gl_FragCoord.xy*max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 screenFlow=vec2(dFdx(vFlowUv.x),dFdy(vFlowUv.x));
 vec2 dir=length(screenFlow)>0.0000001?normalize(screenFlow):normalize(vec2(flow.x,-flow.y));
 vec2 sdir=vec2(-dir.y,dir.x);
 vec2 warp=(dir*(wave*0.0235+tension*0.0130+ridge*0.0074+flowBreak*0.0024+streamJet*0.0094+flowMist*0.0115+microRelief*0.0046+microWave*0.0034+flowRelief*0.0062+authoredRelief*0.0034+currentShear*0.0062+refractionStreak*0.0088+breath*0.0200+tremble*0.0064)+sdir*(cross*0.0100+ripple*0.0048+chop*0.0038+mistC*0.0045+microRelief*0.0035+microWave*0.0026+flowRelief*0.0042+authoredRelief*0.0028+currentShear*0.0068+refractionStreak*0.0038+breathB*0.0092+trembleCross*0.0048))*uTrWaterFlowFx0.y*uTrWaterFlowFx1.w*uTrWaterToggle0.y*(0.86+bumpScale*0.18)*sceneScale*detailWarp;
 vec2 shimmerWarp=(dir*(ripple*0.0130+chop*0.0072+tension*0.0070+streamJet*0.0070+flowMist*0.0105+microRelief*0.0082+microWave*0.0068+flowRelief*0.0094+authoredRelief*0.0048+currentShear*0.0062+refractionStreak*0.0105+breath*0.0120+tremble*0.0072)+sdir*(cross*0.0082+wave*0.0026+mistB*0.0045+microRelief*0.0058+microWave*0.0042+flowRelief*0.0062+authoredRelief*0.0036+currentShear*0.0060+refractionStreak*0.0045+breathA*0.0080+trembleCross*0.0056))*(0.84+bumpScale*0.22)*sceneScale*detailFine;
 warp=softLimitVec2(warp,0.052*sceneScale);
 shimmerWarp=softLimitVec2(shimmerWarp,0.043*sceneScale);
 float viewTop=sat(vViewTop);
 float viewGrazing=pow(sat(1.0-viewTop),1.18);
 float safeSurfaceScene=sceneScale*0.52;
 float bottomStrength=clamp(TR456_WATER_FLOW_BOTTOM_REFRACTION_STRENGTH,0.0,2.0)*safeSurfaceScene;
 float bottomScale=clamp(TR456_WATER_FLOW_BOTTOM_REFRACTION_SCALE,0.25,2.5);
 float bottomVisibility=clamp(TR456_WATER_FLOW_BOTTOM_VISIBILITY,0.0,1.5)*(0.74+viewTop*0.62)*(1.0-viewGrazing*0.28)*safeSurfaceScene;
 vec2 floorWarp=(warp*(1.80+1.28*bottomScale)+shimmerWarp*(0.88+0.90*bottomScale))*bottomStrength;
 floorWarp=softLimitVec2(floorWarp,0.070*sceneScale);
 vec2 floorSideWarp=sdir*(dot(floorWarp,dir)*0.42+dot(shimmerWarp,sdir)*1.35)+dir*(dot(floorWarp,sdir)*-0.34+refractionStreak*0.030*bottomStrength);
 floorSideWarp=softLimitVec2(floorSideWarp,0.052*sceneScale);
 vec3 baseScene=texture(uTrWaterScene,clamp(screen,vec2(0.001),vec2(0.999))).rgb;
 vec2 sceneWarp=softLimitVec2(warp+shimmerWarp*uTrWaterToggle0.y,0.062*sceneScale);
 vec3 scene=texture(uTrWaterScene,clamp(screen+sceneWarp,vec2(0.001),vec2(0.999))).rgb;
 if(chromaGain>0.001){vec2 chromaShift=normalize(sceneWarp+dir*0.0007+sdir*0.0003)*chromaGain*0.010;scene.r=texture(uTrWaterScene,clamp(screen+sceneWarp+chromaShift,vec2(0.001),vec2(0.999))).r;scene.b=texture(uTrWaterScene,clamp(screen+sceneWarp-chromaShift,vec2(0.001),vec2(0.999))).b;}
 vec3 floorA=texture(uTrWaterScene,clamp(screen+floorWarp+floorSideWarp*0.30,vec2(0.001),vec2(0.999))).rgb;
 vec3 floorB=texture(uTrWaterScene,clamp(screen-floorWarp*0.64+floorSideWarp*0.74,vec2(0.001),vec2(0.999))).rgb;
 vec3 floorC=texture(uTrWaterScene,clamp(screen+floorWarp*0.38-floorSideWarp*1.05,vec2(0.001),vec2(0.999))).rgb;
 vec3 floorScene=floorA*0.52+floorB*0.28+floorC*0.20;
 scene=mix(baseScene,scene,0.34*sceneScale);
 floorScene=mix(baseScene,floorScene,0.32*sceneScale);
 vec3 refractDelta=floorScene-baseScene;
 vec3 sceneDelta=scene-baseScene;
 vec3 floorPairDelta=floorA-floorB;
 vec3 safeRefractDelta=vec3(clamp(dot(refractDelta,vec3(0.28,0.52,0.20)),-0.070,0.070));
 vec3 safeSceneDelta=vec3(clamp(dot(sceneDelta,vec3(0.28,0.52,0.20)),-0.066,0.066));
 vec3 safeFloorPairDelta=vec3(clamp(dot(floorPairDelta,vec3(0.28,0.52,0.20)),-0.060,0.060));
 vec3 lensRaw=safeSceneDelta*0.84+safeRefractDelta*1.00+safeFloorPairDelta*0.30;
 float lensLuma=dot(lensRaw,vec3(0.28,0.52,0.20));
 vec3 lensDelta=clamp(vec3(lensLuma)*0.62+(lensRaw-vec3(lensLuma))*0.18,vec3(-0.125),vec3(0.125));
 float floorDelta=sat(length(floorA-floorB)*3.35+length(refractDelta)*3.05+length(sceneDelta)*2.25+abs(wave)*0.10+ridge*0.060+flowReliefAbs*0.08+authoredReliefAbs*0.06+refractionStreak*0.08);
 float bottomLens=sat((0.20+floorDelta*1.16)*bottomVisibility);
 float lensEnergy=sat(length(sceneDelta)*4.80+length(refractDelta)*5.30+length(floorA-floorB)*3.90+bottomLens*0.24);
#if TR456_WATER_FLOW_DEBUG_MODE == 2
 vec3 debugHeat=clamp(vec3(bottomLens,length(sceneDelta)*5.0,length(refractDelta)*5.0),0.0,1.0);
 trshaderFragColor=vec4(debugHeat,0.88);
 return;
#endif
 scene=mix(scene,floorScene,sat(0.34*bottomVisibility*sceneScale));
 vec2 reflAnchor=screen+warp*0.28+shimmerWarp*0.18*uTrWaterToggle0.y;
 vec2 reflBase=reflAnchor+dir*(wave*0.010+tension*0.006+flowRelief*0.004+breath*0.006)+sdir*(cross*0.004+ripple*0.002+chop*0.0015+flowRelief*0.003+breathB*0.003);
 vec2 reflStretch=dir*(0.024+ridge*0.012+flowMist*0.010+breathAbs*0.010+specStreak*0.012+secondaryReflection*0.006);
 vec2 reflScatter=sdir*(0.006+abs(cross)*0.006+trembleAbs*0.004+specStreak*0.004);
 vec3 refl0=texture(uTrWaterScene,clamp(reflBase+reflStretch+reflScatter,vec2(0.001),vec2(0.999))).rgb;
 vec3 refl1=texture(uTrWaterScene,clamp(reflBase-reflStretch*0.82-reflScatter*0.45,vec2(0.001),vec2(0.999))).rgb;
 vec3 refl2=texture(uTrWaterScene,clamp(reflBase+reflStretch*0.86+sdir*(mistB*0.006+0.004),vec2(0.001),vec2(0.999))).rgb;
 vec3 refl=refl0*0.52+refl1*0.32+refl2*0.16;
 float reflLum=dot(refl,vec3(0.28,0.52,0.20));
 float reflHi=smoothstep(0.24,0.82,reflLum);
 refl=mix(vec3(reflLum)*vec3(0.82,0.94,1.02)+vec3(0.012,0.034,0.040),refl,0.34+reflHi*0.42);
 refl=mix(refl,scene*vec3(0.68,0.80,0.86)+vec3(0.010,0.030,0.034),0.09+flowMist*0.055);
 vec3 flowHighlightColor=mix(vec3(reflLum)*vec3(0.90,0.98,1.04),refl,0.52+reflHi*0.30);
 flowHighlightColor=max(flowHighlightColor+vec3(0.006,0.010,0.010),vec3(0.045,0.056,0.054));
 vec3 flowSpecColor=mix(flowHighlightColor,vec3(0.86,0.94,0.91),0.16);
 float fres=sat(0.30+abs(wave)*0.20+tension*0.30+crest*0.12+microWaveAbs*0.05+flowReliefAbs*0.06);
 float waterStrength=uTrWaterFlowFx2.x*uTrWaterSyntheticProfile.z*clamp(localeDetail,0.86,1.18);
 vec3 lit=clamp(sqrt(max(vLight,vec3(0.0)))*1.35,vec3(0.58),vec3(1.72));
 float authoredDelta=authoredDirectLum-authoredDirectLocalLum;
 float authoredCaustic=smoothstep(0.30,1.08,
   abs(authoredDelta)*4.20+authoredDirectRange*0.70+
   authoredReliefAbs*0.28+ridge*0.06);
 float authoredThread=smoothstep(0.70,1.18,
   authoredCaustic*(0.62+ridge*0.18)+glintBase*0.12+capFilm*0.055);
 float authoredHigh=sat(0.50+authoredDelta*2.80);
 vec3 authoredMilky=(vec3(0.058,0.168,0.176)+
   vec3(0.016,0.044,0.040)*(authoredHigh-0.5)+
   vec3(0.042,0.112,0.104)*authoredCaustic+
   vec3(0.090,0.184,0.160)*authoredThread)*lit;
 vec3 procBase=vec3(0.040,0.138,0.150)+
   vec3(0.026,0.086,0.088)*(swell*0.5+0.5)+
   vec3(0.016,0.052,0.054)*(ridge*0.68+tension*0.32);
 vec3 texWater=mix(procBase,authoredMilky,0.48+0.18*sceneScale)+
   vec3(0.046,0.128,0.118)*authoredCaustic+
   vec3(0.058,0.156,0.142)*authoredThread;
 vec3 tint=vec3(0.007,0.038,0.052)*uTrWaterSyntheticInfo.y;
 vec3 localeGrade=mix(vec3(0.96,1.05,1.07),vec3(1.08,1.03,0.94),sat(localeTint*0.5+0.5));
 localeGrade=mix(vec3(1.0),localeGrade,localeAmount*(0.16+abs(localeTint)*0.26));
 vec3 paintedBase=(mix(procBase,texWater,0.66+0.16*sceneScale)+
   tint*(0.05+waterStrength*0.045))*localeGrade;
 float originalLum=texLuma;
 float sceneLum=dot(scene,vec3(0.28,0.52,0.20));
 float originalEdge=smoothstep(0.018,0.110,fwidth(originalLum));
 float sceneEdge=smoothstep(0.020,0.120,fwidth(sceneLum));
 float underBreak=sat(max(originalEdge,sceneEdge*0.62)+seamMask*0.42+breakup*0.08);
 vec3 underBase=mix(authoredMilky,paintedBase,0.42+underBreak*0.34);
 vec3 authoredTopRaw=vec3(authoredHigh);
 float authoredTopLum=dot(authoredTopRaw,vec3(0.28,0.52,0.20));
 float authoredTopContrast=sat((abs(authoredDelta)*4.70+
   authoredDirectRange*0.72+texRange*0.18+authoredReliefAbs*0.32+
   authoredDirect.a*0.040)*authoredActive);
 vec3 authoredTopColor=(vec3(0.056,0.158,0.160)+
   vec3(0.020,0.050,0.046)*(authoredHigh-0.5)+
   vec3(0.054,0.126,0.112)*authoredTopContrast+
   vec3(0.104,0.204,0.174)*authoredThread)*lit;
 authoredTopColor=mix(authoredTopColor,vec3(0.060,0.174,0.172),0.040+flowMist*0.012);
 float authoredTopAlpha=sat((0.055+authoredTopContrast*0.48+
   authoredMask*0.22+textureGrain*0.075+ridge*0.018+capFilm*0.016+
   authoredDirect.a*0.024+tex.a*0.018)*(1.0-sceneEdge*0.14)*(1.0-seamMask*0.20)*
   authoredActive*detailMid*mix(0.82,1.15,sat(texGain*0.50)));
 float surfacePresence=sat((0.62+ridge*0.050+flowMist*0.038+
   authoredCaustic*0.080+glint*0.040+bottomLens*0.024)*mix(0.76,1.0,sceneScale));
 vec3 water=mix(baseScene,paintedBase,surfacePresence);
 vec3 refractedScene=mix(scene,floorScene,clamp(bottomLens*0.72,0.0,0.82));
 float refractedLum=dot(refractedScene,vec3(0.28,0.52,0.20));
 refractedScene=mix(refractedScene,vec3(refractedLum)*vec3(0.82,0.96,1.04),0.18*uTrWaterToggle0.y*sceneScale);
 water=mix(water,refractedScene*vec3(0.80,0.94,0.98)+tint*0.06,
   sat(0.16*(1.0-sceneEdge*0.22)*uTrWaterFlowFx0.y*
   (0.84+bottomVisibility*0.18)*sceneScale));
 water=mix(water,floorScene*vec3(0.82,0.96,1.00)+tint*0.05,
   sat(bottomLens*0.10*(1.0-viewGrazing*0.24)*sceneScale));
 water+=safeFloorPairDelta*bottomLens*(0.070+breakupGain*0.024)*sceneScale;
 water+=safeRefractDelta*bottomLens*(0.090+secondaryOpacity*0.034)*sceneScale;
 water+=safeSceneDelta*sat(bottomLens+flowBreak*0.12)*(0.080+secondaryMotion*0.028)*sceneScale;
 water+=lensDelta*(0.085+bottomLens*0.095+flowBreak*0.035)*sceneScale;
 water+=vec3(0.006,0.018,0.018)*lensEnergy*(0.070+bottomLens*0.070)*sceneScale;
 float flowHaze=sat((flowMist*0.62+capFilm*0.18+streamJet*0.10+textureGrain*0.10)*(0.58+sceneScale*0.42));
 water=mix(water,water*vec3(0.96,1.04,1.02)+authoredMilky*0.26+
   vec3(0.018,0.058,0.054),flowHaze*(0.080+bottomLens*0.035)*sceneScale);
 water+=vec3(0.008,0.024,0.023)*flowHaze*(0.45+bottomLens*0.25)*sceneScale;
 float wetGloss=sat((0.16+fres*0.38+capFilm*0.12+ridge*0.055+flowMist*0.040+
   authoredMask*0.045)*(0.78+viewGrazing*0.34))*uTrWaterToggle1.y*localeReflect*detailSpec;
 float crestSpark=pow(sat(glintBase*0.46+specularBreakup*0.18+crest*0.08+
   ridge*0.060+authoredReliefAbs*0.035),22.0)*uTrWaterFlowFx1.x*
   uTrWaterToggle1.y*localeReflect*detailSpec*(0.58+specStreak*0.14);
 float gloss=sat(wetGloss*0.48+glint*0.54+crestSpark*0.24+ridge*0.10+microWaveAbs*0.055+fres*0.14);
 float angleReflect=mix(0.82,1.35,sat(viewGrazing*clamp(TR456_WATER_FLOW_FRESNEL_REFLECTION,0.0,2.0)));
 float reflAmt=sat((0.120+fres*0.34+wetGloss*0.090+ridge*0.088+glint*0.125+crestSpark*0.055+flowMist*0.070+microReliefAbs*0.030+microWaveAbs*0.026+specularBreakup*specStreak*0.052)*uTrWaterSyntheticInfo.z*uTrWaterFlowFx0.x*uTrWaterFlowFx3.w*uTrWaterSyntheticProfile.w*uTrWaterToggle0.z*localeReflect*angleReflect*(0.96+secondaryReflection*0.42));
 water=mix(water,refl*vec3(0.90,1.00,1.02)+tint*0.06+
   flowHighlightColor*(0.024+0.028*reflHi)*gloss,reflAmt);
 water*=clamp(vec3(1.0)+vec3(reliefShade*0.65,reliefShade*0.78,reliefShade*0.82),vec3(0.82),vec3(1.18));
 water-=vec3(0.012,0.022,0.024)*smoothstep(0.34,0.88,-wave+abs(cross)*0.16)*uTrWaterFlowFx2.y;
 water+=vec3(0.007,0.020,0.022)*(ridge*uTrWaterFlowFx2.y+tension*uTrWaterFlowFx2.z)*crossToggle;
 water+=flowHighlightColor*(glint*0.18+wetGloss*0.024+crestSpark*0.12)+vec3(0.012,0.026,0.024)*foam+flowSpecColor*specularBreakup*specStreak*0.030;
 water+=vec3(0.010,0.028,0.027)*(streamJet*0.42+ridge*0.24+flowMist*0.16+microReliefAbs*0.10+microWaveAbs*0.06+flowReliefAbs*0.10+authoredReliefAbs*0.10+capFilm*0.06+textureGrain*0.14+trembleAbs*0.04)*waterStrength;
 water=mix(water,underBase,0.090*uTrWaterToggle1.x*(1.0-seamMask*0.18)*(1.0-underBreak*0.52)*sceneScale);
 water+=clamp(underBase-vec3(dot(underBase,vec3(0.28,0.52,0.20))),vec3(-0.18),vec3(0.18))*0.090*(1.0-seamMask*0.25)*(1.0-underBreak*0.62)*sceneScale;
 water+=flowHighlightColor*(glint*(0.18+ridge*0.10)+crestSpark*(0.08+ridge*0.10));
 water+=vec3(0.070,0.148,0.132)*authoredCaustic*authoredActive*
   (0.16+ridge*0.10+capFilm*0.08)*sceneScale;
 water+=vec3(0.105,0.195,0.172)*authoredThread*authoredActive*
   (0.13+glint*0.08)*sceneScale;
 water+=vec3(0.012,0.026,0.026)*foam*(0.55+flowBreak*0.18);
 water+=vec3(0.003,0.011,0.013)*(ridge*(0.20+capFilm*0.20)+flowMist*0.18+microReliefAbs*0.07+microWaveAbs*0.05+flowReliefAbs*0.08)*uTrWaterFlowFx2.y;
 water=mix(water,water*0.96+vec3(0.014,0.050,0.052)+scene*vec3(0.020,0.034,0.036),(0.024+ridge*0.008+streamJet*0.006+flowMist*0.020+textureGrain*0.008)*sceneScale);
 water=mix(water,underBase,0.044*uTrWaterToggle1.x*(1.0-seamMask*0.20)*(1.0-underBreak*0.56)*sceneScale);
 water=mix(water,mix(baseScene,paintedBase+vec3(0.018,0.062,0.060),0.46),0.08*(1.0-sceneScale));
 water=mix(vec3(0.010,0.032,0.040),water,vFog);
 float sceneAnchor=sat((0.76+ridge*0.055+glint*0.050+wetGloss*0.034+
   crestSpark*0.020+reflAmt*0.090+flowMist*0.050+flowReliefAbs*0.020+
   authoredMask*0.020+authoredCaustic*0.040+textureGrain*0.014)*mix(0.82,1.0,sceneScale));
 water=mix(baseScene,water,sceneAnchor);
 vec3 authoredTopInk=authoredTopColor*(0.92+authoredTopContrast*0.24)+
   vec3(0.040,0.096,0.086)*authoredThread+
   flowHighlightColor*(wetGloss*0.028+crestSpark*0.028);
 vec3 authoredTopTarget=mix(water*0.92+authoredTopInk*0.58,
   water*0.64+authoredTopInk*0.82,authoredTopContrast*0.30);
 water=mix(water,authoredTopTarget,authoredTopAlpha);
 water+=clamp(authoredTopColor-vec3(authoredTopLum)*vec3(0.62,0.94,1.06),
   vec3(-0.12),vec3(0.18))*authoredTopAlpha*0.40;
 water+=vec3(authoredDelta)*authoredTopAlpha*0.055;
 water+=flowHighlightColor*(glint*(0.22+ridge*0.12)+wetGloss*0.018+crestSpark*(0.12+ridge*0.10))*uTrWaterToggle1.y;
 water+=flowSpecColor*specularBreakup*specStreak*(0.020+ridge*0.034+crestSpark*0.012)*uTrWaterToggle1.y;
 water+=vec3(0.016,0.046,0.046)*flowMist*crossToggle*sceneScale;
 float flowOpacity=clamp(TR456_WATER_FLOW_OPACITY,0.08,0.60);
 float stableTexAlpha=mix(0.36,clamp(tex.a*0.90,0.0,1.0),0.16+0.18*sceneScale);
 float alpha=clamp(stableTexAlpha*uTrWaterSyntheticInfo.x*flowOpacity*4.0*(0.98+ridge*0.050+tension*0.045+streamJet*0.026+glint*0.040+flowReliefAbs*0.024+bottomLens*0.040+secondaryOpacity*0.030),mix(0.30,0.38,sceneScale),mix(0.58,0.70,sceneScale))*uTrWaterToggle0.x;
 float compositeLens=sat((bottomLens*1.08+flowBreak*0.24+ridge*0.11)*uTrWaterToggle0.y*sceneScale);
 water+=lensDelta*(0.075+secondaryOpacity*0.030)*compositeLens*(1.0/max(alpha,0.36));
 water+=vec3(0.002,0.008,0.010)*lensEnergy*compositeLens*.36;
 trshaderFragColor=vec4(clamp(water,0.0,1.0),alpha);
}
