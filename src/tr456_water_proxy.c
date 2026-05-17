#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef char GLchar;
typedef unsigned char GLboolean;

#define HOOK_PROC(fn) ((PROC)(void *)(fn))
#ifndef TR456_DIAG_BUILD
#define TR456_DIAG_BUILD 0
#endif
#define TR456_EFFECT_TOGGLE_MASK 0x093Bu
static HMODULE g_self;
static HMODULE g_old_gl;
static char g_dir[MAX_PATH];
static char g_mod_dir[MAX_PATH];
static HANDLE g_log_handle=INVALID_HANDLE_VALUE;
static SRWLOCK g_log_lock=SRWLOCK_INIT;

static const char k_surface_key[]="vec3 tc = vWorldPos.xyz / 1024.0 * uParams.x;";
static const char k_reflect_key[]="float hC = texture(sNoise, vec3(uv, t)).x;";
static const char k_ssr_key[]="float not_water = 1 - texture(sTex0, vec3(uv_refract.xy, 0)).w;";
static const char k_flow_vertex_key[]="uv += uParams.xy * uModelMatrix[3].x;";

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_VIEWPORT 0x0BA2
#define GL_RGB 0x1907
#define GL_BGR 0x80E0
#define GL_RGBA 0x1908
#define GL_BGRA 0x80E1
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
#define GL_FRAMEBUFFER 0x8D40
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_PROGRAM_BINARY_LENGTH 0x8741
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
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
#define GL_ZERO 0
#define GL_ONE 1
#define GL_LESS 0x0201
#define GL_LEQUAL 0x0203
#define GL_ALWAYS 0x0207
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_BUFFER_SIZE 0x8764
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

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
  GLint params_loc;
  GLint material_profile_loc;
  int toggles_valid;
  unsigned int toggles_mask;
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
static int g_runtime_fbo_reflection=1;
static int g_runtime_fbo_capture_interval=1;
static int g_runtime_fbo_warmup_frames;
static int g_runtime_fbo_scale=1;
static int g_diag_insert_down;
static unsigned int g_diag_poll_frame;
static int g_diag_session;
static int g_diag_active_frames;
static int g_diag_lines_left;
static int g_runtime_verbose_log;
static int g_runtime_shader_binary_cache;
static int g_runtime_refresh_flow_texture_signatures;
static int g_runtime_ripple_min_count;
static int g_runtime_ripple_center_mode;
static int g_runtime_synthetic_surface;
static int g_runtime_synthetic_standing_only;
static int g_runtime_synthetic_flow_surface;
static int g_runtime_synthetic_reflect_surface;
static int g_runtime_flow_texture_fallback;
static GLfloat g_runtime_synthetic_opacity;
static GLfloat g_runtime_synthetic_tint;
static GLfloat g_runtime_synthetic_reflection;
static int g_runtime_synthetic_compile_delay_frames;
static unsigned int g_effect_toggle_mask=TR456_EFFECT_TOGGLE_MASK;
static unsigned int g_effect_hotkey_down_mask;
static unsigned int g_effect_hotkey_poll_frame;
static unsigned int g_synthetic_surface_logged;
static unsigned int g_synthetic_compile_delay_logged;
static unsigned int g_flow_material_bypass_logged;
static unsigned int g_flow_surface_texture_logged;
static unsigned int g_flow_texture_upload_probe_logged;
static unsigned int g_flow_surface_gate_logged;
static unsigned int g_flow_surface_confirmed_logged;
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
static unsigned int g_diag_cpu_contact_log_frame;
static unsigned int g_diag_synthetic_contact_log_frame;

typedef struct {
  GLfloat min_x;
  GLfloat max_x;
  GLfloat min_y;
  GLfloat max_y;
  GLfloat min_z;
  GLfloat max_z;
  GLfloat center_x;
  GLfloat center_y;
  GLfloat center_z;
  unsigned int frame;
  GLuint source_program;
  int source_type;
  GLsizei count;
} SyntheticContactSurface;

static SyntheticContactSurface g_synthetic_contact_surfaces[64];
static unsigned int g_synthetic_contact_surface_cursor;
static unsigned int g_synthetic_contact_log_frame;
#if TR456_DIAG_BUILD
static volatile LONG g_diag_wgl_query_count;
static volatile LONG g_diag_shader_source_count;
static volatile LONG g_diag_draw_call_count;
#endif
static volatile LONG g_runtime_started;
static SRWLOCK g_ini_lock=SRWLOCK_INIT;
static char *g_ini_text;
static int g_ini_loaded;

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
  int binary_cache_loaded;
  int compile_stage;
  unsigned int compile_step_frame;
  uint64_t binary_cache_key;
  GLuint pending_vs;
  GLuint pending_fs;
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
  int valid;
  unsigned int frame;
  unsigned int serial;
  GLuint program;
  int program_type;
  GLenum mode;
  GLsizei count;
  int count_known;
  int synthetic_standing;
  int synthetic_flow;
  int synthetic_surface;
  int synthetic_ready;
  int skip_original;
  const char *capture_reason;
} SyntheticDrawDecision;

typedef struct {
  unsigned int start_frame;
  unsigned int frames;
  unsigned long long draw_calls;
  unsigned long long tracked_water_draws;
  unsigned long long decision_hits;
  unsigned long long decision_misses;
  unsigned long long decision_ticks;
  unsigned long long synthetic_candidates;
  unsigned long long synthetic_standing;
  unsigned long long synthetic_flow;
  unsigned long long synthetic_ready;
  unsigned long long original_skips;
  unsigned long long capture_requests;
  unsigned long long capture_updates;
  unsigned long long capture_resizes;
  unsigned long long capture_ticks;
  unsigned long long synthetic_begin;
  unsigned long long synthetic_draws;
  unsigned long long synthetic_setup_ticks;
  unsigned long long synthetic_draw_ticks;
  unsigned long long ripple_contact_checks;
  unsigned long long ripple_contact_hits;
  unsigned long long ripple_contact_ticks;
} PerfTelemetry;

#define TR456_SHADER_BINARY_CACHE_MAGIC 0x53543436u
#define TR456_SHADER_BINARY_CACHE_VERSION 1u
#define TR456_SHADER_BINARY_CACHE_MAX_BYTES (8u*1024u*1024u)

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint64_t key;
  int32_t attr_coord;
  int32_t attr_normal;
  int32_t attr_light;
  int32_t attr_color;
  uint32_t binary_format;
  uint32_t binary_size;
  uint32_t reserved[4];
} TrshaderProgramBinaryCacheHeader;

typedef struct {
  GLint old_program;
  GLint old_blend;
  GLint old_depth;
  GLint old_cull;
  GLint old_depth_mask;
  GLint old_depth_func;
  GLint old_blend_func[4];
  GLint old_active_texture;
  GLint old_scene_texture_2d;
  int old_scene_texture_valid;
} SyntheticSurfaceDrawState;

static SyntheticSurfacePass g_synthetic_surface;
static SyntheticDrawDecision g_synthetic_draw_decision_cache;
static PerfTelemetry g_perf;
static LARGE_INTEGER g_perf_freq;
static int g_perf_freq_ready;
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
  { "tr456_water_synthetic_vertex.glsl", "synthetic water vertex shader", 0, 0 },
  { "tr456_water_synthetic.glsl", "synthetic water fragment shader", 0, 0 }
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
typedef void (APIENTRY *PFNGLTEXIMAGE3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLTEXSUBIMAGE3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLTEXTURESUBIMAGE3D)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLTEXTUREIMAGE3DEXT)(GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
typedef void (APIENTRY *PFNGLTEXTURESUBIMAGE3DEXT)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *);
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
typedef void (APIENTRY *PFNGLUNIFORM1IV)(GLint, GLsizei, const GLint *);
typedef void (APIENTRY *PFNGLUNIFORM4F)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY *PFNGLUNIFORM4FV)(GLint, GLsizei, const GLfloat *);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FV)(GLint, GLsizei, GLboolean, const GLfloat *);
typedef void (APIENTRY *PFNGLGETUNIFORMFV)(GLuint, GLint, GLfloat *);
typedef void (APIENTRY *PFNGLGETUNIFORMIV)(GLuint, GLint, GLint *);
typedef void (APIENTRY *PFNGLGETPROGRAMIV)(GLuint, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOG)(GLuint, GLsizei, GLsizei *, GLchar *);
typedef void (APIENTRY *PFNGLPROGRAMPARAMETERI)(GLuint, GLenum, GLint);
typedef void (APIENTRY *PFNGLGETPROGRAMBINARY)(GLuint, GLsizei, GLsizei *, GLenum *, void *);
typedef void (APIENTRY *PFNGLPROGRAMBINARY)(GLuint, GLenum, const void *, GLsizei);
typedef GLenum (APIENTRY *PFNGLGETERROR)(void);
typedef const unsigned char *(APIENTRY *PFNGLGETSTRING)(GLenum);
typedef void (APIENTRY *PFNGLBINDATTRIBLOCATION)(GLuint, GLuint, const GLchar *);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATION)(GLuint, const GLchar *);
typedef GLboolean (APIENTRY *PFNGLISENABLED)(GLenum);
typedef void (APIENTRY *PFNGLENABLE)(GLenum);
typedef void (APIENTRY *PFNGLDISABLE)(GLenum);
typedef void (APIENTRY *PFNGLDEPTHMASK)(GLboolean);
typedef void (APIENTRY *PFNGLDEPTHFUNC)(GLenum);
typedef void (APIENTRY *PFNGLBLENDFUNC)(GLenum, GLenum);
typedef void (APIENTRY *PFNGLBLENDFUNCSEPARATE)(GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRY *PFNGLVIEWPORT)(GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLGETVERTEXATTRIBIV)(GLuint, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETVERTEXATTRIBPOINTERV)(GLuint, GLenum, void **);
typedef void (APIENTRY *PFNGLBINDBUFFER)(GLenum, GLuint);
typedef void (APIENTRY *PFNGLGETBUFFERPARAMETERIV)(GLenum, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETBUFFERSUBDATA)(GLenum, intptr_t, intptr_t, void *);
typedef BOOL (WINAPI *PFNWGLSWAPBUFFERS)(HDC);
typedef BOOL (WINAPI *PFNWGLSWAPLAYERBUFFERS)(HDC, UINT);

#include "tr456_lab_common.h"
#include "tr456_wet_lara_lab.h"

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
static int ensure_synthetic_surface_program(void);

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
static int diag_is_active(void);
static int runtime_verbose_log(void);
static void perf_log_summary(const char *where, int reset_after);
static int ini_int(const char *key, int fallback);
static void ini_string(const char *key, const char *fallback, char *out,
                       size_t out_size);

static int file_exists(const char *path) {
  DWORD attr=GetFileAttributesA(path);
  return attr!=INVALID_FILE_ATTRIBUTES && !(attr&FILE_ATTRIBUTE_DIRECTORY);
}

static DWORD file_size_quick(const char *path) {
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
  const DWORD needle_len=(DWORD)strlen(needle);
  if(!buf || !needle || !needle_len || size<needle_len) return 0;
  for(DWORD i=0;i<=size-needle_len;i++) {
    if(!memcmp(buf+i,needle,needle_len)) return 1;
  }
  return 0;
}

static int file_contains_text_marker(const char *path, const char *marker) {
  HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,
    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return 0;

  const DWORD marker_len=(DWORD)strlen(marker);
  unsigned char buf[8192+64];
  DWORD carry=0;
  int found=0;
  while(!found) {
    DWORD got=0;
    if(!ReadFile(h,buf+carry,8192,&got,0) || got==0) break;
    const DWORD total=carry+got;
    if(buffer_contains_bytes(buf,total,marker)) {
      found=1;
      break;
    }
    if(marker_len>1 && total>=marker_len-1) {
      carry=marker_len-1;
      memmove(buf,buf+total-carry,carry);
    } else {
      carry=total;
    }
  }
  CloseHandle(h);
  return found;
}

static int is_own_proxy_file(const char *path) {
  return file_contains_text_marker(path,"tr456 water proxy loaded") ||
         file_contains_text_marker(path,"tr456_water_proxy.log");
}

static int should_load_chain_opengl(const char *path, const char *system_path,
                                    const char *label) {
  DWORD chain_size=file_size_quick(path);
  if(chain_size==INVALID_FILE_SIZE) return 0;
  if(system_path && system_path[0]) {
    DWORD system_size=file_size_quick(system_path);
    if(system_size!=INVALID_FILE_SIZE && chain_size==system_size) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "skipped %s because it matches system OpenGL",
        label && label[0] ? label : "chain OpenGL DLL");
      log_line(msg);
      return 0;
    }
  }
  if(chain_size<=768u*1024u && is_own_proxy_file(path)) {
    char msg[192];
    snprintf(msg,sizeof(msg),
      "skipped %s because it is another TR456 proxy",
      label && label[0] ? label : "chain OpenGL DLL");
    log_line(msg);
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
    int *surface_out, int *joint_out, int *joint_count_out,
    GLfloat *dist_out, GLfloat *dy_out, GLfloat *water_y_out) {
  if(!joints || !view) return 0;
  if(min_joints<1) min_joints=1;
  if(min_joints>32) min_joints=32;
  if(max_age_frames<0) max_age_frames=0;
  if(margin<0.0f) margin=0.0f;
  if(vertical<1.0f) vertical=1.0f;

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
      GLfloat vertical_ok=(jy<=s->max_y+vertical && jy>=s->min_y-vertical);
      if(dist<=0.001f && vertical_ok) {
        near_count++;
        if(dy<near_dy) {
          near_dy=dy;
          near_dist=dist;
          near_joint=j;
        }
      }
      GLfloat score=dist+f_max(dy-vertical,0.0f);
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
    }
    return 0;
  }

  char path[MAX_PATH];
  if(!join_game_path(path,file)) return 0;
  diag_logf("DIAG chain candidate label=\"%s\" path=\"%s\" exists=%d",
    label && label[0] ? label : file,path,file_exists(path));
  if(!file_exists(path)) return 0;
  if(!should_load_chain_opengl(path,system_path,
      label && label[0] ? label : file))
    return 0;

  g_old_gl=LoadLibraryA(path);
  if(g_old_gl) {
    char msg[224];
    snprintf(msg,sizeof(msg),"loaded %s",label && label[0] ? label : file);
    log_line(msg);
    return 1;
  }
  diag_logf("DIAG LoadLibrary %s failed gle=%lu",path,
    (unsigned long)GetLastError());
  return 0;
}

