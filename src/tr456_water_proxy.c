#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef char GLchar;

static HMODULE g_self;
static HMODULE g_old_gl;
static char g_dir[MAX_PATH];
static char g_mod_dir[MAX_PATH];

static const char k_surface_key[]="vec3 tc = vWorldPos.xyz / 1024.0 * uParams.x;";
static const char k_reflect_key[]="float hC = texture(sNoise, vec3(uv, t)).x;";
static const char k_ssr_key[]="float not_water = 1 - texture(sTex0, vec3(uv_refract.xy, 0)).w;";

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE15 0x84CF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_VIEWPORT 0x0BA2
#define GL_RGBA 0x1908
#define GL_RGBA8 0x8058
#define GL_UNSIGNED_BYTE 0x1401
#define GL_LINEAR 0x2601
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803

enum {
  SHADER_WATER_SURFACE=1,
  SHADER_WATER_REFLECT=2,
  SHADER_WATER_SSR=3
};

typedef struct {
  GLuint shader;
  int type;
} ShaderTrack;

typedef struct {
  GLuint program;
  int type;
  GLint scene_loc;
  GLint info_loc;
} ProgramTrack;

static ShaderTrack g_shader_tracks[64];
static ProgramTrack g_program_tracks[64];
static GLuint g_current_program;
static int g_current_program_type;
static GLuint g_scene_tex;
static int g_scene_w;
static int g_scene_h;
static int g_logged_capture;
static int g_scene_captured;
static int g_logged_use_ssr;

typedef void (APIENTRY *PFNGLSHADERSOURCE)(GLuint, GLsizei, const GLchar * const *, const GLint *);
typedef PROC (WINAPI *PFNWGLGETPROCADDRESS)(LPCSTR);
typedef void (APIENTRY *PFNGLATTACHSHADER)(GLuint, GLuint);
typedef void (APIENTRY *PFNGLUSEPROGRAM)(GLuint);
typedef void (APIENTRY *PFNGLDRAWELEMENTS)(GLenum, GLsizei, GLenum, const void *);
typedef void (APIENTRY *PFNGLDRAWARRAYS)(GLenum, GLint, GLsizei);
typedef void (APIENTRY *PFNGLDRAWRANGEELEMENTS)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *);
typedef void (APIENTRY *PFNGLDRAWELEMENTSBASEVERTEX)(GLenum, GLsizei, GLenum, const void *, GLint);
typedef void (APIENTRY *PFNGLDRAWRANGEELEMENTSBASEVERTEX)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint);
typedef void (APIENTRY *PFNGLDRAWARRAYSINSTANCED)(GLenum, GLint, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLDRAWELEMENTSINSTANCED)(GLenum, GLsizei, GLenum, const void *, GLsizei);
typedef void (APIENTRY *PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint);
typedef void (APIENTRY *PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE)(GLenum, GLint, GLsizei, GLsizei, GLuint);
typedef void (APIENTRY *PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLuint);
typedef void (APIENTRY *PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint, GLuint);
typedef void (APIENTRY *PFNGLMULTIDRAWARRAYS)(GLenum, const GLint *, const GLsizei *, GLsizei);
typedef void (APIENTRY *PFNGLMULTIDRAWELEMENTS)(GLenum, const GLsizei *, GLenum, const void * const *, GLsizei);
typedef void (APIENTRY *PFNGLMULTIDRAWELEMENTSBASEVERTEX)(GLenum, const GLsizei *, GLenum, const void * const *, GLsizei, const GLint *);
typedef void (APIENTRY *PFNGLDRAWARRAYSINDIRECT)(GLenum, const void *);
typedef void (APIENTRY *PFNGLDRAWELEMENTSINDIRECT)(GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLMULTIDRAWARRAYSINDIRECT)(GLenum, const void *, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLMULTIDRAWELEMENTSINDIRECT)(GLenum, GLenum, const void *, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLGETINTEGERV)(GLenum, GLint *);
typedef void (APIENTRY *PFNGLGENTEXTURES)(GLsizei, GLuint *);
typedef void (APIENTRY *PFNGLBINDTEXTURE)(GLenum, GLuint);
typedef void (APIENTRY *PFNGLTEXPARAMETERI)(GLenum, GLenum, GLint);
typedef void (APIENTRY *PFNGLTEXIMAGE2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLCOPYTEXSUBIMAGE2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLACTIVETEXTURE)(GLenum);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATION)(GLuint, const GLchar *);
typedef void (APIENTRY *PFNGLUNIFORM1I)(GLint, GLint);
typedef void (APIENTRY *PFNGLUNIFORM4F)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef GLenum (APIENTRY *PFNGLGETERROR)(void);
typedef BOOL (WINAPI *PFNWGLSWAPBUFFERS)(HDC);
typedef BOOL (WINAPI *PFNWGLSWAPLAYERBUFFERS)(HDC, UINT);

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

