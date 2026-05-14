#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#endif
#ifndef TR456_WATER_GAME_RIPPLE_STRENGTH
#define TR456_WATER_GAME_RIPPLE_STRENGTH 1.0
#endif
#ifndef TR456_WATER_RIPPLE_SPRITE_VISUAL
#define TR456_WATER_RIPPLE_SPRITE_VISUAL 0.0
#endif

uniform sampler3D sNoise;
uniform sampler2DArray sTex0;
uniform sampler2DArray sTex1;
uniform sampler2DArrayShadow sShadow;
uniform sampler2DArray sTex0_wrap;
uniform sampler2DArray sTex0_mirror;
in vec4 vTexCoord;
in vec4 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
in vec4 vSProj;
uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform mat4 uShadowMatrix;
uniform vec4 uFogColor;
uniform vec4 uContacts[16];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uTrWaterRippleInfo;
uniform vec4 uJoints[32 * 3];
uniform vec4 uLightPos[4];
uniform vec4 uLightCol[4];
uniform vec4 uAmbient[6];
out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(.299,.587,.114)); }

void main(){
 vec4 tex=texture(sTex0,vec3(vTexCoord.xy,vLayer));
 vec4 src=tex*vColor;
 vec4 original=src;
 original.rgb=mix(uFogColor.rgb*original.a,original.rgb,vFog);
 float waterSprite=step(.5,uTrWaterRippleInfo.x);
 float visualSprite=waterSprite*clamp(TR456_WATER_RIPPLE_SPRITE_VISUAL,0.0,1.0);
 float largeSplash=step(uTrWaterRippleInfo.z+.5,uTrWaterRippleInfo.y)*
  step(1.5,abs(uParams.w))*step(abs(uParams.x),.01);
 if(largeSplash>.5){
  float a=original.a*.10;
  vec3 mist=mix(vec3(luma(original.rgb))*vec3(.72,.86,.84),original.rgb,.18);
  fragColor=vec4(mist*(.07+a*.22),a);
  return;
 }
#if TR456_WATER_DEBUG_MODE != 10
 if(visualSprite<=.001){
  fragColor=original;
  return;
 }
#endif
 float strength=clamp(TR456_WATER_GAME_RIPPLE_STRENGTH,0.0,2.5);
 vec2 texel=vec2(.0034,0.0);
 float center=luma(max(src.rgb,vec3(0.0)));
 float texCenter=luma(texture(sTex0,vec3(vTexCoord.xy,vLayer)).rgb);
 float texX=luma(texture(sTex0,vec3(vTexCoord.xy+texel.xy,vLayer)).rgb);
 float texY=luma(texture(sTex0,vec3(vTexCoord.xy+texel.yx,vLayer)).rgb);
 float edge=sat((abs(texCenter-texX)+abs(texCenter-texY))*2.4);
 float alphaGate=smoothstep(.012,.20,src.a);
 float line=smoothstep(.045,.48,center+edge*.65)*alphaGate;
 float n=texture(sNoise,vec3(vTexCoord.xy*3.4,uModelMatrix[3].x*.018)).x;
 line*=.82+.18*n;

 vec4 waterized=original;
 vec3 crest=vec3(.12,.34,.36)*pow(line,1.25)*(.38+.18*strength);
 vec3 cool=original.rgb*vec3(.78,.96,1.02)+crest;
 waterized.rgb=mix(original.rgb,cool,sat(line*(.24+.18*strength)));
 waterized.a=original.a;

#if TR456_WATER_DEBUG_MODE == 10
 fragColor=mix(original,vec4(line,edge,0.0,original.a),waterSprite);
#else
 fragColor=mix(original,waterized,visualSprite);
#endif
}