static void ensure_old_gl(void) {
  if(g_old_gl) return;
  char system_path[MAX_PATH];
  system_opengl_path(system_path);
  diag_logf("DIAG ensure_old_gl begin dir=\"%s\" mod=\"%s\" system=\"%s\"",
    g_dir,g_mod_dir,system_path);
  if(g_dir[0]) {
    if(ini_int("ReShadeChain",1)) {
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
  if(!system_path[0]) return;
  g_old_gl=LoadLibraryA(system_path);
  if(g_old_gl) log_line("loaded system opengl32.dll");
  else diag_logf("DIAG LoadLibrary system opengl32.dll failed gle=%lu",
    (unsigned long)GetLastError());
}

FARPROC old_proc(const char *name) {
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

static void ensure_ini_loaded(void) {
  if(g_ini_loaded) return;
  AcquireSRWLockExclusive(&g_ini_lock);
  if(!g_ini_loaded) {
    g_ini_text=read_text("tr456_water.ini");
    diag_logf("DIAG ini load %s dir=\"%s\" mod=\"%s\"",
      g_ini_text ? "ok" : "missing",g_dir,g_mod_dir);
    g_ini_loaded=1;
  }
  ReleaseSRWLockExclusive(&g_ini_lock);
}

static int ascii_lower(int c) {
  return (c>='A' && c<='Z') ? (c-'A'+'a') : c;
}

static int ascii_iequals_range(const char *start, const char *end,
                               const char *text) {
  const char *p=start;
  const char *q=text;
  while(p<end && *q) {
    if(ascii_lower((unsigned char)*p)!=ascii_lower((unsigned char)*q))
      return 0;
    p++;
    q++;
  }
  return p==end && !*q;
}

static const char *trim_left_range(const char *p, const char *end) {
  while(p<end && (*p==' ' || *p=='\t')) p++;
  return p;
}

static const char *trim_right_range(const char *start, const char *end) {
  while(end>start && (end[-1]==' ' || end[-1]=='\t')) end--;
  return end;
}

static int ini_lookup_value(const char *key, char *out, size_t out_size) {
  if(out && out_size) out[0]=0;
  if(!key || !out || !out_size) return 0;
  ensure_ini_loaded();
  if(!g_ini_text) return 0;

  int in_water=0;
  const char *line=g_ini_text;
  while(*line) {
    const char *line_end=line;
    while(*line_end && *line_end!='\r' && *line_end!='\n') line_end++;
    const char *start=trim_left_range(line,line_end);
    const char *end=trim_right_range(start,line_end);

    if(start<end && *start!=';' && *start!='#') {
      if(*start=='[') {
        const char *close=start+1;
        while(close<end && *close!=']') close++;
        if(close<end) {
          const char *section_start=trim_left_range(start+1,close);
          const char *section_end=trim_right_range(section_start,close);
          in_water=ascii_iequals_range(section_start,section_end,"Water");
        }
      } else if(in_water) {
        const char *eq=start;
        while(eq<end && *eq!='=') eq++;
        if(eq<end) {
          const char *key_start=trim_left_range(start,eq);
          const char *key_end=trim_right_range(key_start,eq);
          if(ascii_iequals_range(key_start,key_end,key)) {
            const char *value_start=trim_left_range(eq+1,end);
            const char *value_end=trim_right_range(value_start,end);
            size_t n=(size_t)(value_end-value_start);
            if(n>=out_size) n=out_size-1;
            memcpy(out,value_start,n);
            out[n]=0;
            return 1;
          }
        }
      }
    }

    line=line_end;
    while(*line=='\r' || *line=='\n') line++;
  }
  return 0;
}

static int ini_int(const char *key, int fallback) {
  char buf[64];
  if(!g_dir[0] || !ini_lookup_value(key,buf,sizeof(buf))) return fallback;
  char *end=0;
  long v=strtol(buf,&end,10);
  return end==buf ? fallback : (int)v;
}

static float ini_float(const char *key, float fallback) {
  char buf[64];
  if(!g_dir[0] || !ini_lookup_value(key,buf,sizeof(buf))) return fallback;
  char *end=0;
  double v=strtod(buf,&end);
  return end==buf ? fallback : (float)v;
}

static void ini_string(const char *key, const char *fallback, char *out,
                       size_t out_size) {
  if(!out || !out_size) return;
  out[0]=0;
  if(!g_dir[0] || !ini_lookup_value(key,out,out_size)) {
    if(fallback)
      lstrcpynA(out,fallback,(int)out_size);
  }
}

static void load_runtime_config(void) {
  if(g_runtime_config_loaded) return;
  g_runtime_shader_patching=ini_int("WaterShaderPatching",0);
  g_runtime_verbose_log=ini_int("VerboseLog",0);
  g_runtime_shader_binary_cache=ini_int("ShaderBinaryCache",1);
  g_runtime_refresh_flow_texture_signatures=
    ini_int("RefreshFlowTextureSignatures",0);
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
  g_effect_toggle_mask=(unsigned int)ini_int("EffectToggleMask",
    TR456_EFFECT_TOGGLE_MASK)&TR456_EFFECT_TOGGLE_MASK;
  g_runtime_ripple_min_count=ini_int("RippleSpriteMinCount",192);
  g_runtime_ripple_center_mode=ini_int("RippleSpriteCenterMode",1);
  if(g_runtime_ripple_center_mode<0) g_runtime_ripple_center_mode=0;
  if(g_runtime_ripple_center_mode>1) g_runtime_ripple_center_mode=1;
  g_runtime_synthetic_surface=ini_int("SyntheticWaterSurface",0);
  g_runtime_synthetic_standing_only=ini_int("SyntheticStandingWaterOnly",0);
  g_runtime_synthetic_flow_surface=ini_int("SyntheticFlowSurface",0);
  g_runtime_synthetic_reflect_surface=ini_int("SyntheticReflectSurface",1);
  g_runtime_flow_texture_fallback=ini_int("FlowTextureFallback",1);
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
  g_runtime_config_loaded=1;
}

static void build_shader_defines(char *out, size_t out_size) {
  const float refract_wave=ini_float("RefractionWaveStrength",0.90f);
  const float water_volume=ini_float("WaterVolumeStrength",0.45f);
  const float shoreline=ini_float("ShorelineStrength",0.35f);
  const float refract=ini_float("RefractStrength",0.75f);
  const float reflect=ini_float("ReflectStrength",0.85f);
  const float glint=ini_float("GlintStrength",0.22f);
  const float foam=ini_float("FoamStrength",0.20f);
  const float chroma=ini_float("ChromaStrength",0.10f);
  const float surface_caustic=ini_float("SurfaceCausticStrength",0.0f);
  const float surface_blue_stripe=ini_float("SurfaceBlueStripeStrength",0.0f);
  const float depth=ini_float("DepthStrength",0.45f);
  const float surface_relief=ini_float("SurfaceRelief",0.85f);
  const float wake_strength=ini_float("WakeStrength",1.0f);
  const float wake_width=ini_float("WakeWidth",0.42f);
  const float wake_length=ini_float("WakeLength",0.58f);
  const float contact_ripple_decay=ini_float("ContactRippleDecay",1.10f);
  const float contact_wake_directional=ini_float("ContactWakeDirectional",0.72f);
  const float flow_contact=ini_float("FlowContactStrength",1.0f);
  const float flow_contact_normal=ini_float("FlowContactNormalStrength",1.0f);
  const float rain_ripple=ini_float("RainRippleStrength",1.12f);
  const float wet_edge=ini_float("WetEdgeStrength",0.84f);
  const float micro_ripple=ini_float("MicroRippleStrength",0.48f);
  const float swell_strength=ini_float("SwellStrength",1.08f);
  const float wake_wave=ini_float("WakeWaveStrength",1.0f);
  const float edge_wave=ini_float("EdgeWaveStrength",0.75f);
  const float reflection_contrast=ini_float("ReflectionContrast",1.32f);
  const float reflection_shimmer=ini_float("ReflectionShimmer",0.18f);
  const float fresnel_strength=ini_float("FresnelStrength",1.18f);
  const float contact_edge=ini_float("ContactEdge",0.72f);
  const float depth_absorption=ini_float("DepthAbsorption",1.08f);
  const float water_saturation=ini_float("WaterSaturation",1.16f);
  const float water_brightness=ini_float("WaterBrightness",0.92f);
  const float water_texture=ini_float("WaterTextureStrength",1.0f);
  const float bump_mapping=ini_float("BumpMappingStrength",0.0f);
  const float bump_scale=ini_float("BumpMappingScale",1.0f);
  const float flow_bump=ini_float("FlowBumpMappingStrength",0.0f);
  const float synthetic_bump=ini_float("SyntheticBumpMappingStrength",0.0f);
  const float flow_detail=ini_float("FlowDetailStrength",0.0f);
  const float flow_strength=ini_float("FlowWaterStrength",0.85f);
  const float flow_reflection=ini_float("FlowReflectionStrength",0.45f);
  const float flow_opacity=ini_float("FlowOpacity",0.38f);
  const float flow_chroma=ini_float("FlowChromaStrength",0.10f);
  const float flow_standing_blend=ini_float("FlowStandingBlend",0.0f);
  const float flow_vertex=ini_float("FlowVertexStrength",0.0f);
  const float flow_wave=ini_float("FlowWaveStrength",0.85f);
  const float flow_volume_wave=ini_float("FlowVolumeWaveStrength",0.62f);
  const float flow_volume_wave_scale=ini_float("FlowVolumeWaveScale",1.0f);
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
  const float synthetic_reflection=ini_float("SyntheticSurfaceReflection",0.34f);
  const int synthetic_bump_enabled=(bump_mapping*synthetic_bump)>0.001f;
  const int synthetic_reflection_enabled=
    (fbo_reflection && synthetic_reflection*reflect>0.001f);
  const int synthetic_flow_reflection_enabled=
    (fbo_reflection && synthetic_reflection*flow_reflection*reflect>0.001f);
  snprintf(out,out_size,
    "#define TR456_WATER_REFRACTION_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_VOLUME_STRENGTH %.6f\n"
    "#define TR456_WATER_SHORELINE_STRENGTH %.6f\n"
    "#define TR456_WATER_REFRACT_STRENGTH %.6f\n"
    "#define TR456_WATER_REFLECT_STRENGTH %.6f\n"
    "#define TR456_WATER_GLINT_STRENGTH %.6f\n"
    "#define TR456_WATER_FOAM_STRENGTH %.6f\n"
    "#define TR456_WATER_CHROMA_STRENGTH %.6f\n"
    "#define TR456_WATER_SURFACE_CAUSTIC %.6f\n"
    "#define TR456_WATER_BLUE_STRIPE %.6f\n"
    "#define TR456_WATER_DEPTH_STRENGTH %.6f\n"
    "#define TR456_WATER_SURFACE_RELIEF %.6f\n"
    "#define TR456_WATER_WAKE_STRENGTH %.6f\n"
    "#define TR456_WATER_WAKE_WIDTH %.6f\n"
    "#define TR456_WATER_WAKE_LENGTH %.6f\n"
    "#define TR456_WATER_RAIN_RIPPLE %.6f\n"
    "#define TR456_WATER_WET_EDGE %.6f\n"
    "#define TR456_WATER_MICRO_RIPPLE %.6f\n"
    "#define TR456_WATER_SWELL_STRENGTH %.6f\n"
    "#define TR456_WATER_WAKE_WAVE %.6f\n"
    "#define TR456_WATER_CONTACT_RIPPLE_DECAY %.6f\n"
    "#define TR456_WATER_CONTACT_WAKE_DIRECTIONAL %.6f\n"
    "#define TR456_WATER_EDGE_WAVE %.6f\n"
    "#define TR456_WATER_REFLECTION_CONTRAST %.6f\n"
    "#define TR456_WATER_REFLECTION_SHIMMER %.6f\n"
    "#define TR456_WATER_FRESNEL_STRENGTH %.6f\n"
    "#define TR456_WATER_CONTACT_EDGE %.6f\n"
    "#define TR456_WATER_DEPTH_ABSORPTION %.6f\n"
    "#define TR456_WATER_COLOR_SATURATION %.6f\n"
    "#define TR456_WATER_BRIGHTNESS %.6f\n"
    "#define TR456_WATER_TEXTURE_STRENGTH %.6f\n"
    "#define TR456_WATER_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_BUMP_SCALE %.6f\n"
    "#define TR456_WATER_FLOW_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_SYNTHETIC_BUMP_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_DETAIL %.6f\n"
    "#define TR456_WATER_FLOW_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_REFLECTION %.6f\n"
    "#define TR456_WATER_FLOW_OPACITY %.6f\n"
    "#define TR456_WATER_FLOW_CHROMA %.6f\n"
    "#define TR456_WATER_FLOW_STANDING_BLEND %.6f\n"
    "#define TR456_WATER_FLOW_VERTEX_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_VOLUME_WAVE %.6f\n"
    "#define TR456_WATER_FLOW_VOLUME_WAVE_SCALE %.6f\n"
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
    "#define TR456_WATER_FLOW_CONTACT_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_CONTACT_NORMAL %.6f\n"
    "#define TR456_WATER_FBO_REFLECTION %d\n"
    "#define TR456_WATER_SYNTHETIC_BUMP_ENABLED %d\n"
    "#define TR456_WATER_SYNTHETIC_REFLECTION_ENABLED %d\n"
    "#define TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED %d\n",
    (double)refract_wave,
    (double)water_volume,(double)shoreline,
    (double)refract,(double)reflect,
    (double)glint,(double)foam,(double)chroma,
    (double)surface_caustic,(double)surface_blue_stripe,
    (double)depth,(double)surface_relief,
    (double)wake_strength,(double)wake_width,(double)wake_length,
    (double)rain_ripple,(double)wet_edge,
    (double)micro_ripple,
    (double)swell_strength,(double)wake_wave,
    (double)contact_ripple_decay,(double)contact_wake_directional,
    (double)edge_wave,
    (double)reflection_contrast,(double)reflection_shimmer,
    (double)fresnel_strength,
    (double)contact_edge,(double)depth_absorption,
    (double)water_saturation,(double)water_brightness,(double)water_texture,
    (double)bump_mapping,(double)bump_scale,
    (double)flow_bump,(double)synthetic_bump,(double)flow_detail,
    (double)flow_strength,(double)flow_reflection,(double)flow_opacity,
    (double)flow_chroma,
    (double)flow_standing_blend,
    (double)flow_vertex,
    (double)flow_wave,
    (double)flow_volume_wave,
    (double)flow_volume_wave_scale,
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
    (double)flow_contact,
    (double)flow_contact_normal,
    fbo_reflection,
    synthetic_bump_enabled,
    synthetic_reflection_enabled,
    synthetic_flow_reflection_enabled);
  if(!g_shader_defines_logged) {
    char msg[1024];
    snprintf(msg,sizeof(msg),
      "shader defines flow strength=%.3f opacity=%.3f speed=%.3f dir=%.0f chroma=%.3f standing=%.3f vertex=%.3f wave=%.3f volumeWave=%.3f/%.3f warp=%.3f surfaceDist=%.3f tension=%.3f crossDist=%.3f contact=%.3f/%.3f wakeDir=%.3f rippleDecay=%.3f reflectionShimmer=%.3f originalDeform=%.3f detail=%.3f/%.3f fbo=%d toggles=0x%03X",
      (double)flow_strength,(double)flow_opacity,
      (double)flow_speed,(double)flow_direction_sign,(double)flow_chroma,
       (double)flow_standing_blend,
       (double)flow_vertex,(double)flow_wave,
       (double)flow_volume_wave,(double)flow_volume_wave_scale,
       (double)flow_refraction_warp,
      (double)flow_surface_distortion,(double)flow_surface_tension,
      (double)flow_cross_distortion,
      (double)flow_contact,(double)flow_contact_normal,
      (double)contact_wake_directional,(double)contact_ripple_decay,
      (double)reflection_shimmer,
      (double)flow_original_deformation,
      0.0,(double)flow_detail,
      fbo_reflection,g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK);
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

static char *synthetic_surface_vertex_shader(void) {
  return configured_shader("tr456_water_synthetic_vertex.glsl","synthetic water vertex shader");
}

static char *synthetic_surface_shader(void) {
  return configured_shader("tr456_water_synthetic.glsl","synthetic water fragment shader");
}

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
  if(!(g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK)) {
    log_line("water shader patching armed with no effect toggles; original game water shaders will be used");
    return;
  }
  if(!include_heavy || !g_runtime_synthetic_surface) {
    log_line("synthetic shader preload skipped; using lazy loading");
    return;
  }
  preload_one_shader(synthetic_surface_vertex_shader);
  preload_one_shader(synthetic_surface_shader);
  log_line("preloaded synthetic water shader sources");
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
  int mode=ini_int("ShaderPreload",0);
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

static void runtime_start_once(void) {
  if(InterlockedCompareExchange(&g_runtime_started,1,0)!=0) return;
#if TR456_DIAG_BUILD
  char exe[MAX_PATH]="";
  char cwd[MAX_PATH]="";
  GetModuleFileNameA(0,exe,MAX_PATH);
  GetCurrentDirectoryA(MAX_PATH,cwd);
  diag_logf("DIAG runtime_start_once exe=\"%s\" cwd=\"%s\"",exe,cwd);
#endif
  log_line("tr456 water proxy loaded");
  start_shader_preload();
}

static int is_flow_vertex_shader(uint32_t hash) {
  return hash==0x7158F169u;
}

static int is_reflect_vertex_shader(uint32_t hash) {
  return hash==0x57FF35F3u;
}

static int is_flow_shader(uint32_t hash) {
  return hash==0x71E894DDu;
}

static int is_ripple_shader(uint32_t hash) {
  return hash==0x48E4F81Au;
}

static int shader_tracking_enabled(int type) {
  load_runtime_config();
  if(!g_runtime_shader_patching) return 0;
  return type==SHADER_WATER_SURFACE || type==SHADER_WATER_REFLECT ||
    type==SHADER_WATER_SSR || type==SHADER_WATER_FLOW ||
    type==SHADER_WATER_RIPPLE;
}

typedef int (*ShaderHashMatch)(uint32_t);

typedef struct {
  const char *label;
  int type;
  const char *contains;
  ShaderHashMatch hash_match;
} ShaderSourcePatch;

static const ShaderSourcePatch g_source_patches[] = {
  { "tracked surface shader", SHADER_WATER_SURFACE, k_surface_key, 0 },
  { "tracked reflect vertex shader", SHADER_WATER_REFLECT, 0, is_reflect_vertex_shader },
  { "tracked reflect shader", SHADER_WATER_REFLECT, k_reflect_key, 0 },
  { "tracked screen-space water shader", SHADER_WATER_SSR, k_ssr_key, 0 },
  { "tracked flow water vertex shader", SHADER_WATER_FLOW, k_flow_vertex_key, is_flow_vertex_shader },
  { "tracked flow water shader", SHADER_WATER_FLOW, 0, is_flow_shader },
  { "tracked ripple sprite shader", SHADER_WATER_RIPPLE, 0, is_ripple_shader }
};

static uint32_t fnv1a_update(uint32_t h, const char *text, size_t len) {
  if(!text) return h;
  for(size_t i=0;i<len;i++) {
    h^=(unsigned char)text[i];
    h*=16777619u;
  }
  return h;
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
      g_program_tracks[i].params_loc=-2;
      g_program_tracks[i].material_profile_loc=-2;
      for(int j=0;j<3;j++)
        g_program_tracks[i].toggle_loc[j]=-2;
      g_program_tracks[i].proj_matrix_loc=-2;
      g_program_tracks[i].ripple_info_loc=-2;
      g_program_tracks[i].toggles_valid=0;
      g_program_tracks[i].toggles_mask=0;
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
    p->params_loc=-2;
    p->material_profile_loc=-2;
    for(int i=0;i<3;i++)
      p->toggle_loc[i]=-2;
    p->proj_matrix_loc=-2;
    p->ripple_info_loc=-2;
    p->toggles_valid=0;
    p->toggles_mask=0;
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

static GLint trshader_cached_uniform_location(GLuint program, GLint *slot,
                                              const char *name) {
  if(!program || !name) return -1;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location) return -1;
  if(slot) {
    if(*slot==-2)
      *slot=gl->get_uniform_location(program,name);
    return *slot;
  }
  return gl->get_uniform_location(program,name);
}

static int trshader_uniform_single_digit_index(const char *name,
                                               const char *base, int max) {
  size_t n=strlen(base);
  if(strncmp(name,base,n) || name[n]!='[') return -1;
  int idx=(int)(name[n+1]-'0');
  if(idx<0 || idx>=max || name[n+2]!=']' || name[n+3]) return -1;
  return idx;
}

static GLint trshader_current_uniform_location(const char *name) {
  ProgramTrack *p=program_track(g_current_program,0);
  if(p) {
    if(!strcmp(name,"uParams"))
      return trshader_cached_uniform_location(g_current_program,&p->params_loc,name);
    if(!strcmp(name,"uTrWaterDrawInfo"))
      return trshader_cached_uniform_location(g_current_program,&p->draw_info_loc,name);
    if(!strcmp(name,"uTrWaterToggle0"))
      return trshader_cached_uniform_location(g_current_program,&p->toggle_loc[0],name);
    if(!strcmp(name,"uTrWaterToggle1"))
      return trshader_cached_uniform_location(g_current_program,&p->toggle_loc[1],name);
    if(!strcmp(name,"uTrWaterToggle2"))
      return trshader_cached_uniform_location(g_current_program,&p->toggle_loc[2],name);
    if(!strcmp(name,"uProjMatrix"))
      return trshader_cached_uniform_location(g_current_program,&p->proj_matrix_loc,name);
    int idx=trshader_uniform_single_digit_index(name,"uModelMatrix",4);
    if(idx>=0)
      return trshader_cached_uniform_location(g_current_program,
        &p->model_matrix_loc[idx],name);
    idx=trshader_uniform_single_digit_index(name,"uViewMatrix",4);
    if(idx>=0)
      return trshader_cached_uniform_location(g_current_program,
        &p->view_matrix_loc[idx],name);
  }
  return trshader_cached_uniform_location(g_current_program,0,name);
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
  return g_diag_active_frames>0 && g_diag_lines_left>0;
}

static int runtime_verbose_log(void) {
  return g_runtime_verbose_log || diag_is_active();
}

static void diag_consume_line(void) {
  if(g_diag_active_frames>0 && g_diag_lines_left>0)
    g_diag_lines_left--;
}

static void diag_begin(const char *where) {
  load_runtime_config();
  g_diag_session++;
  g_diag_active_frames=150;
  g_diag_lines_left=420;
  g_diag_cpu_contact_log_frame=0;
  char msg[256];
  snprintf(msg,sizeof(msg),
    "diag insert begin session=%d frame=%u where=%s current_program=%u current_type=%s capture_frames=%d max_lines=%d",
    g_diag_session,g_frame_index,where ? where : "unknown",g_current_program,
    shader_type_name(g_current_program_type),g_diag_active_frames,g_diag_lines_left);
  log_line(msg);
  snprintf(msg,sizeof(msg),
    "diag config session=%d patch=%d fbo=%d rippleMin=%d centerMode=%d toggles=0x%03X",
    g_diag_session,g_runtime_shader_patching,g_runtime_fbo_reflection,
    g_runtime_ripple_min_count,g_runtime_ripple_center_mode,
    (unsigned int)g_effect_toggle_mask);
  log_line(msg);
  diag_log_program_snapshot();
  perf_log_summary(where,1);
  tr456_wet_lara_diag_begin(where);
  diag_log_cpu_contact_state("diag-begin");
}

static void diag_poll_insert(const char *where) {
  if(g_diag_poll_frame==g_frame_index) return;
  g_diag_poll_frame=g_frame_index;
  SHORT s=GetAsyncKeyState(VK_INSERT);
  int down=(s&0x8000)!=0;
  if(down && !g_diag_insert_down)
    diag_begin(where);
  g_diag_insert_down=down;
}

static unsigned long long perf_ticks_now(void) {
  LARGE_INTEGER q;
  QueryPerformanceCounter(&q);
  return (unsigned long long)q.QuadPart;
}

static double perf_ticks_ms(unsigned long long ticks) {
  if(!g_perf_freq_ready) {
    QueryPerformanceFrequency(&g_perf_freq);
    if(!g_perf_freq.QuadPart) g_perf_freq.QuadPart=1;
    g_perf_freq_ready=1;
  }
  return (double)ticks*1000.0/(double)g_perf_freq.QuadPart;
}

static void perf_reset(void) {
  memset(&g_perf,0,sizeof(g_perf));
  g_perf.start_frame=g_frame_index;
}

static void perf_ensure_started(void) {
  if(!g_perf.start_frame)
    perf_reset();
}

static void perf_note_frame(void) {
  perf_ensure_started();
  g_perf.frames++;
}

static void perf_note_draw(int tracked_water) {
  perf_ensure_started();
  g_perf.draw_calls++;
  if(tracked_water)
    g_perf.tracked_water_draws++;
}

static void perf_note_decision(int cache_hit,
                               const SyntheticDrawDecision *decision,
                               unsigned long long ticks) {
  perf_ensure_started();
  if(cache_hit) {
    g_perf.decision_hits++;
    return;
  }
  g_perf.decision_misses++;
  g_perf.decision_ticks+=ticks;
  if(!decision) return;
  if(decision->synthetic_surface) g_perf.synthetic_candidates++;
  if(decision->synthetic_standing) g_perf.synthetic_standing++;
  if(decision->synthetic_flow) g_perf.synthetic_flow++;
  if(decision->synthetic_ready) g_perf.synthetic_ready++;
  if(decision->skip_original) g_perf.original_skips++;
}

static void perf_note_capture(int updated, int resized,
                              unsigned long long ticks) {
  perf_ensure_started();
  g_perf.capture_requests++;
  if(updated) g_perf.capture_updates++;
  if(resized) g_perf.capture_resizes++;
  g_perf.capture_ticks+=ticks;
}

static void perf_note_synthetic_begin(unsigned long long setup_ticks) {
  perf_ensure_started();
  g_perf.synthetic_begin++;
  g_perf.synthetic_setup_ticks+=setup_ticks;
}

static void perf_note_synthetic_draw(unsigned long long ticks) {
  perf_ensure_started();
  g_perf.synthetic_draws++;
  g_perf.synthetic_draw_ticks+=ticks;
}

static void perf_note_ripple_contact(int hit, unsigned long long ticks) {
  perf_ensure_started();
  g_perf.ripple_contact_checks++;
  if(hit) g_perf.ripple_contact_hits++;
  g_perf.ripple_contact_ticks+=ticks;
}

static void perf_log_summary(const char *where, int reset_after) {
  perf_ensure_started();
  char msg[1200];
  snprintf(msg,sizeof(msg),
    "perf telemetry where=%s frames=%u span=%u-%u draws=%llu water=%llu decision=%llu/%llu decisionMs=%.3f synthetic candidates=%llu standing=%llu flow=%llu ready=%llu skipOriginal=%llu capture req=%llu update=%llu resize=%llu captureMs=%.3f synthetic begin=%llu draws=%llu setupMs=%.3f drawCpuMs=%.3f contact checks=%llu hits=%llu contactMs=%.3f",
    where ? where : "unknown",
    g_perf.frames,g_perf.start_frame,g_frame_index,
    g_perf.draw_calls,g_perf.tracked_water_draws,
    g_perf.decision_hits,g_perf.decision_misses,
    perf_ticks_ms(g_perf.decision_ticks),
    g_perf.synthetic_candidates,g_perf.synthetic_standing,
    g_perf.synthetic_flow,g_perf.synthetic_ready,g_perf.original_skips,
    g_perf.capture_requests,g_perf.capture_updates,g_perf.capture_resizes,
    perf_ticks_ms(g_perf.capture_ticks),
    g_perf.synthetic_begin,g_perf.synthetic_draws,
    perf_ticks_ms(g_perf.synthetic_setup_ticks),
    perf_ticks_ms(g_perf.synthetic_draw_ticks),
    g_perf.ripple_contact_checks,g_perf.ripple_contact_hits,
    perf_ticks_ms(g_perf.ripple_contact_ticks));
  log_line(msg);
  if(g_diag_active_frames>0 && g_diag_lines_left>0)
    diag_consume_line();
  if(reset_after)
    perf_reset();
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
    case 2: return "unused";
    case 3: return "flow lanes/swirl";
    case 4: return "flow refraction warp";
    case 5: return "flow reflection";
    case 6: return "surface refraction warp";
    case 7: return "unused";
    case 8: return "surface foam/glint";
    case 9: return "surface reflection";
    case 10: return "unused";
    case 11: return "game/contact ripples";
    default: return "unknown";
  }
}

static float effect_toggle_value(int index) {
  return (g_effect_toggle_mask & (1u<<(unsigned int)index)) ? 1.0f : 0.0f;
}

static int effect_toggle_active_index(int index) {
  switch(index) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 8:
    case 11:
      return 1;
    default:
      return 0;
  }
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
  if(g_effect_hotkey_poll_frame==g_frame_index) return;
  g_effect_hotkey_poll_frame=g_frame_index;
  int ctrl=(GetAsyncKeyState(VK_CONTROL)&0x8000) ||
    (GetAsyncKeyState(VK_LCONTROL)&0x8000) ||
    (GetAsyncKeyState(VK_RCONTROL)&0x8000);
  int chord_j=(GetAsyncKeyState('J')&0x8000);
  unsigned int down=0;
  if(ctrl && chord_j) {
    for(int i=0;i<12;i++) {
      if(effect_toggle_active_index(i) && effect_toggle_key_down(i))
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
      g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK);
    log_line(msg);
  }
}

static void apply_effect_toggles(GLuint program) {
  ProgramTrack *p=program_track(program,0);
  if(!p || !p->type) return;
  unsigned int mask=g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK;
  if(p->toggles_valid && p->toggles_mask==mask) return;
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
    shadow_call_uniform_4f(gl,p->toggle_loc[0],
      effect_toggle_value(0),effect_toggle_value(1),
      effect_toggle_value(2),effect_toggle_value(3));
  if(p->toggle_loc[1]>=0)
    shadow_call_uniform_4f(gl,p->toggle_loc[1],
      effect_toggle_value(4),effect_toggle_value(5),
      effect_toggle_value(6),effect_toggle_value(7));
  if(p->toggle_loc[2]>=0)
    shadow_call_uniform_4f(gl,p->toggle_loc[2],
      effect_toggle_value(8),effect_toggle_value(9),
      effect_toggle_value(10),effect_toggle_value(11));
  p->toggles_mask=mask;
  p->toggles_valid=1;
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
      if(!shadow_read_uniform_floats(program,p->contacts_loc[i],values[i],4))
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
      if(!shadow_read_uniform_floats(program,p->contact_motion_loc[i],values[i],4))
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

static int add_ripple_contact(GLfloat x, GLfloat y, GLfloat z, GLfloat radius,
                              int *created_out) {
  const unsigned int lifetime=160u;
  const unsigned int stale=48u;
  int best=-1;
  float best_d2=1000000000.0f;
  radius=f_min(f_max(radius,12.0f),680.0f);
  if(created_out) *created_out=0;

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
  if(created_out) *created_out=1;
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
  if(!shadow_read_uniform_floats(g_current_program,p->proj_matrix_loc,proj,16))
    gl->get_uniform_fv(g_current_program,p->proj_matrix_loc,proj);
  shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
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
    if(diag_is_active()) {
      char msg[160];
      snprintf(msg,sizeof(msg),
        "ripple contact skip frame=%u count=%d threshold=%d reason=count candidate=0",
        g_frame_index,(int)count,g_runtime_ripple_min_count);
      log_line(msg);
      diag_consume_line();
    }
    return;
  }
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p) {
    if(diag_is_active()) {
      log_line("ripple contact skip reason=no_program_track");
      diag_consume_line();
    }
    return;
  }
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->get_uniform_fv) {
    if(diag_is_active()) {
      log_line("ripple contact skip reason=missing_gl_uniform_api");
      diag_consume_line();
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
      if(diag_is_active()) {
        char msg[144];
        snprintf(msg,sizeof(msg),
          "ripple contact skip frame=%u reason=no_model_matrix row=%d",
          g_frame_index,i);
        log_line(msg);
        diag_consume_line();
      }
      return;
    }
    if(!shadow_read_uniform_floats(g_current_program,p->model_matrix_loc[i],row[i],4))
      gl->get_uniform_fv(g_current_program,p->model_matrix_loc[i],row[i]);

    if(p->view_matrix_loc[i]==-2) {
      char name[32];
      snprintf(name,sizeof(name),"uViewMatrix[%d]",i);
      p->view_matrix_loc[i]=gl->get_uniform_location(g_current_program,name);
    }
    if(p->view_matrix_loc[i]>=0) {
      if(!shadow_read_uniform_floats(g_current_program,p->view_matrix_loc[i],view[i],4))
        gl->get_uniform_fv(g_current_program,p->view_matrix_loc[i],view[i]);
    } else
      view[i][3]=0.0f;
  }

  const float center=g_runtime_ripple_center_mode ? 0.5f : 0.0f;
  GLfloat world[3];
  transform_model_point_to_world(row,view,(GLfloat[3]){center,center,0.0f},world);
  GLfloat world_radius=ripple_world_radius_from_model(row);

  const int should_log_contact=
    diag_is_active() && g_ripple_contact_log_frame!=g_frame_index;
  GLfloat x=0.0f;
  GLfloat y=0.0f;
  GLfloat radius=0.0f;
  int projected=0;
  if(should_log_contact)
    projected=ripple_screen_from_program(p,row,view,&x,&y,&radius);
  int created=0;
  int slot_index=add_ripple_contact(world[0],world[1],world[2],
    world_radius,&created);
  if(slot_index>=0 && slot_index<16) {
    RippleContact *wet_c=&g_ripple_contacts[slot_index];
    tr456_wet_lara_note_ripple_circle(slot_index,created,count,
      wet_c->x,wet_c->y,wet_c->z,wet_c->radius,wet_c->speed,
      is_screen_contact_value(wet_c->x,wet_c->y,wet_c->z));
  }
  if(should_log_contact) {
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
      "ripple contact frame=%u slot=%d created=%d count=%d threshold=%d center=%.1f world=(%.1f %.1f %.1f) radius=%.1f screen%s=(%.3f %.3f) radius_px=%.1f motion=(%.2f %.2f %.2f) speed=%.2f scale=(%.1f %.1f %.1f)",
      g_frame_index,slot_index,created,(int)count,g_runtime_ripple_min_count,
      (double)center,(double)world[0],(double)world[1],
      (double)world[2],(double)world_radius,projected ? "" : "?",
      (double)x,(double)y,
      (double)radius,c ? (double)c->vx : 0.0,c ? (double)c->vy : 0.0,
      c ? (double)c->vz : 0.0,c ? (double)c->speed : 0.0,
      (double)sx,(double)sy,(double)sz);
    log_line(msg);
    g_ripple_contact_log_frame=g_frame_index;
    diag_consume_line();
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
      shadow_call_uniform_4fv(gl,p->contacts_loc[0],16,&values[0][0]);
    } else if(gl->uniform_4f) {
      for(int i=0;i<16;i++) {
        if(p->contacts_loc[i]==-2) {
          char name[32];
          snprintf(name,sizeof(name),"uContacts[%d]",i);
          p->contacts_loc[i]=gl->get_uniform_location(program,name);
        }
        if(p->contacts_loc[i]>=0) {
          const GLfloat *c=values[i];
          shadow_call_uniform_4f(gl,p->contacts_loc[i],c[0],c[1],c[2],c[3]);
        }
      }
    }
  }

  if(p->contact_motion_loc[0]==-2)
    p->contact_motion_loc[0]=gl->get_uniform_location(program,"uContactMotion[0]");
  if(p->contact_motion_loc[0]>=0 && gl->uniform_4fv) {
    shadow_call_uniform_4fv(gl,p->contact_motion_loc[0],16,&motions[0][0]);
  } else if(gl->uniform_4f) {
    for(int i=0;i<16;i++) {
      if(p->contact_motion_loc[i]==-2) {
        char name[40];
        snprintf(name,sizeof(name),"uContactMotion[%d]",i);
        p->contact_motion_loc[i]=gl->get_uniform_location(program,name);
      }
      if(p->contact_motion_loc[i]>=0) {
        const GLfloat *m=motions[i];
        shadow_call_uniform_4f(gl,p->contact_motion_loc[i],m[0],m[1],m[2],m[3]);
      }
    }
  }
}

