static PFNGLATTACHSHADER real_attach_shader(void);

static PFNGLCREATESHADER real_create_shader(void) {
  static PFNGLCREATESHADER p;
  if(!p) p=(PFNGLCREATESHADER)gl_proc("glCreateShader");
  if(!p) p=(PFNGLCREATESHADER)gl_proc("glCreateShaderObjectARB");
  return p;
}

static PFNGLCREATEPROGRAM real_create_program(void) {
  static PFNGLCREATEPROGRAM p;
  if(!p) p=(PFNGLCREATEPROGRAM)gl_proc("glCreateProgram");
  if(!p) p=(PFNGLCREATEPROGRAM)gl_proc("glCreateProgramObjectARB");
  return p;
}

static PFNGLDELETEPROGRAM real_delete_program(void) {
  static PFNGLDELETEPROGRAM p;
  if(!p) p=(PFNGLDELETEPROGRAM)gl_proc("glDeleteProgram");
  if(!p) p=(PFNGLDELETEPROGRAM)gl_proc("glDeleteObjectARB");
  return p;
}

static PFNGLDELETESHADER real_delete_shader(void) {
  static PFNGLDELETESHADER p;
  if(!p) p=(PFNGLDELETESHADER)gl_proc("glDeleteShader");
  if(!p) p=(PFNGLDELETESHADER)gl_proc("glDeleteObjectARB");
  return p;
}

static void invalidate_synthetic_surface_program(void) {
  if(!g_synthetic_surface.program && !g_synthetic_surface.pending_vs &&
     !g_synthetic_surface.pending_fs) {
    memset(&g_synthetic_surface,0,sizeof(g_synthetic_surface));
    return;
  }
  PFNGLDELETEPROGRAM del_program=real_delete_program();
  PFNGLDELETESHADER del_shader=real_delete_shader();
  if(del_program && g_synthetic_surface.program)
    del_program(g_synthetic_surface.program);
  if(del_shader && g_synthetic_surface.pending_vs)
    del_shader(g_synthetic_surface.pending_vs);
  if(del_shader && g_synthetic_surface.pending_fs)
    del_shader(g_synthetic_surface.pending_fs);
  memset(&g_synthetic_surface,0,sizeof(g_synthetic_surface));
}

static PFNGLBINDATTRIBLOCATION real_bind_attrib_location(void) {
  static PFNGLBINDATTRIBLOCATION p;
  if(!p) p=(PFNGLBINDATTRIBLOCATION)gl_proc("glBindAttribLocation");
  return p;
}

static PFNGLBINDFRAGDATALOCATION real_bind_frag_data_location(void) {
  static PFNGLBINDFRAGDATALOCATION p;
  if(!p) p=(PFNGLBINDFRAGDATALOCATION)gl_proc("glBindFragDataLocation");
  if(!p) p=(PFNGLBINDFRAGDATALOCATION)gl_proc("glBindFragDataLocationEXT");
  return p;
}

static PFNGLGETATTRIBLOCATION real_get_attrib_location(void) {
  static PFNGLGETATTRIBLOCATION p;
  if(!p) p=(PFNGLGETATTRIBLOCATION)gl_proc("glGetAttribLocation");
  return p;
}

static PFNGLUNIFORMMATRIX4FV real_uniform_matrix_4fv(void) {
  static PFNGLUNIFORMMATRIX4FV p;
  if(!p) p=(PFNGLUNIFORMMATRIX4FV)gl_proc("glUniformMatrix4fv");
  return p;
}

static PFNGLUNIFORM1I real_uniform_1i(void) {
  static PFNGLUNIFORM1I p;
  if(!p) p=(PFNGLUNIFORM1I)gl_proc("glUniform1i");
  return p;
}

static PFNGLUNIFORM1IV real_uniform_1iv(void) {
  static PFNGLUNIFORM1IV p;
  if(!p) p=(PFNGLUNIFORM1IV)gl_proc("glUniform1iv");
  return p;
}

static PFNGLUNIFORM4F real_uniform_4f(void) {
  static PFNGLUNIFORM4F p;
  if(!p) p=(PFNGLUNIFORM4F)gl_proc("glUniform4f");
  return p;
}

static PFNGLUNIFORM4FV real_uniform_4fv(void) {
  static PFNGLUNIFORM4FV p;
  if(!p) p=(PFNGLUNIFORM4FV)gl_proc("glUniform4fv");
  return p;
}

static PFNGLACTIVETEXTURE real_active_texture(void) {
  static PFNGLACTIVETEXTURE p;
  if(!p) p=(PFNGLACTIVETEXTURE)gl_proc("glActiveTexture");
  if(!p) p=(PFNGLACTIVETEXTURE)gl_proc("glActiveTextureARB");
  return p;
}

static PFNGLBINDTEXTURE real_bind_texture(void) {
  static PFNGLBINDTEXTURE p;
  if(!p) p=(PFNGLBINDTEXTURE)old_proc("glBindTexture");
  return p;
}

static PFNGLBINDFRAMEBUFFER real_bind_framebuffer(void) {
  static PFNGLBINDFRAMEBUFFER p;
  if(!p) p=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebuffer");
  if(!p) p=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebufferEXT");
  return p;
}

static PFNGLVIEWPORT real_viewport(void) {
  static PFNGLVIEWPORT p;
  if(!p) p=(PFNGLVIEWPORT)old_proc("glViewport");
  return p;
}

static PFNGLENABLE real_enable(void) {
  static PFNGLENABLE p;
  if(!p) p=(PFNGLENABLE)old_proc("glEnable");
  return p;
}

static PFNGLDISABLE real_disable(void) {
  static PFNGLDISABLE p;
  if(!p) p=(PFNGLDISABLE)old_proc("glDisable");
  return p;
}

static PFNGLDEPTHMASK real_depth_mask(void) {
  static PFNGLDEPTHMASK p;
  if(!p) p=(PFNGLDEPTHMASK)old_proc("glDepthMask");
  return p;
}

static PFNGLDEPTHFUNC real_depth_func(void) {
  static PFNGLDEPTHFUNC p;
  if(!p) p=(PFNGLDEPTHFUNC)old_proc("glDepthFunc");
  return p;
}

static PFNGLBLENDFUNC real_blend_func(void) {
  static PFNGLBLENDFUNC p;
  if(!p) p=(PFNGLBLENDFUNC)old_proc("glBlendFunc");
  return p;
}

