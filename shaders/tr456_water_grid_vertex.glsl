#version 150

uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];

in vec4 aCoord;
in vec4 aNormal;
in vec4 aLight;
in vec4 aColor;

out vec3 gGridPos;
out vec3 gGridWorldPos;
out vec3 gGridNormal;
out vec3 gGridLight;
out vec3 gGridColor;
out vec2 gGridUv;

vec4 clipFromPos(vec3 p){
 return uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p),
                         dot(uViewMatrix[1].xyz,p),
                         dot(uViewMatrix[2].xyz,p),1.0);
}

void main(){
 vec4 coord=vec4(aCoord);
 vec3 normal=normalize(aNormal.xyz-127.0);
 vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[1],vec4(coord.xyz,1.0)),
             dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);
 gGridPos=p.xyz;
 gGridWorldPos=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);
 gGridNormal=normal;
 gGridLight=pow(aLight.xyz,vec3(2.2));
 gGridColor=pow(aColor.xyz,vec3(2.2));
 gGridUv=vec2(aLight.w,aColor.w);
 gl_Position=clipFromPos(p.xyz);
}
