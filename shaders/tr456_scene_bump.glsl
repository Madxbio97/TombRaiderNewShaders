vec3 trSceneApplyBump(vec3 trSceneColor, TrScenePostFrame trSceneF) {
 float trSceneStrength=trSceneF.bump;
 if(trSceneStrength<=0.001) return trSceneColor;

 vec2 trScenePx=trSceneF.texel*trSceneF.bumpScale;
 float trSceneY=trSceneLuma(trSceneColor);
 float trSceneL=trSceneSampleLuma(trSceneF.uv-vec2(trScenePx.x,0.0));
 float trSceneR=trSceneSampleLuma(trSceneF.uv+vec2(trScenePx.x,0.0));
 float trSceneD=trSceneSampleLuma(trSceneF.uv-vec2(0.0,trScenePx.y));
 float trSceneU=trSceneSampleLuma(trSceneF.uv+vec2(0.0,trScenePx.y));

 vec2 trSceneFineGrad=vec2(trSceneR-trSceneL,trSceneU-trSceneD);
 vec2 trSceneGrad=trSceneFineGrad;
 float trSceneEdge=max(abs(trSceneGrad.x),abs(trSceneGrad.y));
 float trSceneLocalAvg=(trSceneL+trSceneR+trSceneD+trSceneU)*0.25;
 float trSceneDetail=abs(trSceneY-trSceneLocalAvg);
 float trSceneMaterialMask=smoothstep(0.006,0.095,trSceneEdge+trSceneDetail*1.7);
 float trSceneHardEdgeMask=1.0-smoothstep(0.22,0.55,trSceneEdge);
 float trSceneToneMask=smoothstep(0.025,0.18,trSceneY)*
   (1.0-smoothstep(0.94,1.0,trSceneY));
 float trSceneMask=trSceneMaterialMask*trSceneHardEdgeMask*trSceneToneMask;

 vec2 trSceneWarp=-trSceneGrad*trSceneF.texel*
   (5.5+12.0*trSceneF.bumpScale)*trSceneStrength*trSceneHardEdgeMask;
 vec3 trSceneWarped=trSceneSampleScene(trSceneF.uv+trSceneWarp);
 vec3 trSceneN=normalize(vec3(-trSceneGrad*(4.2+trSceneF.bumpScale*4.0),1.0));
 float trSceneLambert=dot(trSceneN,normalize(vec3(-0.34,0.46,0.82)));
 float trSceneShade=(trSceneLambert-0.86)*0.55;
 float trSceneSpec=pow(trSceneSat(trSceneLambert),16.0)*
   smoothstep(0.015,0.18,trSceneDetail);
 vec3 trSceneOut=mix(trSceneColor,trSceneWarped,
   trSceneSat(0.10+trSceneStrength*0.22)*trSceneMask);
 trSceneOut+=trSceneColor*trSceneShade*trSceneStrength*trSceneMask;
 trSceneOut+=vec3(0.020,0.023,0.021)*trSceneSpec*trSceneStrength*trSceneMask;
 return max(trSceneOut,vec3(0.0));
}
