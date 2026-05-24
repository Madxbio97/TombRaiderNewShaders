static void draw_synthetic_surface_elements(GLenum mode, GLsizei count, GLenum type, const void *indices);
static void draw_synthetic_surface_arrays(GLenum mode, GLint first, GLsizei count);
static PFNGLDRAWRANGEELEMENTS real_draw_range_elements(void);
static PFNGLDRAWELEMENTSBASEVERTEX real_draw_elements_base_vertex(void);
static void draw_synthetic_surface_elements_base_vertex(GLenum mode, GLsizei count,
                                                        GLenum type, const void *indices,
                                                        GLint base_vertex);
static PFNGLDRAWRANGEELEMENTSBASEVERTEX real_draw_range_elements_base_vertex(void);
static PFNGLDRAWARRAYSINSTANCED real_draw_arrays_instanced(void);
static PFNGLDRAWELEMENTSINSTANCED real_draw_elements_instanced(void);
static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX real_draw_elements_instanced_base_vertex(void);
static PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE real_draw_arrays_instanced_base_instance(void);
static PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE real_draw_elements_instanced_base_instance(void);
static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE real_draw_elements_instanced_base_vertex_base_instance(void);
static PFNGLDRAWARRAYSINDIRECT real_draw_arrays_indirect(void);
static PFNGLDRAWELEMENTSINDIRECT real_draw_elements_indirect(void);
static PFNGLMULTIDRAWARRAYSINDIRECT real_multi_draw_arrays_indirect(void);
static PFNGLMULTIDRAWELEMENTSINDIRECT real_multi_draw_elements_indirect(void);

__declspec(dllexport) void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
  runtime_start_once();
  PFNGLDRAWELEMENTS real=real_draw_elements();
  if(real) {
    note_draw("glDrawElements",mode,count);
#if TR456_DIAG_BUILD
    {
      LONG n=InterlockedIncrement(&g_diag_draw_call_count);
      if(n<=96) {
        diag_logf("DIAG glDrawElements #%ld mode=0x%X count=%d indexType=0x%X program=%u programType=%s",
          (long)n,(unsigned int)mode,(int)count,(unsigned int)type,
          g_current_program,shader_type_name(g_current_program_type));
      }
    }
#endif
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface","synthetic water surface");
    if(!decision->skip_original) {
      real(mode,count,type,indices);
      tr456_wet_lara_draw_elements("glDrawElements",real,mode,count,type,indices);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements(mode,count,type,indices);
  }
}

static PFNGLDRAWARRAYS real_draw_arrays(void) {
  static PFNGLDRAWARRAYS p;
  if(!p) p=(PFNGLDRAWARRAYS)gl_proc("glDrawArrays");
  return p;
}

static int ensure_swap_debug_program(void) {
  if(g_swap_debug_program) return 1;
  if(g_swap_debug_tried) return 0;
  g_swap_debug_tried=1;

  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  if(!create_program || !attach || !link) return 0;

  GLuint vs=compile_water_shader(GL_VERTEX_SHADER,
    "swap debug vertex",swap_debug_vertex_shader);
  GLuint fs=compile_water_shader(GL_FRAGMENT_SHADER,
    "swap debug fragment",swap_debug_fragment_shader);
  if(!vs || !fs) {
    PFNGLDELETESHADER del_shader=real_delete_shader();
    if(del_shader) {
      if(vs) del_shader(vs);
      if(fs) del_shader(fs);
    }
    return 0;
  }

  GLuint program=create_program();
  if(!program) {
    PFNGLDELETESHADER del_shader=real_delete_shader();
    if(del_shader) { del_shader(vs); del_shader(fs); }
    return 0;
  }
  PFNGLBINDFRAGDATALOCATION bind_frag=real_bind_frag_data_location();
  if(bind_frag)
    bind_frag(program,0,"trshaderFragColor");
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
    snprintf(msg,sizeof(msg),"swap debug overlay link failed log=%s",logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    return 0;
  }

  g_swap_debug_program=program;
  if(runtime_verbose_log()) {
    char msg[128];
    snprintf(msg,sizeof(msg),"swap debug overlay linked program=%u",program);
    log_line(msg);
  }
  return 1;
}

static void draw_swap_debug_overlay(void) {
  load_runtime_config();
  if(!ini_int("SwapDebugOverlay",0)) return;
  if(!ensure_swap_debug_program()) return;

  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLDRAWARRAYS draw=real_draw_arrays();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  PFNGLBLENDEQUATIONSEPARATE blend_eq=real_blend_equation_separate();
  PFNGLCOLORMASK color_mask=real_color_mask();
  CaptureGL *gl=capture_gl();
  if(!use_program || !draw) return;

  GLint old_program=0,old_blend=0,old_depth=0,old_cull=0;
  GLint old_stencil=0,old_scissor=0,old_alpha=0;
  GLint old_sample_alpha=0,old_rasterizer=0;
  GLint old_depth_mask=1,old_depth_func=GL_LEQUAL;
  GLint old_blend_func[4]={
    GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA
  };
  GLint old_blend_eq[2]={GL_FUNC_ADD,GL_FUNC_ADD};
  GLint old_color_mask[4]={1,1,1,1};
  GLint old_draw_fbo=0;
  GLint viewport[4]={0,0,0,0};
  GLint old_clip[8]={0,0,0,0,0,0,0,0};

  if(gl && gl->get_integer) {
    shadow_get_integer_or_gl(GL_CURRENT_PROGRAM,&old_program);
    shadow_get_integer_or_gl(GL_DRAW_FRAMEBUFFER_BINDING,&old_draw_fbo);
    shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
    shadow_get_integer_or_gl(GL_BLEND,&old_blend);
    shadow_get_integer_or_gl(GL_DEPTH_TEST,&old_depth);
    shadow_get_integer_or_gl(GL_CULL_FACE,&old_cull);
    gl->get_integer(GL_STENCIL_TEST,&old_stencil);
    gl->get_integer(GL_SCISSOR_TEST,&old_scissor);
    gl->get_integer(GL_ALPHA_TEST,&old_alpha);
    gl->get_integer(GL_SAMPLE_ALPHA_TO_COVERAGE,&old_sample_alpha);
    gl->get_integer(GL_RASTERIZER_DISCARD,&old_rasterizer);
    gl->get_integer(GL_COLOR_WRITEMASK,old_color_mask);
    gl->get_integer(GL_BLEND_EQUATION_RGB,&old_blend_eq[0]);
    gl->get_integer(GL_BLEND_EQUATION_ALPHA,&old_blend_eq[1]);
    for(int i=0;i<8;i++)
      gl->get_integer((GLenum)(GL_CLIP_DISTANCE0+i),&old_clip[i]);
    shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,&old_depth_mask);
    shadow_get_integer_or_gl(GL_DEPTH_FUNC,&old_depth_func);
    shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&old_blend_func[0]);
    shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&old_blend_func[1]);
    shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&old_blend_func[2]);
    shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&old_blend_func[3]);
  }

  if(gl && gl->bind_framebuffer)
    shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,0);
  if(disable) {
    disable(GL_BLEND); shadow_note_enable(GL_BLEND,0);
    disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0);
    disable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,0);
    disable(GL_STENCIL_TEST);
    disable(GL_SCISSOR_TEST);
    disable(GL_ALPHA_TEST);
    disable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    disable(GL_RASTERIZER_DISCARD);
    for(int i=0;i<8;i++)
      disable((GLenum)(GL_CLIP_DISTANCE0+i));
  }
  if(depth_mask) { depth_mask(GL_FALSE); shadow_note_depth_mask(GL_FALSE); }
  if(depth_func) { depth_func(GL_ALWAYS); shadow_note_depth_func(GL_ALWAYS); }
  if(color_mask) color_mask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  if(blend_eq) blend_eq(GL_FUNC_ADD,GL_FUNC_ADD);
  if(blend_func) {
    blend_func(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
    shadow_note_blend_func(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
  }

  use_program(g_swap_debug_program);
  shadow_note_use_program(g_swap_debug_program);
  trshader_compat_clear_gl_errors();
  draw(GL_TRIANGLES,0,6);
  GLenum err=(gl && gl->get_error) ? gl->get_error() : 0;

  if(runtime_verbose_log() && g_swap_debug_logged<16u) {
    char msg[256];
    snprintf(msg,sizeof(msg),
      "swap debug overlay draw frame=%u fbo=%d viewport=%d,%d,%d,%d glerr=0x%04X",
      g_frame_index,old_draw_fbo,viewport[0],viewport[1],viewport[2],
      viewport[3],(unsigned int)err);
    log_line(msg);
    g_swap_debug_logged++;
  }

  if(color_mask) {
    color_mask((GLboolean)(old_color_mask[0] ? 1 : 0),
      (GLboolean)(old_color_mask[1] ? 1 : 0),
      (GLboolean)(old_color_mask[2] ? 1 : 0),
      (GLboolean)(old_color_mask[3] ? 1 : 0));
  }
  if(blend_eq)
    blend_eq((GLenum)old_blend_eq[0],(GLenum)old_blend_eq[1]);
  if(blend_func) {
    blend_func((GLenum)old_blend_func[0],(GLenum)old_blend_func[1],
      (GLenum)old_blend_func[2],(GLenum)old_blend_func[3]);
    shadow_note_blend_func((GLenum)old_blend_func[0],
      (GLenum)old_blend_func[1],(GLenum)old_blend_func[2],
      (GLenum)old_blend_func[3]);
  }
  if(depth_mask) {
    depth_mask((GLboolean)(old_depth_mask ? 1 : 0));
    shadow_note_depth_mask((GLboolean)(old_depth_mask ? 1 : 0));
  }
  if(depth_func) {
    depth_func((GLenum)old_depth_func);
    shadow_note_depth_func((GLenum)old_depth_func);
  }
  if(old_cull) { if(enable) { enable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,1); } } else { if(disable) { disable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,0); } }
  if(old_stencil) { if(enable) enable(GL_STENCIL_TEST); } else { if(disable) disable(GL_STENCIL_TEST); }
  if(old_scissor) { if(enable) enable(GL_SCISSOR_TEST); } else { if(disable) disable(GL_SCISSOR_TEST); }
  if(old_alpha) { if(enable) enable(GL_ALPHA_TEST); } else { if(disable) disable(GL_ALPHA_TEST); }
  if(old_sample_alpha) { if(enable) enable(GL_SAMPLE_ALPHA_TO_COVERAGE); } else { if(disable) disable(GL_SAMPLE_ALPHA_TO_COVERAGE); }
  if(old_rasterizer) { if(enable) enable(GL_RASTERIZER_DISCARD); } else { if(disable) disable(GL_RASTERIZER_DISCARD); }
  for(int i=0;i<8;i++) {
    GLenum cap=(GLenum)(GL_CLIP_DISTANCE0+i);
    if(old_clip[i]) { if(enable) enable(cap); }
    else { if(disable) disable(cap); }
  }
  if(old_depth) { if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); } } else { if(disable) { disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0); } }
  if(old_blend) { if(enable) { enable(GL_BLEND); shadow_note_enable(GL_BLEND,1); } } else { if(disable) { disable(GL_BLEND); shadow_note_enable(GL_BLEND,0); } }
  if(gl && gl->bind_framebuffer)
    shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,(GLuint)old_draw_fbo);
  use_program((GLuint)old_program);
  shadow_note_use_program((GLuint)old_program);
}

