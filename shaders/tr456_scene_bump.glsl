vec3 trSceneApplyBump(vec3 trSceneColor, TrScenePostFrame trSceneF) {
 float trSceneStrength=trSceneF.bump;
 if(trSceneStrength<=0.001) return trSceneColor;

 vec2 trScenePx=trSceneF.texel*trSceneF.bumpScale;
 float trSceneY=trSceneLuma(trSceneColor);
 float trSceneL=trSceneSampleLuma(trSceneF.uv-vec2(trScenePx.x,0.0));
 float trSceneR=trSceneSampleLuma(trSceneF.uv+vec2(trScenePx.x,0.0));
 float trSceneD=trSceneSampleLuma(trSceneF.uv-vec2(0.0,trScenePx.y));
 float trSceneU=trSceneSampleLuma(trSceneF.uv+vec2(0.0,trScenePx.y));

 vec2 trSceneGrad=vec2(trSceneR-trSceneL,trSceneU-trSceneD);
 float trSceneEdge=max(abs(trSceneGrad.x),abs(trSceneGrad.y));
 float trSceneLocalAvg=(trSceneL+trSceneR+trSceneD+trSceneU)*0.25;
 float trSceneHi=trSceneY-trSceneLocalAvg;
 float trSceneDetail=abs(trSceneHi);
 float trSceneMaterialMask=smoothstep(0.010,0.085,trSceneEdge+trSceneDetail*1.35);
 float trSceneHardEdgeMask=1.0-smoothstep(0.18,0.44,trSceneEdge);
 float trSceneToneMask=smoothstep(0.025,0.18,trSceneY)*
   (1.0-smoothstep(0.94,1.0,trSceneY));
 float trSceneMask=trSceneMaterialMask*trSceneHardEdgeMask*trSceneToneMask;

 vec3 trSceneLight=normalize(vec3(-0.38,0.42,0.82));
 vec3 trSceneN=normalize(vec3(-trSceneGrad*(5.0+trSceneF.bumpScale*4.0),1.0));
 float trSceneLambert=dot(trSceneN,trSceneLight);
 float trSceneFlatLambert=trSceneLight.z;
 float trSceneDirectional=clamp(trSceneLambert-trSceneFlatLambert,
   -0.18,0.16);
 float trSceneRelief=clamp(trSceneDirectional*0.72+trSceneHi*0.34,
   -0.10,0.10);
 vec3 trSceneOut=trSceneColor*
   (1.0+trSceneRelief*trSceneStrength*trSceneMask);
 return max(trSceneOut,vec3(0.0));
}