static PFNGLBLENDFUNCSEPARATE real_blend_func_separate(void) {
  static PFNGLBLENDFUNCSEPARATE p;
  if(!p) p=(PFNGLBLENDFUNCSEPARATE)gl_proc("glBlendFuncSeparate");
  if(!p) p=(PFNGLBLENDFUNCSEPARATE)gl_proc("glBlendFuncSeparateEXT");
  return p;
}

static PFNGLBLENDEQUATIONSEPARATE real_blend_equation_separate(void) {
  static PFNGLBLENDEQUATIONSEPARATE p;
  if(!p) p=(PFNGLBLENDEQUATIONSEPARATE)gl_proc("glBlendEquationSeparate");
  if(!p) p=(PFNGLBLENDEQUATIONSEPARATE)gl_proc("glBlendEquationSeparateEXT");
  return p;
}

static PFNGLCOLORMASK real_color_mask(void) {
  static PFNGLCOLORMASK p;
  if(!p) p=(PFNGLCOLORMASK)old_proc("glColorMask");
  return p;
}

static PFNGLGETSHADERIV real_get_shader_iv(void) {
  static PFNGLGETSHADERIV p;
  if(!p) p=(PFNGLGETSHADERIV)gl_proc("glGetShaderiv");
  if(!p) p=(PFNGLGETSHADERIV)gl_proc("glGetObjectParameterivARB");
  return p;
}

static PFNGLGETSHADERINFOLOG real_get_shader_info_log(void) {
  static PFNGLGETSHADERINFOLOG p;
  if(!p) p=(PFNGLGETSHADERINFOLOG)gl_proc("glGetShaderInfoLog");
  if(!p) p=(PFNGLGETSHADERINFOLOG)gl_proc("glGetInfoLogARB");
  return p;
}

