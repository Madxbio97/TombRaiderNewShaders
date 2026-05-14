#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef char GLchar;
typedef unsigned char GLboolean;

#define HOOK_PROC(fn) ((PROC)(void *)(fn))
#define TR456_WATER_MAX_MESH_SUBDIVISION 5
#define TR456_WATER_DEFAULT_MESH_SUBDIVISION 0

static HMODULE g_self;
static HMODULE g_old_gl;
static char g_dir[MAX_PATH];
static char g_mod_dir[MAX_PATH];
static HANDLE g_log_handle=INVALID_HANDLE_VALUE;
static SRWLOCK g_log_lock=SRWLOCK_INIT;

static const char k_surface_key[]="vec3 tc = vWorldPos.xyz / 1024.0 * uParams.x;";
static const char k_reflect_key[]="float hC = texture(sNoise, vec3(uv, t)).x;";
static const char k_ssr_key[]="float not_water = 1 - texture(sTex0, vec3(uv_refract.xy, 0)).w;";

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_VIEWPORT 0x0BA2
#define GL_RGBA 0x1908
#define GL_RGBA8 0x8058
#define GL_LINEAR 0x2601
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_BLEND 0x0BE2
#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_FUNC 0x0B74
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE 1
#define GL_LEQUAL 0x0203
#define GL_FALSE 0
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_UNSIGNED_BYTE 0x1401

enum {
  SHADER_WATER_SURFACE=1,
  SHADER_WATER_REFLECT=2,
  SHADER_WATER_SSR=3,
  SHADER_WATER_FLOW=4,
  SHADER_WATER_RIPPLE=5
};

typedef struct {
  GLuint shader;
  int type;
  uint32_t hash;
  unsigned int len;
  int dumped;
  int compile_logged;
  char preview[128];
} ShaderTrack;

typedef struct {
  GLuint program;
  int type;
  GLuint shaders[8];
  uint32_t shader_hashes[8];
  int shader_types[8];
  int shader_count;
  GLint scene_loc;
  GLint info_loc;
  GLint toggle_loc[3];
  GLint contacts_loc[16];
  GLint contact_motion_loc[16];
  GLint model_matrix_loc[4];
  GLint view_matrix_loc[4];
  GLint proj_matrix_loc;
  GLint ripple_info_loc;
  GLint draw_info_loc;
  GLint material_profile_loc;
  unsigned int uniform_frame;
  int uniform_w;
  int uniform_h;
  unsigned int contact_log_frame;
  unsigned int last_frame;
  unsigned int draw_count;
  unsigned int frame_draw_frame;
  unsigned int frame_draw_count;
  GLenum frame_last_mode;
  int frame_last_count;
  int current_duplicate_pass;
  GLenum last_mode;
  int last_count;
  GLuint geometry_shader;
  int geometry_attached;
} ProgramTrack;

static ShaderTrack g_shader_tracks[512];
static ProgramTrack g_program_tracks[512];
static GLuint g_current_program;
static int g_current_program_type;
static GLuint g_scene_tex;
static GLuint g_scene_fbo;
static int g_scene_w;
static int g_scene_h;
static int g_scene_view_w;
static int g_scene_view_h;
static int g_scene_scale=1;
static int g_scene_has_pixels;
static int g_logged_capture;
static int g_logged_capture_scale_fallback;
static int g_scene_captured;
static int g_logged_use_ssr;
static unsigned int g_frame_index=1;
static int g_runtime_config_loaded;
static int g_runtime_shader_patching;
static int g_runtime_game_shader_replacement=1;
static int g_runtime_fbo_reflection=1;
static int g_runtime_fbo_capture_interval=1;
static int g_runtime_fbo_warmup_frames;
static int g_runtime_fbo_scale=1;
static int g_diag_insert_down;
static int g_diag_session;
static int g_diag_active_frames;
static int g_diag_lines_left;
static int g_diag_dump_unknown_shaders;
static int g_diag_log_unknown_shaders;
static int g_runtime_debug_mode;
static int g_runtime_patch_ripple;
static int g_runtime_ripple_min_count;
static int g_runtime_ripple_center_mode;
static int g_runtime_contact_mesh_subdivision;
static int g_runtime_contact_diagnostic_log;
static int g_runtime_water_grid_overlay;
static int g_runtime_water_grid_flow_overlay;
static int g_runtime_synthetic_surface;
static int g_runtime_synthetic_standing_only;
static int g_runtime_synthetic_flow_surface;
static int g_runtime_synthetic_flow_only;
static GLfloat g_runtime_synthetic_opacity;
static GLfloat g_runtime_synthetic_tint;
static GLfloat g_runtime_synthetic_reflection;
static int g_runtime_synthetic_compile_delay_frames;
static unsigned int g_effect_toggle_mask=0x0BFFu;
static unsigned int g_effect_hotkey_down_mask;
static unsigned int g_water_grid_overlay_draw_logged;
static unsigned int g_water_grid_overlay_draw_logged_by_type[3];
static unsigned int g_water_grid_overlay_skip_logged;
static unsigned int g_synthetic_surface_logged;
static unsigned int g_synthetic_compile_delay_logged;
static unsigned int g_flow_material_bypass_logged;
static unsigned int g_flow_surface_texture_logged;
static unsigned int g_flow_surface_gate_logged;
static unsigned int g_flow_surface_confirmed_logged;
static unsigned int g_surface_cascade_bypass_logged;
static unsigned int g_water_draw_logged_by_type[6];
static int g_shader_defines_logged;
static GLfloat g_contact_cache[16][4];
static GLfloat g_contact_motion_cache[16][4];
static unsigned int g_contact_cache_frame;
static int g_contact_cache_valid;

static void diag_log_cpu_contact_state(const char *where);

typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat radius;
  GLfloat vx;
  GLfloat vy;
  GLfloat vz;
  GLfloat speed;
  unsigned int first_frame;
  unsigned int last_frame;
} RippleContact;

static RippleContact g_ripple_contacts[16];
static unsigned int g_ripple_contact_cursor;
static unsigned int g_ripple_contact_log_frame;
static unsigned int g_ripple_contact_diag_lines;
static unsigned int g_diag_cpu_contact_log_frame;
static GLuint g_surface_geometry_shader;
static GLuint g_flow_geometry_shader;
static int g_surface_geometry_compiled;
static int g_flow_geometry_compiled;

typedef struct {
  GLuint texture;
  GLint layer;
  const char *name;
} FlowTextureLayer;

static GLuint g_flow_surface_texture_objects[64];
static FlowTextureLayer g_flow_surface_texture_layers[128];

typedef struct {
  GLuint program;
  int tried;
  int ready;
  int failed;
  GLint attr_coord;
  GLint attr_normal;
  GLint attr_light;
  GLint attr_color;
  GLint loc_proj;
  GLint loc_model;
  GLint loc_view;
  GLint loc_contacts;
  GLint loc_params;
  GLint loc_capture_info;
  GLint loc_toggle2;
  GLint loc_grid_info;
} WaterGridOverlay;

static WaterGridOverlay g_water_grid_overlays[2];

typedef struct {
  GLuint program;
  int tried;
  int ready;
  int failed;
  GLint attr_coord;
  GLint attr_normal;
  GLint attr_light;
  GLint attr_color;
  GLint loc_proj;
  GLint loc_model;
  GLint loc_view;
  GLint loc_scene;
  GLint loc_capture_info;
  GLint loc_synthetic_info;
  GLint loc_synthetic_mode;
  GLint loc_synthetic_profile;
  GLint loc_params;
  GLint loc_draw_info;
  GLint loc_toggle0;
  GLint loc_toggle1;
  GLint loc_toggle2;
  GLint loc_contacts;
  GLint loc_contact_motion;
} SyntheticSurfacePass;

typedef struct {
  GLint old_program;
  GLint old_blend;
  GLint old_depth;
  GLint old_cull;
  GLint old_depth_mask;
  GLint old_depth_func;
  GLint old_blend_func[4];
} SyntheticSurfaceDrawState;

static SyntheticSurfacePass g_synthetic_surface;
static volatile LONG g_shader_preload_started;
static SRWLOCK g_shader_defines_lock=SRWLOCK_INIT;
static int g_shader_defines_ready;
static char g_shader_defines_cache[12288];

typedef struct {
  const char *file;
  const char *label;
  char *text;
  int loaded;
} ShaderTextCache;

static SRWLOCK g_shader_text_lock=SRWLOCK_INIT;
static ShaderTextCache g_shader_text_cache[] = {
  { "tr456_water_surface.glsl", "surface shader", 0, 0 },
  { "tr456_water_surface_vertex.glsl", "surface vertex shader", 0, 0 },
  { "tr456_water_reflect.glsl", "reflect shader", 0, 0 },
  { "tr456_water_reflect_vertex.glsl", "reflect vertex shader", 0, 0 },
  { "tr456_water_ssr.glsl", "screen-space water shader", 0, 0 },
  { "tr456_water_flow.glsl", "flow water shader", 0, 0 },
  { "tr456_water_flow_vertex.glsl", "flow water vertex shader", 0, 0 },
  { "tr456_water_surface_geometry.glsl", "surface geometry shader", 0, 0 },
  { "tr456_water_flow_geometry.glsl", "flow water geometry shader", 0, 0 },
  { "tr456_water_grid_vertex.glsl", "water grid vertex shader", 0, 0 },
  { "tr456_water_grid_geometry.glsl", "water grid geometry shader", 0, 0 },
  { "tr456_water_grid.glsl", "water grid fragment shader", 0, 0 },
  { "tr456_water_synthetic_vertex.glsl", "synthetic water vertex shader", 0, 0 },
  { "tr456_water_synthetic.glsl", "synthetic water fragment shader", 0, 0 },
  { "tr456_water_ripple.glsl", "ripple sprite shader", 0, 0 }
};

typedef void (APIENTRY *PFNGLSHADERSOURCE)(GLuint, GLsizei, const GLchar * const *, const GLint *);
typedef GLuint (APIENTRY *PFNGLCREATESHADER)(GLenum);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAM)(void);
typedef void (APIENTRY *PFNGLCOMPILESHADER)(GLuint);
typedef void (APIENTRY *PFNGLLINKPROGRAM)(GLuint);
typedef PROC (WINAPI *PFNWGLGETPROCADDRESS)(LPCSTR);
typedef void (APIENTRY *PFNGLATTACHSHADER)(GLuint, GLuint);
typedef void (APIENTRY *PFNGLDELETEPROGRAM)(GLuint);
typedef void (APIENTRY *PFNGLDELETESHADER)(GLuint);
typedef void (APIENTRY *PFNGLGETSHADERIV)(GLuint, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOG)(GLuint, GLsizei, GLsizei *, GLchar *);
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
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXIMAGE2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXSUBIMAGE2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXIMAGE3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXSUBIMAGE3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTURESUBIMAGE2D)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTURESUBIMAGE3D)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT)(GLuint, GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT)(GLuint, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT)(GLuint, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *);
typedef void (APIENTRY *PFNGLGETINTEGERV)(GLenum, GLint *);
typedef void (APIENTRY *PFNGLGENTEXTURES)(GLsizei, GLuint *);
typedef void (APIENTRY *PFNGLBINDTEXTURE)(GLenum, GLuint);
typedef void (APIENTRY *PFNGLTEXPARAMETERI)(GLenum, GLenum, GLint);
typedef void (APIENTRY *PFNGLTEXIMAGE2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLCOPYTEXSUBIMAGE2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLGENFRAMEBUFFERS)(GLsizei, GLuint *);
typedef void (APIENTRY *PFNGLBINDFRAMEBUFFER)(GLenum, GLuint);
typedef void (APIENTRY *PFNGLFRAMEBUFFERTEXTURE2D)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef GLenum (APIENTRY *PFNGLCHECKFRAMEBUFFERSTATUS)(GLenum);
typedef void (APIENTRY *PFNGLBLITFRAMEBUFFER)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
typedef void (APIENTRY *PFNGLACTIVETEXTURE)(GLenum);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATION)(GLuint, const GLchar *);
typedef void (APIENTRY *PFNGLUNIFORM1I)(GLint, GLint);
typedef void (APIENTRY *PFNGLUNIFORM4F)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY *PFNGLUNIFORM4FV)(GLint, GLsizei, const GLfloat *);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FV)(GLint, GLsizei, GLboolean, const GLfloat *);
typedef void (APIENTRY *PFNGLGETUNIFORMFV)(GLuint, GLint, GLfloat *);
typedef void (APIENTRY *PFNGLGETUNIFORMIV)(GLuint, GLint, GLint *);
typedef void (APIENTRY *PFNGLGETPROGRAMIV)(GLuint, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOG)(GLuint, GLsizei, GLsizei *, GLchar *);
typedef GLenum (APIENTRY *PFNGLGETERROR)(void);
typedef void (APIENTRY *PFNGLBINDATTRIBLOCATION)(GLuint, GLuint, const GLchar *);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATION)(GLuint, const GLchar *);
typedef GLboolean (APIENTRY *PFNGLISENABLED)(GLenum);
typedef void (APIENTRY *PFNGLENABLE)(GLenum);
typedef void (APIENTRY *PFNGLDISABLE)(GLenum);
typedef void (APIENTRY *PFNGLDEPTHMASK)(GLboolean);
typedef void (APIENTRY *PFNGLDEPTHFUNC)(GLenum);
typedef void (APIENTRY *PFNGLBLENDFUNCSEPARATE)(GLenum, GLenum, GLenum, GLenum);
typedef BOOL (WINAPI *PFNWGLSWAPBUFFERS)(HDC);
typedef BOOL (WINAPI *PFNWGLSWAPLAYERBUFFERS)(HDC, UINT);

typedef struct {
  int tried;
  int ok;
  PFNGLGETINTEGERV get_integer;
  PFNGLGENTEXTURES gen_textures;
  PFNGLBINDTEXTURE bind_texture;
  PFNGLTEXPARAMETERI tex_parameter_i;
  PFNGLTEXIMAGE2D tex_image_2d;
  PFNGLCOPYTEXSUBIMAGE2D copy_tex_sub_image_2d;
  PFNGLGENFRAMEBUFFERS gen_framebuffers;
  PFNGLBINDFRAMEBUFFER bind_framebuffer;
  PFNGLFRAMEBUFFERTEXTURE2D framebuffer_texture_2d;
  PFNGLCHECKFRAMEBUFFERSTATUS check_framebuffer_status;
  PFNGLBLITFRAMEBUFFER blit_framebuffer;
  PFNGLACTIVETEXTURE active_texture;
  PFNGLGETUNIFORMLOCATION get_uniform_location;
  PFNGLUNIFORM1I uniform_1i;
  PFNGLUNIFORM4F uniform_4f;
  PFNGLUNIFORM4FV uniform_4fv;
  PFNGLGETUNIFORMFV get_uniform_fv;
  PFNGLGETUNIFORMIV get_uniform_iv;
  PFNGLGETERROR get_error;
} CaptureGL;

static CaptureGL g_capture_gl;
static CaptureGL *capture_gl(void);

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

static HANDLE open_log_handle(void) {
  char path[MAX_PATH];
  HANDLE h=INVALID_HANDLE_VALUE;
  if(join_mod_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE && join_game_path(path,"tr456_water_proxy.log"))
    h=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  return h;
}

