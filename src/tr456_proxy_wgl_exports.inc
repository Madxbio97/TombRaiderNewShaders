static void trshader_apply_wgl_pixel_format_attribs(
    PIXELFORMATDESCRIPTOR *pfd, const int *attribs) {
  if(!pfd || !attribs) return;
  for(int i=0;i<256 && attribs[i];i+=2) {
    int attr=attribs[i];
    int value=attribs[i+1];
    switch(attr) {
      case TR456_WGL_DRAW_TO_WINDOW_ARB:
        if(value) pfd->dwFlags|=PFD_DRAW_TO_WINDOW;
        else pfd->dwFlags&=~PFD_DRAW_TO_WINDOW;
        break;
      case TR456_WGL_DRAW_TO_BITMAP_ARB:
        if(value) pfd->dwFlags|=PFD_DRAW_TO_BITMAP;
        else pfd->dwFlags&=~PFD_DRAW_TO_BITMAP;
        break;
      case TR456_WGL_SUPPORT_OPENGL_ARB:
        if(value) pfd->dwFlags|=PFD_SUPPORT_OPENGL;
        else pfd->dwFlags&=~PFD_SUPPORT_OPENGL;
        break;
      case TR456_WGL_DOUBLE_BUFFER_ARB:
        if(value) pfd->dwFlags|=PFD_DOUBLEBUFFER;
        else pfd->dwFlags&=~PFD_DOUBLEBUFFER;
        break;
      case TR456_WGL_STEREO_ARB:
        if(value) pfd->dwFlags|=PFD_STEREO;
        else pfd->dwFlags&=~PFD_STEREO;
        break;
      case TR456_WGL_PIXEL_TYPE_ARB:
        pfd->iPixelType=(BYTE)(value==TR456_WGL_TYPE_COLORINDEX_ARB ?
          PFD_TYPE_COLORINDEX : PFD_TYPE_RGBA);
        break;
      case TR456_WGL_COLOR_BITS_ARB:
        if(value>0 && value<256) pfd->cColorBits=(BYTE)value;
        break;
      case TR456_WGL_RED_BITS_ARB:
        if(value>0 && value<256) pfd->cRedBits=(BYTE)value;
        break;
      case TR456_WGL_GREEN_BITS_ARB:
        if(value>0 && value<256) pfd->cGreenBits=(BYTE)value;
        break;
      case TR456_WGL_BLUE_BITS_ARB:
        if(value>0 && value<256) pfd->cBlueBits=(BYTE)value;
        break;
      case TR456_WGL_ALPHA_BITS_ARB:
        if(value>0 && value<256) pfd->cAlphaBits=(BYTE)value;
        break;
      case TR456_WGL_DEPTH_BITS_ARB:
        if(value>=0 && value<256) pfd->cDepthBits=(BYTE)value;
        break;
      case TR456_WGL_STENCIL_BITS_ARB:
        if(value>=0 && value<256) pfd->cStencilBits=(BYTE)value;
        break;
      case TR456_WGL_ACCELERATION_ARB:
      default:
        break;
    }
  }
}

static BOOL WINAPI trshader_wgl_choose_pixel_format_arb(HDC hdc,
    const int *attrib_i, const FLOAT *attrib_f, UINT max_formats,
    int *formats, UINT *num_formats) {
  (void)attrib_f;
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory(&pfd,sizeof(pfd));
  pfd.nSize=sizeof(pfd);
  pfd.nVersion=1;
  pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
  pfd.iPixelType=PFD_TYPE_RGBA;
  pfd.cColorBits=32;
  pfd.cDepthBits=24;
  pfd.cStencilBits=8;
  pfd.iLayerType=PFD_MAIN_PLANE;
  trshader_apply_wgl_pixel_format_attribs(&pfd,attrib_i);

  int format=hdc ? ChoosePixelFormat(hdc,&pfd) : 0;
  if(num_formats) *num_formats=format ? 1u : 0u;
  if(format && formats && max_formats>0)
    formats[0]=format;
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_wgl_fallback_count);
  if(n<=16)
    boot_logf("wglChoosePixelFormatARB fallback #%ld hdc=%p format=%d max=%u gle=%lu",
      (long)n,(void*)hdc,format,(unsigned int)max_formats,
      format ? 0ul : (unsigned long)GetLastError());
#endif
  return format ? TRUE : FALSE;
}

