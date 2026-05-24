#define TR456_SHADOW_TEX_UNITS 32
#define TR456_SHADOW_UNIFORM_SLOTS 4096
#define TR456_SHADOW_UNIFORM_FLOATS 64
#define TR456_SHADOW_UNIFORM_INTS 16

typedef struct {
  GLuint program;
  GLint loc;
  GLsizei float_count;
  GLfloat floats[TR456_SHADOW_UNIFORM_FLOATS];
  GLsizei int_count;
  GLint ints[TR456_SHADOW_UNIFORM_INTS];
} TrshaderShadowUniform;

typedef struct {
  int initialized;
  GLint current_program;
  int current_program_valid;
  GLint active_texture;
  int active_texture_valid;
  GLuint tex2d[TR456_SHADOW_TEX_UNITS];
  GLuint tex2d_array[TR456_SHADOW_TEX_UNITS];
  unsigned int tex2d_valid_mask;
  unsigned int tex2d_array_valid_mask;
  GLint viewport[4];
  int viewport_valid;
  GLint read_fbo;
  GLint draw_fbo;
  int read_fbo_valid;
  int draw_fbo_valid;
  GLint blend;
  GLint depth_test;
  GLint cull_face;
  GLint depth_mask;
  GLint depth_func;
  GLint blend_src_rgb;
  GLint blend_dst_rgb;
  GLint blend_src_alpha;
  GLint blend_dst_alpha;
  unsigned int state_valid_mask;
} TrshaderShadowState;

enum {
  TR456_SHADOW_BLEND_VALID=1u<<0,
  TR456_SHADOW_DEPTH_TEST_VALID=1u<<1,
  TR456_SHADOW_CULL_FACE_VALID=1u<<2,
  TR456_SHADOW_DEPTH_MASK_VALID=1u<<3,
  TR456_SHADOW_DEPTH_FUNC_VALID=1u<<4,
  TR456_SHADOW_BLEND_FUNC_VALID=1u<<5
};

static TrshaderShadowState g_shadow_state;
static TrshaderShadowUniform g_shadow_uniforms[TR456_SHADOW_UNIFORM_SLOTS];

static void shadow_init_defaults(void) {
  if(g_shadow_state.initialized) return;
  memset(&g_shadow_state,0,sizeof(g_shadow_state));
  g_shadow_state.initialized=1;
  g_shadow_state.current_program=0;
  g_shadow_state.current_program_valid=1;
  g_shadow_state.active_texture=GL_TEXTURE0;
  g_shadow_state.active_texture_valid=1;
  g_shadow_state.read_fbo=0;
  g_shadow_state.draw_fbo=0;
  g_shadow_state.read_fbo_valid=1;
  g_shadow_state.draw_fbo_valid=1;
  g_shadow_state.blend=0;
  g_shadow_state.depth_test=0;
  g_shadow_state.cull_face=0;
  g_shadow_state.depth_mask=1;
  g_shadow_state.depth_func=GL_LESS;
  g_shadow_state.blend_src_rgb=GL_ONE;
  g_shadow_state.blend_dst_rgb=GL_ZERO;
  g_shadow_state.blend_src_alpha=GL_ONE;
  g_shadow_state.blend_dst_alpha=GL_ZERO;
  g_shadow_state.state_valid_mask=TR456_SHADOW_BLEND_VALID|
    TR456_SHADOW_DEPTH_TEST_VALID|TR456_SHADOW_CULL_FACE_VALID|
    TR456_SHADOW_DEPTH_MASK_VALID|TR456_SHADOW_DEPTH_FUNC_VALID|
    TR456_SHADOW_BLEND_FUNC_VALID;
  g_shadow_state.tex2d_valid_mask=0xFFFFFFFFu;
  g_shadow_state.tex2d_array_valid_mask=0xFFFFFFFFu;
}

static int shadow_texture_unit_index(GLenum texture) {
  if(texture<GL_TEXTURE0) return -1;
  unsigned int unit=(unsigned int)(texture-GL_TEXTURE0);
  return unit<TR456_SHADOW_TEX_UNITS ? (int)unit : -1;
}

