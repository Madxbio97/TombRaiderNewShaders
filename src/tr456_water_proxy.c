#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <process.h>

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
#define TR456_PROXY_BUILD_VERSION "1.2.23"
#ifndef TR456_STARTUP_LOG
#define TR456_STARTUP_LOG 0
#endif
#define TR456_EFFECT_TOGGLE_MASK 0x0FFFu
static HMODULE g_self;
static HMODULE g_old_gl;
static char g_dir[MAX_PATH];
static char g_mod_dir[MAX_PATH];
static HANDLE g_log_handle=INVALID_HANDLE_VALUE;
static SRWLOCK g_log_lock=SRWLOCK_INIT;
#if TR456_STARTUP_LOG
static SRWLOCK g_boot_log_lock=SRWLOCK_INIT;
static volatile LONG g_boot_old_proc_count;
static volatile LONG g_boot_wgl_query_count;
static volatile LONG g_boot_wgl_fallback_count;
static volatile LONG g_boot_wgl_delete_count;
static volatile LONG g_boot_icd_query_count;
static volatile LONG g_boot_icd_hit_count;
static volatile LONG g_boot_shader_source_count;
static volatile LONG g_boot_swap_count;
static volatile LONG g_boot_exception_logged;
static LPTOP_LEVEL_EXCEPTION_FILTER g_prev_exception_filter;
#endif
static __thread int g_real_wgl_query_depth;
static __thread int g_wgl_delete_context_depth;
static __thread int g_shader_source_hook_depth;

static const char k_surface_key[]="vec3 tc = vWorldPos.xyz / 1024.0 * uParams.x;";
static const char k_reflect_key[]="float hC = texture(sNoise, vec3(uv, t)).x;";
static const char k_ssr_key[]="float not_water = 1 - texture(sTex0, vec3(uv_refract.xy, 0)).w;";
static const char k_flow_vertex_key[]="uv += uParams.xy * uModelMatrix[3].x;";

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE14 0x84CE
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
#define GL_STENCIL_TEST 0x0B90
#define GL_SCISSOR_TEST 0x0C11
#define GL_ALPHA_TEST 0x0BC0
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_CLIP_DISTANCE0 0x3000
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_FUNC 0x0B74
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_FUNC_ADD 0x8006
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
#define GL_VERTEX_ARRAY_BINDING 0x85B5
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
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
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
  GLint flow_fx_loc[4];
  GLint flow_sampler_loc;
  GLint coord_attr_loc;
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
static GLuint g_underlay_tex;
static int g_scene_w;
static int g_scene_h;
static int g_scene_view_w;
static int g_scene_view_h;
static int g_scene_scale=1;
static int g_scene_has_pixels;
static int g_underlay_w;
static int g_underlay_h;
static int g_underlay_view_w;
static int g_underlay_view_h;
static int g_underlay_has_pixels;
static int g_logged_capture;
static int g_logged_capture_scale_fallback;
static int g_logged_underlay_capture;
static int g_scene_captured;
static int g_logged_use_ssr;
static unsigned int g_frame_index=1;
static int g_runtime_config_loaded;
static int g_runtime_shader_patching;
static int g_runtime_fbo_reflection=1;
static int g_runtime_fbo_capture_interval=1;
static int g_runtime_fbo_warmup_frames;
static int g_runtime_fbo_scale=1;
static int g_runtime_fbo_error_check;
static int g_diag_insert_down;
static unsigned int g_diag_poll_frame;
static int g_diag_session;
static int g_diag_active_frames;
static int g_diag_lines_left;
static int g_runtime_verbose_log;
static int g_runtime_perf_telemetry;
static int g_runtime_shader_binary_cache;
static int g_runtime_dump_flow_shader_source;
static int g_runtime_refresh_flow_texture_signatures;
static GLfloat g_runtime_flow_fx0[4];
static GLfloat g_runtime_flow_fx1[4];
static GLfloat g_runtime_flow_fx2[4];
static GLfloat g_runtime_flow_fx3[4];
static int g_runtime_ripple_min_count;
static int g_runtime_ripple_center_mode;
static int g_runtime_synthetic_surface;
static int g_runtime_synthetic_standing_only;
static int g_runtime_synthetic_standing_replace_original;
static int g_runtime_synthetic_standing_bounds_guard=1;
static int g_runtime_synthetic_standing_preserve_mask=1;
static int g_runtime_synthetic_flow_surface;
static int g_runtime_synthetic_flow_replace_original;
static int g_runtime_flow_lite_surface;
static int g_runtime_flow_inplace_patch=1;
static int g_runtime_synthetic_reflect_surface;
static int g_runtime_synthetic_overlay_depth_mode;
static int g_runtime_synthetic_debug_solid;
static int g_runtime_flow_texture_fallback;
static GLfloat g_runtime_synthetic_reflect_original_mask=1.0f;
static int g_runtime_underlay_pattern;
static int g_runtime_underlay_flow_pattern;
static GLfloat g_runtime_synthetic_opacity;
static GLfloat g_runtime_synthetic_tint;
static GLfloat g_runtime_synthetic_reflection;
static GLfloat g_runtime_underlay_pattern_strength;
static int g_runtime_synthetic_compile_delay_frames;
static int g_runtime_synthetic_contact_max_samples=192;
static unsigned int g_effect_toggle_mask=TR456_EFFECT_TOGGLE_MASK;
static unsigned int g_effect_hotkey_down_mask;
static unsigned int g_effect_hotkey_poll_frame;
static unsigned int g_synthetic_surface_logged;
static unsigned int g_synthetic_surface_logged_by_type[6];
static unsigned int g_synthetic_overlay_depth_logged;
static unsigned int g_synthetic_clip_state_logged;
static unsigned int g_synthetic_attrib_state_logged;
static unsigned int g_synthetic_uniform_state_logged;
static unsigned int g_synthetic_compile_delay_logged;
static GLuint g_swap_debug_program;
static int g_swap_debug_tried;
static unsigned int g_swap_debug_logged;
static unsigned int g_flow_material_bypass_logged;
static unsigned int g_flow_surface_texture_logged;
static unsigned int g_flow_texture_upload_probe_logged;
static unsigned int g_flow_surface_gate_logged;
static unsigned int g_flow_surface_confirmed_logged;
static unsigned int g_water_draw_logged_by_type[6];

