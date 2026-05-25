#version 150
uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uTrWaterSyntheticInfo;
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;
out vec2 vFlowUv;
out vec2 vOrigUv;
out vec2 vFlowDir;
out vec3 vWorldPos;
out vec3 vLight;
out float vLayer;
out float vFog;
out float vSpeed;
out float vViewTop;
void main(){
 vec4 coord=vec4(aCoord);
 vec4 normal=vec4(aNormal);
 vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),dot(uModelMatrix[1],vec4(coord.xyz,1.0)),dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);
 float flowSign=mix(-1.0,1.0,step(0.0,TR456_WATER_FLOW_DIRECTION_SIGN));
 vec2 flow=(length(uParams.xy)>0.00001?normalize(uParams.xy):normalize(vec2(0.92,0.38)))*flowSign;
 vec2 side=vec2(-flow.y,flow.x);
 vOrigUv=vec2(aLight.w,aColor.w)+uParams.xy*uTrWaterSyntheticInfo.w*
   clamp(TR456_WATER_FLOW_ORIGINAL_SYNC,0.0,1.0);
 vec3 worldBase=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vec2 flowUv=vec2(dot(worldBase.xz,flow),dot(worldBase.xz,side))*0.00072;
 float flowVertexStrength=clamp(TR456_WATER_FLOW_VERTEX_STRENGTH,0.0,1.25);
 if(flowVertexStrength>0.0001){
  float flowLen=max(length(uParams.xy),0.22);
  float flowTime=uTrWaterSyntheticInfo.w*clamp(TR456_WATER_FLOW_SPEED,0.20,3.0)*(0.74+flowLen*0.22);
  vec2 wavePos=vec2(dot(worldBase.xz,flow),dot(worldBase.xz,side))*0.0084;
  float lane=sin(wavePos.y*1.55-flowTime*0.11)*0.32+
    sin(wavePos.y*3.10+flowTime*0.08)*0.16;
  float broad=sin(wavePos.x*2.50+lane-flowTime*0.28);
  float crossRoll=sin(wavePos.y*2.20+wavePos.x*0.32-flowTime*0.18);
  float viscousBreath=sin(wavePos.x*1.20+
    sin(wavePos.y*1.05-flowTime*0.10)*0.34-flowTime*0.16);
  float smallLift=sin(wavePos.x*6.70-wavePos.y*0.55-flowTime*0.58);
  float lift=(broad*0.48+crossRoll*0.22+viscousBreath*0.18+smallLift*0.12)*
    flowVertexStrength*14.0*clamp(TR456_WATER_FLOW_STRENGTH,0.0,1.5);
  p.y+=lift;
 }
 vec3 world=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vFlowUv=flowUv;
 vFlowDir=flow;
 vWorldPos=world;
 vLight=clamp(pow(aLight.xyz,vec3(2.2))+pow(aColor.xyz,vec3(2.2))*0.35,0.0,1.6);
 vLayer=normal.w;
 vSpeed=max(length(uParams.xy),0.22);
 vViewTop=clamp(abs(normalize(-p.xyz+vec3(0.0,0.0001,0.0)).y),0.0,1.0);
 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),dot(uViewMatrix[1].xyz,p.xyz),dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