static int shadow_active_unit_index(void) {
  shadow_init_defaults();
  return shadow_texture_unit_index((GLenum)g_shadow_state.active_texture);
}

static void shadow_note_use_program(GLuint program) {
  shadow_init_defaults();
  g_shadow_state.current_program=(GLint)program;
  g_shadow_state.current_program_valid=1;
}

static void shadow_note_active_texture(GLenum texture) {
  shadow_init_defaults();
  if(shadow_texture_unit_index(texture)>=0) {
    g_shadow_state.active_texture=(GLint)texture;
    g_shadow_state.active_texture_valid=1;
  } else {
    g_shadow_state.active_texture_valid=0;
  }
}

static void shadow_note_bind_texture(GLenum target, GLuint texture) {
  shadow_init_defaults();
  int unit=shadow_active_unit_index();
  if(unit<0) return;
  unsigned int bit=1u<<(unsigned int)unit;
  if(target==GL_TEXTURE_2D) {
    g_shadow_state.tex2d[unit]=texture;
    g_shadow_state.tex2d_valid_mask|=bit;
  } else if(target==GL_TEXTURE_2D_ARRAY) {
    g_shadow_state.tex2d_array[unit]=texture;
    g_shadow_state.tex2d_array_valid_mask|=bit;
  }
}

static void shadow_note_bind_framebuffer(GLenum target, GLuint framebuffer) {
  shadow_init_defaults();
  if(target==GL_FRAMEBUFFER || target==GL_READ_FRAMEBUFFER) {
    g_shadow_state.read_fbo=(GLint)framebuffer;
    g_shadow_state.read_fbo_valid=1;
  }
  if(target==GL_FRAMEBUFFER || target==GL_DRAW_FRAMEBUFFER) {
    g_shadow_state.draw_fbo=(GLint)framebuffer;
    g_shadow_state.draw_fbo_valid=1;
  }
}

static void shadow_note_viewport(GLint x, GLint y, GLsizei w, GLsizei h) {
  shadow_init_defaults();
  g_shadow_state.viewport[0]=x;
  g_shadow_state.viewport[1]=y;
  g_shadow_state.viewport[2]=w;
  g_shadow_state.viewport[3]=h;
  g_shadow_state.viewport_valid=1;
}

static void shadow_note_enable(GLenum cap, int enabled) {
  shadow_init_defaults();
  if(cap==GL_BLEND) {
    g_shadow_state.blend=enabled ? 1 : 0;
    g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_VALID;
  } else if(cap==GL_DEPTH_TEST) {
    g_shadow_state.depth_test=enabled ? 1 : 0;
    g_shadow_state.state_valid_mask|=TR456_SHADOW_DEPTH_TEST_VALID;
  } else if(cap==GL_CULL_FACE) {
    g_shadow_state.cull_face=enabled ? 1 : 0;
    g_shadow_state.state_valid_mask|=TR456_SHADOW_CULL_FACE_VALID;
  }
}

static void shadow_note_depth_mask(GLboolean flag) {
  shadow_init_defaults();
  g_shadow_state.depth_mask=flag ? 1 : 0;
  g_shadow_state.state_valid_mask|=TR456_SHADOW_DEPTH_MASK_VALID;
}

static void shadow_note_depth_func(GLenum func) {
  shadow_init_defaults();
  g_shadow_state.depth_func=(GLint)func;
  g_shadow_state.state_valid_mask|=TR456_SHADOW_DEPTH_FUNC_VALID;
}

static void shadow_note_blend_func(GLenum src_rgb, GLenum dst_rgb,
                                   GLenum src_alpha, GLenum dst_alpha) {
  shadow_init_defaults();
  g_shadow_state.blend_src_rgb=(GLint)src_rgb;
  g_shadow_state.blend_dst_rgb=(GLint)dst_rgb;
  g_shadow_state.blend_src_alpha=(GLint)src_alpha;
  g_shadow_state.blend_dst_alpha=(GLint)dst_alpha;
  g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_FUNC_VALID;
}

