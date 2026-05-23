#include "tr456_lab_common.h"

static GLuint tr456_lab_compile_shader(GLenum stage, const char *label,
                                       const char *text) {
  PFNGLCREATESHADER create=real_create_shader();
  PFNGLSHADERSOURCE source=real_shader_source("glShaderSource");
  PFNGLCOMPILESHADER compile=real_compile_shader();
  if(!create || !source || !compile || !text) return 0;
  GLuint shader=create(stage);
  if(!shader) return 0;
  GLint len=(GLint)strlen(text);
  const GLchar *src=text;
  source(shader,1,&src,&len);
  compile(shader);
  GLint ok=1;
  PFNGLGETSHADERIV getiv=real_get_shader_iv();
  if(getiv) getiv(shader,GL_COMPILE_STATUS,&ok);
  if(ok) return shader;

  char logbuf[1024];
  GLsizei got=0;
  logbuf[0]=0;
  PFNGLGETSHADERINFOLOG getlog=real_get_shader_info_log();
  if(getlog)
    getlog(shader,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
  logbuf[sizeof(logbuf)-1]=0;
  char msg[1200];
  snprintf(msg,sizeof(msg),"lab shader compile failed stage=%s log=%s",
    label ? label : "shader",logbuf);
  log_line(msg);
  PFNGLDELETESHADER del=real_delete_shader();
  if(del) del(shader);
  return 0;
}

static int tr456_lab_read_mat4(const char *name, GLfloat out[16]) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv ||
     !g_current_program) return 0;
  GLint loc=gl->get_uniform_location(g_current_program,name);
  if(loc<0) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int tr456_lab_read_vec4_array(const char *base, int count,
                                     GLfloat *out) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv ||
     !g_current_program) return 0;
  for(int i=0;i<count;i++) {
    char name[48];
    snprintf(name,sizeof(name),"%s[%d]",base,i);
    GLint loc=gl->get_uniform_location(g_current_program,name);
    if(loc<0) return 0;
    gl->get_uniform_fv(g_current_program,loc,out+i*4);
  }
  return 1;
}

static int tr456_lab_read_matrix_or_vec4_array(const char *base,
                                               GLfloat out[16]) {
  if(tr456_lab_read_vec4_array(base,4,out))
    return 1;
  return tr456_lab_read_mat4(base,out);
}

static int tr456_lab_has_matrix_or_vec4_array(const char *base) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !g_current_program || !base)
    return 0;
  char name[48];
  snprintf(name,sizeof(name),"%s[0]",base);
  if(gl->get_uniform_location(g_current_program,name)>=0)
    return 1;
  return gl->get_uniform_location(g_current_program,base)>=0;
}