static void update_draw_info_uniform(GLenum mode, GLsizei count) {
  if(!is_tracked_water_shader_type(g_current_program_type)) return;
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
  shadow_call_uniform_4f(gl,p->draw_info_loc,
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
  shadow_call_uniform_4f(gl,p->ripple_info_loc,water,(GLfloat)count,(GLfloat)threshold,0.0f);
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
  if(!gl || !g_current_program) return 0;
  GLint loc=trshader_current_uniform_location(name);
  if(loc<0) return 0;
  if(shadow_read_uniform_floats(g_current_program,loc,out,4))
    return 1;
  if(!gl->get_uniform_fv) return 0;
  gl->get_uniform_fv(g_current_program,loc,out);
  return 1;
}

static int read_uniform_vec4_index_now(const char *base, int index, GLfloat out[4]) {
  char name[48];
  snprintf(name,sizeof(name),"%s[%d]",base,index);
  return read_uniform_vec4_now(name,out);
}

static PFNGLGETVERTEXATTRIBIV real_get_vertex_attrib_iv(void) {
  static PFNGLGETVERTEXATTRIBIV p;
  if(!p) p=(PFNGLGETVERTEXATTRIBIV)gl_proc("glGetVertexAttribiv");
  return p;
}

static PFNGLGETVERTEXATTRIBPOINTERV real_get_vertex_attrib_pointer_v(void) {
  static PFNGLGETVERTEXATTRIBPOINTERV p;
  if(!p) p=(PFNGLGETVERTEXATTRIBPOINTERV)gl_proc("glGetVertexAttribPointerv");
  return p;
}

static PFNGLBINDBUFFER real_bind_buffer(void) {
  static PFNGLBINDBUFFER p;
  if(!p) p=(PFNGLBINDBUFFER)gl_proc("glBindBuffer");
  return p;
}

static PFNGLGETBUFFERPARAMETERIV real_get_buffer_parameter_iv(void) {
  static PFNGLGETBUFFERPARAMETERIV p;
  if(!p) p=(PFNGLGETBUFFERPARAMETERIV)gl_proc("glGetBufferParameteriv");
  return p;
}

static PFNGLGETBUFFERSUBDATA real_get_buffer_sub_data(void) {
  static PFNGLGETBUFFERSUBDATA p;
  if(!p) p=(PFNGLGETBUFFERSUBDATA)gl_proc("glGetBufferSubData");
  return p;
}

static int tr456_gl_component_size(GLenum type) {
  switch(type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
      return 4;
    default:
      return 0;
  }
}

static GLfloat tr456_decode_gl_scalar(const unsigned char *p, GLenum type) {
  switch(type) {
    case GL_BYTE: return (GLfloat)(*(const signed char *)p);
    case GL_UNSIGNED_BYTE: return (GLfloat)(*(const unsigned char *)p);
    case GL_SHORT: {
      int16_t v=0; memcpy(&v,p,sizeof(v)); return (GLfloat)v;
    }
    case GL_UNSIGNED_SHORT: {
      uint16_t v=0; memcpy(&v,p,sizeof(v)); return (GLfloat)v;
    }
    case GL_INT: {
      int32_t v=0; memcpy(&v,p,sizeof(v)); return (GLfloat)v;
    }
    case GL_UNSIGNED_INT: {
      uint32_t v=0; memcpy(&v,p,sizeof(v)); return (GLfloat)v;
    }
    case GL_FLOAT: {
      GLfloat v=0.0f; memcpy(&v,p,sizeof(v)); return v;
    }
    default:
      return 0.0f;
  }
}

typedef struct {
  GLint size;
  GLint stride;
  GLenum type;
  GLint normalized;
  GLuint buffer;
  intptr_t offset;
  int component_size;
} Tr456AttribSource;

static int tr456_current_coord_attrib(Tr456AttribSource *out) {
  if(!out || !g_current_program) return 0;
  PFNGLGETATTRIBLOCATION get_attr=(PFNGLGETATTRIBLOCATION)gl_proc("glGetAttribLocation");
  PFNGLGETVERTEXATTRIBIV getiv=real_get_vertex_attrib_iv();
  PFNGLGETVERTEXATTRIBPOINTERV getptr=real_get_vertex_attrib_pointer_v();
  if(!get_attr || !getiv || !getptr) return 0;
  GLint loc=get_attr(g_current_program,"aCoord");
  if(loc<0 || loc>15) return 0;
  memset(out,0,sizeof(*out));
  GLint enabled=0;
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&enabled);
  if(!enabled) return 0;
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_SIZE,&out->size);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_STRIDE,&out->stride);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_TYPE,(GLint *)&out->type);
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,&out->normalized);
  GLint buffer=0;
  getiv((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,&buffer);
  out->buffer=(GLuint)buffer;
  void *ptr=0;
  getptr((GLuint)loc,GL_VERTEX_ATTRIB_ARRAY_POINTER,&ptr);
  out->offset=(intptr_t)ptr;
  out->component_size=tr456_gl_component_size(out->type);
  if(out->size<3 || out->component_size<=0 || !out->buffer) return 0;
  if(out->stride<=0) out->stride=out->size*out->component_size;
  return 1;
}