static HGLRC WINAPI trshader_wgl_create_context_attribs_arb(
    HDC hdc, HGLRC share, const int *attribs) {
  (void)attribs;
  PFNWGLCREATECONTEXT real_create=trshader_real_wgl_create_context();
  HGLRC rc=real_create ? real_create(hdc) : 0;
  if(rc && share) {
    PFNWGLSHARELISTS real_share=trshader_real_wgl_share_lists();
    if(real_share && !real_share(share,rc))
      boot_logf("wglCreateContextAttribsARB fallback share failed share=%p rc=%p gle=%lu",
        (void*)share,(void*)rc,(unsigned long)GetLastError());
  }
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_wgl_fallback_count);
  if(n<=16)
    boot_logf("wglCreateContextAttribsARB fallback #%ld hdc=%p share=%p rc=%p gle=%lu",
      (long)n,(void*)hdc,(void*)share,(void*)rc,
      rc ? 0ul : (unsigned long)GetLastError());
#endif
  return rc;
}

static PROC trshader_wgl_extension_fallback_proc(const char *name) {
  if(!name) return 0;
  if(lstrcmpA(name,"wglChoosePixelFormatARB")==0)
    return HOOK_PROC(trshader_wgl_choose_pixel_format_arb);
  if(lstrcmpA(name,"wglCreateContextAttribsARB")==0)
    return HOOK_PROC(trshader_wgl_create_context_attribs_arb);
  return 0;
}

__declspec(dllexport) HGLRC WINAPI wglCreateContext(HDC hdc) {
  runtime_start_once();
  boot_logf("wglCreateContext enter hdc=%p",(void*)hdc);
  PFNWGLCREATECONTEXT real=trshader_real_wgl_create_context();
  HGLRC rc=real ? real(hdc) : 0;
  boot_logf("wglCreateContext leave hdc=%p rc=%p gle=%lu",
    (void*)hdc,(void*)rc,rc ? 0ul : (unsigned long)GetLastError());
  return rc;
}

__declspec(dllexport) BOOL WINAPI wglDeleteContext(HGLRC rc) {
  runtime_start_once();
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_wgl_delete_count);
  if(n<=32)
    boot_logf("wglDeleteContext enter #%ld depth=%d rc=%p",
      (long)n,g_wgl_delete_context_depth,(void*)rc);
#endif
  if(g_wgl_delete_context_depth>0) {
#if TR456_STARTUP_LOG
    if(n<=32)
      boot_logf("wglDeleteContext reentrant #%ld rc=%p returning true",
        (long)n,(void*)rc);
#endif
    return TRUE;
  }
  PFNWGLDELETECONTEXT real=trshader_real_wgl_delete_context();
  g_wgl_delete_context_depth++;
  BOOL ok=real ? real(rc) : FALSE;
  g_wgl_delete_context_depth--;
#if TR456_STARTUP_LOG
  if(n<=32)
    boot_logf("wglDeleteContext leave #%ld rc=%p ok=%d gle=%lu",
      (long)n,(void*)rc,(int)ok,ok ? 0ul : (unsigned long)GetLastError());
#endif
  return ok;
}

__declspec(dllexport) HGLRC WINAPI wglGetCurrentContext(void) {
  PFNWGLGETCURRENTCONTEXT real=trshader_real_wgl_get_current_context();
  HGLRC rc=real ? real() : 0;
  return rc;
}

__declspec(dllexport) BOOL WINAPI wglMakeCurrent(HDC hdc, HGLRC rc) {
  runtime_start_once();
  boot_logf("wglMakeCurrent enter hdc=%p rc=%p",(void*)hdc,(void*)rc);
  PFNWGLMAKECURRENT real=trshader_real_wgl_make_current();
  BOOL ok=real ? real(hdc,rc) : FALSE;
  boot_logf("wglMakeCurrent leave hdc=%p rc=%p ok=%d gle=%lu current=%p",
    (void*)hdc,(void*)rc,(int)ok,ok ? 0ul : (unsigned long)GetLastError(),
    (void*)wglGetCurrentContext());
  if(ok && rc)
    trshader_compat_report_once("wglMakeCurrent");
  return ok;
}

__declspec(dllexport) BOOL WINAPI wglSwapBuffers(HDC hdc) {
  runtime_start_once();
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_swap_count);
  if(n<=12)
    boot_logf("wglSwapBuffers enter #%ld hdc=%p",(long)n,(void*)hdc);