static int shadow_get_integer(GLenum pname, GLint *out) {
  if(!out) return 0;
  shadow_init_defaults();
  switch(pname) {
    case GL_CURRENT_PROGRAM:
      if(g_shadow_state.current_program_valid) { *out=g_shadow_state.current_program; return 1; }
      return 0;
    case GL_ACTIVE_TEXTURE:
      if(g_shadow_state.active_texture_valid) { *out=g_shadow_state.active_texture; return 1; }
      return 0;
    case GL_VIEWPORT:
      if(g_shadow_state.viewport_valid) {
        memcpy(out,g_shadow_state.viewport,sizeof(g_shadow_state.viewport));
        return 1;
      }
      return 0;
    case GL_READ_FRAMEBUFFER_BINDING:
      if(g_shadow_state.read_fbo_valid) { *out=g_shadow_state.read_fbo; return 1; }
      return 0;
    case GL_DRAW_FRAMEBUFFER_BINDING:
      if(g_shadow_state.draw_fbo_valid) { *out=g_shadow_state.draw_fbo; return 1; }
      return 0;
    case GL_BLEND:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_BLEND_VALID) { *out=g_shadow_state.blend; return 1; }
      return 0;
    case GL_DEPTH_TEST:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_DEPTH_TEST_VALID) { *out=g_shadow_state.depth_test; return 1; }
      return 0;
    case GL_CULL_FACE:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_CULL_FACE_VALID) { *out=g_shadow_state.cull_face; return 1; }
      return 0;
    case GL_DEPTH_WRITEMASK:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_DEPTH_MASK_VALID) { *out=g_shadow_state.depth_mask; return 1; }
      return 0;
    case GL_DEPTH_FUNC:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_DEPTH_FUNC_VALID) { *out=g_shadow_state.depth_func; return 1; }
      return 0;
    case GL_BLEND_SRC_RGB:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_BLEND_FUNC_VALID) { *out=g_shadow_state.blend_src_rgb; return 1; }
      return 0;
    case GL_BLEND_DST_RGB:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_BLEND_FUNC_VALID) { *out=g_shadow_state.blend_dst_rgb; return 1; }
      return 0;
    case GL_BLEND_SRC_ALPHA:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_BLEND_FUNC_VALID) { *out=g_shadow_state.blend_src_alpha; return 1; }
      return 0;
    case GL_BLEND_DST_ALPHA:
      if(g_shadow_state.state_valid_mask&TR456_SHADOW_BLEND_FUNC_VALID) { *out=g_shadow_state.blend_dst_alpha; return 1; }
      return 0;
  }
  return 0;
}

static void shadow_seed_integer(GLenum pname, const GLint *value) {
  if(!value) return;
  shadow_init_defaults();
  switch(pname) {
    case GL_CURRENT_PROGRAM:
      g_shadow_state.current_program=value[0];
      g_shadow_state.current_program_valid=1;
      return;
    case GL_ACTIVE_TEXTURE:
      shadow_note_active_texture((GLenum)value[0]);
      return;
    case GL_VIEWPORT:
      shadow_note_viewport(value[0],value[1],value[2],value[3]);
      return;
    case GL_READ_FRAMEBUFFER_BINDING:
      g_shadow_state.read_fbo=value[0];
      g_shadow_state.read_fbo_valid=1;
      return;
    case GL_DRAW_FRAMEBUFFER_BINDING:
      g_shadow_state.draw_fbo=value[0];
      g_shadow_state.draw_fbo_valid=1;
      return;
    case GL_BLEND:
      shadow_note_enable(GL_BLEND,value[0]!=0);
      return;
    case GL_DEPTH_TEST:
      shadow_note_enable(GL_DEPTH_TEST,value[0]!=0);
      return;
    case GL_CULL_FACE:
      shadow_note_enable(GL_CULL_FACE,value[0]!=0);
      return;
    case GL_DEPTH_WRITEMASK:
      shadow_note_depth_mask((GLboolean)(value[0]!=0));
      return;
    case GL_DEPTH_FUNC:
      shadow_note_depth_func((GLenum)value[0]);
      return;
    case GL_BLEND_SRC_RGB:
      g_shadow_state.blend_src_rgb=value[0];
      g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_FUNC_VALID;
      return;
    case GL_BLEND_DST_RGB:
      g_shadow_state.blend_dst_rgb=value[0];
      g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_FUNC_VALID;
      return;
    case GL_BLEND_SRC_ALPHA:
      g_shadow_state.blend_src_alpha=value[0];
      g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_FUNC_VALID;
      return;
    case GL_BLEND_DST_ALPHA:
      g_shadow_state.blend_dst_alpha=value[0];
      g_shadow_state.state_valid_mask|=TR456_SHADOW_BLEND_FUNC_VALID;
      return;
  }
}