static int file_exists(const char *path) {
  DWORD attr=GetFileAttributesA(path);
  return attr!=INVALID_FILE_ATTRIBUTES && !(attr&FILE_ATTRIBUTE_DIRECTORY);
}

static int runtime_path(char *out, const char *file) {
  char path[MAX_PATH];
  if(join_mod_path(path,file) && file_exists(path)) {
    lstrcpynA(out,path,MAX_PATH);
    return 1;
  }
  return join_game_path(out,file);
}

static void log_line(const char *line) {
  if(!g_dir[0]) return;
  char path[MAX_PATH];
  HANDLE h=INVALID_HANDLE_VALUE;
  if(join_mod_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE && join_game_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return;
  DWORD w=0;
  WriteFile(h,line,(DWORD)strlen(line),&w,0);
  WriteFile(h,"\r\n",2,&w,0);
  CloseHandle(h);
}

static void ensure_old_gl(void) {
  if(g_old_gl) return;
  char path[MAX_PATH];
  if(g_dir[0]) {
    if(join_game_path(path,"OpenGL32_orig.dll")) {
      g_old_gl=LoadLibraryA(path);
      if(g_old_gl) {
        log_line("loaded OpenGL32_orig.dll");
        return;
      }
    }
  }
  UINT n=GetSystemDirectoryA(path,MAX_PATH);
  if(!n || n>=MAX_PATH) return;
  lstrcatA(path,"\\opengl32.dll");
  g_old_gl=LoadLibraryA(path);
  if(g_old_gl) log_line("loaded system opengl32.dll");
}

static FARPROC old_proc(const char *name) {
  ensure_old_gl();
  return g_old_gl ? GetProcAddress(g_old_gl,name) : 0;
}

static FARPROC gl_proc(const char *name) {
  PFNWGLGETPROCADDRESS real_wgl=(PFNWGLGETPROCADDRESS)old_proc("wglGetProcAddress");
  FARPROC p=0;
  if(real_wgl) p=(FARPROC)real_wgl(name);
  if((uintptr_t)p<=4u || (uintptr_t)p==(uintptr_t)-1)
    p=0;
  if(!p) p=old_proc(name);
  return p;
}

static char *read_text(const char *file) {
  if(!g_dir[0]) return 0;
  char path[MAX_PATH];
  if(!runtime_path(path,file)) return 0;
  HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return 0;
  DWORD size=GetFileSize(h,0);
  if(size==INVALID_FILE_SIZE || size>1024*1024) { CloseHandle(h); return 0; }
  char *buf=(char*)malloc(size+1);
  if(!buf) { CloseHandle(h); return 0; }
  DWORD got=0;
  if(!ReadFile(h,buf,size,&got,0)) { CloseHandle(h); free(buf); return 0; }
  CloseHandle(h);
  buf[got]=0;
  return buf;
}

static char *dup_text(const char *text) {
  size_t len=strlen(text);
  char *out=(char*)malloc(len+1);
  if(!out) return 0;
  memcpy(out,text,len+1);
  return out;
}

static int config_path(char *out) {
  return runtime_path(out,"tr456_water.ini");
}

static int ini_int(const char *key, int fallback) {
  if(!g_dir[0]) return fallback;
  char path[MAX_PATH];
  char def[32];
  char buf[64];
  if(!config_path(path)) return fallback;
  snprintf(def,sizeof(def),"%d",fallback);
  GetPrivateProfileStringA("Water",key,def,buf,sizeof(buf),path);
  char *end=0;
  long v=strtol(buf,&end,10);
  return end==buf ? fallback : (int)v;
}

static float ini_float(const char *key, float fallback) {
  if(!g_dir[0]) return fallback;
  char path[MAX_PATH];
  char def[64];
  char buf[64];
  if(!config_path(path)) return fallback;
  snprintf(def,sizeof(def),"%.4f",(double)fallback);
  GetPrivateProfileStringA("Water",key,def,buf,sizeof(buf),path);
  char *end=0;
  double v=strtod(buf,&end);
  return end==buf ? fallback : (float)v;
}

static void shader_defines(char *out, size_t out_size) {
  const int debug=ini_int("DebugMode",0);
  const float surface_wave=ini_float("SurfaceWave",1.0f);
  const float refract=ini_float("RefractStrength",1.0f);
  const float reflect=ini_float("ReflectStrength",1.0f);
  const float ssr=ini_float("SSRStrength",1.0f);
  const float glint=ini_float("GlintStrength",1.0f);
  const float foam=ini_float("FoamStrength",0.75f);
  const float chroma=ini_float("ChromaStrength",0.55f);
  const float tint=ini_float("TintStrength",1.0f);
  const float opacity=ini_float("Opacity",0.62f);
  const float force_reflection=ini_float("ForceReflection",0.65f);
  const float scene_reflection=ini_float("SceneReflectionStrength",1.0f);
  const float caustics=ini_float("CausticsStrength",1.10f);
  const float depth=ini_float("DepthStrength",1.0f);
  const float ripple=ini_float("RippleStrength",0.65f);
  const float ripple_x=ini_float("RippleCenterX",0.50f);
  const float ripple_y=ini_float("RippleCenterY",0.62f);
  const float reflection_contrast=ini_float("ReflectionContrast",1.45f);
  const float rough_reflection=ini_float("RoughReflection",0.85f);
  const float fresnel_strength=ini_float("FresnelStrength",1.10f);
  const float bottom_caustics=ini_float("BottomCaustics",0.85f);
  const float contact_edge=ini_float("ContactEdge",0.72f);
  const float depth_absorption=ini_float("DepthAbsorption",0.88f);
  const float wall_stretch=ini_float("WallReflectionStretch",0.84f);
  const float water_saturation=ini_float("WaterSaturation",1.18f);
  const float water_brightness=ini_float("WaterBrightness",0.86f);
  const int fbo_reflection=ini_int("FramebufferReflection",1);
  snprintf(out,out_size,
    "#define TR456_WATER_DEBUG_MODE %d\n"
    "#define TR456_WATER_SURFACE_WAVE %.6f\n"
    "#define TR456_WATER_REFRACT_STRENGTH %.6f\n"
    "#define TR456_WATER_REFLECT_STRENGTH %.6f\n"
    "#define TR456_WATER_SSR_STRENGTH %.6f\n"
    "#define TR456_WATER_GLINT_STRENGTH %.6f\n"
    "#define TR456_WATER_FOAM_STRENGTH %.6f\n"
    "#define TR456_WATER_CHROMA_STRENGTH %.6f\n"
    "#define TR456_WATER_TINT_STRENGTH %.6f\n"
    "#define TR456_WATER_OPACITY %.6f\n"
    "#define TR456_WATER_FORCE_REFLECTION %.6f\n"
    "#define TR456_WATER_SCENE_REFLECTION %.6f\n"
    "#define TR456_WATER_CAUSTICS_STRENGTH %.6f\n"
    "#define TR456_WATER_DEPTH_STRENGTH %.6f\n"
    "#define TR456_WATER_RIPPLE_STRENGTH %.6f\n"
    "#define TR456_WATER_RIPPLE_CENTER_X %.6f\n"
    "#define TR456_WATER_RIPPLE_CENTER_Y %.6f\n"
    "#define TR456_WATER_REFLECTION_CONTRAST %.6f\n"
    "#define TR456_WATER_ROUGH_REFLECTION %.6f\n"
    "#define TR456_WATER_FRESNEL_STRENGTH %.6f\n"
    "#define TR456_WATER_BOTTOM_CAUSTICS %.6f\n"
    "#define TR456_WATER_CONTACT_EDGE %.6f\n"
    "#define TR456_WATER_DEPTH_ABSORPTION %.6f\n"
    "#define TR456_WATER_WALL_STRETCH %.6f\n"
    "#define TR456_WATER_COLOR_SATURATION %.6f\n"
    "#define TR456_WATER_BRIGHTNESS %.6f\n"
    "#define TR456_WATER_FBO_REFLECTION %d\n",
    debug,(double)surface_wave,(double)refract,(double)reflect,(double)ssr,
    (double)glint,(double)foam,(double)chroma,(double)tint,(double)opacity,
    (double)force_reflection,(double)scene_reflection,(double)caustics,(double)depth,
    (double)ripple,(double)ripple_x,(double)ripple_y,(double)reflection_contrast,
    (double)rough_reflection,(double)fresnel_strength,(double)bottom_caustics,
    (double)contact_edge,(double)depth_absorption,(double)wall_stretch,
    (double)water_saturation,(double)water_brightness,
    fbo_reflection);
}

static char *inject_defines(const char *src) {
  char defs[2048];
  shader_defines(defs,sizeof(defs));
  const size_t defs_len=strlen(defs);
  const size_t src_len=strlen(src);
  const char *nl=0;
  size_t head_len=0;
  if(!strncmp(src,"#version",8)) {
    nl=strchr(src,'\n');
    if(nl) head_len=(size_t)(nl-src)+1;
  }
  char *out=(char*)malloc(src_len+defs_len+2);
  if(!out) return dup_text(src);
  if(head_len) {
    memcpy(out,src,head_len);
    memcpy(out+head_len,defs,defs_len);
    memcpy(out+head_len+defs_len,src+head_len,src_len-head_len+1);
  } else {
    memcpy(out,defs,defs_len);
    memcpy(out+defs_len,src,src_len+1);
  }
  return out;
}

static char *configured_shader(const char *file, const char *label) {
  char *text=read_text(file);
  char msg[128];
  snprintf(msg,sizeof(msg),"%s %s",text ? "using external" : "missing external",label);
  log_line(msg);
  if(!text) return 0;
  char *out=inject_defines(text);
  if(text) free(text);
  return out;
}

static char *surface_shader(void) {
  return configured_shader("tr456_water_surface.glsl","surface shader");
}

static char *reflect_shader(void) {
  return configured_shader("tr456_water_reflect.glsl","reflect shader");
}

static char *ssr_shader(void) {
  return configured_shader("tr456_water_ssr.glsl","screen-space water shader");
}

static uint32_t fnv1a(const char *text) {
  uint32_t h=2166136261u;
  if(!text) return h;
  while(*text) {
    h^=(unsigned char)*text++;
    h*=16777619u;
  }
  return h;
}

static void set_shader_type(GLuint shader, int type) {
  if(!shader || !type) return;
  for(size_t i=0;i<sizeof(g_shader_tracks)/sizeof(g_shader_tracks[0]);i++) {
    if(g_shader_tracks[i].shader==shader || !g_shader_tracks[i].shader) {
      g_shader_tracks[i].shader=shader;
      g_shader_tracks[i].type=type;
      return;
    }
  }
}

static int shader_type(GLuint shader) {
  for(size_t i=0;i<sizeof(g_shader_tracks)/sizeof(g_shader_tracks[0]);i++) {
    if(g_shader_tracks[i].shader==shader) return g_shader_tracks[i].type;
  }
  return 0;
}

static ProgramTrack *program_track(GLuint program, int create) {
  if(!program) return 0;
  for(size_t i=0;i<sizeof(g_program_tracks)/sizeof(g_program_tracks[0]);i++) {
    if(g_program_tracks[i].program==program) return &g_program_tracks[i];
  }
  if(!create) return 0;
  for(size_t i=0;i<sizeof(g_program_tracks)/sizeof(g_program_tracks[0]);i++) {
    if(!g_program_tracks[i].program) {
      g_program_tracks[i].program=program;
      g_program_tracks[i].scene_loc=-2;
      g_program_tracks[i].info_loc=-2;
      return &g_program_tracks[i];
    }
  }
  return 0;
}

static void set_program_type(GLuint program, int type) {
  ProgramTrack *p=program_track(program,1);
  if(p && type>p->type) {
    p->type=type;
    p->scene_loc=-2;
    p->info_loc=-2;
  }
}

static int program_type(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  return p ? p->type : 0;
}

static void prepare_scene_capture(const char *reason) {
  if(!g_current_program_type || !ini_int("FramebufferReflection",1))
    return;

  PFNGLGETINTEGERV get_integer=(PFNGLGETINTEGERV)gl_proc("glGetIntegerv");
  PFNGLGENTEXTURES gen_textures=(PFNGLGENTEXTURES)gl_proc("glGenTextures");
  PFNGLBINDTEXTURE bind_texture=(PFNGLBINDTEXTURE)gl_proc("glBindTexture");
  PFNGLTEXPARAMETERI tex_parameter_i=(PFNGLTEXPARAMETERI)gl_proc("glTexParameteri");
  PFNGLTEXIMAGE2D tex_image_2d=(PFNGLTEXIMAGE2D)gl_proc("glTexImage2D");
  PFNGLCOPYTEXSUBIMAGE2D copy_tex_sub_image_2d=(PFNGLCOPYTEXSUBIMAGE2D)gl_proc("glCopyTexSubImage2D");
  PFNGLACTIVETEXTURE active_texture=(PFNGLACTIVETEXTURE)gl_proc("glActiveTexture");
  PFNGLGETUNIFORMLOCATION get_uniform_location=(PFNGLGETUNIFORMLOCATION)gl_proc("glGetUniformLocation");
  PFNGLUNIFORM1I uniform_1i=(PFNGLUNIFORM1I)gl_proc("glUniform1i");
  PFNGLUNIFORM4F uniform_4f=(PFNGLUNIFORM4F)gl_proc("glUniform4f");
  PFNGLGETERROR get_error=(PFNGLGETERROR)gl_proc("glGetError");

  if(!get_integer || !gen_textures || !bind_texture || !tex_parameter_i ||
     !tex_image_2d || !copy_tex_sub_image_2d || !active_texture ||
     !get_uniform_location || !uniform_1i || !uniform_4f) {
    if(!g_logged_capture)
      log_line("framebuffer reflection capture skipped: missing required GL entry point");
    return;
  }

  GLint viewport[4]={0,0,0,0};
  get_integer(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) return;

  GLint old_active=0;
  get_integer(GL_ACTIVE_TEXTURE,&old_active);
  active_texture(GL_TEXTURE15);

  if(!g_scene_tex) {
    gen_textures(1,&g_scene_tex);
    bind_texture(GL_TEXTURE_2D,g_scene_tex);
    tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    g_scene_w=0;
    g_scene_h=0;
  } else {
    bind_texture(GL_TEXTURE_2D,g_scene_tex);
  }

  if(g_scene_w!=viewport[2] || g_scene_h!=viewport[3]) {
    tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,viewport[2],viewport[3],0,GL_RGBA,GL_UNSIGNED_BYTE,0);
    g_scene_w=viewport[2];
    g_scene_h=viewport[3];
  }

  GLenum err=0;
  if(!g_scene_captured) {
    copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],viewport[2],viewport[3]);
    err=get_error ? get_error() : 0;
    g_scene_captured=1;
    if(!g_logged_capture) {
      char msg[224];
      snprintf(msg,sizeof(msg),"framebuffer reflection capture enabled size=%dx%d tex=%u program=%u type=%d reason=%s glerr=0x%04X",
        viewport[2],viewport[3],g_scene_tex,g_current_program,g_current_program_type,
        reason ? reason : "unknown",(unsigned int)err);
      log_line(msg);
      g_logged_capture=1;
    }
  }

  ProgramTrack *track=program_track(g_current_program,1);
  if(track) {
    if(track->scene_loc==-2)
      track->scene_loc=get_uniform_location(g_current_program,"uTrWaterScene");
    if(track->scene_loc>=0)
      uniform_1i(track->scene_loc,15);
    if(track->info_loc==-2)
      track->info_loc=get_uniform_location(g_current_program,"uTrWaterCaptureInfo");
    if(track->info_loc>=0)
      uniform_4f(track->info_loc,1.0f/(GLfloat)viewport[2],1.0f/(GLfloat)viewport[3],
        (GLfloat)viewport[2],(GLfloat)viewport[3]);
  }

  active_texture((GLenum)old_active);

}