static int tr456_bound_buffer_size(GLenum target, GLint *size_out) {
  PFNGLGETBUFFERPARAMETERIV getparam=real_get_buffer_parameter_iv();
  if(!getparam || !size_out) return 0;
  *size_out=0;
  getparam(target,GL_BUFFER_SIZE,size_out);
  return *size_out>0;
}

static int tr456_read_bound_buffer(GLenum target, intptr_t offset,
                                   intptr_t size, void *out) {
  PFNGLGETBUFFERSUBDATA getsub=real_get_buffer_sub_data();
  if(!getsub || !out || offset<0 || size<=0) return 0;
  GLint buffer_size=0;
  if(!tr456_bound_buffer_size(target,&buffer_size)) return 0;
  if(offset+size>(intptr_t)buffer_size) return 0;
  getsub(target,offset,size,out);
  return 1;
}

static int tr456_read_coord_vertex(const Tr456AttribSource *attr,
                                   GLuint vertex, GLfloat out[3]) {
  if(!attr || !out) return 0;
  unsigned char bytes[64];
  intptr_t offset=attr->offset+(intptr_t)vertex*(intptr_t)attr->stride;
  intptr_t size=(intptr_t)attr->component_size*(intptr_t)attr->size;
  if(size<=0 || size>(intptr_t)sizeof(bytes)) return 0;
  if(!tr456_read_bound_buffer(GL_ARRAY_BUFFER,offset,size,bytes)) return 0;
  out[0]=tr456_decode_gl_scalar(bytes,attr->type);
  out[1]=tr456_decode_gl_scalar(bytes+attr->component_size,attr->type);
  out[2]=tr456_decode_gl_scalar(bytes+attr->component_size*2,attr->type);
  return 1;
}

static int tr456_index_size(GLenum type) {
  switch(type) {
    case GL_UNSIGNED_BYTE: return 1;
    case GL_UNSIGNED_SHORT: return 2;
    case GL_UNSIGNED_INT: return 4;
    default: return 0;
  }
}

static int tr456_read_element_index(GLenum type, const void *indices,
                                    GLsizei element, GLuint *out) {
  if(!out || element<0) return 0;
  int size=tr456_index_size(type);
  if(size<=0) return 0;
  GLint ebo=0;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_integer) return 0;
  shadow_get_integer_or_gl(GL_ELEMENT_ARRAY_BUFFER_BINDING,&ebo);
  if(!ebo) return 0;
  unsigned char bytes[4]={0,0,0,0};
  intptr_t offset=(intptr_t)(uintptr_t)indices+(intptr_t)element*(intptr_t)size;
  if(!tr456_read_bound_buffer(GL_ELEMENT_ARRAY_BUFFER,offset,size,bytes))
    return 0;
  if(type==GL_UNSIGNED_BYTE) *out=(GLuint)bytes[0];
  else if(type==GL_UNSIGNED_SHORT) {
    uint16_t v=0; memcpy(&v,bytes,sizeof(v)); *out=(GLuint)v;
  } else {
    uint32_t v=0; memcpy(&v,bytes,sizeof(v)); *out=(GLuint)v;
  }
  return 1;
}

static void tr456_record_synthetic_bounds(const GLfloat minv[3],
                                          const GLfloat maxv[3],
                                          GLsizei count) {
  GLfloat width=maxv[0]-minv[0];
  GLfloat depth=maxv[2]-minv[2];
  if(width<32.0f || depth<32.0f) return;
  int slot=(int)(g_synthetic_contact_surface_cursor++&
    (unsigned int)((sizeof(g_synthetic_contact_surfaces)/
    sizeof(g_synthetic_contact_surfaces[0]))-1u));
  SyntheticContactSurface *s=&g_synthetic_contact_surfaces[slot];
  s->min_x=minv[0]; s->max_x=maxv[0];
  s->min_y=minv[1]; s->max_y=maxv[1];
  s->min_z=minv[2]; s->max_z=maxv[2];
  s->center_x=(minv[0]+maxv[0])*0.5f;
  s->center_y=(minv[1]+maxv[1])*0.5f;
  s->center_z=(minv[2]+maxv[2])*0.5f;
  s->frame=g_frame_index;
  s->source_program=g_current_program;
  s->source_type=g_current_program_type;
  s->count=count;
  if(diag_is_active() && g_synthetic_contact_log_frame!=g_frame_index) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "synthetic contact surface frame=%u slot=%d type=%s count=%d xz=(%.1f..%.1f %.1f..%.1f) y=%.1f..%.1f",
      g_frame_index,slot,shader_type_name(g_current_program_type),(int)count,
      (double)minv[0],(double)maxv[0],(double)minv[2],(double)maxv[2],
      (double)minv[1],(double)maxv[1]);
    log_line(msg);
    g_synthetic_contact_log_frame=g_frame_index;
    diag_consume_line();
  }
}

