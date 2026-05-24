#version 150
uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;
out vec2 vFlowUv;
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
 vec2 flow=length(uParams.xy)>0.00001?normalize(uParams.xy):normalize(vec2(0.92,0.38));
 vec2 side=vec2(-flow.y,flow.x);
 vec3 world=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 vFlowUv=vec2(dot(world.xz,flow),dot(world.xz,side))*0.00072;
 vFlowDir=flow;
 vWorldPos=world;
 vLight=clamp(pow(aLight.xyz,vec3(2.2))+pow(aColor.xyz,vec3(2.2))*0.35,0.0,1.6);
 vLayer=normal.w;
 vSpeed=max(length(uParams.xy),0.22);
 vViewTop=clamp(abs(normalize(-p.xyz+vec3(0.0,0.0001,0.0)).y),0.0,1.0);
 vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);
 gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),dot(uViewMatrix[1].xyz,p.xyz),dot(uViewMatrix[2].xyz,p.xyz),p.w);
}