static char *join_sources(GLsizei count, const GLchar * const *strings, const GLint *lengths) {
  if(count<=0 || !strings) return 0;
  size_t total=0;
  for(GLsizei i=0;i<count;i++) {
    if(!strings[i]) continue;
    size_t n=(lengths && lengths[i]>=0) ? (size_t)lengths[i] : strlen(strings[i]);
    if(total+n>4*1024*1024) return 0;
    total+=n;
  }
  char *out=(char*)malloc(total+1);
  if(!out) return 0;
  char *p=out;
  for(GLsizei i=0;i<count;i++) {
    if(!strings[i]) continue;
    size_t n=(lengths && lengths[i]>=0) ? (size_t)lengths[i] : strlen(strings[i]);
    memcpy(p,strings[i],n);
    p+=n;
  }
  *p=0;
  return out;
}

static PFNGLSHADERSOURCE real_shader_source(const char *name) {
  static PFNGLSHADERSOURCE p;
  if(p) return p;
  PFNWGLGETPROCADDRESS real_wgl=(PFNWGLGETPROCADDRESS)old_proc("wglGetProcAddress");
  if(real_wgl) p=(PFNGLSHADERSOURCE)real_wgl(name);
  if(!p) p=(PFNGLSHADERSOURCE)old_proc(name);
  return p;
}

