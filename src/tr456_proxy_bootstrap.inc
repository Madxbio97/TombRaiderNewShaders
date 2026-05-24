static int format_path(char *out, const char *base, const char *file) {
  int n=snprintf(out,MAX_PATH,"%s\\%s",base,file);
  if(n<0 || n>=MAX_PATH) {
    out[0]=0;
    return 0;
  }
  return 1;
}

static void set_dir(void) {
  DWORD n=GetModuleFileNameA(g_self,g_dir,MAX_PATH);
  if(!n || n>=MAX_PATH) { g_dir[0]=0; return; }
  for(DWORD i=n;i>0;i--) {
    if(g_dir[i-1]=='\\' || g_dir[i-1]=='/') {
      g_dir[i-1]=0;
      if(!format_path(g_mod_dir,g_dir,"tr456_water"))
        g_mod_dir[0]=0;
      return;
    }
  }
  g_dir[0]=0;
  g_mod_dir[0]=0;
}

static int join_game_path(char *out, const char *file) {
  return g_dir[0] && format_path(out,g_dir,file);
}

static int join_mod_path(char *out, const char *file) {
  return g_mod_dir[0] && format_path(out,g_mod_dir,file);
}

static void log_line(const char *line);
static void boot_log_line(const char *line);
static void boot_logf(const char *fmt, ...);
static int diag_is_active(void);
static int runtime_verbose_log(void);
static void perf_log_summary(const char *where, int reset_after);
static void invalidate_effective_contact_cache(void);
static int ini_int(const char *key, int fallback);
static float ini_float(const char *key, float fallback);
static void ini_string(const char *key, const char *fallback, char *out,
                       size_t out_size);
static void trshader_compat_read_config(void);
static void trshader_compat_apply_runtime_policy(void);
static void trshader_compat_report_once(const char *where);
static int trshader_compat_shader_binary_cache_enabled(void);
static void trshader_compat_note_synthetic_failure(const char *reason);
static void trshader_compat_clear_gl_errors(void);
static void trshader_compat_note_synthetic_error(GLenum err,
                                                 const char *where);
static void invalidate_synthetic_surface_program(void);

static int file_exists(const char *path) {
  if(!path || !path[0]) return 0;
  DWORD attr=GetFileAttributesA(path);
  return attr!=INVALID_FILE_ATTRIBUTES && !(attr&FILE_ATTRIBUTE_DIRECTORY);
}

static DWORD file_size_quick(const char *path) {
  if(!path || !path[0]) return INVALID_FILE_SIZE;
  HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,
    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return INVALID_FILE_SIZE;
  DWORD size=GetFileSize(h,0);
  CloseHandle(h);
  return size;
}

static int system_opengl_path(char *out) {
  UINT n=GetSystemDirectoryA(out,MAX_PATH);
  if(!n || n>=MAX_PATH) {
    out[0]=0;
    return 0;
  }
  if(lstrlenA(out)>MAX_PATH-14) {
    out[0]=0;
    return 0;
  }
  lstrcatA(out,"\\opengl32.dll");
  return 1;
}

static int buffer_contains_bytes(const unsigned char *buf, DWORD size,
                                 const char *needle) {
  if(!buf || !needle || !needle[0]) return 0;
  const DWORD needle_len=(DWORD)strlen(needle);
  if(size<needle_len) return 0;
  for(DWORD i=0;i<=size-needle_len;i++) {
    if(!memcmp(buf+i,needle,needle_len)) return 1;
  }
  return 0;
}

static int file_contains_text_marker(const char *path, const char *marker) {
  if(!path || !path[0] || !marker || !marker[0]) return 0;
  HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,
    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return 0;

  const DWORD marker_len=(DWORD)strlen(marker);
  unsigned char buf[8192+64];
  DWORD carry=0;
  for(;;) {
    DWORD got=0;
    if(!ReadFile(h,buf+carry,8192,&got,0) || got==0) break;
    const DWORD total=carry+got;
    if(buffer_contains_bytes(buf,total,marker)) {
      CloseHandle(h);
      return 1;
    }
    if(marker_len>1 && total>=marker_len-1) {
      carry=marker_len-1;
      memmove(buf,buf+total-carry,carry);
    } else {
      carry=total;
    }
  }
  CloseHandle(h);
  return 0;
}

static int is_own_proxy_file(const char *path) {
  return file_contains_text_marker(path,"tr456 water proxy loaded") ||
         file_contains_text_marker(path,"tr456_water_proxy.log");
}