typedef enum {
  TR456_COMPAT_AUTO=0,
  TR456_COMPAT_FULL=1,
  TR456_COMPAT_SHADER_ONLY=2,
  TR456_COMPAT_VANILLA=3
} TrshaderCompatMode;

typedef enum {
  TR456_GPU_UNKNOWN=0,
  TR456_GPU_NVIDIA=1,
  TR456_GPU_AMD=2,
  TR456_GPU_INTEL=3,
  TR456_GPU_MESA=4,
  TR456_GPU_MICROSOFT=5
} TrshaderCompatGpu;

typedef struct {
  int config_loaded;
  int driver_ready;
  int report_enabled;
  int gl_error_check;
  int gl_error_warmup_draws;
  int max_synthetic_errors;
  int report_logged;
  int cache_bypass_logged;
  int synthetic_error_count;
  int synthetic_success_count;
  int synthetic_error_check_retired;
  int synthetic_disabled_logged;
  TrshaderCompatMode mode;
  TrshaderCompatGpu gpu;
  int gl_major;
  int gl_minor;
  char mode_text[32];
  char profile[48];
  char fallback_reason[160];
  char vendor[128];
  char renderer[160];
  char version[128];
  char glsl[128];
  char reshade_dll[MAX_PATH];
} TrshaderCompatState;

static TrshaderCompatState g_compat;
static int g_shader_defines_logged;
static GLfloat g_contact_cache[16][4];
static GLfloat g_contact_motion_cache[16][4];
static unsigned int g_contact_cache_frame;
static int g_contact_cache_valid;
static GLfloat g_effective_contact_cache[16][4];
static GLfloat g_effective_contact_motion_cache[16][4];
static unsigned int g_effective_contact_cache_frame;
static int g_effective_contact_cache_valid;
static int g_effective_contact_cache_count;
static int g_effective_contact_cache_source;

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
static unsigned int g_synthetic_standing_bounds_skip_logged;
#if TR456_DIAG_BUILD
static volatile LONG g_diag_wgl_query_count;
static volatile LONG g_diag_shader_source_count;
static volatile LONG g_diag_draw_call_count;
#endif
static volatile LONG g_runtime_started;
static SRWLOCK g_ini_lock=SRWLOCK_INIT;
static char *g_ini_text;
static int g_ini_loaded;
static FILETIME g_ini_write_time;
static int g_ini_write_time_valid;

typedef struct {
  GLuint texture;
  GLint layer;
  const char *name;
} FlowTextureLayer;

static GLuint g_flow_surface_texture_objects[64];
static FlowTextureLayer g_flow_surface_texture_layers[128];

typedef struct {
  uint64_t binary_cache_key;
  GLuint program;
  int tried;
  int ready;
  int failed;
  int binary_cache_loaded;
  int compile_stage;
  unsigned int compile_step_frame;
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
  GLint loc_flow_tex0_wrap;
  GLint loc_underlay;
  GLint loc_capture_info;
  GLint loc_underlay_info;
  GLint loc_synthetic_info;
  GLint loc_synthetic_mode;
  GLint loc_synthetic_profile;
  GLint loc_params;
  GLint loc_draw_info;
  GLint loc_toggle0;
  GLint loc_toggle1;
  GLint loc_toggle2;
  GLint loc_flow_fx0;
  GLint loc_flow_fx1;
  GLint loc_flow_fx2;
  GLint loc_flow_fx3;
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
  int synthetic_flow_lite;
  int synthetic_surface;
  int synthetic_ready;
  int skip_original;
  const char *capture_reason;
} SyntheticDrawDecision;

static const SyntheticDrawDecision *current_synthetic_draw_decision(
  GLenum mode, GLsizei count, int count_known);

typedef struct {
  unsigned int start_frame;
  unsigned int frames;
  unsigned long long draw_calls;
  unsigned long long tracked_water_draws;
  unsigned long long water_draws_by_type[6];
  unsigned long long frame_interval_samples;
  unsigned long long frame_interval_ticks;
  unsigned long long frame_interval_max_ticks;
  unsigned long long decision_hits;
  unsigned long long decision_misses;
  unsigned long long decision_ticks;
  unsigned long long decision_max_ticks;
  unsigned long long synthetic_candidates;
  unsigned long long synthetic_standing;
  unsigned long long synthetic_flow;
  unsigned long long synthetic_ready;
  unsigned long long original_skips;
  unsigned long long capture_requests;
  unsigned long long capture_updates;
  unsigned long long capture_resizes;
  unsigned long long capture_ticks;
  unsigned long long capture_max_ticks;
  unsigned long long capture_update_ticks;
  unsigned long long capture_update_max_ticks;
  unsigned long long synthetic_begin;
  unsigned long long synthetic_draws;
  unsigned long long synthetic_begin_by_type[6];
  unsigned long long synthetic_draws_by_type[6];
  unsigned long long synthetic_setup_ticks;
  unsigned long long synthetic_setup_max_ticks;
  unsigned long long synthetic_draw_ticks;
  unsigned long long synthetic_draw_max_ticks;
  unsigned long long wet_lara_candidates;
  unsigned long long wet_lara_replays;
  unsigned long long wet_lara_vertices;
  unsigned long long wet_lara_draw_ticks;
  unsigned long long wet_lara_draw_max_ticks;
  unsigned long long ripple_contact_checks;
  unsigned long long ripple_contact_hits;
  unsigned long long ripple_contact_ticks;
  unsigned long long ripple_contact_max_ticks;
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
  GLint old_stencil;
  GLint old_scissor;
  GLint old_alpha;
  GLint old_sample_alpha_to_coverage;
  GLint old_clip_distance[8];
  GLint old_rasterizer_discard;
  GLint old_depth_mask;
  GLint old_depth_func;
  GLint old_blend_func[4];
  GLint old_blend_equation[2];
  GLint old_color_mask[4];
  GLint old_active_texture;
  GLint old_scene_texture_2d;
  GLint old_underlay_texture_2d;
  GLint old_flow_texture_array;
  int old_scene_texture_valid;
  int old_underlay_texture_valid;
  int old_flow_texture_array_valid;
} SyntheticSurfaceDrawState;