static void APIENTRY hook_glShaderSource(GLuint shader, GLsizei count, const GLchar * const *strings, const GLint *lengths) {
  PFNGLSHADERSOURCE real=real_shader_source("glShaderSource");
  if(!real) return;
  char *src=join_sources(count,strings,lengths);
  char *replacement=0;
  const char *tag=0;
  int type=0;
  if(src && strstr(src,k_surface_key)) {
    replacement=surface_shader();
    tag="patched surface shader";
    type=SHADER_WATER_SURFACE;
  } else if(src && strstr(src,k_reflect_key)) {
    replacement=reflect_shader();
    tag="patched reflect shader";
    type=SHADER_WATER_REFLECT;
  } else if(src && strstr(src,k_ssr_key)) {
    replacement=ssr_shader();
    tag="patched screen-space water shader";
    type=SHADER_WATER_SSR;
  }
  if(replacement) {
    set_shader_type(shader,type);
    GLint len=(GLint)strlen(replacement);
    const GLchar *one=replacement;
    real(shader,1,&one,&len);
    char msg[160];
    snprintf(msg,sizeof(msg),"%s shader=%u original_hash=0x%08X replacement_len=%ld",
      tag ? tag : "patched shader",shader,(unsigned int)fnv1a(src),(long)len);
    log_line(msg);
    free(replacement);
  } else {
    real(shader,count,strings,lengths);
  }
  if(src) free(src);
}