static void log_line(const char *line) {
  if(!g_dir[0] || !line) return;
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

static int diagnostics_dir(char *out) {
  char base[MAX_PATH];
  if(!join_mod_path(base,"diagnostics")) return 0;
  CreateDirectoryA(base,0);
  lstrcpynA(out,base,MAX_PATH);
  return 1;
}

static void write_diagnostic_text(const char *name, const char *text) {
  char dir[MAX_PATH];
  char path[MAX_PATH];
  if(!text || !diagnostics_dir(dir)) return;
  if(snprintf(path,MAX_PATH,"%s\\%s",dir,name)<0) return;
  HANDLE h=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return;
  DWORD w=0;
  WriteFile(h,text,(DWORD)strlen(text),&w,0);
  CloseHandle(h);
}

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

static void load_runtime_config(void) {
  if(g_runtime_config_loaded) return;
  g_runtime_shader_patching=ini_int("WaterShaderPatching",0);
  g_runtime_game_shader_replacement=ini_int("GameShaderReplacement",1);
  g_runtime_debug_mode=ini_int("DebugMode",0);
  g_runtime_fbo_reflection=ini_int("FramebufferReflection",0);
  g_runtime_fbo_capture_interval=ini_int("FramebufferCaptureInterval",1);
  if(g_runtime_fbo_capture_interval<1) g_runtime_fbo_capture_interval=1;
  if(g_runtime_fbo_capture_interval>8) g_runtime_fbo_capture_interval=8;
  g_runtime_fbo_warmup_frames=ini_int("FramebufferWarmupFrames",0);
  if(g_runtime_fbo_warmup_frames<0) g_runtime_fbo_warmup_frames=0;
  if(g_runtime_fbo_warmup_frames>600) g_runtime_fbo_warmup_frames=600;
  g_runtime_fbo_scale=ini_int("FramebufferScale",1);
  if(g_runtime_fbo_scale<1) g_runtime_fbo_scale=1;
  if(g_runtime_fbo_scale>4) g_runtime_fbo_scale=4;
  g_runtime_patch_ripple=ini_int("PatchRipplePass",0);
  g_effect_toggle_mask=(unsigned int)ini_int("EffectToggleMask",0)&0x0FFFu;
  g_runtime_ripple_min_count=ini_int("RippleSpriteMinCount",192);
  g_runtime_ripple_center_mode=ini_int("RippleSpriteCenterMode",1);
  g_runtime_contact_diagnostic_log=ini_int("ContactDiagnosticLog",0);
  if(g_runtime_ripple_center_mode<0) g_runtime_ripple_center_mode=0;
  if(g_runtime_ripple_center_mode>1) g_runtime_ripple_center_mode=1;
  g_runtime_contact_mesh_subdivision=ini_int("ContactMeshSubdivision",TR456_WATER_DEFAULT_MESH_SUBDIVISION);
  if(g_runtime_contact_mesh_subdivision<0) g_runtime_contact_mesh_subdivision=0;
  if(g_runtime_contact_mesh_subdivision>TR456_WATER_MAX_MESH_SUBDIVISION) g_runtime_contact_mesh_subdivision=TR456_WATER_MAX_MESH_SUBDIVISION;
  g_runtime_water_grid_overlay=ini_int("WaterGridOverlay",0);
  g_runtime_water_grid_flow_overlay=ini_int("WaterGridFlowOverlay",0);
  g_runtime_synthetic_surface=ini_int("SyntheticWaterSurface",0);
  g_runtime_synthetic_standing_only=ini_int("SyntheticStandingWaterOnly",0);
  g_runtime_synthetic_flow_surface=ini_int("SyntheticFlowSurface",0);
  g_runtime_synthetic_flow_only=ini_int("SyntheticFlowOnly",1);
  g_runtime_synthetic_opacity=ini_float("SyntheticSurfaceOpacity",0.58f);
  if(g_runtime_synthetic_opacity<0.0f) g_runtime_synthetic_opacity=0.0f;
  if(g_runtime_synthetic_opacity>1.0f) g_runtime_synthetic_opacity=1.0f;
  g_runtime_synthetic_tint=ini_float("SyntheticSurfaceTint",1.0f);
  if(g_runtime_synthetic_tint<0.0f) g_runtime_synthetic_tint=0.0f;
  if(g_runtime_synthetic_tint>2.0f) g_runtime_synthetic_tint=2.0f;
  g_runtime_synthetic_reflection=ini_float("SyntheticSurfaceReflection",0.34f);
  if(g_runtime_synthetic_reflection<0.0f) g_runtime_synthetic_reflection=0.0f;
  if(g_runtime_synthetic_reflection>2.0f) g_runtime_synthetic_reflection=2.0f;
  g_runtime_synthetic_compile_delay_frames=ini_int("SyntheticCompileDelayFrames",0);
  if(g_runtime_synthetic_compile_delay_frames<0) g_runtime_synthetic_compile_delay_frames=0;
  if(g_runtime_synthetic_compile_delay_frames>600) g_runtime_synthetic_compile_delay_frames=600;
  g_diag_dump_unknown_shaders=ini_int("DiagnosticDumpShaders",0);
  g_diag_log_unknown_shaders=ini_int("DiagnosticLogShaders",0);
  g_runtime_config_loaded=1;
}

static void build_shader_defines(char *out, size_t out_size) {
  const int debug=ini_int("DebugMode",0);
  const int reflection_quality=ini_int("ReflectionQuality",1);
  const float surface_wave=ini_float("SurfaceWave",1.0f);
  const float surface_vertex=ini_float("SurfaceVertexStrength",0.0f);
  const float surface_vertex_wave=ini_float("SurfaceVertexWaveStrength",0.0f);
  const float pixel_wave=ini_float("PixelWaveStrength",0.85f);
  const float refract_wave=ini_float("RefractionWaveStrength",0.90f);
  const float deep_caustics=ini_float("DeepCausticsStrength",0.35f);
  const float water_volume=ini_float("WaterVolumeStrength",0.45f);
  const float shoreline=ini_float("ShorelineStrength",0.35f);
  const float game_ripple=ini_float("GameRippleStrength",0.70f);
  const float ripple_sprite_visual=ini_float("RippleSpriteVisual",0.0f);
  const float refract=ini_float("RefractStrength",0.75f);
  const float reflect=ini_float("ReflectStrength",0.85f);
  const float ssr=ini_float("SSRStrength",0.50f);
  const float glint=ini_float("GlintStrength",0.22f);
  const float foam=ini_float("FoamStrength",0.20f);
  const float chroma=ini_float("ChromaStrength",0.10f);
  const float tint=ini_float("TintStrength",0.30f);
  const float opacity=ini_float("Opacity",0.48f);
  const float force_reflection=ini_float("ForceReflection",0.35f);
  const float scene_reflection=ini_float("SceneReflectionStrength",0.65f);
  const float caustics=ini_float("CausticsStrength",0.25f);
  const float depth=ini_float("DepthStrength",0.45f);
  const float ripple=ini_float("RippleStrength",0.65f);
  const float ripple_x=ini_float("RippleCenterX",0.50f);
  const float ripple_y=ini_float("RippleCenterY",0.62f);
  const float surface_relief=ini_float("SurfaceRelief",0.85f);
  const float safe_volume=ini_float("SafeVolumeStrength",0.25f);
  const float tile_seam_softening=ini_float("TileSeamSoftening",1.0f);
  const float tile_seam_width=ini_float("TileSeamWidth",0.035f);
  const float wake_strength=ini_float("WakeStrength",1.0f);
  const float wake_width=ini_float("WakeWidth",0.42f);
  const float wake_length=ini_float("WakeLength",0.58f);
  const float contact_wave=ini_float("ContactWaveStrength",1.05f);
  const float contact_radius=ini_float("ContactWaveRadius",0.82f);
  const float contact_speed=ini_float("ContactWaveSpeed",0.86f);
  const float contact_vertex=ini_float("ContactVertexStrength",0.42f);
  const float contact_normal=ini_float("ContactNormalStrength",0.95f);
  const int contact_coord=ini_int("ContactCoordMode",1);
  int contact_mesh=ini_int("ContactMeshSubdivision",TR456_WATER_DEFAULT_MESH_SUBDIVISION);
  if(contact_mesh<0) contact_mesh=0;
  if(contact_mesh>TR456_WATER_MAX_MESH_SUBDIVISION) contact_mesh=TR456_WATER_MAX_MESH_SUBDIVISION;
  const float contact_mesh_strength=ini_float("ContactMeshStrength",0.78f);
  const float water_polygonal_strength=ini_float("WaterPolygonalStrength",0.0f);
  const float water_polygonal_scale=ini_float("WaterPolygonalScale",620.0f);
  const float water_polygonal_normal=ini_float("WaterPolygonalNormal",0.0f);
  const float water_polygonal_flow=ini_float("WaterPolygonalFlow",0.0f);
  const float water_physics_mesh=ini_float("WaterPhysicsMesh",0.0f);
  const float water_physics_strength=ini_float("WaterPhysicsStrength",0.72f);
  const float water_physics_scale=ini_float("WaterPhysicsScale",560.0f);
  const float water_physics_contact=ini_float("WaterPhysicsContact",0.95f);
  const float water_physics_rain=ini_float("WaterPhysicsRain",0.55f);
  const float water_physics_chop=ini_float("WaterPhysicsChop",0.34f);
  const float water_physics_normal=ini_float("WaterPhysicsNormal",0.90f);
  int water_grid_subdivision=ini_int("WaterGridSubdivision",8);
  if(water_grid_subdivision<1) water_grid_subdivision=1;
  if(water_grid_subdivision>8) water_grid_subdivision=8;
  const float water_grid_strength=ini_float("WaterGridStrength",0.92f);
  const float water_grid_opacity=ini_float("WaterGridOpacity",0.38f);
  const float water_grid_flow_opacity=ini_float("WaterGridFlowOpacity",0.42f);
  const float water_sim_strength=ini_float("WaterSimStrength",1.0f);
  const float water_sim_scale=ini_float("WaterSimScale",1.0f);
  const float water_sim_speed=ini_float("WaterSimSpeed",1.10f);
  const float water_sim_gerstner=ini_float("WaterSimGerstnerStrength",0.88f);
  const float water_sim_shallow=ini_float("WaterSimShallowStrength",0.68f);
  const float water_sim_contact=ini_float("WaterSimContactStrength",1.18f);
  const float water_sim_rain=ini_float("WaterSimRainStrength",0.78f);
  const float water_sim_flow=ini_float("WaterSimFlowStrength",0.96f);
  const float water_sim_damping=ini_float("WaterSimDamping",0.985f);
  const float water_sim_grid_step=ini_float("WaterSimGridStep",260.0f);
  const float calm_mirror=ini_float("CalmMirrorStrength",0.72f);
  const float rain_ripple=ini_float("RainRippleStrength",1.12f);
  const float wet_edge=ini_float("WetEdgeStrength",0.84f);
  const float micro_ripple=ini_float("MicroRippleStrength",0.48f);
  const float micro_scale=ini_float("MicroRippleScale",0.86f);
  const float mirror_roughness=ini_float("MirrorRoughness",1.30f);
  const float swell_strength=ini_float("SwellStrength",1.08f);
  const float swell_scale=ini_float("SwellScale",0.82f);
  const float wake_wave=ini_float("WakeWaveStrength",1.0f);
  const float edge_wave=ini_float("EdgeWaveStrength",0.75f);
  const float edge_width=ini_float("EdgeWaveWidth",0.09f);
  const float reflection_contrast=ini_float("ReflectionContrast",1.32f);
  const float rough_reflection=ini_float("RoughReflection",1.24f);
  const float fresnel_strength=ini_float("FresnelStrength",1.18f);
  const float bottom_caustics=ini_float("BottomCaustics",0.96f);
  const float contact_edge=ini_float("ContactEdge",0.72f);
  const float depth_absorption=ini_float("DepthAbsorption",1.08f);
  const float wall_stretch=ini_float("WallReflectionStretch",0.84f);
  const float water_saturation=ini_float("WaterSaturation",1.16f);
  const float water_brightness=ini_float("WaterBrightness",0.92f);
  const float water_texture=ini_float("WaterTextureStrength",1.0f);
  const float water_detail=ini_float("WaterDetailStrength",0.0f);
  const float water_detail_scale=ini_float("WaterDetailScale",1.0f);
  const float flow_detail=ini_float("FlowDetailStrength",0.0f);
  const float flow_detail_scale=ini_float("FlowDetailScale",1.0f);
  const float bump_mapping=ini_float("BumpMappingStrength",0.0f);
  const float bump_scale=ini_float("BumpMappingScale",1.0f);
  const float flow_bump=ini_float("FlowBumpMappingStrength",0.0f);
  const float synthetic_bump=ini_float("SyntheticBumpMappingStrength",0.0f);
  const float flow_strength=ini_float("FlowWaterStrength",0.85f);
  const float flow_reflection=ini_float("FlowReflectionStrength",0.45f);
  const float flow_opacity=ini_float("FlowOpacity",0.38f);
  const float flow_chroma=ini_float("FlowChromaStrength",0.10f);
  const float flow_caustics=ini_float("FlowCausticsStrength",0.20f);
  const float flow_standing_blend=ini_float("FlowStandingBlend",0.0f);
  const float flow_vertex=ini_float("FlowVertexStrength",0.0f);
  const float flow_wave=ini_float("FlowWaveStrength",0.85f);
  const float flow_speed=ini_float("FlowSpeed",1.0f);
  const float flow_streak_foam=ini_float("FlowStreakFoam",0.0f);
  const float flow_lane=ini_float("FlowLaneStrength",0.25f);
  const float flow_swirl=ini_float("FlowSwirlStrength",0.20f);
  const float flow_single_layer=ini_float("FlowSingleLayer",0.0f);
  const float flow_secondary_motion=ini_float("FlowSecondaryMotion",0.0f);
  const float flow_secondary_opacity=ini_float("FlowSecondaryOpacity",0.0f);
  const float flow_secondary_reflection=ini_float("FlowSecondaryReflection",0.18f);
  const float flow_aeration=ini_float("FlowAerationStrength",0.0f);
  const float flow_glint=ini_float("FlowGlintStrength",0.35f);
  const float flow_refraction_warp=ini_float("FlowRefractionWarp",0.70f);
  const float flow_surface_distortion=ini_float("FlowSurfaceDistortion",0.75f);
  const float flow_surface_tension=ini_float("FlowSurfaceTension",0.0f);
  const float flow_cross_distortion=ini_float("FlowCrossDistortion",0.60f);
  const float flow_direction_sign=ini_float("FlowDirectionSign",1.0f);
  const float flow_original_deformation=ini_float("FlowOriginalDeformation",0.85f);
  const float flow_body=ini_float("FlowBodyStrength",0.12f);
  const float flow_ridge=ini_float("FlowRidgeStrength",0.20f);
  const float flow_edge_foam=ini_float("FlowEdgeFoamStrength",0.0f);
  const float flow_ribbon=ini_float("FlowRibbonStrength",0.25f);
  const float flow_eddy_foam=ini_float("FlowEddyFoamStrength",0.0f);
  const float flow_depth_body=ini_float("FlowDepthBodyStrength",0.12f);
  const float flow_specular_streak=ini_float("FlowSpecularStreakStrength",0.30f);
  const float flow_cross_wave=ini_float("FlowCrossWaveStrength",0.25f);
  const float flow_breakup=ini_float("FlowBreakupStrength",0.20f);
  const int fbo_reflection=ini_int("FramebufferReflection",1);
  snprintf(out,out_size,
    "#define TR456_WATER_DEBUG_MODE %d\n"
    "#define TR456_WATER_REFLECTION_QUALITY %d\n"
    "#define TR456_WATER_SURFACE_WAVE %.6f\n"
    "#define TR456_WATER_SURFACE_VERTEX_STRENGTH %.6f\n"
    "#define TR456_WATER_SURFACE_VERTEX_WAVE %.6f\n"
    "#define TR456_WATER_PIXEL_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_REFRACTION_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_DEEP_CAUSTICS_STRENGTH %.6f\n"
    "#define TR456_WATER_VOLUME_STRENGTH %.6f\n"
    "#define TR456_WATER_SHORELINE_STRENGTH %.6f\n"
    "#define TR456_WATER_GAME_RIPPLE_STRENGTH %.6f\n"
    "#define TR456_WATER_RIPPLE_SPRITE_VISUAL %.6f\n"
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
    "#define TR456_WATER_SURFACE_RELIEF %.6f\n"
    "#define TR456_WATER_SAFE_VOLUME %.6f\n"
    "#define TR456_WATER_TILE_SEAM_SOFTENING %.6f\n"
    "#define TR456_WATER_TILE_SEAM_WIDTH %.6f\n"
    "#define TR456_WATER_WAKE_STRENGTH %.6f\n"
    "#define TR456_WATER_WAKE_WIDTH %.6f\n"
    "#define TR456_WATER_WAKE_LENGTH %.6f\n"
    "#define TR456_WATER_CONTACT_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_CONTACT_WAVE_RADIUS %.6f\n"
    "#define TR456_WATER_CONTACT_WAVE_SPEED %.6f\n"
    "#define TR456_WATER_CONTACT_VERTEX_STRENGTH %.6f\n"
    "#define TR456_WATER_CONTACT_NORMAL_STRENGTH %.6f\n"
    "#define TR456_WATER_CONTACT_COORD_MODE %d\n"
    "#define TR456_WATER_MESH_SUBDIVISION %d\n"
    "#define TR456_WATER_CONTACT_MESH_STRENGTH %.6f\n"
    "#define TR456_WATER_POLYGONAL_STRENGTH %.6f\n"
    "#define TR456_WATER_POLYGONAL_SCALE %.6f\n"
    "#define TR456_WATER_POLYGONAL_NORMAL %.6f\n"
    "#define TR456_WATER_POLYGONAL_FLOW %.6f\n"
    "#define TR456_WATER_PHYSICS_MESH %.6f\n"
    "#define TR456_WATER_PHYSICS_STRENGTH %.6f\n"
    "#define TR456_WATER_PHYSICS_SCALE %.6f\n"
    "#define TR456_WATER_PHYSICS_CONTACT %.6f\n"
    "#define TR456_WATER_PHYSICS_RAIN %.6f\n"
    "#define TR456_WATER_PHYSICS_CHOP %.6f\n"
    "#define TR456_WATER_PHYSICS_NORMAL %.6f\n"
    "#define TR456_WATER_GRID_SUBDIVISION %d\n"
    "#define TR456_WATER_GRID_STRENGTH %.6f\n"
    "#define TR456_WATER_GRID_OPACITY %.6f\n"
    "#define TR456_WATER_GRID_FLOW_OPACITY %.6f\n"
    "#define TR456_WATER_SIM_STRENGTH %.6f\n"
    "#define TR456_WATER_SIM_SCALE %.6f\n"
    "#define TR456_WATER_SIM_SPEED %.6f\n"
    "#define TR456_WATER_SIM_GERSTNER %.6f\n"
    "#define TR456_WATER_SIM_SHALLOW %.6f\n"
    "#define TR456_WATER_SIM_CONTACT %.6f\n"
    "#define TR456_WATER_SIM_RAIN %.6f\n"
    "#define TR456_WATER_SIM_FLOW %.6f\n"
    "#define TR456_WATER_SIM_DAMPING %.6f\n"
    "#define TR456_WATER_SIM_GRID_STEP %.6f\n"
    "#define TR456_WATER_CALM_MIRROR %.6f\n"
    "#define TR456_WATER_RAIN_RIPPLE %.6f\n"
    "#define TR456_WATER_WET_EDGE %.6f\n"
    "#define TR456_WATER_MICRO_RIPPLE %.6f\n"
    "#define TR456_WATER_MICRO_SCALE %.6f\n"
    "#define TR456_WATER_MIRROR_ROUGHNESS %.6f\n"
    "#define TR456_WATER_SWELL_STRENGTH %.6f\n"
    "#define TR456_WATER_SWELL_SCALE %.6f\n"
    "#define TR456_WATER_WAKE_WAVE %.6f\n"
    "#define TR456_WATER_EDGE_WAVE %.6f\n"
    "#define TR456_WATER_EDGE_WIDTH %.6f\n"
    "#define TR456_WATER_REFLECTION_CONTRAST %.6f\n"
    "#define TR456_WATER_ROUGH_REFLECTION %.6f\n"
    "#define TR456_WATER_FRESNEL_STRENGTH %.6f\n"
    "#define TR456_WATER_BOTTOM_CAUSTICS %.6f\n"
    "#define TR456_WATER_CONTACT_EDGE %.6f\n"
    "#define TR456_WATER_DEPTH_ABSORPTION %.6f\n"
    "#define TR456_WATER_WALL_STRETCH %.6f\n"
    "#define TR456_WATER_COLOR_SATURATION %.6f\n"
    "#define TR456_WATER_BRIGHTNESS %.6f\n"
    "#define TR456_WATER_TEXTURE_STRENGTH %.6f\n"
    "#define TR456_WATER_DETAIL_STRENGTH %.6f\n"
    "#define TR456_WATER_DETAIL_SCALE %.6f\n"
    "#define TR456_WATER_FLOW_DETAIL %.6f\n"
    "#define TR456_WATER_FLOW_DETAIL_SCALE %.6f\n"
    "#define TR456_WATER_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_BUMP_SCALE %.6f\n"
    "#define TR456_WATER_FLOW_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_SYNTHETIC_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_REFLECTION %.6f\n"
    "#define TR456_WATER_FLOW_OPACITY %.6f\n"
    "#define TR456_WATER_FLOW_CHROMA %.6f\n"
    "#define TR456_WATER_FLOW_CAUSTICS %.6f\n"
    "#define TR456_WATER_FLOW_STANDING_BLEND %.6f\n"
    "#define TR456_WATER_FLOW_VERTEX_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_SPEED %.6f\n"
    "#define TR456_WATER_FLOW_STREAK_FOAM %.6f\n"
    "#define TR456_WATER_FLOW_LANE %.6f\n"
    "#define TR456_WATER_FLOW_SWIRL %.6f\n"
    "#define TR456_WATER_FLOW_SINGLE_LAYER %.6f\n"
    "#define TR456_WATER_FLOW_SECONDARY_MOTION %.6f\n"
    "#define TR456_WATER_FLOW_SECONDARY_OPACITY %.6f\n"
    "#define TR456_WATER_FLOW_SECONDARY_REFLECTION %.6f\n"
    "#define TR456_WATER_FLOW_AERATION %.6f\n"
    "#define TR456_WATER_FLOW_GLINT %.6f\n"
    "#define TR456_WATER_FLOW_REFRACTION_WARP %.6f\n"
    "#define TR456_WATER_FLOW_SURFACE_DISTORTION %.6f\n"
    "#define TR456_WATER_FLOW_SURFACE_TENSION %.6f\n"
    "#define TR456_WATER_FLOW_CROSS_DISTORTION %.6f\n"
    "#define TR456_WATER_FLOW_DIRECTION_SIGN %.6f\n"
    "#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION %.6f\n"
    "#define TR456_WATER_FLOW_BODY %.6f\n"
    "#define TR456_WATER_FLOW_RIDGE %.6f\n"
    "#define TR456_WATER_FLOW_EDGE_FOAM %.6f\n"
    "#define TR456_WATER_FLOW_RIBBON %.6f\n"
    "#define TR456_WATER_FLOW_EDDY_FOAM %.6f\n"
    "#define TR456_WATER_FLOW_DEPTH_BODY %.6f\n"
    "#define TR456_WATER_FLOW_SPECULAR_STREAK %.6f\n"
    "#define TR456_WATER_FLOW_CROSS_WAVE %.6f\n"
    "#define TR456_WATER_FLOW_BREAKUP %.6f\n"
    "#define TR456_WATER_FBO_REFLECTION %d\n",
    debug,reflection_quality,(double)surface_wave,(double)surface_vertex,(double)surface_vertex_wave,
    (double)pixel_wave,(double)refract_wave,(double)deep_caustics,
    (double)water_volume,(double)shoreline,(double)game_ripple,
    (double)ripple_sprite_visual,
    (double)refract,(double)reflect,(double)ssr,
    (double)glint,(double)foam,(double)chroma,(double)tint,(double)opacity,
    (double)force_reflection,(double)scene_reflection,(double)caustics,(double)depth,
    (double)ripple,(double)ripple_x,(double)ripple_y,(double)surface_relief,
    (double)safe_volume,
    (double)tile_seam_softening,(double)tile_seam_width,
    (double)wake_strength,(double)wake_width,(double)wake_length,
    (double)contact_wave,(double)contact_radius,(double)contact_speed,
    (double)contact_vertex,(double)contact_normal,contact_coord,
    contact_mesh,(double)contact_mesh_strength,
    (double)water_polygonal_strength,(double)water_polygonal_scale,
    (double)water_polygonal_normal,(double)water_polygonal_flow,
    (double)water_physics_mesh,(double)water_physics_strength,
    (double)water_physics_scale,(double)water_physics_contact,
    (double)water_physics_rain,(double)water_physics_chop,
    (double)water_physics_normal,
    water_grid_subdivision,(double)water_grid_strength,(double)water_grid_opacity,
    (double)water_grid_flow_opacity,
    (double)water_sim_strength,(double)water_sim_scale,(double)water_sim_speed,
    (double)water_sim_gerstner,(double)water_sim_shallow,
    (double)water_sim_contact,(double)water_sim_rain,(double)water_sim_flow,
    (double)water_sim_damping,(double)water_sim_grid_step,
    (double)calm_mirror,(double)rain_ripple,(double)wet_edge,
    (double)micro_ripple,(double)micro_scale,(double)mirror_roughness,
    (double)swell_strength,(double)swell_scale,(double)wake_wave,
    (double)edge_wave,(double)edge_width,
    (double)reflection_contrast,(double)rough_reflection,(double)fresnel_strength,(double)bottom_caustics,
    (double)contact_edge,(double)depth_absorption,(double)wall_stretch,
    (double)water_saturation,(double)water_brightness,(double)water_texture,
    (double)water_detail,(double)water_detail_scale,
    (double)flow_detail,(double)flow_detail_scale,
    (double)bump_mapping,(double)bump_scale,
    (double)flow_bump,(double)synthetic_bump,
    (double)flow_strength,(double)flow_reflection,(double)flow_opacity,
    (double)flow_chroma,(double)flow_caustics,
    (double)flow_standing_blend,
    (double)flow_vertex,
    (double)flow_wave,
    (double)flow_speed,
    (double)flow_streak_foam,
    (double)flow_lane,
    (double)flow_swirl,
    (double)flow_single_layer,
    (double)flow_secondary_motion,
    (double)flow_secondary_opacity,
    (double)flow_secondary_reflection,
    (double)flow_aeration,
    (double)flow_glint,
    (double)flow_refraction_warp,
    (double)flow_surface_distortion,
    (double)flow_surface_tension,
    (double)flow_cross_distortion,
    (double)flow_direction_sign,
    (double)flow_original_deformation,
    (double)flow_body,
    (double)flow_ridge,
    (double)flow_edge_foam,
    (double)flow_ribbon,
    (double)flow_eddy_foam,
    (double)flow_depth_body,
    (double)flow_specular_streak,
    (double)flow_cross_wave,
    (double)flow_breakup,
    fbo_reflection);
  if(!g_shader_defines_logged) {
    char msg[768];
    snprintf(msg,sizeof(msg),
      "shader defines flow strength=%.3f opacity=%.3f speed=%.3f dir=%.0f chroma=%.3f caustics=%.3f standing=%.3f vertex=%.3f wave=%.3f warp=%.3f surfaceDist=%.3f tension=%.3f crossDist=%.3f originalDeform=%.3f detail=%.3f/%.3f grid=%d/%d gridOpacity=%.3f flowGridOpacity=%.3f fbo=%d toggles=0x%03X",
      (double)flow_strength,(double)flow_opacity,
      (double)flow_speed,(double)flow_direction_sign,(double)flow_chroma,
      (double)flow_caustics,(double)flow_standing_blend,
      (double)flow_vertex,(double)flow_wave,
      (double)flow_refraction_warp,
      (double)flow_surface_distortion,(double)flow_surface_tension,
      (double)flow_cross_distortion,
      (double)flow_original_deformation,
      (double)water_detail,(double)flow_detail,
      g_runtime_water_grid_overlay,g_runtime_water_grid_flow_overlay,
      (double)water_grid_opacity,(double)water_grid_flow_opacity,
      fbo_reflection,g_effect_toggle_mask&0x0FFFu);
    log_line(msg);
    g_shader_defines_logged=1;
  }
}

static void shader_defines(char *out, size_t out_size) {
  if(!out || !out_size) return;
  out[0]=0;

  AcquireSRWLockShared(&g_shader_defines_lock);
  if(g_shader_defines_ready) {
    snprintf(out,out_size,"%s",g_shader_defines_cache);
    ReleaseSRWLockShared(&g_shader_defines_lock);
    return;
  }
  ReleaseSRWLockShared(&g_shader_defines_lock);

  AcquireSRWLockExclusive(&g_shader_defines_lock);
  if(!g_shader_defines_ready) {
    build_shader_defines(g_shader_defines_cache,sizeof(g_shader_defines_cache));
    g_shader_defines_ready=1;
  }
  snprintf(out,out_size,"%s",g_shader_defines_cache);
  ReleaseSRWLockExclusive(&g_shader_defines_lock);
}

static char *inject_defines(const char *src) {
  char defs[12288];
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

static ShaderTextCache *shader_text_cache_entry(const char *file) {
  for(size_t i=0;i<sizeof(g_shader_text_cache)/sizeof(g_shader_text_cache[0]);i++) {
    if(!strcmp(g_shader_text_cache[i].file,file))
      return &g_shader_text_cache[i];
  }
  return 0;
}

static char *configured_shader(const char *file, const char *label) {
  ShaderTextCache *entry=shader_text_cache_entry(file);
  if(!entry) {
    char *text=read_text(file);
    if(!text) return 0;
    char *out=inject_defines(text);
    free(text);
    return out;
  }

  AcquireSRWLockShared(&g_shader_text_lock);
  if(entry->loaded) {
    char *cached=entry->text ? dup_text(entry->text) : 0;
    ReleaseSRWLockShared(&g_shader_text_lock);
    return cached;
  }
  ReleaseSRWLockShared(&g_shader_text_lock);

  AcquireSRWLockExclusive(&g_shader_text_lock);
  if(!entry->loaded) {
    char *text=read_text(file);
    char msg[128];
    snprintf(msg,sizeof(msg),"%s %s",text ? "using external" : "missing external",label);
    log_line(msg);
    if(text) {
      entry->text=inject_defines(text);
      free(text);
    }
    entry->loaded=1;
  }
  char *out=entry->text ? dup_text(entry->text) : 0;
  ReleaseSRWLockExclusive(&g_shader_text_lock);
  return out;
}

static char *surface_shader(void) {
  return configured_shader("tr456_water_surface.glsl","surface shader");
}

static char *surface_vertex_shader(void) {
  return configured_shader("tr456_water_surface_vertex.glsl","surface vertex shader");
}

static char *reflect_shader(void) {
  return configured_shader("tr456_water_reflect.glsl","reflect shader");
}

static char *reflect_vertex_shader(void) {
  return configured_shader("tr456_water_reflect_vertex.glsl","reflect vertex shader");
}

static char *ssr_shader(void) {
  return configured_shader("tr456_water_ssr.glsl","screen-space water shader");
}

static char *flow_shader(void) {
  return configured_shader("tr456_water_flow.glsl","flow water shader");
}

static char *flow_vertex_shader(void) {
  return configured_shader("tr456_water_flow_vertex.glsl","flow water vertex shader");
}

static char *surface_geometry_shader(void) {
  return configured_shader("tr456_water_surface_geometry.glsl","surface geometry shader");
}

static char *flow_geometry_shader(void) {
  return configured_shader("tr456_water_flow_geometry.glsl","flow water geometry shader");
}

static char *water_grid_vertex_shader(void) {
  return configured_shader("tr456_water_grid_vertex.glsl","water grid vertex shader");
}

static char *water_grid_geometry_shader(void) {
  return configured_shader("tr456_water_grid_geometry.glsl","water grid geometry shader");
}

static char *water_grid_fragment_shader(void) {
  return configured_shader("tr456_water_grid.glsl","water grid fragment shader");
}

static char *synthetic_surface_vertex_shader(void) {
  return configured_shader("tr456_water_synthetic_vertex.glsl","synthetic water vertex shader");
}

static char *synthetic_surface_shader(void) {
  return configured_shader("tr456_water_synthetic.glsl","synthetic water fragment shader");
}

static char *ripple_shader(void) {
  return configured_shader("tr456_water_ripple.glsl","ripple sprite shader");
}

static int shader_replacement_enabled(int type);
static int game_shader_replacement_enabled(int type);

static void preload_one_shader(char *(*load)(void)) {
  char *text=load ? load() : 0;
  if(text) free(text);
}

static void preload_shader_sources(int include_heavy) {
  load_runtime_config();
  if(!g_runtime_shader_patching) {
    log_line("water shader patching disabled; original game water shaders will be used");
    return;
  }
  if(!(g_effect_toggle_mask&0x0FFFu)) {
    log_line("water shader patching armed with no effect toggles; original game water shaders will be used");
    return;
  }
  if(game_shader_replacement_enabled(SHADER_WATER_SURFACE)) {
    preload_one_shader(surface_shader);
    preload_one_shader(surface_vertex_shader);
  }
  if(game_shader_replacement_enabled(SHADER_WATER_REFLECT)) {
    preload_one_shader(reflect_shader);
    preload_one_shader(reflect_vertex_shader);
  }
  if(game_shader_replacement_enabled(SHADER_WATER_SSR))
    preload_one_shader(ssr_shader);
  if(game_shader_replacement_enabled(SHADER_WATER_FLOW)) {
    preload_one_shader(flow_shader);
    preload_one_shader(flow_vertex_shader);
  }
  if(include_heavy && g_runtime_contact_mesh_subdivision>0 &&
      game_shader_replacement_enabled(SHADER_WATER_SURFACE)) {
    preload_one_shader(surface_geometry_shader);
  }
  if(include_heavy && g_runtime_contact_mesh_subdivision>0 &&
      game_shader_replacement_enabled(SHADER_WATER_FLOW)) {
    preload_one_shader(flow_geometry_shader);
  }
  if(include_heavy && g_runtime_water_grid_overlay) {
    preload_one_shader(water_grid_vertex_shader);
    preload_one_shader(water_grid_geometry_shader);
    preload_one_shader(water_grid_fragment_shader);
  }
  if(include_heavy && g_runtime_synthetic_surface) {
    preload_one_shader(synthetic_surface_vertex_shader);
    preload_one_shader(synthetic_surface_shader);
  }
  if(g_runtime_patch_ripple)
    preload_one_shader(ripple_shader);
  log_line(include_heavy ? "preloaded water shader sources" :
    "preloaded core water shader sources");
}

static DWORD WINAPI shader_preload_thread(LPVOID arg) {
  uintptr_t packed=(uintptr_t)arg;
  DWORD delay_ms=(DWORD)(packed&0x7FFFFFFFu);
  int include_heavy=(packed&0x80000000u)!=0;
  if(delay_ms) Sleep(delay_ms);
  preload_shader_sources(include_heavy);
  return 0;
}

static void start_shader_preload(void) {
  int mode=ini_int("ShaderPreload",1);
  if(mode<=0) {
    log_line("water shader source preload disabled; using lazy loading");
    return;
  }
  if(InterlockedCompareExchange(&g_shader_preload_started,1,0)!=0) return;
  DWORD delay_ms=mode==1 ? 9000u : 0u;
  uintptr_t packed=(uintptr_t)delay_ms;
  if(mode>=2) packed|=0x80000000u;
  HANDLE h=CreateThread(0,0,shader_preload_thread,(LPVOID)packed,0,0);
  if(h) {
    SetThreadPriority(h,THREAD_PRIORITY_BELOW_NORMAL);
    CloseHandle(h);
  } else {
    preload_shader_sources(mode>=2);
  }
}

static int is_flow_vertex_shader(uint32_t hash) {
  return hash==0x7158F169u;
}

static int is_surface_vertex_shader(uint32_t hash) {
  return hash==0x27E1D0CBu;
}

static int is_reflect_vertex_shader(uint32_t hash) {
  return hash==0x57FF35F3u;
}

static int is_flow_shader(uint32_t hash) {
  return hash==0x71E894DDu;
}

static int is_ripple_shader(uint32_t hash) {
  return g_runtime_patch_ripple && hash==0x48E4F81Au;
}

enum {
  EFFECT_FLOW_FOAM=1u<<0,
  EFFECT_FLOW_CHROMA=1u<<1,
  EFFECT_FLOW_CAUSTICS=1u<<2,
  EFFECT_FLOW_LANES=1u<<3,
  EFFECT_FLOW_WARP=1u<<4,
  EFFECT_FLOW_REFLECTION=1u<<5,
  EFFECT_SURFACE_WARP=1u<<6,
  EFFECT_SURFACE_CAUSTICS=1u<<7,
  EFFECT_SURFACE_FOAM=1u<<8,
  EFFECT_SURFACE_REFLECTION=1u<<9,
  EFFECT_MESH_DISPLACEMENT=1u<<10,
  EFFECT_CONTACT_RIPPLES=1u<<11
};

static int shader_replacement_enabled(int type) {
  load_runtime_config();
  if(!g_runtime_shader_patching) return 0;
  unsigned int mask=g_effect_toggle_mask&0x0FFFu;
  if(!mask) return 0;
  switch(type) {
    case SHADER_WATER_FLOW:
      return (mask&(EFFECT_FLOW_FOAM|EFFECT_FLOW_CHROMA|EFFECT_FLOW_CAUSTICS|
        EFFECT_FLOW_LANES|EFFECT_FLOW_WARP|EFFECT_FLOW_REFLECTION|
        EFFECT_MESH_DISPLACEMENT|EFFECT_CONTACT_RIPPLES))!=0;
    case SHADER_WATER_SURFACE:
      return (mask&(EFFECT_SURFACE_WARP|EFFECT_SURFACE_CAUSTICS|
        EFFECT_SURFACE_FOAM|EFFECT_SURFACE_REFLECTION|
        EFFECT_MESH_DISPLACEMENT|EFFECT_CONTACT_RIPPLES))!=0;
    case SHADER_WATER_REFLECT:
      return (mask&(EFFECT_SURFACE_CAUSTICS|EFFECT_SURFACE_FOAM|
        EFFECT_SURFACE_REFLECTION|EFFECT_CONTACT_RIPPLES))!=0;
    case SHADER_WATER_SSR:
      return (mask&(EFFECT_SURFACE_WARP|EFFECT_SURFACE_CAUSTICS|
        EFFECT_SURFACE_FOAM|EFFECT_SURFACE_REFLECTION|
        EFFECT_CONTACT_RIPPLES))!=0;
    case SHADER_WATER_RIPPLE:
      return g_runtime_patch_ripple && (mask&EFFECT_CONTACT_RIPPLES)!=0;
    default:
      return 0;
  }
}

static int shader_tracking_enabled(int type) {
  load_runtime_config();
  if(!g_runtime_shader_patching) return 0;
  if(type==SHADER_WATER_RIPPLE) return shader_replacement_enabled(type);
  if(g_runtime_synthetic_surface) return 1;
  return shader_replacement_enabled(type);
}

static int game_shader_replacement_enabled(int type) {
  if(!shader_replacement_enabled(type)) return 0;
  if(type==SHADER_WATER_RIPPLE) return 1;
  if(type==SHADER_WATER_FLOW) return 0;
  return g_runtime_game_shader_replacement!=0;
}

typedef char *(*ShaderLoader)(void);
typedef int (*ShaderHashMatch)(uint32_t);

typedef struct {
  const char *label;
  int type;
  const char *contains;
  ShaderHashMatch hash_match;
  ShaderLoader load;
} ShaderSourcePatch;

static const ShaderSourcePatch g_source_patches[] = {
  { "patched surface shader", SHADER_WATER_SURFACE, k_surface_key, 0, surface_shader },
  { "patched reflect vertex shader", SHADER_WATER_REFLECT, 0, is_reflect_vertex_shader, reflect_vertex_shader },
  { "patched reflect shader", SHADER_WATER_REFLECT, k_reflect_key, 0, reflect_shader },
  { "patched screen-space water shader", SHADER_WATER_SSR, k_ssr_key, 0, ssr_shader },
  { "patched flow water vertex shader", SHADER_WATER_FLOW, 0, is_flow_vertex_shader, flow_vertex_shader },
  { "patched flow water shader", SHADER_WATER_FLOW, 0, is_flow_shader, flow_shader },
  { "patched ripple sprite shader", SHADER_WATER_RIPPLE, 0, is_ripple_shader, ripple_shader }
};

static uint32_t fnv1a_update(uint32_t h, const char *text, size_t len) {
  if(!text) return h;
  for(size_t i=0;i<len;i++) {
    h^=(unsigned char)text[i];
    h*=16777619u;
  }
  return h;
}

static uint32_t fnv1a(const char *text) {
  uint32_t h=2166136261u;
  return text ? fnv1a_update(h,text,strlen(text)) : h;
}

static size_t shader_source_part_len(const GLchar *text, GLint len) {
  if(!text) return 0;
  return len>=0 ? (size_t)len : strlen(text);
}

static uint32_t fnv1a_sources(GLsizei count, const GLchar * const *strings,
                              const GLint *lengths, unsigned int *out_len) {
  uint32_t h=2166136261u;
  size_t total=0;
  if(count>0 && strings) {
    for(GLsizei i=0;i<count;i++) {
      size_t n=shader_source_part_len(strings[i],lengths ? lengths[i] : -1);
      h=fnv1a_update(h,strings[i],n);
      total+=n;
    }
  }
  if(out_len)
    *out_len=total>0xFFFFFFFFu ? 0xFFFFFFFFu : (unsigned int)total;
  return h;
}

static int mem_contains_text(const char *hay, size_t hay_len, const char *needle) {
  if(!hay || !needle) return 0;
  const size_t needle_len=strlen(needle);
  if(!needle_len) return 1;
  if(hay_len<needle_len) return 0;
  const size_t last=hay_len-needle_len;
  for(size_t i=0;i<=last;i++) {
    if(hay[i]==needle[0] && !memcmp(hay+i,needle,needle_len))
      return 1;
  }
  return 0;
}

static int shader_sources_contain(GLsizei count, const GLchar * const *strings,
                                  const GLint *lengths, const char *needle) {
  if(count<=0 || !strings || !needle) return 0;
  for(GLsizei i=0;i<count;i++) {
    size_t n=shader_source_part_len(strings[i],lengths ? lengths[i] : -1);
    if(mem_contains_text(strings[i],n,needle)) return 1;
  }
  return 0;
}

static const ShaderSourcePatch *find_source_patch_sources(
    GLsizei count, const GLchar * const *strings, const GLint *lengths,
    uint32_t hash) {
  for(size_t i=0;i<sizeof(g_source_patches)/sizeof(g_source_patches[0]);i++) {
    const ShaderSourcePatch *patch=&g_source_patches[i];
    if(!shader_tracking_enabled(patch->type)) continue;
    if(patch->hash_match && patch->hash_match(hash)) return patch;
    if(patch->contains &&
       shader_sources_contain(count,strings,lengths,patch->contains))
      return patch;
  }
  return 0;
}

static ShaderTrack *shader_track(GLuint shader, int create) {
  if(!shader) return 0;
  for(size_t i=0;i<sizeof(g_shader_tracks)/sizeof(g_shader_tracks[0]);i++) {
    if(g_shader_tracks[i].shader==shader) return &g_shader_tracks[i];
  }
  if(!create) return 0;
  for(size_t i=0;i<sizeof(g_shader_tracks)/sizeof(g_shader_tracks[0]);i++) {
    if(!g_shader_tracks[i].shader) {
      g_shader_tracks[i].shader=shader;
      return &g_shader_tracks[i];
    }
  }
  return 0;
}

static void make_preview(char *out, size_t out_size, const char *text) {
  if(!out_size) return;
  out[0]=0;
  if(!text) return;
  size_t j=0;
  for(size_t i=0;text[i] && j+1<out_size;i++) {
    char c=text[i];
    if(c=='\r' || c=='\n' || c=='\t') c=' ';
    if((unsigned char)c<32) c=' ';
    out[j++]=c;
  }
  out[j]=0;
}

static void set_shader_info(GLuint shader, int type, uint32_t hash, unsigned int len, const char *src) {
  ShaderTrack *s=shader_track(shader,1);
  if(!s) return;
  if(type>s->type) s->type=type;
  s->hash=hash;
  s->len=len;
  s->compile_logged=0;
  make_preview(s->preview,sizeof(s->preview),src);
}

static void dump_unknown_shader(GLuint shader, uint32_t hash, const char *src) {
  if(!g_diag_dump_unknown_shaders || !src) return;
  ShaderTrack *s=shader_track(shader,1);
  if(!s || s->dumped || s->type) return;
  char name[64];
  snprintf(name,sizeof(name),"shader_%08X.glsl",(unsigned int)hash);
  write_diagnostic_text(name,src);
  s->dumped=1;
}

static void set_shader_type(GLuint shader, int type) {
  if(!shader || !type) return;
  ShaderTrack *s=shader_track(shader,1);
  if(s && type>s->type) s->type=type;
}

static int shader_type(GLuint shader) {
  ShaderTrack *s=shader_track(shader,0);
  return s ? s->type : 0;
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
      g_program_tracks[i].draw_info_loc=-2;
      g_program_tracks[i].material_profile_loc=-2;
      for(int j=0;j<3;j++)
        g_program_tracks[i].toggle_loc[j]=-2;
      g_program_tracks[i].proj_matrix_loc=-2;
      g_program_tracks[i].ripple_info_loc=-2;
      for(int j=0;j<16;j++)
        g_program_tracks[i].contacts_loc[j]=-2;
      for(int j=0;j<16;j++)
        g_program_tracks[i].contact_motion_loc[j]=-2;
      for(int j=0;j<4;j++)
        g_program_tracks[i].model_matrix_loc[j]=-2;
      for(int j=0;j<4;j++)
        g_program_tracks[i].view_matrix_loc[j]=-2;
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
    p->draw_info_loc=-2;
    p->material_profile_loc=-2;
    for(int i=0;i<3;i++)
      p->toggle_loc[i]=-2;
    p->proj_matrix_loc=-2;
    p->ripple_info_loc=-2;
    for(int i=0;i<16;i++)
      p->contacts_loc[i]=-2;
    for(int i=0;i<16;i++)
      p->contact_motion_loc[i]=-2;
    for(int i=0;i<4;i++)
      p->model_matrix_loc[i]=-2;
    for(int i=0;i<4;i++)
      p->view_matrix_loc[i]=-2;
    p->uniform_frame=0;
    p->uniform_w=0;
    p->uniform_h=0;
  }
}

static int program_type(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  return p ? p->type : 0;
}

static void attach_program_shader_info(GLuint program, GLuint shader) {
  ProgramTrack *p=program_track(program,1);
  ShaderTrack *s=shader_track(shader,0);
  if(!p) return;
  for(int i=0;i<p->shader_count;i++) {
    if(p->shaders[i]==shader) return;
  }
  if(p->shader_count<8) {
    int i=p->shader_count++;
    p->shaders[i]=shader;
    p->shader_hashes[i]=s ? s->hash : 0;
    p->shader_types[i]=s ? s->type : 0;
  }
  if(s && s->type>p->type)
    set_program_type(program,s->type);
}

static void program_shader_summary(ProgramTrack *p, char *out, size_t out_size) {
  if(!out_size) return;
  out[0]=0;
  if(!p) return;
  size_t pos=0;
  for(int i=0;i<p->shader_count && pos+1<out_size;i++) {
    int n=snprintf(out+pos,out_size-pos,"%s%u:%08X:%s",
      i ? "," : "",p->shaders[i],(unsigned int)p->shader_hashes[i],
      shader_type_name(p->shader_types[i]));
    if(n<0) break;
    if((size_t)n>=out_size-pos) {
      pos=out_size-1;
      break;
    }
    pos+=(size_t)n;
  }
  out[pos]=0;
}

static void diag_log_program_snapshot(void) {
  char msg[512];
  for(size_t i=0;i<sizeof(g_program_tracks)/sizeof(g_program_tracks[0]);i++) {
    ProgramTrack *p=&g_program_tracks[i];
    if(!p->program) continue;
    char shaders[256];
    program_shader_summary(p,shaders,sizeof(shaders));
    snprintf(msg,sizeof(msg),
      "diag program session=%d program=%u type=%s shader_count=%d shaders=[%s] last_frame=%u draws=%u last_mode=0x%X last_count=%d",
      g_diag_session,p->program,shader_type_name(p->type),p->shader_count,shaders,
      p->last_frame,p->draw_count,(unsigned int)p->last_mode,p->last_count);
    log_line(msg);
  }
}

static int diag_is_active(void) {
  return (g_diag_active_frames>0 && g_diag_lines_left>0) ||
    g_runtime_debug_mode==10;
}

static void diag_consume_line(void) {
  if(g_diag_active_frames>0 && g_diag_lines_left>0)
    g_diag_lines_left--;
}

static void diag_begin(const char *where) {
  load_runtime_config();
  g_diag_session++;
  g_diag_active_frames=ini_int("DiagnosticFrames",150);
  if(g_diag_active_frames<15) g_diag_active_frames=15;
  g_diag_lines_left=ini_int("DiagnosticMaxLines",420);
  if(g_diag_lines_left<40) g_diag_lines_left=40;
  g_diag_cpu_contact_log_frame=0;
  char msg[256];
  snprintf(msg,sizeof(msg),
    "diag insert begin session=%d frame=%u where=%s current_program=%u current_type=%s capture_frames=%d max_lines=%d",
    g_diag_session,g_frame_index,where ? where : "unknown",g_current_program,
    shader_type_name(g_current_program_type),g_diag_active_frames,g_diag_lines_left);
  log_line(msg);
  snprintf(msg,sizeof(msg),
    "diag config session=%d patch=%d fbo=%d rippleMin=%d centerMode=%d toggles=0x%03X meshSubdiv=%d contactLog=%d",
    g_diag_session,g_runtime_shader_patching,g_runtime_fbo_reflection,
    g_runtime_ripple_min_count,g_runtime_ripple_center_mode,
    (unsigned int)g_effect_toggle_mask,g_runtime_contact_mesh_subdivision,
    g_runtime_contact_diagnostic_log);
  log_line(msg);
  diag_log_program_snapshot();
  diag_log_cpu_contact_state("diag-begin");
}

static void diag_poll_insert(const char *where) {
  SHORT s=GetAsyncKeyState(VK_INSERT);
  int down=(s&0x8000)!=0;
  if(down && !g_diag_insert_down)
    diag_begin(where);
  g_diag_insert_down=down;
}

static void diag_log_program_use(GLuint program) {
  if(g_diag_active_frames<=0 || g_diag_lines_left<=0) return;
  ProgramTrack *p=program_track(program,0);
  char shaders[256];
  program_shader_summary(p,shaders,sizeof(shaders));
  char msg[512];
  snprintf(msg,sizeof(msg),
    "diag use session=%d frame=%u program=%u type=%s shaders=[%s]",
    g_diag_session,g_frame_index,program,shader_type_name(p ? p->type : 0),shaders);
  log_line(msg);
  g_diag_lines_left--;
}

static const char *effect_toggle_name(int index) {
  switch(index) {
    case 0: return "flow foam/streaks";
    case 1: return "flow chroma";
    case 2: return "flow caustics";
    case 3: return "flow lanes/swirl";
    case 4: return "flow refraction warp";
    case 5: return "flow reflection";
    case 6: return "surface refraction warp";
    case 7: return "surface caustics";
    case 8: return "surface foam/glint";
    case 9: return "surface reflection";
    case 10: return "mesh displacement";
    case 11: return "game/contact ripples";
    default: return "unknown";
  }
}

static float effect_toggle_value(int index) {
  return (g_effect_toggle_mask & (1u<<(unsigned int)index)) ? 1.0f : 0.0f;
}

static int effect_toggle_key_down(int index) {
  switch(index) {
    case 0: return (GetAsyncKeyState('1')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD1)&0x8000);
    case 1: return (GetAsyncKeyState('2')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD2)&0x8000);
    case 2: return (GetAsyncKeyState('3')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD3)&0x8000);
    case 3: return (GetAsyncKeyState('4')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD4)&0x8000);
    case 4: return (GetAsyncKeyState('5')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD5)&0x8000);
    case 5: return (GetAsyncKeyState('6')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD6)&0x8000);
    case 6: return (GetAsyncKeyState('7')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD7)&0x8000);
    case 7: return (GetAsyncKeyState('8')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD8)&0x8000);
    case 8: return (GetAsyncKeyState('9')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD9)&0x8000);
    case 9: return (GetAsyncKeyState('0')&0x8000) ||
      (GetAsyncKeyState(VK_NUMPAD0)&0x8000);
    case 10: return (GetAsyncKeyState(VK_OEM_MINUS)&0x8000) ||
      (GetAsyncKeyState(VK_SUBTRACT)&0x8000);
    case 11: return (GetAsyncKeyState(VK_OEM_PLUS)&0x8000) ||
      (GetAsyncKeyState(VK_ADD)&0x8000);
    default: return 0;
  }
}

