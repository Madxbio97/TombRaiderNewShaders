#version 150

out vec2 vTrScenePostUv;

void main(){
 vec2 trshaderP;
 if(gl_VertexID==0) {
  trshaderP=vec2(-1.0,-1.0);
 } else if(gl_VertexID==1) {
  trshaderP=vec2(3.0,-1.0);
 } else {
  trshaderP=vec2(-1.0,3.0);
 }
 vTrScenePostUv=trshaderP*0.5+0.5;
 gl_Position=vec4(trshaderP,0.0,1.0);
}
