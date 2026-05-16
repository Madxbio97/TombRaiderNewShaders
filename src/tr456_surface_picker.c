#include "tr456_surface_picker.h"

typedef struct {
  GLuint program;
  GLint attr_coord;
  GLint loc_proj;
  GLint loc_model;
  GLint loc_view;
  GLint loc_color;
  int ready;
  int failed;
} Tr456SurfacePickerProgram;

typedef struct {
  int loaded;
  int enabled;
  int water_only;
  int selected;
  int last_frame_candidates;
  int frame_candidates;
  int page_up_down;
  int page_down_down;
  unsigned int poll_frame;
  unsigned int selected_log_frame;
  float opacity;
  Tr456SurfacePickerProgram programs[16];
} Tr456SurfacePickerState;

typedef struct {
  GLint old_program;
  GLint old_blend;
  GLint old_depth;
  GLint old_cull;
  GLint old_depth_mask;
  GLint old_depth_func;
  GLint old_blend_func[4];
} Tr456SurfacePickerDrawState;

static Tr456SurfacePickerState g_surface_picker;

static void tr456_surface_picker_load_config(void) {
  if(g_surface_picker.loaded) return;
  g_surface_picker.enabled=ini_int("SurfacePicker",0);
  g_surface_picker.water_only=ini_int("SurfacePickerWaterOnly",0);
  g_surface_picker.selected=ini_int("SurfacePickerStart",0);
  if(g_surface_picker.selected<0) g_surface_picker.selected=0;
  g_surface_picker.opacity=ini_float("SurfacePickerOpacity",0.58f);
  if(g_surface_picker.opacity<0.10f) g_surface_picker.opacity=0.10f;
  if(g_surface_picker.opacity>1.0f) g_surface_picker.opacity=1.0f;
  g_surface_picker.loaded=1;
}

static int tr456_surface_picker_triangle_mode(GLenum mode) {
  return mode==GL_TRIANGLES || mode==GL_TRIANGLE_STRIP ||
    mode==GL_TRIANGLE_FAN;
}

static int tr456_surface_picker_current_compatible(GLint *attr_out) {
  PFNGLGETATTRIBLOCATION get_attr=real_get_attrib_location();
  CaptureGL *gl=capture_gl();
  if(!get_attr || !gl || !gl->get_uniform_location || !g_current_program)
    return 0;
  GLint attr=get_attr(g_current_program,"aCoord");
  if(attr<0 || attr>15) return 0;
  if(gl->get_uniform_location(g_current_program,"uProjMatrix")<0) return 0;
  if(gl->get_uniform_location(g_current_program,"uModelMatrix[0]")<0) return 0;
  if(gl->get_uniform_location(g_current_program,"uViewMatrix[0]")<0) return 0;
  if(attr_out) *attr_out=attr;
  return 1;
}

static int tr456_surface_picker_is_candidate(GLenum mode, GLsizei count,
                                             int count_known, GLint *attr_out) {
  tr456_surface_picker_load_config();
  if(!g_surface_picker.enabled) return 0;
  if(!tr456_surface_picker_triangle_mode(mode)) return 0;
  if(count_known && count<=0) return 0;
  if(g_surface_picker.water_only &&
     !is_tracked_water_shader_type(g_current_program_type)) return 0;
  return tr456_surface_picker_current_compatible(attr_out);
}

static int tr456_surface_picker_note_candidate(const char *call, GLenum mode,
                                               GLsizei count, int count_known,
                                               GLint *attr_out) {
  GLint attr=-1;
  if(!tr456_surface_picker_is_candidate(mode,count,count_known,&attr))
    return -1;
  int index=g_surface_picker.frame_candidates++;
  if(attr_out) *attr_out=attr;
  if(index==g_surface_picker.selected && diag_is_active() &&
     g_surface_picker.selected_log_frame!=g_frame_index) {
    char msg[320];
    snprintf(msg,sizeof(msg),
      "surface picker selected session=%d frame=%u index=%d call=%s program=%u type=%s mode=0x%X count=%d attr=%d lastFrameCandidates=%d",
      g_diag_session,g_frame_index,index,call ? call : "draw",
      g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,(int)attr,
      g_surface_picker.last_frame_candidates);
    log_line(msg);
    diag_consume_line();
    g_surface_picker.selected_log_frame=g_frame_index;
  }
  return index;
}