static int read_current_uniform_matrix4(const char *name, GLfloat out[16]) {
  CaptureGL *gl=capture_gl();
  if(!gl || !g_current_program) return 0;
  GLint loc=trshader_current_uniform_location(name);
  if(loc<0) return 0;
  if(shadow_read_uniform_floats(g_current_program,loc,out,16))
    return 1;
  if(!gl->get_uniform_fv) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int read_current_uniform_vec4_array(const char *base, int count, GLfloat *out) {
  CaptureGL *gl=capture_gl();
  if(!gl || !g_current_program) return 0;
  for(int i=0;i<count;i++) {
    char name[48];
    snprintf(name,sizeof(name),"%s[%d]",base,i);
    GLint loc=trshader_current_uniform_location(name);
    if(loc<0) return 0;
    if(!shadow_read_uniform_floats(g_current_program,loc,out+i*4,4)) {
      if(!gl->get_uniform_fv) return 0;
      gl->get_uniform_fv(g_current_program,loc,out+i*4);
    }
  }
  return 1;
}

static void read_current_uniform_vec4_default(const char *name, GLfloat out[4],
                                              GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  out[0]=x; out[1]=y; out[2]=z; out[3]=w;
  CaptureGL *gl=capture_gl();
  if(!gl || !g_current_program) return;
  GLint loc=trshader_current_uniform_location(name);
  if(loc<0) return;
  if(shadow_read_uniform_floats(g_current_program,loc,out,4))
    return;
  if(gl->get_uniform_fv) gl->get_uniform_fv(g_current_program,loc,out);
}

static int read_current_uniform_matrix_or_vec4_array(const char *name,
                                                     GLfloat out[16]) {
  if(read_current_uniform_vec4_array(name,4,out))
    return 1;
  return read_current_uniform_matrix4(name,out);
}

static void log_synthetic_vertex_attrib_state(const char *label, GLint loc) {
  if(loc<0) return;
  PFNGLGETVERTEXATTRIBIV getiv=real_get_vertex_attrib_iv();
  PFNGLGETVERTEXATTRIBPOINTERV getptr=real_get_vertex_attrib_pointer_v();
  if(!getiv || !getptr) return;

  GLint enabled=0;
  GLint size=0;
  GLint stride=0;
  GLint type=0;
  GLint normalized=0;
  GLint buffer=0;
  void *ptr=0;
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&enabled);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_SIZE,&size);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_STRIDE,&stride);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_TYPE,&type);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,&normalized);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,&buffer);
  getptr((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_POINTER,&ptr);

  char msg[320];
  snprintf(msg,sizeof(msg),
    "synthetic attrib %s loc=%d enabled=%d size=%d stride=%d type=0x%X norm=%d buffer=%d ptr=%p",
    label ? label : "attr",(int)loc,(int)enabled,(int)size,
    (int)stride,(unsigned int)type,(int)normalized,(int)buffer,ptr);
  log_line(msg);
}

static void log_synthetic_attrib_state(GLenum mode, GLsizei count,
                                       int count_known) {
  if(!runtime_verbose_log() || g_synthetic_attrib_state_logged>=8u)
    return;

  CaptureGL *gl=capture_gl();
  GLint actual_program=0;
  GLint vao=0;
  GLint ebo=0;
  GLint array_buffer=0;
  if(gl && gl->get_integer) {
    gl->get_integer(GL_CURRENT_PROGRAM,&actual_program);
    gl->get_integer(GL_VERTEX_ARRAY_BINDING,&vao);
    gl->get_integer(GL_ELEMENT_ARRAY_BUFFER_BINDING,&ebo);
    gl->get_integer(GL_ARRAY_BUFFER_BINDING,&array_buffer);
  }

  GLint src[4]={-1,-1,-1,-1};
  int has_src=current_water_attrib_locations(src);
  GLint syn[4]={
    g_synthetic_surface.attr_coord,
    g_synthetic_surface.attr_normal,
    g_synthetic_surface.attr_light,
    g_synthetic_surface.attr_color
  };

  char msg[448];
  snprintf(msg,sizeof(msg),
    "synthetic attrib probe frame=%u sourceProgram=%u sourceType=%s actualProgram=%d mode=0x%X count=%d countKnown=%d vao=%d ebo=%d arrayBuffer=%d srcLocs%s=(%d,%d,%d,%d) synLocs=(%d,%d,%d,%d)",
    g_frame_index,g_current_program,shader_type_name(g_current_program_type),
    (int)actual_program,(unsigned int)mode,(int)count,count_known,
    (int)vao,(int)ebo,(int)array_buffer,has_src ? "" : "?",
    (int)src[0],(int)src[1],(int)src[2],(int)src[3],
    (int)syn[0],(int)syn[1],(int)syn[2],(int)syn[3]);
  log_line(msg);

  static const char *names[4]={"coord","normal","light","color"};
  for(int i=0;i<4;i++) {
    char label[32];
    snprintf(label,sizeof(label),"syn_%s",names[i]);
    log_synthetic_vertex_attrib_state(label,syn[i]);
    if(src[i]>=0 && src[i]!=syn[i]) {
      snprintf(label,sizeof(label),"src_%s",names[i]);
      log_synthetic_vertex_attrib_state(label,src[i]);
    }
  }
  g_synthetic_attrib_state_logged++;
}

static void synthetic_water_profile(GLenum mode, GLsizei count, int count_known,
                                    const GLfloat params[4],
                                    const GLfloat model3[4],
                                    GLfloat profile[4]) {
  (void)mode;
  profile[0]=1.0f;
  profile[1]=1.0f;
  profile[2]=1.0f;
  profile[3]=1.0f;

  if(g_current_program_type==SHADER_WATER_FLOW) {
    WaterDrawProfile flow;
    GLfloat flow_params[4];
    memcpy(flow_params,params,sizeof(flow_params));
    if(!current_flow_draw_profile(mode,count,count_known,&flow,flow_params)) {
      flow_profile_set_surface(&flow,"flow surface");
    }
    profile[0]=(GLfloat)flow.id;
    profile[1]=flow.foam_scale;
    profile[2]=flow.opacity_scale;
    profile[3]=flow.reflection_scale;
  } else if(g_current_program_type==SHADER_WATER_SURFACE) {
    profile[0]=(GLfloat)WATER_PROFILE_STANDING_SURFACE;
    profile[1]=1.08f;
    profile[2]=1.00f;
    profile[3]=0.82f;
  } else if(g_current_program_type==SHADER_WATER_REFLECT) {
    profile[0]=(GLfloat)WATER_PROFILE_STANDING;
    profile[1]=(model3[1]>4096.0f) ? 1.14f : 1.00f;
    profile[2]=1.00f;
    profile[3]=0.92f;
  }
}