static int is_mesa_chain_opengl_file(const char *path) {
  return file_contains_text_marker(path,"libgallium_wgl");
}

static int runtime_vulkan_only(void) {
  return ini_int("VulkanOnly",1);
}

static void force_mesa_zink_env_if_needed(const char *path, const char *label) {
  if(!ini_int("MesaZinkChain",1)) return;
  if(!path || !path[0] || !is_mesa_chain_opengl_file(path)) return;

  SetEnvironmentVariableA("GALLIUM_DRIVER","zink");
  SetEnvironmentVariableA("MESA_LOADER_DRIVER_OVERRIDE","zink");
  SetEnvironmentVariableA("LIBGL_ALWAYS_SOFTWARE","0");

  char msg[224];
  snprintf(msg,sizeof(msg),
    "forced Mesa/Zink environment for %s",
    label && label[0] ? label : "OpenGL chain DLL");
  log_line(msg);
  boot_logf("chain mesa-zink-env label=%s path=\"%s\"",
    label && label[0] ? label : "chain",path);
}

static int should_load_chain_opengl(const char *path, const char *system_path,
                                    const char *label) {
  if(!path || !path[0]) {
    boot_logf("chain skip label=%s path=\"\" reason=missing-path",
      label && label[0] ? label : "chain");
    return 0;
  }
  DWORD chain_size=file_size_quick(path);
  if(chain_size==INVALID_FILE_SIZE) {
    boot_logf("chain skip label=%s path=\"%s\" reason=size-failed gle=%lu",
      label && label[0] ? label : "chain",path,
      (unsigned long)GetLastError());
    return 0;
  }
  if(system_path && system_path[0]) {
    DWORD system_size=file_size_quick(system_path);
    if(system_size!=INVALID_FILE_SIZE && chain_size==system_size) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "skipped %s because it matches system OpenGL",
        label && label[0] ? label : "chain OpenGL DLL");
      log_line(msg);
      boot_logf("chain skip label=%s path=\"%s\" reason=system-size-match size=%lu",
        label && label[0] ? label : "chain",path,(unsigned long)chain_size);
      return 0;
    }
  }
  if(chain_size<=768u*1024u && is_own_proxy_file(path)) {
    char msg[192];
    snprintf(msg,sizeof(msg),
      "skipped %s because it is another TR456 proxy",
      label && label[0] ? label : "chain OpenGL DLL");
    log_line(msg);
    boot_logf("chain skip label=%s path=\"%s\" reason=own-proxy size=%lu",
      label && label[0] ? label : "chain",path,(unsigned long)chain_size);
    return 0;
  }
  return 1;
}

static int runtime_path(char *out, const char *file) {
  char path[MAX_PATH];
  if(join_mod_path(path,file) && file_exists(path)) {
    lstrcpynA(out,path,MAX_PATH);
    return 1;
  }
  return join_game_path(out,file);
}