static SyntheticSurfacePass g_synthetic_surface;
static SyntheticSurfacePass g_flow_lite_surface;
static SyntheticDrawDecision g_synthetic_draw_decision_cache;
static PerfTelemetry g_perf;
static LARGE_INTEGER g_perf_freq;
static int g_perf_freq_ready;
static unsigned long long g_perf_last_frame_ticks;
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
typedef void (APIENTRY *PFNGLBINDFRAGDATALOCATION)(GLuint, GLuint, const GLchar *);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATION)(GLuint, const GLchar *);
typedef GLboolean (APIENTRY *PFNGLISENABLED)(GLenum);
typedef void (APIENTRY *PFNGLENABLE)(GLenum);
typedef void (APIENTRY *PFNGLDISABLE)(GLenum);
typedef void (APIENTRY *PFNGLDEPTHMASK)(GLboolean);
typedef void (APIENTRY *PFNGLDEPTHFUNC)(GLenum);
typedef void (APIENTRY *PFNGLBLENDFUNC)(GLenum, GLenum);
typedef void (APIENTRY *PFNGLBLENDFUNCSEPARATE)(GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRY *PFNGLBLENDEQUATIONSEPARATE)(GLenum, GLenum);
typedef void (APIENTRY *PFNGLCOLORMASK)(GLboolean, GLboolean, GLboolean, GLboolean);
typedef void (APIENTRY *PFNGLVIEWPORT)(GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY *PFNGLGETVERTEXATTRIBIV)(GLuint, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETVERTEXATTRIBPOINTERV)(GLuint, GLenum, void **);
typedef void (APIENTRY *PFNGLBINDBUFFER)(GLenum, GLuint);
typedef void (APIENTRY *PFNGLGETBUFFERPARAMETERIV)(GLenum, GLenum, GLint *);
typedef void (APIENTRY *PFNGLGETBUFFERSUBDATA)(GLenum, intptr_t, intptr_t, void *);
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXT)(HDC);
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARB)(HDC, HGLRC, const int *);
typedef BOOL (WINAPI *PFNWGLDELETECONTEXT)(HGLRC);
typedef HGLRC (WINAPI *PFNWGLGETCURRENTCONTEXT)(void);
typedef BOOL (WINAPI *PFNWGLMAKECURRENT)(HDC, HGLRC);
typedef BOOL (WINAPI *PFNWGLSHARELISTS)(HGLRC, HGLRC);
typedef PROC (WINAPI *PFNDRVGETPROCADDRESS)(LPCSTR);
typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC, const int *,
                                                  const FLOAT *, UINT,
                                                  int *, UINT *);
typedef BOOL (WINAPI *PFNWGLSWAPBUFFERS)(HDC);
typedef BOOL (WINAPI *PFNWGLSWAPLAYERBUFFERS)(HDC, UINT);

#define TR456_MAX_ICD_RESOLVERS 8
#define TR456_ICD_PROC_CACHE_SIZE 128

typedef struct {
  HMODULE module;
  PFNDRVGETPROCADDRESS get_proc;
  char name[64];
} TrshaderIcdResolver;

typedef struct {
  char name[64];
  FARPROC proc;
} TrshaderIcdProcCache;

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
static SRWLOCK g_icd_lock=SRWLOCK_INIT;
static TrshaderIcdResolver g_icd_resolvers[TR456_MAX_ICD_RESOLVERS];
static int g_icd_resolver_count;
static TrshaderIcdProcCache g_icd_proc_cache[TR456_ICD_PROC_CACHE_SIZE];
static unsigned int g_icd_proc_cache_cursor;
static CaptureGL *capture_gl(void);
static int ensure_synthetic_surface_program(void);
static int ensure_flow_lite_surface_program(void);
static void prepare_scene_capture_for_flow_inplace(void);

#include "tr456_proxy_shadow_state.inc"

#include "tr456_proxy_bootstrap.inc"

#include "tr456_proxy_runtime_config.inc"

static char *synthetic_surface_vertex_shader(void) {
  return configured_shader("tr456_water_synthetic_vertex.glsl","synthetic water vertex shader");
}

static char *synthetic_surface_shader(void) {
  return configured_shader("tr456_water_synthetic.glsl",
    "synthetic water fragment shader");
}