static void tr456_record_synthetic_surface_vertices(
    GLsizei count, int has_first, GLint first,
    int has_indices, GLenum type, const void *indices, GLint base_vertex) {
  if(count<=0) return;
  Tr456AttribSource attr;
  if(!tr456_current_coord_attrib(&attr)) return;
  PFNGLBINDBUFFER bind_buffer=real_bind_buffer();
  if(!bind_buffer) return;
  GLint old_array=0;
  shadow_get_integer_or_gl(GL_ARRAY_BUFFER_BINDING,&old_array);
  bind_buffer(GL_ARRAY_BUFFER,attr.buffer);

  GLfloat row[3][4];
  GLfloat view[3][4];
  for(int i=0;i<3;i++) {
    if(!read_uniform_vec4_index_now("uModelMatrix",i,row[i])) {
      bind_buffer(GL_ARRAY_BUFFER,(GLuint)old_array);
      return;
    }
    if(!read_uniform_vec4_index_now("uViewMatrix",i,view[i])) {
      view[i][0]=0.0f; view[i][1]=0.0f; view[i][2]=0.0f; view[i][3]=0.0f;
    }
  }

  const int max_samples=512;
  int samples=count<max_samples ? (int)count : max_samples;
  GLfloat minv[3]={1000000000.0f,1000000000.0f,1000000000.0f};
  GLfloat maxv[3]={-1000000000.0f,-1000000000.0f,-1000000000.0f};
  int got=0;
  for(int s=0;s<samples;s++) {
    GLsizei pos=(samples<=1) ? 0 :
      (GLsizei)(((long long)s*(long long)(count-1))/(long long)(samples-1));
    GLuint vertex=0;
    if(has_indices) {
      if(!tr456_read_element_index(type,indices,pos,&vertex)) continue;
      vertex=(GLuint)((GLint)vertex+base_vertex);
    } else {
      vertex=(GLuint)((has_first ? first : 0)+pos);
    }
    GLfloat local[3];
    GLfloat world[3];
    if(!tr456_read_coord_vertex(&attr,vertex,local)) continue;
    transform_model_point_to_world((const GLfloat (*)[4])row,
      (const GLfloat (*)[4])view,local,world);
    for(int k=0;k<3;k++) {
      if(world[k]<minv[k]) minv[k]=world[k];
      if(world[k]>maxv[k]) maxv[k]=world[k];
    }
    got++;
  }

  bind_buffer(GL_ARRAY_BUFFER,(GLuint)old_array);
  if(got>=3)
    tr456_record_synthetic_bounds(minv,maxv,count);
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
  if(!gl || !gl->get_uniform_location)
    return 0;

  GLint loc=gl->get_uniform_location(g_current_program,"sTex0_wrap");
  if(loc<0)
    loc=gl->get_uniform_location(g_current_program,"sTex0");
  if(loc<0) return 0;

  GLint unit=0;
  if(!shadow_read_uniform_int(g_current_program,loc,&unit)) {
    if(!gl->get_uniform_iv) return 0;
    gl->get_uniform_iv(g_current_program,loc,&unit);
  }
  if(unit<0 || unit>31) return 0;

  GLuint cached=0;
  if(shadow_get_bound_texture(GL_TEXTURE_2D_ARRAY,unit,&cached))
    return cached;

  if(!gl->get_integer || !gl->active_texture) return 0;
  GLint old_active=GL_TEXTURE0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,(GLenum)(GL_TEXTURE0+unit));
  GLint tex=0;
  gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&tex);
  if(tex>0)
    shadow_note_bind_texture(GL_TEXTURE_2D_ARRAY,(GLuint)tex);
  if(old_active)
    shadow_call_active_texture(gl,(GLenum)old_active);
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
  {262144,0x493192825E6F7160ULL,349440,0x8D0D07DD23E1BF91ULL,"3/TEX/227.DDS"},
  {262144,0xFFCCABEC89B2B59AULL,349440,0x8981CD297A33039BULL,"3/TEX/1317.DDS"},
  {262144,0x6C2C5506E7429365ULL,349440,0x19EA21A6029DA8A2ULL,"3/TEX/1318.DDS"},
  {262144,0xDEEEF2963F04984BULL,349440,0x51EEBE9D7CE06868ULL,"3/TEX/1319.DDS"},
  {262144,0xB21820F721312BBFULL,349440,0xCFB6FA688D1D546AULL,"3/TEX/1320.DDS"},
  {262144,0x7D69C41F6D39A0B7ULL,349440,0xDF19A8BE89308D55ULL,"3/TEX/1321.DDS"},
  {262144,0x6F7D167CA7217E86ULL,349440,0x31F4858034AE8329ULL,"3/TEX/1322.DDS"},
  {262144,0x085DABA69E4E5C78ULL,349440,0xFCE33EE92387EDCDULL,"3/TEX/1323.DDS"},
  {262144,0xF0743510F636AB69ULL,349440,0xFCF85836A7F0254DULL,"3/TEX/1324.DDS"},
  {262144,0x7DDDA576814CD4A0ULL,349440,0xCBDE7C53572647D8ULL,"3/TEX/1325.DDS"},
  {262144,0x752C0DBDA1DE3EDEULL,349440,0xEBCA6FCB79300666ULL,"3/TEX/1326.DDS"},
  {262144,0xA437F3B6D12D1F00ULL,349440,0x5828801F44F2E7F0ULL,"3/TEX/1327.DDS"},
  {262144,0x4A06B004813D0780ULL,349440,0x55D9F42CA06AF870ULL,"3/TEX/1328.DDS"},
  {262144,0xB5878783B6A9C711ULL,349440,0xA8E664551166BC4CULL,"3/TEX/1329.DDS"},
  {262144,0x570ECC212541C2ABULL,349440,0x9422B9777927B897ULL,"3/TEX/1330.DDS"},
  {262144,0xA64A4498EE319A0DULL,349440,0xA6401081F034D373ULL,"3/TEX/1331.DDS"},
  {262144,0xD4E930689D9ACBC1ULL,349440,0x0B9C9F3B6BE2DA77ULL,"3/TEX/1332.DDS"},
  {262144,0x32FA9E1BA4768978ULL,349440,0x9649C7F64693CA47ULL,"3/TEX/1333.DDS"},
  {262144,0x6C204AAC23C9B4C5ULL,349440,0x5DF1F38960A532FDULL,"3/TEX/1334.DDS"},
  {262144,0x867D571DB1CB556EULL,349440,0xD62C4B4AE134EBB1ULL,"3/TEX/1335.DDS"},
  {262144,0x2E6FCD6C3FA0586EULL,349440,0x86DF83C8D2C98B3DULL,"3/TEX/1336.DDS"}
};

static int g_flow_texture_signatures_loaded;

static uint64_t fnv1a64_update(uint64_t h, const void *data, size_t size) {
  const unsigned char *p=(const unsigned char*)data;
  if(!p) return h;
  for(size_t i=0;i<size;i++) {
    h^=(uint64_t)p[i];
    h*=1099511628211ULL;
  }
  return h;
}

static uint64_t fnv1a64_bytes(const void *data, size_t size) {
  return fnv1a64_update(14695981039346656037ULL,data,size);
}

static void hash64_parts(uint64_t hash, unsigned int *hi, unsigned int *lo) {
  if(hi) *hi=(unsigned int)(hash>>32);
  if(lo) *lo=(unsigned int)(hash&0xFFFFFFFFu);
}

static int flow_upload_stride_interesting(size_t stride) {
  return stride==262144u || stride==349440u || stride==1048576u;
}

static size_t uncompressed_upload_size_3d(GLsizei width, GLsizei height,
                                          GLsizei depth, GLenum format,
                                          GLenum type) {
  size_t channels=0;
  if(format==GL_RGBA || format==GL_BGRA) channels=4;
  else if(format==GL_RGB || format==GL_BGR) channels=3;
  if(!channels || type!=GL_UNSIGNED_BYTE || width<=0 || height<=0 ||
     depth<=0)
    return 0;
  return (size_t)width*(size_t)height*(size_t)depth*channels;
}

static void log_flow_texture_upload_probe(const char *kind, GLuint texture,
                                          GLint level, GLint zoffset,
                                          GLsizei depth, GLsizei width,
                                          GLsizei height, GLenum format,
                                          GLenum type, size_t total_size,
                                          const void *data) {
  if(!runtime_verbose_log())
    return;
  if(!texture || !data || level!=0 || !total_size ||
     g_flow_texture_upload_probe_logged>=96u)
    return;
  if(total_size>64u*1024u*1024u)
    return;

  const size_t strides[3]={349440u,262144u,1048576u};
  const unsigned char *bytes=(const unsigned char*)data;
  int logged_chunk=0;
  for(size_t si=0;si<sizeof(strides)/sizeof(strides[0]);si++) {
    const size_t stride=strides[si];
    if(!flow_upload_stride_interesting(stride) || total_size<stride ||
       (total_size%stride)!=0)
      continue;
    const size_t chunks=total_size/stride;
    if(depth>0 && chunks!=(size_t)depth)
      continue;
    for(size_t chunk=0;chunk<chunks &&
        g_flow_texture_upload_probe_logged<96u;chunk++) {
      const size_t hash_size=stride<262144u ? stride : 262144u;
      const uint64_t h=fnv1a64_bytes(bytes+chunk*stride,hash_size);
      unsigned int hi=0,lo=0;
      hash64_parts(h,&hi,&lo);
      char msg[320];
      snprintf(msg,sizeof(msg),
        "flow texture upload probe kind=%s tex=%u level=%d layer=%d bytes=%u stride=%u head=%u hash=%08X%08X size=%dx%d fmt=0x%X type=0x%X",
        kind ? kind : "upload",texture,(int)level,
        (int)(zoffset+(GLint)chunk),(unsigned int)total_size,
        (unsigned int)stride,(unsigned int)hash_size,hi,lo,
        (int)width,(int)height,(unsigned int)format,(unsigned int)type);
      log_line(msg);
      g_flow_texture_upload_probe_logged++;
      logged_chunk=1;
    }
    if(logged_chunk)
      return;
  }

  if(g_flow_texture_upload_probe_logged>=96u)
    return;
  if(total_size<262144u && !(width==512 && height==512))
    return;
  const size_t hash_size=total_size<262144u ? total_size : 262144u;
  const uint64_t h=fnv1a64_bytes(data,hash_size);
  unsigned int hi=0,lo=0;
  hash64_parts(h,&hi,&lo);
  char msg[320];
  snprintf(msg,sizeof(msg),
    "flow texture upload probe kind=%s tex=%u level=%d layer=%d depth=%d bytes=%u head=%u hash=%08X%08X size=%dx%d fmt=0x%X type=0x%X",
    kind ? kind : "upload",texture,(int)level,(int)zoffset,(int)depth,
    (unsigned int)total_size,(unsigned int)hash_size,hi,lo,
    (int)width,(int)height,(unsigned int)format,(unsigned int)type);
  log_line(msg);
  g_flow_texture_upload_probe_logged++;
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
  load_runtime_config();
  if(!g_runtime_refresh_flow_texture_signatures) return;
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
  GLsizei cached_size[4]={0,0,0,0};
  uint64_t cached_hash[4]={0,0,0,0};
  int cached_count=0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    GLsizei sizes[2]={s->base_size,s->payload_size};
    uint64_t hashes[2]={s->base_hash,s->payload_hash};
    for(int j=0;j<2;j++) {
      GLsizei size=sizes[j];
      if(size<=0 || image_size<size) continue;
      int cache_index=-1;
      for(int k=0;k<cached_count;k++) {
        if(cached_size[k]==size) {
          cache_index=k;
          break;
        }
      }
      if(cache_index<0) {
        if(cached_count>=4) continue;
        cache_index=cached_count++;
        cached_size[cache_index]=size;
        cached_hash[cache_index]=fnv1a64_bytes(data,(size_t)size);
      }
      if(cached_hash[cache_index]==hashes[j])
        return s->name;
    }
  }
  return 0;
}

static GLuint current_bound_texture_for_target(GLenum target) {
  CaptureGL *gl=capture_gl();
  int unit=shadow_active_unit_index();
  GLuint cached=0;
  if(unit>=0 && shadow_get_bound_texture(target,unit,&cached))
    return cached;
  if(!gl || !gl->get_integer) return 0;
  GLint binding=0;
  if(target==GL_TEXTURE_2D_ARRAY)
    gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&binding);
  else if(target==GL_TEXTURE_2D)
    gl->get_integer(GL_TEXTURE_BINDING_2D,&binding);
  if(target==GL_TEXTURE_2D_ARRAY || target==GL_TEXTURE_2D)
    shadow_note_bind_texture(target,binding>0 ? (GLuint)binding : 0u);
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
  uint64_t h=0;
  int hashed=0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    if(s->base_size==image_size) {
      if(!hashed) {
        h=fnv1a64_bytes(data,(size_t)image_size);
        hashed=1;
      }
      if(h==s->base_hash)
        return s->name;
    }
    if(s->payload_size==image_size) {
      if(!hashed) {
        h=fnv1a64_bytes(data,(size_t)image_size);
        hashed=1;
      }
      if(h==s->payload_hash)
        return s->name;
    }
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
    const unsigned char *p=(const unsigned char*)data;
    GLsizei strides[8];
    int stride_count=0;
    for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
        sizeof(g_flow_texture_signatures[0]);i++) {
      const FlowTextureSignature *s=&g_flow_texture_signatures[i];
      GLsizei candidates[2]={s->payload_size,s->base_size};
      for(int ci=0;ci<2 && stride_count<8;ci++) {
        GLsizei candidate=candidates[ci];
        int known=0;
        if(candidate<=0) continue;
        for(int si=0;si<stride_count;si++) {
          if(strides[si]==candidate) {
            known=1;
            break;
          }
        }
        if(!known)
          strides[stride_count++]=candidate;
      }
    }
    for(int si=0;si<stride_count;si++) {
      GLsizei stride=strides[si];
      if(image_size<stride || (image_size%stride)!=0)
        continue;
      int chunks=image_size/stride;
      for(int chunk=0;chunk<chunks;chunk++) {
        const unsigned char *chunk_data=p+(size_t)chunk*(size_t)stride;
        uint64_t full_hash=fnv1a64_bytes(chunk_data,(size_t)stride);
        uint64_t base_hash=0;
        int have_base=0;
        for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
            sizeof(g_flow_texture_signatures[0]);i++) {
          const FlowTextureSignature *s=&g_flow_texture_signatures[i];
          int matched=0;
          if(stride==s->payload_size && full_hash==s->payload_hash)
            matched=1;
          if(!matched && stride==s->base_size && full_hash==s->base_hash)
            matched=1;
          if(!matched && stride==s->payload_size && s->base_size>0 &&
             s->base_size<=stride) {
            if(!have_base) {
              base_hash=fnv1a64_bytes(chunk_data,(size_t)s->base_size);
              have_base=1;
            }
            matched=base_hash==s->base_hash;
          }
          if(matched) {
            GLint layer=zoffset;
            if(chunks==depth)
              layer=zoffset+chunk;
            remember_flow_surface_texture_layer(texture,layer,s->name);
          }
        }
      }
    }
    if(!flow_surface_texture_known(texture)) {
      uint64_t base_hash=0;
      int have_base=0;
      for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
          sizeof(g_flow_texture_signatures[0]);i++) {
        const FlowTextureSignature *s=&g_flow_texture_signatures[i];
        if(s->base_size>0 && image_size>s->base_size) {
          if(!have_base) {
            base_hash=fnv1a64_bytes(data,(size_t)s->base_size);
            have_base=1;
          }
          if(base_hash==s->base_hash)
            remember_flow_surface_texture_layer(texture,zoffset,s->name);
        }
      }
    }
    if(!flow_surface_texture_known(texture) &&
       !flow_surface_texture_has_precise_layers(texture))
      log_flow_texture_upload_probe("compressed",texture,level,zoffset,depth,
        0,0,0,0,(size_t)image_size,data);
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
    else if(target==GL_TEXTURE_2D_ARRAY && level==0 && image_size>0)
      log_flow_texture_upload_probe("compressed",texture,level,zoffset,depth,
        0,0,0,0,(size_t)image_size,data);
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

static void note_uncompressed_texture_upload_for_texture(GLuint texture,
                                                         GLenum target,
                                                         GLint level,
                                                         GLint zoffset,
                                                         GLsizei width,
                                                         GLsizei height,
                                                         GLsizei depth,
                                                         GLenum format,
                                                         GLenum type,
                                                         const void *data) {
  if(target!=GL_TEXTURE_2D_ARRAY || level!=0 || !texture || !data)
    return;
  const size_t total=uncompressed_upload_size_3d(width,height,depth,format,
    type);
  if(!total)
    return;
  log_flow_texture_upload_probe("uncompressed",texture,level,zoffset,depth,
    width,height,format,type,total,data);
}

static void note_uncompressed_texture_upload(GLenum target, GLint level,
                                             GLint zoffset, GLsizei width,
                                             GLsizei height, GLsizei depth,
                                             GLenum format, GLenum type,
                                             const void *data) {
  note_uncompressed_texture_upload_for_texture(
    current_bound_texture_for_target(target),target,level,zoffset,width,
    height,depth,format,type,data);
}

static int current_water_attrib_locations(GLint locs[4]);