static void log_tracked_shader_compile(GLuint shader, const char *label) {
  ShaderTrack *s=shader_track(shader,0);
  if(!s || !s->type || s->compile_logged) return;
  if(!runtime_verbose_log()) {
    s->compile_logged=1;
    return;
  }

  PFNGLGETSHADERIV getiv=real_get_shader_iv();
  if(!getiv) return;
  GLint ok=1;
  getiv(shader,GL_COMPILE_STATUS,&ok);
  s->compile_logged=1;

  if(ok) {
    char msg[192];
    snprintf(msg,sizeof(msg),"compiled %s shader=%u type=%s hash=0x%08X len=%u",
      label ? label : "water shader",shader,shader_type_name(s->type),
      (unsigned int)s->hash,s->len);
    log_line(msg);
    return;
  }

  char logbuf[1024];
  GLsizei got=0;
  logbuf[0]=0;
  PFNGLGETSHADERINFOLOG getlog=real_get_shader_info_log();
  if(getlog)
    getlog(shader,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
  logbuf[sizeof(logbuf)-1]=0;

  char msg[1200];
  snprintf(msg,sizeof(msg),"water shader compile failed shader=%u type=%s label=%s log=%s",
    shader,shader_type_name(s->type),label ? label : "water shader",logbuf);
  log_line(msg);
}

static void APIENTRY hook_glCompileShader(GLuint shader) {
  PFNGLCOMPILESHADER real=real_compile_shader();
  if(real) real(shader);
  log_tracked_shader_compile(shader,"water shader");
}

static PFNGLGETPROGRAMIV real_get_program_iv(void) {
  static PFNGLGETPROGRAMIV p;
  if(!p) p=(PFNGLGETPROGRAMIV)gl_proc("glGetProgramiv");
  if(!p) p=(PFNGLGETPROGRAMIV)gl_proc("glGetObjectParameterivARB");
  return p;
}

static PFNGLGETPROGRAMINFOLOG real_get_program_info_log(void) {
  static PFNGLGETPROGRAMINFOLOG p;
  if(!p) p=(PFNGLGETPROGRAMINFOLOG)gl_proc("glGetProgramInfoLog");
  if(!p) p=(PFNGLGETPROGRAMINFOLOG)gl_proc("glGetInfoLogARB");
  return p;
}

static PFNGLPROGRAMPARAMETERI real_program_parameter_i(void) {
  static PFNGLPROGRAMPARAMETERI p;
  if(!p) p=(PFNGLPROGRAMPARAMETERI)gl_proc("glProgramParameteri");
  if(!p) p=(PFNGLPROGRAMPARAMETERI)gl_proc("glProgramParameteriARB");
  return p;
}

static PFNGLGETPROGRAMBINARY real_get_program_binary(void) {
  static PFNGLGETPROGRAMBINARY p;
  if(!p) p=(PFNGLGETPROGRAMBINARY)gl_proc("glGetProgramBinary");
  return p;
}

static PFNGLPROGRAMBINARY real_program_binary(void) {
  static PFNGLPROGRAMBINARY p;
  if(!p) p=(PFNGLPROGRAMBINARY)gl_proc("glProgramBinary");
  return p;
}

static PFNGLGETSTRING real_get_string(void) {
  static PFNGLGETSTRING p;
  if(!p) p=(PFNGLGETSTRING)gl_proc("glGetString");
  return p;
}

static void log_program_link_status(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !p->type) return;
  if(!runtime_verbose_log()) return;
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(!getiv) return;

  GLint ok=1;
  getiv(program,GL_LINK_STATUS,&ok);
  if(ok) {
    char msg[128];
    snprintf(msg,sizeof(msg),"linked water program=%u type=%s",
      program,shader_type_name(p->type));
    log_line(msg);
    return;
  }

  char logbuf[1024];
  GLsizei got=0;
  logbuf[0]=0;
  PFNGLGETPROGRAMINFOLOG getlog=real_get_program_info_log();
  if(getlog)
    getlog(program,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
  logbuf[sizeof(logbuf)-1]=0;
  char msg[1200];
  snprintf(msg,sizeof(msg),"water program link failed program=%u type=%s log=%s",
    program,shader_type_name(p->type),logbuf);
  log_line(msg);
}

static GLuint compile_water_shader(GLenum stage, const char *label, char *(*load)(void)) {
  PFNGLCREATESHADER create=real_create_shader();
  PFNGLSHADERSOURCE source=real_shader_source("glShaderSource");
  PFNGLCOMPILESHADER compile=real_compile_shader();
  if(!create || !source || !compile) return 0;
  char *text=load ? load() : 0;
  if(!text) return 0;
  GLuint shader=create(stage);
  if(!shader) {
    free(text);
    return 0;
  }
  GLint len=(GLint)strlen(text);
  const GLchar *one=text;
  source(shader,1,&one,&len);
  compile(shader);
  GLint ok=1;
  PFNGLGETSHADERIV getiv=real_get_shader_iv();
  if(getiv) getiv(shader,GL_COMPILE_STATUS,&ok);
  if(!ok) {
    char logbuf[1024];
    GLsizei got=0;
    logbuf[0]=0;
    PFNGLGETSHADERINFOLOG getlog=real_get_shader_info_log();
    if(getlog)
      getlog(shader,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
    logbuf[sizeof(logbuf)-1]=0;
    char msg[1200];
    snprintf(msg,sizeof(msg),"synthetic water shader compile failed stage=%s log=%s",
      label ? label : "shader",logbuf);
    log_line(msg);
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) del(shader);
    free(text);
    return 0;
  }
  free(text);
  return shader;
}

static int current_water_attrib_locations(GLint locs[4]) {
  PFNGLGETATTRIBLOCATION get_attr=real_get_attrib_location();
  if(!get_attr || !g_current_program) return 0;
  locs[0]=get_attr(g_current_program,"aCoord");
  locs[1]=get_attr(g_current_program,"aNormal");
  locs[2]=get_attr(g_current_program,"aLight");
  locs[3]=get_attr(g_current_program,"aColor");
  return locs[0]>=0;
}

static int trshader_has_current_gl_context(void) {
  PFNWGLGETCURRENTCONTEXT current=
    (PFNWGLGETCURRENTCONTEXT)old_proc("wglGetCurrentContext");
  return current && current()!=0;
}

static const char *trshader_gl_string(GLenum name) {
  if(!trshader_has_current_gl_context())
    return "";
  PFNGLGETSTRING get=real_get_string();
  const unsigned char *text=get ? get(name) : 0;
  return text ? (const char*)text : "";
}

static int trshader_contains_i(const char *text, const char *needle) {
  if(!text || !needle || !needle[0]) return 0;
  size_t needle_len=strlen(needle);
  for(const char *p=text;*p;p++) {
    size_t i=0;
    while(i<needle_len && p[i] &&
          ascii_lower((unsigned char)p[i])==
          ascii_lower((unsigned char)needle[i]))
      i++;
    if(i==needle_len)
      return 1;
  }
  return 0;
}

static const char *trshader_compat_mode_name(TrshaderCompatMode mode) {
  switch(mode) {
    case TR456_COMPAT_FULL: return "Full";
    case TR456_COMPAT_SHADER_ONLY: return "ShaderOnly";
    case TR456_COMPAT_VANILLA: return "Vanilla";
    case TR456_COMPAT_AUTO:
    default: return "Auto";
  }
}

static const char *trshader_compat_gpu_name(TrshaderCompatGpu gpu) {
  switch(gpu) {
    case TR456_GPU_NVIDIA: return "nvidia";
    case TR456_GPU_AMD: return "amd";
    case TR456_GPU_INTEL: return "intel";
    case TR456_GPU_MESA: return "mesa";
    case TR456_GPU_MICROSOFT: return "microsoft";
    case TR456_GPU_UNKNOWN:
    default: return "unknown";
  }
}

static TrshaderCompatMode trshader_compat_parse_mode(const char *text) {
  if(!text || !text[0]) return TR456_COMPAT_AUTO;
  if(trshader_contains_i(text,"vanilla") ||
     trshader_contains_i(text,"off") ||
     trshader_contains_i(text,"pass"))
    return TR456_COMPAT_VANILLA;
  if(trshader_contains_i(text,"shader") ||
     trshader_contains_i(text,"safe"))
    return TR456_COMPAT_SHADER_ONLY;
  if(trshader_contains_i(text,"full") ||
     trshader_contains_i(text,"force"))
    return TR456_COMPAT_FULL;
  return TR456_COMPAT_AUTO;
}

static void trshader_compat_copy_gl_string(char *out, size_t out_size,
                                           GLenum name) {
  const char *text=trshader_gl_string(name);
  if(!out || !out_size) return;
  out[0]=0;
  if(text[0])
    lstrcpynA(out,text,(int)out_size);
}

static void trshader_compat_parse_gl_version(void) {
  const char *p=g_compat.version;
  g_compat.gl_major=0;
  g_compat.gl_minor=0;
  while(*p && (*p<'0' || *p>'9')) p++;
  if(!*p) return;
  g_compat.gl_major=(int)strtol(p,(char**)&p,10);
  if(*p=='.') {
    p++;
    g_compat.gl_minor=(int)strtol(p,0,10);
  }
}

static int trshader_compat_gl_below(int major, int minor) {
  if(g_compat.gl_major<=0) return 0;
  if(g_compat.gl_major<major) return 1;
  if(g_compat.gl_major>major) return 0;
  return g_compat.gl_minor<minor;
}

static TrshaderCompatGpu trshader_compat_detect_gpu(void) {
  if(trshader_contains_i(g_compat.vendor,"AMD") ||
     trshader_contains_i(g_compat.vendor,"ATI") ||
     trshader_contains_i(g_compat.renderer,"AMD") ||
     trshader_contains_i(g_compat.renderer,"ATI") ||
     trshader_contains_i(g_compat.renderer,"Radeon") ||
     trshader_contains_i(g_compat.renderer,"RADV"))
    return TR456_GPU_AMD;
  if(trshader_contains_i(g_compat.vendor,"NVIDIA") ||
     trshader_contains_i(g_compat.renderer,"NVIDIA"))
    return TR456_GPU_NVIDIA;
  if(trshader_contains_i(g_compat.vendor,"Intel") ||
     trshader_contains_i(g_compat.renderer,"Intel"))
    return TR456_GPU_INTEL;
  if(trshader_contains_i(g_compat.vendor,"Microsoft") ||
     trshader_contains_i(g_compat.renderer,"GDI Generic") ||
     trshader_contains_i(g_compat.renderer,"Microsoft"))
    return TR456_GPU_MICROSOFT;
  if(trshader_contains_i(g_compat.vendor,"Mesa") ||
     trshader_contains_i(g_compat.renderer,"Mesa") ||
     trshader_contains_i(g_compat.renderer,"llvmpipe") ||
     trshader_contains_i(g_compat.renderer,"zink") ||
     trshader_contains_i(g_compat.version,"Mesa"))
    return TR456_GPU_MESA;
  return TR456_GPU_UNKNOWN;
}

static int trshader_compat_refresh_driver(void) {
  if(g_compat.driver_ready) return 1;
  boot_log_line("compat refresh driver begin");
  trshader_compat_copy_gl_string(g_compat.vendor,sizeof(g_compat.vendor),
    GL_VENDOR);
  trshader_compat_copy_gl_string(g_compat.renderer,sizeof(g_compat.renderer),
    GL_RENDERER);
  trshader_compat_copy_gl_string(g_compat.version,sizeof(g_compat.version),
    GL_VERSION);
  trshader_compat_copy_gl_string(g_compat.glsl,sizeof(g_compat.glsl),
    GL_SHADING_LANGUAGE_VERSION);
  if(!g_compat.vendor[0] && !g_compat.renderer[0] && !g_compat.version[0]) {
    boot_log_line("compat refresh driver skipped: no current GL strings");
    return 0;
  }
  trshader_compat_parse_gl_version();
  g_compat.gpu=trshader_compat_detect_gpu();
  g_compat.driver_ready=1;
  trshader_compat_apply_runtime_policy();
  boot_logf("compat driver ready gpu=%s gl=%d.%d vendor=\"%s\" renderer=\"%s\" version=\"%s\" glsl=\"%s\"",
    trshader_compat_gpu_name(g_compat.gpu),g_compat.gl_major,
    g_compat.gl_minor,g_compat.vendor,g_compat.renderer,g_compat.version,
    g_compat.glsl);
  return 1;
}

static int trshader_compat_driver_uses_fragile_binaries(void) {
  return g_compat.gpu==TR456_GPU_AMD ||
    g_compat.gpu==TR456_GPU_MESA ||
    trshader_contains_i(g_compat.version,"Mesa") ||
    trshader_contains_i(g_compat.renderer,"RADV") ||
    trshader_contains_i(g_compat.renderer,"zink") ||
    trshader_contains_i(g_compat.renderer,"llvmpipe");
}

static int trshader_compat_driver_is_zink(void) {
  return trshader_contains_i(g_compat.renderer,"zink") ||
    trshader_contains_i(g_compat.version,"zink");
}

static void trshader_compat_read_config(void) {
  char mode[32];
  memset(&g_compat,0,sizeof(g_compat));
  ini_string("CompatMode","Auto",mode,sizeof(mode));
  g_compat.mode=trshader_compat_parse_mode(mode);
  lstrcpynA(g_compat.mode_text,trshader_compat_mode_name(g_compat.mode),
    sizeof(g_compat.mode_text));
  g_compat.report_enabled=ini_int("CompatReport",1);
  g_compat.gl_error_check=ini_int("CompatGlErrorCheck",0);
  g_compat.gl_error_warmup_draws=ini_int("CompatGlErrorWarmupDraws",0);
  if(g_compat.gl_error_warmup_draws<0) g_compat.gl_error_warmup_draws=0;
  if(g_compat.gl_error_warmup_draws>10000)
    g_compat.gl_error_warmup_draws=10000;
  g_compat.allow_system_full=ini_int("CompatAllowSystemFull",0) ? 1 : 0;
  g_compat.max_synthetic_errors=ini_int("CompatMaxSyntheticErrors",4);
  if(g_compat.max_synthetic_errors<0) g_compat.max_synthetic_errors=0;
  if(g_compat.max_synthetic_errors>64) g_compat.max_synthetic_errors=64;
  ini_string("ReShadeDll","OpenGL32_reshade.dll",g_compat.reshade_dll,
    sizeof(g_compat.reshade_dll));
  g_compat.config_loaded=1;
}

static void trshader_compat_set_fallback(const char *reason) {
  if(!reason || !reason[0]) return;
  if(!g_compat.fallback_reason[0])
    lstrcpynA(g_compat.fallback_reason,reason,
      sizeof(g_compat.fallback_reason));
}

static void trshader_compat_disable_synthetic_runtime(void) {
  g_runtime_synthetic_surface=0;
  g_runtime_synthetic_standing_replace_original=0;
  g_runtime_synthetic_flow_surface=0;
  g_runtime_synthetic_flow_replace_original=0;
  g_runtime_flow_lite_surface=0;
  g_runtime_synthetic_reflect_surface=0;
}

static void trshader_compat_apply_runtime_policy(void) {
  if(!g_compat.config_loaded)
    return;

  lstrcpynA(g_compat.profile,"full",sizeof(g_compat.profile));

  if(g_runtime_safe_mode) {
    g_runtime_shader_patching=0;
    g_runtime_fbo_reflection=0;
    trshader_compat_disable_synthetic_runtime();
    g_runtime_flow_texture_fallback=0;
    g_runtime_native_water_overlay_suppress=0;
    lstrcpynA(g_compat.profile,"safe-mode",sizeof(g_compat.profile));
    trshader_compat_set_fallback("SafeMode=1");
    return;
  }

  if(g_compat.mode==TR456_COMPAT_VANILLA) {
    g_runtime_shader_patching=0;
    g_runtime_fbo_reflection=0;
    trshader_compat_disable_synthetic_runtime();
    g_runtime_flow_texture_fallback=0;
    lstrcpynA(g_compat.profile,"vanilla-pass-through",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback("CompatMode=Vanilla");
    return;
  }

  if(g_compat.mode==TR456_COMPAT_SHADER_ONLY) {
    g_runtime_fbo_reflection=0;
    trshader_compat_disable_synthetic_runtime();
    lstrcpynA(g_compat.profile,"shader-only",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback("CompatMode=ShaderOnly");
    return;
  }

  if(g_compat.mode==TR456_COMPAT_FULL) {
    lstrcpynA(g_compat.profile,"forced-full",sizeof(g_compat.profile));
    return;
  }

  lstrcpynA(g_compat.profile,"auto",sizeof(g_compat.profile));
  if(!g_compat.driver_ready)
    return;

  if(g_compat.gpu==TR456_GPU_MICROSOFT || trshader_compat_gl_below(3,2)) {
    g_runtime_fbo_reflection=0;
    trshader_compat_disable_synthetic_runtime();
    lstrcpynA(g_compat.profile,"auto-shader-only",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback(
      g_compat.gpu==TR456_GPU_MICROSOFT ?
      "Microsoft OpenGL fallback driver" : "OpenGL version below 3.2");
  } else if(!g_compat.allow_system_full &&
            !trshader_compat_driver_is_zink()) {
    g_runtime_fbo_reflection=0;
    trshader_compat_disable_synthetic_runtime();
    lstrcpynA(g_compat.profile,"auto-system-shader-only",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback(
      "non-Zink OpenGL full effects disabled by CompatAllowSystemFull=0");
  } else if(g_compat.gpu==TR456_GPU_AMD) {
    lstrcpynA(g_compat.profile,"auto-amd-safe",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback("program binary cache bypass");
  } else if(g_compat.gpu==TR456_GPU_MESA) {
    lstrcpynA(g_compat.profile,"auto-mesa-safe",
      sizeof(g_compat.profile));
    trshader_compat_set_fallback("Mesa/RADV program binary cache bypass");
  }
}

static void trshader_compat_report_once(const char *where) {
  load_runtime_config();
  if(g_compat.report_logged || !g_compat.report_enabled)
    return;
  if(!trshader_compat_refresh_driver())
    return;

  char msg[1024];
  const char *cache_state=!g_runtime_shader_binary_cache ? "off" :
    (!g_runtime_synthetic_surface ? "unused" :
    (trshader_compat_shader_binary_cache_enabled() ? "on" :
      "driver-bypass"));
  snprintf(msg,sizeof(msg),
    "compat report where=%s mode=%s profile=%s gpu=%s gl=%d.%d vendor=\"%s\" renderer=\"%s\" version=\"%s\" glsl=\"%s\" shaderPatching=%d shaderCache=%s fboReflection=%d synthetic=%d flow=%d reflect=%d flowFallback=%d allowSystemFull=%d reshadeChain=%d reshadeDll=\"%s\" glErrorCheck=%d glErrorWarmup=%d fallback=\"%s\"",
    where ? where : "unknown",g_compat.mode_text,g_compat.profile,
    trshader_compat_gpu_name(g_compat.gpu),g_compat.gl_major,
    g_compat.gl_minor,g_compat.vendor,g_compat.renderer,g_compat.version,
    g_compat.glsl,g_runtime_shader_patching,cache_state,
    g_runtime_fbo_reflection,g_runtime_synthetic_surface,
    g_runtime_synthetic_flow_surface,g_runtime_synthetic_reflect_surface,
    g_runtime_flow_texture_fallback,g_compat.allow_system_full,
    ini_int("ReShadeChain",1),
    g_compat.reshade_dll,g_compat.gl_error_check,
    g_compat.gl_error_warmup_draws,
    g_compat.fallback_reason[0] ? g_compat.fallback_reason : "none");
  log_line(msg);
  boot_log_line(msg);
  g_compat.report_logged=1;
}

static int trshader_compat_shader_binary_cache_enabled(void) {
  load_runtime_config();
  if(!g_runtime_shader_binary_cache || !g_runtime_synthetic_surface)
    return 0;
  trshader_compat_refresh_driver();
  if(g_compat.mode==TR456_COMPAT_AUTO &&
     trshader_compat_driver_uses_fragile_binaries()) {
    if(!g_compat.cache_bypass_logged) {
      char msg[224];
      snprintf(msg,sizeof(msg),
        "shader binary cache disabled for %s OpenGL driver; using GLSL compile path",
        trshader_compat_gpu_name(g_compat.gpu));
      log_line(msg);
      g_compat.cache_bypass_logged=1;
    }
    return 0;
  }
  return 1;
}

static void trshader_compat_note_synthetic_failure(const char *reason) {
  trshader_compat_disable_synthetic_runtime();
  lstrcpynA(g_compat.fallback_reason,
    reason ? reason : "synthetic water failure",
    sizeof(g_compat.fallback_reason));
  if(!g_compat.synthetic_disabled_logged) {
    char msg[256];
    snprintf(msg,sizeof(msg),
      "compat fallback: synthetic water disabled, original game water will continue reason=\"%s\"",
      reason ? reason : "unknown");
    log_line(msg);
    g_compat.synthetic_disabled_logged=1;
  }
}

static void trshader_compat_clear_gl_errors(void) {
  if(!g_compat.gl_error_check) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_error) return;
  for(int i=0;i<8;i++) {
    GLenum err=gl->get_error();
    if(!err) break;
  }
}

static void trshader_compat_note_synthetic_error(GLenum err,
                                                 const char *where) {
  if(!g_compat.gl_error_check)
    return;
  if(!err) {
    g_compat.synthetic_error_count=0;
    if(g_compat.gl_error_warmup_draws>0 &&
       g_compat.synthetic_success_count<g_compat.gl_error_warmup_draws) {
      g_compat.synthetic_success_count++;
      if(g_compat.synthetic_success_count>=g_compat.gl_error_warmup_draws &&
         !g_compat.synthetic_error_check_retired) {
        g_compat.synthetic_error_check_retired=1;
        g_compat.gl_error_check=0;
        log_line("compat synthetic gl error warmup complete; disabling per-draw glGetError checks");
      }
    }
    return;
  }
  g_compat.synthetic_success_count=0;
  if(g_compat.synthetic_error_count<0x7FFFFFFF)
    g_compat.synthetic_error_count++;
  if(g_compat.synthetic_error_count==1 ||
     g_compat.synthetic_error_count==g_compat.max_synthetic_errors) {
    char msg[256];
    snprintf(msg,sizeof(msg),
      "compat synthetic gl error where=%s err=0x%04X consecutive=%d limit=%d",
      where ? where : "draw",(unsigned int)err,
      g_compat.synthetic_error_count,g_compat.max_synthetic_errors);
    log_line(msg);
  }
  if(g_compat.max_synthetic_errors>0 &&
     g_compat.synthetic_error_count>=g_compat.max_synthetic_errors) {
    g_synthetic_surface.failed=1;
    trshader_compat_note_synthetic_failure("repeated synthetic GL errors");
  }
}

static int trshader_shader_binary_cache_enabled(void) {
  return trshader_compat_shader_binary_cache_enabled();
}

static uint64_t trshader_hash_cstr(uint64_t h, const char *text) {
  static const char zero=0;
  if(text) h=fnv1a64_update(h,text,strlen(text));
  return fnv1a64_update(h,&zero,1);
}

static uint64_t trshader_synthetic_binary_cache_key(const GLint locs[4]) {
  char *vs=synthetic_surface_vertex_shader();
  char *fs=synthetic_surface_shader();
  if(!vs || !fs) {
    free(vs);
    free(fs);
    return 0;
  }

  static const char label[]="tr456 synthetic surface binary cache v1";
  uint64_t h=14695981039346656037ULL;
  h=fnv1a64_update(h,label,sizeof(label));
  h=trshader_hash_cstr(h,trshader_gl_string(GL_VENDOR));
  h=trshader_hash_cstr(h,trshader_gl_string(GL_RENDERER));
  h=trshader_hash_cstr(h,trshader_gl_string(GL_VERSION));
  h=fnv1a64_update(h,locs,sizeof(GLint)*4u);
  h=trshader_hash_cstr(h,vs);
  h=trshader_hash_cstr(h,fs);

  free(vs);
  free(fs);
  return h ? h : 1u;
}

static int trshader_synthetic_binary_cache_path(uint64_t key,
                                                const GLint locs[4],
                                                char *out) {
  char dir[MAX_PATH];
  unsigned int hi=0;
  unsigned int lo=0;
  if(!out || !join_mod_path(dir,"shader_cache")) return 0;
  CreateDirectoryA(dir,0);
  hash64_parts(key,&hi,&lo);
  int n=snprintf(out,MAX_PATH,
    "%s\\synthetic_surface_%08X%08X_a%d_%d_%d_%d.bin",
    dir,hi,lo,(int)locs[0],(int)locs[1],(int)locs[2],(int)locs[3]);
  if(n<=0 || n>=MAX_PATH) {
    out[0]=0;
    return 0;
  }
  return 1;
}

static int trshader_read_exact(HANDLE h, void *buf, DWORD size) {
  DWORD got=0;
  return ReadFile(h,buf,size,&got,0) && got==size;
}

static int trshader_write_exact(HANDLE h, const void *buf, DWORD size) {
  DWORD wrote=0;
  return WriteFile(h,buf,size,&wrote,0) && wrote==size;
}

static void trshader_store_synthetic_uniforms(SyntheticSurfacePass *s,
                                              GLuint program,
                                              CaptureGL *gl) {
  s->program=program;
  s->loc_proj=gl->get_uniform_location(program,"uProjMatrix");
  s->loc_model=gl->get_uniform_location(program,"uModelMatrix[0]");
  s->loc_view=gl->get_uniform_location(program,"uViewMatrix[0]");
  s->loc_scene=gl->get_uniform_location(program,"uTrWaterScene");
  s->loc_flow_tex0_wrap=gl->get_uniform_location(program,"sTex0_wrap");
  s->loc_underlay=gl->get_uniform_location(program,"uTrWaterUnderlay");
  s->loc_capture_info=gl->get_uniform_location(program,"uTrWaterCaptureInfo");
  s->loc_underlay_info=gl->get_uniform_location(program,"uTrWaterUnderlayInfo");
  s->loc_synthetic_info=gl->get_uniform_location(program,"uTrWaterSyntheticInfo");
  s->loc_synthetic_mode=gl->get_uniform_location(program,"uTrWaterSyntheticMode");
  s->loc_synthetic_profile=gl->get_uniform_location(program,"uTrWaterSyntheticProfile");
  s->loc_params=gl->get_uniform_location(program,"uParams");
  s->loc_draw_info=gl->get_uniform_location(program,"uTrWaterDrawInfo");
  s->loc_toggle0=gl->get_uniform_location(program,"uTrWaterToggle0");
  s->loc_toggle1=gl->get_uniform_location(program,"uTrWaterToggle1");
  s->loc_toggle2=gl->get_uniform_location(program,"uTrWaterToggle2");
  s->loc_flow_fx0=gl->get_uniform_location(program,"uTrWaterFlowFx0");
  s->loc_flow_fx1=gl->get_uniform_location(program,"uTrWaterFlowFx1");
  s->loc_flow_fx2=gl->get_uniform_location(program,"uTrWaterFlowFx2");
  s->loc_flow_fx3=gl->get_uniform_location(program,"uTrWaterFlowFx3");
  s->loc_contacts=gl->get_uniform_location(program,"uContacts[0]");
  s->loc_contact_motion=gl->get_uniform_location(program,"uContactMotion[0]");
}

static int trshader_try_load_synthetic_program_binary(SyntheticSurfacePass *s,
                                                      CaptureGL *gl,
                                                      const GLint locs[4]) {
  if(!trshader_shader_binary_cache_enabled()) return 0;
  PFNGLPROGRAMBINARY program_binary=real_program_binary();
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLDELETEPROGRAM del_program=real_delete_program();
  if(!program_binary || !getiv || !create_program) return 0;

  uint64_t key=trshader_synthetic_binary_cache_key(locs);
  if(!key) return 0;
  s->binary_cache_key=key;

  char path[MAX_PATH];
  if(!trshader_synthetic_binary_cache_path(key,locs,path)) return 0;
  HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return 0;

  TrshaderProgramBinaryCacheHeader header;
  int valid=trshader_read_exact(h,&header,(DWORD)sizeof(header));
  valid=valid && header.magic==TR456_SHADER_BINARY_CACHE_MAGIC &&
    header.version==TR456_SHADER_BINARY_CACHE_VERSION &&
    header.key==key &&
    header.attr_coord==(int32_t)locs[0] &&
    header.attr_normal==(int32_t)locs[1] &&
    header.attr_light==(int32_t)locs[2] &&
    header.attr_color==(int32_t)locs[3] &&
    header.binary_size>0 &&
    header.binary_size<=TR456_SHADER_BINARY_CACHE_MAX_BYTES;
  if(!valid) {
    CloseHandle(h);
    DeleteFileA(path);
    return 0;
  }

  void *data=malloc(header.binary_size);
  if(!data) {
    CloseHandle(h);
    return 0;
  }
  valid=trshader_read_exact(h,data,(DWORD)header.binary_size);
  CloseHandle(h);
  if(!valid) {
    free(data);
    DeleteFileA(path);
    return 0;
  }

  GLuint program=create_program();
  if(!program) {
    free(data);
    return 0;
  }
  program_binary(program,(GLenum)header.binary_format,data,
    (GLsizei)header.binary_size);
  free(data);

  GLint ok=0;
  getiv(program,GL_LINK_STATUS,&ok);
  if(!ok) {
    if(del_program) del_program(program);
    DeleteFileA(path);
    log_line("synthetic water shader binary cache rejected by driver; rebuilding");
    return 0;
  }

  trshader_store_synthetic_uniforms(s,program,gl);
  s->compile_stage=4;
  s->ready=1;
  s->binary_cache_loaded=1;
  char msg[256];
  snprintf(msg,sizeof(msg),
    "synthetic water shader binary cache loaded program=%u key=0x%08X%08X",
    program,(unsigned int)(key>>32),(unsigned int)(key&0xFFFFFFFFu));
  log_line(msg);
  return 1;
}

static void trshader_save_synthetic_program_binary(SyntheticSurfacePass *s,
                                                   GLuint program,
                                                   const GLint locs[4]) {
  if(!trshader_shader_binary_cache_enabled()) return;
  PFNGLGETPROGRAMBINARY get_binary=real_get_program_binary();
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(!get_binary || !getiv) return;

  uint64_t key=s->binary_cache_key;
  if(!key) {
    key=trshader_synthetic_binary_cache_key(locs);
    s->binary_cache_key=key;
  }
  if(!key) return;

  GLint binary_size=0;
  getiv(program,GL_PROGRAM_BINARY_LENGTH,&binary_size);
  if(binary_size<=0 ||
     binary_size>(GLint)TR456_SHADER_BINARY_CACHE_MAX_BYTES) return;

  void *data=malloc((size_t)binary_size);
  if(!data) return;
  GLsizei got=0;
  GLenum binary_format=0;
  get_binary(program,(GLsizei)binary_size,&got,&binary_format,data);
  if(got<=0 || got>binary_size || !binary_format) {
    free(data);
    return;
  }

  char path[MAX_PATH];
  if(!trshader_synthetic_binary_cache_path(key,locs,path)) {
    free(data);
    return;
  }
  HANDLE h=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) {
    free(data);
    return;
  }

  TrshaderProgramBinaryCacheHeader header;
  memset(&header,0,sizeof(header));
  header.magic=TR456_SHADER_BINARY_CACHE_MAGIC;
  header.version=TR456_SHADER_BINARY_CACHE_VERSION;
  header.key=key;
  header.attr_coord=(int32_t)locs[0];
  header.attr_normal=(int32_t)locs[1];
  header.attr_light=(int32_t)locs[2];
  header.attr_color=(int32_t)locs[3];
  header.binary_format=(uint32_t)binary_format;
  header.binary_size=(uint32_t)got;

  int ok=trshader_write_exact(h,&header,(DWORD)sizeof(header)) &&
    trshader_write_exact(h,data,(DWORD)got);
  CloseHandle(h);
  free(data);
  if(!ok) {
    DeleteFileA(path);
    return;
  }

  char msg[256];
  snprintf(msg,sizeof(msg),
    "synthetic water shader binary cache saved bytes=%d key=0x%08X%08X",
    (int)got,(unsigned int)(key>>32),(unsigned int)(key&0xFFFFFFFFu));
  log_line(msg);
}

static PFNGLUSEPROGRAM real_use_program(void);
static PFNGLTEXIMAGE3D real_tex_image_3d(void);

#include "tr456_lab_common.c"
#include "tr456_wet_lara_lab.c"

static int ensure_synthetic_surface_program(void) {
  load_runtime_config();
  trshader_compat_refresh_driver();
  if(!g_runtime_synthetic_surface) return 0;
  if(g_runtime_synthetic_compile_delay_frames>0 &&
     g_frame_index<(unsigned int)g_runtime_synthetic_compile_delay_frames) {
    if(!g_synthetic_compile_delay_logged) {
      char msg[160];
      snprintf(msg,sizeof(msg),
        "synthetic water surface compile delayed until frame %d",
        g_runtime_synthetic_compile_delay_frames);
      log_line(msg);
      g_synthetic_compile_delay_logged=1;
    }
    return 0;
  }
  SyntheticSurfacePass *s=&g_synthetic_surface;
  if(s->ready) return 1;
  if(s->failed) return 0;

  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  PFNGLBINDATTRIBLOCATION bind_attr=real_bind_attrib_location();
  PFNGLBINDFRAGDATALOCATION bind_frag=real_bind_frag_data_location();
  CaptureGL *gl=capture_gl();
  if(!create_program || !attach || !link || !bind_attr ||
     !gl->get_uniform_location) {
    log_line("synthetic water surface disabled: missing GL program entry point");
    trshader_compat_note_synthetic_failure("missing GL program entry point");
    s->failed=1;
    return 0;
  }

  if(!s->tried) {
    GLint locs[4]={-1,-1,-1,-1};
    if(!current_water_attrib_locations(locs)) {
      log_line("synthetic water surface disabled: candidate program has no aCoord location");
      trshader_compat_note_synthetic_failure("candidate program has no aCoord location");
      s->failed=1;
      return 0;
    }
    s->attr_coord=locs[0];
    s->attr_normal=locs[1];
    s->attr_light=locs[2];
    s->attr_color=locs[3];
    s->tried=1;
    if(trshader_try_load_synthetic_program_binary(s,gl,locs))
      return 1;
    s->compile_stage=1;
    s->compile_step_frame=g_frame_index;
    if(!g_runtime_synthetic_compile_sync)
      return 0;
  }

  if(!g_runtime_synthetic_compile_sync &&
     s->compile_step_frame==g_frame_index)
    return 0;
  s->compile_step_frame=g_frame_index;

  switch(s->compile_stage) {
    case 1:
      s->pending_vs=compile_water_shader(GL_VERTEX_SHADER,
        "synthetic vertex",synthetic_surface_vertex_shader);
      if(!s->pending_vs) {
        trshader_compat_note_synthetic_failure("synthetic vertex shader compile failed");
        s->failed=1;
        return 0;
      }
      s->compile_stage=2;
      if(!g_runtime_synthetic_compile_sync)
        return 0;
      /* fall through */

    case 2:
      s->pending_fs=compile_water_shader(GL_FRAGMENT_SHADER,
        "synthetic fragment",synthetic_surface_shader);
      if(!s->pending_fs) {
        PFNGLDELETESHADER del=real_delete_shader();
        if(del && s->pending_vs) del(s->pending_vs);
        s->pending_vs=0;
        trshader_compat_note_synthetic_failure("synthetic fragment shader compile failed");
        s->failed=1;
        return 0;
      }
      s->compile_stage=3;
      if(!g_runtime_synthetic_compile_sync)
        return 0;
      break;

    default:
      break;
  }

  if(s->compile_stage!=3 || !s->pending_vs || !s->pending_fs) {
    trshader_compat_note_synthetic_failure("synthetic shader compile state invalid");
    s->failed=1;
    return 0;
  }

  GLuint vs=s->pending_vs;
  GLuint fs=s->pending_fs;
  GLuint program=create_program();
  if(!program) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) {
      del(vs);
      del(fs);
    }
    s->pending_vs=0;
    s->pending_fs=0;
    trshader_compat_note_synthetic_failure("synthetic program allocation failed");
    s->failed=1;
    return 0;
  }

  static const char *names[4]={"aCoord","aNormal","aLight","aColor"};
  GLint locs[4]={s->attr_coord,s->attr_normal,s->attr_light,s->attr_color};
  for(int i=0;i<4;i++)
    if(locs[i]>=0) bind_attr(program,(GLuint)locs[i],names[i]);
  if(bind_frag)
    bind_frag(program,0,"trshaderFragColor");
  PFNGLPROGRAMPARAMETERI program_parameter_i=real_program_parameter_i();
  if(trshader_shader_binary_cache_enabled() && program_parameter_i)
    program_parameter_i(program,GL_PROGRAM_BINARY_RETRIEVABLE_HINT,GL_TRUE);
  attach(program,vs);
  attach(program,fs);
  link(program);

  GLint ok=1;
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(getiv) getiv(program,GL_LINK_STATUS,&ok);
  PFNGLDELETESHADER del_shader=real_delete_shader();
  if(del_shader) {
    del_shader(vs);
    del_shader(fs);
  }
  s->pending_vs=0;
  s->pending_fs=0;
  if(!ok) {
    char logbuf[1024];
    GLsizei got=0;
    logbuf[0]=0;
    PFNGLGETPROGRAMINFOLOG getlog=real_get_program_info_log();
    if(getlog)
      getlog(program,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
    logbuf[sizeof(logbuf)-1]=0;
    char msg[1200];
    snprintf(msg,sizeof(msg),"synthetic water surface link failed log=%s",logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    trshader_compat_note_synthetic_failure("synthetic water surface link failed");
    s->failed=1;
    return 0;
  }

  trshader_store_synthetic_uniforms(s,program,gl);
  trshader_save_synthetic_program_binary(s,program,locs);
  s->compile_stage=4;
  s->ready=1;
  char msg[224];
  snprintf(msg,sizeof(msg),
    "synthetic water surface linked program=%u attrs coord=%d normal=%d light=%d color=%d",
    program,locs[0],locs[1],locs[2],locs[3]);
  log_line(msg);
  return 1;
}

static int ensure_flow_lite_surface_program(void) {
  load_runtime_config();
  trshader_compat_refresh_driver();
  if(!g_runtime_synthetic_surface || !g_runtime_flow_lite_surface) return 0;
  SyntheticSurfacePass *s=&g_flow_lite_surface;
  if(s->ready) return 1;
  if(s->failed) return 0;

  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  PFNGLBINDATTRIBLOCATION bind_attr=real_bind_attrib_location();
  PFNGLBINDFRAGDATALOCATION bind_frag=real_bind_frag_data_location();
  CaptureGL *gl=capture_gl();
  if(!create_program || !attach || !link || !bind_attr ||
     !gl->get_uniform_location) {
    log_line("flow lite surface disabled: missing GL program entry point");
    s->failed=1;
    return 0;
  }

  GLint locs[4]={-1,-1,-1,-1};
  if(!current_water_attrib_locations(locs)) {
    log_line("flow lite surface disabled: candidate program has no aCoord location");
    s->failed=1;
    return 0;
  }
  s->attr_coord=locs[0];
  s->attr_normal=locs[1];
  s->attr_light=locs[2];
  s->attr_color=locs[3];

  GLuint vs=compile_water_shader(GL_VERTEX_SHADER,
    "flow lite vertex",flow_lite_vertex_shader);
  GLuint fs=compile_water_shader(GL_FRAGMENT_SHADER,
    "flow lite fragment",flow_lite_fragment_shader);
  if(!vs || !fs) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) {
      if(vs) del(vs);
      if(fs) del(fs);
    }
    s->failed=1;
    return 0;
  }

  GLuint program=create_program();
  if(!program) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) { del(vs); del(fs); }
    s->failed=1;
    return 0;
  }

  static const char *names[4]={"aCoord","aNormal","aLight","aColor"};
  for(int i=0;i<4;i++)
    if(locs[i]>=0) bind_attr(program,(GLuint)locs[i],names[i]);
  if(bind_frag)
    bind_frag(program,0,"trshaderFragColor");
  attach(program,vs);
  attach(program,fs);
  link(program);

  PFNGLDELETESHADER del_shader=real_delete_shader();
  if(del_shader) { del_shader(vs); del_shader(fs); }

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
    snprintf(msg,sizeof(msg),"flow lite surface link failed log=%s",logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    s->failed=1;
    return 0;
  }

  trshader_store_synthetic_uniforms(s,program,gl);
  s->ready=1;
  char msg[224];
  snprintf(msg,sizeof(msg),
    "flow lite surface linked program=%u attrs coord=%d normal=%d light=%d color=%d",
    program,locs[0],locs[1],locs[2],locs[3]);
  log_line(msg);
  return 1;
}