static HANDLE open_log_handle(void) {
  char path[MAX_PATH];
  HANDLE h=INVALID_HANDLE_VALUE;
  if(join_mod_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE && join_game_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  return h;
}

static int logging_marker_present(void) {
  char path[MAX_PATH];
  return join_game_path(path,"logs.txt") && file_exists(path);
}

static void boot_log_line(const char *line) {
#if TR456_STARTUP_LOG
  if(!g_dir[0] || !line) return;
  char path[MAX_PATH];
  HANDLE h=INVALID_HANDLE_VALUE;
  AcquireSRWLockExclusive(&g_boot_log_lock);
  if(g_mod_dir[0]) {
    CreateDirectoryA(g_mod_dir,0);
    if(format_path(path,g_mod_dir,"tr456_water_startup.log"))
      h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  }
  if(h==INVALID_HANDLE_VALUE &&
     join_game_path(path,"tr456_water_startup.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
      0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h!=INVALID_HANDLE_VALUE) {
    char prefix[96];
    DWORD w=0;
    snprintf(prefix,sizeof(prefix),"[%010llu p%lu t%lu] ",
      (unsigned long long)GetTickCount64(),
      (unsigned long)GetCurrentProcessId(),
      (unsigned long)GetCurrentThreadId());
    WriteFile(h,prefix,(DWORD)strlen(prefix),&w,0);
    WriteFile(h,line,(DWORD)strlen(line),&w,0);
    WriteFile(h,"\r\n",2,&w,0);
    CloseHandle(h);
  }
  ReleaseSRWLockExclusive(&g_boot_log_lock);
#else
  (void)line;
#endif
}

static void boot_logf(const char *fmt, ...) {
#if TR456_STARTUP_LOG
  char msg[1024];
  va_list ap;
  va_start(ap,fmt);
  vsnprintf(msg,sizeof(msg),fmt,ap);
  va_end(ap);
  msg[sizeof(msg)-1]=0;
  boot_log_line(msg);
#else
  (void)fmt;
#endif
}

static void log_line(const char *line) {
  if(!g_dir[0] || !line) return;
  if(!logging_marker_present()) {
    AcquireSRWLockExclusive(&g_log_lock);
    if(g_log_handle!=INVALID_HANDLE_VALUE) {
      CloseHandle(g_log_handle);
      g_log_handle=INVALID_HANDLE_VALUE;
    }
    ReleaseSRWLockExclusive(&g_log_lock);
    return;
  }
  AcquireSRWLockExclusive(&g_log_lock);
  if(g_log_handle==INVALID_HANDLE_VALUE)
    g_log_handle=open_log_handle();
  if(g_log_handle==INVALID_HANDLE_VALUE) {
    ReleaseSRWLockExclusive(&g_log_lock);
    return;
  }
  DWORD w=0;
  WriteFile(g_log_handle,line,(DWORD)strlen(line),&w,0);
  WriteFile(g_log_handle,"\r\n",2,&w,0);
  ReleaseSRWLockExclusive(&g_log_lock);
}

#if TR456_DIAG_BUILD
static void diag_logf(const char *fmt, ...) {
  char msg[768];
  va_list ap;
  va_start(ap,fmt);
  vsnprintf(msg,sizeof(msg),fmt,ap);
  va_end(ap);
  msg[sizeof(msg)-1]=0;
  log_line(msg);
}
#else
static void diag_logf(const char *fmt, ...) {
  (void)fmt;
}
#endif

static const char *shader_type_name(int type) {
  switch(type) {
    case SHADER_WATER_SURFACE: return "surface";
    case SHADER_WATER_REFLECT: return "reflect";
    case SHADER_WATER_SSR: return "ssr";
    case SHADER_WATER_FLOW: return "flow";
    case SHADER_WATER_RIPPLE: return "ripple";
    default: return "unknown";
  }
}

static int is_tracked_water_shader_type(int type) {
  return type>=SHADER_WATER_SURFACE && type<=SHADER_WATER_RIPPLE;
}

static float f_abs(float x) {
  return x<0.0f ? -x : x;
}

static float f_sqrt(float x) {
  return x<=0.0f ? 0.0f : (float)sqrt((double)x);
}

static float f_min(float a, float b) {
  return a<b ? a : b;
}

static float f_max(float a, float b) {
  return a>b ? a : b;
}

static int synthetic_surface_lara_contact(
    const GLfloat joints[96][4], const GLfloat view[16],
    int min_joints, int max_age_frames, GLfloat margin, GLfloat vertical,
    GLfloat above_surface, GLfloat dir,
    int *surface_out, int *joint_out, int *joint_count_out,
    GLfloat *dist_out, GLfloat *dy_out, GLfloat *water_y_out) {
  if(!joints || !view) return 0;
  if(min_joints<1) min_joints=1;
  if(min_joints>32) min_joints=32;
  if(max_age_frames<0) max_age_frames=0;
  if(margin<0.0f) margin=0.0f;
  if(vertical<1.0f) vertical=1.0f;
  if(above_surface<0.0f) above_surface=0.0f;
  if(above_surface>512.0f) above_surface=512.0f;
  dir=dir>=0.0f ? 1.0f : -1.0f;

  int best_surface=-1;
  int best_joint=-1;
  int best_count=0;
  GLfloat best_dist=1000000000.0f;
  GLfloat best_dy=1000000000.0f;
  GLfloat best_water_y=0.0f;

  for(int i=0;i<(int)(sizeof(g_synthetic_contact_surfaces)/
      sizeof(g_synthetic_contact_surfaces[0]));i++) {
    const SyntheticContactSurface *s=&g_synthetic_contact_surfaces[i];
    if(!s->frame) continue;
    unsigned int age=g_frame_index>=s->frame ? g_frame_index-s->frame : 0u;
    if(age>(unsigned int)max_age_frames) continue;
    GLfloat width=s->max_x-s->min_x;
    GLfloat depth=s->max_z-s->min_z;
    if(width<32.0f || depth<32.0f) continue;

    GLfloat water_y=(s->min_y+s->max_y)*0.5f;
    int near_count=0;
    int near_joint=-1;
    GLfloat near_dist=1000000000.0f;
    GLfloat near_dy=1000000000.0f;

    for(int j=0;j<32;j++) {
      GLfloat jx=joints[j*3+0][3]+view[3];
      GLfloat jy=joints[j*3+1][3]+view[7];
      GLfloat jz=joints[j*3+2][3]+view[11];
      GLfloat dx=0.0f;
      GLfloat dz=0.0f;
      if(jx<s->min_x-margin) dx=(s->min_x-margin)-jx;
      else if(jx>s->max_x+margin) dx=jx-(s->max_x+margin);
      if(jz<s->min_z-margin) dz=(s->min_z-margin)-jz;
      else if(jz>s->max_z+margin) dz=jz-(s->max_z+margin);
      GLfloat dist=f_sqrt(dx*dx+dz*dz);
      GLfloat dy=f_abs(jy-water_y);
      GLfloat above_excess=dir<0.0f ?
        jy-(water_y+above_surface) : (water_y-above_surface)-jy;
      GLfloat below_excess=dir<0.0f ?
        (water_y-vertical)-jy : jy-(water_y+vertical);
      GLfloat vertical_excess=f_max(f_max(above_excess,below_excess),0.0f);
      if(dist<=0.001f && vertical_excess<=0.001f) {
        near_count++;
        if(dy<near_dy) {
          near_dy=dy;
          near_dist=dist;
          near_joint=j;
        }
      }
      GLfloat score=dist+vertical_excess;
      if(score<best_dist) {
        best_dist=score;
        best_dy=dy;
        best_surface=i;
        best_joint=j;
        best_water_y=water_y;
      }
    }

    if(near_count>best_count ||
       (near_count==best_count && near_count>0 && near_dy<best_dy)) {
      best_count=near_count;
      best_surface=i;
      best_joint=near_joint;
      best_dist=near_dist;
      best_dy=near_dy;
      best_water_y=water_y;
    }
    if(near_count>=min_joints) {
      if(surface_out) *surface_out=i;
      if(joint_out) *joint_out=near_joint;
      if(joint_count_out) *joint_count_out=near_count;
      if(dist_out) *dist_out=near_dist;
      if(dy_out) *dy_out=near_dy;
      if(water_y_out) *water_y_out=water_y;
      return 1;
    }
  }

  if(surface_out) *surface_out=best_surface;
  if(joint_out) *joint_out=best_joint;
  if(joint_count_out) *joint_count_out=best_count;
  if(dist_out) *dist_out=best_dist;
  if(dy_out) *dy_out=best_dy;
  if(water_y_out) *water_y_out=best_water_y;
  return 0;
}

static int chain_dll_filename_ok(const char *file) {
  if(!file || !file[0]) return 0;
  if(strlen(file)>180) return 0;
  for(const char *p=file;*p;p++) {
    if(*p=='\\' || *p=='/' || *p==':')
      return 0;
  }
  return 1;
}

static int try_load_chain_opengl(const char *file, const char *label,
                                 const char *system_path) {
  if(!chain_dll_filename_ok(file)) {
    if(file && file[0]) {
      char msg[224];
      snprintf(msg,sizeof(msg),
        "skipped unsafe OpenGL chain filename \"%s\"",file);
      log_line(msg);
      boot_logf("chain skip unsafe filename=\"%s\"",file);
    }
    return 0;
  }

  char path[MAX_PATH];
  if(!join_game_path(path,file)) return 0;
  diag_logf("DIAG chain candidate label=\"%s\" path=\"%s\" exists=%d",
    label && label[0] ? label : file,path,file_exists(path));
  boot_logf("chain candidate label=%s file=%s path=\"%s\" exists=%d",
    label && label[0] ? label : file,file,path,file_exists(path));
  if(!file_exists(path)) return 0;
  if(!should_load_chain_opengl(path,system_path,
      label && label[0] ? label : file))
    return 0;
  if(runtime_vulkan_only() &&
     lstrcmpiA(file,"OpenGL32_orig.dll")==0 &&
     !is_mesa_chain_opengl_file(path)) {
    char msg[224];
    snprintf(msg,sizeof(msg),
      "skipped %s because VulkanOnly requires Mesa/Zink",
      label && label[0] ? label : file);
    log_line(msg);
    boot_logf("chain skip label=%s path=\"%s\" reason=vulkan-only-non-mesa",
      label && label[0] ? label : file,path);
    return 0;
  }

  force_mesa_zink_env_if_needed(path,label && label[0] ? label : file);
  g_old_gl=LoadLibraryA(path);
  if(g_old_gl) {
    char msg[224];
    snprintf(msg,sizeof(msg),"loaded %s",label && label[0] ? label : file);
    log_line(msg);
    boot_logf("chain loaded label=%s path=\"%s\" handle=%p",
      label && label[0] ? label : file,path,(void*)g_old_gl);
    return 1;
  }
  boot_logf("chain LoadLibrary failed label=%s path=\"%s\" gle=%lu",
    label && label[0] ? label : file,path,(unsigned long)GetLastError());
  diag_logf("DIAG LoadLibrary %s failed gle=%lu",path,
    (unsigned long)GetLastError());
  return 0;
}

static void ensure_old_gl(void) {
  if(g_old_gl) return;
  char system_path[MAX_PATH];
  system_opengl_path(system_path);
  boot_logf("ensure_old_gl begin dir=\"%s\" mod=\"%s\" system=\"%s\"",
    g_dir,g_mod_dir,system_path);
  diag_logf("DIAG ensure_old_gl begin dir=\"%s\" mod=\"%s\" system=\"%s\"",
    g_dir,g_mod_dir,system_path);
  int vulkan_only=runtime_vulkan_only();
  if(g_dir[0]) {
    if(!vulkan_only && ini_int("ReShadeChain",1)) {
      char reshade_file[MAX_PATH];
      ini_string("ReShadeDll","OpenGL32_reshade.dll",
        reshade_file,sizeof(reshade_file));
      if(try_load_chain_opengl(reshade_file,"ReShade OpenGL DLL",
          system_path))
        return;
      if(lstrcmpiA(reshade_file,"OpenGL32_reshade.dll")!=0 &&
         try_load_chain_opengl("OpenGL32_reshade.dll",
           "ReShade OpenGL DLL",system_path))
        return;
      if(try_load_chain_opengl("ReShade64.dll","ReShade64.dll",
          system_path))
        return;
    }
    if(try_load_chain_opengl("OpenGL32_orig.dll","OpenGL32_orig.dll",
        system_path)) {
      return;
    }
  }
  if(vulkan_only) {
    log_line("VulkanOnly=1 requires OpenGL32_orig.dll to be Mesa/Zink; system OpenGL fallback disabled");
    boot_logf("system opengl skipped reason=vulkan-only");
    diag_logf("DIAG system OpenGL skipped because VulkanOnly=1");
    return;
  }
  if(!system_path[0]) return;
  g_old_gl=LoadLibraryA(system_path);
  if(g_old_gl) {
    log_line("loaded system opengl32.dll");
    boot_logf("system opengl loaded path=\"%s\" handle=%p",
      system_path,(void*)g_old_gl);
  } else {
    boot_logf("system opengl LoadLibrary failed path=\"%s\" gle=%lu",
      system_path,(unsigned long)GetLastError());
    diag_logf("DIAG LoadLibrary system opengl32.dll failed gle=%lu",
      (unsigned long)GetLastError());
  }
}

FARPROC old_proc(const char *name) {
  if(!name || !*name) {
#if TR456_STARTUP_LOG
    LONG n=InterlockedIncrement(&g_boot_old_proc_count);
    if(n<=260)
      boot_logf("old_proc #%ld name=%s handle=%p proc=%p gle=%lu",
        (long)n,name ? name : "(null)",(void*)g_old_gl,(void*)0,0ul);
#endif
    return 0;
  }
  ensure_old_gl();
  FARPROC p=g_old_gl ? GetProcAddress(g_old_gl,name) : 0;
#if TR456_STARTUP_LOG
  LONG n=InterlockedIncrement(&g_boot_old_proc_count);
  if(n<=260)
    boot_logf("old_proc #%ld name=%s handle=%p proc=%p gle=%lu",
      (long)n,name ? name : "(null)",(void*)g_old_gl,(void*)p,
      p ? 0ul : (unsigned long)GetLastError());
#endif
  return p;
}

static int trshader_valid_gl_proc(PROC p) {
  return p && (uintptr_t)p>4u && (uintptr_t)p!=(uintptr_t)-1;
}

static int trshader_icd_cacheable_name(const char *name) {
  return name && strlen(name)<sizeof(g_icd_proc_cache[0].name);
}

static FARPROC trshader_icd_cache_lookup(const char *name) {
  if(!trshader_icd_cacheable_name(name))
    return 0;
  FARPROC p=0;
  AcquireSRWLockShared(&g_icd_lock);
  for(unsigned int i=0;i<TR456_ICD_PROC_CACHE_SIZE;i++) {
    if(g_icd_proc_cache[i].proc &&
       lstrcmpA(g_icd_proc_cache[i].name,name)==0) {
      p=g_icd_proc_cache[i].proc;
      break;
    }
  }
  ReleaseSRWLockShared(&g_icd_lock);
  return p;
}

static void trshader_icd_cache_store(const char *name, FARPROC proc) {
  if(!proc || !trshader_icd_cacheable_name(name))
    return;
  AcquireSRWLockExclusive(&g_icd_lock);
  for(unsigned int i=0;i<TR456_ICD_PROC_CACHE_SIZE;i++) {
    if(g_icd_proc_cache[i].proc &&
       lstrcmpA(g_icd_proc_cache[i].name,name)==0) {
      ReleaseSRWLockExclusive(&g_icd_lock);
      return;
    }
  }
  unsigned int slot=g_icd_proc_cache_cursor++ % TR456_ICD_PROC_CACHE_SIZE;
  lstrcpynA(g_icd_proc_cache[slot].name,name,
    sizeof(g_icd_proc_cache[slot].name));
  g_icd_proc_cache[slot].proc=proc;
  ReleaseSRWLockExclusive(&g_icd_lock);
}

static int trshader_refresh_icd_resolvers(LONG query_index) {
  AcquireSRWLockShared(&g_icd_lock);
  int ready=g_icd_resolver_count;
  ReleaseSRWLockShared(&g_icd_lock);
  if(ready>0)
    return ready;

  HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,
    GetCurrentProcessId());
  if(snap==INVALID_HANDLE_VALUE) {
    if(query_index>0 && query_index<=80)
      boot_logf("icd resolver snapshot failed gle=%lu",
        (unsigned long)GetLastError());
    return 0;
  }

  TrshaderIcdResolver found[TR456_MAX_ICD_RESOLVERS];
  ZeroMemory(found,sizeof(found));
  int found_count=0;
  MODULEENTRY32 me;
  ZeroMemory(&me,sizeof(me));
  me.dwSize=sizeof(me);
  if(Module32First(snap,&me)) {
    do {
      HMODULE mod=me.hModule;
      if(!mod || mod==g_self || mod==g_old_gl)
        continue;
      PFNDRVGETPROCADDRESS drv=
        (PFNDRVGETPROCADDRESS)GetProcAddress(mod,"DrvGetProcAddress");
      if(!drv)
        continue;
      found[found_count].module=mod;
      found[found_count].get_proc=drv;
      lstrcpynA(found[found_count].name,me.szModule,
        sizeof(found[found_count].name));
      found_count++;
      if(query_index>0 && query_index<=80)
        boot_logf("icd resolver found module=\"%s\" drv=%p",
          me.szModule,(void*)drv);
      if(found_count>=TR456_MAX_ICD_RESOLVERS)
        break;
    } while(Module32Next(snap,&me));
  } else if(query_index>0 && query_index<=80) {
    boot_logf("icd resolver first failed gle=%lu",
      (unsigned long)GetLastError());
  }
  CloseHandle(snap);

  if(found_count<=0)
    return 0;

  AcquireSRWLockExclusive(&g_icd_lock);
  if(g_icd_resolver_count<=0) {
    memcpy(g_icd_resolvers,found,sizeof(found[0])*(size_t)found_count);
    g_icd_resolver_count=found_count;
  }
  ready=g_icd_resolver_count;
  ReleaseSRWLockExclusive(&g_icd_lock);
  return ready;
}

static FARPROC icd_proc(const char *name) {
  if(!name || !*name)
    return 0;
  FARPROC cached=trshader_icd_cache_lookup(name);
  if(cached)
    return cached;
#if TR456_STARTUP_LOG
  LONG q=InterlockedIncrement(&g_boot_icd_query_count);
#else
  LONG q=0;
#endif
  if(!trshader_refresh_icd_resolvers(q))
    return 0;

  TrshaderIcdResolver resolvers[TR456_MAX_ICD_RESOLVERS];
  int resolver_count=0;
  AcquireSRWLockShared(&g_icd_lock);
  resolver_count=g_icd_resolver_count;
  if(resolver_count>TR456_MAX_ICD_RESOLVERS)
    resolver_count=TR456_MAX_ICD_RESOLVERS;
  memcpy(resolvers,g_icd_resolvers,sizeof(resolvers[0])*(size_t)resolver_count);
  ReleaseSRWLockShared(&g_icd_lock);

  FARPROC out=0;
  for(int i=0;i<resolver_count;i++) {
    PFNDRVGETPROCADDRESS drv=resolvers[i].get_proc;
    if(!drv)
      continue;
    PROC p=drv(name);
    if(!trshader_valid_gl_proc(p))
      p=0;
#if TR456_STARTUP_LOG
    if(q<=80)
      boot_logf("icd_proc query name=\"%s\" module=\"%s\" proc=%p",
        name,resolvers[i].name,(void*)p);
#endif
    if(p) {
      out=(FARPROC)p;
#if TR456_STARTUP_LOG
      LONG h=InterlockedIncrement(&g_boot_icd_hit_count);
      if(h<=80)
        boot_logf("icd_proc hit #%ld name=\"%s\" module=\"%s\" proc=%p",
          (long)h,name,resolvers[i].name,(void*)out);
#endif
      trshader_icd_cache_store(name,out);
      break;
    }
  }
  return out;
}

static PFNWGLGETPROCADDRESS trshader_real_wgl_get_proc_address(void) {
  static PFNWGLGETPROCADDRESS p;
  if(!p) p=(PFNWGLGETPROCADDRESS)old_proc("wglGetProcAddress");
  return p;
}

static PFNWGLCREATECONTEXT trshader_real_wgl_create_context(void) {
  static PFNWGLCREATECONTEXT p;
  if(!p) p=(PFNWGLCREATECONTEXT)old_proc("wglCreateContext");
  return p;
}

static PFNWGLDELETECONTEXT trshader_real_wgl_delete_context(void) {
  static PFNWGLDELETECONTEXT p;
  if(!p) p=(PFNWGLDELETECONTEXT)old_proc("wglDeleteContext");
  return p;
}

static PFNWGLGETCURRENTCONTEXT trshader_real_wgl_get_current_context(void) {
  static PFNWGLGETCURRENTCONTEXT p;
  if(!p) p=(PFNWGLGETCURRENTCONTEXT)old_proc("wglGetCurrentContext");
  return p;
}

static PFNWGLMAKECURRENT trshader_real_wgl_make_current(void) {
  static PFNWGLMAKECURRENT p;
  if(!p) p=(PFNWGLMAKECURRENT)old_proc("wglMakeCurrent");
  return p;
}

static PFNWGLSHARELISTS trshader_real_wgl_share_lists(void) {
  static PFNWGLSHARELISTS p;
  if(!p) p=(PFNWGLSHARELISTS)old_proc("wglShareLists");
  return p;
}

static PFNWGLSWAPBUFFERS trshader_real_wgl_swap_buffers(void) {
  static PFNWGLSWAPBUFFERS p;
  if(!p) p=(PFNWGLSWAPBUFFERS)old_proc("wglSwapBuffers");
  return p;
}

static PFNWGLSWAPLAYERBUFFERS trshader_real_wgl_swap_layer_buffers(void) {
  static PFNWGLSWAPLAYERBUFFERS p;
  if(!p) p=(PFNWGLSWAPLAYERBUFFERS)old_proc("wglSwapLayerBuffers");
  return p;
}

static FARPROC gl_proc(const char *name) {
  FARPROC p=old_proc(name);
  if(p)
    return p;
  p=icd_proc(name);
  if(p)
    return p;
  PFNWGLGETPROCADDRESS real_wgl=trshader_real_wgl_get_proc_address();
  if(real_wgl) {
    if(g_real_wgl_query_depth>0) {
      boot_logf("gl_proc reentrant real wglGetProcAddress name=\"%s\" returning null",
        name ? name : "(null)");
      return 0;
    }
    g_real_wgl_query_depth++;
    p=(FARPROC)real_wgl(name);
    g_real_wgl_query_depth--;
  }
  if(!trshader_valid_gl_proc((PROC)p))
    p=0;
  return p;
}