static void tr456_surface_picker_log_selection(const char *action) {
  char msg[192];
  snprintf(msg,sizeof(msg),
    "surface picker %s selected=%d lastFrameCandidates=%d frame=%u waterOnly=%d",
    action ? action : "select",g_surface_picker.selected,
    g_surface_picker.last_frame_candidates,g_frame_index,
    g_surface_picker.water_only);
  log_line(msg);
}

static void tr456_surface_picker_poll_hotkeys(const char *where) {
  (void)where;
  tr456_surface_picker_load_config();
  if(!g_surface_picker.enabled) return;
  if(g_surface_picker.poll_frame==g_frame_index) return;
  g_surface_picker.poll_frame=g_frame_index;

  int prev=(GetAsyncKeyState(VK_PRIOR)&0x8000)!=0;
  int next=(GetAsyncKeyState(VK_NEXT)&0x8000)!=0;
  int max_count=g_surface_picker.last_frame_candidates>0 ?
    g_surface_picker.last_frame_candidates : g_surface_picker.frame_candidates;

  if(prev && !g_surface_picker.page_up_down) {
    if(max_count>0) {
      g_surface_picker.selected--;
      if(g_surface_picker.selected<0)
        g_surface_picker.selected=max_count-1;
    } else {
      g_surface_picker.selected=0;
    }
    tr456_surface_picker_log_selection("previous");
  }

  if(next && !g_surface_picker.page_down_down) {
    if(max_count>0)
      g_surface_picker.selected=(g_surface_picker.selected+1)%max_count;
    else
      g_surface_picker.selected++;
    tr456_surface_picker_log_selection("next");
  }

  g_surface_picker.page_up_down=prev;
  g_surface_picker.page_down_down=next;
}

static void tr456_surface_picker_end_frame(void) {
  tr456_surface_picker_load_config();
  if(!g_surface_picker.enabled) {
    g_surface_picker.frame_candidates=0;
    return;
  }
  g_surface_picker.last_frame_candidates=g_surface_picker.frame_candidates;
  if(g_surface_picker.last_frame_candidates>0 &&
     g_surface_picker.selected>=g_surface_picker.last_frame_candidates)
    g_surface_picker.selected=g_surface_picker.last_frame_candidates-1;
  if(g_surface_picker.selected<0) g_surface_picker.selected=0;
  g_surface_picker.frame_candidates=0;
}

static void tr456_surface_picker_diag_begin(const char *where) {
  tr456_surface_picker_load_config();
  char msg[320];
  snprintf(msg,sizeof(msg),
    "surface picker diag session=%d frame=%u where=%s enabled=%d waterOnly=%d selected=%d lastFrameCandidates=%d opacity=%.2f",
    g_diag_session,g_frame_index,where ? where : "unknown",
    g_surface_picker.enabled,g_surface_picker.water_only,
    g_surface_picker.selected,g_surface_picker.last_frame_candidates,
    (double)g_surface_picker.opacity);
  log_line(msg);
}

static GLuint tr456_surface_picker_compile_shader(GLenum stage,
                                                  const char *label,
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
  snprintf(msg,sizeof(msg),"surface picker shader compile failed stage=%s log=%s",
    label ? label : "shader",logbuf);
  log_line(msg);
  PFNGLDELETESHADER del=real_delete_shader();
  if(del) del(shader);
  return 0;
}