static int shadow_get_integer_or_gl(GLenum pname, GLint *out) {
  if(shadow_get_integer(pname,out)) return 1;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_integer || !out) return 0;
  gl->get_integer(pname,out);
  shadow_seed_integer(pname,out);
  return 1;
}

static int shadow_get_bound_texture(GLenum target, GLint unit, GLuint *texture) {
  if(!texture || unit<0 || unit>=TR456_SHADOW_TEX_UNITS) return 0;
  shadow_init_defaults();
  unsigned int bit=1u<<(unsigned int)unit;
  if(target==GL_TEXTURE_2D &&
     (g_shadow_state.tex2d_valid_mask&bit)) {
    *texture=g_shadow_state.tex2d[unit];
    return 1;
  }
  if(target==GL_TEXTURE_2D_ARRAY &&
     (g_shadow_state.tex2d_array_valid_mask&bit)) {
    *texture=g_shadow_state.tex2d_array[unit];
    return 1;
  }
  return 0;
}

static unsigned int shadow_uniform_hash(GLuint program, GLint loc) {
  unsigned int h=(unsigned int)program*2654435761u;
  h^=(unsigned int)loc*2246822519u;
  return h&(TR456_SHADOW_UNIFORM_SLOTS-1u);
}

static TrshaderShadowUniform *shadow_uniform_slot(GLuint program, GLint loc,
                                                  int create) {
  if(!program || loc<0) return 0;
  unsigned int start=shadow_uniform_hash(program,loc);
  for(unsigned int i=0;i<TR456_SHADOW_UNIFORM_SLOTS;i++) {
    unsigned int idx=(start+i)&(TR456_SHADOW_UNIFORM_SLOTS-1u);
    TrshaderShadowUniform *u=&g_shadow_uniforms[idx];
    if(u->program==program && u->loc==loc) return u;
    if(!u->program) {
      if(!create) return 0;
      memset(u,0,sizeof(*u));
      u->program=program;
      u->loc=loc;
      return u;
    }
  }
  return 0;
}

static void shadow_note_uniform_floats(GLuint program, GLint loc,
                                       const GLfloat *values, GLsizei count) {
  if(!values || count<=0) return;
  TrshaderShadowUniform *u=shadow_uniform_slot(program,loc,1);
  if(!u) return;
  if(count>TR456_SHADOW_UNIFORM_FLOATS) count=TR456_SHADOW_UNIFORM_FLOATS;
  memcpy(u->floats,values,(size_t)count*sizeof(GLfloat));
  u->float_count=count;
}

static void shadow_note_uniform_ints(GLuint program, GLint loc,
                                     const GLint *values, GLsizei count) {
  if(!values || count<=0) return;
  TrshaderShadowUniform *u=shadow_uniform_slot(program,loc,1);
  if(!u) return;
  if(count>TR456_SHADOW_UNIFORM_INTS) count=TR456_SHADOW_UNIFORM_INTS;
  memcpy(u->ints,values,(size_t)count*sizeof(GLint));
  u->int_count=count;
}

static int shadow_read_uniform_floats(GLuint program, GLint loc,
                                      GLfloat *out, GLsizei count) {
  if(!out || count<=0) return 0;
  TrshaderShadowUniform *u=shadow_uniform_slot(program,loc,0);
  if(!u || u->float_count<count) return 0;
  memcpy(out,u->floats,(size_t)count*sizeof(GLfloat));
  return 1;
}

