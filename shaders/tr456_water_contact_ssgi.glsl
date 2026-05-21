#if TR456_WATER_CONTACT_SSGI_ENABLED
float trshaderContactSSGIMask(vec3 trshaderW, vec2 trshaderScreen,
                              vec3 trshaderNormal, float trshaderEnergy){
 float trshaderStrength=clamp(TR456_WATER_CONTACT_SSGI_STRENGTH,0.0,1.5);
 if(trshaderStrength<=.001) return 0.0;
 float trshaderContact=0.0;
 for(int trshaderI=0;trshaderI<TR456_WATER_CONTACT_SSGI_MAX;trshaderI++){
  if(trshaderI>=TR456_WATER_CONTACT_MAX_ACTIVE) break;
  vec4 trshaderC=uContacts[trshaderI];
  float trshaderOn=step(.001,dot(abs(trshaderC),vec4(1.0)));
  if(trshaderOn<=.001) continue;
  float trshaderRadius=trshaderContactRadius(trshaderC)*
    clamp(TR456_WATER_CONTACT_SSGI_RADIUS,.35,2.4);
  vec2 trshaderD=trshaderW.xz-trshaderC.xz;
  float trshaderDist=length(trshaderD)+.001;
  float trshaderVertical=1.0-smoothstep(70.0,560.0,abs(trshaderW.y-trshaderC.y));
  float trshaderAge=mod(abs(trshaderC.w),512.0);
  float trshaderAgeFade=1.0-smoothstep(132.0,260.0,trshaderAge);
  float trshaderCore=1.0-smoothstep(trshaderRadius*.035,trshaderRadius*1.10,trshaderDist);
  float trshaderPenumbra=1.0-smoothstep(trshaderRadius*.30,trshaderRadius*1.72,trshaderDist);
  float trshaderMotion=trshaderSat(uContactMotion[trshaderI].w*.055);
  trshaderContact=max(trshaderContact,
    (trshaderCore*.78+trshaderPenumbra*.26)*trshaderVertical*
    trshaderAgeFade*trshaderOn*(.82+.18*trshaderMotion));
 }
 if(trshaderContact<=.001) return 0.0;
 vec2 trshaderInv=max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));
 vec2 trshaderDir=normalize(vec2(trshaderNormal.x,-trshaderNormal.z)+vec2(.0001,.0003));
 vec2 trshaderSide=vec2(-trshaderDir.y,trshaderDir.x);
 vec2 trshaderS=clamp(trshaderScreen,vec2(.001),vec2(.999));
 float trshaderL0=trshaderLuma(texture(uTrWaterScene,trshaderS).rgb);
 float trshaderL1=trshaderLuma(texture(uTrWaterScene,
   clamp(trshaderS+trshaderDir*trshaderInv*3.5,vec2(.001),vec2(.999))).rgb);
 float trshaderL2=trshaderLuma(texture(uTrWaterScene,
   clamp(trshaderS+trshaderSide*trshaderInv*2.5,vec2(.001),vec2(.999))).rgb);
 float trshaderEdge=max(abs(trshaderL1-trshaderL0),abs(trshaderL2-trshaderL0));
 float trshaderScreenOcc=smoothstep(.018,.145,trshaderEdge)*.55+
   (1.0-smoothstep(.18,.70,trshaderL0))*.30+.36;
 return trshaderSat(trshaderContact*trshaderStrength*trshaderScreenOcc*
   (.82+.32*trshaderSat(trshaderEnergy)));
}

vec3 trshaderApplyContactSSGI(vec3 trshaderCol, float trshaderMask){
 float trshaderM=trshaderSat(trshaderMask);
 vec3 trshaderBounce=vec3(.020,.040,.038)*trshaderM;
 return max(trshaderCol*(1.0-trshaderM*.38)+trshaderBounce,vec3(0.0));
}
#else
float trshaderContactSSGIMask(vec3 trshaderW, vec2 trshaderScreen,
                              vec3 trshaderNormal, float trshaderEnergy){
 return 0.0;
}

vec3 trshaderApplyContactSSGI(vec3 trshaderCol, float trshaderMask){
 return trshaderCol;
}
#endif