static void setup_synthetic_surface_uniforms(GLenum mode, GLsizei count,
                                             int count_known) {
  SyntheticSurfacePass *s=&g_synthetic_surface;
  CaptureGL *gl=capture_gl();
  if(!s->ready || !gl || !gl->uniform_4fv) return;
  unsigned long long perf_t0=perf_ticks_now();
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();

  GLfloat proj[16]={0.0f};
  GLfloat model[16]={0.0f};
  GLfloat view[16]={0.0f};
  int has_proj=read_current_uniform_matrix4("uProjMatrix",proj);
  int has_model=read_current_uniform_matrix_or_vec4_array("uModelMatrix",model);
  int has_view=read_current_uniform_matrix_or_vec4_array("uViewMatrix",view);

  if(runtime_verbose_log() && !g_synthetic_uniform_state_logged) {
    GLint actual_program=0;
    if(gl->get_integer)
      gl->get_integer(GL_CURRENT_PROGRAM,&actual_program);
    GLint src_proj=trshader_current_uniform_location("uProjMatrix");
    GLint src_model0=trshader_current_uniform_location("uModelMatrix[0]");
    GLint src_model=trshader_current_uniform_location("uModelMatrix");
    GLint src_view0=trshader_current_uniform_location("uViewMatrix[0]");
    GLint src_view=trshader_current_uniform_location("uViewMatrix");
    char msg[768];
    snprintf(msg,sizeof(msg),
      "synthetic uniform probe frame=%u sourceProgram=%u sourceType=%s actualProgram=%d dstProgram=%u has=%d/%d/%d srcLocs proj=%d model0=%d model=%d view0=%d view=%d dstLocs proj=%d model=%d view=%d proj=(%.3f %.3f %.3f %.3f) modelRow0=(%.3f %.3f %.3f %.3f) modelRow3=(%.3f %.3f %.3f %.3f) viewRow0=(%.3f %.3f %.3f %.3f) viewRow3=(%.3f %.3f %.3f %.3f)",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (int)actual_program,g_synthetic_surface.program,
      has_proj,has_model,has_view,
      (int)src_proj,(int)src_model0,(int)src_model,
      (int)src_view0,(int)src_view,
      (int)s->loc_proj,(int)s->loc_model,(int)s->loc_view,
      (double)proj[0],(double)proj[5],(double)proj[10],(double)proj[14],
      (double)model[0],(double)model[1],(double)model[2],(double)model[3],
      (double)model[12],(double)model[13],(double)model[14],(double)model[15],
      (double)view[0],(double)view[1],(double)view[2],(double)view[3],
      (double)view[12],(double)view[13],(double)view[14],(double)view[15]);
    log_line(msg);
    g_synthetic_uniform_state_logged=1;
  }

  if(s->loc_proj>=0 && matrix4 && has_proj) {
    matrix4(s->loc_proj,1,GL_FALSE,proj);
    shadow_note_uniform_matrix4fv(s->loc_proj,1,GL_FALSE,proj);
  }

  if(s->loc_model>=0 && has_model)
    shadow_call_uniform_4fv(gl,s->loc_model,4,model);

  if(s->loc_view>=0 && has_view)
    shadow_call_uniform_4fv(gl,s->loc_view,4,view);

  if(s->loc_scene>=0 && gl->uniform_1i)
    shadow_call_uniform_1i(gl,s->loc_scene,15);
  if(s->loc_underlay>=0 && gl->uniform_1i)
    shadow_call_uniform_1i(gl,s->loc_underlay,14);

  if(gl->active_texture && gl->bind_texture && g_scene_tex) {
    GLint old_active=GL_TEXTURE0;
    shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    shadow_call_active_texture(gl,GL_TEXTURE14);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      g_underlay_has_pixels ? g_underlay_tex : 0);
    if(old_active) shadow_call_active_texture(gl,(GLenum)old_active);
  }

  int capture_view_w=g_scene_view_w>0 ? g_scene_view_w : g_scene_w;
  int capture_view_h=g_scene_view_h>0 ? g_scene_view_h : g_scene_h;
  if(capture_view_w<=0) capture_view_w=1920;
  if(capture_view_h<=0) capture_view_h=1080;
  GLfloat inv_w=1.0f/(GLfloat)capture_view_w;
  GLfloat inv_h=1.0f/(GLfloat)capture_view_h;
  if(s->loc_capture_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_capture_info,inv_w,inv_h,(GLfloat)capture_view_w,(GLfloat)capture_view_h);
  int underlay_view_w=g_underlay_view_w>0 ? g_underlay_view_w : capture_view_w;
  int underlay_view_h=g_underlay_view_h>0 ? g_underlay_view_h : capture_view_h;
  GLfloat underlay_enabled=(g_underlay_has_pixels &&
    (current_draw_uses_water_underlay_pattern() ||
     current_draw_uses_reflect_original_mask())) ? 1.0f : 0.0f;
  if(s->loc_underlay_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_underlay_info,
      1.0f/(GLfloat)underlay_view_w,
      1.0f/(GLfloat)underlay_view_h,
      underlay_enabled,g_runtime_underlay_pattern_strength);

  GLfloat params[4]={0.0f,0.0f,1.0f,0.0f};
  if(g_current_program_type==SHADER_WATER_FLOW) {
    WaterDrawProfile flow;
    flow_profile_set_original(&flow,"unclassified flow");
    if(!current_flow_draw_profile(mode,count,count_known,&flow,params)) {
      params[0]=0.0f; params[1]=0.0f; params[2]=1.0f; params[3]=0.0f;
    }
  } else {
    read_current_uniform_vec4_default("uParams",params,0.0f,0.0f,1.0f,0.0f);
  }
  if(s->loc_params>=0)
    shadow_call_uniform_4fv(gl,s->loc_params,1,params);

  GLfloat draw_info[4]={
    (GLfloat)g_frame_index,1.0f,0.0f,0.0f
  };
  read_current_uniform_vec4_default("uTrWaterDrawInfo",draw_info,
    (GLfloat)g_frame_index,1.0f,0.0f,0.0f);
  if(s->loc_draw_info>=0)
    shadow_call_uniform_4fv(gl,s->loc_draw_info,1,draw_info);

  GLfloat toggle0[4]={
    effect_toggle_value(0),effect_toggle_value(1),
    effect_toggle_value(2),effect_toggle_value(3)
  };
  GLfloat toggle1[4]={
    effect_toggle_value(4),effect_toggle_value(5),
    effect_toggle_value(6),effect_toggle_value(7)
  };
  GLfloat toggle2[4]={
    effect_toggle_value(8),effect_toggle_value(9),
    effect_toggle_value(10),effect_toggle_value(11)
  };
  if(s->loc_toggle0>=0)
    shadow_call_uniform_4fv(gl,s->loc_toggle0,1,toggle0);
  if(s->loc_toggle1>=0)
    shadow_call_uniform_4fv(gl,s->loc_toggle1,1,toggle1);
  if(s->loc_toggle2>=0)
    shadow_call_uniform_4fv(gl,s->loc_toggle2,1,toggle2);

  GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
  GLfloat synthetic_time=0.0f;
  if(read_uniform_vec4_index_now("uModelMatrix",3,model3))
    synthetic_time=model3[0];

  GLfloat contacts[16][4];
  GLfloat motions[16][4];
  memset(contacts,0,sizeof(contacts));
  memset(motions,0,sizeof(motions));
  int contact_source=0;
  int contact_count=build_effective_contact_values(contacts,motions,&contact_source);
  if(g_current_program_type==SHADER_WATER_FLOW &&
     g_diag_synthetic_contact_log_frame!=g_frame_index &&
     diag_is_active()) {
    char msg[512];
    snprintf(msg,sizeof(msg),
      "synthetic contacts frame=%u source=%d active=%d sum=%.2f motion_sum=%.2f c0=(%.1f %.1f %.1f %.1f) m0=(%.2f %.2f %.2f %.2f)",
      g_frame_index,contact_source,contact_count,
      (double)contact_values_sum_abs(contacts),
      (double)contact_values_sum_abs(motions),
      (double)contacts[0][0],(double)contacts[0][1],
      (double)contacts[0][2],(double)contacts[0][3],
      (double)motions[0][0],(double)motions[0][1],
      (double)motions[0][2],(double)motions[0][3]);
    log_line(msg);
    g_diag_synthetic_contact_log_frame=g_frame_index;
    diag_consume_line();
  }
  if(s->loc_contacts>=0)
    shadow_call_uniform_4fv(gl,s->loc_contacts,16,&contacts[0][0]);
  if(s->loc_contact_motion>=0)
    shadow_call_uniform_4fv(gl,s->loc_contact_motion,16,&motions[0][0]);

  if(s->loc_synthetic_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_synthetic_info,
      g_runtime_synthetic_opacity,g_runtime_synthetic_tint,
      g_runtime_synthetic_reflection,synthetic_time);

  if(s->loc_synthetic_mode>=0 && gl->uniform_4f) {
    GLfloat flow_mode=g_current_program_type==SHADER_WATER_FLOW ? 1.0f : 0.0f;
    shadow_call_uniform_4f(gl,s->loc_synthetic_mode,
      flow_mode,(GLfloat)g_current_program_type,draw_info[2],draw_info[3]);
  }

  if(s->loc_synthetic_profile>=0 && gl->uniform_4fv) {
    GLfloat profile[4];
    synthetic_water_profile(mode,count,count_known,params,model3,profile);
    shadow_call_uniform_4fv(gl,s->loc_synthetic_profile,1,profile);
  }
  perf_note_synthetic_begin(perf_ticks_elapsed(perf_t0));
}