static int flow_params_are_authored_surface(const GLfloat params[4]) {
  return params && fabsf(params[2])<0.00030f && fabsf(params[3])>=40.0f;
}

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
  const int precise_layers=flow_surface_texture_has_precise_layers(texture);
  if(mixed_out && precise_layers)
    *mixed_out=1;
  if(precise_layers)
    return 0;
  return flow_surface_texture_known(texture);
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
  load_runtime_config();
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
      int texture_match=
        current_flow_draw_matches_texture_layers(tex,count,count_known,
          &texture_layer,&mixed_layers);
      if(texture_match) {
        flow_profile_set_surface(&g_flow_profile_cache_profile,
          "flow surface texture");
      } else if(g_runtime_flow_texture_fallback &&
                flow_params_are_authored_surface(local_params)) {
        flow_profile_set_surface(&g_flow_profile_cache_profile,
          "flow material fallback");
        texture_match=2;
      } else {
        flow_profile_set_original(&g_flow_profile_cache_profile,
          "non-flow texture");
      }
      g_flow_profile_cache_profile.texture_object=tex;
      g_flow_profile_cache_profile.texture_checked=1;
      g_flow_profile_cache_profile.texture_match=texture_match;
      g_flow_profile_cache_profile.texture_layer=texture_layer;
      g_flow_profile_cache_profile.texture_mixed_layers=mixed_layers;
      if(texture_match && runtime_verbose_log()) {
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
      if(!texture_match && runtime_verbose_log() &&
         g_flow_material_bypass_logged<32u) {
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
  shadow_call_uniform_4f(gl,p->material_profile_loc,
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
  (void)count;
  (void)count_known;
  if(!profile || !profile->texture_match) {
    if(reason_out) *reason_out=profile && profile->name ?
      profile->name : "flow texture";
    return 0;
  }

  const int authored_surface_wave=flow_params_are_authored_surface(params);
  const int allowlisted=authored_surface_wave && profile->texture_match;

  if(!allowlisted && reason_out) {
    *reason_out=!authored_surface_wave ? "flow material" : "unknown";
  }
  return allowlisted;
}

static int current_draw_is_synthetic_flow_candidate_raw(GLenum mode, GLsizei count,
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
  if(!allowlisted && runtime_verbose_log() && g_flow_surface_gate_logged<48u) {
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

static int current_draw_is_synthetic_standing_candidate_raw(GLenum mode, GLsizei count,
                                                            int count_known) {
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_shader_patching) return 0;
  if(count_known && (count<3 || count>262144)) return 0;
  if(mode!=GL_TRIANGLES && mode!=GL_TRIANGLE_STRIP && mode!=GL_TRIANGLE_FAN)
    return 0;
  if(g_current_program_type!=SHADER_WATER_SURFACE &&
     g_current_program_type!=SHADER_WATER_REFLECT)
    return 0;

  GLfloat model3[4];
  int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
  if(g_current_program_type==SHADER_WATER_SURFACE) {
    GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
    const int has_params=read_uniform_vec4_now("uParams",params);
    if(!has_model3 || !has_params || fabsf(params[3])<=0.0001f)
      return 0;
    return 1;
  }
  if(!g_runtime_synthetic_reflect_surface) return 0;
  if(!count_known) return 0;
  if(mode!=GL_TRIANGLES || count>2048 || (count%3)!=0) return 0;
  return has_model3;
}

static const SyntheticDrawDecision *current_synthetic_draw_decision(GLenum mode,
                                                                    GLsizei count,
                                                                    int count_known) {
  const unsigned int serial=current_draw_serial();
  SyntheticDrawDecision *d=&g_synthetic_draw_decision_cache;
  if(d->valid &&
     d->frame==g_frame_index &&
     d->serial==serial &&
     d->program==g_current_program &&
     d->program_type==g_current_program_type &&
     d->mode==mode &&
     d->count==count &&
     d->count_known==count_known) {
    perf_note_decision(1,d,0);
    return d;
  }

  unsigned long long t0=perf_ticks_now();
  memset(d,0,sizeof(*d));
  d->valid=1;
  d->frame=g_frame_index;
  d->serial=serial;
  d->program=g_current_program;
  d->program_type=g_current_program_type;
  d->mode=mode;
  d->count=count;
  d->count_known=count_known;
  d->synthetic_standing=
    current_draw_is_synthetic_standing_candidate_raw(mode,count,count_known);
  d->synthetic_flow=
    current_draw_is_synthetic_flow_candidate_raw(mode,count,count_known);
  d->synthetic_surface=d->synthetic_standing || d->synthetic_flow;
  d->synthetic_ready=d->synthetic_surface &&
    ensure_synthetic_surface_program();
  load_runtime_config();
  if(g_runtime_synthetic_surface && g_runtime_synthetic_standing_only &&
     g_runtime_shader_patching && g_synthetic_surface.ready) {
    if((g_current_program_type==SHADER_WATER_SURFACE ||
        g_current_program_type==SHADER_WATER_REFLECT) &&
       d->synthetic_standing)
      d->skip_original=1;
    else if(g_current_program_type==SHADER_WATER_SSR)
      d->skip_original=1;
  }
  d->capture_reason=d->synthetic_flow ? "synthetic flow surface" :
    "synthetic water surface";
  perf_note_decision(0,d,perf_ticks_now()-t0);
  return d;
}

static int current_draw_is_synthetic_flow_candidate(GLenum mode, GLsizei count,
                                                    int count_known) {
  return current_synthetic_draw_decision(mode,count,count_known)->synthetic_flow;
}

static int current_draw_is_synthetic_surface_candidate(GLenum mode, GLsizei count,
                                                       int count_known) {
  return current_synthetic_draw_decision(mode,count,count_known)->synthetic_surface;
}

static int current_draw_should_skip_original_for_synthetic_ready(GLenum mode,
                                                                 GLsizei count,
                                                                 int count_known,
                                                                 int synthetic_ready,
                                                                 int synthetic_flow) {
  (void)synthetic_ready;
  (void)synthetic_flow;
  return current_synthetic_draw_decision(mode,count,count_known)->skip_original;
}

static void prepare_program_frame_draw_state(ProgramTrack *p) {
  if(!p) return;
  if(p->frame_draw_frame!=g_frame_index) {
    p->frame_draw_frame=g_frame_index;
    p->frame_draw_count=0;
    p->frame_last_mode=0;
    p->frame_last_count=-2147483647;
    p->current_duplicate_pass=0;
  }
}

static void note_draw(const char *name, GLenum mode, GLsizei count) {
  poll_effect_hotkeys();
  diag_poll_insert(name);
  tr456_wet_lara_note_draw(name,count,
    count>=0 && !(name && strncmp(name,"glMultiDraw",11)==0));
  const int tracked_water=is_tracked_water_shader_type(g_current_program_type);
  perf_note_draw(tracked_water);
  ProgramTrack *p=program_track(g_current_program,
    tracked_water || runtime_verbose_log());
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(g_current_program);
  if(tracked_water) {
    apply_effect_toggles(g_current_program);
    apply_contact_cache(g_current_program);
    update_draw_info_uniform(mode,count);
    update_flow_material_profile_uniform(mode,count,count>=0);
    if(runtime_verbose_log() &&
       g_current_program_type>0 && g_current_program_type<6 &&
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
  if(g_current_program_type==SHADER_WATER_RIPPLE) {
    const int ripple_contact_candidate=is_water_ripple_draw_count(count);
    unsigned long long perf_contact_t0=perf_ticks_now();
    record_ripple_contact_from_program(count);
    perf_note_ripple_contact(ripple_contact_candidate,
      perf_ticks_now()-perf_contact_t0);
    diag_log_cpu_contact_state(name);
  }
  if(tracked_water)
    diag_log_contacts(g_current_program,name);
  if(p) {
    prepare_program_frame_draw_state(p);
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
      shadow_get_integer_or_gl(GL_BLEND,&blend);
      shadow_get_integer_or_gl(GL_DEPTH_TEST,&depth);
      shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,&depth_mask);
      shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&blend_src_rgb);
      shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&blend_dst_rgb);
      shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&blend_src_alpha);
      shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&blend_dst_alpha);
    }
    GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
    GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
    GLfloat draw_info[4]={0.0f,0.0f,0.0f,0.0f};
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
  tr456_wet_lara_end_frame();
  g_scene_captured=0;
  perf_note_frame();
  g_frame_index++;
  if(diag_is_active() && (g_frame_index%120u)==0u)
    perf_log_summary("diag-window",0);
  if(g_diag_active_frames>0) {
    g_diag_active_frames--;
    if(!g_diag_active_frames) {
      perf_log_summary("diag-end",1);
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

static void prepare_scene_capture_internal(const char *reason,
                                           int allow_unknown_program) {
  unsigned long long perf_t0=perf_ticks_now();
  int perf_capture_updated=0;
  int perf_capture_resized=0;
  if(!allow_unknown_program && !g_current_program_type)
    goto perf_done;

  load_runtime_config();
  if(!g_runtime_fbo_reflection)
    goto perf_done;

  CaptureGL *gl=capture_gl();
  if(!gl->ok) {
    if(!g_logged_capture)
      log_line("framebuffer reflection capture skipped: missing required GL entry point");
    goto perf_done;
  }

  GLint viewport[4]={0,0,0,0};
  shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) goto perf_done;
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

  GLint old_active=GL_TEXTURE0;
  GLint old_scene_texture_2d=0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,GL_TEXTURE15);
  shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,&old_scene_texture_2d);

  if(!g_scene_tex) {
    gl->gen_textures(1,&g_scene_tex);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
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
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
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
    perf_capture_resized=1;
    g_scene_has_pixels=0;
  }

  GLenum err=0;
  if(!g_scene_captured) {
    int capture_interval=g_runtime_fbo_capture_interval;
    if(capture_interval<1) capture_interval=1;
    int capture_now=resized || !g_scene_has_pixels ||
      capture_interval<=1 ||
      (g_frame_index%(unsigned int)capture_interval)==0u;
    if(capture_now) {
      if(scale>1) {
        GLint old_read_fbo=0;
        GLint old_draw_fbo=0;
        if(!g_scene_fbo)
          gl->gen_framebuffers(1,&g_scene_fbo);
        if(g_scene_fbo) {
          shadow_get_integer_or_gl(GL_READ_FRAMEBUFFER_BINDING,&old_read_fbo);
          shadow_get_integer_or_gl(GL_DRAW_FRAMEBUFFER_BINDING,&old_draw_fbo);
          shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,g_scene_fbo);
          gl->framebuffer_texture_2d(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,g_scene_tex,0);
          GLenum status=gl->check_framebuffer_status(GL_DRAW_FRAMEBUFFER);
          if(status==GL_FRAMEBUFFER_COMPLETE) {
            shadow_call_bind_framebuffer(gl,GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
            gl->blit_framebuffer(viewport[0],viewport[1],
              viewport[0]+viewport[2],viewport[1]+viewport[3],
              0,0,capture_w,capture_h,GL_COLOR_BUFFER_BIT,GL_LINEAR);
            err=gl->get_error ? gl->get_error() : 0;
          } else {
            err=status ? status : 1u;
          }
          shadow_call_bind_framebuffer(gl,GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
          shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,(GLuint)old_draw_fbo);
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
          shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
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
      perf_capture_updated=1;
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
        shadow_call_uniform_1i(gl,track->scene_loc,15);
      if(track->info_loc==-2)
        track->info_loc=gl->get_uniform_location(g_current_program,"uTrWaterCaptureInfo");
      if(track->info_loc>=0)
        shadow_call_uniform_4f(gl,track->info_loc,1.0f/(GLfloat)viewport[2],1.0f/(GLfloat)viewport[3],
          (GLfloat)viewport[2],(GLfloat)viewport[3]);
      track->uniform_frame=g_frame_index;
      track->uniform_w=viewport[2];
      track->uniform_h=viewport[3];
    }
  }

  shadow_call_bind_texture(gl,GL_TEXTURE_2D,(GLuint)old_scene_texture_2d);
  shadow_call_active_texture(gl,(GLenum)old_active);

perf_done:
  perf_note_capture(perf_capture_updated,perf_capture_resized,
    perf_ticks_now()-perf_t0);
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
  char *src=0;
  int type=0;
  const ShaderSourcePatch *patch=find_source_patch_sources(count,strings,lengths,src_hash);
  if(patch)
    type=patch->type;
  const int need_src=runtime_verbose_log() || TR456_DIAG_BUILD;
  if(need_src)
    src=join_sources(count,strings,lengths);
  set_shader_info(shader,type,src_hash,src_len,src);
  if(type)
    set_shader_type(shader,type);
#if TR456_DIAG_BUILD
  {
    LONG n=InterlockedIncrement(&g_diag_shader_source_count);
    if(n<=180) {
      ShaderTrack *tracked=shader_track(shader,0);
      diag_logf("DIAG glShaderSource #%ld shader=%u hash=0x%08X len=%u type=%s patch=%s replacement=%d preview=\"%s\"",
        (long)n,shader,(unsigned int)src_hash,src_len,
        shader_type_name(type),patch ? patch->label : "none",
        0,
        tracked ? tracked->preview : "");
    }
  }
#endif
  if(patch && type!=SHADER_WATER_RIPPLE && runtime_verbose_log()) {
    char msg[176];
    snprintf(msg,sizeof(msg),"tracked original %s shader=%u hash=0x%08X len=%u",
      shader_type_name(type),shader,(unsigned int)src_hash,src_len);
    log_line(msg);
  }
  real(shader,count,strings,lengths);
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

static const char *trshader_gl_string(GLenum name) {
  PFNGLGETSTRING get=real_get_string();
  const unsigned char *text=get ? get(name) : 0;
  return text ? (const char*)text : "";
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
    if(vs) free(vs);
    if(fs) free(fs);
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
}

static int trshader_try_load_synthetic_program_binary(SyntheticSurfacePass *s,
                                                      CaptureGL *gl,
                                                      const GLint locs[4]) {
  if(!g_runtime_shader_binary_cache) return 0;
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
  if(!g_runtime_shader_binary_cache) return;
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
  CaptureGL *gl=capture_gl();
  if(!create_program || !attach || !link || !bind_attr || !gl ||
     !gl->get_uniform_location) {
    log_line("synthetic water surface disabled: missing GL program entry point");
    s->failed=1;
    return 0;
  }

  if(!s->tried) {
    GLint locs[4]={-1,-1,-1,-1};
    if(!current_water_attrib_locations(locs)) {
      log_line("synthetic water surface disabled: candidate program has no aCoord location");
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
    return 0;
  }

  if(s->compile_step_frame==g_frame_index)
    return 0;
  s->compile_step_frame=g_frame_index;

  if(s->compile_stage==1) {
    s->pending_vs=compile_water_shader(GL_VERTEX_SHADER,
      "synthetic vertex",synthetic_surface_vertex_shader);
    if(!s->pending_vs) {
      s->failed=1;
      return 0;
    }
    s->compile_stage=2;
    return 0;
  }

  if(s->compile_stage==2) {
    s->pending_fs=compile_water_shader(GL_FRAGMENT_SHADER,
      "synthetic fragment",synthetic_surface_shader);
    if(!s->pending_fs) {
      PFNGLDELETESHADER del=real_delete_shader();
      if(del && s->pending_vs) del(s->pending_vs);
      s->pending_vs=0;
      s->failed=1;
      return 0;
    }
    s->compile_stage=3;
    return 0;
  }

  if(s->compile_stage!=3 || !s->pending_vs || !s->pending_fs) {
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
    s->failed=1;
    return 0;
  }

  static const char *names[4]={"aCoord","aNormal","aLight","aColor"};
  GLint locs[4]={s->attr_coord,s->attr_normal,s->attr_light,s->attr_color};
  for(int i=0;i<4;i++)
    if(locs[i]>=0) bind_attr(program,(GLuint)locs[i],names[i]);
  PFNGLPROGRAMPARAMETERI program_parameter_i=real_program_parameter_i();
  if(g_runtime_shader_binary_cache && program_parameter_i)
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

static PFNGLUSEPROGRAM real_use_program(void) {
  static PFNGLUSEPROGRAM p;
  if(!p) p=(PFNGLUSEPROGRAM)gl_proc("glUseProgram");
  return p;
}

static void APIENTRY hook_glUseProgram(GLuint program) {
  PFNGLUSEPROGRAM real=real_use_program();
  if(real) real(program);
  shadow_note_use_program(program);
  g_current_program=program;
  g_current_program_type=program_type(program);
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(program);
  if(is_tracked_water_shader_type(g_current_program_type)) {
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
}

static void APIENTRY hook_glActiveTexture(GLenum texture) {
  PFNGLACTIVETEXTURE real=real_active_texture();
  if(real) real(texture);
  shadow_note_active_texture(texture);
}

static void APIENTRY hook_glBindTexture(GLenum target, GLuint texture) {
  PFNGLBINDTEXTURE real=real_bind_texture();
  if(real) real(target,texture);
  shadow_note_bind_texture(target,texture);
}

static void APIENTRY hook_glBindFramebuffer(GLenum target, GLuint framebuffer) {
  PFNGLBINDFRAMEBUFFER real=real_bind_framebuffer();
  if(real) real(target,framebuffer);
  shadow_note_bind_framebuffer(target,framebuffer);
}

static void APIENTRY hook_glViewport(GLint x, GLint y, GLsizei width,
                                     GLsizei height) {
  PFNGLVIEWPORT real=real_viewport();
  if(real) real(x,y,width,height);
  shadow_note_viewport(x,y,width,height);
}

static void APIENTRY hook_glEnable(GLenum cap) {
  PFNGLENABLE real=real_enable();
  if(real) real(cap);
  shadow_note_enable(cap,1);
}

static void APIENTRY hook_glDisable(GLenum cap) {
  PFNGLDISABLE real=real_disable();
  if(real) real(cap);
  shadow_note_enable(cap,0);
}

static void APIENTRY hook_glDepthMask(GLboolean flag) {
  PFNGLDEPTHMASK real=real_depth_mask();
  if(real) real(flag);
  shadow_note_depth_mask(flag);
}

static void APIENTRY hook_glDepthFunc(GLenum func) {
  PFNGLDEPTHFUNC real=real_depth_func();
  if(real) real(func);
  shadow_note_depth_func(func);
}

static void APIENTRY hook_glBlendFunc(GLenum sfactor, GLenum dfactor) {
  PFNGLBLENDFUNC real=real_blend_func();
  if(real) real(sfactor,dfactor);
  shadow_note_blend_func(sfactor,dfactor,sfactor,dfactor);
}

static void APIENTRY hook_glBlendFuncSeparate(GLenum src_rgb, GLenum dst_rgb,
                                              GLenum src_alpha,
                                              GLenum dst_alpha) {
  PFNGLBLENDFUNCSEPARATE real=real_blend_func_separate();
  if(real) real(src_rgb,dst_rgb,src_alpha,dst_alpha);
  shadow_note_blend_func(src_rgb,dst_rgb,src_alpha,dst_alpha);
}

static void APIENTRY hook_glUniform1i(GLint location, GLint v0) {
  PFNGLUNIFORM1I real=real_uniform_1i();
  if(real) real(location,v0);
  shadow_note_uniform_1i(location,v0);
}

static void APIENTRY hook_glUniform1iv(GLint location, GLsizei count,
                                       const GLint *value) {
  PFNGLUNIFORM1IV real=real_uniform_1iv();
  if(real) real(location,count,value);
  shadow_note_uniform_1iv(location,count,value);
}

static void APIENTRY hook_glUniform4f(GLint location, GLfloat v0, GLfloat v1,
                                      GLfloat v2, GLfloat v3) {
  PFNGLUNIFORM4F real=real_uniform_4f();
  if(real) real(location,v0,v1,v2,v3);
  shadow_note_uniform_4f(location,v0,v1,v2,v3);
}

static void APIENTRY hook_glUniform4fv(GLint location, GLsizei count,
                                       const GLfloat *value) {
  PFNGLUNIFORM4FV real=real_uniform_4fv();
  if(real) real(location,count,value);
  shadow_note_uniform_4fv(location,count,value);
}

static void APIENTRY hook_glUniformMatrix4fv(GLint location, GLsizei count,
                                             GLboolean transpose,
                                             const GLfloat *value) {
  PFNGLUNIFORMMATRIX4FV real=real_uniform_matrix_4fv();
  if(real) real(location,count,transpose,value);
  shadow_note_uniform_matrix4fv(location,count,transpose,value);
}

__declspec(dllexport) void APIENTRY glBindTexture(GLenum target,
                                                  GLuint texture) {
  hook_glBindTexture(target,texture);
}

__declspec(dllexport) void APIENTRY glViewport(GLint x, GLint y,
                                               GLsizei width,
                                               GLsizei height) {
  hook_glViewport(x,y,width,height);
}

__declspec(dllexport) void APIENTRY glEnable(GLenum cap) {
  hook_glEnable(cap);
}

__declspec(dllexport) void APIENTRY glDisable(GLenum cap) {
  hook_glDisable(cap);
}

__declspec(dllexport) void APIENTRY glDepthMask(GLboolean flag) {
  hook_glDepthMask(flag);
}

__declspec(dllexport) void APIENTRY glDepthFunc(GLenum func) {
  hook_glDepthFunc(func);
}

__declspec(dllexport) void APIENTRY glBlendFunc(GLenum sfactor,
                                                GLenum dfactor) {
  hook_glBlendFunc(sfactor,dfactor);
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

static PFNGLTEXIMAGE3D real_tex_image_3d(void) {
  static PFNGLTEXIMAGE3D p;
  if(!p) p=(PFNGLTEXIMAGE3D)gl_proc("glTexImage3D");
  return p;
}

static PFNGLTEXSUBIMAGE3D real_tex_sub_image_3d(void) {
  static PFNGLTEXSUBIMAGE3D p;
  if(!p) p=(PFNGLTEXSUBIMAGE3D)gl_proc("glTexSubImage3D");
  return p;
}

static PFNGLTEXTURESUBIMAGE3D real_texture_sub_image_3d(void) {
  static PFNGLTEXTURESUBIMAGE3D p;
  if(!p) p=(PFNGLTEXTURESUBIMAGE3D)gl_proc("glTextureSubImage3D");
  return p;
}

static PFNGLTEXTUREIMAGE3DEXT real_texture_image_3d_ext(void) {
  static PFNGLTEXTUREIMAGE3DEXT p;
  if(!p) p=(PFNGLTEXTUREIMAGE3DEXT)gl_proc("glTextureImage3DEXT");
  return p;
}

static PFNGLTEXTURESUBIMAGE3DEXT real_texture_sub_image_3d_ext(void) {
  static PFNGLTEXTURESUBIMAGE3DEXT p;
  if(!p) p=(PFNGLTEXTURESUBIMAGE3DEXT)gl_proc("glTextureSubImage3DEXT");
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

static void APIENTRY hook_glTexImage3D(GLenum target, GLint level,
    GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLenum format, GLenum type, const void *data) {
  PFNGLTEXIMAGE3D real=real_tex_image_3d();
  note_uncompressed_texture_upload(target,level,0,width,height,depth,format,
    type,data);
  if(real) real(target,level,internalformat,width,height,depth,border,format,
    type,data);
}

static void APIENTRY hook_glTexSubImage3D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXSUBIMAGE3D real=real_tex_sub_image_3d();
  note_uncompressed_texture_upload(target,level,zoffset,width,height,depth,
    format,type,data);
  if(real) real(target,level,xoffset,yoffset,zoffset,width,height,depth,
    format,type,data);
}

static void APIENTRY hook_glTextureSubImage3D(GLuint texture, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTURESUBIMAGE3D real=real_texture_sub_image_3d();
  note_uncompressed_texture_upload_for_texture(texture,GL_TEXTURE_2D_ARRAY,
    level,zoffset,width,height,depth,format,type,data);
  if(real) real(texture,level,xoffset,yoffset,zoffset,width,height,depth,
    format,type,data);
}

static void APIENTRY hook_glTextureImage3DEXT(GLuint texture, GLenum target,
    GLint level, GLint internalformat, GLsizei width, GLsizei height,
    GLsizei depth, GLint border, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTUREIMAGE3DEXT real=real_texture_image_3d_ext();
  note_uncompressed_texture_upload_for_texture(texture,target,level,0,width,
    height,depth,format,type,data);
  if(real) real(texture,target,level,internalformat,width,height,depth,border,
    format,type,data);
}

static void APIENTRY hook_glTextureSubImage3DEXT(GLuint texture, GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTURESUBIMAGE3DEXT real=real_texture_sub_image_3d_ext();
  note_uncompressed_texture_upload_for_texture(texture,target,level,zoffset,
    width,height,depth,format,type,data);
  if(real) real(texture,target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
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

__declspec(dllexport) void APIENTRY glTexImage3D(GLenum target, GLint level,
    GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLenum format, GLenum type, const void *data) {
  hook_glTexImage3D(target,level,internalformat,width,height,depth,border,
    format,type,data);
}

__declspec(dllexport) void APIENTRY glTexSubImage3D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureSubImage3D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureSubImage3D(texture,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint internalformat, GLsizei width,
    GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureImage3DEXT(texture,target,level,internalformat,width,height,
    depth,border,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureSubImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureSubImage3DEXT(texture,target,level,xoffset,yoffset,zoffset,
    width,height,depth,format,type,data);
}

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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface" : "synthetic water surface");
    if(!skip_original) {
      real(mode,count,type,indices);
      tr456_wet_lara_draw_elements("glDrawElements",real,mode,count,type,indices);
    }
    if(synthetic_ready)
      draw_synthetic_surface_elements(mode,count,type,indices);
  }
}

static PFNGLDRAWARRAYS real_draw_arrays(void) {
  static PFNGLDRAWARRAYS p;
  if(!p) p=(PFNGLDRAWARRAYS)gl_proc("glDrawArrays");
  return p;
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

  GLfloat proj[16];
  if(s->loc_proj>=0 && matrix4 && read_current_uniform_matrix4("uProjMatrix",proj)) {
    matrix4(s->loc_proj,1,GL_FALSE,proj);
    shadow_note_uniform_matrix4fv(s->loc_proj,1,GL_FALSE,proj);
  }

  GLfloat model[16];
  if(s->loc_model>=0 && read_current_uniform_vec4_array("uModelMatrix",4,model))
    shadow_call_uniform_4fv(gl,s->loc_model,4,model);

  GLfloat view[16];
  if(s->loc_view>=0 && read_current_uniform_vec4_array("uViewMatrix",4,view))
    shadow_call_uniform_4fv(gl,s->loc_view,4,view);

  if(s->loc_scene>=0 && gl->uniform_1i)
    shadow_call_uniform_1i(gl,s->loc_scene,15);

  if(gl->active_texture && gl->bind_texture && g_scene_tex) {
    GLint old_active=GL_TEXTURE0;
    shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    if(old_active) shadow_call_active_texture(gl,(GLenum)old_active);
  }

  int capture_view_w=g_scene_view_w>0 ? g_scene_view_w : g_scene_w;
  int capture_view_h=g_scene_view_h>0 ? g_scene_view_h : g_scene_h;
  if(capture_view_w<=0) capture_view_w=1920;
  if(capture_view_h<=0) capture_view_h=1080;
  GLfloat inv_w=capture_view_w>0 ? 1.0f/(GLfloat)capture_view_w : 1.0f/1920.0f;
  GLfloat inv_h=capture_view_h>0 ? 1.0f/(GLfloat)capture_view_h : 1.0f/1080.0f;
  if(s->loc_capture_info>=0 && gl->uniform_4f)
    shadow_call_uniform_4f(gl,s->loc_capture_info,inv_w,inv_h,(GLfloat)capture_view_w,(GLfloat)capture_view_h);

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
  perf_note_synthetic_begin(perf_ticks_now()-perf_t0);
}

static void begin_synthetic_surface_state(GLint *old_program, GLint *old_blend,
                                           GLint *old_depth, GLint *old_cull,
                                           GLint *old_depth_mask,
                                           GLint *old_depth_func,
                                           GLint old_blend_func[4]) {
  CaptureGL *gl=capture_gl();
  if(gl && gl->get_integer) {
    shadow_get_integer_or_gl(GL_CURRENT_PROGRAM,old_program);
    shadow_get_integer_or_gl(GL_BLEND,old_blend);
    shadow_get_integer_or_gl(GL_DEPTH_TEST,old_depth);
    shadow_get_integer_or_gl(GL_CULL_FACE,old_cull);
    shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,old_depth_mask);
    shadow_get_integer_or_gl(GL_DEPTH_FUNC,old_depth_func);
    shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&old_blend_func[0]);
    shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&old_blend_func[1]);
    shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&old_blend_func[2]);
    shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&old_blend_func[3]);
  }
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  if(disable) {
    disable(GL_CULL_FACE);
    shadow_note_enable(GL_CULL_FACE,0);
  }
  if(enable) { enable(GL_BLEND); shadow_note_enable(GL_BLEND,1); }
  PFNGLBLENDFUNCSEPARATE blend_func=real_blend_func_separate();
  if(blend_func) {
    blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    shadow_note_blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
  }
  if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); }
  if(depth_func) { depth_func(GL_LEQUAL); shadow_note_depth_func(GL_LEQUAL); }
  if(depth_mask) { depth_mask(GL_FALSE); shadow_note_depth_mask(GL_FALSE); }
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
  if(blend_func) {
    blend_func((GLenum)old_blend_func[0],(GLenum)old_blend_func[1],
      (GLenum)old_blend_func[2],(GLenum)old_blend_func[3]);
    shadow_note_blend_func((GLenum)old_blend_func[0],(GLenum)old_blend_func[1],
      (GLenum)old_blend_func[2],(GLenum)old_blend_func[3]);
  }
  if(depth_mask) { depth_mask((GLboolean)(old_depth_mask ? 1 : 0)); shadow_note_depth_mask((GLboolean)(old_depth_mask ? 1 : 0)); }
  if(depth_func) { depth_func((GLenum)old_depth_func); shadow_note_depth_func((GLenum)old_depth_func); }
  if(old_cull) { if(enable) { enable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,1); } } else { if(disable) { disable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,0); } }
  if(old_depth) { if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); } } else { if(disable) { disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0); } }
  if(old_blend) { if(enable) { enable(GL_BLEND); shadow_note_enable(GL_BLEND,1); } } else { if(disable) { disable(GL_BLEND); shadow_note_enable(GL_BLEND,0); } }
  if(use_program) { use_program((GLuint)old_program); shadow_note_use_program((GLuint)old_program); }
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
  state->old_active_texture=GL_TEXTURE0;
  state->old_scene_texture_2d=0;
  state->old_scene_texture_valid=0;
}