#endif
  PFNWGLSWAPBUFFERS real=trshader_real_wgl_swap_buffers();
  draw_swap_debug_overlay();
  BOOL ok=real ? real(hdc) : FALSE;
#if TR456_STARTUP_LOG
  if(n<=12)
    boot_logf("wglSwapBuffers leave #%ld ok=%d gle=%lu",
      (long)n,(int)ok,ok ? 0ul : (unsigned long)GetLastError());
#endif
  advance_frame();
  return ok;
}

__declspec(dllexport) BOOL WINAPI wglSwapLayerBuffers(HDC hdc, UINT planes) {
  runtime_start_once();
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_swap_count);
  if(n<=12)
    boot_logf("wglSwapLayerBuffers enter #%ld hdc=%p planes=%u",
      (long)n,(void*)hdc,(unsigned int)planes);
#endif
  PFNWGLSWAPLAYERBUFFERS real=trshader_real_wgl_swap_layer_buffers();
  draw_swap_debug_overlay();
  BOOL ok=real ? real(hdc,planes) : FALSE;
#if TR456_STARTUP_LOG
  if(n<=12)
    boot_logf("wglSwapLayerBuffers leave #%ld ok=%d gle=%lu",
      (long)n,(int)ok,ok ? 0ul : (unsigned long)GetLastError());
#endif
  advance_frame();
  return ok;
}

typedef struct {
  const char *name;
  PROC proc;
  int require_driver_proc;
} TrshaderHookProc;

