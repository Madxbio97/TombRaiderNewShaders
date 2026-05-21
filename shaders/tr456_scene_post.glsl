#version 150

uniform sampler2D uTrScenePostScene;
uniform vec4 uTrScenePostInfo;
uniform vec4 uTrScenePostFx;
uniform vec4 uTrScenePostTone;

in vec2 vTrScenePostUv;
out vec4 trshaderFragColor;

float trshaderSat(float trshaderX){ return clamp(trshaderX,0.0,1.0); }
float trshaderLuma(vec3 trshaderC){ return dot(trshaderC,vec3(.2126,.7152,.0722)); }

vec3 trshaderSampleScene(vec2 trshaderUv){
 return texture(uTrScenePostScene,clamp(trshaderUv,vec2(.001),vec2(.999))).rgb;
}

void main(){
 vec2 trshaderUv=clamp(vTrScenePostUv,vec2(.001),vec2(.999));
 vec2 trshaderTexel=max(uTrScenePostInfo.xy,vec2(1.0/8192.0));
 float trshaderBump=clamp(uTrScenePostFx.x,0.0,1.5);
 float trshaderBumpScale=clamp(uTrScenePostFx.y,0.25,3.0);
 float trshaderSSGI=clamp(uTrScenePostFx.z,0.0,1.5);
 float trshaderRadius=clamp(uTrScenePostFx.w,0.35,3.0);
 float trshaderDetail=clamp(uTrScenePostTone.x,0.0,1.0);
 vec2 trshaderStep=trshaderTexel*trshaderRadius;

 vec3 trshaderC=trshaderSampleScene(trshaderUv);
 vec3 trshaderL=trshaderSampleScene(trshaderUv-vec2(trshaderStep.x,0.0));
 vec3 trshaderR=trshaderSampleScene(trshaderUv+vec2(trshaderStep.x,0.0));
 vec3 trshaderD=trshaderSampleScene(trshaderUv-vec2(0.0,trshaderStep.y));
 vec3 trshaderU=trshaderSampleScene(trshaderUv+vec2(0.0,trshaderStep.y));
 vec3 trshaderLD=trshaderSampleScene(trshaderUv-trshaderStep);
 vec3 trshaderRU=trshaderSampleScene(trshaderUv+trshaderStep);
 vec3 trshaderLU=trshaderSampleScene(trshaderUv+vec2(-trshaderStep.x,trshaderStep.y));
 vec3 trshaderRD=trshaderSampleScene(trshaderUv+vec2(trshaderStep.x,-trshaderStep.y));

 float trshaderY=trshaderLuma(trshaderC);
 float trshaderYL=trshaderLuma(trshaderL);
 float trshaderYR=trshaderLuma(trshaderR);
 float trshaderYD=trshaderLuma(trshaderD);
 float trshaderYU=trshaderLuma(trshaderU);
 vec2 trshaderGrad=vec2(trshaderYR-trshaderYL,trshaderYU-trshaderYD);
 float trshaderEdge=max(abs(trshaderGrad.x),abs(trshaderGrad.y));
 float trshaderEdgeMask=1.0-smoothstep(.16,.44,trshaderEdge);

 vec3 trshaderBlur=(trshaderL+trshaderR+trshaderD+trshaderU+
   trshaderLD+trshaderRU+trshaderLU+trshaderRD)*.125;
 vec3 trshaderHi=trshaderC-trshaderBlur;
 float trshaderHiMask=(1.0-smoothstep(.20,.62,length(trshaderHi)))*
   smoothstep(.035,.28,trshaderY);

 vec2 trshaderWarp=-trshaderGrad*trshaderTexel*
   (8.0+18.0*trshaderBumpScale)*trshaderBump*trshaderEdgeMask;
 vec3 trshaderBumped=trshaderSampleScene(trshaderUv+trshaderWarp);
 vec3 trshaderN=normalize(vec3(-trshaderGrad*trshaderBumpScale*3.2,1.0));
 float trshaderMicroLight=trshaderSat(dot(trshaderN,normalize(vec3(-.34,.46,.82)))-.58);
 trshaderC=mix(trshaderC,trshaderBumped,trshaderSat(trshaderBump*.34));
 trshaderC+=trshaderHi*(trshaderDetail*.52+trshaderBump*.18)*trshaderHiMask;
 trshaderC+=vec3(.022,.024,.020)*trshaderMicroLight*trshaderBump*trshaderHiMask;

 float trshaderAround=(trshaderYL+trshaderYR+trshaderYD+trshaderYU+
   trshaderLuma(trshaderLD)+trshaderLuma(trshaderRU)+
   trshaderLuma(trshaderLU)+trshaderLuma(trshaderRD))*.125;
 float trshaderConcavity=trshaderSat((trshaderAround-trshaderY)*2.2);
 float trshaderCrease=smoothstep(.018,.145,trshaderEdge)*
   (1.0-smoothstep(.72,.98,trshaderY));
 float trshaderOcc=trshaderSat((trshaderConcavity*.74+trshaderCrease*.42)*
   trshaderSSGI);
 vec3 trshaderBounce=vec3(.014,.020,.018)*trshaderOcc*
   smoothstep(.18,.86,trshaderAround);
 trshaderC=max(trshaderC*(1.0-trshaderOcc*.38)+trshaderBounce,vec3(0.0));

 trshaderFragColor=vec4(clamp(trshaderC,0.0,1.0),1.0);
}