static int begin_synthetic_surface_draw(GLenum mode, GLsizei count, int count_known,
                                        SyntheticSurfaceDrawState *state) {
  if(!state) return 0;
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  if(!decision->synthetic_surface || !decision->synthetic_ready) return 0;
  if(!g_scene_tex || g_scene_w<=0 || g_scene_h<=0) return 0;

  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!use_program) return 0;

  init_synthetic_surface_draw_state(state);
  CaptureGL *gl=capture_gl();
  if(gl && gl->active_texture && gl->bind_texture) {
    shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&state->old_active_texture);
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,
      &state->old_scene_texture_2d);
    state->old_scene_texture_valid=1;
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
  begin_synthetic_surface_state(&state->old_program,&state->old_blend,
    &state->old_depth,&state->old_cull,&state->old_depth_mask,
    &state->old_depth_func,state->old_blend_func);
  use_program(g_synthetic_surface.program);
  shadow_note_use_program(g_synthetic_surface.program);
  setup_synthetic_surface_uniforms(mode,count,count_known);
  return 1;
}

static void end_synthetic_surface_draw(const SyntheticSurfaceDrawState *state) {
  if(!state) return;
  CaptureGL *gl=capture_gl();
  if(state->old_scene_texture_valid && gl && gl->active_texture &&
     gl->bind_texture) {
    shadow_call_active_texture(gl,GL_TEXTURE15);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,
      (GLuint)state->old_scene_texture_2d);
    shadow_call_active_texture(gl,(GLenum)state->old_active_texture);
  }
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
  if(!runtime_verbose_log()) return;
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

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays(GLenum mode, GLint first, GLsizei count) {
  PFNGLDRAWARRAYS draw=real_draw_arrays();
  if(!draw) return;

  tr456_record_synthetic_surface_vertices(count,1,first,0,0,0,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_base_vertex(GLenum mode, GLsizei count,
                                                        GLenum type, const void *indices,
                                                        GLint base_vertex) {
  PFNGLDRAWELEMENTSBASEVERTEX draw=real_draw_elements_base_vertex();
  if(!draw) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,base_vertex);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,base_vertex);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_range_elements(GLenum mode, GLuint start, GLuint end,
                                                  GLsizei count, GLenum type,
                                                  const void *indices) {
  PFNGLDRAWRANGEELEMENTS draw=real_draw_range_elements();
  if(!draw) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,start,end,count,type,indices);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_range_elements_base_vertex(GLenum mode, GLuint start,
                                                              GLuint end, GLsizei count,
                                                              GLenum type, const void *indices,
                                                              GLint base_vertex) {
  PFNGLDRAWRANGEELEMENTSBASEVERTEX draw=real_draw_range_elements_base_vertex();
  if(!draw) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,base_vertex);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,start,end,count,type,indices,base_vertex);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays_instanced(GLenum mode, GLint first,
                                                    GLsizei count, GLsizei instance_count) {
  PFNGLDRAWARRAYSINSTANCED draw=real_draw_arrays_instanced();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,1,first,0,0,0,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count,instance_count);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced(GLenum mode, GLsizei count,
                                                      GLenum type, const void *indices,
                                                      GLsizei instance_count) {
  PFNGLDRAWELEMENTSINSTANCED draw=real_draw_elements_instanced();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_vertex(GLenum mode, GLsizei count,
                                                                  GLenum type, const void *indices,
                                                                  GLsizei instance_count,
                                                                  GLint base_vertex) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw=real_draw_elements_instanced_base_vertex();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,base_vertex);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_vertex);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,1,base_vertex,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_arrays_instanced_base_instance(GLenum mode, GLint first,
                                                                  GLsizei count,
                                                                  GLsizei instance_count,
                                                                  GLuint base_instance) {
  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE draw=real_draw_arrays_instanced_base_instance();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,1,first,0,0,0,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,first,count,instance_count,base_instance);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,1,first,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_instance(GLenum mode, GLsizei count,
                                                                    GLenum type, const void *indices,
                                                                    GLsizei instance_count,
                                                                    GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE draw=real_draw_elements_instanced_base_instance();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,0);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_instance);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_instanced_base_vertex_base_instance(
    GLenum mode, GLsizei count, GLenum type, const void *indices,
    GLsizei instance_count, GLint base_vertex, GLuint base_instance) {
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE draw=
    real_draw_elements_instanced_base_vertex_base_instance();
  if(!draw || instance_count<=0) return;

  tr456_record_synthetic_surface_vertices(count,0,0,1,type,indices,base_vertex);
  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,count,1,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
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
    if(!current_draw_should_skip_original_for_synthetic_ready(mode,count[i],1,
       1,synthetic_flow))
      return 0;
  }
  return saw;
}