static void begin_synthetic_surface_state(SyntheticSurfaceDrawState *state) {
  if(!state) return;
  CaptureGL *gl=capture_gl();
  const int preserve_standing_mask=
    g_runtime_synthetic_standing_preserve_mask &&
    (g_current_program_type==SHADER_WATER_SURFACE ||
     g_current_program_type==SHADER_WATER_REFLECT);
  if(gl->get_integer) {
    shadow_get_integer_or_gl(GL_CURRENT_PROGRAM,&state->old_program);
    shadow_get_integer_or_gl(GL_BLEND,&state->old_blend);
    shadow_get_integer_or_gl(GL_DEPTH_TEST,&state->old_depth);
    shadow_get_integer_or_gl(GL_CULL_FACE,&state->old_cull);
    gl->get_integer(GL_STENCIL_TEST,&state->old_stencil);
    gl->get_integer(GL_SCISSOR_TEST,&state->old_scissor);
    gl->get_integer(GL_ALPHA_TEST,&state->old_alpha);
    gl->get_integer(GL_SAMPLE_ALPHA_TO_COVERAGE,
      &state->old_sample_alpha_to_coverage);
    gl->get_integer(GL_RASTERIZER_DISCARD,
      &state->old_rasterizer_discard);
    for(int i=0;i<8;i++)
      gl->get_integer((GLenum)(GL_CLIP_DISTANCE0+i),
        &state->old_clip_distance[i]);
    if(runtime_verbose_log() && !g_synthetic_clip_state_logged) {
      unsigned int clip_mask=0;
      for(int i=0;i<8;i++)
        if(state->old_clip_distance[i])
          clip_mask|=1u<<(unsigned int)i;
      char msg[160];
      snprintf(msg,sizeof(msg),
        "synthetic water pre-state rasterizer=%d stencil=%d scissor=%d clip=0x%02X preserveMask=%d",
        state->old_rasterizer_discard,state->old_stencil,
        state->old_scissor,clip_mask,preserve_standing_mask);
      log_line(msg);
      g_synthetic_clip_state_logged=1;
    }
    shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,&state->old_depth_mask);
    shadow_get_integer_or_gl(GL_DEPTH_FUNC,&state->old_depth_func);
    shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&state->old_blend_func[0]);
    shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&state->old_blend_func[1]);
    shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&state->old_blend_func[2]);
    shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&state->old_blend_func[3]);
    gl->get_integer(GL_BLEND_EQUATION_RGB,&state->old_blend_equation[0]);
    gl->get_integer(GL_BLEND_EQUATION_ALPHA,&state->old_blend_equation[1]);
    gl->get_integer(GL_COLOR_WRITEMASK,state->old_color_mask);
  }
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  if(disable) {
    disable(GL_CULL_FACE);
    shadow_note_enable(GL_CULL_FACE,0);
    if(!preserve_standing_mask || !state->old_stencil)
      disable(GL_STENCIL_TEST);
    if(!preserve_standing_mask || !state->old_scissor)
      disable(GL_SCISSOR_TEST);
    disable(GL_ALPHA_TEST);
    disable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    disable(GL_RASTERIZER_DISCARD);
    for(int i=0;i<8;i++)
      disable((GLenum)(GL_CLIP_DISTANCE0+i));
  }
  if(enable && !g_runtime_synthetic_debug_solid) {
    enable(GL_BLEND);
    shadow_note_enable(GL_BLEND,1);
  } else if(disable && g_runtime_synthetic_debug_solid) {
    disable(GL_BLEND);
    shadow_note_enable(GL_BLEND,0);
  }
  PFNGLCOLORMASK color_mask=real_color_mask();
  if(color_mask) color_mask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  PFNGLBLENDEQUATIONSEPARATE blend_eq=real_blend_equation_separate();
  if(blend_eq) blend_eq(GL_FUNC_ADD,GL_FUNC_ADD);
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(blend_func) {
    if(g_runtime_synthetic_debug_solid) {
      blend_func(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
      shadow_note_blend_func(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
    } else {
      blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
      shadow_note_blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    }
  }
  if(runtime_verbose_log() && !g_synthetic_overlay_depth_logged) {
    char msg[160];
    snprintf(msg,sizeof(msg),
      "synthetic water overlay depth mode=%d",
      g_runtime_synthetic_overlay_depth_mode);
    log_line(msg);
    g_synthetic_overlay_depth_logged=1;
  }
  if(g_runtime_synthetic_overlay_depth_mode==1) {
    if(disable) { disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0); }
  } else {
    if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); }
    if(depth_func) {
      GLenum func=g_runtime_synthetic_overlay_depth_mode==2 ?
        GL_ALWAYS : GL_LEQUAL;
      depth_func(func);
      shadow_note_depth_func(func);
    }
  }
  if(depth_mask) { depth_mask(GL_FALSE); shadow_note_depth_mask(GL_FALSE); }
}

static void end_synthetic_surface_state(const SyntheticSurfaceDrawState *state) {
  if(!state) return;
  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLCOLORMASK color_mask=real_color_mask();
  PFNGLBLENDEQUATIONSEPARATE blend_eq=real_blend_equation_separate();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(color_mask) {
    color_mask((GLboolean)(state->old_color_mask[0] ? 1 : 0),
      (GLboolean)(state->old_color_mask[1] ? 1 : 0),
      (GLboolean)(state->old_color_mask[2] ? 1 : 0),
      (GLboolean)(state->old_color_mask[3] ? 1 : 0));
  }
  if(blend_eq)
    blend_eq((GLenum)state->old_blend_equation[0],
      (GLenum)state->old_blend_equation[1]);
  if(blend_func) {
    blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[2],
      (GLenum)state->old_blend_func[3]);
    shadow_note_blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[2],
      (GLenum)state->old_blend_func[3]);
  }
  if(depth_mask) {
    depth_mask((GLboolean)(state->old_depth_mask ? 1 : 0));
    shadow_note_depth_mask((GLboolean)(state->old_depth_mask ? 1 : 0));
  }
  if(depth_func) {
    depth_func((GLenum)state->old_depth_func);
    shadow_note_depth_func((GLenum)state->old_depth_func);
  }
  if(state->old_cull) { if(enable) { enable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,1); } } else { if(disable) { disable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,0); } }
  if(state->old_stencil) { if(enable) enable(GL_STENCIL_TEST); } else { if(disable) disable(GL_STENCIL_TEST); }
  if(state->old_scissor) { if(enable) enable(GL_SCISSOR_TEST); } else { if(disable) disable(GL_SCISSOR_TEST); }
  if(state->old_alpha) { if(enable) enable(GL_ALPHA_TEST); } else { if(disable) disable(GL_ALPHA_TEST); }
  if(state->old_sample_alpha_to_coverage) { if(enable) enable(GL_SAMPLE_ALPHA_TO_COVERAGE); } else { if(disable) disable(GL_SAMPLE_ALPHA_TO_COVERAGE); }
  if(state->old_rasterizer_discard) { if(enable) enable(GL_RASTERIZER_DISCARD); } else { if(disable) disable(GL_RASTERIZER_DISCARD); }
  for(int i=0;i<8;i++) {
    GLenum cap=(GLenum)(GL_CLIP_DISTANCE0+i);
    if(state->old_clip_distance[i]) { if(enable) enable(cap); }
    else { if(disable) disable(cap); }
  }
  if(state->old_depth) { if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); } } else { if(disable) { disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0); } }
  if(state->old_blend) { if(enable) { enable(GL_BLEND); shadow_note_enable(GL_BLEND,1); } } else { if(disable) { disable(GL_BLEND); shadow_note_enable(GL_BLEND,0); } }
  if(use_program) {
    use_program((GLuint)state->old_program);
    shadow_note_use_program((GLuint)state->old_program);
  }
}

static void init_synthetic_surface_draw_state(SyntheticSurfaceDrawState *state) {
  state->old_program=(GLint)g_current_program;
  state->old_blend=0;
  state->old_depth=0;
  state->old_cull=0;
  state->old_stencil=0;
  state->old_scissor=0;
  state->old_alpha=0;
  state->old_sample_alpha_to_coverage=0;
  state->old_rasterizer_discard=0;
  for(int i=0;i<8;i++)
    state->old_clip_distance[i]=0;
  state->old_depth_mask=1;
  state->old_depth_func=GL_LEQUAL;
  state->old_blend_func[0]=GL_SRC_ALPHA;
  state->old_blend_func[1]=GL_ONE_MINUS_SRC_ALPHA;
  state->old_blend_func[2]=GL_ONE;
  state->old_blend_func[3]=GL_ONE_MINUS_SRC_ALPHA;
  state->old_blend_equation[0]=GL_FUNC_ADD;
  state->old_blend_equation[1]=GL_FUNC_ADD;
  state->old_color_mask[0]=1;
  state->old_color_mask[1]=1;
  state->old_color_mask[2]=1;
  state->old_color_mask[3]=1;
  state->old_active_texture=GL_TEXTURE0;
  state->old_scene_texture_2d=0;
  state->old_underlay_texture_2d=0;
  state->old_flow_texture_array=0;
  state->old_scene_texture_valid=0;
  state->old_underlay_texture_valid=0;
  state->old_flow_texture_array_valid=0;
}