static const char *effect_toggle_key_name(int index) {
  switch(index) {
    case 0: return "1";
    case 1: return "2";
    case 2: return "3";
    case 3: return "4";
    case 4: return "5";
    case 5: return "6";
    case 6: return "7";
    case 7: return "8";
    case 8: return "9";
    case 9: return "0";
    case 10: return "-";
    case 11: return "=";
    default: return "?";
  }
}

static void poll_effect_hotkeys(void) {
  int ctrl=(GetAsyncKeyState(VK_CONTROL)&0x8000) ||
    (GetAsyncKeyState(VK_LCONTROL)&0x8000) ||
    (GetAsyncKeyState(VK_RCONTROL)&0x8000);
  int chord_j=(GetAsyncKeyState('J')&0x8000);
  unsigned int down=0;
  if(ctrl && chord_j) {
    for(int i=0;i<12;i++) {
      if(effect_toggle_key_down(i))
        down|=1u<<(unsigned int)i;
    }
  }
  unsigned int pressed=down & ~g_effect_hotkey_down_mask;
  g_effect_hotkey_down_mask=down;
  if(!pressed) return;
  for(int i=0;i<12;i++) {
    unsigned int bit=1u<<(unsigned int)i;
    if(!(pressed&bit)) continue;
    g_effect_toggle_mask^=bit;
    char msg[192];
    snprintf(msg,sizeof(msg),"effect toggle Ctrl+J+%s %s -> %s mask=0x%03X",
      effect_toggle_key_name(i),effect_toggle_name(i),
      (g_effect_toggle_mask&bit) ? "on" : "off",
      g_effect_toggle_mask&0x0FFFu);
    log_line(msg);
  }
}

static void apply_effect_toggles(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !p->type) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  static const char *names[3]={
    "uTrWaterToggle0","uTrWaterToggle1","uTrWaterToggle2"
  };
  for(int i=0;i<3;i++) {
    if(p->toggle_loc[i]==-2)
      p->toggle_loc[i]=gl->get_uniform_location(program,names[i]);
  }
  if(p->toggle_loc[0]>=0)
    gl->uniform_4f(p->toggle_loc[0],
      effect_toggle_value(0),effect_toggle_value(1),
      effect_toggle_value(2),effect_toggle_value(3));
  if(p->toggle_loc[1]>=0)
    gl->uniform_4f(p->toggle_loc[1],
      effect_toggle_value(4),effect_toggle_value(5),
      effect_toggle_value(6),effect_toggle_value(7));
  if(p->toggle_loc[2]>=0)
    gl->uniform_4f(p->toggle_loc[2],
      effect_toggle_value(8),effect_toggle_value(9),
      effect_toggle_value(10),effect_toggle_value(11));
}

static int program_uses_contact_uniforms(int type) {
  return type==SHADER_WATER_SURFACE || type==SHADER_WATER_REFLECT ||
    type==SHADER_WATER_FLOW;
}

static int read_program_contacts(GLuint program, GLfloat values[16][4], float *sum_abs) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !program_uses_contact_uniforms(p->type) || !values || !sum_abs) return 0;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv) return 0;

  int found=0;
  *sum_abs=0.0f;
  for(int i=0;i<16;i++) {
    if(p->contacts_loc[i]==-2) {
      char name[32];
      snprintf(name,sizeof(name),"uContacts[%d]",i);
      p->contacts_loc[i]=gl->get_uniform_location(program,name);
    }
    values[i][0]=values[i][1]=values[i][2]=values[i][3]=0.0f;
    if(p->contacts_loc[i]>=0) {
      gl->get_uniform_fv(program,p->contacts_loc[i],values[i]);
      found=1;
      *sum_abs+=f_abs(values[i][0])+f_abs(values[i][1])+
        f_abs(values[i][2])+f_abs(values[i][3]);
    }
  }
  return found;
}

static int read_program_contact_motions(GLuint program, GLfloat values[16][4], float *sum_abs) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !program_uses_contact_uniforms(p->type) || !values || !sum_abs) return 0;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv) return 0;

  int found=0;
  *sum_abs=0.0f;
  for(int i=0;i<16;i++) {
    if(p->contact_motion_loc[i]==-2) {
      char name[40];
      snprintf(name,sizeof(name),"uContactMotion[%d]",i);
      p->contact_motion_loc[i]=gl->get_uniform_location(program,name);
    }
    values[i][0]=values[i][1]=values[i][2]=values[i][3]=0.0f;
    if(p->contact_motion_loc[i]>=0) {
      gl->get_uniform_fv(program,p->contact_motion_loc[i],values[i]);
      found=1;
      *sum_abs+=f_abs(values[i][0])+f_abs(values[i][1])+
        f_abs(values[i][2])+f_abs(values[i][3]);
    }
  }
  return found;
}

static void update_contact_cache_from_program(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || p->type!=SHADER_WATER_REFLECT) return;
  GLfloat values[16][4];
  float sum_abs=0.0f;
  if(!read_program_contacts(program,values,&sum_abs)) return;
  if(sum_abs<=0.001f) return;
  GLfloat next_motion[16][4];
  memset(next_motion,0,sizeof(next_motion));
  unsigned int dt=(g_contact_cache_valid && g_frame_index>g_contact_cache_frame) ?
    (g_frame_index-g_contact_cache_frame) : 1u;
  float inv_dt=1.0f/(float)dt;
  for(int i=0;i<16;i++) {
    float now=f_abs(values[i][0])+f_abs(values[i][1])+
      f_abs(values[i][2])+f_abs(values[i][3]);
    float prev=f_abs(g_contact_cache[i][0])+f_abs(g_contact_cache[i][1])+
      f_abs(g_contact_cache[i][2])+f_abs(g_contact_cache[i][3]);
    if(!g_contact_cache_valid || now<=0.001f || prev<=0.001f)
      continue;
    float raw_x=(values[i][0]-g_contact_cache[i][0])*inv_dt;
    float raw_y=(values[i][1]-g_contact_cache[i][1])*inv_dt;
    float raw_z=(values[i][2]-g_contact_cache[i][2])*inv_dt;
    const float motion_scale=18.0f;
    float mx=raw_x*motion_scale;
    float my=raw_y*motion_scale;
    float mz=raw_z*motion_scale;
    next_motion[i][0]=g_contact_motion_cache[i][0]*0.58f+mx*0.42f;
    next_motion[i][1]=g_contact_motion_cache[i][1]*0.58f+my*0.42f;
    next_motion[i][2]=g_contact_motion_cache[i][2]*0.58f+mz*0.42f;
    next_motion[i][3]=f_sqrt(next_motion[i][0]*next_motion[i][0]+
      next_motion[i][1]*next_motion[i][1]+
      next_motion[i][2]*next_motion[i][2]);
  }
  memcpy(g_contact_cache,values,sizeof(g_contact_cache));
  memcpy(g_contact_motion_cache,next_motion,sizeof(g_contact_motion_cache));
  g_contact_cache_valid=1;
  g_contact_cache_frame=g_frame_index;
}

static int is_water_ripple_draw_count(GLsizei count) {
  load_runtime_config();
  int threshold=g_runtime_ripple_min_count>0 ? g_runtime_ripple_min_count : 192;
  return count>0 && count<=threshold;
}

static int is_screen_contact_value(float x, float y, float z) {
  return z<0.0f && x>=-0.001f && x<=1.001f &&
    y>=-0.001f && y<=1.001f;
}

static int add_ripple_contact(GLfloat x, GLfloat y, GLfloat z, GLfloat radius) {
  const unsigned int lifetime=160u;
  const unsigned int stale=48u;
  int best=-1;
  float best_d2=1000000000.0f;
  radius=f_min(f_max(radius,12.0f),680.0f);

  for(int i=0;i<16;i++) {
    RippleContact *c=&g_ripple_contacts[i];
    if(!c->first_frame) continue;
    unsigned int age=g_frame_index-c->first_frame;
    unsigned int since=g_frame_index-c->last_frame;
    if(age>lifetime || since>stale) continue;
    int screen_contact=is_screen_contact_value(x,y,z) ||
      is_screen_contact_value(c->x,c->y,c->z);
    float min_radius=f_min(radius,c->radius);
    float dedupe=screen_contact ? f_max(14.0f,min_radius*0.24f) :
      f_max(96.0f,min_radius*0.14f);
    float dedupe2=dedupe*dedupe;
    float dx=screen_contact ? (x-c->x)*1920.0f : x-c->x;
    float dy=screen_contact ? (y-c->y)*1080.0f : y-c->y;
    float dz=screen_contact ? (z-c->z)*0.18f : z-c->z;
    float d2=dx*dx+dy*dy+dz*dz;
    if(d2<dedupe2 && d2<best_d2) {
      best=i;
      best_d2=d2;
    }
  }

  if(best>=0) {
    RippleContact *c=&g_ripple_contacts[best];
    int screen_contact=is_screen_contact_value(x,y,z) ||
      is_screen_contact_value(c->x,c->y,c->z);
    unsigned int dt=(g_frame_index>c->last_frame) ?
      (g_frame_index-c->last_frame) : 1u;
    float inv_dt=1.0f/(float)dt;
    float raw_vx=(x-c->x)*inv_dt;
    float raw_vy=(y-c->y)*inv_dt;
    float raw_vz=(z-c->z)*inv_dt;
    float v_blend=screen_contact ? 0.35f : 0.42f;
    float pos_blend=screen_contact ? 0.32f : 0.22f;
    float z_blend=screen_contact ? 0.26f : 0.20f;
    float radius_blend=screen_contact ? 0.24f : 0.28f;
    c->vx=c->vx*(1.0f-v_blend)+raw_vx*v_blend;
    c->vy=c->vy*(1.0f-v_blend)+raw_vy*v_blend;
    c->vz=c->vz*(1.0f-v_blend)+raw_vz*v_blend;
    c->speed=f_sqrt(c->vx*c->vx+c->vy*c->vy+c->vz*c->vz);
    c->x=c->x*(1.0f-pos_blend)+x*pos_blend;
    c->y=c->y*(1.0f-pos_blend)+y*pos_blend;
    c->z=c->z*(1.0f-z_blend)+(z*z_blend);
    c->radius=c->radius*(1.0f-radius_blend)+radius*radius_blend;
    c->last_frame=g_frame_index;
    return best;
  }

  int slot_index=(int)(g_ripple_contact_cursor++&15u);
  RippleContact *slot=&g_ripple_contacts[slot_index];
  slot->x=x;
  slot->y=y;
  slot->z=z;
  slot->radius=radius;
  slot->vx=0.0f;
  slot->vy=0.0f;
  slot->vz=0.0f;
  slot->speed=0.0f;
  slot->first_frame=g_frame_index;
  slot->last_frame=g_frame_index;
  return slot_index;
}

static int project_view_point(const GLfloat proj[16], const GLfloat p[3],
                              const GLint viewport[4], GLfloat *sx, GLfloat *sy,
                              GLfloat *depth) {
  float x=p[0], y=p[1], z=p[2], w=1.0f;
  float cx=proj[0]*x+proj[4]*y+proj[8]*z+proj[12]*w;
  float cy=proj[1]*x+proj[5]*y+proj[9]*z+proj[13]*w;
  float cz=proj[2]*x+proj[6]*y+proj[10]*z+proj[14]*w;
  float cw=proj[3]*x+proj[7]*y+proj[11]*z+proj[15]*w;
  if(f_abs(cw)<0.00001f) return 0;
  float nx=cx/cw;
  float ny=cy/cw;
  float nz=cz/cw;
  if(nx<-2.0f || nx>2.0f || ny<-2.0f || ny>2.0f) return 0;
  *sx=(nx*0.5f+0.5f);
  *sy=(ny*0.5f+0.5f);
  if(viewport[2]>0 && viewport[3]>0) {
    *sx=(*sx*(float)viewport[2]+(float)viewport[0])/(float)viewport[2];
    *sy=(*sy*(float)viewport[3]+(float)viewport[1])/(float)viewport[3];
  }
  if(depth) *depth=nz;
  return 1;
}

static void transform_model_point_to_view(const GLfloat row[3][4],
                                          const GLfloat view[3][4],
                                          const GLfloat local[3],
                                          GLfloat out[3]) {
  GLfloat p[3];
  p[0]=row[0][0]*local[0]+row[0][1]*local[1]+row[0][2]*local[2]+row[0][3];
  p[1]=row[1][0]*local[0]+row[1][1]*local[1]+row[1][2]*local[2]+row[1][3];
  p[2]=row[2][0]*local[0]+row[2][1]*local[1]+row[2][2]*local[2]+row[2][3];
  out[0]=view[0][0]*p[0]+view[0][1]*p[1]+view[0][2]*p[2];
  out[1]=view[1][0]*p[0]+view[1][1]*p[1]+view[1][2]*p[2];
  out[2]=view[2][0]*p[0]+view[2][1]*p[1]+view[2][2]*p[2];
}

static void transform_model_point_to_world(const GLfloat row[3][4],
                                           const GLfloat view[3][4],
                                           const GLfloat local[3],
                                           GLfloat out[3]) {
  out[0]=row[0][0]*local[0]+row[0][1]*local[1]+row[0][2]*local[2]+
    row[0][3]+view[0][3];
  out[1]=row[1][0]*local[0]+row[1][1]*local[1]+row[1][2]*local[2]+
    row[1][3]+view[1][3];
  out[2]=row[2][0]*local[0]+row[2][1]*local[1]+row[2][2]*local[2]+
    row[2][3]+view[2][3];
}

