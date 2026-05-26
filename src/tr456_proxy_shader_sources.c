#include <stdlib.h>
#include <string.h>

#include "tr456_proxy_shader_sources.h"

char *configured_shader(const char *file, const char *label);

static char *trshader_dup_text(const char *text) {
  if(!text) return 0;
  size_t n=strlen(text);
  char *out=(char*)malloc(n+1);
  if(!out) return 0;
  memcpy(out,text,n+1);
  return out;
}

char *synthetic_surface_vertex_shader(void) {
  return configured_shader("tr456_water_synthetic_vertex.glsl",
    "synthetic water vertex shader");
}

char *synthetic_surface_shader(void) {
  return configured_shader("tr456_water_synthetic.glsl",
    "synthetic water fragment shader");
}

char *flow_lite_vertex_shader(void) {
  return configured_shader("tr456_water_flow_lite_vertex.glsl",
    "FlowLite vertex shader");
}

char *flow_lite_fragment_shader(void) {
  return configured_shader("tr456_water_flow_lite.glsl",
    "FlowLite fragment shader");
}

char *swap_debug_vertex_shader(void) {
  return trshader_dup_text(
    "#version 150\n"
    "out vec3 vDbgColor;\n"
    "void main(){\n"
    " int i=gl_VertexID%6;\n"
    " vec2 p=vec2(-0.96,-0.96);\n"
    " if(i==1) p=vec2(-0.36,-0.96);\n"
    " else if(i==2) p=vec2(-0.96,-0.80);\n"
    " else if(i==3) p=vec2(-0.36,-0.96);\n"
    " else if(i==4) p=vec2(-0.36,-0.80);\n"
    " else if(i==5) p=vec2(-0.96,-0.80);\n"
    " float t=step(-0.66,p.x);\n"
    " vDbgColor=mix(vec3(0.0,1.0,0.95),vec3(1.0,0.0,0.85),t);\n"
    " gl_Position=vec4(p,0.0,1.0);\n"
    "}\n");
}

char *swap_debug_fragment_shader(void) {
  return trshader_dup_text(
    "#version 150\n"
    "in vec3 vDbgColor;\n"
    "out vec4 trshaderFragColor;\n"
    "void main(){ trshaderFragColor=vec4(vDbgColor,1.0); }\n");
}