static void setup_flow_lite_surface_uniforms(GLenum mode, GLsizei count,
                                             int count_known) {
  SyntheticSurfacePass *s=&g_flow_lite_surface;
  CaptureGL *gl=capture_gl();
  if(!s->ready || !gl || !gl->uniform_4fv) return;
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();

  GLfloat proj[16]={0.0f};
  GLfloat model[16]={0.0f};
  GLfloat view[16]={0.0f};
  int has_proj=read_current_uniform_matrix4("uProjMatrix",proj);
  int has_model=read_current_uniform_matrix_or_vec4_array("uModelMatrix",model);
  int has_view=read_current_uniform_matrix_or_vec4_array("uViewMatrix",view);
  if(s->loc_proj>=0 && matrix4 && has_proj) {
    matrix4(s->loc_proj,1,GL_FALSE,proj);
    shadow_note_uniform_matrix4fv(s->loc_proj,1,GL_FALSE,proj);
  }
  if(s->loc_model>=0 && has_model)
    shadow_call_uniform_4fv(gl,s->loc_model,4,model);
  if(s->loc_view>=0 && has_view)
    shadow_call_uniform_4fv(gl,s->loc_view,4,view);

  if(s->loc_scene>=0 && gl->uniform_1i)
    shadow_call_uniform_1i(gl,s->loc_scene,15);
  if(s->loc_flow_tex0_wrap>=0 && gl->uniform_1i)
    shadow_call_uniform_1i(gl,s->loc_flow_tex0_wrap,13);
  int capture_view_w=g_scene_view_w>0 ? g_scene_view_w : g_scene_w;
  int capture_view_h=g_scene_view_h>0 ? g_scene_view_h : g_scene_h;
  if(capture_view_w<=0) capture_view_w=1920;
  if(capture_view_h<=0) capture_view_h=1080;
  if(s->loc_capture_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_capture_info,
      1.0f/(GLfloat)capture_view_w,1.0f/(GLfloat)capture_view_h,
      (GLfloat)capture_view_w,(GLfloat)capture_view_h);

  GLfloat params[4]={0.0f,0.0f,1.0f,0.0f};
  current_flow_draw_profile(mode,count,count_known,0,params);
  if(s->loc_params>=0)
    shadow_call_uniform_4fv(gl,s->loc_params,1,params);

  GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
  GLfloat synthetic_time=0.0f;
  if(read_uniform_vec4_index_now("uModelMatrix",3,model3))
    synthetic_time=model3[0];
  if(s->loc_synthetic_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_synthetic_info,
      g_runtime_synthetic_opacity,g_runtime_synthetic_tint,
      g_runtime_synthetic_reflection,synthetic_time);
  if(s->loc_synthetic_mode>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_synthetic_mode,
      current_flow_texture_object() ? 1.0f : 0.0f,
      (GLfloat)g_current_program_type,0.0f,0.0f);

  GLfloat toggle0[4]={
    effect_toggle_value(0),effect_toggle_value(1),
    effect_toggle_value(2),effect_toggle_value(3)
  };
  GLfloat toggle1[4]={
    effect_toggle_value(4),effect_toggle_value(5),
    effect_toggle_value(6),effect_toggle_value(7)
  };
  if(s->loc_toggle0>=0)
    shadow_call_uniform_4fv(gl,s->loc_toggle0,1,toggle0);
  if(s->loc_toggle1>=0)
    shadow_call_uniform_4fv(gl,s->loc_toggle1,1,toggle1);
  if(s->loc_flow_fx0>=0)
    shadow_call_uniform_4fv(gl,s->loc_flow_fx0,1,g_runtime_flow_fx0);
  if(s->loc_flow_fx1>=0)
    shadow_call_uniform_4fv(gl,s->loc_flow_fx1,1,g_runtime_flow_fx1);
  if(s->loc_flow_fx2>=0)
    shadow_call_uniform_4fv(gl,s->loc_flow_fx2,1,g_runtime_flow_fx2);
  if(s->loc_flow_fx3>=0)
    shadow_call_uniform_4fv(gl,s->loc_flow_fx3,1,g_runtime_flow_fx3);

  if(s->loc_synthetic_profile>=0) {
    GLfloat profile[4];
    synthetic_water_profile(mode,count,count_known,params,model3,profile);
    shadow_call_uniform_4fv(gl,s->loc_synthetic_profile,1,profile);
  }
}

static int begin_flow_lite_surface_draw(GLenum mode, GLsizei count,
                                        int count_known,
                                        SyntheticSurfaceDrawState *state) {
  if(!state) return 0;
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  if(!decision->synthetic_flow_lite || !decision->synthetic_ready) return 0;
  if(!g_scene_tex || g_scene_w<=0 || g_scene_h<=0) return 0;
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!use_program) return 0;

  init_synthetic_surface_draw_state(state);
  CaptureGL *gl=capture_gl();
  if(gl->active_texture && gl->bind_texture) {
    GLuint flow_tex=current_flow_texture_object();
    shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&state->old_active_texture);
    if(flow_tex) {
      shadow_call_active_texture(gl,(GLenum)(GL_TEXTURE0+13));
      shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D_ARRAY,
        &state->old_flow_texture_array);
      state->old_flow_texture_array_valid=1;
      shadow_call_bind_texture(gl,GL_TEXTURE_2D_ARRAY,flow_tex);
    }
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,
      &state->old_scene_texture_2d);
    state->old_scene_texture_valid=1;
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  begin_synthetic_surface_state(state);
  use_program(g_flow_lite_surface.program);
  shadow_note_use_program(g_flow_lite_surface.program);
  setup_flow_lite_surface_uniforms(mode,count,count_known);
  trshader_compat_clear_gl_errors();
  return 1;
}

static void end_flow_lite_surface_draw(const SyntheticSurfaceDrawState *state) {
  if(!state) return;
  CaptureGL *gl=capture_gl();
  if(state->old_flow_texture_array_valid && gl->active_texture &&
     gl->bind_texture) {
    shadow_call_active_texture(gl,(GLenum)(GL_TEXTURE0+13));
    shadow_call_bind_texture(gl,GL_TEXTURE_2D_ARRAY,
      (GLuint)state->old_flow_texture_array);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  if(state->old_scene_texture_valid && gl->active_texture &&
     gl->bind_texture) {
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      (GLuint)state->old_scene_texture_2d);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  end_synthetic_surface_state(state);
}

static int current_draw_uses_flow_lite(GLenum mode, GLsizei count,
                                       int count_known) {
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  return decision->synthetic_flow_lite && decision->synthetic_ready;
}

static int begin_synthetic_surface_draw(GLenum mode, GLsizei count, int count_known,
                                        SyntheticSurfaceDrawState *state) {
  if(!state) return 0;
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  if(!decision->synthetic_surface || !decision->synthetic_ready) return 0;
  if(!g_scene_tex || g_scene_w<=0 || g_scene_h<=0) return 0;
  capture_water_underlay_for_synthetic_surface(decision->capture_reason);

  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!use_program) return 0;

  init_synthetic_surface_draw_state(state);
  CaptureGL *gl=capture_gl();
  if(gl->active_texture && gl->bind_texture) {
    shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&state->old_active_texture);
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,
      &state->old_scene_texture_2d);
    state->old_scene_texture_valid=1;
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    shadow_call_active_texture(gl,GL_TEXTURE14);
    shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,
      &state->old_underlay_texture_2d);
    state->old_underlay_texture_valid=1;
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      g_underlay_has_pixels ? g_underlay_tex : 0);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  log_synthetic_attrib_state(mode,count,count_known);
  begin_synthetic_surface_state(state);
  use_program(g_synthetic_surface.program);
  shadow_note_use_program(g_synthetic_surface.program);
  setup_synthetic_surface_uniforms(mode,count,count_known);
  trshader_compat_clear_gl_errors();
  return 1;
}

static void end_synthetic_surface_draw(const SyntheticSurfaceDrawState *state) {
  if(!state) return;
  CaptureGL *gl=capture_gl();
  if(state->old_underlay_texture_valid && gl->active_texture &&
     gl->bind_texture) {
    shadow_call_active_texture(gl,GL_TEXTURE14);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      (GLuint)state->old_underlay_texture_2d);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  if(state->old_scene_texture_valid && gl->active_texture &&
     gl->bind_texture) {
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      (GLuint)state->old_scene_texture_2d);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  end_synthetic_surface_state(state);
}

static GLenum synthetic_surface_last_error(void) {
  if(!g_compat.gl_error_check) return 0;
  CaptureGL *gl=capture_gl();
  GLenum err=gl->get_error ? gl->get_error() : 0;
  trshader_compat_note_synthetic_error(err,"synthetic surface draw");
  return err;
}