static float ripple_world_radius_from_model(const GLfloat row[3][4]) {
  float sx=f_sqrt(row[0][0]*row[0][0]+row[1][0]*row[1][0]+
    row[2][0]*row[2][0]);
  float sy=f_sqrt(row[0][1]*row[0][1]+row[1][1]*row[1][1]+
    row[2][1]*row[2][1]);
  return f_min(f_max(f_max(sx,sy)*0.28f,96.0f),680.0f);
}

static int project_ripple_local_point(const GLfloat row[3][4],
                                      const GLfloat view[3][4],
                                      const GLfloat proj[16],
                                      const GLint viewport[4],
                                      float lx, float ly,
                                      GLfloat *sx, GLfloat *sy) {
  GLfloat p[3];
  GLfloat depth=0.0f;
  transform_model_point_to_view(row,view,(GLfloat[3]){lx,ly,0.0f},p);
  return project_view_point(proj,p,viewport,sx,sy,&depth);
}

static int ripple_quad_screen_candidate(const GLfloat row[3][4],
                                        const GLfloat view[3][4],
                                        const GLfloat proj[16],
                                        const GLint viewport[4],
                                        int unit_center,
                                        GLfloat *sx, GLfloat *sy,
                                        GLfloat *radius_px) {
  const float min_x=unit_center ? 0.0f : -0.5f;
  const float min_y=unit_center ? 0.0f : -0.5f;
  const float max_x=unit_center ? 1.0f : 0.5f;
  const float max_y=unit_center ? 1.0f : 0.5f;
  GLfloat px[4];
  GLfloat py[4];
  if(!project_ripple_local_point(row,view,proj,viewport,min_x,min_y,&px[0],&py[0])) return 0;
  if(!project_ripple_local_point(row,view,proj,viewport,max_x,min_y,&px[1],&py[1])) return 0;
  if(!project_ripple_local_point(row,view,proj,viewport,max_x,max_y,&px[2],&py[2])) return 0;
  if(!project_ripple_local_point(row,view,proj,viewport,min_x,max_y,&px[3],&py[3])) return 0;

  float cx=(px[0]+px[1]+px[2]+px[3])*.25f;
  float cy=(py[0]+py[1]+py[2]+py[3])*.25f;
  float width_a=f_sqrt((px[1]-px[0])*(px[1]-px[0])*(float)(viewport[2]*viewport[2])+
                       (py[1]-py[0])*(py[1]-py[0])*(float)(viewport[3]*viewport[3]));
  float width_b=f_sqrt((px[2]-px[3])*(px[2]-px[3])*(float)(viewport[2]*viewport[2])+
                       (py[2]-py[3])*(py[2]-py[3])*(float)(viewport[3]*viewport[3]));
  float height_a=f_sqrt((px[3]-px[0])*(px[3]-px[0])*(float)(viewport[2]*viewport[2])+
                        (py[3]-py[0])*(py[3]-py[0])*(float)(viewport[3]*viewport[3]));
  float height_b=f_sqrt((px[2]-px[1])*(px[2]-px[1])*(float)(viewport[2]*viewport[2])+
                        (py[2]-py[1])*(py[2]-py[1])*(float)(viewport[3]*viewport[3]));
  float extent=f_max((width_a+width_b)*.5f,(height_a+height_b)*.5f);
  if(extent<4.0f || extent>2400.0f) return 0;
  *sx=cx;
  *sy=cy;
  *radius_px=f_min(f_max(extent*.28f,10.0f),540.0f);
  return 1;
}

static int ripple_screen_from_program(ProgramTrack *p, const GLfloat row[3][4],
                                      const GLfloat view[3][4], GLfloat *sx,
                                      GLfloat *sy, GLfloat *radius_px) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv || !gl->get_integer)
    return 0;
  if(p->proj_matrix_loc==-2)
    p->proj_matrix_loc=gl->get_uniform_location(g_current_program,"uProjMatrix");
  if(p->proj_matrix_loc<0) return 0;

  GLfloat proj[16];
  GLint viewport[4]={0,0,0,0};
  gl->get_uniform_fv(g_current_program,p->proj_matrix_loc,proj);
  gl->get_integer(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) return 0;

  if(ripple_quad_screen_candidate(row,view,proj,viewport,
      g_runtime_ripple_center_mode ? 1 : 0,sx,sy,radius_px))
    return 1;
  if(ripple_quad_screen_candidate(row,view,proj,viewport,
      g_runtime_ripple_center_mode ? 0 : 1,sx,sy,radius_px))
    return 1;

  GLfloat center[3];
  transform_model_point_to_view(row,view,(GLfloat[3]){0.0f,0.0f,0.0f},center);
  GLfloat cx=0.0f, cy=0.0f, d=0.0f;
  if(!project_view_point(proj,center,viewport,&cx,&cy,&d)) return 0;
  *sx=cx;
  *sy=cy;
  *radius_px=f_min((float)viewport[2],(float)viewport[3])*.035f;
  return 1;
}

static void record_ripple_contact_from_program(GLsizei count) {
  if(g_current_program_type!=SHADER_WATER_RIPPLE) return;
  if(!is_water_ripple_draw_count(count)) {
    if(diag_is_active() ||
       (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<40u)) {
      char msg[160];
      snprintf(msg,sizeof(msg),
        "ripple contact skip frame=%u count=%d threshold=%d reason=count candidate=0",
        g_frame_index,(int)count,g_runtime_ripple_min_count);
      log_line(msg);
      if(diag_is_active()) diag_consume_line();
      else g_ripple_contact_diag_lines++;
    }
    return;
  }
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p) {
    if(diag_is_active() ||
       (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<40u)) {
      log_line("ripple contact skip reason=no_program_track");
      if(diag_is_active()) diag_consume_line();
      else g_ripple_contact_diag_lines++;
    }
    return;
  }
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv) {
    if(diag_is_active() ||
       (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<40u)) {
      log_line("ripple contact skip reason=missing_gl_uniform_api");
      if(diag_is_active()) diag_consume_line();
      else g_ripple_contact_diag_lines++;
    }
    return;
  }

  GLfloat row[3][4];
  GLfloat view[3][4];
  for(int i=0;i<3;i++) {
    if(p->model_matrix_loc[i]==-2) {
      char name[32];
      snprintf(name,sizeof(name),"uModelMatrix[%d]",i);
      p->model_matrix_loc[i]=gl->get_uniform_location(g_current_program,name);
    }
    if(p->model_matrix_loc[i]<0) {
      if(diag_is_active() ||
         (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<40u)) {
        char msg[144];
        snprintf(msg,sizeof(msg),
          "ripple contact skip frame=%u reason=no_model_matrix row=%d",
          g_frame_index,i);
        log_line(msg);
        if(diag_is_active()) diag_consume_line();
        else g_ripple_contact_diag_lines++;
      }
      return;
    }
    gl->get_uniform_fv(g_current_program,p->model_matrix_loc[i],row[i]);

    if(p->view_matrix_loc[i]==-2) {
      char name[32];
      snprintf(name,sizeof(name),"uViewMatrix[%d]",i);
      p->view_matrix_loc[i]=gl->get_uniform_location(g_current_program,name);
    }
    if(p->view_matrix_loc[i]>=0)
      gl->get_uniform_fv(g_current_program,p->view_matrix_loc[i],view[i]);
    else
      view[i][3]=0.0f;
  }

  GLfloat x=0.0f;
  GLfloat y=0.0f;
  GLfloat radius=0.0f;
  if(!ripple_screen_from_program(p,row,view,&x,&y,&radius)) {
    if(diag_is_active() ||
       (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<40u)) {
      char msg[144];
      snprintf(msg,sizeof(msg),
        "ripple contact skip frame=%u count=%d reason=project_failed",
        g_frame_index,(int)count);
      log_line(msg);
      if(diag_is_active()) diag_consume_line();
      else g_ripple_contact_diag_lines++;
    }
    return;
  }

  const float center=g_runtime_ripple_center_mode ? 0.5f : 0.0f;
  GLfloat world[3];
  transform_model_point_to_world(row,view,(GLfloat[3]){center,center,0.0f},world);
  GLfloat world_radius=ripple_world_radius_from_model(row);
  int slot_index=add_ripple_contact(world[0],world[1],world[2],world_radius);
  if(((g_diag_active_frames>0 || g_runtime_debug_mode==10) ||
      (g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<80u)) &&
      g_ripple_contact_log_frame!=g_frame_index) {
    float sx=f_sqrt(row[0][0]*row[0][0]+row[1][0]*row[1][0]+
      row[2][0]*row[2][0]);
    float sy=f_sqrt(row[0][1]*row[0][1]+row[1][1]*row[1][1]+
      row[2][1]*row[2][1]);
    float sz=f_sqrt(row[0][2]*row[0][2]+row[1][2]*row[1][2]+
      row[2][2]*row[2][2]);
    RippleContact *c=(slot_index>=0 && slot_index<16) ?
      &g_ripple_contacts[slot_index] : 0;
    char msg[512];
    snprintf(msg,sizeof(msg),
      "ripple contact frame=%u slot=%d count=%d threshold=%d center=%.1f world=(%.1f %.1f %.1f) radius=%.1f screen=(%.3f %.3f) radius_px=%.1f motion=(%.2f %.2f %.2f) speed=%.2f scale=(%.1f %.1f %.1f)",
      g_frame_index,slot_index,(int)count,g_runtime_ripple_min_count,
      (double)center,(double)world[0],(double)world[1],
      (double)world[2],(double)world_radius,(double)x,(double)y,
      (double)radius,c ? (double)c->vx : 0.0,c ? (double)c->vy : 0.0,
      c ? (double)c->vz : 0.0,c ? (double)c->speed : 0.0,
      (double)sx,(double)sy,(double)sz);
    log_line(msg);
    if(g_runtime_contact_diagnostic_log && g_ripple_contact_diag_lines<80u)
      g_ripple_contact_diag_lines++;
    g_ripple_contact_log_frame=g_frame_index;
    if(diag_is_active()) diag_consume_line();
  }
}

static int build_contact_values(GLfloat values[16][4]) {
  memset(values,0,sizeof(GLfloat)*16u*4u);
  int slot=0;
  const unsigned int lifetime=160u;
  const unsigned int stale=48u;

  for(int i=0;i<16 && slot<16;i++) {
    RippleContact *c=&g_ripple_contacts[i];
    if(!c->first_frame) continue;
    unsigned int age=g_frame_index-c->first_frame;
    unsigned int since=g_frame_index-c->last_frame;
    if(age>lifetime || since>stale) continue;
    values[slot][0]=c->x;
    values[slot][1]=c->y;
    values[slot][2]=c->z;
    if(is_screen_contact_value(c->x,c->y,c->z)) {
      values[slot][3]=(GLfloat)(age+1u);
    } else {
      unsigned int r=(unsigned int)(f_min(f_max(c->radius,12.0f),680.0f)+0.5f);
      values[slot][3]=(GLfloat)(r*512u+age+1u);
    }
    slot++;
  }

  return slot;
}

static int build_contact_motion_values(GLfloat values[16][4]) {
  memset(values,0,sizeof(GLfloat)*16u*4u);
  int slot=0;
  const unsigned int lifetime=160u;
  const unsigned int stale=48u;

  for(int i=0;i<16 && slot<16;i++) {
    RippleContact *c=&g_ripple_contacts[i];
    if(!c->first_frame) continue;
    unsigned int age=g_frame_index-c->first_frame;
    unsigned int since=g_frame_index-c->last_frame;
    if(age>lifetime || since>stale) continue;
    values[slot][0]=c->vx;
    values[slot][1]=c->vy;
    values[slot][2]=c->vz;
    values[slot][3]=c->speed;
    slot++;
  }

  return slot;
}

static float contact_values_sum_abs(const GLfloat values[16][4]) {
  float sum=0.0f;
  for(int i=0;i<16;i++) {
    sum+=f_abs(values[i][0])+f_abs(values[i][1])+
      f_abs(values[i][2])+f_abs(values[i][3]);
  }
  return sum;
}

static int contact_values_active_count(const GLfloat values[16][4]) {
  int count=0;
  for(int i=0;i<16;i++) {
    float sum=f_abs(values[i][0])+f_abs(values[i][1])+
      f_abs(values[i][2])+f_abs(values[i][3]);
    if(sum>0.001f) count++;
  }
  return count;
}

static int build_native_contact_values(GLfloat values[16][4],
                                       GLfloat motions[16][4]) {
  memset(values,0,sizeof(GLfloat)*16u*4u);
  memset(motions,0,sizeof(GLfloat)*16u*4u);
  if(!g_contact_cache_valid) return 0;
  if(g_frame_index-g_contact_cache_frame>90u) return 0;

  int slot=0;
  for(int i=0;i<16 && slot<16;i++) {
    GLfloat *src=g_contact_cache[i];
    float sum=f_abs(src[0])+f_abs(src[1])+f_abs(src[2])+f_abs(src[3]);
    if(sum<=0.001f) continue;
    values[slot][0]=src[0];
    values[slot][1]=src[1];
    values[slot][2]=src[2];
    float radius=f_min(f_max(f_abs(src[3])*0.025f,96.0f),320.0f);
    float age=(float)((g_frame_index+(unsigned int)i*11u)%118u)+1.0f;
    values[slot][3]=(GLfloat)((unsigned int)(radius+0.5f)*512u+
      (unsigned int)(age+0.5f));
    motions[slot][0]=g_contact_motion_cache[i][0];
    motions[slot][1]=g_contact_motion_cache[i][1];
    motions[slot][2]=g_contact_motion_cache[i][2];
    motions[slot][3]=g_contact_motion_cache[i][3];
    slot++;
  }
  return slot;
}

static int strongest_cpu_contact_motion(GLfloat out[4]) {
  GLfloat cpu_values[16][4];
  GLfloat cpu_motions[16][4];
  int cpu_count=build_contact_values(cpu_values);
  build_contact_motion_values(cpu_motions);
  if(cpu_count<=0) return 0;

  int best=-1;
  float best_speed=0.0f;
  for(int i=0;i<16;i++) {
    float sx=cpu_motions[i][0];
    float sy=cpu_motions[i][1];
    float sz=cpu_motions[i][2];
    float speed=f_sqrt(sx*sx+sy*sy+sz*sz);
    if(speed>best_speed) {
      best=i;
      best_speed=speed;
    }
  }
  if(best<0 || best_speed<=0.035f) return 0;

  out[0]=cpu_motions[best][0];
  out[1]=cpu_motions[best][1];
  out[2]=cpu_motions[best][2];
  out[3]=best_speed;
  return 1;
}

static int reinforce_native_contact_motion(GLfloat motions[16][4],
                                           int native_count) {
  if(native_count<=0) return 0;
  GLfloat drive[4]={0.0f,0.0f,0.0f,0.0f};
  if(!strongest_cpu_contact_motion(drive)) return 0;

  int changed=0;
  float drive_speed=f_sqrt(drive[0]*drive[0]+drive[1]*drive[1]+
    drive[2]*drive[2]);
  for(int i=0;i<native_count && i<16;i++) {
    float sx=motions[i][0];
    float sy=motions[i][1];
    float sz=motions[i][2];
    float native_speed=f_sqrt(sx*sx+sy*sy+sz*sz);
    float blend=native_speed<0.75f ? 0.78f : 0.34f;
    if(drive_speed>native_speed*1.35f) blend=f_max(blend,0.56f);
    motions[i][0]=motions[i][0]*(1.0f-blend)+drive[0]*blend;
    motions[i][1]=motions[i][1]*(1.0f-blend)+drive[1]*blend;
    motions[i][2]=motions[i][2]*(1.0f-blend)+drive[2]*blend;
    motions[i][3]=f_sqrt(motions[i][0]*motions[i][0]+
      motions[i][1]*motions[i][1]+motions[i][2]*motions[i][2]);
    changed=1;
  }
  return changed;
}

static int build_effective_contact_values(GLfloat values[16][4],
                                          GLfloat motions[16][4],
                                          int *source) {
  int native_count=build_native_contact_values(values,motions);
  if(native_count>0) {
    int reinforced=reinforce_native_contact_motion(motions,native_count);
    if(source) *source=reinforced ? 3 : 1;
    return native_count;
  }
  int cpu_count=build_contact_values(values);
  build_contact_motion_values(motions);
  if(source) *source=cpu_count>0 ? 2 : 0;
  return cpu_count;
}

static void diag_log_cpu_contact_state(const char *where) {
  if(!diag_is_active()) return;
  if(g_diag_cpu_contact_log_frame==g_frame_index) return;
  GLfloat values[16][4];
  GLfloat motions[16][4];
  GLfloat effective[16][4];
  GLfloat effective_motions[16][4];
  int count=build_contact_values(values);
  build_contact_motion_values(motions);
  int source=0;
  int effective_count=build_effective_contact_values(effective,
    effective_motions,&source);
  float sum=contact_values_sum_abs(values);
  float motion_sum=contact_values_sum_abs(motions);
  float effective_sum=contact_values_sum_abs(effective);
  float effective_motion_sum=contact_values_sum_abs(effective_motions);
  char msg[768];
  snprintf(msg,sizeof(msg),
    "contact cpu frame=%u where=%s active=%d sum=%.2f motion_sum=%.2f effective_source=%d effective_active=%d effective_sum=%.2f effective_motion_sum=%.2f c0=(%.1f %.1f %.1f %.1f) m0=(%.2f %.2f %.2f %.2f) e0=(%.1f %.1f %.1f %.1f) em0=(%.2f %.2f %.2f %.2f)",
    g_frame_index,where ? where : "unknown",count,(double)sum,(double)motion_sum,
    source,effective_count,(double)effective_sum,(double)effective_motion_sum,
    (double)values[0][0],(double)values[0][1],(double)values[0][2],(double)values[0][3],
    (double)motions[0][0],(double)motions[0][1],(double)motions[0][2],(double)motions[0][3],
    (double)effective[0][0],(double)effective[0][1],
    (double)effective[0][2],(double)effective[0][3],
    (double)effective_motions[0][0],(double)effective_motions[0][1],
    (double)effective_motions[0][2],(double)effective_motions[0][3]);
  log_line(msg);
  diag_consume_line();
  g_diag_cpu_contact_log_frame=g_frame_index;
}

static void apply_contact_cache(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !program_uses_contact_uniforms(p->type)) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location) return;
  GLfloat values[16][4];
  GLfloat motions[16][4];
  build_effective_contact_values(values,motions,0);

  if(p->type!=SHADER_WATER_REFLECT) {
    if(p->contacts_loc[0]==-2)
      p->contacts_loc[0]=gl->get_uniform_location(program,"uContacts[0]");
    if(p->contacts_loc[0]>=0 && gl->uniform_4fv) {
      gl->uniform_4fv(p->contacts_loc[0],16,&values[0][0]);
    } else if(gl->uniform_4f) {
      for(int i=0;i<16;i++) {
        if(p->contacts_loc[i]==-2) {
          char name[32];
          snprintf(name,sizeof(name),"uContacts[%d]",i);
          p->contacts_loc[i]=gl->get_uniform_location(program,name);
        }
        if(p->contacts_loc[i]>=0) {
          const GLfloat *c=values[i];
          gl->uniform_4f(p->contacts_loc[i],c[0],c[1],c[2],c[3]);
        }
      }
    }
  }

  if(p->contact_motion_loc[0]==-2)
    p->contact_motion_loc[0]=gl->get_uniform_location(program,"uContactMotion[0]");
  if(p->contact_motion_loc[0]>=0 && gl->uniform_4fv) {
    gl->uniform_4fv(p->contact_motion_loc[0],16,&motions[0][0]);
  } else if(gl->uniform_4f) {
    for(int i=0;i<16;i++) {
      if(p->contact_motion_loc[i]==-2) {
        char name[40];
        snprintf(name,sizeof(name),"uContactMotion[%d]",i);
        p->contact_motion_loc[i]=gl->get_uniform_location(program,name);
      }
      if(p->contact_motion_loc[i]>=0) {
        const GLfloat *m=motions[i];
        gl->uniform_4f(p->contact_motion_loc[i],m[0],m[1],m[2],m[3]);
      }
    }
  }
}

static void update_draw_info_uniform(GLenum mode, GLsizei count) {
  if(!g_current_program_type) return;
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p || !p->type) return;
  if(p->frame_draw_frame!=g_frame_index) {
    p->frame_draw_frame=g_frame_index;
    p->frame_draw_count=0;
    p->frame_last_mode=0;
    p->frame_last_count=-2147483647;
    p->current_duplicate_pass=0;
  }
  const int duplicate_pass=(g_current_program_type==SHADER_WATER_FLOW &&
    p->frame_draw_count>0u &&
    p->frame_last_mode==mode &&
    p->frame_last_count==(int)count);
  p->current_duplicate_pass=duplicate_pass;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  if(p->draw_info_loc==-2)
    p->draw_info_loc=gl->get_uniform_location(g_current_program,"uTrWaterDrawInfo");
  if(p->draw_info_loc<0) return;
  gl->uniform_4f(p->draw_info_loc,
    (GLfloat)g_frame_index,
    (GLfloat)(p->frame_draw_count+1u),
    (GLfloat)count,
    duplicate_pass ? 1.0f : 0.0f);
}

static void update_ripple_draw_info(GLsizei count) {
  if(g_current_program_type!=SHADER_WATER_RIPPLE) return;
  load_runtime_config();
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  if(p->ripple_info_loc==-2)
    p->ripple_info_loc=gl->get_uniform_location(g_current_program,"uTrWaterRippleInfo");
  if(p->ripple_info_loc<0) return;
  int threshold=g_runtime_ripple_min_count>0 ? g_runtime_ripple_min_count : 192;
  float water=is_water_ripple_draw_count(count) ? 1.0f : 0.0f;
  gl->uniform_4f(p->ripple_info_loc,water,(GLfloat)count,(GLfloat)threshold,0.0f);
}

static void diag_log_contacts(GLuint program, const char *where) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !program_uses_contact_uniforms(p->type)) return;
  if(!diag_is_active()) return;
  if(p->contact_log_frame==g_frame_index) return;

  GLfloat uniform_values[16][4];
  GLfloat uniform_motions[16][4];
  GLfloat cpu_values[16][4];
  GLfloat cpu_motions[16][4];
  memset(uniform_values,0,sizeof(uniform_values));
  memset(uniform_motions,0,sizeof(uniform_motions));
  memset(cpu_values,0,sizeof(cpu_values));
  memset(cpu_motions,0,sizeof(cpu_motions));
  float sum_abs=0.0f;
  float motion_sum_abs=0.0f;
  int found=read_program_contacts(program,uniform_values,&sum_abs);
  int motion_found=read_program_contact_motions(program,uniform_motions,
    &motion_sum_abs);
  int cpu_count=build_contact_values(cpu_values);
  build_contact_motion_values(cpu_motions);
  float cpu_sum=contact_values_sum_abs(cpu_values);
  float cpu_motion_sum=contact_values_sum_abs(cpu_motions);
  int uniform_count=contact_values_active_count(uniform_values);

  char msg[1024];
  snprintf(msg,sizeof(msg),
    "contacts frame=%u where=%s program=%u type=%s found=%d uniform_active=%d uniform_sum=%.2f motion_found=%d motion_sum=%.2f cpu_active=%d cpu_sum=%.2f cpu_motion_sum=%.2f locC0=%d locM0=%d u0=(%.1f %.1f %.1f %.1f) um0=(%.2f %.2f %.2f %.2f) cpu0=(%.1f %.1f %.1f %.1f) cm0=(%.2f %.2f %.2f %.2f) u1=(%.1f %.1f %.1f %.1f) cpu1=(%.1f %.1f %.1f %.1f)",
    g_frame_index,where ? where : "unknown",program,shader_type_name(p->type),
    found,uniform_count,(double)sum_abs,motion_found,(double)motion_sum_abs,
    cpu_count,(double)cpu_sum,(double)cpu_motion_sum,
    p->contacts_loc[0],p->contact_motion_loc[0],
    (double)uniform_values[0][0],(double)uniform_values[0][1],
    (double)uniform_values[0][2],(double)uniform_values[0][3],
    (double)uniform_motions[0][0],(double)uniform_motions[0][1],
    (double)uniform_motions[0][2],(double)uniform_motions[0][3],
    (double)cpu_values[0][0],(double)cpu_values[0][1],
    (double)cpu_values[0][2],(double)cpu_values[0][3],
    (double)cpu_motions[0][0],(double)cpu_motions[0][1],
    (double)cpu_motions[0][2],(double)cpu_motions[0][3],
    (double)uniform_values[1][0],(double)uniform_values[1][1],
    (double)uniform_values[1][2],(double)uniform_values[1][3],
    (double)cpu_values[1][0],(double)cpu_values[1][1],
    (double)cpu_values[1][2],(double)cpu_values[1][3]);
  log_line(msg);
  diag_consume_line();
  p->contact_log_frame=g_frame_index;
}

static int read_uniform_vec4_now(const char *name, GLfloat out[4]) {
  out[0]=0.0f; out[1]=0.0f; out[2]=0.0f; out[3]=0.0f;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv || !g_current_program) return 0;
  GLint loc=gl->get_uniform_location(g_current_program,name);
  if(loc<0) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int read_uniform_vec4_index_now(const char *base, int index, GLfloat out[4]) {
  char name[48];
  snprintf(name,sizeof(name),"%s[%d]",base,index);
  return read_uniform_vec4_now(name,out);
}

typedef enum {
  WATER_PROFILE_UNKNOWN=0,
  WATER_PROFILE_STANDING=1,
  WATER_PROFILE_STANDING_SURFACE=2,
  WATER_PROFILE_FLOW_SURFACE=3
} WaterDrawProfileId;

typedef struct {
  WaterDrawProfileId id;
  GLuint texture_object;
  int texture_checked;
  int texture_match;
  GLint texture_layer;
  int texture_mixed_layers;
  const char *name;
  GLfloat foam_scale;
  GLfloat opacity_scale;
  GLfloat reflection_scale;
} WaterDrawProfile;

static void flow_profile_set_surface(WaterDrawProfile *p, const char *name) {
  if(!p) return;
  p->id=WATER_PROFILE_FLOW_SURFACE;
  p->texture_object=0;
  p->texture_checked=0;
  p->texture_match=0;
  p->texture_layer=-1;
  p->texture_mixed_layers=0;
  p->name=name ? name : "flow surface";
  p->foam_scale=0.82f;
  p->opacity_scale=0.95f;
  p->reflection_scale=0.55f;
}

static void flow_profile_set_original(WaterDrawProfile *p, const char *name) {
  flow_profile_set_surface(p,name);
  if(!p) return;
  p->id=WATER_PROFILE_UNKNOWN;
  p->texture_match=0;
  p->foam_scale=0.0f;
  p->opacity_scale=0.0f;
  p->reflection_scale=0.0f;
}

static int water_draw_mode_supported(GLenum mode) {
  return mode==GL_TRIANGLES || mode==GL_TRIANGLE_STRIP || mode==GL_TRIANGLE_FAN;
}