static PFNGLATTACHSHADER real_attach_shader(void) {
  static PFNGLATTACHSHADER p;
  if(!p) p=(PFNGLATTACHSHADER)gl_proc("glAttachShader");
  return p;
}

static void APIENTRY hook_glAttachShader(GLuint program, GLuint shader) {
  PFNGLATTACHSHADER real=real_attach_shader();
  if(real) real(program,shader);
  int type=shader_type(shader);
  if(type) {
    set_program_type(program,type);
    char msg[128];
    snprintf(msg,sizeof(msg),"tracked water program=%u shader=%u type=%d",program,shader,type);
    log_line(msg);
  }
}

static PFNGLUSEPROGRAM real_use_program(void) {
  static PFNGLUSEPROGRAM p;
  if(!p) p=(PFNGLUSEPROGRAM)gl_proc("glUseProgram");
  return p;
}

static void APIENTRY hook_glUseProgram(GLuint program) {
  PFNGLUSEPROGRAM real=real_use_program();
  if(real) real(program);
  g_current_program=program;
  g_current_program_type=program_type(program);
  if(g_current_program_type==SHADER_WATER_SSR && !g_logged_use_ssr) {
    char msg[128];
    snprintf(msg,sizeof(msg),"using tracked screen-space water program=%u",program);
    log_line(msg);
    g_logged_use_ssr=1;
  }
  if(g_current_program_type)
    prepare_scene_capture("glUseProgram");
}