static void APIENTRY hook_glLinkProgram(GLuint program) {
  PFNGLLINKPROGRAM real=real_link_program();
  if(real) real(program);
  log_program_link_status(program);
}

static PFNGLATTACHSHADER real_attach_shader(void) {
  static PFNGLATTACHSHADER p;
  if(!p) p=(PFNGLATTACHSHADER)gl_proc("glAttachShader");
  return p;
}

static void APIENTRY hook_glAttachShader(GLuint program, GLuint shader) {
  PFNGLATTACHSHADER real=real_attach_shader();
  if(real) real(program,shader);
  load_runtime_config();
  attach_program_shader_info(program,shader);
  ShaderTrack *s=shader_track(shader,0);
  int type=shader_type(shader);
  if(s && type && runtime_verbose_log()) {
    char msg[320];
    snprintf(msg,sizeof(msg),"attached shader program=%u shader=%u type=%s hash=0x%08X len=%u preview=\"%s\"",
      program,shader,shader_type_name(s->type),(unsigned int)s->hash,s->len,s->preview);
    log_line(msg);
  }
  if(type) {
    set_program_type(program,type);
    if(runtime_verbose_log()) {
      char msg[128];
      snprintf(msg,sizeof(msg),"tracked water program=%u shader=%u type=%d",program,shader,type);
      log_line(msg);
    }
  }
}