static GLuint current_flow_texture_object(void) {
  if(g_current_program_type!=SHADER_WATER_FLOW || !g_current_program)
    return 0;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_iv ||
     !gl->get_integer || !gl->active_texture)
    return 0;

  GLint loc=gl->get_uniform_location(g_current_program,"sTex0_wrap");
  if(loc<0)
    loc=gl->get_uniform_location(g_current_program,"sTex0");
  if(loc<0) return 0;

  GLint unit=0;
  gl->get_uniform_iv(g_current_program,loc,&unit);
  if(unit<0 || unit>31) return 0;

  GLint old_active=0;
  gl->get_integer(GL_ACTIVE_TEXTURE,&old_active);
  gl->active_texture((GLenum)(GL_TEXTURE0+unit));
  GLint tex=0;
  gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&tex);
  if(old_active)
    gl->active_texture((GLenum)old_active);
  return tex>0 ? (GLuint)tex : 0;
}

typedef struct {
  GLsizei base_size;
  uint64_t base_hash;
  GLsizei payload_size;
  uint64_t payload_hash;
  const char *name;
} FlowTextureSignature;

static FlowTextureSignature g_flow_texture_signatures[]={
  {262144,0x768608304C3F878BULL,349440,0x26DAD8984687EDFBULL,"1/TEX/994.DDS"},
  {262144,0xE518CFE30F854BE0ULL,349440,0x95E6849C08FE9281ULL,"1/TEX/995.DDS"},
  {262144,0x768608304C3F878BULL,349440,0x26DAD8984687EDFBULL,"2/TEX/2077.DDS"},
  {262144,0x493192825E6F7160ULL,349440,0x8D0D07DD23E1BF91ULL,"2/TEX/2078.DDS"},
  {262144,0x768608304C3F878BULL,349440,0x26DAD8984687EDFBULL,"3/TEX/226.DDS"},
  {262144,0x493192825E6F7160ULL,349440,0x8D0D07DD23E1BF91ULL,"3/TEX/227.DDS"}
};

static int g_flow_texture_signatures_loaded;

static uint64_t fnv1a64_bytes(const void *data, size_t size) {
  const unsigned char *p=(const unsigned char*)data;
  uint64_t h=14695981039346656037ULL;
  for(size_t i=0;i<size;i++) {
    h^=(uint64_t)p[i];
    h*=1099511628211ULL;
  }
  return h;
}

static int dds_payload_offset(const unsigned char *data, DWORD size) {
  if(!data || size<128) return 0;
  if(data[0]!='D' || data[1]!='D' || data[2]!='S' || data[3]!=' ')
    return 0;
  if(size>=148 && data[84]=='D' && data[85]=='X' &&
     data[86]=='1' && data[87]=='0')
    return 148;
  return 128;
}

static void load_flow_texture_signatures_from_files(void) {
  if(g_flow_texture_signatures_loaded) return;
  g_flow_texture_signatures_loaded=1;
  if(!g_dir[0]) return;

  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    FlowTextureSignature *s=&g_flow_texture_signatures[i];
    char path[MAX_PATH];
    if(!join_game_path(path,s->name)) continue;

    HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
      0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) continue;
    DWORD size=GetFileSize(h,0);
    if(size==INVALID_FILE_SIZE || size<148 || size>2*1024*1024) {
      CloseHandle(h);
      continue;
    }
    unsigned char *buf=(unsigned char*)malloc(size);
    if(!buf) {
      CloseHandle(h);
      continue;
    }
    DWORD got=0;
    int ok=ReadFile(h,buf,size,&got,0) && got==size;
    CloseHandle(h);
    if(ok) {
      int offset=dds_payload_offset(buf,size);
      if(offset>0 && (DWORD)offset<size) {
        DWORD payload=size-(DWORD)offset;
        DWORD base=payload<262144u ? payload : 262144u;
        s->base_size=(GLsizei)base;
        s->base_hash=fnv1a64_bytes(buf+offset,base);
        s->payload_size=(GLsizei)payload;
        s->payload_hash=fnv1a64_bytes(buf+offset,payload);
        if(g_flow_surface_texture_logged<16u) {
          char msg[256];
          snprintf(msg,sizeof(msg),
            "flow texture signature loaded path=%s base=%d payload=%d",
            s->name,(int)s->base_size,(int)s->payload_size);
          log_line(msg);
          g_flow_surface_texture_logged++;
        }
      }
    }
    free(buf);
  }
}

static const char *flow_texture_signature_match(GLsizei image_size,
                                                const void *data) {
  load_flow_texture_signatures_from_files();
  if(!data || image_size<=0) return 0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    if(s->base_size>0 && image_size>=s->base_size &&
       fnv1a64_bytes(data,(size_t)s->base_size)==s->base_hash)
      return s->name;
    if(s->payload_size>0 && image_size>=s->payload_size &&
       fnv1a64_bytes(data,(size_t)s->payload_size)==s->payload_hash)
      return s->name;
  }
  return 0;
}

static GLuint current_bound_texture_for_target(GLenum target) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_integer) return 0;
  GLint binding=0;
  if(target==GL_TEXTURE_2D_ARRAY)
    gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&binding);
  else if(target==GL_TEXTURE_2D)
    gl->get_integer(GL_TEXTURE_BINDING_2D,&binding);
  return binding>0 ? (GLuint)binding : 0;
}

static int flow_surface_texture_known(GLuint texture) {
  if(!texture) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_objects)/
      sizeof(g_flow_surface_texture_objects[0]);i++) {
    if(g_flow_surface_texture_objects[i]==texture)
      return 1;
  }
  return 0;
}

static int flow_surface_texture_layer_known(GLuint texture, GLint layer) {
  if(!texture || layer<0) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(l->texture==texture && l->layer==layer)
      return 1;
  }
  return 0;
}

static int flow_surface_texture_has_precise_layers(GLuint texture) {
  if(!texture) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(l->texture==texture && l->layer>=0)
      return 1;
  }
  return 0;
}

static void remember_flow_surface_texture(GLuint texture, const char *name) {
  if(!texture || flow_surface_texture_known(texture)) return;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_objects)/
      sizeof(g_flow_surface_texture_objects[0]);i++) {
    if(!g_flow_surface_texture_objects[i]) {
      g_flow_surface_texture_objects[i]=texture;
      if(g_flow_surface_texture_logged<24u) {
        char msg[192];
        snprintf(msg,sizeof(msg),
          "flow water texture matched tex=%u signature=%s",
          texture,name ? name : "water");
        log_line(msg);
        g_flow_surface_texture_logged++;
      }
      return;
    }
  }
}

static void remember_flow_surface_texture_layer(GLuint texture, GLint layer,
                                                const char *name) {
  if(!texture || layer<0) return;
  remember_flow_surface_texture(texture,name);
  if(flow_surface_texture_layer_known(texture,layer)) return;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(!l->texture) {
      l->texture=texture;
      l->layer=layer;
      l->name=name;
      if(g_flow_surface_texture_logged<48u) {
        char msg[224];
        snprintf(msg,sizeof(msg),
          "flow water texture layer matched tex=%u layer=%d signature=%s",
          texture,(int)layer,name ? name : "water");
        log_line(msg);
        g_flow_surface_texture_logged++;
      }
      return;
    }
  }
}

static const char *flow_texture_signature_match_exact(GLsizei image_size,
                                                      const void *data) {
  load_flow_texture_signatures_from_files();
  if(!data || image_size<=0) return 0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    if(s->base_size==image_size &&
       fnv1a64_bytes(data,(size_t)s->base_size)==s->base_hash)
      return s->name;
    if(s->payload_size==image_size &&
       fnv1a64_bytes(data,(size_t)s->payload_size)==s->payload_hash)
      return s->name;
  }
  return 0;
}

static int flow_texture_signature_chunk_match(const FlowTextureSignature *s,
                                              const unsigned char *data,
                                              GLsizei stride) {
  if(!s || !data || stride<=0) return 0;
  if(stride==s->base_size)
    return fnv1a64_bytes(data,(size_t)s->base_size)==s->base_hash;
  if(stride==s->payload_size) {
    if(fnv1a64_bytes(data,(size_t)s->payload_size)==s->payload_hash)
      return 1;
    return s->base_size>0 && s->base_size<=s->payload_size &&
      fnv1a64_bytes(data,(size_t)s->base_size)==s->base_hash;
  }
  return 0;
}

static void note_compressed_texture_upload_for_texture(GLuint texture,
                                                       GLenum target,
                                                       GLint level,
                                                       GLint zoffset,
                                                       GLsizei depth,
                                                       GLsizei image_size,
                                                       const void *data) {
  load_flow_texture_signatures_from_files();
  if(!texture) return;
  if(target==GL_TEXTURE_2D_ARRAY && level==0 && depth>0 &&
     image_size>0 && data) {
    for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
        sizeof(g_flow_texture_signatures[0]);i++) {
      const FlowTextureSignature *s=&g_flow_texture_signatures[i];
      const unsigned char *p=(const unsigned char*)data;
      GLsizei strides[2]={s->payload_size,s->base_size};
      for(int si=0;si<2;si++) {
        GLsizei stride=strides[si];
        if(stride<=0 || image_size<stride || (image_size%stride)!=0)
          continue;
        int chunks=image_size/stride;
        for(int chunk=0;chunk<chunks;chunk++) {
          const unsigned char *chunk_data=p+(size_t)chunk*(size_t)stride;
          if(!flow_texture_signature_chunk_match(s,chunk_data,stride))
            continue;
          GLint layer=zoffset;
          if(chunks==depth)
            layer=zoffset+chunk;
          remember_flow_surface_texture_layer(texture,layer,s->name);
        }
      }
      if(s->base_size>0 && image_size>s->base_size &&
         fnv1a64_bytes(data,(size_t)s->base_size)==s->base_hash) {
        remember_flow_surface_texture_layer(texture,zoffset,s->name);
      }
    }
    return;
  }

  const char *match=flow_texture_signature_match_exact(image_size,data);
  if(match) {
    if(target==GL_TEXTURE_2D_ARRAY && level==0)
      remember_flow_surface_texture_layer(texture,zoffset,match);
    else
      remember_flow_surface_texture(texture,match);
  } else {
    match=flow_texture_signature_match(image_size,data);
    if(match)
      remember_flow_surface_texture(texture,match);
  }
}

static void note_compressed_texture_upload(GLenum target, GLint level,
                                           GLint zoffset, GLsizei depth,
                                           GLsizei image_size,
                                           const void *data) {
  note_compressed_texture_upload_for_texture(
    current_bound_texture_for_target(target),target,level,zoffset,depth,
    image_size,data);
}

static int current_water_attrib_locations(GLint locs[4]);

static int current_flow_draw_matches_texture_layers(GLuint texture,
                                                    GLsizei count,
                                                    int count_known,
                                                    GLint *layer_out,
                                                    int *mixed_out) {
  if(layer_out) *layer_out=-1;
  if(mixed_out) *mixed_out=0;
  (void)count;
  (void)count_known;
  if(!texture)
    return 0;
  if(mixed_out && flow_surface_texture_has_precise_layers(texture))
    *mixed_out=1;
  return flow_surface_texture_has_precise_layers(texture) ||
    flow_surface_texture_known(texture);
}

static int g_flow_profile_cache_valid;
static unsigned int g_flow_profile_cache_frame;
static unsigned int g_flow_profile_cache_draw_serial;
static GLuint g_flow_profile_cache_program;
static GLenum g_flow_profile_cache_mode;
static GLsizei g_flow_profile_cache_count;
static int g_flow_profile_cache_count_known;
static int g_flow_profile_cache_has_params;
static GLfloat g_flow_profile_cache_params[4];
static WaterDrawProfile g_flow_profile_cache_profile;

static unsigned int current_draw_serial(void) {
  ProgramTrack *p=program_track(g_current_program,0);
  return p ? p->frame_draw_count : 0u;
}

static int current_flow_draw_profile(GLenum mode, GLsizei count,
                                     int count_known,
                                     WaterDrawProfile *profile,
                                     GLfloat params[4]) {
  if(g_current_program_type!=SHADER_WATER_FLOW) return 0;
  const unsigned int serial=current_draw_serial();
  if(!g_flow_profile_cache_valid ||
     g_flow_profile_cache_frame!=g_frame_index ||
     g_flow_profile_cache_draw_serial!=serial ||
     g_flow_profile_cache_program!=g_current_program ||
     g_flow_profile_cache_mode!=mode ||
     g_flow_profile_cache_count!=count ||
     g_flow_profile_cache_count_known!=count_known) {
    GLfloat local_params[4]={0.0f,0.0f,0.0f,0.0f};
    int has_params=read_uniform_vec4_now("uParams",local_params);
    if(has_params) {
      GLuint tex=current_flow_texture_object();
      GLint texture_layer=-1;
      int mixed_layers=0;
      const int texture_match=
        current_flow_draw_matches_texture_layers(tex,count,count_known,
          &texture_layer,&mixed_layers);
      if(texture_match) {
        flow_profile_set_surface(&g_flow_profile_cache_profile,
          "flow surface texture");
      } else {
        flow_profile_set_original(&g_flow_profile_cache_profile,
          "non-flow texture");
      }
      g_flow_profile_cache_profile.texture_object=tex;
      g_flow_profile_cache_profile.texture_checked=1;
      g_flow_profile_cache_profile.texture_match=texture_match;
      g_flow_profile_cache_profile.texture_layer=texture_layer;
      g_flow_profile_cache_profile.texture_mixed_layers=mixed_layers;
      if(texture_match) {
        if(g_flow_surface_confirmed_logged<24u) {
          char msg[320];
          snprintf(msg,sizeof(msg),
            "flow surface texture confirmed frame=%u program=%u count=%d tex=%u layer=%d mixed=%d precise=%d params=(%.6f %.6f %.6f %.6f)",
            g_frame_index,g_current_program,(int)count,tex,
            (int)texture_layer,mixed_layers,texture_match,
            (double)local_params[0],(double)local_params[1],
            (double)local_params[2],(double)local_params[3]);
          log_line(msg);
          g_flow_surface_confirmed_logged++;
        }
      }
      if(!texture_match && g_flow_material_bypass_logged<32u) {
        char msg[320];
        snprintf(msg,sizeof(msg),
          "flow original by texture frame=%u program=%u count=%d tex=%u layer=%d knownTex=%d preciseLayers=%d params=(%.6f %.6f %.6f %.6f)",
          g_frame_index,g_current_program,(int)count,tex,(int)texture_layer,
          flow_surface_texture_known(tex),
          flow_surface_texture_has_precise_layers(tex),
          (double)local_params[0],(double)local_params[1],
          (double)local_params[2],(double)local_params[3]);
        log_line(msg);
        g_flow_material_bypass_logged++;
      }
    } else {
      WaterDrawProfile fallback;
      flow_profile_set_original(&fallback,"unclassified flow");
      g_flow_profile_cache_profile=fallback;
    }
    memcpy(g_flow_profile_cache_params,local_params,
      sizeof(g_flow_profile_cache_params));
    g_flow_profile_cache_has_params=has_params;
    g_flow_profile_cache_frame=g_frame_index;
    g_flow_profile_cache_draw_serial=serial;
    g_flow_profile_cache_program=g_current_program;
    g_flow_profile_cache_mode=mode;
    g_flow_profile_cache_count=count;
    g_flow_profile_cache_count_known=count_known;
    g_flow_profile_cache_valid=1;
  }
  if(profile) *profile=g_flow_profile_cache_profile;
  if(params) memcpy(params,g_flow_profile_cache_params,
    sizeof(g_flow_profile_cache_params));
  return g_flow_profile_cache_has_params;
}

static void update_flow_material_profile_uniform(GLenum mode, GLsizei count,
                                                 int count_known) {
  if(g_current_program_type!=SHADER_WATER_FLOW) return;
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p || !p->type) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  if(p->material_profile_loc==-2)
    p->material_profile_loc=gl->get_uniform_location(g_current_program,
      "uTrWaterMaterialProfile");
  if(p->material_profile_loc<0) return;

  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  WaterDrawProfile profile;
  current_flow_draw_profile(mode,count,count_known,&profile,params);
  gl->uniform_4f(p->material_profile_loc,
    (GLfloat)profile.id,
    profile.texture_match ? 0.0f : 1.0f,
    profile.foam_scale,
    profile.reflection_scale);
}

static int current_flow_draw_is_allowlisted_surface(GLsizei count,
                                                    int count_known,
                                                    const GLfloat params[4],
                                                    const WaterDrawProfile *profile,
                                                    const char **reason_out) {
  if(!profile || !profile->texture_match) {
    if(reason_out) *reason_out=profile && profile->name ?
      profile->name : "flow texture";
    return 0;
  }

  const int enough_surface_geometry=count_known && count>=256 && count<=24000;
  const int calm_flow_vector=params &&
    fabsf(params[0])<0.12f && params[1]<-0.45f && params[1]>-0.95f;
  const int authored_surface_wave=params &&
    fabsf(params[2])<0.00030f && fabsf(params[3])>=40.0f;
  const int flow_surface_shape=enough_surface_geometry &&
    calm_flow_vector && authored_surface_wave;
  const int allowlisted=flow_surface_shape && profile->texture_match;

  if(!allowlisted && reason_out) {
    *reason_out=!enough_surface_geometry ? "draw shape" :
      (!calm_flow_vector ? "flow vector" :
      (!authored_surface_wave ? "flow material" : "unknown"));
  }
  return allowlisted;
}

static int current_surface_model_up_alignment(GLfloat *alignment);

static int current_draw_is_synthetic_flow_candidate(GLenum mode, GLsizei count,
                                                    int count_known) {
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_shader_patching) return 0;
  if(!g_runtime_synthetic_flow_surface) return 0;
  if(g_current_program_type!=SHADER_WATER_FLOW) return 0;
  if(count_known && (count<3 || count>262144)) return 0;
  if(!water_draw_mode_supported(mode)) return 0;
  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  WaterDrawProfile profile;
  int has_params=current_flow_draw_profile(mode,count,count_known,&profile,
    params);
  if(!has_params)
    return 0;
  const char *reason="unknown";
  const int allowlisted=current_flow_draw_is_allowlisted_surface(count,
    count_known,params,&profile,&reason);
  if(!allowlisted && g_flow_surface_gate_logged<48u) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "flow surface gate original frame=%u program=%u count=%d reason=%s params=(%.6f %.6f %.6f %.6f)",
      g_frame_index,g_current_program,(int)count,
      reason,
      (double)params[0],(double)params[1],
      (double)params[2],(double)params[3]);
    log_line(msg);
    g_flow_surface_gate_logged++;
  }
  return allowlisted;
}

static int current_surface_model_up_alignment(GLfloat *alignment) {
  GLfloat m0[4],m1[4],m2[4];
  if(!read_uniform_vec4_index_now("uModelMatrix",0,m0) ||
     !read_uniform_vec4_index_now("uModelMatrix",1,m1) ||
     !read_uniform_vec4_index_now("uModelMatrix",2,m2))
    return 0;
  const GLfloat x=m0[1];
  const GLfloat y=m1[1];
  const GLfloat z=m2[1];
  const GLfloat len=sqrtf(x*x+y*y+z*z);
  if(len<=0.0001f) return 0;
  if(alignment) *alignment=fabsf(y)/len;
  return 1;
}

static int current_surface_draw_prefers_original_cascade(GLenum mode,
                                                         GLsizei count,
                                                         int count_known) {
  if(g_current_program_type!=SHADER_WATER_SURFACE) return 0;
  if(!water_draw_mode_supported(mode)) return 0;

  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  const int has_params=read_uniform_vec4_now("uParams",params);
  GLfloat up_alignment=1.0f;
  const int has_alignment=current_surface_model_up_alignment(&up_alignment);
  const int vertical_model=has_alignment && up_alignment<0.48f;
  const int slanted_model=has_alignment && up_alignment<0.72f;
  const GLfloat fine=fabsf(params[2]);
  const GLfloat amp=fabsf(params[3]);
  const int cascade_params=has_params &&
    params[0]>0.34f && params[0]<0.70f &&
    params[1]>0.42f && params[1]<1.10f &&
    fine>0.025f && fine<0.140f && amp<0.0020f;
  const int compact_sheet=count_known && count>=300 && count<=2400;
  const int prefers_original=vertical_model ||
    (slanted_model && cascade_params) ||
    (compact_sheet && cascade_params && has_alignment && up_alignment<0.86f);

  if(prefers_original && g_surface_cascade_bypass_logged<32u) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "surface original material frame=%u program=%u count=%d reason=%s up=%.3f params=(%.6f %.6f %.6f %.6f)",
      g_frame_index,g_current_program,(int)count,
      vertical_model ? "vertical model" :
      (slanted_model ? "slanted cascade" : "compact cascade"),
      (double)up_alignment,(double)params[0],(double)params[1],
      (double)params[2],(double)params[3]);
    log_line(msg);
    g_surface_cascade_bypass_logged++;
  }
  return prefers_original;
}

static int current_draw_is_synthetic_standing_candidate(GLenum mode, GLsizei count,
                                                        int count_known) {
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_shader_patching) return 0;
  if(count_known && (count<3 || count>262144)) return 0;
  if(mode!=GL_TRIANGLES && mode!=GL_TRIANGLE_STRIP && mode!=GL_TRIANGLE_FAN)
    return 0;
  if(g_current_program_type!=SHADER_WATER_REFLECT &&
     g_current_program_type!=SHADER_WATER_SURFACE)
    return 0;

  GLfloat model3[4];
  int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
  if(g_current_program_type==SHADER_WATER_SURFACE) {
    if(current_surface_draw_prefers_original_cascade(mode,count,count_known))
      return 0;
    return has_model3;
  }
  if(!count_known) return 0;
  if(mode!=GL_TRIANGLES || count>2048 || (count%3)!=0) return 0;
  return has_model3;
}

static int current_draw_is_synthetic_surface_candidate(GLenum mode, GLsizei count,
                                                       int count_known) {
  return current_draw_is_synthetic_standing_candidate(mode,count,count_known) ||
    current_draw_is_synthetic_flow_candidate(mode,count,count_known);
}

static int current_draw_should_skip_original_standing_water(GLenum mode, GLsizei count,
                                                            int count_known) {
  (void)mode;
  (void)count;
  (void)count_known;
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_synthetic_standing_only ||
     !g_runtime_shader_patching) return 0;
  if(!g_synthetic_surface.ready) return 0;
  if(g_current_program_type==SHADER_WATER_SURFACE)
    return current_draw_is_synthetic_standing_candidate(mode,count,count_known);
  if(g_current_program_type==SHADER_WATER_SSR)
    return 1;
  return 0;
}

static int current_draw_should_skip_original_flow_water(GLenum mode, GLsizei count,
                                                        int count_known) {
  (void)mode;
  (void)count;
  (void)count_known;
  return 0;
}

static int current_draw_should_skip_original_water_for_synthetic(GLenum mode,
                                                                GLsizei count,
                                                                int count_known) {
  return current_draw_should_skip_original_standing_water(mode,count,count_known) ||
    current_draw_should_skip_original_flow_water(mode,count,count_known);
}