static PFNGLDRAWELEMENTS real_draw_elements(void) {
  static PFNGLDRAWELEMENTS p;
  if(!p) p=(PFNGLDRAWELEMENTS)gl_proc("glDrawElements");
  return p;
}

__declspec(dllexport) void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
  PFNGLDRAWELEMENTS real=real_draw_elements();
  if(real) {
    prepare_scene_capture("glDrawElements");
    real(mode,count,type,indices);
  }
}

static PFNGLDRAWARRAYS real_draw_arrays(void) {
  static PFNGLDRAWARRAYS p;
  if(!p) p=(PFNGLDRAWARRAYS)gl_proc("glDrawArrays");
  return p;
}

__declspec(dllexport) void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  PFNGLDRAWARRAYS real=real_draw_arrays();
  if(real) {
    prepare_scene_capture("glDrawArrays");
    real(mode,first,count);
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
    prepare_scene_capture("glDrawRangeElements");
    real(mode,start,end,count,type,indices);
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
    prepare_scene_capture("glDrawElementsBaseVertex");
    real(mode,count,type,indices,base_vertex);
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
    prepare_scene_capture("glDrawRangeElementsBaseVertex");
    real(mode,start,end,count,type,indices,base_vertex);
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
    prepare_scene_capture("glDrawArraysInstanced");
    real(mode,first,count,instance_count);
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
    prepare_scene_capture("glDrawElementsInstanced");
    real(mode,count,type,indices,instance_count);
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
    prepare_scene_capture("glDrawElementsInstancedBaseVertex");
    real(mode,count,type,indices,instance_count,base_vertex);
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
    prepare_scene_capture("glDrawArraysInstancedBaseInstance");
    real(mode,first,count,instance_count,base_instance);
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
    prepare_scene_capture("glDrawElementsInstancedBaseInstance");
    real(mode,count,type,indices,instance_count,base_instance);
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
    prepare_scene_capture("glDrawElementsInstancedBaseVertexBaseInstance");
    real(mode,count,type,indices,instance_count,base_vertex,base_instance);
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
    prepare_scene_capture("glMultiDrawArrays");
    real(mode,first,count,draw_count);
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
    prepare_scene_capture("glMultiDrawElements");
    real(mode,count,type,indices,draw_count);
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
    prepare_scene_capture("glMultiDrawElementsBaseVertex");
    real(mode,count,type,indices,draw_count,base_vertex);
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
    prepare_scene_capture("glDrawArraysIndirect");
    real(mode,indirect);
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
    prepare_scene_capture("glDrawElementsIndirect");
    real(mode,type,indirect);
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
    prepare_scene_capture("glMultiDrawArraysIndirect");
    real(mode,indirect,draw_count,stride);
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
    prepare_scene_capture("glMultiDrawElementsIndirect");
    real(mode,type,indirect,draw_count,stride);
  }
}