static void log_synthetic_surface_draw(GLenum mode, GLsizei count,
                                       int has_first, GLint first,
                                       int has_base_vertex, GLint base_vertex,
                                       GLenum err) {
  if(!runtime_verbose_log()) return;
  unsigned int type_index=(unsigned int)g_current_program_type;
  if(type_index>=sizeof(g_synthetic_surface_logged_by_type)/
      sizeof(g_synthetic_surface_logged_by_type[0]))
    type_index=0;
  if(g_synthetic_surface_logged_by_type[type_index]>=32u) return;

  GLint draw_fbo=-1;
  GLint viewport[4]={0,0,0,0};
  GLint color_mask[4]={1,1,1,1};
  GLint stencil=0,scissor=0,alpha=0,blend=0,depth=0,rasterizer=0;
  unsigned int clip_mask=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_integer) {
    shadow_get_integer_or_gl(GL_DRAW_FRAMEBUFFER_BINDING,&draw_fbo);
    shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
    gl->get_integer(GL_COLOR_WRITEMASK,color_mask);
    gl->get_integer(GL_STENCIL_TEST,&stencil);
    gl->get_integer(GL_SCISSOR_TEST,&scissor);
    gl->get_integer(GL_ALPHA_TEST,&alpha);
    gl->get_integer(GL_RASTERIZER_DISCARD,&rasterizer);
    for(int i=0;i<8;i++) {
      GLint clip=0;
      gl->get_integer((GLenum)(GL_CLIP_DISTANCE0+i),&clip);
      if(clip) clip_mask|=1u<<(unsigned int)i;
    }
    shadow_get_integer_or_gl(GL_BLEND,&blend);
    shadow_get_integer_or_gl(GL_DEPTH_TEST,&depth);
  }

  char msg[768];
  if(has_first) {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw arrays frame=%u sourceProgram=%u sourceType=%s mode=0x%X first=%d count=%d opacity=%.2f tint=%.2f reflection=%.2f fbo=%d viewport=%d,%d,%d,%d mask=%d%d%d%d blend=%d depth=%d stencil=%d scissor=%d alpha=%d rasterizer=%d clip=0x%02X glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)first,(int)count,
      (double)g_runtime_synthetic_opacity,(double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,draw_fbo,
      viewport[0],viewport[1],viewport[2],viewport[3],
      color_mask[0],color_mask[1],color_mask[2],color_mask[3],
      blend,depth,stencil,scissor,alpha,rasterizer,clip_mask,
      (unsigned int)err);
  } else if(has_base_vertex) {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw basevertex frame=%u sourceProgram=%u sourceType=%s mode=0x%X count=%d base=%d opacity=%.2f tint=%.2f reflection=%.2f fbo=%d viewport=%d,%d,%d,%d mask=%d%d%d%d blend=%d depth=%d stencil=%d scissor=%d alpha=%d rasterizer=%d clip=0x%02X glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,(int)base_vertex,
      (double)g_runtime_synthetic_opacity,
      (double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,draw_fbo,
      viewport[0],viewport[1],viewport[2],viewport[3],
      color_mask[0],color_mask[1],color_mask[2],color_mask[3],
      blend,depth,stencil,scissor,alpha,rasterizer,clip_mask,
      (unsigned int)err);
  } else {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw frame=%u sourceProgram=%u sourceType=%s mode=0x%X count=%d opacity=%.2f tint=%.2f reflection=%.2f fbo=%d viewport=%d,%d,%d,%d mask=%d%d%d%d blend=%d depth=%d stencil=%d scissor=%d alpha=%d rasterizer=%d clip=0x%02X glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,
      (double)g_runtime_synthetic_opacity,(double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,draw_fbo,
      viewport[0],viewport[1],viewport[2],viewport[3],
      color_mask[0],color_mask[1],color_mask[2],color_mask[3],
      blend,depth,stencil,scissor,alpha,rasterizer,clip_mask,
      (unsigned int)err);
  }
  log_line(msg);
  g_synthetic_surface_logged++;
  g_synthetic_surface_logged_by_type[type_index]++;
}