static void note_draw(const char *name, GLenum mode, GLsizei count) {
  poll_effect_hotkeys();
  diag_poll_insert(name);
  ProgramTrack *p=program_track(g_current_program,1);
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(g_current_program);
  if(g_current_program_type) {
    apply_effect_toggles(g_current_program);
    apply_contact_cache(g_current_program);
    update_draw_info_uniform(mode,count);
    update_flow_material_profile_uniform(mode,count,count>=0);
    if(g_current_program_type>0 && g_current_program_type<6 &&
       g_water_draw_logged_by_type[g_current_program_type]<10u) {
      GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle0[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle1[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle2[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat draw_info[4]={0.0f,0.0f,0.0f,0.0f};
      int has_params=read_uniform_vec4_now("uParams",params);
      int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
      int has_toggle0=read_uniform_vec4_now("uTrWaterToggle0",toggle0);
      int has_toggle1=read_uniform_vec4_now("uTrWaterToggle1",toggle1);
      int has_toggle2=read_uniform_vec4_now("uTrWaterToggle2",toggle2);
      int has_draw_info=read_uniform_vec4_now("uTrWaterDrawInfo",draw_info);
      char msg[640];
      snprintf(msg,sizeof(msg),
        "water draw frame=%u call=%s program=%u type=%s mode=0x%X count=%d frame_draw=%u dup=%d params%s=(%.6f %.6f %.6f %.6f) time%s=%.3f toggles%s/%s/%s=(%.0f%.0f%.0f%.0f %.0f%.0f%.0f%.0f %.0f%.0f%.0f%.0f) drawInfo%s=(%.0f %.0f %.0f %.0f)",
        g_frame_index,name ? name : "draw",g_current_program,
        shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,
        p ? p->frame_draw_count+1u : 0u,p ? p->current_duplicate_pass : 0,
        has_params ? "" : "?",
        (double)params[0],(double)params[1],(double)params[2],(double)params[3],
        has_model3 ? "" : "?",(double)model3[0],
        has_toggle0 ? "" : "?",has_toggle1 ? "" : "?",has_toggle2 ? "" : "?",
        (double)toggle0[0],(double)toggle0[1],(double)toggle0[2],(double)toggle0[3],
        (double)toggle1[0],(double)toggle1[1],(double)toggle1[2],(double)toggle1[3],
        (double)toggle2[0],(double)toggle2[1],(double)toggle2[2],(double)toggle2[3],
        has_draw_info ? "" : "?",
        (double)draw_info[0],(double)draw_info[1],(double)draw_info[2],(double)draw_info[3]);
      log_line(msg);
      g_water_draw_logged_by_type[g_current_program_type]++;
    }
  }
  update_ripple_draw_info(count);
  record_ripple_contact_from_program(count);
  if(g_current_program_type==SHADER_WATER_RIPPLE)
    diag_log_cpu_contact_state(name);
  if(g_current_program_type)
    diag_log_contacts(g_current_program,name);
  if(p) {
    if(p->frame_draw_frame!=g_frame_index) {
      p->frame_draw_frame=g_frame_index;
      p->frame_draw_count=0;
      p->frame_last_mode=0;
      p->frame_last_count=-2147483647;
      p->current_duplicate_pass=0;
    }
    p->last_frame=g_frame_index;
    p->draw_count++;
    p->frame_draw_count++;
    p->frame_last_mode=mode;
    p->frame_last_count=(int)count;
    p->last_mode=mode;
    p->last_count=(int)count;
  }
  if(g_diag_active_frames>0 && g_diag_lines_left>0) {
    char shaders[256];
    program_shader_summary(p,shaders,sizeof(shaders));
    GLint blend=-1;
    GLint depth=-1;
    GLint depth_mask=-1;
    GLint blend_src_rgb=-1;
    GLint blend_dst_rgb=-1;
    GLint blend_src_alpha=-1;
    GLint blend_dst_alpha=-1;
    CaptureGL *gl=capture_gl();
    if(gl && gl->get_integer) {
      gl->get_integer(GL_BLEND,&blend);
      gl->get_integer(GL_DEPTH_TEST,&depth);
      gl->get_integer(GL_DEPTH_WRITEMASK,&depth_mask);
      gl->get_integer(GL_BLEND_SRC_RGB,&blend_src_rgb);
      gl->get_integer(GL_BLEND_DST_RGB,&blend_dst_rgb);
      gl->get_integer(GL_BLEND_SRC_ALPHA,&blend_src_alpha);
      gl->get_integer(GL_BLEND_DST_ALPHA,&blend_dst_alpha);
    }
    GLfloat params[4],model3[4],draw_info[4];
    int has_params=read_uniform_vec4_now("uParams",params);
    int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
    int has_draw_info=read_uniform_vec4_now("uTrWaterDrawInfo",draw_info);
    char msg[1024];
    snprintf(msg,sizeof(msg),
      "diag draw session=%d frame=%u call=%s program=%u type=%s mode=0x%X count=%d blend=%d depth=%d depthMask=%d blendRgb=0x%X/0x%X blendA=0x%X/0x%X params%s=(%.4f %.4f %.4f %.4f) model3%s=(%.1f %.1f %.1f %.1f) drawInfo%s=(%.0f %.0f %.0f %.0f) shaders=[%s]",
      g_diag_session,g_frame_index,name ? name : "draw",g_current_program,
      shader_type_name(p ? p->type : 0),(unsigned int)mode,(int)count,blend,depth,
      depth_mask,(unsigned int)blend_src_rgb,(unsigned int)blend_dst_rgb,
      (unsigned int)blend_src_alpha,(unsigned int)blend_dst_alpha,
      has_params ? "" : "?",(double)params[0],(double)params[1],
      (double)params[2],(double)params[3],
      has_model3 ? "" : "?",(double)model3[0],(double)model3[1],
      (double)model3[2],(double)model3[3],
      has_draw_info ? "" : "?",(double)draw_info[0],(double)draw_info[1],
      (double)draw_info[2],(double)draw_info[3],shaders);
    log_line(msg);
    diag_consume_line();
  }
}

static CaptureGL *capture_gl(void) {
  if(g_capture_gl.tried) return &g_capture_gl;
  g_capture_gl.tried=1;
  g_capture_gl.get_integer=(PFNGLGETINTEGERV)gl_proc("glGetIntegerv");
  g_capture_gl.gen_textures=(PFNGLGENTEXTURES)gl_proc("glGenTextures");
  g_capture_gl.bind_texture=(PFNGLBINDTEXTURE)gl_proc("glBindTexture");
  g_capture_gl.tex_parameter_i=(PFNGLTEXPARAMETERI)gl_proc("glTexParameteri");
  g_capture_gl.tex_image_2d=(PFNGLTEXIMAGE2D)gl_proc("glTexImage2D");
  g_capture_gl.copy_tex_sub_image_2d=(PFNGLCOPYTEXSUBIMAGE2D)gl_proc("glCopyTexSubImage2D");
  g_capture_gl.gen_framebuffers=(PFNGLGENFRAMEBUFFERS)gl_proc("glGenFramebuffers");
  if(!g_capture_gl.gen_framebuffers)
    g_capture_gl.gen_framebuffers=(PFNGLGENFRAMEBUFFERS)gl_proc("glGenFramebuffersEXT");
  g_capture_gl.bind_framebuffer=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebuffer");
  if(!g_capture_gl.bind_framebuffer)
    g_capture_gl.bind_framebuffer=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebufferEXT");
  g_capture_gl.framebuffer_texture_2d=(PFNGLFRAMEBUFFERTEXTURE2D)gl_proc("glFramebufferTexture2D");
  if(!g_capture_gl.framebuffer_texture_2d)
    g_capture_gl.framebuffer_texture_2d=(PFNGLFRAMEBUFFERTEXTURE2D)gl_proc("glFramebufferTexture2DEXT");
  g_capture_gl.check_framebuffer_status=(PFNGLCHECKFRAMEBUFFERSTATUS)gl_proc("glCheckFramebufferStatus");
  if(!g_capture_gl.check_framebuffer_status)
    g_capture_gl.check_framebuffer_status=(PFNGLCHECKFRAMEBUFFERSTATUS)gl_proc("glCheckFramebufferStatusEXT");
  g_capture_gl.blit_framebuffer=(PFNGLBLITFRAMEBUFFER)gl_proc("glBlitFramebuffer");
  if(!g_capture_gl.blit_framebuffer)
    g_capture_gl.blit_framebuffer=(PFNGLBLITFRAMEBUFFER)gl_proc("glBlitFramebufferEXT");
  g_capture_gl.active_texture=(PFNGLACTIVETEXTURE)gl_proc("glActiveTexture");
  g_capture_gl.get_uniform_location=(PFNGLGETUNIFORMLOCATION)gl_proc("glGetUniformLocation");
  g_capture_gl.uniform_1i=(PFNGLUNIFORM1I)gl_proc("glUniform1i");
  g_capture_gl.uniform_4f=(PFNGLUNIFORM4F)gl_proc("glUniform4f");
  g_capture_gl.uniform_4fv=(PFNGLUNIFORM4FV)gl_proc("glUniform4fv");
  g_capture_gl.get_uniform_fv=(PFNGLGETUNIFORMFV)gl_proc("glGetUniformfv");
  g_capture_gl.get_uniform_iv=(PFNGLGETUNIFORMIV)gl_proc("glGetUniformiv");
  g_capture_gl.get_error=(PFNGLGETERROR)gl_proc("glGetError");
  g_capture_gl.ok=g_capture_gl.get_integer && g_capture_gl.gen_textures &&
    g_capture_gl.bind_texture && g_capture_gl.tex_parameter_i &&
    g_capture_gl.tex_image_2d && g_capture_gl.copy_tex_sub_image_2d &&
    g_capture_gl.active_texture && g_capture_gl.get_uniform_location &&
    g_capture_gl.uniform_1i && g_capture_gl.uniform_4f;
  return &g_capture_gl;
}

static int capture_gl_can_downsample(CaptureGL *gl) {
  return gl && gl->gen_framebuffers && gl->bind_framebuffer &&
    gl->framebuffer_texture_2d && gl->check_framebuffer_status &&
    gl->blit_framebuffer;
}

static void advance_frame(void) {
  g_scene_captured=0;
  g_frame_index++;
  if(g_diag_active_frames>0) {
    g_diag_active_frames--;
    if(!g_diag_active_frames) {
      char msg[128];
      snprintf(msg,sizeof(msg),"diag insert end session=%d frame=%u",g_diag_session,g_frame_index);
      log_line(msg);
    }
  }
  if(!g_frame_index) {
    g_frame_index=1;
    for(size_t i=0;i<sizeof(g_program_tracks)/sizeof(g_program_tracks[0]);i++)
      g_program_tracks[i].uniform_frame=0;
  }
}

static void prepare_scene_capture_internal(const char *reason, int allow_unknown_program) {
  if(!allow_unknown_program && !g_current_program_type)
    return;

  load_runtime_config();
  if(!g_runtime_fbo_reflection)
    return;

  CaptureGL *gl=capture_gl();
  if(!gl->ok) {
    if(!g_logged_capture)
      log_line("framebuffer reflection capture skipped: missing required GL entry point");
    return;
  }

  GLint viewport[4]={0,0,0,0};
  gl->get_integer(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) return;
  g_scene_view_w=viewport[2];
  g_scene_view_h=viewport[3];

  int scale=g_runtime_fbo_scale;
  if(scale<1) scale=1;
  if(scale>4) scale=4;
  if(scale>1 && !capture_gl_can_downsample(gl)) {
    if(!g_logged_capture_scale_fallback) {
      log_line("framebuffer reflection downsample disabled: missing framebuffer blit entry point");
      g_logged_capture_scale_fallback=1;
    }
    scale=1;
  }
  int capture_w=viewport[2]/scale;
  int capture_h=viewport[3]/scale;
  if(capture_w<1) capture_w=1;
  if(capture_h<1) capture_h=1;

  GLint old_active=0;
  gl->get_integer(GL_ACTIVE_TEXTURE,&old_active);
  gl->active_texture(GL_TEXTURE15);

  if(!g_scene_tex) {
    gl->gen_textures(1,&g_scene_tex);
    gl->bind_texture(GL_TEXTURE_2D,g_scene_tex);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    {
      const unsigned char black[4]={0,0,0,255};
      gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,black);
    }
    g_scene_w=1;
    g_scene_h=1;
    g_scene_scale=1;
    g_scene_has_pixels=1;
  } else {
    gl->bind_texture(GL_TEXTURE_2D,g_scene_tex);
  }

  int warmup=g_runtime_fbo_warmup_frames>0 &&
    g_frame_index<(unsigned int)g_runtime_fbo_warmup_frames;
  int resized=0;
  if(warmup) {
    g_scene_captured=1;
  } else
  if(g_scene_w!=capture_w || g_scene_h!=capture_h || g_scene_scale!=scale) {
    gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,capture_w,capture_h,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
    g_scene_w=capture_w;
    g_scene_h=capture_h;
    g_scene_scale=scale;
    resized=1;
    g_scene_has_pixels=0;
  }

  GLenum err=0;
  if(!g_scene_captured) {
    int capture_now=resized || !g_scene_has_pixels ||
      g_runtime_fbo_capture_interval<=1 ||
      (g_frame_index%(unsigned int)g_runtime_fbo_capture_interval)==0u;
    if(capture_now) {
      if(scale>1) {
        GLint old_read_fbo=0;
        GLint old_draw_fbo=0;
        if(!g_scene_fbo)
          gl->gen_framebuffers(1,&g_scene_fbo);
        if(g_scene_fbo) {
          gl->get_integer(GL_READ_FRAMEBUFFER_BINDING,&old_read_fbo);
          gl->get_integer(GL_DRAW_FRAMEBUFFER_BINDING,&old_draw_fbo);
          gl->bind_framebuffer(GL_DRAW_FRAMEBUFFER,g_scene_fbo);
          gl->framebuffer_texture_2d(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,g_scene_tex,0);
          GLenum status=gl->check_framebuffer_status(GL_DRAW_FRAMEBUFFER);
          if(status==GL_FRAMEBUFFER_COMPLETE) {
            gl->bind_framebuffer(GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
            gl->blit_framebuffer(viewport[0],viewport[1],
              viewport[0]+viewport[2],viewport[1]+viewport[3],
              0,0,capture_w,capture_h,GL_COLOR_BUFFER_BIT,GL_LINEAR);
            err=gl->get_error ? gl->get_error() : 0;
          } else {
            err=status ? status : 1u;
          }
          gl->bind_framebuffer(GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
          gl->bind_framebuffer(GL_DRAW_FRAMEBUFFER,(GLuint)old_draw_fbo);
        } else {
          err=1u;
        }
        if(err) {
          if(!g_logged_capture_scale_fallback) {
            char fallback_msg[160];
            snprintf(fallback_msg,sizeof(fallback_msg),
              "framebuffer reflection downsample fallback to full resolution glerr=0x%04X",
              (unsigned int)err);
            log_line(fallback_msg);
            g_logged_capture_scale_fallback=1;
          }
          gl->bind_texture(GL_TEXTURE_2D,g_scene_tex);
          gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,viewport[2],viewport[3],0,GL_RGBA,GL_UNSIGNED_BYTE,0);
          g_scene_w=viewport[2];
          g_scene_h=viewport[3];
          g_scene_scale=1;
          g_runtime_fbo_scale=1;
          capture_w=viewport[2];
          capture_h=viewport[3];
          scale=1;
          gl->copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],viewport[2],viewport[3]);
          err=gl->get_error ? gl->get_error() : 0;
        }
      } else {
        gl->copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],viewport[2],viewport[3]);
        err=gl->get_error ? gl->get_error() : 0;
      }
      g_scene_has_pixels=err==0;
    }
    g_scene_captured=1;
    if(capture_now && !g_logged_capture) {
      char msg[288];
      snprintf(msg,sizeof(msg),"framebuffer reflection capture enabled size=%dx%d capture=%dx%d scale=%d tex=%u program=%u type=%d reason=%s glerr=0x%04X",
        viewport[2],viewport[3],capture_w,capture_h,scale,g_scene_tex,g_current_program,g_current_program_type,
        reason ? reason : "unknown",(unsigned int)err);
      log_line(msg);
      g_logged_capture=1;
    }
  }

  ProgramTrack *track=program_track(g_current_program,1);
  if(track) {
    if(track->uniform_frame!=g_frame_index || track->uniform_w!=viewport[2] || track->uniform_h!=viewport[3]) {
      if(track->scene_loc==-2)
        track->scene_loc=gl->get_uniform_location(g_current_program,"uTrWaterScene");
      if(track->scene_loc>=0)
        gl->uniform_1i(track->scene_loc,15);
      if(track->info_loc==-2)
        track->info_loc=gl->get_uniform_location(g_current_program,"uTrWaterCaptureInfo");
      if(track->info_loc>=0)
        gl->uniform_4f(track->info_loc,1.0f/(GLfloat)viewport[2],1.0f/(GLfloat)viewport[3],
          (GLfloat)viewport[2],(GLfloat)viewport[3]);
      track->uniform_frame=g_frame_index;
      track->uniform_w=viewport[2];
      track->uniform_h=viewport[3];
    }
  }

  gl->active_texture((GLenum)old_active);

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
  load_runtime_config();
  unsigned int src_len=0;
  uint32_t src_hash=fnv1a_sources(count,strings,lengths,&src_len);
  char *replacement=0;
  char *src=0;
  const char *tag=0;
  int type=0;
  const ShaderSourcePatch *patch=find_source_patch_sources(count,strings,lengths,src_hash);
  if(patch) {
    tag=patch->label;
    type=patch->type;
    if(game_shader_replacement_enabled(type))
      replacement=patch->load();
  }
  src=join_sources(count,strings,lengths);
  set_shader_info(shader,type,src_hash,src_len,src);
  if(type)
    set_shader_type(shader,type);
  if(replacement) {
    GLint len=(GLint)strlen(replacement);
    const GLchar *one=replacement;
    real(shader,1,&one,&len);
    set_shader_info(shader,type,fnv1a(replacement),(unsigned int)len,replacement);
    char msg[160];
    snprintf(msg,sizeof(msg),"%s shader=%u original_hash=0x%08X replacement_len=%ld",
      tag ? tag : "patched shader",shader,(unsigned int)src_hash,(long)len);
    log_line(msg);
    free(replacement);
  } else {
    if(patch && !g_runtime_game_shader_replacement && type!=SHADER_WATER_RIPPLE) {
      char msg[176];
      snprintf(msg,sizeof(msg),"tracked original %s shader=%u hash=0x%08X len=%u",
        shader_type_name(type),shader,(unsigned int)src_hash,src_len);
      log_line(msg);
    }
    if(g_diag_log_unknown_shaders || g_diag_dump_unknown_shaders) {
      if(src) {
        char msg[320];
        snprintf(msg,sizeof(msg),"shader source shader=%u type=unknown hash=0x%08X len=%u preview=\"%s\"",
          shader,(unsigned int)src_hash,src_len,shader_track(shader,0) ? shader_track(shader,0)->preview : "");
        log_line(msg);
        dump_unknown_shader(shader,src_hash,src);
      }
    }
    real(shader,count,strings,lengths);
  }
  if(src) free(src);
}

static PFNGLCOMPILESHADER real_compile_shader(void) {
  static PFNGLCOMPILESHADER p;
  if(!p) p=(PFNGLCOMPILESHADER)gl_proc("glCompileShader");
  if(!p) p=(PFNGLCOMPILESHADER)gl_proc("glCompileShaderARB");
  return p;
}

static PFNGLLINKPROGRAM real_link_program(void) {
  static PFNGLLINKPROGRAM p;
  if(!p) p=(PFNGLLINKPROGRAM)gl_proc("glLinkProgram");
  if(!p) p=(PFNGLLINKPROGRAM)gl_proc("glLinkProgramARB");
  return p;
}

static void prepare_scene_capture(const char *reason) {
  prepare_scene_capture_internal(reason,0);
}

static void prepare_scene_capture_for_synthetic_surface(const char *reason) {
  prepare_scene_capture_internal(reason,1);
}

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

static PFNGLBINDATTRIBLOCATION real_bind_attrib_location(void) {
  static PFNGLBINDATTRIBLOCATION p;
  if(!p) p=(PFNGLBINDATTRIBLOCATION)gl_proc("glBindAttribLocation");
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

static PFNGLENABLE real_enable(void) {
  static PFNGLENABLE p;
  if(!p) p=(PFNGLENABLE)gl_proc("glEnable");
  return p;
}

static PFNGLDISABLE real_disable(void) {
  static PFNGLDISABLE p;
  if(!p) p=(PFNGLDISABLE)gl_proc("glDisable");
  return p;
}

static PFNGLDEPTHMASK real_depth_mask(void) {
  static PFNGLDEPTHMASK p;
  if(!p) p=(PFNGLDEPTHMASK)gl_proc("glDepthMask");
  return p;
}

static PFNGLDEPTHFUNC real_depth_func(void) {
  static PFNGLDEPTHFUNC p;
  if(!p) p=(PFNGLDEPTHFUNC)gl_proc("glDepthFunc");
  return p;
}

static PFNGLBLENDFUNCSEPARATE real_blend_func_separate(void) {
  static PFNGLBLENDFUNCSEPARATE p;
  if(!p) p=(PFNGLBLENDFUNCSEPARATE)gl_proc("glBlendFuncSeparate");
  if(!p) p=(PFNGLBLENDFUNCSEPARATE)gl_proc("glBlendFuncSeparateEXT");
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

static int contact_mesh_subdivision(void) {
  load_runtime_config();
  return g_runtime_contact_mesh_subdivision;
}

static GLuint compile_water_geometry_shader(int type, const char *label, char *(*load)(void)) {
  PFNGLCREATESHADER create=real_create_shader();
  PFNGLSHADERSOURCE source=real_shader_source("glShaderSource");
  PFNGLCOMPILESHADER compile=real_compile_shader();
  if(!create || !source || !compile) {
    log_line("water mesh subdivision skipped: missing geometry shader entry point");
    return 0;
  }

  char *text=load ? load() : 0;
  if(!text) {
    char msg[128];
    snprintf(msg,sizeof(msg),"water mesh subdivision skipped: missing %s",label ? label : "geometry shader");
    log_line(msg);
    return 0;
  }

  GLuint shader=create(GL_GEOMETRY_SHADER);
  if(!shader) {
    free(text);
    log_line("water mesh subdivision skipped: glCreateShader(GL_GEOMETRY_SHADER) failed");
    return 0;
  }

  GLint len=(GLint)strlen(text);
  const GLchar *one=text;
  source(shader,1,&one,&len);
  compile(shader);
  set_shader_info(shader,type,fnv1a(text),(unsigned int)len,text);

  GLint ok=1;
  PFNGLGETSHADERIV getiv=real_get_shader_iv();
  PFNGLGETSHADERINFOLOG getlog=real_get_shader_info_log();
  if(getiv)
    getiv(shader,GL_COMPILE_STATUS,&ok);
  if(!ok) {
    char logbuf[1024];
    GLsizei got=0;
    logbuf[0]=0;
    if(getlog)
      getlog(shader,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
    logbuf[sizeof(logbuf)-1]=0;
    char msg[1200];
    snprintf(msg,sizeof(msg),"water mesh subdivision disabled: %s compile failed: %s",
      label ? label : "geometry shader",logbuf);
    log_line(msg);
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) del(shader);
    free(text);
    return 0;
  }

  char msg[176];
  snprintf(msg,sizeof(msg),"compiled %s shader=%u replacement_len=%ld",
    label ? label : "geometry shader",shader,(long)len);
  log_line(msg);
  free(text);
  return shader;
}

static GLuint cached_water_geometry_shader(int type, const char *label, char *(*load)(void)) {
  GLuint *shader_slot=0;
  int *compiled_slot=0;
  if(type==SHADER_WATER_SURFACE) {
    shader_slot=&g_surface_geometry_shader;
    compiled_slot=&g_surface_geometry_compiled;
  } else if(type==SHADER_WATER_FLOW) {
    shader_slot=&g_flow_geometry_shader;
    compiled_slot=&g_flow_geometry_compiled;
  }
  if(!shader_slot || !compiled_slot) return 0;
  if(*compiled_slot)
    return *shader_slot;
  *compiled_slot=1;
  *shader_slot=compile_water_geometry_shader(type,label,load);
  return *shader_slot;
}

static void attach_water_geometry_for_program(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || p->geometry_attached || contact_mesh_subdivision()<=0) return;
  if(p->type!=SHADER_WATER_SURFACE && p->type!=SHADER_WATER_FLOW) return;
  if(!game_shader_replacement_enabled(p->type)) return;

  PFNGLATTACHSHADER attach=real_attach_shader();
  if(!attach) {
    log_line("water mesh subdivision skipped: missing glAttachShader");
    return;
  }

  const char *label=p->type==SHADER_WATER_SURFACE ?
    "surface geometry shader" : "flow water geometry shader";
  char *(*load)(void)=p->type==SHADER_WATER_SURFACE ?
    surface_geometry_shader : flow_geometry_shader;
  GLuint shader=cached_water_geometry_shader(p->type,label,load);
  if(!shader) return;

  attach(program,shader);
  p->geometry_shader=shader;
  p->geometry_attached=1;
  attach_program_shader_info(program,shader);

  char msg[176];
  snprintf(msg,sizeof(msg),"attached %s program=%u shader=%u subdivision=%d",
    label,program,shader,contact_mesh_subdivision());
  log_line(msg);
}

static void log_program_link_status(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !p->type) return;
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(!getiv) return;

  GLint ok=1;
  getiv(program,GL_LINK_STATUS,&ok);
  if(ok) {
    if(p->geometry_attached) {
      char msg[160];
      snprintf(msg,sizeof(msg),"linked water program=%u type=%s geometry_shader=%u",
        program,shader_type_name(p->type),p->geometry_shader);
      log_line(msg);
    }
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
  snprintf(msg,sizeof(msg),"water program link failed program=%u type=%s geometry_shader=%u log=%s",
    program,shader_type_name(p->type),p->geometry_shader,logbuf);
  log_line(msg);
}

static GLuint compile_grid_overlay_shader(GLenum stage, const char *label, char *(*load)(void)) {
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
    snprintf(msg,sizeof(msg),"water grid overlay shader compile failed stage=%s log=%s",
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

static int water_grid_overlay_index(void) {
  if(g_current_program_type==SHADER_WATER_SURFACE) return 0;
  if(g_current_program_type==SHADER_WATER_FLOW) return 1;
  return -1;
}

static WaterGridOverlay *current_water_grid_overlay(void) {
  int index=water_grid_overlay_index();
  return index>=0 ? &g_water_grid_overlays[index] : 0;
}

static GLuint current_water_grid_overlay_program(void) {
  WaterGridOverlay *o=current_water_grid_overlay();
  return o ? o->program : 0;
}

static int ensure_water_grid_overlay_program(void) {
  load_runtime_config();
  if(!g_runtime_water_grid_overlay) return 0;
  WaterGridOverlay *o=current_water_grid_overlay();
  if(!o) return 0;
  if(o->ready) return 1;
  if(o->failed) return 0;
  if(o->tried) return 0;
  o->tried=1;

  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  PFNGLBINDATTRIBLOCATION bind_attr=real_bind_attrib_location();
  CaptureGL *gl=capture_gl();
  if(!create_program || !attach || !link || !bind_attr || !gl || !gl->get_uniform_location) {
    log_line("water grid overlay disabled: missing GL program entry point");
    o->failed=1;
    return 0;
  }

  GLint locs[4]={-1,-1,-1,-1};
  if(!current_water_attrib_locations(locs)) {
    char msg[160];
    snprintf(msg,sizeof(msg),"water grid overlay disabled: %s program has no aCoord location",
      shader_type_name(g_current_program_type));
    log_line(msg);
    o->failed=1;
    return 0;
  }

  GLuint vs=compile_grid_overlay_shader(GL_VERTEX_SHADER,"vertex",water_grid_vertex_shader);
  GLuint gs=compile_grid_overlay_shader(GL_GEOMETRY_SHADER,"geometry",water_grid_geometry_shader);
  GLuint fs=compile_grid_overlay_shader(GL_FRAGMENT_SHADER,"fragment",water_grid_fragment_shader);
  if(!vs || !gs || !fs) {
    PFNGLDELETESHADER del=real_delete_shader();
    if(del) {
      if(vs) del(vs);
      if(gs) del(gs);
      if(fs) del(fs);
    }
    o->failed=1;
    return 0;
  }

  GLuint program=create_program();
  if(!program) {
    o->failed=1;
    return 0;
  }

  static const char *names[4]={"aCoord","aNormal","aLight","aColor"};
  for(int i=0;i<4;i++) {
    if(locs[i]>=0) bind_attr(program,(GLuint)locs[i],names[i]);
  }
  attach(program,vs);
  attach(program,gs);
  attach(program,fs);
  link(program);

  GLint ok=1;
  PFNGLGETPROGRAMIV getiv=real_get_program_iv();
  if(getiv) getiv(program,GL_LINK_STATUS,&ok);
  PFNGLDELETESHADER del_shader=real_delete_shader();
  if(del_shader) {
    del_shader(vs);
    del_shader(gs);
    del_shader(fs);
  }
  if(!ok) {
    char logbuf[1024];
    GLsizei got=0;
    logbuf[0]=0;
    PFNGLGETPROGRAMINFOLOG getlog=real_get_program_info_log();
    if(getlog)
      getlog(program,(GLsizei)sizeof(logbuf)-1,&got,logbuf);
    logbuf[sizeof(logbuf)-1]=0;
    char msg[1200];
    snprintf(msg,sizeof(msg),"water grid overlay link failed log=%s",logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    o->failed=1;
    return 0;
  }

  o->program=program;
  o->attr_coord=locs[0];
  o->attr_normal=locs[1];
  o->attr_light=locs[2];
  o->attr_color=locs[3];
  o->loc_proj=gl->get_uniform_location(program,"uProjMatrix");
  o->loc_model=gl->get_uniform_location(program,"uModelMatrix[0]");
  o->loc_view=gl->get_uniform_location(program,"uViewMatrix[0]");
  o->loc_contacts=gl->get_uniform_location(program,"uContacts[0]");
  o->loc_params=gl->get_uniform_location(program,"uParams");
  o->loc_capture_info=gl->get_uniform_location(program,"uTrWaterCaptureInfo");
  o->loc_toggle2=gl->get_uniform_location(program,"uTrWaterToggle2");
  o->loc_grid_info=gl->get_uniform_location(program,"uTrWaterGridInfo");
  o->ready=1;
  char msg[224];
  snprintf(msg,sizeof(msg),
    "water grid overlay linked type=%s program=%u attrs coord=%d normal=%d light=%d color=%d",
    shader_type_name(g_current_program_type),program,locs[0],locs[1],locs[2],locs[3]);
  log_line(msg);
  return 1;
}

static int ensure_synthetic_surface_program(void) {
  load_runtime_config();
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
  if(s->tried) return 0;
  s->tried=1;

  PFNGLCREATEPROGRAM create_program=real_create_program();
  PFNGLATTACHSHADER attach=real_attach_shader();
  PFNGLLINKPROGRAM link=real_link_program();
  PFNGLBINDATTRIBLOCATION bind_attr=real_bind_attrib_location();
  CaptureGL *gl=capture_gl();
  if(!create_program || !attach || !link || !bind_attr || !gl ||
     !gl->get_uniform_location) {
    log_line("synthetic water surface disabled: missing GL program entry point");
    s->failed=1;
    return 0;
  }

  GLint locs[4]={-1,-1,-1,-1};
  if(!current_water_attrib_locations(locs)) {
    log_line("synthetic water surface disabled: candidate program has no aCoord location");
    s->failed=1;
    return 0;
  }

  GLuint vs=compile_grid_overlay_shader(GL_VERTEX_SHADER,"synthetic vertex",
    synthetic_surface_vertex_shader);
  GLuint fs=compile_grid_overlay_shader(GL_FRAGMENT_SHADER,"synthetic fragment",
    synthetic_surface_shader);
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
    if(del) {
      del(vs);
      del(fs);
    }
    s->failed=1;
    return 0;
  }

  static const char *names[4]={"aCoord","aNormal","aLight","aColor"};
  for(int i=0;i<4;i++) {
    if(locs[i]>=0) bind_attr(program,(GLuint)locs[i],names[i]);
  }
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
    s->failed=1;
    return 0;
  }

  s->program=program;
  s->attr_coord=locs[0];
  s->attr_normal=locs[1];
  s->attr_light=locs[2];
  s->attr_color=locs[3];
  s->loc_proj=gl->get_uniform_location(program,"uProjMatrix");
  s->loc_model=gl->get_uniform_location(program,"uModelMatrix[0]");
  s->loc_view=gl->get_uniform_location(program,"uViewMatrix[0]");
  s->loc_scene=gl->get_uniform_location(program,"uTrWaterScene");
  s->loc_capture_info=gl->get_uniform_location(program,"uTrWaterCaptureInfo");
  s->loc_synthetic_info=gl->get_uniform_location(program,"uTrWaterSyntheticInfo");
  s->loc_synthetic_mode=gl->get_uniform_location(program,"uTrWaterSyntheticMode");
  s->loc_synthetic_profile=gl->get_uniform_location(program,"uTrWaterSyntheticProfile");
  s->loc_params=gl->get_uniform_location(program,"uParams");
  s->loc_draw_info=gl->get_uniform_location(program,"uTrWaterDrawInfo");
  s->loc_toggle0=gl->get_uniform_location(program,"uTrWaterToggle0");
  s->loc_toggle1=gl->get_uniform_location(program,"uTrWaterToggle1");
  s->loc_toggle2=gl->get_uniform_location(program,"uTrWaterToggle2");
  s->loc_contacts=gl->get_uniform_location(program,"uContacts[0]");
  s->loc_contact_motion=gl->get_uniform_location(program,"uContactMotion[0]");
  s->ready=1;
  char msg[224];
  snprintf(msg,sizeof(msg),
    "synthetic water surface linked program=%u attrs coord=%d normal=%d light=%d color=%d",
    program,locs[0],locs[1],locs[2],locs[3]);
  log_line(msg);
  return 1;
}

static void patch_surface_vertex_for_program(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || p->type!=SHADER_WATER_SURFACE) return;
  if(!game_shader_replacement_enabled(SHADER_WATER_SURFACE)) return;
  PFNGLSHADERSOURCE source=real_shader_source("glShaderSource");
  PFNGLCOMPILESHADER compile=real_compile_shader();
  if(!source || !compile) return;

  for(int i=0;i<p->shader_count;i++) {
    if(!is_surface_vertex_shader(p->shader_hashes[i])) continue;
    GLuint shader=p->shaders[i];
    ShaderTrack *s=shader_track(shader,0);
    if(s && s->type==SHADER_WATER_SURFACE) continue;
    char *replacement=surface_vertex_shader();
    if(!replacement) continue;
    GLint len=(GLint)strlen(replacement);
    const GLchar *one=replacement;
    source(shader,1,&one,&len);
    set_shader_info(shader,SHADER_WATER_SURFACE,fnv1a(replacement),(unsigned int)len,replacement);
    compile(shader);
    set_shader_type(shader,SHADER_WATER_SURFACE);
    log_tracked_shader_compile(shader,"surface vertex shader");
    p->shader_types[i]=SHADER_WATER_SURFACE;
    char msg[176];
    snprintf(msg,sizeof(msg),
      "patched surface vertex shader program=%u shader=%u original_hash=0x%08X replacement_len=%ld",
      program,shader,(unsigned int)p->shader_hashes[i],(long)len);
    log_line(msg);
    free(replacement);
  }
}

static void APIENTRY hook_glLinkProgram(GLuint program) {
  patch_surface_vertex_for_program(program);
  attach_water_geometry_for_program(program);
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
  if(s && (type || g_diag_log_unknown_shaders || g_diag_dump_unknown_shaders)) {
    char msg[320];
    snprintf(msg,sizeof(msg),"attached shader program=%u shader=%u type=%s hash=0x%08X len=%u preview=\"%s\"",
      program,shader,shader_type_name(s->type),(unsigned int)s->hash,s->len,s->preview);
    log_line(msg);
  }
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
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(program);
  if(g_current_program_type) {
    apply_effect_toggles(program);
    apply_contact_cache(program);
  }
  diag_poll_insert("glUseProgram");
  diag_log_program_use(program);
  diag_log_contacts(program,"glUseProgram");
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

static PFNGLCOMPRESSEDTEXIMAGE2D real_compressed_tex_image_2d(void) {
  static PFNGLCOMPRESSEDTEXIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE2D)gl_proc("glCompressedTexImage2D");
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE2D)gl_proc("glCompressedTexImage2DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXSUBIMAGE2D real_compressed_tex_sub_image_2d(void) {
  static PFNGLCOMPRESSEDTEXSUBIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE2D)gl_proc("glCompressedTexSubImage2D");
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE2D)gl_proc("glCompressedTexSubImage2DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXIMAGE3D real_compressed_tex_image_3d(void) {
  static PFNGLCOMPRESSEDTEXIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE3D)gl_proc("glCompressedTexImage3D");
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE3D)gl_proc("glCompressedTexImage3DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXSUBIMAGE3D real_compressed_tex_sub_image_3d(void) {
  static PFNGLCOMPRESSEDTEXSUBIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE3D)gl_proc("glCompressedTexSubImage3D");
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE3D)gl_proc("glCompressedTexSubImage3DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE2D real_compressed_texture_sub_image_2d(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE2D)gl_proc("glCompressedTextureSubImage2D");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE3D real_compressed_texture_sub_image_3d(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE3D)gl_proc("glCompressedTextureSubImage3D");
  return p;
}

static PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT real_compressed_texture_image_2d_ext(void) {
  static PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT)gl_proc("glCompressedTextureImage2DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT real_compressed_texture_image_3d_ext(void) {
  static PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT)gl_proc("glCompressedTextureImage3DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT real_compressed_texture_sub_image_2d_ext(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT)gl_proc("glCompressedTextureSubImage2DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT real_compressed_texture_sub_image_3d_ext(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT)gl_proc("glCompressedTextureSubImage3DEXT");
  return p;
}

static void APIENTRY hook_glCompressedTexImage2D(GLenum target, GLint level,
    GLenum internalformat, GLsizei width, GLsizei height, GLint border,
    GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXIMAGE2D real=real_compressed_tex_image_2d();
  note_compressed_texture_upload(target,level,0,1,image_size,data);
  if(real) real(target,level,internalformat,width,height,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexSubImage2D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXSUBIMAGE2D real=real_compressed_tex_sub_image_2d();
  note_compressed_texture_upload(target,level,0,1,image_size,data);
  if(real) real(target,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexImage3D(GLenum target, GLint level,
    GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXIMAGE3D real=real_compressed_tex_image_3d();
  note_compressed_texture_upload(target,level,0,depth,image_size,data);
  if(real) real(target,level,internalformat,width,height,depth,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexSubImage3D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXSUBIMAGE3D real=real_compressed_tex_sub_image_3d();
  note_compressed_texture_upload(target,level,zoffset,depth,image_size,data);
  if(real) real(target,level,xoffset,yoffset,zoffset,width,height,depth,
    format,image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage2D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2D real=real_compressed_texture_sub_image_2d();
  note_compressed_texture_upload_for_texture(texture,GL_TEXTURE_2D,level,0,1,
    image_size,data);
  if(real) real(texture,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage3D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3D real=real_compressed_texture_sub_image_3d();
  note_compressed_texture_upload_for_texture(texture,GL_TEXTURE_2D_ARRAY,
    level,zoffset,depth,image_size,data);
  if(real) real(texture,level,xoffset,yoffset,zoffset,width,height,depth,
    format,image_size,data);
}

static void APIENTRY hook_glCompressedTextureImage2DEXT(GLuint texture,
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLint border, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT real=real_compressed_texture_image_2d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,1,
    image_size,data);
  if(real) real(texture,target,level,internalformat,width,height,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLsizei depth, GLint border, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT real=real_compressed_texture_image_3d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,depth,
    image_size,data);
  if(real) real(texture,target,level,internalformat,width,height,depth,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage2DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT real=real_compressed_texture_sub_image_2d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,1,
    image_size,data);
  if(real) real(texture,target,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format,
    GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT real=real_compressed_texture_sub_image_3d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,zoffset,
    depth,image_size,data);
  if(real) real(texture,target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexImage2D(GLenum target,
    GLint level, GLenum internalformat, GLsizei width, GLsizei height,
    GLint border, GLsizei image_size, const void *data) {
  hook_glCompressedTexImage2D(target,level,internalformat,width,height,border,
    image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexSubImage2D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,
    format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexImage3D(GLenum target,
    GLint level, GLenum internalformat, GLsizei width, GLsizei height,
    GLsizei depth, GLint border, GLsizei image_size, const void *data) {
  hook_glCompressedTexImage3D(target,level,internalformat,width,height,depth,
    border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexSubImage3D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  hook_glCompressedTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,
    height,depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage2D(
    GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage2D(texture,level,xoffset,yoffset,width,
    height,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage3D(
    GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format,
    GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage3D(texture,level,xoffset,yoffset,zoffset,
    width,height,depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureImage2DEXT(
    GLuint texture, GLenum target, GLint level, GLenum internalformat,
    GLsizei width, GLsizei height, GLint border, GLsizei image_size,
    const void *data) {
  hook_glCompressedTextureImage2DEXT(texture,target,level,internalformat,
    width,height,border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureImage3DEXT(
    GLuint texture, GLenum target, GLint level, GLenum internalformat,
    GLsizei width, GLsizei height, GLsizei depth, GLint border,
    GLsizei image_size, const void *data) {
  hook_glCompressedTextureImage3DEXT(texture,target,level,internalformat,
    width,height,depth,border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage2DEXT(
    GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset,
    GLsizei width, GLsizei height, GLenum format, GLsizei image_size,
    const void *data) {
  hook_glCompressedTextureSubImage2DEXT(texture,target,level,xoffset,yoffset,
    width,height,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage3DEXT(
    GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset,
    GLint zoffset, GLsizei width, GLsizei height, GLsizei depth,
    GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage3DEXT(texture,target,level,xoffset,yoffset,
    zoffset,width,height,depth,format,image_size,data);
}

static void draw_water_grid_overlay_elements(GLenum mode, GLsizei count, GLenum type, const void *indices);
static void draw_synthetic_surface_elements(GLenum mode, GLsizei count, GLenum type, const void *indices);
static void draw_synthetic_surface_arrays(GLenum mode, GLint first, GLsizei count);
static void draw_water_grid_overlay_arrays(GLenum mode, GLint first, GLsizei count);
static PFNGLDRAWRANGEELEMENTS real_draw_range_elements(void);
static PFNGLDRAWELEMENTSBASEVERTEX real_draw_elements_base_vertex(void);
static void draw_water_grid_overlay_elements_base_vertex(GLenum mode, GLsizei count,
                                                         GLenum type, const void *indices,
                                                         GLint base_vertex);
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
static void draw_water_grid_overlay_arrays_instanced(GLenum mode, GLint first,
                                                     GLsizei count, GLsizei instance_count);
static void draw_water_grid_overlay_elements_instanced(GLenum mode, GLsizei count,
                                                       GLenum type, const void *indices,
                                                       GLsizei instance_count);
static void draw_water_grid_overlay_elements_instanced_base_vertex(GLenum mode, GLsizei count,
                                                                   GLenum type, const void *indices,
                                                                   GLsizei instance_count,
                                                                   GLint base_vertex);
static void draw_water_grid_overlay_arrays_indirect(GLenum mode, const void *indirect);
static void draw_water_grid_overlay_elements_indirect(GLenum mode, GLenum type,
                                                      const void *indirect);
static void draw_water_grid_overlay_multi_arrays_indirect(GLenum mode, const void *indirect,
                                                          GLsizei draw_count, GLsizei stride);
static void draw_water_grid_overlay_multi_elements_indirect(GLenum mode, GLenum type,
                                                            const void *indirect,
                                                            GLsizei draw_count, GLsizei stride);

__declspec(dllexport) void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
  PFNGLDRAWELEMENTS real=real_draw_elements();
  if(real) {
    note_draw("glDrawElements",mode,count);
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=
      current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface" : "synthetic water surface");
    else
      prepare_scene_capture("glDrawElements");
    if(!skip_original)
      real(mode,count,type,indices);
    if(synthetic_ready)
      draw_synthetic_surface_elements(mode,count,type,indices);
    draw_water_grid_overlay_elements(mode,count,type,indices);
  }
}

static PFNGLDRAWARRAYS real_draw_arrays(void) {
  static PFNGLDRAWARRAYS p;
  if(!p) p=(PFNGLDRAWARRAYS)gl_proc("glDrawArrays");
  return p;
}

static int read_current_uniform_matrix4(const char *name, GLfloat out[16]) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv || !g_current_program) return 0;
  GLint loc=gl->get_uniform_location(g_current_program,name);
  if(loc<0) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int read_current_uniform_vec4_array(const char *base, int count, GLfloat *out) {
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv || !g_current_program) return 0;
  for(int i=0;i<count;i++) {
    char name[48];
    snprintf(name,sizeof(name),"%s[%d]",base,i);
    GLint loc=gl->get_uniform_location(g_current_program,name);
    if(loc<0) return 0;
    gl->get_uniform_fv(g_current_program,loc,out+i*4);
  }
  return 1;
}

static void read_current_uniform_vec4_default(const char *name, GLfloat out[4],
                                              GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  out[0]=x; out[1]=y; out[2]=z; out[3]=w;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv || !g_current_program) return;
  GLint loc=gl->get_uniform_location(g_current_program,name);
  if(loc>=0) gl->get_uniform_fv(g_current_program,loc,out);
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
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();

  GLfloat proj[16];
  if(s->loc_proj>=0 && matrix4 && read_current_uniform_matrix4("uProjMatrix",proj))
    matrix4(s->loc_proj,1,GL_FALSE,proj);

  GLfloat model[16];
  if(s->loc_model>=0 && read_current_uniform_vec4_array("uModelMatrix",4,model))
    gl->uniform_4fv(s->loc_model,4,model);

  GLfloat view[16];
  if(s->loc_view>=0 && read_current_uniform_vec4_array("uViewMatrix",4,view))
    gl->uniform_4fv(s->loc_view,4,view);

  if(s->loc_scene>=0 && gl->uniform_1i)
    gl->uniform_1i(s->loc_scene,15);

  if(gl->active_texture && gl->bind_texture && g_scene_tex) {
    GLint old_active=0;
    if(gl->get_integer) gl->get_integer(GL_ACTIVE_TEXTURE,&old_active);
    gl->active_texture(GL_TEXTURE15);
    gl->bind_texture(GL_TEXTURE_2D,g_scene_tex);
    if(old_active) gl->active_texture((GLenum)old_active);
  }

  int capture_view_w=g_scene_view_w>0 ? g_scene_view_w : g_scene_w;
  int capture_view_h=g_scene_view_h>0 ? g_scene_view_h : g_scene_h;
  if(capture_view_w<=0) capture_view_w=1920;
  if(capture_view_h<=0) capture_view_h=1080;
  GLfloat inv_w=capture_view_w>0 ? 1.0f/(GLfloat)capture_view_w : 1.0f/1920.0f;
  GLfloat inv_h=capture_view_h>0 ? 1.0f/(GLfloat)capture_view_h : 1.0f/1080.0f;
  if(s->loc_capture_info>=0 && gl->uniform_4f)
    gl->uniform_4f(s->loc_capture_info,inv_w,inv_h,(GLfloat)capture_view_w,(GLfloat)capture_view_h);

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
    gl->uniform_4fv(s->loc_params,1,params);

  GLfloat draw_info[4]={
    (GLfloat)g_frame_index,1.0f,0.0f,0.0f
  };
  read_current_uniform_vec4_default("uTrWaterDrawInfo",draw_info,
    (GLfloat)g_frame_index,1.0f,0.0f,0.0f);
  if(s->loc_draw_info>=0)
    gl->uniform_4fv(s->loc_draw_info,1,draw_info);

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
    gl->uniform_4fv(s->loc_toggle0,1,toggle0);
  if(s->loc_toggle1>=0)
    gl->uniform_4fv(s->loc_toggle1,1,toggle1);
  if(s->loc_toggle2>=0)
    gl->uniform_4fv(s->loc_toggle2,1,toggle2);

  GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
  GLfloat synthetic_time=0.0f;
  if(read_uniform_vec4_index_now("uModelMatrix",3,model3))
    synthetic_time=model3[0];

  GLfloat contacts[16][4];
  GLfloat motions[16][4];
  memset(contacts,0,sizeof(contacts));
  memset(motions,0,sizeof(motions));
  build_contact_values(contacts);
  build_contact_motion_values(motions);
  if(s->loc_contacts>=0)
    gl->uniform_4fv(s->loc_contacts,16,&contacts[0][0]);
  if(s->loc_contact_motion>=0)
    gl->uniform_4fv(s->loc_contact_motion,16,&motions[0][0]);

  if(s->loc_synthetic_info>=0 && gl->uniform_4f)
    gl->uniform_4f(s->loc_synthetic_info,
      g_runtime_synthetic_opacity,g_runtime_synthetic_tint,
      g_runtime_synthetic_reflection,synthetic_time);

  if(s->loc_synthetic_mode>=0 && gl->uniform_4f) {
    GLfloat flow_mode=g_current_program_type==SHADER_WATER_FLOW ? 1.0f : 0.0f;
    gl->uniform_4f(s->loc_synthetic_mode,
      flow_mode,(GLfloat)g_current_program_type,draw_info[2],draw_info[3]);
  }

  if(s->loc_synthetic_profile>=0 && gl->uniform_4fv) {
    GLfloat profile[4];
    synthetic_water_profile(mode,count,count_known,params,model3,profile);
    gl->uniform_4fv(s->loc_synthetic_profile,1,profile);
  }
}

static void begin_synthetic_surface_state(GLint *old_program, GLint *old_blend,
                                           GLint *old_depth, GLint *old_cull,
                                           GLint *old_depth_mask,
                                           GLint *old_depth_func,
                                           GLint old_blend_func[4]) {
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_integer) {
    gl->get_integer(GL_CURRENT_PROGRAM,old_program);
    gl->get_integer(GL_BLEND,old_blend);
    gl->get_integer(GL_DEPTH_TEST,old_depth);
    gl->get_integer(GL_CULL_FACE,old_cull);
    gl->get_integer(GL_DEPTH_WRITEMASK,old_depth_mask);
    gl->get_integer(GL_DEPTH_FUNC,old_depth_func);
    gl->get_integer(GL_BLEND_SRC_RGB,&old_blend_func[0]);
    gl->get_integer(GL_BLEND_DST_RGB,&old_blend_func[1]);
    gl->get_integer(GL_BLEND_SRC_ALPHA,&old_blend_func[2]);
    gl->get_integer(GL_BLEND_DST_ALPHA,&old_blend_func[3]);
  }
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  if(disable) {
    disable(GL_BLEND);
    disable(GL_CULL_FACE);
  }
  if(enable) enable(GL_DEPTH_TEST);
  if(depth_func) depth_func(GL_LEQUAL);
  if(depth_mask) depth_mask(GL_FALSE);
}

static void end_synthetic_surface_state(GLint old_program, GLint old_blend,
                                        GLint old_depth, GLint old_cull,
                                        GLint old_depth_mask,
                                        GLint old_depth_func,
                                        const GLint old_blend_func[4]) {
  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(blend_func)
    blend_func((GLenum)old_blend_func[0],(GLenum)old_blend_func[1],
      (GLenum)old_blend_func[2],(GLenum)old_blend_func[3]);
  if(depth_mask) depth_mask((GLboolean)(old_depth_mask ? 1 : 0));
  if(depth_func) depth_func((GLenum)old_depth_func);
  if(old_cull) { if(enable) enable(GL_CULL_FACE); } else { if(disable) disable(GL_CULL_FACE); }
  if(old_depth) { if(enable) enable(GL_DEPTH_TEST); } else { if(disable) disable(GL_DEPTH_TEST); }
  if(old_blend) { if(enable) enable(GL_BLEND); } else { if(disable) disable(GL_BLEND); }
  if(use_program) use_program((GLuint)old_program);
}

static void init_synthetic_surface_draw_state(SyntheticSurfaceDrawState *state) {
  state->old_program=(GLint)g_current_program;
  state->old_blend=0;
  state->old_depth=0;
  state->old_cull=0;
  state->old_depth_mask=1;
  state->old_depth_func=GL_LEQUAL;
  state->old_blend_func[0]=GL_SRC_ALPHA;
  state->old_blend_func[1]=GL_ONE_MINUS_SRC_ALPHA;
  state->old_blend_func[2]=GL_ONE;
  state->old_blend_func[3]=GL_ONE_MINUS_SRC_ALPHA;
}

static int begin_synthetic_surface_draw(GLenum mode, GLsizei count, int count_known,
                                        SyntheticSurfaceDrawState *state) {
  if(!state) return 0;
  if(!current_draw_is_synthetic_surface_candidate(mode,count,count_known)) return 0;
  if(!g_scene_tex || g_scene_w<=0 || g_scene_h<=0) return 0;
  if(!ensure_synthetic_surface_program()) return 0;

  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!use_program) return 0;

  init_synthetic_surface_draw_state(state);
  begin_synthetic_surface_state(&state->old_program,&state->old_blend,
    &state->old_depth,&state->old_cull,&state->old_depth_mask,
    &state->old_depth_func,state->old_blend_func);
  use_program(g_synthetic_surface.program);
  setup_synthetic_surface_uniforms(mode,count,count_known);
  return 1;
}

static void end_synthetic_surface_draw(const SyntheticSurfaceDrawState *state) {
  if(!state) return;
  end_synthetic_surface_state(state->old_program,state->old_blend,
    state->old_depth,state->old_cull,state->old_depth_mask,
    state->old_depth_func,state->old_blend_func);
}

static GLenum synthetic_surface_last_error(void) {
  CaptureGL *gl=capture_gl();
  return (gl && gl->get_error) ? gl->get_error() : 0;
}

static void log_synthetic_surface_draw(GLenum mode, GLsizei count,
                                       int has_first, GLint first,
                                       int has_base_vertex, GLint base_vertex,
                                       GLenum err) {
  if(g_synthetic_surface_logged>=16u) return;

  char msg[288];
  if(has_first) {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw arrays frame=%u sourceProgram=%u sourceType=%s mode=0x%X first=%d count=%d opacity=%.2f tint=%.2f reflection=%.2f glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)first,(int)count,
      (double)g_runtime_synthetic_opacity,(double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,(unsigned int)err);
  } else if(has_base_vertex) {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw basevertex frame=%u sourceProgram=%u sourceType=%s mode=0x%X count=%d base=%d opacity=%.2f tint=%.2f reflection=%.2f glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,(int)base_vertex,
      (double)g_runtime_synthetic_opacity,(double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,(unsigned int)err);
  } else {
    snprintf(msg,sizeof(msg),
      "synthetic water surface draw frame=%u sourceProgram=%u sourceType=%s mode=0x%X count=%d opacity=%.2f tint=%.2f reflection=%.2f glerr=0x%04X",
      g_frame_index,g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,
      (double)g_runtime_synthetic_opacity,(double)g_runtime_synthetic_tint,
      (double)g_runtime_synthetic_reflection,(unsigned int)err);
  }
  log_line(msg);
  g_synthetic_surface_logged++;
}

static void draw_synthetic_surface_elements(GLenum mode, GLsizei count,
                                            GLenum type, const void *indices) {
  PFNGLDRAWELEMENTS draw=real_draw_elements();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices);
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays(GLenum mode, GLint first, GLsizei count) {
  PFNGLDRAWARRAYS draw=real_draw_arrays();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,first,count);
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_base_vertex(GLenum mode, GLsizei count,
                                                        GLenum type, const void *indices,
                                                        GLint base_vertex) {
  PFNGLDRAWELEMENTSBASEVERTEX draw=real_draw_elements_base_vertex();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices,base_vertex);
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_range_elements(GLenum mode, GLuint start, GLuint end,
                                                  GLsizei count, GLenum type,
                                                  const void *indices) {
  PFNGLDRAWRANGEELEMENTS draw=real_draw_range_elements();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,start,end,count,type,indices);
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
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,start,end,count,type,indices,base_vertex);
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays_instanced(GLenum mode, GLint first,
                                                    GLsizei count, GLsizei instance_count) {
  PFNGLDRAWARRAYSINSTANCED draw=real_draw_arrays_instanced();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,first,count,instance_count);
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced(GLenum mode, GLsizei count,
                                                      GLenum type, const void *indices,
                                                      GLsizei instance_count) {
  PFNGLDRAWELEMENTSINSTANCED draw=real_draw_elements_instanced();
  if(!draw || instance_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count);
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
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex);
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
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,first,count,instance_count,base_instance);
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
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_instance);
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
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
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
    if(current_draw_is_synthetic_surface_candidate(mode,count[i],1)) {
      found=1;
      if(has_flow && current_draw_is_synthetic_flow_candidate(mode,count[i],1))
        *has_flow=1;
    }
  }
  return found;
}

static int synthetic_multi_draw_should_skip_original(GLenum mode, const GLsizei *count,
                                                     GLsizei draw_count) {
  int saw=0;
  if(!count || draw_count<=0 || !g_synthetic_surface.ready) return 0;
  for(GLsizei i=0;i<draw_count;i++) {
    if(count[i]<=0) continue;
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count[i],1);
    if(!current_draw_is_synthetic_surface_candidate(mode,count[i],1))
      return 0;
    saw=1;
    if(!current_draw_should_skip_original_water_for_synthetic(mode,count[i],1) &&
       synthetic_flow)
      return 0;
  }
  return saw;
}

static void draw_synthetic_surface_arrays_indirect(GLenum mode, const void *indirect) {
  PFNGLDRAWARRAYSINDIRECT draw=real_draw_arrays_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  draw(mode,indirect);
  log_synthetic_surface_draw(mode,-1,1,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_indirect(GLenum mode, GLenum type,
                                                     const void *indirect) {
  PFNGLDRAWELEMENTSINDIRECT draw=real_draw_elements_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  draw(mode,type,indirect);
  log_synthetic_surface_draw(mode,-1,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_multi_arrays_indirect(GLenum mode, const void *indirect,
                                                         GLsizei draw_count, GLsizei stride) {
  PFNGLMULTIDRAWARRAYSINDIRECT draw=real_multi_draw_arrays_indirect();
  if(!draw || draw_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  draw(mode,indirect,draw_count,stride);
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
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  draw(mode,type,indirect,draw_count,stride);
  log_synthetic_surface_draw(mode,draw_count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static int water_grid_overlay_should_draw_common(GLenum mode, GLsizei count, int count_known) {
  load_runtime_config();
  if(!g_runtime_water_grid_overlay) return 0;
  if(g_runtime_debug_mode>0) return 0;
  if(g_current_program_type!=SHADER_WATER_SURFACE && g_current_program_type!=SHADER_WATER_FLOW) return 0;
  if(g_current_program_type==SHADER_WATER_FLOW && !g_runtime_water_grid_flow_overlay) return 0;
  if((mode!=GL_TRIANGLES && mode!=GL_TRIANGLE_STRIP && mode!=GL_TRIANGLE_FAN) ||
     (count_known && count<3)) {
    if(g_water_grid_overlay_skip_logged<12u) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "water grid overlay skip frame=%u type=%s mode=0x%X count=%d",
        g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,
        count_known ? (int)count : -1);
      log_line(msg);
      g_water_grid_overlay_skip_logged++;
    }
    return 0;
  }
  return 1;
}

static int water_grid_overlay_should_draw(GLenum mode, GLsizei count) {
  return water_grid_overlay_should_draw_common(mode,count,1);
}

static int water_grid_overlay_should_draw_indirect(GLenum mode) {
  return water_grid_overlay_should_draw_common(mode,0,0);
}

static int water_grid_overlay_should_log_draw(void) {
  int index=2;
  if(g_current_program_type==SHADER_WATER_SURFACE) index=0;
  else if(g_current_program_type==SHADER_WATER_FLOW) index=1;
  if(g_water_grid_overlay_draw_logged_by_type[index]>=14u) return 0;
  g_water_grid_overlay_draw_logged_by_type[index]++;
  return 1;
}

static void setup_water_grid_overlay_uniforms(void) {
  WaterGridOverlay *o=current_water_grid_overlay();
  CaptureGL *gl=capture_gl();
  if(!o || !gl || !gl->uniform_4fv) return;
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();

  GLfloat proj[16];
  if(o->loc_proj>=0 && matrix4 && read_current_uniform_matrix4("uProjMatrix",proj))
    matrix4(o->loc_proj,1,GL_FALSE,proj);

  GLfloat model[16];
  if(o->loc_model>=0 && read_current_uniform_vec4_array("uModelMatrix",4,model))
    gl->uniform_4fv(o->loc_model,4,model);

  GLfloat view[16];
  if(o->loc_view>=0 && read_current_uniform_vec4_array("uViewMatrix",4,view))
    gl->uniform_4fv(o->loc_view,4,view);

  GLfloat params[4];
  read_current_uniform_vec4_default("uParams",params,1.0f,1.0f,1.0f,0.0f);
  if(o->loc_params>=0)
    gl->uniform_4fv(o->loc_params,1,params);

  GLfloat capture[4];
  read_current_uniform_vec4_default("uTrWaterCaptureInfo",capture,1.0f/1920.0f,1.0f/1080.0f,1920.0f,1080.0f);
  if(o->loc_capture_info>=0)
    gl->uniform_4fv(o->loc_capture_info,1,capture);

  GLfloat toggle2[4]={
    effect_toggle_value(8),effect_toggle_value(9),
    effect_toggle_value(10),effect_toggle_value(11)
  };
  if(o->loc_toggle2>=0)
    gl->uniform_4fv(o->loc_toggle2,1,toggle2);

  GLfloat grid_info[4]={
    g_current_program_type==SHADER_WATER_FLOW ? 1.0f : 0.0f,
    g_current_program_type==SHADER_WATER_SURFACE ? 1.0f : 0.0f,
    0.0f,0.0f
  };
  if(o->loc_grid_info>=0)
    gl->uniform_4fv(o->loc_grid_info,1,grid_info);

  GLfloat contacts[16][4];
  float sum_abs=0.0f;
  memset(contacts,0,sizeof(contacts));
  int found=read_program_contacts(g_current_program,contacts,&sum_abs);
  if(!found || sum_abs<=0.001f) {
    GLfloat built[16][4];
    if(build_contact_values(built)) {
      float built_sum=contact_values_sum_abs(built);
      if(!found || built_sum>sum_abs) {
        memcpy(contacts,built,sizeof(contacts));
        sum_abs=built_sum;
      }
    }
  }
  if(o->loc_contacts>=0)
    gl->uniform_4fv(o->loc_contacts,16,&contacts[0][0]);
}

static void begin_water_grid_overlay_state(GLint *old_program, GLint *old_blend,
                                           GLint *old_depth, GLint *old_cull,
                                           GLint *old_depth_mask,
                                           GLint old_blend_func[4]) {
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_integer) {
    gl->get_integer(GL_CURRENT_PROGRAM,old_program);
    gl->get_integer(GL_BLEND,old_blend);
    gl->get_integer(GL_DEPTH_TEST,old_depth);
    gl->get_integer(GL_CULL_FACE,old_cull);
    gl->get_integer(GL_DEPTH_WRITEMASK,old_depth_mask);
    gl->get_integer(GL_BLEND_SRC_RGB,&old_blend_func[0]);
    gl->get_integer(GL_BLEND_DST_RGB,&old_blend_func[1]);
    gl->get_integer(GL_BLEND_SRC_ALPHA,&old_blend_func[2]);
    gl->get_integer(GL_BLEND_DST_ALPHA,&old_blend_func[3]);
  }
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(enable) enable(GL_BLEND);
  if(enable) enable(GL_DEPTH_TEST);
  if(disable) disable(GL_CULL_FACE);
  if(depth_mask) depth_mask(GL_FALSE);
  if(blend_func) blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
}

static void end_water_grid_overlay_state(GLint old_program, GLint old_blend,
                                         GLint old_depth, GLint old_cull,
                                         GLint old_depth_mask,
                                         const GLint old_blend_func[4]) {
  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(blend_func)
    blend_func((GLenum)old_blend_func[0],(GLenum)old_blend_func[1],
      (GLenum)old_blend_func[2],(GLenum)old_blend_func[3]);
  if(depth_mask) depth_mask((GLboolean)(old_depth_mask ? 1 : 0));
  if(old_cull) { if(enable) enable(GL_CULL_FACE); } else { if(disable) disable(GL_CULL_FACE); }
  if(old_depth) { if(enable) enable(GL_DEPTH_TEST); } else { if(disable) disable(GL_DEPTH_TEST); }
  if(old_blend) { if(enable) enable(GL_BLEND); } else { if(disable) disable(GL_BLEND); }
  if(use_program) use_program((GLuint)old_program);
}

static void draw_water_grid_overlay_elements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWELEMENTS draw=real_draw_elements();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,count,type,indices);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[192];
    snprintf(msg,sizeof(msg),"water grid overlay draw elements frame=%u type=%s mode=0x%X count=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

static void draw_water_grid_overlay_arrays(GLenum mode, GLint first, GLsizei count) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWARRAYS draw=real_draw_arrays();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,first,count);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[192];
    snprintf(msg,sizeof(msg),"water grid overlay draw arrays frame=%u type=%s mode=0x%X first=%d count=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)first,(int)count,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

__declspec(dllexport) void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  PFNGLDRAWARRAYS real=real_draw_arrays();
  if(real) {
    note_draw("glDrawArrays",mode,count);
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays" : "synthetic water surface arrays");
    else
      prepare_scene_capture("glDrawArrays");
    if(!skip_original)
      real(mode,first,count);
    if(synthetic_ready)
      draw_synthetic_surface_arrays(mode,first,count);
    draw_water_grid_overlay_arrays(mode,first,count);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface range" : "synthetic water surface range");
    else
      prepare_scene_capture("glDrawRangeElements");
    if(!skip_original)
      real(mode,start,end,count,type,indices);
    if(synthetic_ready)
      draw_synthetic_surface_range_elements(mode,start,end,count,type,indices);
    draw_water_grid_overlay_elements(mode,count,type,indices);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface basevertex" : "synthetic water surface basevertex");
    else
      prepare_scene_capture("glDrawElementsBaseVertex");
    if(!skip_original)
      real(mode,count,type,indices,base_vertex);
    if(synthetic_ready)
      draw_synthetic_surface_elements_base_vertex(mode,count,type,indices,base_vertex);
    draw_water_grid_overlay_elements_base_vertex(mode,count,type,indices,base_vertex);
  }
}

static void draw_water_grid_overlay_elements_base_vertex(GLenum mode, GLsizei count,
                                                         GLenum type, const void *indices,
                                                         GLint base_vertex) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWELEMENTSBASEVERTEX draw=real_draw_elements_base_vertex();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,count,type,indices,base_vertex);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[224];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw basevertex frame=%u type=%s mode=0x%X count=%d base=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,
      (int)base_vertex,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface range basevertex" : "synthetic water surface range basevertex");
    else
      prepare_scene_capture("glDrawRangeElementsBaseVertex");
    if(!skip_original)
      real(mode,start,end,count,type,indices,base_vertex);
    if(synthetic_ready)
      draw_synthetic_surface_range_elements_base_vertex(mode,start,end,count,type,indices,base_vertex);
    draw_water_grid_overlay_elements_base_vertex(mode,count,type,indices,base_vertex);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays instanced" : "synthetic water surface arrays instanced");
    else
      prepare_scene_capture("glDrawArraysInstanced");
    if(!skip_original)
      real(mode,first,count,instance_count);
    if(synthetic_ready)
      draw_synthetic_surface_arrays_instanced(mode,first,count,instance_count);
    draw_water_grid_overlay_arrays_instanced(mode,first,count,instance_count);
  }
}

static void draw_water_grid_overlay_arrays_instanced(GLenum mode, GLint first,
                                                     GLsizei count, GLsizei instance_count) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWARRAYSINSTANCED draw=real_draw_arrays_instanced();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,first,count,instance_count);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[224];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw arrays instanced frame=%u type=%s mode=0x%X first=%d count=%d instances=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)first,
      (int)count,(int)instance_count,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface elements instanced" : "synthetic water surface elements instanced");
    else
      prepare_scene_capture("glDrawElementsInstanced");
    if(!skip_original)
      real(mode,count,type,indices,instance_count);
    if(synthetic_ready)
      draw_synthetic_surface_elements_instanced(mode,count,type,indices,instance_count);
    draw_water_grid_overlay_elements_instanced(mode,count,type,indices,instance_count);
  }
}

static void draw_water_grid_overlay_elements_instanced(GLenum mode, GLsizei count,
                                                       GLenum type, const void *indices,
                                                       GLsizei instance_count) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWELEMENTSINSTANCED draw=real_draw_elements_instanced();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,count,type,indices,instance_count);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[224];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw elements instanced frame=%u type=%s mode=0x%X count=%d instances=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,
      (int)instance_count,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface instanced basevertex" : "synthetic water surface instanced basevertex");
    else
      prepare_scene_capture("glDrawElementsInstancedBaseVertex");
    if(!skip_original)
      real(mode,count,type,indices,instance_count,base_vertex);
    if(synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_vertex(mode,count,type,indices,instance_count,base_vertex);
    draw_water_grid_overlay_elements_instanced_base_vertex(mode,count,type,indices,instance_count,base_vertex);
  }
}

static void draw_water_grid_overlay_elements_instanced_base_vertex(GLenum mode, GLsizei count,
                                                                   GLenum type, const void *indices,
                                                                   GLsizei instance_count,
                                                                   GLint base_vertex) {
  if(!water_grid_overlay_should_draw(mode,count)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw=real_draw_elements_instanced_base_vertex();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,count,type,indices,instance_count,base_vertex);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[240];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw instanced basevertex frame=%u type=%s mode=0x%X count=%d instances=%d base=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,
      (int)instance_count,(int)base_vertex,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays instanced baseinstance" :
        "synthetic water surface arrays instanced baseinstance");
    else
      prepare_scene_capture("glDrawArraysInstancedBaseInstance");
    if(!skip_original)
      real(mode,first,count,instance_count,base_instance);
    if(synthetic_ready)
      draw_synthetic_surface_arrays_instanced_base_instance(mode,first,count,instance_count,base_instance);
    draw_water_grid_overlay_arrays_instanced(mode,first,count,instance_count);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface elements instanced baseinstance" :
        "synthetic water surface elements instanced baseinstance");
    else
      prepare_scene_capture("glDrawElementsInstancedBaseInstance");
    if(!skip_original)
      real(mode,count,type,indices,instance_count,base_instance);
    if(synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_instance(mode,count,type,indices,instance_count,base_instance);
    draw_water_grid_overlay_elements_instanced(mode,count,type,indices,instance_count);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_water_for_synthetic(mode,count,1) ||
      (synthetic_ready && !synthetic_flow);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface instanced basevertex baseinstance" :
        "synthetic water surface instanced basevertex baseinstance");
    else
      prepare_scene_capture("glDrawElementsInstancedBaseVertexBaseInstance");
    if(!skip_original)
      real(mode,count,type,indices,instance_count,base_vertex,base_instance);
    if(synthetic_ready)
      draw_synthetic_surface_elements_instanced_base_vertex_base_instance(
        mode,count,type,indices,instance_count,base_vertex,base_instance);
    draw_water_grid_overlay_elements_instanced_base_vertex(mode,count,type,indices,instance_count,base_vertex);
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
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int skip_original=synthetic_ready && first && count &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw arrays" : "synthetic water multi draw arrays");
    else
      prepare_scene_capture("glMultiDrawArrays");
    if(!skip_original)
      real(mode,first,count,draw_count);
    if(synthetic_ready && first && count) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
          draw_synthetic_surface_arrays(mode,first[i],count[i]);
      }
    }
    if(first && count) {
      for(GLsizei i=0;i<draw_count;i++)
        draw_water_grid_overlay_arrays(mode,first[i],count[i]);
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
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int skip_original=synthetic_ready && count && indices &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw elements" : "synthetic water multi draw elements");
    else
      prepare_scene_capture("glMultiDrawElements");
    if(!skip_original)
      real(mode,count,type,indices,draw_count);
    if(synthetic_ready && count && indices) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
          draw_synthetic_surface_elements(mode,count[i],type,indices[i]);
      }
    }
    if(count && indices) {
      for(GLsizei i=0;i<draw_count;i++)
        draw_water_grid_overlay_elements(mode,count[i],type,indices[i]);
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
    int synthetic_surface=synthetic_multi_draw_has_candidate(mode,count,draw_count,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int skip_original=synthetic_ready && count && indices && base_vertex &&
      synthetic_multi_draw_should_skip_original(mode,count,draw_count);
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi draw elements base vertex" :
        "synthetic water multi draw elements base vertex");
    else
      prepare_scene_capture("glMultiDrawElementsBaseVertex");
    if(!skip_original)
      real(mode,count,type,indices,draw_count,base_vertex);
    if(synthetic_ready && count && indices && base_vertex) {
      for(GLsizei i=0;i<draw_count;i++) {
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
          draw_synthetic_surface_elements_base_vertex(
            mode,count[i],type,indices[i],base_vertex[i]);
      }
    }
    if(count && indices && base_vertex) {
      for(GLsizei i=0;i<draw_count;i++)
        draw_water_grid_overlay_elements_base_vertex(mode,count[i],type,indices[i],base_vertex[i]);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      (current_draw_should_skip_original_water_for_synthetic(mode,0,0) ||
       (synthetic_ready && !synthetic_flow));
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow arrays indirect" : "synthetic water arrays indirect");
    else
      prepare_scene_capture("glDrawArraysIndirect");
    if(!skip_original)
      real(mode,indirect);
    if(synthetic_ready)
      draw_synthetic_surface_arrays_indirect(mode,indirect);
    draw_water_grid_overlay_arrays_indirect(mode,indirect);
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      (current_draw_should_skip_original_water_for_synthetic(mode,0,0) ||
       (synthetic_ready && !synthetic_flow));
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow elements indirect" : "synthetic water elements indirect");
    else
      prepare_scene_capture("glDrawElementsIndirect");
    if(!skip_original)
      real(mode,type,indirect);
    if(synthetic_ready)
      draw_synthetic_surface_elements_indirect(mode,type,indirect);
    draw_water_grid_overlay_elements_indirect(mode,type,indirect);
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
    int synthetic_surface=draw_count>0 &&
      current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      (current_draw_should_skip_original_water_for_synthetic(mode,0,0) ||
       (synthetic_ready && !synthetic_flow));
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi arrays indirect" :
        "synthetic water multi arrays indirect");
    else
      prepare_scene_capture("glMultiDrawArraysIndirect");
    if(!skip_original)
      real(mode,indirect,draw_count,stride);
    if(synthetic_ready)
      draw_synthetic_surface_multi_arrays_indirect(mode,indirect,draw_count,stride);
    draw_water_grid_overlay_multi_arrays_indirect(mode,indirect,draw_count,stride);
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
    int synthetic_surface=draw_count>0 &&
      current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      (current_draw_should_skip_original_water_for_synthetic(mode,0,0) ||
       (synthetic_ready && !synthetic_flow));
    if(synthetic_surface)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi elements indirect" :
        "synthetic water multi elements indirect");
    else
      prepare_scene_capture("glMultiDrawElementsIndirect");
    if(!skip_original)
      real(mode,type,indirect,draw_count,stride);
    if(synthetic_ready)
      draw_synthetic_surface_multi_elements_indirect(mode,type,indirect,draw_count,stride);
    draw_water_grid_overlay_multi_elements_indirect(mode,type,indirect,draw_count,stride);
  }
}