static PROC trshader_hook_proc_for_name(LPCSTR name) {
  if(!name || !*name)
    return 0;
  static const TrshaderHookProc hooks[]={
    {"glShaderSource",HOOK_PROC(hook_glShaderSource),0},
    {"glShaderSourceARB",HOOK_PROC(hook_glShaderSource),0},
    {"glCompileShader",HOOK_PROC(hook_glCompileShader),0},
    {"glCompileShaderARB",HOOK_PROC(hook_glCompileShader),0},
    {"glLinkProgram",HOOK_PROC(hook_glLinkProgram),0},
    {"glLinkProgramARB",HOOK_PROC(hook_glLinkProgram),0},
    {"glAttachShader",HOOK_PROC(hook_glAttachShader),0},
    {"glUseProgram",HOOK_PROC(hook_glUseProgram),0},
    {"glActiveTexture",HOOK_PROC(hook_glActiveTexture),0},
    {"glActiveTextureARB",HOOK_PROC(hook_glActiveTexture),0},
    {"glBindTexture",HOOK_PROC(hook_glBindTexture),0},
    {"glBindFramebuffer",HOOK_PROC(hook_glBindFramebuffer),0},
    {"glBindFramebufferEXT",HOOK_PROC(hook_glBindFramebuffer),0},
    {"glViewport",HOOK_PROC(hook_glViewport),0},
    {"glEnable",HOOK_PROC(hook_glEnable),0},
    {"glDisable",HOOK_PROC(hook_glDisable),0},
    {"glDepthMask",HOOK_PROC(hook_glDepthMask),0},
    {"glDepthFunc",HOOK_PROC(hook_glDepthFunc),0},
    {"glBlendFunc",HOOK_PROC(hook_glBlendFunc),0},
    {"glBlendFuncSeparate",HOOK_PROC(hook_glBlendFuncSeparate),0},
    {"glBlendFuncSeparateEXT",HOOK_PROC(hook_glBlendFuncSeparate),0},
    {"glUniform1i",HOOK_PROC(hook_glUniform1i),0},
    {"glUniform1iv",HOOK_PROC(hook_glUniform1iv),0},
    {"glUniform4f",HOOK_PROC(hook_glUniform4f),0},
    {"glUniform4fv",HOOK_PROC(hook_glUniform4fv),0},
    {"glUniformMatrix4fv",HOOK_PROC(hook_glUniformMatrix4fv),0},
    {"glCompressedTexImage2D",HOOK_PROC(hook_glCompressedTexImage2D),0},
    {"glCompressedTexImage2DARB",HOOK_PROC(hook_glCompressedTexImage2D),0},
    {"glCompressedTexSubImage2D",HOOK_PROC(hook_glCompressedTexSubImage2D),0},
    {"glCompressedTexSubImage2DARB",HOOK_PROC(hook_glCompressedTexSubImage2D),0},
    {"glCompressedTexImage3D",HOOK_PROC(hook_glCompressedTexImage3D),0},
    {"glCompressedTexImage3DARB",HOOK_PROC(hook_glCompressedTexImage3D),0},
    {"glCompressedTexSubImage3D",HOOK_PROC(hook_glCompressedTexSubImage3D),0},
    {"glCompressedTexSubImage3DARB",HOOK_PROC(hook_glCompressedTexSubImage3D),0},
    {"glCompressedTextureSubImage2D",HOOK_PROC(hook_glCompressedTextureSubImage2D),0},
    {"glCompressedTextureSubImage3D",HOOK_PROC(hook_glCompressedTextureSubImage3D),0},
    {"glCompressedTextureImage2DEXT",HOOK_PROC(hook_glCompressedTextureImage2DEXT),0},
    {"glCompressedTextureImage3DEXT",HOOK_PROC(hook_glCompressedTextureImage3DEXT),0},
    {"glCompressedTextureSubImage2DEXT",HOOK_PROC(hook_glCompressedTextureSubImage2DEXT),0},
    {"glCompressedTextureSubImage3DEXT",HOOK_PROC(hook_glCompressedTextureSubImage3DEXT),0},
    {"glTexImage3D",HOOK_PROC(hook_glTexImage3D),0},
    {"glTexSubImage3D",HOOK_PROC(hook_glTexSubImage3D),0},
    {"glTextureSubImage3D",HOOK_PROC(hook_glTextureSubImage3D),0},
    {"glTextureImage3DEXT",HOOK_PROC(hook_glTextureImage3DEXT),0},
    {"glTextureSubImage3DEXT",HOOK_PROC(hook_glTextureSubImage3DEXT),0},
    {"glDrawElements",HOOK_PROC(glDrawElements),0},
    {"glDrawArrays",HOOK_PROC(glDrawArrays),0},
    {"glDrawRangeElements",HOOK_PROC(hook_glDrawRangeElements),0},
    {"glDrawElementsBaseVertex",HOOK_PROC(hook_glDrawElementsBaseVertex),0},
    {"glDrawRangeElementsBaseVertex",HOOK_PROC(hook_glDrawRangeElementsBaseVertex),0},
    {"glDrawArraysInstanced",HOOK_PROC(hook_glDrawArraysInstanced),0},
    {"glDrawArraysInstancedARB",HOOK_PROC(hook_glDrawArraysInstanced),0},
    {"glDrawElementsInstanced",HOOK_PROC(hook_glDrawElementsInstanced),0},
    {"glDrawElementsInstancedARB",HOOK_PROC(hook_glDrawElementsInstanced),0},
    {"glDrawElementsInstancedBaseVertex",HOOK_PROC(hook_glDrawElementsInstancedBaseVertex),0},
    {"glDrawArraysInstancedBaseInstance",HOOK_PROC(hook_glDrawArraysInstancedBaseInstance),1},
    {"glDrawElementsInstancedBaseInstance",HOOK_PROC(hook_glDrawElementsInstancedBaseInstance),1},
    {"glDrawElementsInstancedBaseVertexBaseInstance",HOOK_PROC(hook_glDrawElementsInstancedBaseVertexBaseInstance),1},
    {"glMultiDrawArrays",HOOK_PROC(hook_glMultiDrawArrays),0},
    {"glMultiDrawArraysEXT",HOOK_PROC(hook_glMultiDrawArrays),0},
    {"glMultiDrawElements",HOOK_PROC(hook_glMultiDrawElements),0},
    {"glMultiDrawElementsEXT",HOOK_PROC(hook_glMultiDrawElements),0},
    {"glMultiDrawElementsBaseVertex",HOOK_PROC(hook_glMultiDrawElementsBaseVertex),0},
    {"glDrawArraysIndirect",HOOK_PROC(hook_glDrawArraysIndirect),1},
    {"glDrawElementsIndirect",HOOK_PROC(hook_glDrawElementsIndirect),1},
    {"glMultiDrawArraysIndirect",HOOK_PROC(hook_glMultiDrawArraysIndirect),1},
    {"glMultiDrawElementsIndirect",HOOK_PROC(hook_glMultiDrawElementsIndirect),1}
  };
  const size_t hook_count=sizeof(hooks)/sizeof(hooks[0]);
  for(size_t i=0;i<hook_count;i++) {
    if(lstrcmpA(name,hooks[i].name)!=0)
      continue;
    if(hooks[i].require_driver_proc && !gl_proc(name))
      return 0;
    diag_logf("DIAG hook returned for %s",name);
    return hooks[i].proc;
  }
  return 0;
}