static void draw_synthetic_surface_arrays_indirect(GLenum mode, const void *indirect) {
  PFNGLDRAWARRAYSINDIRECT draw=real_draw_arrays_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,indirect);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,-1,1,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_elements_indirect(GLenum mode, GLenum type,
                                                     const void *indirect) {
  PFNGLDRAWELEMENTSINDIRECT draw=real_draw_elements_indirect();
  if(!draw) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,type,indirect);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,-1,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

static void draw_synthetic_surface_multi_arrays_indirect(GLenum mode, const void *indirect,
                                                         GLsizei draw_count, GLsizei stride) {
  PFNGLMULTIDRAWARRAYSINDIRECT draw=real_multi_draw_arrays_indirect();
  if(!draw || draw_count<=0) return;

  SyntheticSurfaceDrawState state;
  if(!begin_synthetic_surface_draw(mode,0,0,&state)) return;
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,indirect,draw_count,stride);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
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
  unsigned long long perf_t0=perf_ticks_now();
  draw(mode,type,indirect,draw_count,stride);
  perf_note_synthetic_draw(perf_ticks_now()-perf_t0);
  log_synthetic_surface_draw(mode,draw_count,0,0,0,0,synthetic_surface_last_error());
  end_synthetic_surface_draw(&state);
}

__declspec(dllexport) void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  runtime_start_once();
  PFNGLDRAWARRAYS real=real_draw_arrays();
  if(real) {
    note_draw("glDrawArrays",mode,count);
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays" : "synthetic water surface arrays");
    if(!skip_original) {
      real(mode,first,count);
      tr456_wet_lara_draw_arrays("glDrawArrays",real,mode,first,count);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface range" : "synthetic water surface range");
    if(!skip_original) {
      real(mode,start,end,count,type,indices);
      tr456_wet_lara_draw_range_elements("glDrawRangeElements",real,mode,
        start,end,count,type,indices);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface basevertex" : "synthetic water surface basevertex");
    if(!skip_original) {
      real(mode,count,type,indices,base_vertex);
      tr456_wet_lara_draw_elements_base_vertex("glDrawElementsBaseVertex",
        real,mode,count,type,indices,base_vertex);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface range basevertex" : "synthetic water surface range basevertex");
    if(!skip_original) {
      real(mode,start,end,count,type,indices,base_vertex);
      tr456_wet_lara_draw_range_elements_base_vertex(
        "glDrawRangeElementsBaseVertex",real,mode,start,end,count,type,
        indices,base_vertex);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays instanced" : "synthetic water surface arrays instanced");
    if(!skip_original) {
      real(mode,first,count,instance_count);
      tr456_wet_lara_draw_arrays_instanced("glDrawArraysInstanced",real,
        mode,first,count,instance_count);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface elements instanced" : "synthetic water surface elements instanced");
    if(!skip_original) {
      real(mode,count,type,indices,instance_count);
      tr456_wet_lara_draw_elements_instanced("glDrawElementsInstanced",
        real,mode,count,type,indices,instance_count);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface instanced basevertex" : "synthetic water surface instanced basevertex");
    if(!skip_original) {
      real(mode,count,type,indices,instance_count,base_vertex);
      tr456_wet_lara_draw_elements_instanced_base_vertex(
        "glDrawElementsInstancedBaseVertex",real,mode,count,type,indices,
        instance_count,base_vertex);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface arrays instanced baseinstance" :
        "synthetic water surface arrays instanced baseinstance");
    if(!skip_original) {
      real(mode,first,count,instance_count,base_instance);
      tr456_wet_lara_draw_arrays_instanced_base_instance(
        "glDrawArraysInstancedBaseInstance",real,mode,first,count,
        instance_count,base_instance);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface elements instanced baseinstance" :
        "synthetic water surface elements instanced baseinstance");
    if(!skip_original) {
      real(mode,count,type,indices,instance_count,base_instance);
      tr456_wet_lara_draw_elements_instanced_base_instance(
        "glDrawElementsInstancedBaseInstance",real,mode,count,type,indices,
        instance_count,base_instance);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,count,1);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,count,1);
    int skip_original=current_draw_should_skip_original_for_synthetic_ready(mode,count,1,
      synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow surface instanced basevertex baseinstance" :
        "synthetic water surface instanced basevertex baseinstance");
    if(!skip_original) {
      real(mode,count,type,indices,instance_count,base_vertex,base_instance);
      tr456_wet_lara_draw_elements_instanced_base_vertex_base_instance(
        "glDrawElementsInstancedBaseVertexBaseInstance",real,mode,count,type,
        indices,instance_count,base_vertex,base_instance);
    }
    if(synthetic_ready)
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
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
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
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
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
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
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
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
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
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
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
        if(current_draw_is_synthetic_surface_candidate(mode,count[i],1))
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      current_draw_should_skip_original_for_synthetic_ready(mode,0,0,
        synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow arrays indirect" : "synthetic water arrays indirect");
    if(!skip_original) {
      real(mode,indirect);
      tr456_wet_lara_draw_arrays_indirect("glDrawArraysIndirect",real,
        mode,indirect);
    }
    if(synthetic_ready)
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
    int synthetic_surface=current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      current_draw_should_skip_original_for_synthetic_ready(mode,0,0,
        synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow elements indirect" : "synthetic water elements indirect");
    if(!skip_original) {
      real(mode,type,indirect);
      tr456_wet_lara_draw_elements_indirect("glDrawElementsIndirect",real,
        mode,type,indirect);
    }
    if(synthetic_ready)
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
    int synthetic_surface=draw_count>0 &&
      current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      current_draw_should_skip_original_for_synthetic_ready(mode,0,0,
        synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi arrays indirect" :
        "synthetic water multi arrays indirect");
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
    int synthetic_surface=draw_count>0 &&
      current_draw_is_synthetic_surface_candidate(mode,0,0);
    int synthetic_ready=synthetic_surface && ensure_synthetic_surface_program();
    int synthetic_flow=current_draw_is_synthetic_flow_candidate(mode,0,0);
    int skip_original=synthetic_surface &&
      current_draw_should_skip_original_for_synthetic_ready(mode,0,0,
        synthetic_ready,synthetic_flow);
    if(synthetic_ready)
      prepare_scene_capture_for_synthetic_surface(
        g_current_program_type==SHADER_WATER_FLOW ?
        "synthetic flow multi elements indirect" :
        "synthetic water multi elements indirect");
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

__declspec(dllexport) BOOL WINAPI wglSwapBuffers(HDC hdc) {
  runtime_start_once();
  PFNWGLSWAPBUFFERS real=(PFNWGLSWAPBUFFERS)old_proc("wglSwapBuffers");
  BOOL ok=real ? real(hdc) : FALSE;
  advance_frame();
  return ok;
}

__declspec(dllexport) BOOL WINAPI wglSwapLayerBuffers(HDC hdc, UINT planes) {
  runtime_start_once();
  PFNWGLSWAPLAYERBUFFERS real=(PFNWGLSWAPLAYERBUFFERS)old_proc("wglSwapLayerBuffers");
  BOOL ok=real ? real(hdc,planes) : FALSE;
  advance_frame();
  return ok;
}

__declspec(dllexport) PROC WINAPI wglGetProcAddress(LPCSTR name) {
  runtime_start_once();
#if TR456_DIAG_BUILD
  {
    LONG n=InterlockedIncrement(&g_diag_wgl_query_count);
    if(n<=220)
      diag_logf("DIAG wglGetProcAddress #%ld name=\"%s\"",
        (long)n,name ? name : "(null)");
  }
#endif
  if(name && (!lstrcmpA(name,"glShaderSource") || !lstrcmpA(name,"glShaderSourceARB"))) {
    diag_logf("DIAG hook returned for %s",name);
    return HOOK_PROC(hook_glShaderSource);
  }
  if(name && (!lstrcmpA(name,"glCompileShader") || !lstrcmpA(name,"glCompileShaderARB"))) {
    diag_logf("DIAG hook returned for %s",name);
    return HOOK_PROC(hook_glCompileShader);
  }
  if(name && (!lstrcmpA(name,"glLinkProgram") || !lstrcmpA(name,"glLinkProgramARB"))) {
    diag_logf("DIAG hook returned for %s",name);
    return HOOK_PROC(hook_glLinkProgram);
  }
  if(name && !lstrcmpA(name,"glAttachShader")) {
    diag_logf("DIAG hook returned for %s",name);
    return HOOK_PROC(hook_glAttachShader);
  }
  if(name && !lstrcmpA(name,"glUseProgram")) {
    diag_logf("DIAG hook returned for %s",name);
    return HOOK_PROC(hook_glUseProgram);
  }
  if(name && (!lstrcmpA(name,"glActiveTexture") ||
     !lstrcmpA(name,"glActiveTextureARB"))) {
    return HOOK_PROC(hook_glActiveTexture);
  }
  if(name && !lstrcmpA(name,"glBindTexture")) {
    return HOOK_PROC(hook_glBindTexture);
  }
  if(name && (!lstrcmpA(name,"glBindFramebuffer") ||
     !lstrcmpA(name,"glBindFramebufferEXT"))) {
    return HOOK_PROC(hook_glBindFramebuffer);
  }
  if(name && !lstrcmpA(name,"glViewport")) {
    return HOOK_PROC(hook_glViewport);
  }
  if(name && !lstrcmpA(name,"glEnable")) {
    return HOOK_PROC(hook_glEnable);
  }
  if(name && !lstrcmpA(name,"glDisable")) {
    return HOOK_PROC(hook_glDisable);
  }
  if(name && !lstrcmpA(name,"glDepthMask")) {
    return HOOK_PROC(hook_glDepthMask);
  }
  if(name && !lstrcmpA(name,"glDepthFunc")) {
    return HOOK_PROC(hook_glDepthFunc);
  }
  if(name && !lstrcmpA(name,"glBlendFunc")) {
    return HOOK_PROC(hook_glBlendFunc);
  }
  if(name && (!lstrcmpA(name,"glBlendFuncSeparate") ||
     !lstrcmpA(name,"glBlendFuncSeparateEXT"))) {
    return HOOK_PROC(hook_glBlendFuncSeparate);
  }
  if(name && !lstrcmpA(name,"glUniform1i")) {
    return HOOK_PROC(hook_glUniform1i);
  }
  if(name && !lstrcmpA(name,"glUniform1iv")) {
    return HOOK_PROC(hook_glUniform1iv);
  }
  if(name && !lstrcmpA(name,"glUniform4f")) {
    return HOOK_PROC(hook_glUniform4f);
  }
  if(name && !lstrcmpA(name,"glUniform4fv")) {
    return HOOK_PROC(hook_glUniform4fv);
  }
  if(name && !lstrcmpA(name,"glUniformMatrix4fv")) {
    return HOOK_PROC(hook_glUniformMatrix4fv);
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
  if(name && !lstrcmpA(name,"glTexImage3D")) {
    return HOOK_PROC(hook_glTexImage3D);
  }
  if(name && !lstrcmpA(name,"glTexSubImage3D")) {
    return HOOK_PROC(hook_glTexSubImage3D);
  }
  if(name && !lstrcmpA(name,"glTextureSubImage3D")) {
    return HOOK_PROC(hook_glTextureSubImage3D);
  }
  if(name && !lstrcmpA(name,"glTextureImage3DEXT")) {
    return HOOK_PROC(hook_glTextureImage3DEXT);
  }
  if(name && !lstrcmpA(name,"glTextureSubImage3DEXT")) {
    return HOOK_PROC(hook_glTextureSubImage3DEXT);
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
#if TR456_DIAG_BUILD
    {
      char self_path[MAX_PATH]="";
      char exe_path[MAX_PATH]="";
      char cwd[MAX_PATH]="";
      char mod_ini[MAX_PATH]="";
      char mod_synth[MAX_PATH]="";
      char root_ini[MAX_PATH]="";
      GetModuleFileNameA(g_self,self_path,MAX_PATH);
      GetModuleFileNameA(0,exe_path,MAX_PATH);
      GetCurrentDirectoryA(MAX_PATH,cwd);
      join_mod_path(mod_ini,"tr456_water.ini");
      join_mod_path(mod_synth,"tr456_water_synthetic.glsl");
      join_game_path(root_ini,"tr456_water.ini");
      log_line("DIAG diagnostic OpenGL32.dll loaded from DllMain");
      diag_logf("DIAG paths self=\"%s\" exe=\"%s\" cwd=\"%s\"",self_path,
        exe_path,cwd);
      diag_logf("DIAG dirs game=\"%s\" mod=\"%s\"",g_dir,g_mod_dir);
      diag_logf("DIAG files mod_ini=%d mod_synthetic=%d root_ini=%d",
        file_exists(mod_ini),file_exists(mod_synth),file_exists(root_ini));
    }
#endif
  } else if(reason==DLL_PROCESS_DETACH) {
#if TR456_DIAG_BUILD
    log_line("DIAG diagnostic OpenGL32.dll unloading");
#endif
    AcquireSRWLockExclusive(&g_log_lock);
    if(g_log_handle!=INVALID_HANDLE_VALUE) {
      CloseHandle(g_log_handle);
      g_log_handle=INVALID_HANDLE_VALUE;
    }
    ReleaseSRWLockExclusive(&g_log_lock);
  }
  return TRUE;
}