static int shadow_read_uniform_int(GLuint program, GLint loc, GLint *out) {
  if(!out) return 0;
  TrshaderShadowUniform *u=shadow_uniform_slot(program,loc,0);
  if(!u || u->int_count<1) return 0;
  *out=u->ints[0];
  return 1;
}

static void shadow_note_uniform_1i(GLint loc, GLint v0) {
  shadow_init_defaults();
  GLint vals[1]={v0};
  shadow_note_uniform_ints((GLuint)g_shadow_state.current_program,loc,vals,1);
}

static void shadow_note_uniform_1iv(GLint loc, GLsizei count, const GLint *v) {
  shadow_init_defaults();
  if(count<=0 || !v) return;
  shadow_note_uniform_ints((GLuint)g_shadow_state.current_program,loc,v,count);
  for(GLsizei i=1;i<count && i<TR456_SHADOW_UNIFORM_INTS;i++)
    shadow_note_uniform_ints((GLuint)g_shadow_state.current_program,loc+i,v+i,1);
}

static void shadow_note_uniform_4f(GLint loc, GLfloat x, GLfloat y,
                                   GLfloat z, GLfloat w) {
  shadow_init_defaults();
  GLfloat vals[4]={x,y,z,w};
  shadow_note_uniform_floats((GLuint)g_shadow_state.current_program,loc,vals,4);
}

static void shadow_note_uniform_4fv(GLint loc, GLsizei count,
                                    const GLfloat *v) {
  shadow_init_defaults();
  if(count<=0 || !v) return;
  GLsizei floats=count*4;
  shadow_note_uniform_floats((GLuint)g_shadow_state.current_program,loc,v,floats);
  for(GLsizei i=0;i<count;i++)
    shadow_note_uniform_floats((GLuint)g_shadow_state.current_program,
      loc+i,v+i*4,4);
}

static void shadow_note_uniform_matrix4fv(GLint loc, GLsizei count,
                                          GLboolean transpose,
                                          const GLfloat *v) {
  shadow_init_defaults();
  if(count<=0 || !v) return;
  GLfloat tmp[16];
  const GLfloat *src=v;
  if(transpose && count==1) {
    for(int r=0;r<4;r++)
      for(int c=0;c<4;c++)
        tmp[c*4+r]=v[r*4+c];
    src=tmp;
  }
  GLsizei floats=count*16;
  if(floats>TR456_SHADOW_UNIFORM_FLOATS)
    floats=TR456_SHADOW_UNIFORM_FLOATS;
  shadow_note_uniform_floats((GLuint)g_shadow_state.current_program,loc,src,floats);
}

static void shadow_call_active_texture(CaptureGL *gl, GLenum texture) {
  if(gl && gl->active_texture) {
    gl->active_texture(texture);
    shadow_note_active_texture(texture);
  }
}

static void shadow_call_bind_texture(CaptureGL *gl, GLenum target, GLuint texture) {
  if(gl && gl->bind_texture) {
    gl->bind_texture(target,texture);
    shadow_note_bind_texture(target,texture);
  }
}

static void shadow_call_bind_framebuffer(CaptureGL *gl, GLenum target,
                                         GLuint framebuffer) {
  if(gl && gl->bind_framebuffer) {
    gl->bind_framebuffer(target,framebuffer);
    shadow_note_bind_framebuffer(target,framebuffer);
  }
}

static void shadow_call_uniform_1i(CaptureGL *gl, GLint loc, GLint v0) {
  if(gl && gl->uniform_1i) {
    gl->uniform_1i(loc,v0);
    shadow_note_uniform_1i(loc,v0);
  }
}

static void shadow_call_uniform_4f(CaptureGL *gl, GLint loc, GLfloat x,
                                   GLfloat y, GLfloat z, GLfloat w) {
  if(gl && gl->uniform_4f) {
    gl->uniform_4f(loc,x,y,z,w);
    shadow_note_uniform_4f(loc,x,y,z,w);
  }
}

static void shadow_call_uniform_4fv(CaptureGL *gl, GLint loc, GLsizei count,
                                    const GLfloat *v) {
  if(gl && gl->uniform_4fv) {
    gl->uniform_4fv(loc,count,v);
    shadow_note_uniform_4fv(loc,count,v);
  }
}