__declspec(dllexport) PROC WINAPI wglGetProcAddress(LPCSTR name) {
  runtime_start_once();
#if TR456_STARTUP_LOG
  LONG boot_n=InterlockedIncrement(&g_boot_wgl_query_count);
  if(boot_n<=260)
    boot_logf("wglGetProcAddress #%ld name=\"%s\"",
      (long)boot_n,name ? name : "(null)");
#endif
#if TR456_DIAG_BUILD
  {
    LONG n=InterlockedIncrement(&g_diag_wgl_query_count);
    if(n<=220)
      diag_logf("DIAG wglGetProcAddress #%ld name=\"%s\"",
        (long)n,name ? name : "(null)");
  }
#endif
  PROC hook=trshader_hook_proc_for_name(name);
  if(hook)
    return hook;
  {
    FARPROC core=old_proc(name);
    if(core) {
#if TR456_STARTUP_LOG
      LONG n=boot_n;
      if(n<=260)
        boot_logf("wglGetProcAddress core-export name=\"%s\" proc=%p",
          name ? name : "(null)",(void*)core);
#endif
      return (PROC)core;
    }
  }
  if(g_real_wgl_query_depth>0) {
    PROC icd=(PROC)icd_proc(name);
    if(icd) {
#if TR456_STARTUP_LOG
      LONG n=boot_n;
      if(n<=260)
        boot_logf("wglGetProcAddress reentrant-icd name=\"%s\" proc=%p",
          name ? name : "(null)",(void*)icd);
#endif
      return icd;
    }
#if TR456_STARTUP_LOG
    LONG n=boot_n;
    if(n<=260)
      boot_logf("wglGetProcAddress reentrant name=\"%s\" returning null",
        name ? name : "(null)");
#endif
    return 0;
  }
  PROC fallback=trshader_wgl_extension_fallback_proc(name);
  PROC icd=(PROC)icd_proc(name);
  if(icd) {
#if TR456_STARTUP_LOG
    LONG n=boot_n;
    if(n<=260)
      boot_logf("wglGetProcAddress icd-direct name=\"%s\" proc=%p",
        name ? name : "(null)",(void*)icd);
#endif
    return icd;
  }
  PFNWGLGETPROCADDRESS real=trshader_real_wgl_get_proc_address();
  if(!real)
    return fallback;
  g_real_wgl_query_depth++;
  PROC p=real(name);
  g_real_wgl_query_depth--;
  if(!trshader_valid_gl_proc(p))
    p=0;
  if(!p && fallback) {
#if TR456_STARTUP_LOG
    LONG n=boot_n;
    if(n<=260)
      boot_logf("wglGetProcAddress fallback-shim name=\"%s\" proc=%p",
        name ? name : "(null)",(void*)fallback);
#endif
    p=fallback;
  }
#if TR456_STARTUP_LOG
  LONG n=boot_n;
  if(n<=260)
    boot_logf("wglGetProcAddress driver-return name=\"%s\" proc=%p",
      name ? name : "(null)",(void*)p);
#endif
  return p;
}

#if TR456_STARTUP_LOG
static LONG WINAPI tr456_unhandled_exception_filter(
    struct _EXCEPTION_POINTERS *info) {
  if(InterlockedCompareExchange(&g_boot_exception_logged,1,0)==0 &&
     info && info->ExceptionRecord) {
    EXCEPTION_RECORD *rec=info->ExceptionRecord;
    boot_logf("UNHANDLED EXCEPTION code=0x%08lX flags=0x%08lX address=%p params=%lu ip=%p",
      (unsigned long)rec->ExceptionCode,
      (unsigned long)rec->ExceptionFlags,
      rec->ExceptionAddress,
      (unsigned long)rec->NumberParameters,
      info->ContextRecord ? (void*)info->ContextRecord->Rip : 0);
    for(DWORD i=0;i<rec->NumberParameters && i<8;i++)
      boot_logf("UNHANDLED EXCEPTION param[%lu]=0x%p",
        (unsigned long)i,(void*)rec->ExceptionInformation[i]);
  }
  if(g_prev_exception_filter)
    return g_prev_exception_filter(info);
  return EXCEPTION_CONTINUE_SEARCH;
}
#endif