static Tr456SurfacePickerProgram *tr456_surface_picker_program_for_attr(
  GLint attr_coord) {
  if(attr_coord<0 || attr_coord>=16) return 0;
  Tr456SurfacePickerProgram *p=&g_surface_picker.programs[attr_coord];
  if(p->ready) return p;
  if(p->failed) return 0;

  PFNGLCREATEPROGRAM create=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  PFNGLBINDATTRIBLOCATION bind_attr=real_bind_attrib_location();
  CaptureGL *gl=capture_gl();
  if(!create || !attach || !link || !bind_attr || !gl ||
     !gl->get_uniform_location) {
    p->failed=1;
    return 0;
  }

  static const char *vs_text=
    "#version 150\n"
    "uniform mat4 uProjMatrix;\n"
    "uniform vec4 uViewMatrix[4];\n"
    "uniform vec4 uModelMatrix[4];\n"
    "in vec4 aCoord;\n"
    "void main(){\n"
    " vec4 trshaderSrc=vec4(aCoord.xyz,1.0);\n"
    " vec3 trshaderW=vec3(dot(uModelMatrix[0],trshaderSrc),dot(uModelMatrix[1],trshaderSrc),dot(uModelMatrix[2],trshaderSrc));\n"
    " gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,trshaderW),dot(uViewMatrix[1].xyz,trshaderW),dot(uViewMatrix[2].xyz,trshaderW),1.0);\n"
    "}\n";
  static const char *fs_text=
    "#version 150\n"
    "uniform vec4 uPickerColor;\n"
    "out vec4 trshaderFragColor;\n"
    "void main(){ trshaderFragColor=uPickerColor; }\n";

  GLuint vs=tr456_surface_picker_compile_shader(GL_VERTEX_SHADER,
    "picker vertex",vs_text);
  GLuint fs=tr456_surface_picker_compile_shader(GL_FRAGMENT_SHADER,
    "picker fragment",fs_text);
  if(!vs || !fs) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) {
      if(vs) del(vs);
      if(fs) del(fs);
    }
    p->failed=1;
    return 0;
  }

  GLuint program=create();
  if(!program) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) {
      del(vs);
      del(fs);
    }
    p->failed=1;
    return 0;
  }
  bind_attr(program,(GLuint)attr_coord,"aCoord");
  attach(program,vs);
  attach(program,fs);
  link(program);
  PFNGLDELETESHADER del_shader=real_delete_shader();
  if(del_shader) {
    del_shader(vs);
    del_shader(fs);
  }

  GLint ok=1;
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(getiv) getiv(program,GL_LINK_STATUS,&ok);
  if(!ok) {
    char logbuf[1024];
    GLsizei got=0;
    logbuf[0]=0;
    PFNGLGETPROGRAMINFOLOG getlog=real_get_program_info_log();
    if(getlog)
      getlog(program,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
    logbuf[sizeof(logbuf)-1]=0;
    char msg[1200];
    snprintf(msg,sizeof(msg),"surface picker link failed attr=%d log=%s",
      (int)attr_coord,logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    p->failed=1;
    return 0;
  }

  p->program=program;
  p->attr_coord=attr_coord;
  p->loc_proj=gl->get_uniform_location(program,"uProjMatrix");
  p->loc_model=gl->get_uniform_location(program,"uModelMatrix[0]");
  p->loc_view=gl->get_uniform_location(program,"uViewMatrix[0]");
  p->loc_color=gl->get_uniform_location(program,"uPickerColor");
  p->ready=1;
  {
    char msg[160];
    snprintf(msg,sizeof(msg),
      "surface picker linked program=%u attrCoord=%d",
      program,(int)attr_coord);
    log_line(msg);
  }
  return p;
}

static int tr456_surface_picker_read_mat4(const char *name, GLfloat out[16]) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv ||
     !g_current_program) return 0;
  GLint loc=gl->get_uniform_location(g_current_program,name);
  if(loc<0) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int tr456_surface_picker_read_vec4_array(const char *base, int count,
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

static int tr456_surface_picker_setup_uniforms(
  const Tr456SurfacePickerProgram *program) {
  CaptureGL *gl=capture_gl();
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();
  if(!program || !gl || !gl->uniform_4fv || !matrix4) return 0;

  GLfloat proj[16];
  GLfloat model[16];
  GLfloat view[16];
  if(!tr456_surface_picker_read_mat4("uProjMatrix",proj)) return 0;
  if(!tr456_surface_picker_read_vec4_array("uModelMatrix",4,model)) return 0;
  if(!tr456_surface_picker_read_vec4_array("uViewMatrix",4,view)) return 0;
  if(program->loc_proj>=0)
    matrix4(program->loc_proj,1,GL_FALSE,proj);
  if(program->loc_model>=0)
    gl->uniform_4fv(program->loc_model,4,model);
  if(program->loc_view>=0)
    gl->uniform_4fv(program->loc_view,4,view);
  if(program->loc_color>=0 && gl->uniform_4f)
    gl->uniform_4f(program->loc_color,0.0f,1.0f,0.08f,
      g_surface_picker.opacity);
  return 1;
}

static void tr456_surface_picker_begin_state(
  Tr456SurfacePickerDrawState *state) {
  CaptureGL *gl=capture_gl();
  memset(state,0,sizeof(*state));
  state->old_program=(GLint)g_current_program;
  state->old_depth_mask=1;
  state->old_depth_func=GL_LEQUAL;
  state->old_blend_func[0]=GL_SRC_ALPHA;
  state->old_blend_func[1]=GL_ONE_MINUS_SRC_ALPHA;
  state->old_blend_func[2]=GL_ONE;
  state->old_blend_func[3]=GL_ONE_MINUS_SRC_ALPHA;
  if(gl && gl->get_integer) {
    gl->get_integer(GL_CURRENT_PROGRAM,&state->old_program);
    gl->get_integer(GL_BLEND,&state->old_blend);
    gl->get_integer(GL_DEPTH_TEST,&state->old_depth);
    gl->get_integer(GL_CULL_FACE,&state->old_cull);
    gl->get_integer(GL_DEPTH_WRITEMASK,&state->old_depth_mask);
    gl->get_integer(GL_DEPTH_FUNC,&state->old_depth_func);
    gl->get_integer(GL_BLEND_SRC_RGB,&state->old_blend_func[0]);
    gl->get_integer(GL_BLEND_DST_RGB,&state->old_blend_func[1]);
    gl->get_integer(GL_BLEND_SRC_ALPHA,&state->old_blend_func[2]);
    gl->get_integer(GL_BLEND_DST_ALPHA,&state->old_blend_func[3]);
  }

  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(disable) disable(GL_CULL_FACE);
  if(enable) {
    enable(GL_BLEND);
    enable(GL_DEPTH_TEST);
  }
  if(depth_func) depth_func(GL_LEQUAL);
  if(depth_mask) depth_mask(GL_FALSE);
  if(blend_func)
    blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,
      GL_ONE_MINUS_SRC_ALPHA);
}