static void draw_water_grid_overlay_arrays_indirect(GLenum mode, const void *indirect) {
  if(!water_grid_overlay_should_draw_indirect(mode)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWARRAYSINDIRECT draw=real_draw_arrays_indirect();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,indirect);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[192];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw arrays indirect frame=%u type=%s mode=0x%X glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

static void draw_water_grid_overlay_elements_indirect(GLenum mode, GLenum type,
                                                      const void *indirect) {
  if(!water_grid_overlay_should_draw_indirect(mode)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLDRAWELEMENTSINDIRECT draw=real_draw_elements_indirect();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,type,indirect);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[208];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw elements indirect frame=%u type=%s mode=0x%X indexType=0x%X glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,
      (unsigned int)type,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

static void draw_water_grid_overlay_multi_arrays_indirect(GLenum mode, const void *indirect,
                                                          GLsizei draw_count, GLsizei stride) {
  if(draw_count<=0) return;
  if(!water_grid_overlay_should_draw_indirect(mode)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLMULTIDRAWARRAYSINDIRECT draw=real_multi_draw_arrays_indirect();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,indirect,draw_count,stride);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[224];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw multi arrays indirect frame=%u type=%s mode=0x%X draws=%d stride=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,
      (int)draw_count,(int)stride,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

static void draw_water_grid_overlay_multi_elements_indirect(GLenum mode, GLenum type,
                                                            const void *indirect,
                                                            GLsizei draw_count, GLsizei stride) {
  if(draw_count<=0) return;
  if(!water_grid_overlay_should_draw_indirect(mode)) return;
  if(!ensure_water_grid_overlay_program()) return;
  PFNGLMULTIDRAWELEMENTSINDIRECT draw=real_multi_draw_elements_indirect();
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!draw || !use_program) return;
  GLint old_program=(GLint)g_current_program;
  GLint old_blend=0,old_depth=0,old_cull=0,old_depth_mask=1;
  GLint old_blend_func[4]={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA};
  begin_water_grid_overlay_state(&old_program,&old_blend,&old_depth,&old_cull,&old_depth_mask,old_blend_func);
  use_program(current_water_grid_overlay_program());
  setup_water_grid_overlay_uniforms();
  draw(mode,type,indirect,draw_count,stride);
  GLenum err=0;
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_error) err=gl->get_error();
  if(water_grid_overlay_should_log_draw()) {
    char msg[240];
    snprintf(msg,sizeof(msg),
      "water grid overlay draw multi elements indirect frame=%u type=%s mode=0x%X indexType=0x%X draws=%d stride=%d glerr=0x%04X",
      g_frame_index,shader_type_name(g_current_program_type),(unsigned int)mode,
      (unsigned int)type,(int)draw_count,(int)stride,(unsigned int)err);
    log_line(msg);
    g_water_grid_overlay_draw_logged++;
  }
  end_water_grid_overlay_state(old_program,old_blend,old_depth,old_cull,old_depth_mask,old_blend_func);
}

__declspec(dllexport) BOOL WINAPI wglSwapBuffers(HDC hdc) {
  PFNWGLSWAPBUFFERS real=(PFNWGLSWAPBUFFERS)old_proc("wglSwapBuffers");
  BOOL ok=real ? real(hdc) : FALSE;
  advance_frame();
  return ok;
}

__declspec(dllexport) BOOL WINAPI wglSwapLayerBuffers(HDC hdc, UINT planes) {
  PFNWGLSWAPLAYERBUFFERS real=(PFNWGLSWAPLAYERBUFFERS)old_proc("wglSwapLayerBuffers");
  BOOL ok=real ? real(hdc,planes) : FALSE;
  advance_frame();
  return ok;
}

__declspec(dllexport) PROC WINAPI wglGetProcAddress(LPCSTR name) {
  if(name && (!lstrcmpA(name,"glShaderSource") || !lstrcmpA(name,"glShaderSourceARB"))) {
    return HOOK_PROC(hook_glShaderSource);
  }
  if(name && (!lstrcmpA(name,"glCompileShader") || !lstrcmpA(name,"glCompileShaderARB"))) {
    return HOOK_PROC(hook_glCompileShader);
  }
  if(name && (!lstrcmpA(name,"glLinkProgram") || !lstrcmpA(name,"glLinkProgramARB"))) {
    return HOOK_PROC(hook_glLinkProgram);
  }
  if(name && !lstrcmpA(name,"glAttachShader")) {
    return HOOK_PROC(hook_glAttachShader);
  }
  if(name && !lstrcmpA(name,"glUseProgram")) {
    return HOOK_PROC(hook_glUseProgram);
  }
  if(name && (!lstrcmpA(name,"glCompressedTexImage2D") ||
     !lstrcmpA(name,"glCompressedTexImage2DARB"))) {
    return HOOK_PROC(hook_glCompressedTexImage2D);
  }
  if(name && (!lstrcmpA(name,"glCompressedTexSubImage2D") ||
     !lstrcmpA(name,"glCompressedTexSubImage2DARB"))) {
    return HOOK_PROC(hook_glCompressedTexSubImage2D);
  }
  if(name && (!lstrcmpA(name,"glCompressedTexImage3D") ||
     !lstrcmpA(name,"glCompressedTexImage3DARB"))) {
    return HOOK_PROC(hook_glCompressedTexImage3D);
  }
  if(name && (!lstrcmpA(name,"glCompressedTexSubImage3D") ||
     !lstrcmpA(name,"glCompressedTexSubImage3DARB"))) {
    return HOOK_PROC(hook_glCompressedTexSubImage3D);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureSubImage2D")) {
    return HOOK_PROC(hook_glCompressedTextureSubImage2D);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureSubImage3D")) {
    return HOOK_PROC(hook_glCompressedTextureSubImage3D);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureImage2DEXT")) {
    return HOOK_PROC(hook_glCompressedTextureImage2DEXT);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureImage3DEXT")) {
    return HOOK_PROC(hook_glCompressedTextureImage3DEXT);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureSubImage2DEXT")) {
    return HOOK_PROC(hook_glCompressedTextureSubImage2DEXT);
  }
  if(name && !lstrcmpA(name,"glCompressedTextureSubImage3DEXT")) {
    return HOOK_PROC(hook_glCompressedTextureSubImage3DEXT);
  }
  if(name && !lstrcmpA(name,"glDrawElements")) {
    return HOOK_PROC(glDrawElements);
  }
  if(name && !lstrcmpA(name,"glDrawArrays")) {
    return HOOK_PROC(glDrawArrays);
  }
  if(name && !lstrcmpA(name,"glDrawRangeElements")) {
    return HOOK_PROC(hook_glDrawRangeElements);
  }
  if(name && !lstrcmpA(name,"glDrawElementsBaseVertex")) {
    return HOOK_PROC(hook_glDrawElementsBaseVertex);
  }
  if(name && !lstrcmpA(name,"glDrawRangeElementsBaseVertex")) {
    return HOOK_PROC(hook_glDrawRangeElementsBaseVertex);
  }
  if(name && (!lstrcmpA(name,"glDrawArraysInstanced") || !lstrcmpA(name,"glDrawArraysInstancedARB"))) {
    return HOOK_PROC(hook_glDrawArraysInstanced);
  }
  if(name && (!lstrcmpA(name,"glDrawElementsInstanced") || !lstrcmpA(name,"glDrawElementsInstancedARB"))) {
    return HOOK_PROC(hook_glDrawElementsInstanced);
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseVertex")) {
    return HOOK_PROC(hook_glDrawElementsInstancedBaseVertex);
  }
  if(name && !lstrcmpA(name,"glDrawArraysInstancedBaseInstance")) {
    return gl_proc(name) ? HOOK_PROC(hook_glDrawArraysInstancedBaseInstance) : 0;
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseInstance")) {
    return gl_proc(name) ? HOOK_PROC(hook_glDrawElementsInstancedBaseInstance) : 0;
  }
  if(name && !lstrcmpA(name,"glDrawElementsInstancedBaseVertexBaseInstance")) {
    return gl_proc(name) ? HOOK_PROC(hook_glDrawElementsInstancedBaseVertexBaseInstance) : 0;
  }
  if(name && (!lstrcmpA(name,"glMultiDrawArrays") || !lstrcmpA(name,"glMultiDrawArraysEXT"))) {
    return HOOK_PROC(hook_glMultiDrawArrays);
  }
  if(name && (!lstrcmpA(name,"glMultiDrawElements") || !lstrcmpA(name,"glMultiDrawElementsEXT"))) {
    return HOOK_PROC(hook_glMultiDrawElements);
  }
  if(name && !lstrcmpA(name,"glMultiDrawElementsBaseVertex")) {
    return HOOK_PROC(hook_glMultiDrawElementsBaseVertex);
  }
  if(name && !lstrcmpA(name,"glDrawArraysIndirect")) {
    return gl_proc(name) ? HOOK_PROC(hook_glDrawArraysIndirect) : 0;
  }
  if(name && !lstrcmpA(name,"glDrawElementsIndirect")) {
    return gl_proc(name) ? HOOK_PROC(hook_glDrawElementsIndirect) : 0;
  }
  if(name && !lstrcmpA(name,"glMultiDrawArraysIndirect")) {
    return gl_proc(name) ? HOOK_PROC(hook_glMultiDrawArraysIndirect) : 0;
  }
  if(name && !lstrcmpA(name,"glMultiDrawElementsIndirect")) {
    return gl_proc(name) ? HOOK_PROC(hook_glMultiDrawElementsIndirect) : 0;
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
    start_shader_preload();
  } else if(reason==DLL_PROCESS_DETACH) {
    AcquireSRWLockExclusive(&g_log_lock);
    if(g_log_handle!=INVALID_HANDLE_VALUE) {
      CloseHandle(g_log_handle);
      g_log_handle=INVALID_HANDLE_VALUE;
    }
    ReleaseSRWLockExclusive(&g_log_lock);
  }
  return TRUE;
}
