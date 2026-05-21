#version 150

uniform sampler2D uTrScenePostScene;
uniform vec4 uTrScenePostInfo;
uniform vec4 uTrScenePostFx;
uniform vec4 uTrScenePostTone;

in vec2 vTrScenePostUv;
out vec4 trshaderFragColor;

struct TrScenePostFrame {
 vec2 uv;
 vec2 texel;
 float ssgi;
 float ssgiRadius;
 float detail;
 float frame;
};

float trSceneSat(float trSceneX){ return clamp(trSceneX,0.0,1.0); }
float trSceneLuma(vec3 trSceneC){ return dot(trSceneC,vec3(.2126,.7152,.0722)); }

vec2 trSceneClampUv(vec2 trSceneUv) {
 return clamp(trSceneUv,vec2(.001),vec2(.999));
}

vec3 trSceneSampleScene(vec2 trSceneUv){
 return texture(uTrScenePostScene,trSceneClampUv(trSceneUv)).rgb;
}

float trSceneSampleLuma(vec2 trSceneUv) {
 return trSceneLuma(trSceneSampleScene(trSceneUv));
}

/* TR456_SCENE_SSGI_INCLUDE */

void main(){
 TrScenePostFrame trSceneF;
 trSceneF.uv=trSceneClampUv(vTrScenePostUv);
 trSceneF.texel=max(uTrScenePostInfo.xy,vec2(1.0/8192.0));
 trSceneF.ssgi=clamp(uTrScenePostFx.x,0.0,1.5);
 trSceneF.ssgiRadius=clamp(uTrScenePostFx.y,0.35,3.0);
 trSceneF.detail=clamp(uTrScenePostTone.x,0.0,1.0);
 trSceneF.frame=uTrScenePostTone.y;

 vec3 trSceneColor=trSceneSampleScene(trSceneF.uv);
 trSceneColor=trSceneApplySSGI(trSceneColor,trSceneF);
 trshaderFragColor=vec4(clamp(trSceneColor,0.0,1.0),1.0);
}