static void preload_one_shader(char *(*load)(void)) {
  char *text=load ? load() : 0;
  free(text);
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

static unsigned __stdcall shader_preload_thread(void *arg) {
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
  HANDLE h=(HANDLE)_beginthreadex(0,0,shader_preload_thread,(void*)packed,0,0);
  if(h) {
    SetThreadPriority(h,THREAD_PRIORITY_BELOW_NORMAL);
    CloseHandle(h);
  } else {
    preload_shader_sources(mode>=2);
  }
}

static void runtime_start_once(void) {
  if(InterlockedCompareExchange(&g_runtime_started,1,0)!=0) return;
  char exe[MAX_PATH]="";
  char cwd[MAX_PATH]="";
  GetModuleFileNameA(0,exe,MAX_PATH);
  GetCurrentDirectoryA(MAX_PATH,cwd);
  boot_logf("runtime_start_once exe=\"%s\" cwd=\"%s\"",exe,cwd);
#if TR456_DIAG_BUILD
  diag_logf("DIAG runtime_start_once exe=\"%s\" cwd=\"%s\"",exe,cwd);
#endif
  log_line("tr456 water proxy loaded build=" TR456_PROXY_BUILD_VERSION);
  boot_log_line("runtime_start_once starting shader preload");
  start_shader_preload();
  boot_log_line("runtime_start_once done");
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
      for(int j=0;j<4;j++)
        g_program_tracks[i].flow_fx_loc[j]=-2;
      g_program_tracks[i].flow_sampler_loc=-2;
      g_program_tracks[i].coord_attr_loc=-2;
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
    for(int i=0;i<4;i++)
      p->flow_fx_loc[i]=-2;
    p->flow_sampler_loc=-2;
    p->coord_attr_loc=-2;
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
  if(!name || !base || max<=0) return -1;
  size_t n=strlen(base);
  if(strncmp(name,base,n)!=0 || name[n]!='[') return -1;
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
    size_t remaining=out_size-pos;
    int n=snprintf(out+pos,remaining,"%s%u:%08X:%s",
      i ? "," : "",p->shaders[i],(unsigned int)p->shader_hashes[i],
      shader_type_name(p->shader_types[i]));
    if(n<0) break;
    if((size_t)n>=remaining) {
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
    "diag config session=%d patch=%d fbo=%d standingReplace=%d rippleMin=%d centerMode=%d toggles=0x%03X",
    g_diag_session,g_runtime_shader_patching,g_runtime_fbo_reflection,
    g_runtime_synthetic_standing_replace_original,
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

static int perf_timing_enabled(void) {
  return g_runtime_perf_telemetry || diag_is_active();
}

static unsigned long long perf_ticks_now(void) {
  if(!perf_timing_enabled()) return 0;
  LARGE_INTEGER q;
  QueryPerformanceCounter(&q);
  return (unsigned long long)q.QuadPart;
}

static unsigned long long perf_ticks_elapsed(unsigned long long start) {
  if(!start) return 0;
  unsigned long long now=perf_ticks_now();
  return now>=start ? now-start : 0;
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

static void perf_note_frame_interval(void) {
  unsigned long long now=perf_ticks_now();
  if(!now) {
    g_perf_last_frame_ticks=0;
    return;
  }
  perf_ensure_started();
  if(g_perf_last_frame_ticks) {
    unsigned long long ticks=now>=g_perf_last_frame_ticks ?
      now-g_perf_last_frame_ticks : 0;
    g_perf.frame_interval_samples++;
    g_perf.frame_interval_ticks+=ticks;
    if(ticks>g_perf.frame_interval_max_ticks)
      g_perf.frame_interval_max_ticks=ticks;
  }
  g_perf_last_frame_ticks=now;
}

static void perf_note_frame(void) {
  perf_ensure_started();
  g_perf.frames++;
}

static void perf_note_draw(int tracked_water) {
  perf_ensure_started();
  g_perf.draw_calls++;
  if(tracked_water) {
    g_perf.tracked_water_draws++;
    if(g_current_program_type>0 && g_current_program_type<6)
      g_perf.water_draws_by_type[g_current_program_type]++;
  }
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
  if(ticks>g_perf.decision_max_ticks)
    g_perf.decision_max_ticks=ticks;
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
  if(ticks>g_perf.capture_max_ticks)
    g_perf.capture_max_ticks=ticks;
  if(updated) {
    g_perf.capture_update_ticks+=ticks;
    if(ticks>g_perf.capture_update_max_ticks)
      g_perf.capture_update_max_ticks=ticks;
  }
}

static void perf_note_synthetic_begin(unsigned long long setup_ticks) {
  perf_ensure_started();
  g_perf.synthetic_begin++;
  if(g_current_program_type>0 && g_current_program_type<6)
    g_perf.synthetic_begin_by_type[g_current_program_type]++;
  g_perf.synthetic_setup_ticks+=setup_ticks;
  if(setup_ticks>g_perf.synthetic_setup_max_ticks)
    g_perf.synthetic_setup_max_ticks=setup_ticks;
}

static void perf_note_synthetic_draw(unsigned long long ticks) {
  perf_ensure_started();
  g_perf.synthetic_draws++;
  if(g_current_program_type>0 && g_current_program_type<6)
    g_perf.synthetic_draws_by_type[g_current_program_type]++;
  g_perf.synthetic_draw_ticks+=ticks;
  if(ticks>g_perf.synthetic_draw_max_ticks)
    g_perf.synthetic_draw_max_ticks=ticks;
}

static void perf_note_wet_lara_candidate(void) {
  perf_ensure_started();
  g_perf.wet_lara_candidates++;
}

static void perf_note_wet_lara_replay(GLsizei count, int count_known,
                                      unsigned long long ticks) {
  perf_ensure_started();
  g_perf.wet_lara_replays++;
  if(count_known && count>0)
    g_perf.wet_lara_vertices+=(unsigned long long)count;
  g_perf.wet_lara_draw_ticks+=ticks;
  if(ticks>g_perf.wet_lara_draw_max_ticks)
    g_perf.wet_lara_draw_max_ticks=ticks;
}

static void perf_note_ripple_contact(int hit, unsigned long long ticks) {
  perf_ensure_started();
  g_perf.ripple_contact_checks++;
  if(hit) g_perf.ripple_contact_hits++;
  g_perf.ripple_contact_ticks+=ticks;
  if(ticks>g_perf.ripple_contact_max_ticks)
    g_perf.ripple_contact_max_ticks=ticks;
}

static void perf_log_summary(const char *where, int reset_after) {
  perf_ensure_started();
  double frame_avg_ms=g_perf.frame_interval_samples ?
    perf_ticks_ms(g_perf.frame_interval_ticks)/
      (double)g_perf.frame_interval_samples : 0.0;
  char msg[2400];
  snprintf(msg,sizeof(msg),
    "perf telemetry where=%s frames=%u span=%u-%u frameMs avg=%.3f max=%.3f samples=%llu draws=%llu water=%llu waterByType surf/ref/ssr/flow/ripple=%llu/%llu/%llu/%llu/%llu decision=%llu/%llu decisionMs=%.3f max=%.3f synthetic candidates=%llu standing=%llu flow=%llu ready=%llu skipOriginal=%llu capture req=%llu update=%llu resize=%llu captureMs total=%.3f max=%.3f update=%.3f updateMax=%.3f synthetic begin=%llu draws=%llu beginByType surf/ref/ssr/flow/ripple=%llu/%llu/%llu/%llu/%llu drawByType surf/ref/ssr/flow/ripple=%llu/%llu/%llu/%llu/%llu setupMs total=%.3f max=%.3f drawCpuMs total=%.3f max=%.3f wetLara candidates=%llu replays=%llu vertices=%llu drawCpuMs total=%.3f max=%.3f contact checks=%llu hits=%llu contactMs=%.3f max=%.3f toggles=0x%03X",
    where ? where : "unknown",
    g_perf.frames,g_perf.start_frame,g_frame_index,
    frame_avg_ms,perf_ticks_ms(g_perf.frame_interval_max_ticks),
    g_perf.frame_interval_samples,
    g_perf.draw_calls,g_perf.tracked_water_draws,
    g_perf.water_draws_by_type[SHADER_WATER_SURFACE],
    g_perf.water_draws_by_type[SHADER_WATER_REFLECT],
    g_perf.water_draws_by_type[SHADER_WATER_SSR],
    g_perf.water_draws_by_type[SHADER_WATER_FLOW],
    g_perf.water_draws_by_type[SHADER_WATER_RIPPLE],
    g_perf.decision_hits,g_perf.decision_misses,
    perf_ticks_ms(g_perf.decision_ticks),
    perf_ticks_ms(g_perf.decision_max_ticks),
    g_perf.synthetic_candidates,g_perf.synthetic_standing,
    g_perf.synthetic_flow,g_perf.synthetic_ready,g_perf.original_skips,
    g_perf.capture_requests,g_perf.capture_updates,g_perf.capture_resizes,
    perf_ticks_ms(g_perf.capture_ticks),
    perf_ticks_ms(g_perf.capture_max_ticks),
    perf_ticks_ms(g_perf.capture_update_ticks),
    perf_ticks_ms(g_perf.capture_update_max_ticks),
    g_perf.synthetic_begin,g_perf.synthetic_draws,
    g_perf.synthetic_begin_by_type[SHADER_WATER_SURFACE],
    g_perf.synthetic_begin_by_type[SHADER_WATER_REFLECT],
    g_perf.synthetic_begin_by_type[SHADER_WATER_SSR],
    g_perf.synthetic_begin_by_type[SHADER_WATER_FLOW],
    g_perf.synthetic_begin_by_type[SHADER_WATER_RIPPLE],
    g_perf.synthetic_draws_by_type[SHADER_WATER_SURFACE],
    g_perf.synthetic_draws_by_type[SHADER_WATER_REFLECT],
    g_perf.synthetic_draws_by_type[SHADER_WATER_SSR],
    g_perf.synthetic_draws_by_type[SHADER_WATER_FLOW],
    g_perf.synthetic_draws_by_type[SHADER_WATER_RIPPLE],
    perf_ticks_ms(g_perf.synthetic_setup_ticks),
    perf_ticks_ms(g_perf.synthetic_setup_max_ticks),
    perf_ticks_ms(g_perf.synthetic_draw_ticks),
    perf_ticks_ms(g_perf.synthetic_draw_max_ticks),
    g_perf.wet_lara_candidates,g_perf.wet_lara_replays,
    g_perf.wet_lara_vertices,
    perf_ticks_ms(g_perf.wet_lara_draw_ticks),
    perf_ticks_ms(g_perf.wet_lara_draw_max_ticks),
    g_perf.ripple_contact_checks,g_perf.ripple_contact_hits,
    perf_ticks_ms(g_perf.ripple_contact_ticks),
    perf_ticks_ms(g_perf.ripple_contact_max_ticks),
    g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK);
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
    case 0: return "flow overlay";
    case 1: return "flow refraction warp";
    case 2: return "flow reflection";
    case 3: return "flow foam/streaks";
    case 4: return "flow lanes/swirl";
    case 5: return "flow glints/specular";
    case 6: return "flow tension/cross waves";
    case 7: return "flow micro detail/bump";
    case 8: return "flow contacts";
    case 9: return "surface foam";
    case 10: return "surface reflection";
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
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
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
  invalidate_effective_contact_cache();
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
  const unsigned int lifetime=240u;
  const unsigned int stale=72u;
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
    float v_blend=screen_contact ? 0.35f : 0.50f;
    float pos_blend=screen_contact ? 0.32f : 0.10f;
    float z_blend=screen_contact ? 0.26f : 0.10f;
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
    invalidate_effective_contact_cache();
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
  invalidate_effective_contact_cache();
  return slot_index;
}

static void tr456_water_emit_lara_impulse(GLfloat x, GLfloat y, GLfloat z,
                                          GLfloat radius, GLfloat strength) {
  if(strength<=0.001f)
    return;
  int created=0;
  int slot_index=add_ripple_contact(x,y,z,radius,&created);
  RippleContact *c=&g_ripple_contacts[slot_index];
  float extra=f_min(f_max(strength,0.0f),4.0f)*22.0f;
  c->speed=f_max(c->speed,extra);
  c->vy=f_max(c->vy,extra*0.18f);
  if(created) {
    float n=(float)((g_frame_index*1664525u+1013904223u)&1023u)*
      (1.0f/1024.0f);
    c->vx+=(n-0.5f)*extra*0.16f;
    c->vz+=(0.5f-f_abs(n-0.5f))*extra*0.14f;
  }
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
      char msg[256];
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
  RippleContact *wet_c=&g_ripple_contacts[slot_index];
  tr456_wet_lara_note_ripple_circle(slot_index,created,count,
    wet_c->x,wet_c->y,wet_c->z,wet_c->radius,wet_c->speed,
    is_screen_contact_value(wet_c->x,wet_c->y,wet_c->z));
  if(should_log_contact) {
    float sx=f_sqrt(row[0][0]*row[0][0]+row[1][0]*row[1][0]+
      row[2][0]*row[2][0]);
    float sy=f_sqrt(row[0][1]*row[0][1]+row[1][1]*row[1][1]+
      row[2][1]*row[2][1]);
    float sz=f_sqrt(row[0][2]*row[0][2]+row[1][2]*row[1][2]+
      row[2][2]*row[2][2]);
    RippleContact *c=wet_c;
    char msg[512];
    snprintf(msg,sizeof(msg),
      "ripple contact frame=%u slot=%d created=%d count=%d threshold=%d center=%.1f world=(%.1f %.1f %.1f) radius=%.1f screen%s=(%.3f %.3f) radius_px=%.1f motion=(%.2f %.2f %.2f) speed=%.2f scale=(%.1f %.1f %.1f)",
      g_frame_index,slot_index,created,(int)count,g_runtime_ripple_min_count,
      (double)center,(double)world[0],(double)world[1],
      (double)world[2],(double)world_radius,projected ? "" : "?",
      (double)x,(double)y,
      (double)radius,(double)c->vx,(double)c->vy,
      (double)c->vz,(double)c->speed,
      (double)sx,(double)sy,(double)sz);
    log_line(msg);
    g_ripple_contact_log_frame=g_frame_index;
    diag_consume_line();
  }
}

static int build_contact_values(GLfloat values[16][4]) {
  memset(values,0,sizeof(GLfloat)*16u*4u);
  int slot=0;
  const unsigned int lifetime=240u;
  const unsigned int stale=72u;

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
  const unsigned int lifetime=240u;
  const unsigned int stale=72u;

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

static void invalidate_effective_contact_cache(void) {
  g_effective_contact_cache_valid=0;
  g_effective_contact_cache_frame=0;
  g_effective_contact_cache_count=0;
  g_effective_contact_cache_source=0;
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
  if(g_effective_contact_cache_valid &&
     g_effective_contact_cache_frame==g_frame_index) {
    memcpy(values,g_effective_contact_cache,sizeof(g_effective_contact_cache));
    memcpy(motions,g_effective_contact_motion_cache,
      sizeof(g_effective_contact_motion_cache));
    if(source) *source=g_effective_contact_cache_source;
    return g_effective_contact_cache_count;
  }

  int native_count=build_native_contact_values(values,motions);
  if(native_count>0) {
    int reinforced=reinforce_native_contact_motion(motions,native_count);
    if(source) *source=reinforced ? 3 : 1;
    memcpy(g_effective_contact_cache,values,sizeof(g_effective_contact_cache));
    memcpy(g_effective_contact_motion_cache,motions,
      sizeof(g_effective_contact_motion_cache));
    g_effective_contact_cache_count=native_count;
    g_effective_contact_cache_source=reinforced ? 3 : 1;
    g_effective_contact_cache_frame=g_frame_index;
    g_effective_contact_cache_valid=1;
    return native_count;
  }
  int cpu_count=build_contact_values(values);
  build_contact_motion_values(motions);
  if(source) *source=cpu_count>0 ? 2 : 0;
  memcpy(g_effective_contact_cache,values,sizeof(g_effective_contact_cache));
  memcpy(g_effective_contact_motion_cache,motions,
    sizeof(g_effective_contact_motion_cache));
  g_effective_contact_cache_count=cpu_count;
  g_effective_contact_cache_source=cpu_count>0 ? 2 : 0;
  g_effective_contact_cache_frame=g_frame_index;
  g_effective_contact_cache_valid=1;
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
  intptr_t offset;
  GLint size;
  GLint stride;
  GLenum type;
  GLint normalized;
  GLuint buffer;
  int component_size;
} Tr456AttribSource;

static PFNGLGETATTRIBLOCATION real_get_attrib_location(void);

static int tr456_current_coord_attrib(Tr456AttribSource *out) {
  if(!out || !g_current_program) return 0;
  PFNGLGETATTRIBLOCATION get_attr=real_get_attrib_location();
  PFNGLGETVERTEXATTRIBIV getiv=real_get_vertex_attrib_iv();
  PFNGLGETVERTEXATTRIBPOINTERV getptr=real_get_vertex_attrib_pointer_v();
  if(!get_attr || !getiv || !getptr) return 0;
  ProgramTrack *track=program_track(g_current_program,0);
  GLint loc=-1;
  if(track) {
    if(track->coord_attr_loc==-2)
      track->coord_attr_loc=get_attr(g_current_program,"aCoord");
    loc=track->coord_attr_loc;
  } else {
    loc=get_attr(g_current_program,"aCoord");
  }
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
  else if(type==GL_UNSIGNED_SHORT)
    *out=(GLuint)bytes[0]|((GLuint)bytes[1]<<8);
  else
    *out=(GLuint)bytes[0]|((GLuint)bytes[1]<<8)|
      ((GLuint)bytes[2]<<16)|((GLuint)bytes[3]<<24);
  return 1;
}

#define TR456_SYNTHETIC_CAPTURE_MAX_BUFFER_BYTES (1024*1024)

static GLuint tr456_decode_index_bytes(GLenum type, const unsigned char *bytes) {
  if(type==GL_UNSIGNED_BYTE)
    return (GLuint)bytes[0];
  if(type==GL_UNSIGNED_SHORT)
    return (GLuint)bytes[0]|((GLuint)bytes[1]<<8);
  return (GLuint)bytes[0]|((GLuint)bytes[1]<<8)|
    ((GLuint)bytes[2]<<16)|((GLuint)bytes[3]<<24);
}

static int tr456_decode_coord_bytes(const Tr456AttribSource *attr,
                                    const unsigned char *bytes,
                                    GLfloat out[3]) {
  if(!attr || !bytes || !out || attr->component_size<=0)
    return 0;
  out[0]=tr456_decode_gl_scalar(bytes,attr->type);
  out[1]=tr456_decode_gl_scalar(bytes+attr->component_size,attr->type);
  out[2]=tr456_decode_gl_scalar(bytes+attr->component_size*2,attr->type);
  return 1;
}

typedef struct {
  GLfloat minv[3];
  GLfloat maxv[3];
  int samples;
  int got;
} Tr456SyntheticSurfaceBounds;

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

static int tr456_measure_synthetic_surface_vertices(
    GLsizei count, int has_first, GLint first,
    int has_indices, GLenum type, const void *indices, GLint base_vertex,
    Tr456SyntheticSurfaceBounds *bounds) {
  if(bounds) memset(bounds,0,sizeof(*bounds));
  if(count<=0 || !bounds) return 0;
  load_runtime_config();
  Tr456AttribSource attr;
  if(!tr456_current_coord_attrib(&attr)) return 0;
  int max_samples=g_runtime_synthetic_contact_max_samples;
  if(max_samples<32) max_samples=32;
  if(max_samples>512) max_samples=512;
  int samples=count<max_samples ? (int)count : max_samples;
  bounds->samples=samples;

  GLuint sample_vertices[512];
  int sample_valid[512];
  GLuint min_vertex=0xffffffffu;
  GLuint max_vertex=0u;
  unsigned char *index_cache=0;
  const int index_size=has_indices ? tr456_index_size(type) : 0;

  if(has_indices && index_size>0) {
    const intptr_t index_offset=(intptr_t)(uintptr_t)indices;
    const intptr_t index_bytes=(intptr_t)count*(intptr_t)index_size;
    if(index_offset>=0 && index_bytes>0 &&
       index_bytes<=(intptr_t)TR456_SYNTHETIC_CAPTURE_MAX_BUFFER_BYTES) {
      index_cache=(unsigned char*)malloc((size_t)index_bytes);
      if(index_cache &&
         !tr456_read_bound_buffer(GL_ELEMENT_ARRAY_BUFFER,index_offset,
           index_bytes,index_cache)) {
        free(index_cache);
        index_cache=0;
      }
    }
  }

  int valid_samples=0;
  for(int s=0;s<samples;s++) {
    GLsizei pos=(samples<=1) ? 0 :
      (GLsizei)(((long long)s*(long long)(count-1))/(long long)(samples-1));
    GLuint vertex=0;
    int valid=0;
    if(has_indices) {
      if(index_cache && index_size>0) {
        vertex=tr456_decode_index_bytes(type,
          index_cache+(size_t)pos*(size_t)index_size);
        valid=1;
      } else {
        valid=tr456_read_element_index(type,indices,pos,&vertex);
      }
      if(valid)
        vertex=(GLuint)((GLint)vertex+base_vertex);
    } else {
      vertex=(GLuint)((has_first ? first : 0)+pos);
      valid=1;
    }
    sample_vertices[s]=vertex;
    sample_valid[s]=valid;
    if(valid) {
      if(vertex<min_vertex) min_vertex=vertex;
      if(vertex>max_vertex) max_vertex=vertex;
      valid_samples++;
    }
  }
  free(index_cache);
  if(valid_samples<=0) return 0;

  PFNGLBINDBUFFER bind_buffer=real_bind_buffer();
  if(!bind_buffer) return 0;
  GLint old_array=0;
  shadow_get_integer_or_gl(GL_ARRAY_BUFFER_BINDING,&old_array);
  bind_buffer(GL_ARRAY_BUFFER,attr.buffer);

  const intptr_t vertex_size=(intptr_t)attr.component_size*(intptr_t)attr.size;
  intptr_t vertex_cache_offset=0;
  intptr_t vertex_cache_size=0;
  unsigned char *vertex_cache=0;
  if(vertex_size>0 && min_vertex!=0xffffffffu && max_vertex>=min_vertex) {
    vertex_cache_offset=attr.offset+(intptr_t)min_vertex*(intptr_t)attr.stride;
    const intptr_t last_offset=attr.offset+
      (intptr_t)max_vertex*(intptr_t)attr.stride+vertex_size;
    vertex_cache_size=last_offset-vertex_cache_offset;
    if(vertex_cache_offset>=0 && vertex_cache_size>0 &&
       vertex_cache_size<=(intptr_t)TR456_SYNTHETIC_CAPTURE_MAX_BUFFER_BYTES) {
      vertex_cache=(unsigned char*)malloc((size_t)vertex_cache_size);
      if(vertex_cache &&
         !tr456_read_bound_buffer(GL_ARRAY_BUFFER,vertex_cache_offset,
           vertex_cache_size,vertex_cache)) {
        free(vertex_cache);
        vertex_cache=0;
      }
    }
  }

  GLfloat row[3][4];
  GLfloat view[3][4];
  for(int i=0;i<3;i++) {
    if(!read_uniform_vec4_index_now("uModelMatrix",i,row[i])) {
      free(vertex_cache);
      bind_buffer(GL_ARRAY_BUFFER,(GLuint)old_array);
      return 0;
    }
    if(!read_uniform_vec4_index_now("uViewMatrix",i,view[i])) {
      view[i][0]=0.0f; view[i][1]=0.0f; view[i][2]=0.0f; view[i][3]=0.0f;
    }
  }

  GLfloat minv[3]={1000000000.0f,1000000000.0f,1000000000.0f};
  GLfloat maxv[3]={-1000000000.0f,-1000000000.0f,-1000000000.0f};
  int got=0;
  for(int s=0;s<samples;s++) {
    if(!sample_valid[s]) continue;
    GLuint vertex=sample_vertices[s];
    GLfloat local[3];
    GLfloat world[3];
    if(vertex_cache) {
      const intptr_t rel=attr.offset+(intptr_t)vertex*(intptr_t)attr.stride-
        vertex_cache_offset;
      if(rel<0 || rel+vertex_size>vertex_cache_size ||
         !tr456_decode_coord_bytes(&attr,vertex_cache+rel,local))
        continue;
    } else if(!tr456_read_coord_vertex(&attr,vertex,local)) {
      continue;
    }
    transform_model_point_to_world((const GLfloat (*)[4])row,
      (const GLfloat (*)[4])view,local,world);
    for(int k=0;k<3;k++) {
      if(world[k]<minv[k]) minv[k]=world[k];
      if(world[k]>maxv[k]) maxv[k]=world[k];
    }
    got++;
  }

  free(vertex_cache);
  bind_buffer(GL_ARRAY_BUFFER,(GLuint)old_array);
  if(got<3) return 0;
  memcpy(bounds->minv,minv,sizeof(bounds->minv));
  memcpy(bounds->maxv,maxv,sizeof(bounds->maxv));
  bounds->got=got;
  return 1;
}

static void tr456_record_synthetic_surface_vertices(
    GLsizei count, int has_first, GLint first,
    int has_indices, GLenum type, const void *indices, GLint base_vertex) {
  if(!tr456_wet_lara_synthetic_contact_enabled()) return;
  Tr456SyntheticSurfaceBounds bounds;
  if(tr456_measure_synthetic_surface_vertices(count,has_first,first,
       has_indices,type,indices,base_vertex,&bounds))
    tr456_record_synthetic_bounds(bounds.minv,bounds.maxv,count);
}

static int tr456_synthetic_standing_bounds_allowed(
    const Tr456SyntheticSurfaceBounds *bounds, const char **reason_out) {
  if(reason_out) *reason_out=0;
  if(!bounds || bounds->got<3) {
    if(reason_out) *reason_out="unmeasured";
    return 0;
  }
  const GLfloat width=bounds->maxv[0]-bounds->minv[0];
  const GLfloat height=bounds->maxv[1]-bounds->minv[1];
  const GLfloat depth=bounds->maxv[2]-bounds->minv[2];
  const GLfloat min_xz=f_min(width,depth);
  const GLfloat max_xz=f_max(width,depth);
  const GLfloat height_limit=f_min(f_max(192.0f,min_xz*0.08f),512.0f);
  if(width<24.0f || depth<24.0f) {
    if(reason_out) *reason_out="tiny footprint";
    return 0;
  }
  if(width>65536.0f || depth>65536.0f) {
    if(reason_out) *reason_out="huge footprint";
    return 0;
  }
  if(height>height_limit) {
    if(reason_out) *reason_out="tall footprint";
    return 0;
  }
  if(min_xz<96.0f && max_xz>min_xz*48.0f) {
    if(reason_out) *reason_out="thin strip";
    return 0;
  }
  return 1;
}

static void tr456_log_synthetic_standing_bounds_skip(
    const char *reason, GLsizei count,
    const Tr456SyntheticSurfaceBounds *bounds) {
  if(!runtime_verbose_log() || g_synthetic_standing_bounds_skip_logged>=48u)
    return;
  const GLfloat minx=bounds ? bounds->minv[0] : 0.0f;
  const GLfloat maxx=bounds ? bounds->maxv[0] : 0.0f;
  const GLfloat miny=bounds ? bounds->minv[1] : 0.0f;
  const GLfloat maxy=bounds ? bounds->maxv[1] : 0.0f;
  const GLfloat minz=bounds ? bounds->minv[2] : 0.0f;
  const GLfloat maxz=bounds ? bounds->maxv[2] : 0.0f;
  const int got=bounds ? bounds->got : 0;
  char msg[384];
  snprintf(msg,sizeof(msg),
    "standing water bounds guard skipped frame=%u program=%u count=%d reason=%s got=%d x=%.1f..%.1f y=%.1f..%.1f z=%.1f..%.1f",
    g_frame_index,g_current_program,(int)count,
    reason ? reason : "unknown",got,
    (double)minx,(double)maxx,(double)miny,(double)maxy,
    (double)minz,(double)maxz);
  log_line(msg);
  g_synthetic_standing_bounds_skip_logged++;
}

static int tr456_prepare_synthetic_surface_geometry(
    GLenum mode, GLsizei count, int count_known,
    int has_first, GLint first, int has_indices, GLenum type,
    const void *indices, GLint base_vertex) {
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  if(!decision->synthetic_standing ||
     g_current_program_type!=SHADER_WATER_SURFACE) {
    tr456_record_synthetic_surface_vertices(count,has_first,first,
      has_indices,type,indices,base_vertex);
    return 1;
  }

  load_runtime_config();
  if(!g_runtime_synthetic_standing_bounds_guard) {
    tr456_record_synthetic_surface_vertices(count,has_first,first,
      has_indices,type,indices,base_vertex);
    return 1;
  }

  Tr456SyntheticSurfaceBounds bounds;
  if(!tr456_measure_synthetic_surface_vertices(count,has_first,first,
       has_indices,type,indices,base_vertex,&bounds)) {
    tr456_log_synthetic_standing_bounds_skip("unmeasured",count,0);
    return 0;
  }
  const char *reason=0;
  if(!tr456_synthetic_standing_bounds_allowed(&bounds,&reason)) {
    tr456_log_synthetic_standing_bounds_skip(reason,count,&bounds);
    return 0;
  }
  if(tr456_wet_lara_synthetic_contact_enabled())
    tr456_record_synthetic_bounds(bounds.minv,bounds.maxv,count);
  return 1;
}

#include "tr456_proxy_flow_runtime.inc"

#include "tr456_proxy_programs.inc"

#include "tr456_proxy_gl_hooks.inc"

#include "tr456_proxy_draw_dispatch.inc"

#include "tr456_proxy_wgl_exports.inc"

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved) {
  (void)reserved;
  if(reason==DLL_PROCESS_ATTACH) {
    g_self=(HMODULE)inst;
    DisableThreadLibraryCalls(inst);
    set_dir();
#if TR456_STARTUP_LOG
    g_prev_exception_filter=
      SetUnhandledExceptionFilter(tr456_unhandled_exception_filter);
    {
      char self_path[MAX_PATH]="";
      char exe_path[MAX_PATH]="";
      char cwd[MAX_PATH]="";
      GetModuleFileNameA(g_self,self_path,MAX_PATH);
      GetModuleFileNameA(0,exe_path,MAX_PATH);
      GetCurrentDirectoryA(MAX_PATH,cwd);
      boot_logf("DllMain attach self=\"%s\" exe=\"%s\" cwd=\"%s\" dir=\"%s\" mod=\"%s\" cmd=\"%s\"",
        self_path,exe_path,cwd,g_dir,g_mod_dir,GetCommandLineA());
    }
#endif
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
    boot_log_line("DllMain detach");
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