static void tr456_surface_picker_end_state(
  const Tr456SurfacePickerDrawState *state) {
  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(blend_func)
    blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[2],
      (GLenum)state->old_blend_func[3]);
  if(depth_mask) depth_mask((GLboolean)(state->old_depth_mask ? 1 : 0));
  if(depth_func) depth_func((GLenum)state->old_depth_func);
  if(state->old_cull) { if(enable) enable(GL_CULL_FACE); }
  else { if(disable) disable(GL_CULL_FACE); }
  if(state->old_depth) { if(enable) enable(GL_DEPTH_TEST); }
  else { if(disable) disable(GL_DEPTH_TEST); }
  if(state->old_blend) { if(enable) enable(GL_BLEND); }
  else { if(disable) disable(GL_BLEND); }
  if(use_program) use_program((GLuint)state->old_program);
}

static int tr456_surface_picker_begin_draw(GLint attr_coord,
                                           Tr456SurfacePickerDrawState *state) {
  Tr456SurfacePickerProgram *program=
    tr456_surface_picker_program_for_attr(attr_coord);
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!program || !use_program) return 0;
  tr456_surface_picker_begin_state(state);
  use_program(program->program);
  if(!tr456_surface_picker_setup_uniforms(program)) {
    tr456_surface_picker_end_state(state);
    return 0;
  }
  return 1;
}