__declspec(dllexport) BOOL WINAPI wglSwapBuffers(HDC hdc) {
  PFNWGLSWAPBUFFERS real=(PFNWGLSWAPBUFFERS)old_proc("wglSwapBuffers");
  BOOL ok=real ? real(hdc) : FALSE;
  g_scene_captured=0;
  return ok;
}

__declspec(dllexport) BOOL WINAPI wglSwapLayerBuffers(HDC hdc, UINT planes) {
  PFNWGLSWAPLAYERBUFFERS real=(PFNWGLSWAPLAYERBUFFERS)old_proc("wglSwapLayerBuffers");
  BOOL ok=real ? real(hdc,planes) : FALSE;
  g_scene_captured=0;
  return ok;
}

__declspec(dllexport) PROC WINAPI wglGetProcAddress(LPCSTR name) {
  if(name && (!lstrcmpA(name,"glShaderSource") || !lstrcmpA(name,"glShaderSourceARB"))) {
    return (PROC)hook_glShaderSource;
  }
  if(name && !lstrcmpA(name,"glAttachShader")) {
    return (PROC)hook_glAttachShader;
  }
  if(name && !lstrcmpA(name,"glUseProgram")) {
    return (PROC)hook_glUseProgram;
  }
  if(name && !lstrcmpA(name,"glDrawElements")) {
    return (PROC)glDrawElements;
  }
  if(name && !lstrcmpA(name,"glDrawArrays")) {
    return (PROC)glDrawArrays;
  }
  if(name && !lstrcmpA(name,"glDrawRangeElements")) {
    return (PROC)hook_glDrawRangeElements;
  }
  if(name && !lstrcmpA(name,"glDrawElementsBaseVertex")) {
    return (PROC)hook_glDrawElementsBaseVertex;
  }
  if(name && !lstrcmpA(name,"glDrawRangeElementsBaseVertex")) {
    return (PROC)hook_glDrawRangeElementsBaseVertex;
  }
  if(name && (!lstrcmpA(name,"glDrawArraysInstanced") || !lstrcmpA(name,"glDrawArraysInstancedARB"))) {
    return (PROC)hook_glDrawArraysInstanced;
  }
  if(name && (!lstrcmpA(name,"glDrawElementsInstanced") || !lstrcmpA(name,"glDrawElementsInstancedARB"))) {
    return (PROC)hook_glDrawElementsInstanced;
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseVertex")) {
    return (PROC)hook_glDrawElementsInstancedBaseVertex;
  }
  if(name && !lstrcmpA(name,"glDrawArraysInstancedBaseInstance")) {
    return (PROC)hook_glDrawArraysInstancedBaseInstance;
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseInstance")) {
    return (PROC)hook_glDrawElementsInstancedBaseInstance;
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseVertexBaseInstance")) {
    return (PROC)hook_glDrawElementsInstancedBaseVertexBaseInstance;
  }
  if(name && (!lstrcmpA(name,"glMultiDrawArrays") || !lstrcmpA(name,"glMultiDrawArraysEXT"))) {
    return (PROC)hook_glMultiDrawArrays;
  }
  if(name && (!lstrcmpA(name,"glMultiDrawElements") || !lstrcmpA(name,"glMultiDrawElementsEXT"))) {
    return (PROC)hook_glMultiDrawElements;
  }
  if(name && !lstrcmpA(name,"glMultiDrawElementsBaseVertex")) {
    return (PROC)hook_glMultiDrawElementsBaseVertex;
  }
  if(name && !lstrcmpA(name,"glDrawArraysIndirect")) {
    return (PROC)hook_glDrawArraysIndirect;
  }
  if(name && !lstrcmpA(name,"glDrawElementsIndirect")) {
    return (PROC)hook_glDrawElementsIndirect;
  }
  if(name && !lstrcmpA(name,"glMultiDrawArraysIndirect")) {
    return (PROC)hook_glMultiDrawArraysIndirect;
  }
  if(name && !lstrcmpA(name,"glMultiDrawElementsIndirect")) {
    return (PROC)hook_glMultiDrawElementsIndirect;
  }
  PFNWGLGETPROCADDRESS real=(PFNWGLGETPROCADDRESS)old_proc("wglGetProcAddress");
  return real ? real(name) : 0;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved) {
  (void)reserved;
  if(reason==DLL_PROCESS_ATTACH) {
    g_self=(HMODULE)inst;
    DisableThreadLibraryCalls(inst);
    set_dir();
    log_line("tr456 water proxy loaded");
  }
  return TRUE;
}
