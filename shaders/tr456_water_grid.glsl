#version 150

#ifndef TR456_WATER_GRID_OPACITY
#define TR456_WATER_GRID_OPACITY 0.50
#endif
#ifndef TR456_WATER_GRID_STRENGTH
#define TR456_WATER_GRID_STRENGTH 1.0
#endif
#ifndef TR456_WATER_GRID_FLOW_OPACITY
#define TR456_WATER_GRID_FLOW_OPACITY 0.42
#endif

in vec3 vGridWave;
in vec3 vGridColor;
in float vGridFog;
in float vGridFlow;

out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float fastPow2(float x){ return x*x; }

void main(){
 float slope=length(vGridWave.xy);
 float height=abs(vGridWave.z);
 float flow=sat(vGridFlow);
 float energy=sat(height*1.85+slope*10.0);
 float crest=smoothstep(.10,.66,height+slope*4.4);
 float trough=smoothstep(.07,.46,height);
 vec3 calmBody=mix(vec3(.020,.130,.150),vec3(.036,.205,.230),sat(vGridFog*.75+.20));
 vec3 flowBody=mix(vec3(.034,.050,.048),vec3(.070,.100,.090),sat(height*.85+slope*5.0));
 vec3 body=mix(calmBody,flowBody,flow);
 vec3 lip=mix(vec3(.46,.88,.96),vec3(.34,.52,.46),flow);
 vec3 foam=mix(vec3(.78,.96,.98),vec3(.54,.66,.56),flow);
 vec3 localLight=clamp(vGridColor*.48+vec3(.34),0.0,1.18);
 vec3 light=mix(vec3(.86),localLight,mix(.20,.30,flow));
 vec3 col=mix(body,lip,crest*mix(.70,.82,flow));
 col=mix(col,foam,fastPow2(crest)*mix(.45,.18,flow));
 col+=vec3(.050,.055,.048)*flow*smoothstep(.20,.82,slope*7.5+height*.8);
 col*=light;
 float alpha=(crest*.185+trough*.024+slope*.20)*
    mix(clamp(TR456_WATER_GRID_OPACITY,0.0,.82),
      clamp(TR456_WATER_GRID_FLOW_OPACITY,0.0,.82),flow);
 alpha*=smoothstep(.035,.16,energy)*mix(.55,1.0,vGridFog);
 fragColor=vec4(col,alpha);
}