static void draw_synthetic_surface_elements(GLenum mode, GLsizei count,
                                            GLenum type, const void *indices) {
  PFNGLDRAWELEMENTS draw=real_draw_elements();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays(GLenum mode, GLint first, GLsizei count) {
  PFNGLDRAWARRAYS draw=real_draw_arrays();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,first,count);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,1,first,0,0,0,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_base_vertex(GLenum mode, GLsizei count,
                                                        GLenum type, const void *indices,
                                                        GLint base_vertex) {
  PFNGLDRAWELEMENTSBASEVERTEX draw=real_draw_elements_base_vertex();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices,base_vertex);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,base_vertex))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,base_vertex);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_range_elements(GLenum mode, GLuint start, GLuint end,
                                                  GLsizei count, GLenum type,
                                                  const void *indices) {
  PFNGLDRAWRANGEELEMENTS draw=real_draw_range_elements();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,start,end,count,type,indices);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,start,end,count,type,indices);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_range_elements_base_vertex(GLenum mode, GLuint start,
                                                              GLuint end, GLsizei count,
                                                              GLenum type, const void *indices,
                                                              GLint base_vertex) {
  PFNGLDRAWRANGEELEMENTSBASEVERTEX draw=real_draw_range_elements_base_vertex();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,start,end,count,type,indices,base_vertex);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,base_vertex))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,start,end,count,type,indices,base_vertex);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays_instanced(GLenum mode, GLint first,
                                                    GLsizei count, GLsizei instance_count) {
  PFNGLDRAWARRAYSINSTANCED draw=real_draw_arrays_instanced();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,first,count,instance_count);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,1,first,0,0,0,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count,instance_count);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced(GLenum mode, GLsizei count,
                                                      GLenum type, const void *indices,
                                                      GLsizei instance_count) {
  PFNGLDRAWELEMENTSINSTANCED draw=real_draw_elements_instanced();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices,instance_count);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_vertex(GLenum mode, GLsizei count,
                                                                  GLenum type, const void *indices,
                                                                  GLsizei instance_count,
                                                                  GLint base_vertex) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw=real_draw_elements_instanced_base_vertex();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices,instance_count,base_vertex);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,base_vertex))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_vertex);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays_instanced_base_instance(GLenum mode, GLint first,
                                                                  GLsizei count,
                                                                  GLsizei instance_count,
                                                                  GLuint base_instance) {
  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE draw=real_draw_arrays_instanced_base_instance();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,first,count,instance_count,base_instance);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,1,first,0,0,0,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count,instance_count,base_instance);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_instance(GLenum mode, GLsizei count,
                                                                    GLenum type, const void *indices,
                                                                    GLsizei instance_count,
                                                                    GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE draw=real_draw_elements_instanced_base_instance();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices,instance_count,base_instance);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,0))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_instance);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_vertex_base_instance(
    GLenum mode, GLsizei count, GLenum type, const void *indices,
    GLsizei instance_count, GLint base_vertex, GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE draw=
    real_draw_elements_instanced_base_vertex_base_instance();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,count,1)) {
    if(!begin_flow_lite_surface_draw(mode,count,1,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!tr456_prepare_synthetic_surface_geometry(
       mode,count,1,0,0,1,type,indices,base_vertex))
    return;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static int synthetic_multi_draw_has_candidate(GLenum mode, const GLsizei *count,
                                              GLsizei draw_count, int *has_flow) {
  int found=0;
  if(has_flow) *has_flow=0;
  if(!count || draw_count<=0) return 0;
  for(GLsizei i=0;i<draw_count;i++) {
    if(count[i]<=0) continue;
    const SyntheticDrawDecision *decision=
      current_synthetic_draw_decision(mode,count[i],1);
    if(decision->synthetic_surface) {
      found=1;
      if(has_flow && decision->synthetic_flow)
        *has_flow=1;
    }
  }
  return found;
}

static int synthetic_multi_draw_should_skip_original(GLenum mode, const GLsizei *count,
                                                     GLsizei draw_count) {
  int saw=0;
  if(!count || draw_count<=0) return 0;
  if(g_current_program_type==SHADER_WATER_FLOW && g_runtime_flow_lite_surface) {
    if(!g_flow_lite_surface.ready) return 0;
  } else if(!g_synthetic_surface.ready) {
    return 0;
  }
  for(GLsizei i=0;i<draw_count;i++) {
    if(count[i]<=0) continue;
    const SyntheticDrawDecision *decision=
      current_synthetic_draw_decision(mode,count[i],1);
    if(!decision->synthetic_surface)
      return 0;
    saw=1;
    if(!decision->skip_original)
      return 0;
  }
  return saw;
}

static void draw_synthetic_surface_arrays_indirect(GLenum mode, const void *indirect) {
  PFNGLDRAWARRAYSINDIRECT draw=real_draw_arrays_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,0,0)) {
    if(!begin_flow_lite_surface_draw(mode,0,0,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,indirect);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,-1,1,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,indirect);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,-1,1,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_indirect(GLenum mode, GLenum type,
                                                     const void *indirect) {
  PFNGLDRAWELEMENTSINDIRECT draw=real_draw_elements_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,0,0)) {
    if(!begin_flow_lite_surface_draw(mode,0,0,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,type,indirect);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,-1,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,type,indirect);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,-1,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_multi_arrays_indirect(GLenum mode, const void *indirect,
                                                         GLsizei draw_count, GLsizei stride) {
  PFNGLMULTIDRAWARRAYSINDIRECT draw=real_multi_draw_arrays_indirect();
  if(!draw || draw_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,0,0)) {
    if(!begin_flow_lite_surface_draw(mode,0,0,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,indirect,draw_count,stride);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,draw_count,1,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,indirect,draw_count,stride);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,draw_count,1,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_multi_elements_indirect(GLenum mode, GLenum type,
                                                           const void *indirect,
                                                           GLsizei draw_count,
                                                           GLsizei stride) {
  PFNGLMULTIDRAWELEMENTSINDIRECT draw=real_multi_draw_elements_indirect();
  if(!draw || draw_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(current_draw_uses_flow_lite(mode,0,0)) {
    if(!begin_flow_lite_surface_draw(mode,0,0,&state)) return;
    unsigned long long perf_t0=perf_ticks_now();
    draw(mode,type,indirect,draw_count,stride);
    perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
    log_synthetic_surface_draw(mode,draw_count,0,0,0,0,synthetic_surface_last_error());
    end_flow_lite_surface_draw(&state);
    return;
  }

  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,type,indirect,draw_count,stride);
  perf_note_synthetic_draw(perf_ticks_elapsed(perf_t0));
  log_synthetic_surface_draw(mode,draw_count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

__declspec(dllexport) void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  runtime_start_once();
  PFNGLDRAWARRAYS real=real_draw_arrays();
  if(real) {
    note_draw("glDrawArrays",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface arrays","synthetic water surface arrays");
    if(!decision->skip_original) {
      real(mode,first,count);
      tr456_wet_lara_draw_arrays("glDrawArrays",real,mode,first,count);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_arrays(mode,first,count);
  }
}

static PFNGLDRAWRANGEELEMENTS real_draw_range_elements(void) {
  static PFNGLDRAWRANGEELEMENTS p;
  if(!p) p=(PFNGLDRAWRANGEELEMENTS)gl_proc("glDrawRangeElements");
  return p;
}

static void APIENTRY hook_glDrawRangeElements(GLenum mode, GLuint start, GLuint end,
                                              GLsizei count, GLenum type, const void *indices) {
  PFNGLDRAWRANGEELEMENTS real=real_draw_range_elements();
  if(real) {
    note_draw("glDrawRangeElements",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface range","synthetic water surface range");
    if(!decision->skip_original) {
      real(mode,start,end,count,type,indices);
      tr456_wet_lara_draw_range_elements("glDrawRangeElements",real,mode,
        start,end,count,type,indices);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_range_elements(mode,start,end,count,type,indices);
  }
}

static PFNGLDRAWELEMENTSBASEVERTEX real_draw_elements_base_vertex(void) {
  static PFNGLDRAWELEMENTSBASEVERTEX p;
  if(!p) p=(PFNGLDRAWELEMENTSBASEVERTEX)gl_proc("glDrawElementsBaseVertex");
  return p;
}

static void APIENTRY hook_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type,
                                                   const void *indices, GLint base_vertex) {
  PFNGLDRAWELEMENTSBASEVERTEX real=real_draw_elements_base_vertex();
  if(real) {
    note_draw("glDrawElementsBaseVertex",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface basevertex",
        "synthetic water surface basevertex");
    if(!decision->skip_original) {
      real(mode,count,type,indices,base_vertex);
      tr456_wet_lara_draw_elements_base_vertex("glDrawElementsBaseVertex",
        real,mode,count,type,indices,base_vertex);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_base_vertex(mode,count,type,indices,base_vertex);
  }
}

static PFNGLDRAWRANGEELEMENTSBASEVERTEX real_draw_range_elements_base_vertex(void) {
  static PFNGLDRAWRANGEELEMENTSBASEVERTEX p;
  if(!p) p=(PFNGLDRAWRANGEELEMENTSBASEVERTEX)gl_proc("glDrawRangeElementsBaseVertex");
  return p;
}

static void APIENTRY hook_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end,
                                                        GLsizei count, GLenum type,
                                                        const void *indices, GLint base_vertex) {
  PFNGLDRAWRANGEELEMENTSBASEVERTEX real=real_draw_range_elements_base_vertex();
  if(real) {
    note_draw("glDrawRangeElementsBaseVertex",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface range basevertex",
        "synthetic water surface range basevertex");
    if(!decision->skip_original) {
      real(mode,start,end,count,type,indices,base_vertex);
      tr456_wet_lara_draw_range_elements_base_vertex(
        "glDrawRangeElementsBaseVertex",real,mode,start,end,count,type,
        indices,base_vertex);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_range_elements_base_vertex(mode,start,end,count,type,indices,base_vertex);
  }
}

static PFNGLDRAWARRAYSINSTANCED real_draw_arrays_instanced(void) {
  static PFNGLDRAWARRAYSINSTANCED p;
  if(!p) p=(PFNGLDRAWARRAYSINSTANCED)gl_proc("glDrawArraysInstanced");
  if(!p) p=(PFNGLDRAWARRAYSINSTANCED)gl_proc("glDrawArraysInstancedARB");
  return p;
}

static void APIENTRY hook_glDrawArraysInstanced(GLenum mode, GLint first,
                                                GLsizei count, GLsizei instance_count) {
  PFNGLDRAWARRAYSINSTANCED real=real_draw_arrays_instanced();
  if(real) {
    note_draw("glDrawArraysInstanced",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface arrays instanced",
        "synthetic water surface arrays instanced");
    if(!decision->skip_original) {
      real(mode,first,count,instance_count);
      tr456_wet_lara_draw_arrays_instanced("glDrawArraysInstanced",real,
        mode,first,count,instance_count);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_arrays_instanced(mode,first,count,instance_count);
  }
}

static PFNGLDRAWELEMENTSINSTANCED real_draw_elements_instanced(void) {
  static PFNGLDRAWELEMENTSINSTANCED p;
  if(!p) p=(PFNGLDRAWELEMENTSINSTANCED)gl_proc("glDrawElementsInstanced");
  if(!p) p=(PFNGLDRAWELEMENTSINSTANCED)gl_proc("glDrawElementsInstancedARB");
  return p;
}

static void APIENTRY hook_glDrawElementsInstanced(GLenum mode, GLsizei count,
                                                  GLenum type, const void *indices,
                                                  GLsizei instance_count) {
  PFNGLDRAWELEMENTSINSTANCED real=real_draw_elements_instanced();
  if(real) {
    note_draw("glDrawElementsInstanced",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface elements instanced",
        "synthetic water surface elements instanced");
    if(!decision->skip_original) {
      real(mode,count,type,indices,instance_count);
      tr456_wet_lara_draw_elements_instanced("glDrawElementsInstanced",
        real,mode,count,type,indices,instance_count);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_instanced(mode,count,type,indices,instance_count);
  }
}

static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX real_draw_elements_instanced_base_vertex(void) {
  static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX p;
  if(!p) p=(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX)gl_proc("glDrawElementsInstancedBaseVertex");
  return p;
}

static void APIENTRY hook_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count,
                                                            GLenum type, const void *indices,
                                                            GLsizei instance_count,
                                                            GLint base_vertex) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX real=real_draw_elements_instanced_base_vertex();
  if(real) {
    note_draw("glDrawElementsInstancedBaseVertex",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface instanced basevertex",
        "synthetic water surface instanced basevertex");
    if(!decision->skip_original) {
      real(mode,count,type,indices,instance_count,base_vertex);
      tr456_wet_lara_draw_elements_instanced_base_vertex(
        "glDrawElementsInstancedBaseVertex",real,mode,count,type,indices,
        instance_count,base_vertex);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_vertex(mode,count,type,indices,instance_count,base_vertex);
  }
}

static PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE real_draw_arrays_instanced_base_instance(void) {
  static PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE p;
  if(!p) p=(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE)gl_proc("glDrawArraysInstancedBaseInstance");
  return p;
}

static void APIENTRY hook_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first,
                                                            GLsizei count, GLsizei instance_count,
                                                            GLuint base_instance) {
  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE real=real_draw_arrays_instanced_base_instance();
  if(real) {
    note_draw("glDrawArraysInstancedBaseInstance",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface arrays instanced baseinstance",
        "synthetic water surface arrays instanced baseinstance");
    if(!decision->skip_original) {
      real(mode,first,count,instance_count,base_instance);
      tr456_wet_lara_draw_arrays_instanced_base_instance(
        "glDrawArraysInstancedBaseInstance",real,mode,first,count,
        instance_count,base_instance);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_arrays_instanced_base_instance(mode,first,count,instance_count,base_instance);
  }
}

static PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE real_draw_elements_instanced_base_instance(void) {
  static PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE p;
  if(!p) p=(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE)gl_proc("glDrawElementsInstancedBaseInstance");
  return p;
}

static void APIENTRY hook_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count,
                                                              GLenum type, const void *indices,
                                                              GLsizei instance_count,
                                                              GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE real=real_draw_elements_instanced_base_instance();
  if(real) {
    note_draw("glDrawElementsInstancedBaseInstance",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface elements instanced baseinstance",
        "synthetic water surface elements instanced baseinstance");
    if(!decision->skip_original) {
      real(mode,count,type,indices,instance_count,base_instance);
      tr456_wet_lara_draw_elements_instanced_base_instance(
        "glDrawElementsInstancedBaseInstance",real,mode,count,type,indices,
        instance_count,base_instance);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_instance(mode,count,type,indices,instance_count,base_instance);
  }
}

static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE real_draw_elements_instanced_base_vertex_base_instance(void) {
  static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE p;
  if(!p) p=(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE)gl_proc("glDrawElementsInstancedBaseVertexBaseInstance");
  return p;
}

static void APIENTRY hook_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count,
                                                                        GLenum type, const void *indices,
                                                                        GLsizei instance_count,
                                                                        GLint base_vertex,
                                                                        GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE real=real_draw_elements_instanced_base_vertex_base_instance();
  if(real) {
    note_draw("glDrawElementsInstancedBaseVertexBaseInstance",mode,count);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,count,1,
        "synthetic flow surface instanced basevertex baseinstance",
        "synthetic water surface instanced basevertex baseinstance");
    if(!decision->skip_original) {
      real(mode,count,type,indices,instance_count,base_vertex,base_instance);
      tr456_wet_lara_draw_elements_instanced_base_vertex_base_instance(
        "glDrawElementsInstancedBaseVertexBaseInstance",real,mode,count,type,
        indices,instance_count,base_vertex,base_instance);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_vertex_base_instance(
        mode,count,type,indices,instance_count,base_vertex,base_instance);
  }
}

static PFNGLMULTIDRAWARRAYS real_multi_draw_arrays(void) {
  static PFNGLMULTIDRAWARRAYS p;
  if(!p) p=(PFNGLMULTIDRAWARRAYS)gl_proc("glMultiDrawArrays");
  if(!p) p=(PFNGLMULTIDRAWARRAYS)gl_proc("glMultiDrawArraysEXT");
  return p;
}

static void APIENTRY hook_glMultiDrawArrays(GLenum mode, const GLint *first,
                                            const GLsizei *count, GLsizei draw_count) {
  PFNGLMULTIDRAWARRAYS real=real_multi_draw_arrays();
  if(real) {
    note_draw("glMultiDrawArrays",mode,draw_count);
    tr456_wet_lara_note_multi_draw("glMultiDrawArrays",count,draw_count);
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_current_synthetic_surface_program();
    int skip_original=synthetic_ready && first && count &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw arrays" : "synthetic water multi draw arrays");
    if(!skip_original) {
      real(mode,first,count,draw_count);
      tr456_wet_lara_multi_draw_arrays("glMultiDrawArrays",
        real_draw_arrays(),mode,first,count,draw_count);
    }
    if(synthetic_ready && first && count) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_synthetic_draw_decision(mode,count[i],1)->synthetic_surface)
          draw_synthetic_surface_arrays(mode,first[i],count[i]);
      }
    }
  }
}

static PFNGLMULTIDRAWELEMENTS real_multi_draw_elements(void) {
  static PFNGLMULTIDRAWELEMENTS p;
  if(!p) p=(PFNGLMULTIDRAWELEMENTS)gl_proc("glMultiDrawElements");
  if(!p) p=(PFNGLMULTIDRAWELEMENTS)gl_proc("glMultiDrawElementsEXT");
  return p;
}

static void APIENTRY hook_glMultiDrawElements(GLenum mode, const GLsizei *count,
                                              GLenum type, const void * const *indices,
                                              GLsizei draw_count) {
  PFNGLMULTIDRAWELEMENTS real=real_multi_draw_elements();
  if(real) {
    note_draw("glMultiDrawElements",mode,draw_count);
    tr456_wet_lara_note_multi_draw("glMultiDrawElements",count,draw_count);
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_current_synthetic_surface_program();
    int skip_original=synthetic_ready && count && indices &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw elements" : "synthetic water multi draw elements");
    if(!skip_original) {
      real(mode,count,type,indices,draw_count);
      tr456_wet_lara_multi_draw_elements("glMultiDrawElements",
        real_draw_elements(),mode,count,type,indices,draw_count);
    }
    if(synthetic_ready && count && indices) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_synthetic_draw_decision(mode,count[i],1)->synthetic_surface)
          draw_synthetic_surface_elements(mode,count[i],type,indices[i]);
      }
    }
  }
}

static PFNGLMULTIDRAWELEMENTSBASEVERTEX real_multi_draw_elements_base_vertex(void) {
  static PFNGLMULTIDRAWELEMENTSBASEVERTEX p;
  if(!p) p=(PFNGLMULTIDRAWELEMENTSBASEVERTEX)gl_proc("glMultiDrawElementsBaseVertex");
  return p;
}

static void APIENTRY hook_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count,
                                                        GLenum type, const void * const *indices,
                                                        GLsizei draw_count, const GLint *base_vertex) {
  PFNGLMULTIDRAWELEMENTSBASEVERTEX real=real_multi_draw_elements_base_vertex();
  if(real) {
    note_draw("glMultiDrawElementsBaseVertex",mode,draw_count);
    tr456_wet_lara_note_multi_draw("glMultiDrawElementsBaseVertex",count,
      draw_count);
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_current_synthetic_surface_program();
    int skip_original=synthetic_ready && count && indices && base_vertex &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw elements base vertex" :
        "synthetic water multi draw elements base vertex");
    if(!skip_original) {
      real(mode,count,type,indices,draw_count,base_vertex);
      tr456_wet_lara_multi_draw_elements_base_vertex(
        "glMultiDrawElementsBaseVertex",real_draw_elements_base_vertex(),
        mode,count,type,indices,draw_count,base_vertex);
    }
    if(synthetic_ready && count && indices && base_vertex) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_synthetic_draw_decision(mode,count[i],1)->synthetic_surface)
          draw_synthetic_surface_elements_base_vertex(
            mode,count[i],type,indices[i],base_vertex[i]);
      }
    }
  }
}