static int tr456_surface_picker_selected(const char *call, GLenum mode,
                                         GLsizei count, int count_known,
                                         GLint *attr_out) {
  GLint attr=-1;
  int index=tr456_surface_picker_note_candidate(call,mode,count,count_known,
    &attr);
  if(index<0 || index!=g_surface_picker.selected) return 0;
  if(attr_out) *attr_out=attr;
  return 1;
}

static void tr456_surface_picker_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, GLint first, GLsizei count) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,first,count);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_range_elements(const char *call,
  PFNGLDRAWRANGEELEMENTS draw, GLenum mode, GLuint start, GLuint end,
  GLsizei count, GLenum type, const void *indices) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,start,end,count,type,indices);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_base_vertex(const char *call,
  PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLint base_vertex) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices,base_vertex);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_range_elements_base_vertex(
  const char *call, PFNGLDRAWRANGEELEMENTSBASEVERTEX draw, GLenum mode,
  GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices,
  GLint base_vertex) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,start,end,count,type,indices,base_vertex);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_arrays_instanced(const char *call,
  PFNGLDRAWARRAYSINSTANCED draw, GLenum mode, GLint first, GLsizei count,
  GLsizei instance_count) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,first,count,instance_count);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_instanced(const char *call,
  PFNGLDRAWELEMENTSINSTANCED draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLsizei instance_count) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices,instance_count);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_instanced_base_vertex(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLint base_vertex) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_arrays_instanced_base_instance(
  const char *call, PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLint first, GLsizei count, GLsizei instance_count, GLuint base_instance) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,first,count,instance_count,base_instance);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_instanced_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLuint base_instance) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices,instance_count,base_instance);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_instanced_base_vertex_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE draw,
  GLenum mode, GLsizei count, GLenum type, const void *indices,
  GLsizei instance_count, GLint base_vertex, GLuint base_instance) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_multi_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, const GLint *first, const GLsizei *count,
  GLsizei draw_count) {
  if(!first || !count || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_surface_picker_draw_arrays(call,draw,mode,first[i],count[i]);
}

static void tr456_surface_picker_multi_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, const GLsizei *count, GLenum type,
  const void * const *indices, GLsizei draw_count) {
  if(!count || !indices || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_surface_picker_draw_elements(call,draw,mode,count[i],type,indices[i]);
}

static void tr456_surface_picker_multi_draw_elements_base_vertex(
  const char *call, PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode,
  const GLsizei *count, GLenum type, const void * const *indices,
  GLsizei draw_count, const GLint *base_vertex) {
  if(!count || !indices || !base_vertex || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_surface_picker_draw_elements_base_vertex(call,draw,mode,count[i],
      type,indices[i],base_vertex[i]);
}

static void tr456_surface_picker_draw_arrays_indirect(const char *call,
  PFNGLDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,0,0,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,indirect);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_draw_elements_indirect(const char *call,
  PFNGLDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect) {
  GLint attr=-1;
  if(!draw ||
     !tr456_surface_picker_selected(call,mode,0,0,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,type,indirect);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_multi_draw_arrays_indirect(const char *call,
  PFNGLMULTIDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect,
  GLsizei draw_count, GLsizei stride) {
  GLint attr=-1;
  if(!draw || draw_count<=0 ||
     !tr456_surface_picker_selected(call,mode,draw_count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,indirect,draw_count,stride);
  tr456_surface_picker_end_state(&state);
}

static void tr456_surface_picker_multi_draw_elements_indirect(const char *call,
  PFNGLMULTIDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect, GLsizei draw_count, GLsizei stride) {
  GLint attr=-1;
  if(!draw || draw_count<=0 ||
     !tr456_surface_picker_selected(call,mode,draw_count,1,&attr)) return;
  Tr456SurfacePickerDrawState state;
  if(!tr456_surface_picker_begin_draw(attr,&state)) return;
  draw(mode,type,indirect,draw_count,stride);
  tr456_surface_picker_end_state(&state);
}
