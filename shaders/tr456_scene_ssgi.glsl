vec3 trSceneApplySSGI(vec3 trSceneColor, TrScenePostFrame trSceneF) {
 float trSceneStrength=trSceneF.ssgi;
 if(trSceneStrength<=0.001) return trSceneColor;

 vec2 trSceneStep=trSceneF.texel*(1.0+trSceneF.ssgiRadius*1.55);
 float trSceneY=trSceneLuma(trSceneColor);

 vec3 trSceneL=trSceneSampleScene(trSceneF.uv-vec2(trSceneStep.x,0.0));
 vec3 trSceneR=trSceneSampleScene(trSceneF.uv+vec2(trSceneStep.x,0.0));
 vec3 trSceneD=trSceneSampleScene(trSceneF.uv-vec2(0.0,trSceneStep.y));
 vec3 trSceneU=trSceneSampleScene(trSceneF.uv+vec2(0.0,trSceneStep.y));
 vec3 trSceneLD=trSceneSampleScene(trSceneF.uv-trSceneStep);
 vec3 trSceneRU=trSceneSampleScene(trSceneF.uv+trSceneStep);
 vec3 trSceneLU=trSceneSampleScene(trSceneF.uv+vec2(-trSceneStep.x,trSceneStep.y));
 vec3 trSceneRD=trSceneSampleScene(trSceneF.uv+vec2(trSceneStep.x,-trSceneStep.y));

 vec3 trSceneNearAvg=(trSceneL+trSceneR+trSceneD+trSceneU+
   trSceneLD+trSceneRU+trSceneLU+trSceneRD)*0.125;
 float trSceneNearY=trSceneLuma(trSceneNearAvg);
 float trSceneEdge=max(abs(trSceneLuma(trSceneR)-trSceneLuma(trSceneL)),
   abs(trSceneLuma(trSceneU)-trSceneLuma(trSceneD)));
 float trSceneCavity=trSceneSat((trSceneNearY-trSceneY)*3.2);
 float trSceneCrease=smoothstep(0.018,0.145,trSceneEdge)*
   (1.0-smoothstep(0.72,0.98,trSceneY));
 float trSceneFlatGuard=1.0-smoothstep(0.0,0.018,
   abs(trSceneNearY-trSceneY)+trSceneEdge);
 float trSceneToneGuard=smoothstep(0.04,0.22,trSceneY)*
   (1.0-smoothstep(0.88,1.0,trSceneY));
 float trSceneOcc=trSceneSat((trSceneCavity*0.82+trSceneCrease*0.18)*
   trSceneStrength);
 trSceneOcc*=trSceneToneGuard*(1.0-trSceneFlatGuard*0.78);

 vec3 trSceneOut=trSceneColor*(1.0-trSceneOcc*0.34);
 return max(trSceneOut,vec3(0.0));
}