static PFNGLDRAWARRAYSINDIRECT real_draw_arrays_indirect(void) {
  static PFNGLDRAWARRAYSINDIRECT p;
  if(!p) p=(PFNGLDRAWARRAYSINDIRECT)gl_proc("glDrawArraysIndirect");
  return p;
}

static void APIENTRY hook_glDrawArraysIndirect(GLenum mode, const void *indirect) {
  PFNGLDRAWARRAYSINDIRECT real=real_draw_arrays_indirect();
  if(real) {
    note_draw("glDrawArraysIndirect",mode,-1);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,0,0,
        "synthetic flow arrays indirect","synthetic water arrays indirect");
    if(!(decision->synthetic_surface && decision->skip_original)) {
      real(mode,indirect);
      tr456_wet_lara_draw_arrays_indirect("glDrawArraysIndirect",real,
        mode,indirect);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_arrays_indirect(mode,indirect);
  }
}

static PFNGLDRAWELEMENTSINDIRECT real_draw_elements_indirect(void) {
  static PFNGLDRAWELEMENTSINDIRECT p;
  if(!p) p=(PFNGLDRAWELEMENTSINDIRECT)gl_proc("glDrawElementsIndirect");
  return p;
}

static void APIENTRY hook_glDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect) {
  PFNGLDRAWELEMENTSINDIRECT real=real_draw_elements_indirect();
  if(real) {
    note_draw("glDrawElementsIndirect",mode,-1);
    const SyntheticDrawDecision *decision=
      prepare_synthetic_surface_draw_decision(mode,0,0,
        "synthetic flow elements indirect","synthetic water elements indirect");
    if(!(decision->synthetic_surface && decision->skip_original)) {
      real(mode,type,indirect);
      tr456_wet_lara_draw_elements_indirect("glDrawElementsIndirect",real,
        mode,type,indirect);
    }
    if(decision->synthetic_ready)
      draw_synthetic_surface_elements_indirect(mode,type,indirect);
  }
}

static PFNGLMULTIDRAWARRAYSINDIRECT real_multi_draw_arrays_indirect(void) {
  static PFNGLMULTIDRAWARRAYSINDIRECT p;
  if(!p) p=(PFNGLMULTIDRAWARRAYSINDIRECT)gl_proc("glMultiDrawArraysIndirect");
  return p;
}

static void APIENTRY hook_glMultiDrawArraysIndirect(GLenum mode, const void *indirect,
                                                    GLsizei draw_count, GLsizei stride) {
  PFNGLMULTIDRAWARRAYSINDIRECT real=real_multi_draw_arrays_indirect();
  if(real) {
    note_draw("glMultiDrawArraysIndirect",mode,draw_count);
    const SyntheticDrawDecision *decision=draw_count>0 ?
      prepare_synthetic_surface_draw_decision(mode,0,0,
        "synthetic flow multi arrays indirect",
        "synthetic water multi arrays indirect") :
      current_synthetic_draw_decision(mode,0,0);
    int synthetic_ready=draw_count>0 && decision->synthetic_ready;
    int skip_original=draw_count>0 && decision->synthetic_surface &&
      decision->skip_original;
    if(!skip_original) {
      real(mode,indirect,draw_count,stride);
      tr456_wet_lara_multi_draw_arrays_indirect(
        "glMultiDrawArraysIndirect",real,mode,indirect,draw_count,stride);
    }
    if(synthetic_ready)
      draw_synthetic_surface_multi_arrays_indirect(mode,indirect,draw_count,stride);
  }
}

static PFNGLMULTIDRAWELEMENTSINDIRECT real_multi_draw_elements_indirect(void) {
  static PFNGLMULTIDRAWELEMENTSINDIRECT p;
  if(!p) p=(PFNGLMULTIDRAWELEMENTSINDIRECT)gl_proc("glMultiDrawElementsIndirect");
  return p;
}

static void APIENTRY hook_glMultiDrawElementsIndirect(GLenum mode, GLenum type,
                                                      const void *indirect,
                                                      GLsizei draw_count, GLsizei stride) {
  PFNGLMULTIDRAWELEMENTSINDIRECT real=real_multi_draw_elements_indirect();
  if(real) {
    note_draw("glMultiDrawElementsIndirect",mode,draw_count);
    const SyntheticDrawDecision *decision=draw_count>0 ?
      prepare_synthetic_surface_draw_decision(mode,0,0,
        "synthetic flow multi elements indirect",
        "synthetic water multi elements indirect") :
      current_synthetic_draw_decision(mode,0,0);
    int synthetic_ready=draw_count>0 && decision->synthetic_ready;
    int skip_original=draw_count>0 && decision->synthetic_surface &&
      decision->skip_original;
    if(!skip_original) {
      real(mode,type,indirect,draw_count,stride);
      tr456_wet_lara_multi_draw_elements_indirect(
        "glMultiDrawElementsIndirect",real,mode,type,indirect,draw_count,
        stride);
    }
    if(synthetic_ready)
      draw_synthetic_surface_multi_elements_indirect(mode,type,indirect,draw_count,stride);
  }
}

#define TR456_WGL_DRAW_TO_WINDOW_ARB 0x2001
#define TR456_WGL_DRAW_TO_BITMAP_ARB 0x2002
#define TR456_WGL_ACCELERATION_ARB 0x2003
#define TR456_WGL_SUPPORT_OPENGL_ARB 0x2010
#define TR456_WGL_DOUBLE_BUFFER_ARB 0x2011
#define TR456_WGL_STEREO_ARB 0x2012
#define TR456_WGL_PIXEL_TYPE_ARB 0x2013
#define TR456_WGL_COLOR_BITS_ARB 0x2014
#define TR456_WGL_RED_BITS_ARB 0x2015
#define TR456_WGL_GREEN_BITS_ARB 0x2017
#define TR456_WGL_BLUE_BITS_ARB 0x2019
#define TR456_WGL_ALPHA_BITS_ARB 0x201B
#define TR456_WGL_DEPTH_BITS_ARB 0x2022
#define TR456_WGL_STENCIL_BITS_ARB 0x2023
#define TR456_WGL_TYPE_RGBA_ARB 0x202B
#define TR456_WGL_TYPE_COLORINDEX_ARB 0x202C
