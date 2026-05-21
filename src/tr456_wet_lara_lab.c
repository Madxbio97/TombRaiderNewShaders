#include "tr456_wet_lara_lab.h"

typedef struct {
  GLuint program;
  GLint attr_coord;
  GLint attr_normal;
  GLint attr_light;
  GLint attr_color;
  GLint loc_proj;
  GLint loc_model;
  GLint loc_view;
  GLint loc_joints;
  GLint loc_info;
  GLint loc_tint;
  GLint loc_timing;
  GLint loc_detail;
  GLint loc_drops;
  GLint loc_light_pos;
  GLint loc_light_col;
  int ready;
  int failed;
  int use_joints;
} Tr456WetLaraProgram;

#define TR456_WET_LARA_PROGRAM_COUNT 32

typedef struct {
  int loaded;
  int enabled;
  int contact_only;
  int object_gate;
  int require_normal;
  int use_draw_counts;
  int use_timing_draw;
  int use_water_contact;
  int use_ripple_circle;
  int use_synthetic_contact;
  int underwater_sustain;
  int underwater_can_start;
  int ripple_circle_new_only;
  int ripple_circle_allow_screen;
  int ripple_min_count;
  int full_body_from_timing;
  int partial_wet;
  int partial_carry_after_exit;
  int contact_fallback;
  int use_joints;
  int require_joints;
  int debug_visible;
  int trace_log;
  int trace_interval_frames;
  int min_count;
  int max_count;
  int max_per_frame;
  int draw_count_count;
  int draw_counts[16];
  int timing_count;
  int hold_frames;
  int dry_frames;
  int exit_grace_frames;
  float dry_seconds;
  int frame_draws;
  int last_frame_draws;
  int frame_candidates;
  int last_frame_candidates;
  int frame_timing_draws;
  int last_frame_timing_draws;
  int frame_start;
  int frame_end;
  int water_enter_frames;
  int water_grace_frames;
  int water_min_joints;
  int water_streak;
  int synthetic_enter_frames;
  int synthetic_grace_frames;
  int synthetic_min_joints;
  int synthetic_surface_age_frames;
  int synthetic_streak;
  int underwater_min_joints;
  int ripple_grace_frames;
  unsigned int diag_log_frame;
  unsigned int timing_log_frame;
  unsigned int timing_log_count;
  unsigned int contact_log_frame;
  unsigned int contact_log_count;
  unsigned int contact_miss_log_frame;
  unsigned int contact_miss_log_count;
  unsigned int synthetic_log_frame;
  unsigned int synthetic_log_count;
  unsigned int synthetic_miss_log_frame;
  unsigned int synthetic_miss_log_count;
  unsigned int underwater_log_frame;
  unsigned int underwater_log_count;
  unsigned int trace_log_frame;
  unsigned int trace_last_period_frame;
  int trace_last_phase;
  int trace_last_mask;
  float trace_last_wet;
  unsigned int last_timing_frame;
  unsigned int last_water_frame;
  unsigned int last_ripple_frame;
  unsigned int last_synthetic_frame;
  unsigned int last_underwater_frame;
  unsigned int last_active_source_frame;
  unsigned int last_raw_active_mask;
  unsigned int last_lara_pose_frame;
  unsigned int last_water_plane_frame;
  unsigned int last_drip_frame;
  unsigned long long last_timing_ms;
  unsigned long long last_water_ms;
  unsigned long long last_ripple_ms;
  unsigned long long last_synthetic_ms;
  unsigned long long last_underwater_ms;
  unsigned int water_eval_frame;
  unsigned int synthetic_eval_frame;
  unsigned int ripple_log_frame;
  unsigned int ripple_log_count;
  unsigned long long wet_contact_start_ms;
  unsigned long long wet_last_update_ms;
  unsigned int wet_last_update_frame;
  float wet_amount;
  float wet_delay_seconds;
  float wet_ramp_seconds;
  float opacity;
  float specular;
  float droplet_strength;
  float streak_strength;
  float cloth_darkening;
  int contact_ripples;
  int drip_ripples;
  int drip_interval_frames;
  float contact_ripple_radius;
  float contact_ripple_strength;
  float drip_ripple_radius;
  float drip_ripple_strength;
  float last_lara_x;
  float last_lara_y;
  float last_lara_z;
  float last_water_y;
  float partial_rise;
  float partial_fade;
  float partial_full_margin;
  float partial_carry_max_delta;
  float partial_direction;
  float partial_line_y;
  float partial_line_dir;
  float partial_water_y;
  float partial_body_min_y;
  float partial_body_max_y;
  float underwater_margin;
  float tint_r;
  float tint_g;
  float tint_b;
  float radius_scale;
  float vertical_radius;
  float fallback_radius_scale;
  float fallback_vertical;
  float fallback_min_speed;
  float synthetic_margin;
  float synthetic_vertical;
  float synthetic_above_surface;
  float depth_bias;
  int partial_line_valid;
  Tr456WetLaraProgram programs[TR456_WET_LARA_PROGRAM_COUNT];
} Tr456WetLaraState;

typedef struct {
  GLint old_program;
  GLint old_blend;
  GLint old_depth;
  GLint old_cull;
  GLint old_depth_mask;
  GLint old_depth_func;
  GLint old_blend_func[4];
} Tr456WetLaraDrawState;

static Tr456WetLaraState g_wet_lara;

static void tr456_wet_lara_add_draw_count(int count) {
  if(count<=0 || g_wet_lara.draw_count_count>=
     (int)(sizeof(g_wet_lara.draw_counts)/
     sizeof(g_wet_lara.draw_counts[0])))
    return;
  for(int i=0;i<g_wet_lara.draw_count_count;i++) {
    if(g_wet_lara.draw_counts[i]==count)
      return;
  }
  g_wet_lara.draw_counts[g_wet_lara.draw_count_count++]=count;
}

static void tr456_wet_lara_load_config(void) {
  if(g_wet_lara.loaded) return;
  static const int default_lara_counts[]={42735,11016,22140,11904};
  g_wet_lara.enabled=ini_int("WetLara",0);
  g_wet_lara.contact_only=ini_int("WetLaraContactOnly",1);
  g_wet_lara.object_gate=ini_int("WetLaraObjectGate",0);
  g_wet_lara.require_normal=ini_int("WetLaraRequireNormal",1);
  g_wet_lara.use_draw_counts=ini_int("WetLaraUseDrawCounts",1);
  g_wet_lara.use_timing_draw=ini_int("WetLaraUseTimingDraw",1);
  g_wet_lara.use_water_contact=ini_int("WetLaraUseWaterContact",1);
  g_wet_lara.use_ripple_circle=ini_int("WetLaraUseRippleCircle",1);
  g_wet_lara.use_synthetic_contact=ini_int("WetLaraUseSyntheticContact",0);
  g_wet_lara.underwater_sustain=ini_int("WetLaraUnderwaterSustain",1);
  g_wet_lara.underwater_can_start=ini_int("WetLaraUnderwaterCanStart",0);
  g_wet_lara.ripple_circle_new_only=ini_int("WetLaraRippleCircleNewOnly",1);
  g_wet_lara.ripple_circle_allow_screen=
    ini_int("WetLaraRippleCircleAllowScreen",0);
  g_wet_lara.ripple_min_count=ini_int("WetLaraRippleMinCount",0);
  g_wet_lara.full_body_from_timing=ini_int("WetLaraFullBodyFromTiming",0);
  g_wet_lara.partial_wet=ini_int("WetLaraPartialWet",1);
  g_wet_lara.partial_carry_after_exit=ini_int("WetLaraPartialCarryAfterExit",1);
  g_wet_lara.contact_fallback=ini_int("WetLaraContactFallback",1);
  g_wet_lara.use_joints=ini_int("WetLaraUseJoints",1);
  g_wet_lara.require_joints=ini_int("WetLaraRequireJoints",1);
  g_wet_lara.debug_visible=ini_int("WetLaraDebugVisible",0);
  g_wet_lara.trace_log=ini_int("WetLaraTraceLog",0);
  g_wet_lara.trace_interval_frames=
    ini_int("WetLaraTraceIntervalFrames",30);
  g_wet_lara.min_count=ini_int("WetLaraMinCount",24);
  g_wet_lara.max_count=ini_int("WetLaraMaxCount",12000);
  g_wet_lara.max_per_frame=ini_int("WetLaraMaxPerFrame",8);
  g_wet_lara.draw_count_count=0;
  for(int i=0;i<16;i++) {
    char key[48];
    int fallback=i<(int)(sizeof(default_lara_counts)/
      sizeof(default_lara_counts[0])) ? default_lara_counts[i] : -1;
    snprintf(key,sizeof(key),"WetLaraDrawCount%d",i);
    tr456_wet_lara_add_draw_count(ini_int(key,fallback));
  }
  if(g_wet_lara.draw_count_count<=0)
    g_wet_lara.use_draw_counts=0;
  g_wet_lara.timing_count=ini_int("WetLaraTimingDrawCount",1194);
  g_wet_lara.hold_frames=ini_int("WetLaraHoldFrames",0);
  g_wet_lara.dry_frames=ini_int("WetLaraDryFrames",1200);
  g_wet_lara.exit_grace_frames=ini_int("WetLaraExitGraceFrames",24);
  g_wet_lara.dry_seconds=ini_float("WetLaraDrySeconds",20.0f);
  g_wet_lara.frame_start=ini_int("WetLaraFrameStart",-1);
  g_wet_lara.frame_end=ini_int("WetLaraFrameEnd",-1);
  g_wet_lara.water_enter_frames=ini_int("WetLaraWaterEnterFrames",3);
  g_wet_lara.water_grace_frames=ini_int("WetLaraWaterGraceFrames",12);
  g_wet_lara.water_min_joints=ini_int("WetLaraWaterMinJoints",3);
  g_wet_lara.synthetic_enter_frames=
    ini_int("WetLaraSyntheticEnterFrames",2);
  g_wet_lara.synthetic_grace_frames=
    ini_int("WetLaraSyntheticGraceFrames",12);
  g_wet_lara.synthetic_min_joints=ini_int("WetLaraSyntheticMinJoints",2);
  g_wet_lara.synthetic_surface_age_frames=
    ini_int("WetLaraSyntheticSurfaceAgeFrames",12);
  g_wet_lara.underwater_min_joints=ini_int("WetLaraUnderwaterMinJoints",8);
  g_wet_lara.ripple_grace_frames=ini_int("WetLaraRippleGraceFrames",0);
  g_wet_lara.wet_delay_seconds=ini_float("WetLaraWetDelaySeconds",1.0f);
  g_wet_lara.wet_ramp_seconds=ini_float("WetLaraWetRampSeconds",1.25f);
  g_wet_lara.opacity=ini_float("WetLaraOpacity",0.56f);
  g_wet_lara.specular=ini_float("WetLaraSpecular",4.00f);
  g_wet_lara.droplet_strength=ini_float("WetLaraDropletStrength",0.42f);
  g_wet_lara.streak_strength=ini_float("WetLaraStreakStrength",0.34f);
  g_wet_lara.cloth_darkening=ini_float("WetLaraClothDarkening",2.00f);
  g_wet_lara.contact_ripples=ini_int("WetLaraContactRipples",1);
  g_wet_lara.drip_ripples=ini_int("WetLaraDripRipples",1);
  g_wet_lara.drip_interval_frames=ini_int("WetLaraDripIntervalFrames",18);
  g_wet_lara.contact_ripple_radius=
    ini_float("WetLaraContactRippleRadius",185.0f);
  g_wet_lara.contact_ripple_strength=
    ini_float("WetLaraContactRippleStrength",1.18f);
  g_wet_lara.drip_ripple_radius=ini_float("WetLaraDripRippleRadius",86.0f);
  g_wet_lara.drip_ripple_strength=
    ini_float("WetLaraDripRippleStrength",0.38f);
  g_wet_lara.partial_rise=ini_float("WetLaraPartialRise",80.0f);
  g_wet_lara.partial_fade=ini_float("WetLaraPartialFade",90.0f);
  g_wet_lara.partial_full_margin=ini_float("WetLaraPartialFullMargin",45.0f);
  g_wet_lara.partial_carry_max_delta=
    ini_float("WetLaraPartialCarryMaxDelta",4096.0f);
  g_wet_lara.partial_direction=ini_float("WetLaraPartialDirection",-1.0f);
  g_wet_lara.underwater_margin=ini_float("WetLaraUnderwaterMargin",96.0f);
  g_wet_lara.tint_r=ini_float("WetLaraTintR",1.05f);
  g_wet_lara.tint_g=ini_float("WetLaraTintG",0.96f);
  g_wet_lara.tint_b=ini_float("WetLaraTintB",0.86f);
  g_wet_lara.radius_scale=ini_float("WetLaraRadiusScale",2.35f);
  g_wet_lara.vertical_radius=ini_float("WetLaraVerticalRadius",1550.0f);
  g_wet_lara.fallback_radius_scale=
    ini_float("WetLaraWaterContactRadius",
      ini_float("WetLaraContactFallbackRadius",2.50f));
  g_wet_lara.fallback_vertical=
    ini_float("WetLaraWaterContactVertical",
      ini_float("WetLaraContactFallbackVertical",520.0f));
  g_wet_lara.fallback_min_speed=
    ini_float("WetLaraWaterContactMinSpeed",
      ini_float("WetLaraContactFallbackMinSpeed",0.0f));
  g_wet_lara.synthetic_margin=ini_float("WetLaraSyntheticMargin",96.0f);
  g_wet_lara.synthetic_vertical=ini_float("WetLaraSyntheticVertical",720.0f);
  g_wet_lara.synthetic_above_surface=
    ini_float("WetLaraSyntheticAboveSurface",64.0f);
  g_wet_lara.depth_bias=ini_float("WetLaraDepthBias",0.00018f);
  if(g_wet_lara.min_count<3) g_wet_lara.min_count=3;
  if(g_wet_lara.max_count<g_wet_lara.min_count)
    g_wet_lara.max_count=g_wet_lara.min_count;
  if(g_wet_lara.timing_count<0) g_wet_lara.timing_count=0;
  if(g_wet_lara.hold_frames<0) g_wet_lara.hold_frames=0;
  if(g_wet_lara.hold_frames>3600) g_wet_lara.hold_frames=3600;
  if(g_wet_lara.dry_frames<0) g_wet_lara.dry_frames=0;
  if(g_wet_lara.dry_frames>3600) g_wet_lara.dry_frames=3600;
  if(g_wet_lara.exit_grace_frames<0) g_wet_lara.exit_grace_frames=0;
  if(g_wet_lara.exit_grace_frames>240)
    g_wet_lara.exit_grace_frames=240;
  if(g_wet_lara.dry_seconds<0.0f) g_wet_lara.dry_seconds=0.0f;
  if(g_wet_lara.dry_seconds>120.0f) g_wet_lara.dry_seconds=120.0f;
  if(g_wet_lara.water_enter_frames<1) g_wet_lara.water_enter_frames=1;
  if(g_wet_lara.water_enter_frames>60) g_wet_lara.water_enter_frames=60;
  if(g_wet_lara.water_grace_frames<0) g_wet_lara.water_grace_frames=0;
  if(g_wet_lara.water_grace_frames>240) g_wet_lara.water_grace_frames=240;
  if(g_wet_lara.water_min_joints<1) g_wet_lara.water_min_joints=1;
  if(g_wet_lara.water_min_joints>32) g_wet_lara.water_min_joints=32;
  if(g_wet_lara.synthetic_enter_frames<1)
    g_wet_lara.synthetic_enter_frames=1;
  if(g_wet_lara.synthetic_enter_frames>30)
    g_wet_lara.synthetic_enter_frames=30;
  if(g_wet_lara.synthetic_grace_frames<0)
    g_wet_lara.synthetic_grace_frames=0;
  if(g_wet_lara.synthetic_grace_frames>120)
    g_wet_lara.synthetic_grace_frames=120;
  if(g_wet_lara.synthetic_min_joints<1)
    g_wet_lara.synthetic_min_joints=1;
  if(g_wet_lara.synthetic_min_joints>32)
    g_wet_lara.synthetic_min_joints=32;
  if(g_wet_lara.underwater_sustain<0) g_wet_lara.underwater_sustain=0;
  if(g_wet_lara.underwater_sustain>1) g_wet_lara.underwater_sustain=1;
  if(g_wet_lara.underwater_can_start<0)
    g_wet_lara.underwater_can_start=0;
  if(g_wet_lara.underwater_can_start>1)
    g_wet_lara.underwater_can_start=1;
  if(g_wet_lara.underwater_min_joints<1)
    g_wet_lara.underwater_min_joints=1;
  if(g_wet_lara.underwater_min_joints>32)
    g_wet_lara.underwater_min_joints=32;
  if(g_wet_lara.synthetic_surface_age_frames<1)
    g_wet_lara.synthetic_surface_age_frames=1;
  if(g_wet_lara.synthetic_surface_age_frames>120)
    g_wet_lara.synthetic_surface_age_frames=120;
  if(g_wet_lara.ripple_grace_frames<0) g_wet_lara.ripple_grace_frames=0;
  if(g_wet_lara.ripple_grace_frames>240)
    g_wet_lara.ripple_grace_frames=240;
  if(g_wet_lara.ripple_min_count<0) g_wet_lara.ripple_min_count=0;
  if(g_wet_lara.ripple_min_count>4096) g_wet_lara.ripple_min_count=4096;
  if(g_wet_lara.wet_delay_seconds<0.0f)
    g_wet_lara.wet_delay_seconds=0.0f;
  if(g_wet_lara.wet_delay_seconds>10.0f)
    g_wet_lara.wet_delay_seconds=10.0f;
  if(g_wet_lara.wet_ramp_seconds<0.001f)
    g_wet_lara.wet_ramp_seconds=0.001f;
  if(g_wet_lara.wet_ramp_seconds>10.0f)
    g_wet_lara.wet_ramp_seconds=10.0f;
  if(g_wet_lara.partial_wet<0) g_wet_lara.partial_wet=0;
  if(g_wet_lara.partial_wet>1) g_wet_lara.partial_wet=1;
  if(g_wet_lara.partial_carry_after_exit<0)
    g_wet_lara.partial_carry_after_exit=0;
  if(g_wet_lara.partial_carry_after_exit>1)
    g_wet_lara.partial_carry_after_exit=1;
  if(g_wet_lara.contact_fallback<0) g_wet_lara.contact_fallback=0;
  if(g_wet_lara.contact_fallback>1) g_wet_lara.contact_fallback=1;
  if(g_wet_lara.debug_visible<0) g_wet_lara.debug_visible=0;
  if(g_wet_lara.debug_visible>1) g_wet_lara.debug_visible=1;
  if(g_wet_lara.trace_log<0) g_wet_lara.trace_log=0;
  if(g_wet_lara.trace_log>1) g_wet_lara.trace_log=1;
  if(g_wet_lara.trace_interval_frames<1)
    g_wet_lara.trace_interval_frames=1;
  if(g_wet_lara.trace_interval_frames>600)
    g_wet_lara.trace_interval_frames=600;
  if(g_wet_lara.max_per_frame<1) g_wet_lara.max_per_frame=1;
  if(g_wet_lara.max_per_frame>128) g_wet_lara.max_per_frame=128;
  if(g_wet_lara.frame_end>=0 && g_wet_lara.frame_end<g_wet_lara.frame_start)
    g_wet_lara.frame_end=g_wet_lara.frame_start;
  if(g_wet_lara.opacity<0.0f) g_wet_lara.opacity=0.0f;
  if(g_wet_lara.opacity>0.80f) g_wet_lara.opacity=0.80f;
  if(g_wet_lara.specular<0.0f) g_wet_lara.specular=0.0f;
  if(g_wet_lara.specular>4.0f) g_wet_lara.specular=4.0f;
  if(g_wet_lara.droplet_strength<0.0f)
    g_wet_lara.droplet_strength=0.0f;
  if(g_wet_lara.droplet_strength>2.0f)
    g_wet_lara.droplet_strength=2.0f;
  if(g_wet_lara.streak_strength<0.0f)
    g_wet_lara.streak_strength=0.0f;
  if(g_wet_lara.streak_strength>2.0f)
    g_wet_lara.streak_strength=2.0f;
  if(g_wet_lara.cloth_darkening<0.0f)
    g_wet_lara.cloth_darkening=0.0f;
  if(g_wet_lara.cloth_darkening>2.0f)
    g_wet_lara.cloth_darkening=2.0f;
  if(g_wet_lara.contact_ripples<0) g_wet_lara.contact_ripples=0;
  if(g_wet_lara.contact_ripples>1) g_wet_lara.contact_ripples=1;
  if(g_wet_lara.drip_ripples<0) g_wet_lara.drip_ripples=0;
  if(g_wet_lara.drip_ripples>1) g_wet_lara.drip_ripples=1;
  if(g_wet_lara.drip_interval_frames<4)
    g_wet_lara.drip_interval_frames=4;
  if(g_wet_lara.drip_interval_frames>180)
    g_wet_lara.drip_interval_frames=180;
  if(g_wet_lara.contact_ripple_radius<24.0f)
    g_wet_lara.contact_ripple_radius=24.0f;
  if(g_wet_lara.contact_ripple_radius>680.0f)
    g_wet_lara.contact_ripple_radius=680.0f;
  if(g_wet_lara.contact_ripple_strength<0.0f)
    g_wet_lara.contact_ripple_strength=0.0f;
  if(g_wet_lara.contact_ripple_strength>4.0f)
    g_wet_lara.contact_ripple_strength=4.0f;
  if(g_wet_lara.drip_ripple_radius<18.0f)
    g_wet_lara.drip_ripple_radius=18.0f;
  if(g_wet_lara.drip_ripple_radius>320.0f)
    g_wet_lara.drip_ripple_radius=320.0f;
  if(g_wet_lara.drip_ripple_strength<0.0f)
    g_wet_lara.drip_ripple_strength=0.0f;
  if(g_wet_lara.drip_ripple_strength>2.0f)
    g_wet_lara.drip_ripple_strength=2.0f;
  if(g_wet_lara.partial_rise<0.0f) g_wet_lara.partial_rise=0.0f;
  if(g_wet_lara.partial_rise>2048.0f)
    g_wet_lara.partial_rise=2048.0f;
  if(g_wet_lara.partial_fade<1.0f) g_wet_lara.partial_fade=1.0f;
  if(g_wet_lara.partial_fade>2048.0f)
    g_wet_lara.partial_fade=2048.0f;
  if(g_wet_lara.partial_full_margin<0.0f)
    g_wet_lara.partial_full_margin=0.0f;
  if(g_wet_lara.partial_full_margin>2048.0f)
    g_wet_lara.partial_full_margin=2048.0f;
  if(g_wet_lara.partial_carry_max_delta<64.0f)
    g_wet_lara.partial_carry_max_delta=64.0f;
  if(g_wet_lara.partial_carry_max_delta>16384.0f)
    g_wet_lara.partial_carry_max_delta=16384.0f;
  if(g_wet_lara.partial_direction>=0.0f)
    g_wet_lara.partial_direction=1.0f;
  else
    g_wet_lara.partial_direction=-1.0f;
  if(g_wet_lara.underwater_margin<0.0f)
    g_wet_lara.underwater_margin=0.0f;
  if(g_wet_lara.underwater_margin>1024.0f)
    g_wet_lara.underwater_margin=1024.0f;
  if(g_wet_lara.tint_r<0.0f) g_wet_lara.tint_r=0.0f;
  if(g_wet_lara.tint_r>2.0f) g_wet_lara.tint_r=2.0f;
  if(g_wet_lara.tint_g<0.0f) g_wet_lara.tint_g=0.0f;
  if(g_wet_lara.tint_g>2.0f) g_wet_lara.tint_g=2.0f;
  if(g_wet_lara.tint_b<0.0f) g_wet_lara.tint_b=0.0f;
  if(g_wet_lara.tint_b>2.0f) g_wet_lara.tint_b=2.0f;
  if(g_wet_lara.radius_scale<0.25f) g_wet_lara.radius_scale=0.25f;
  if(g_wet_lara.radius_scale>8.0f) g_wet_lara.radius_scale=8.0f;
  if(g_wet_lara.vertical_radius<64.0f) g_wet_lara.vertical_radius=64.0f;
  if(g_wet_lara.vertical_radius>4096.0f)
    g_wet_lara.vertical_radius=4096.0f;
  if(g_wet_lara.fallback_radius_scale<0.50f)
    g_wet_lara.fallback_radius_scale=0.50f;
  if(g_wet_lara.fallback_radius_scale>12.0f)
    g_wet_lara.fallback_radius_scale=12.0f;
  if(g_wet_lara.fallback_vertical<64.0f)
    g_wet_lara.fallback_vertical=64.0f;
  if(g_wet_lara.fallback_vertical>4096.0f)
    g_wet_lara.fallback_vertical=4096.0f;
  if(g_wet_lara.fallback_min_speed<0.0f)
    g_wet_lara.fallback_min_speed=0.0f;
  if(g_wet_lara.fallback_min_speed>64.0f)
    g_wet_lara.fallback_min_speed=64.0f;
  if(g_wet_lara.synthetic_margin<0.0f) g_wet_lara.synthetic_margin=0.0f;
  if(g_wet_lara.synthetic_margin>512.0f) g_wet_lara.synthetic_margin=512.0f;
  if(g_wet_lara.synthetic_vertical<64.0f) g_wet_lara.synthetic_vertical=64.0f;
  if(g_wet_lara.synthetic_vertical>4096.0f)
    g_wet_lara.synthetic_vertical=4096.0f;
  if(g_wet_lara.synthetic_above_surface<0.0f)
    g_wet_lara.synthetic_above_surface=0.0f;
  if(g_wet_lara.synthetic_above_surface>512.0f)
    g_wet_lara.synthetic_above_surface=512.0f;
  if(g_wet_lara.depth_bias<0.0f) g_wet_lara.depth_bias=0.0f;
  if(g_wet_lara.depth_bias>0.004f) g_wet_lara.depth_bias=0.004f;
  g_wet_lara.loaded=1;
}

static int tr456_wet_lara_triangle_mode(GLenum mode) {
  return mode==GL_TRIANGLES || mode==GL_TRIANGLE_STRIP ||
    mode==GL_TRIANGLE_FAN;
}

static unsigned long long tr456_wet_lara_now_ms(void) {
  return (unsigned long long)GetTickCount64();
}

static float tr456_wet_lara_smooth(float t) {
  t=f_min(f_max(t,0.0f),1.0f);
  return t*t*(3.0f-2.0f*t);
}

static int tr456_wet_lara_source_active(unsigned int last_frame,
                                        int grace_frames) {
  if(!last_frame)
    return 0;
  unsigned int age=g_frame_index>=last_frame ? (g_frame_index-last_frame) : 0u;
  return age<=(unsigned int)grace_frames;
}

static int tr456_wet_lara_draw_count_matches(GLsizei count, int count_known) {
  if(!g_wet_lara.use_draw_counts)
    return 1;
  if(!count_known)
    return 0;
  for(int i=0;i<g_wet_lara.draw_count_count;i++) {
    if(count==g_wet_lara.draw_counts[i])
      return 1;
  }
  return 0;
}

static void tr456_wet_lara_note_draw(const char *call, GLsizei count,
                                     int count_known) {
  tr456_wet_lara_load_config();
  if(!g_wet_lara.enabled || !g_wet_lara.use_timing_draw ||
     g_wet_lara.timing_count<=0 ||
     !count_known || count!=g_wet_lara.timing_count)
    return;
  g_wet_lara.last_timing_frame=g_frame_index;
  g_wet_lara.last_timing_ms=tr456_wet_lara_now_ms();
  g_wet_lara.frame_timing_draws++;
  if((diag_is_active() || g_wet_lara.timing_log_count<24u) &&
     g_wet_lara.timing_log_frame!=g_frame_index) {
    char msg[256];
    snprintf(msg,sizeof(msg),
      "wet lara timing session=%d frame=%u call=%s count=%d timingCount=%d",
      g_diag_session,g_frame_index,call ? call : "draw",(int)count,
      g_wet_lara.timing_count);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.timing_log_count++;
    g_wet_lara.timing_log_frame=g_frame_index;
  }
}

static void tr456_wet_lara_note_multi_draw(const char *call,
                                           const GLsizei *count,
                                           GLsizei draw_count) {
  if(!count || draw_count<=0)
    return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_wet_lara_note_draw(call,count[i],1);
}

static void tr456_wet_lara_note_ripple_circle(int slot, int created,
                                              GLsizei count, GLfloat x,
                                              GLfloat y, GLfloat z,
                                              GLfloat radius, GLfloat speed,
                                              int screen_contact) {
  tr456_wet_lara_load_config();
  if(!g_wet_lara.enabled || !g_wet_lara.use_ripple_circle)
    return;
  if(g_wet_lara.ripple_circle_new_only && !created)
    return;
  if(screen_contact && !g_wet_lara.ripple_circle_allow_screen)
    return;
  if(g_wet_lara.ripple_min_count>0 && count<g_wet_lara.ripple_min_count)
    return;
  g_wet_lara.last_ripple_frame=g_frame_index;
  g_wet_lara.last_ripple_ms=tr456_wet_lara_now_ms();
  if((diag_is_active() || g_wet_lara.ripple_log_count<32u) &&
     g_wet_lara.ripple_log_frame!=g_frame_index) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "wet lara ripple circle session=%d frame=%u wetAmount=1.000 slot=%d created=%d count=%d world=(%.1f %.1f %.1f) radius=%.1f speed=%.2f screen=%d",
      g_diag_session,g_frame_index,slot,created,(int)count,(double)x,
      (double)y,(double)z,(double)radius,(double)speed,screen_contact);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.ripple_log_count++;
    g_wet_lara.ripple_log_frame=g_frame_index;
  }
}

static void tr456_wet_lara_note_water_contact(int slot, int joint,
                                              int joint_count, float dist,
                                              float dy, float radius,
                                              float speed) {
  if(!g_wet_lara.enabled || !g_wet_lara.use_water_contact)
    return;
  g_wet_lara.last_water_frame=g_frame_index;
  g_wet_lara.last_water_ms=tr456_wet_lara_now_ms();
  if((diag_is_active() || g_wet_lara.contact_log_count<24u) &&
     g_wet_lara.contact_log_frame!=g_frame_index) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "wet lara water contact session=%d frame=%u wetAmount=1.000 slot=%d joint=%d joints=%d dist=%.1f dy=%.1f radius=%.1f speed=%.2f streak=%d",
      g_diag_session,g_frame_index,slot,joint,joint_count,(double)dist,
      (double)dy,(double)radius,(double)speed,g_wet_lara.water_streak);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.contact_log_count++;
    g_wet_lara.contact_log_frame=g_frame_index;
  }
}

static void tr456_wet_lara_note_synthetic_contact(int surface, int joint,
                                                  int joint_count,
                                                  float dist, float dy,
                                                  float water_y) {
  if(!g_wet_lara.enabled || !g_wet_lara.use_synthetic_contact)
    return;
  g_wet_lara.last_synthetic_frame=g_frame_index;
  g_wet_lara.last_synthetic_ms=tr456_wet_lara_now_ms();
  if((diag_is_active() || g_wet_lara.synthetic_log_count<24u) &&
     g_wet_lara.synthetic_log_frame!=g_frame_index) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "wet lara synthetic contact session=%d frame=%u wetAmount=1.000 surface=%d joint=%d joints=%d dist=%.1f dy=%.1f waterY=%.1f streak=%d",
      g_diag_session,g_frame_index,surface,joint,joint_count,(double)dist,
      (double)dy,(double)water_y,g_wet_lara.synthetic_streak);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.synthetic_log_count++;
    g_wet_lara.synthetic_log_frame=g_frame_index;
  }
}

#define TR456_WET_LARA_SRC_WATER      0x01
#define TR456_WET_LARA_SRC_TIMING     0x02
#define TR456_WET_LARA_SRC_RIPPLE     0x04
#define TR456_WET_LARA_SRC_SYNTHETIC  0x08
#define TR456_WET_LARA_SRC_UNDERWATER 0x10

static int tr456_wet_lara_active_source_mask(void) {
  int mask=0;
  if(g_wet_lara.use_water_contact &&
     tr456_wet_lara_source_active(g_wet_lara.last_water_frame,
       g_wet_lara.water_grace_frames))
    mask|=TR456_WET_LARA_SRC_WATER;
  if(g_wet_lara.use_timing_draw &&
     tr456_wet_lara_source_active(g_wet_lara.last_timing_frame,
       g_wet_lara.hold_frames))
    mask|=TR456_WET_LARA_SRC_TIMING;
  if(g_wet_lara.use_ripple_circle &&
     tr456_wet_lara_source_active(g_wet_lara.last_ripple_frame,
       g_wet_lara.ripple_grace_frames))
    mask|=TR456_WET_LARA_SRC_RIPPLE;
  if(g_wet_lara.use_synthetic_contact &&
     tr456_wet_lara_source_active(g_wet_lara.last_synthetic_frame,
       g_wet_lara.synthetic_grace_frames))
    mask|=TR456_WET_LARA_SRC_SYNTHETIC;
  int underwater_holds_existing=g_wet_lara.underwater_can_start ||
    g_wet_lara.wet_amount>0.002f ||
    tr456_wet_lara_source_active(g_wet_lara.last_synthetic_frame,
      g_wet_lara.synthetic_grace_frames) ||
    tr456_wet_lara_source_active(g_wet_lara.last_water_frame,
      g_wet_lara.water_grace_frames);
  if(g_wet_lara.underwater_sustain && underwater_holds_existing &&
     tr456_wet_lara_source_active(g_wet_lara.last_underwater_frame,
       g_wet_lara.synthetic_grace_frames))
    mask|=TR456_WET_LARA_SRC_UNDERWATER;
  return mask;
}

static unsigned long long tr456_wet_lara_resume_start_ms(
    unsigned long long now) {
  float seed=f_min(f_max(g_wet_lara.wet_amount,0.0f),1.0f);
  if(seed<=0.001f)
    return now;
  float age_sec=g_wet_lara.wet_delay_seconds+
    g_wet_lara.wet_ramp_seconds*seed;
  if(age_sec<0.0f) age_sec=0.0f;
  unsigned long long age_ms=(unsigned long long)(age_sec*1000.0f);
  return now>age_ms ? now-age_ms : now;
}

static const char *tr456_wet_lara_phase_name(int phase) {
  switch(phase) {
    case 1: return "wetting";
    case 2: return "hold";
    case 3: return "exit-grace";
    case 4: return "drying";
    default: return "dry";
  }
}

static void tr456_wet_lara_trace_state(int raw_mask, int active,
    int exit_grace, float old_wet, float target, float wet, float dt,
    unsigned int frame_delta) {
  if(!g_wet_lara.trace_log || g_wet_lara.trace_log_frame==g_frame_index)
    return;

  int phase=0;
  if(active && target>old_wet+0.002f)
    phase=1;
  else if(active)
    phase=2;
  else if(exit_grace)
    phase=3;
  else if(wet>0.002f)
    phase=4;

  int transition=phase!=g_wet_lara.trace_last_phase ||
    raw_mask!=g_wet_lara.trace_last_mask ||
    f_abs(wet-g_wet_lara.trace_last_wet)>=0.12f;
  unsigned int interval=(unsigned int)g_wet_lara.trace_interval_frames;
  int periodic=!g_wet_lara.trace_last_period_frame ||
    g_frame_index-g_wet_lara.trace_last_period_frame>=interval;
  if(!transition && !periodic)
    return;

  unsigned int water_age=g_wet_lara.last_water_frame ?
    g_frame_index-g_wet_lara.last_water_frame : 999999u;
  unsigned int synthetic_age=g_wet_lara.last_synthetic_frame ?
    g_frame_index-g_wet_lara.last_synthetic_frame : 999999u;
  unsigned int underwater_age=g_wet_lara.last_underwater_frame ?
    g_frame_index-g_wet_lara.last_underwater_frame : 999999u;
  unsigned int pose_age=g_wet_lara.last_lara_pose_frame ?
    g_frame_index-g_wet_lara.last_lara_pose_frame : 999999u;
  unsigned int plane_age=g_wet_lara.last_water_plane_frame ?
    g_frame_index-g_wet_lara.last_water_plane_frame : 999999u;

  char msg[1024];
  snprintf(msg,sizeof(msg),
    "wet lara trace frame=%u phase=%s transition=%d mask=0x%02X active=%d grace=%d wet=%.3f old=%.3f target=%.3f dt=%.3f df=%u start=%llu ages=(water:%u synthetic:%u underwater:%u pose:%u plane:%u) streaks=(water:%d synthetic:%d) partial=(valid:%d line:%.1f dir:%.0f water:%.1f body:%.1f..%.1f) draws=%d candidates=%d",
    g_frame_index,tr456_wet_lara_phase_name(phase),transition,
    raw_mask,active,exit_grace,(double)wet,(double)old_wet,
    (double)target,(double)dt,frame_delta,
    (unsigned long long)g_wet_lara.wet_contact_start_ms,
    water_age,synthetic_age,underwater_age,pose_age,plane_age,
    g_wet_lara.water_streak,g_wet_lara.synthetic_streak,
    g_wet_lara.partial_line_valid,(double)g_wet_lara.partial_line_y,
    (double)g_wet_lara.partial_line_dir,
    (double)g_wet_lara.partial_water_y,
    (double)g_wet_lara.partial_body_min_y,
    (double)g_wet_lara.partial_body_max_y,
    g_wet_lara.frame_draws,g_wet_lara.frame_candidates);
  log_line(msg);

  g_wet_lara.trace_log_frame=g_frame_index;
  g_wet_lara.trace_last_phase=phase;
  g_wet_lara.trace_last_mask=raw_mask;
  g_wet_lara.trace_last_wet=wet;
  if(periodic)
    g_wet_lara.trace_last_period_frame=g_frame_index;
}

static float tr456_wet_lara_wet_amount(void) {
  unsigned long long now=tr456_wet_lara_now_ms();
  if(!g_wet_lara.wet_last_update_ms)
    g_wet_lara.wet_last_update_ms=now;
  unsigned long long elapsed_ms=now>=g_wet_lara.wet_last_update_ms ?
    now-g_wet_lara.wet_last_update_ms : 0u;
  float dt=(float)elapsed_ms*0.001f;
  if(dt>0.25f) dt=0.25f;
  unsigned int frame_delta=g_frame_index>=g_wet_lara.wet_last_update_frame ?
    g_frame_index-g_wet_lara.wet_last_update_frame : 0u;
  if(frame_delta>12u) frame_delta=12u;
  int raw_active_mask=tr456_wet_lara_active_source_mask();
  int active=raw_active_mask!=0;
  int exit_grace=0;
  float target=0.0f;
  float old_wet=g_wet_lara.wet_amount;

  if(active) {
    g_wet_lara.last_active_source_frame=g_frame_index;
    g_wet_lara.last_raw_active_mask=(unsigned int)raw_active_mask;
    if(!g_wet_lara.wet_contact_start_ms)
      g_wet_lara.wet_contact_start_ms=
        tr456_wet_lara_resume_start_ms(now);
    float age_sec=(float)(now-g_wet_lara.wet_contact_start_ms)*0.001f;
    float ramp_t=(age_sec-g_wet_lara.wet_delay_seconds)/
      g_wet_lara.wet_ramp_seconds;
    target=tr456_wet_lara_smooth(ramp_t);
  } else {
    if(g_wet_lara.last_active_source_frame &&
       g_wet_lara.wet_amount>0.001f) {
      unsigned int inactive_frames=g_frame_index>=
        g_wet_lara.last_active_source_frame ?
        g_frame_index-g_wet_lara.last_active_source_frame : 0u;
      exit_grace=inactive_frames<=(unsigned int)g_wet_lara.exit_grace_frames;
    }
    g_wet_lara.wet_contact_start_ms=0;
  }

  if(target>g_wet_lara.wet_amount) {
    g_wet_lara.wet_amount=target;
  } else if(active) {
    /* While Lara is still in water, keep existing dampness during the delay. */
  } else if(exit_grace) {
    /* Ignore short contact holes before entering the real drying phase. */
  } else if(target<g_wet_lara.wet_amount) {
    if(g_wet_lara.dry_seconds<=0.001f && g_wet_lara.dry_frames<=0)
      g_wet_lara.wet_amount=target;
    else {
      float drop=0.0f;
      if(g_wet_lara.dry_seconds>0.001f && dt>0.0f)
        drop=dt/g_wet_lara.dry_seconds;
      if(g_wet_lara.dry_frames>0 && frame_delta>0u) {
        float frame_drop=(float)frame_delta/(float)g_wet_lara.dry_frames;
        drop=drop>0.0f ? f_max(drop,frame_drop) : frame_drop;
      }
      g_wet_lara.wet_amount=f_max(target,g_wet_lara.wet_amount-drop);
    }
  }

  float clamped=f_min(f_max(g_wet_lara.wet_amount,0.0f),1.0f);
  tr456_wet_lara_trace_state(raw_active_mask,active,exit_grace,
    old_wet,target,clamped,dt,frame_delta);
  g_wet_lara.wet_last_update_ms=now;
  g_wet_lara.wet_last_update_frame=g_frame_index;
  return clamped;
}

static void tr456_wet_lara_reset_partial_line_if_dry(void) {
  if(tr456_wet_lara_wet_amount()>0.001f)
    return;
  g_wet_lara.partial_line_valid=0;
  g_wet_lara.partial_line_y=0.0f;
  g_wet_lara.partial_line_dir=0.0f;
  g_wet_lara.partial_water_y=0.0f;
  g_wet_lara.partial_body_min_y=0.0f;
  g_wet_lara.partial_body_max_y=0.0f;
}

static void tr456_wet_lara_update_partial_line_from_joints(
    const GLfloat joints[96][4], const GLfloat view[16], GLfloat water_y) {
  if(!g_wet_lara.partial_wet || !joints || !view)
    return;

  GLfloat min_y=1000000000.0f;
  GLfloat max_y=-1000000000.0f;
  for(int j=0;j<32;j++) {
    GLfloat jy=joints[j*3+1][3]+view[7];
    if(jy<min_y) min_y=jy;
    if(jy>max_y) max_y=jy;
  }
  GLfloat body_h=max_y-min_y;
  if(body_h<32.0f)
    return;

  GLfloat dir=g_wet_lara.partial_direction;
  GLfloat line=water_y-dir*g_wet_lara.partial_rise;

  if(dir<0.0f && water_y>=max_y-g_wet_lara.partial_full_margin)
    line=max_y+g_wet_lara.partial_fade;
  else if(dir>0.0f && water_y<=min_y+g_wet_lara.partial_full_margin)
    line=min_y-g_wet_lara.partial_fade;

  if(!g_wet_lara.partial_line_valid ||
     g_wet_lara.partial_line_dir*dir<=0.0f) {
    g_wet_lara.partial_line_y=line;
    g_wet_lara.partial_line_dir=dir;
    g_wet_lara.partial_line_valid=1;
  } else if(dir<0.0f) {
    if(line>g_wet_lara.partial_line_y)
      g_wet_lara.partial_line_y=line;
  } else if(line<g_wet_lara.partial_line_y) {
    g_wet_lara.partial_line_y=line;
  }

  g_wet_lara.partial_water_y=water_y;
  g_wet_lara.partial_body_min_y=min_y;
  g_wet_lara.partial_body_max_y=max_y;
}

static void tr456_wet_lara_carry_partial_line_from_joints(
    const GLfloat joints[96][4], const GLfloat view[16]) {
  if(!g_wet_lara.partial_carry_after_exit ||
     !g_wet_lara.partial_wet ||
     !g_wet_lara.partial_line_valid ||
     g_wet_lara.wet_amount<=0.002f ||
     !joints || !view)
    return;

  float old_h=g_wet_lara.partial_body_max_y-
    g_wet_lara.partial_body_min_y;
  if(old_h<32.0f)
    return;

  GLfloat min_y=1000000000.0f;
  GLfloat max_y=-1000000000.0f;
  for(int j=0;j<32;j++) {
    GLfloat jy=joints[j*3+1][3]+view[7];
    if(jy<min_y) min_y=jy;
    if(jy>max_y) max_y=jy;
  }
  float new_h=max_y-min_y;
  if(new_h<32.0f)
    return;

  float old_center=(g_wet_lara.partial_body_min_y+
    g_wet_lara.partial_body_max_y)*0.5f;
  float new_center=(min_y+max_y)*0.5f;
  float delta=new_center-old_center;
  if(f_abs(delta)>g_wet_lara.partial_carry_max_delta)
    return;

  g_wet_lara.partial_line_y+=delta;
  g_wet_lara.partial_body_min_y=min_y;
  g_wet_lara.partial_body_max_y=max_y;
}

static void tr456_wet_lara_update_pose_cache(
    const GLfloat joints[96][4], const GLfloat view[16]) {
  if(!joints || !view)
    return;
  float sx=0.0f;
  float sy=0.0f;
  float sz=0.0f;
  int count=0;
  for(int j=0;j<32;j++) {
    sx+=joints[j*3+0][3]+view[3];
    sy+=joints[j*3+1][3]+view[7];
    sz+=joints[j*3+2][3]+view[11];
    count++;
  }
  if(count<=0)
    return;
  float inv=1.0f/(float)count;
  g_wet_lara.last_lara_x=sx*inv;
  g_wet_lara.last_lara_y=sy*inv;
  g_wet_lara.last_lara_z=sz*inv;
  g_wet_lara.last_lara_pose_frame=g_frame_index;
}

static void tr456_wet_lara_emit_contact_ripple(
    const GLfloat joints[96][4], const GLfloat view[16],
    int joint, int joint_count, GLfloat water_y) {
  if(!g_wet_lara.contact_ripples ||
     g_wet_lara.contact_ripple_strength<=0.001f ||
     !joints || !view || joint<0 || joint>=32)
    return;
  GLfloat x=joints[joint*3+0][3]+view[3];
  GLfloat z=joints[joint*3+2][3]+view[11];
  float crowd=f_min((float)joint_count,10.0f);
  float radius=g_wet_lara.contact_ripple_radius*(0.72f+crowd*0.045f);
  float strength=g_wet_lara.contact_ripple_strength*(0.85f+crowd*0.035f);
  tr456_water_emit_lara_impulse(x,water_y,z,radius,strength);
}

static void tr456_wet_lara_maybe_emit_drip_ripple(float wet) {
  if(!g_wet_lara.drip_ripples ||
     g_wet_lara.drip_ripple_strength<=0.001f ||
     wet<=0.08f)
    return;
  if(tr456_wet_lara_active_source_mask()!=0)
    return;
  if(!g_wet_lara.last_lara_pose_frame ||
     !g_wet_lara.last_water_plane_frame)
    return;
  unsigned int pose_age=g_frame_index>=g_wet_lara.last_lara_pose_frame ?
    g_frame_index-g_wet_lara.last_lara_pose_frame : 0u;
  unsigned int water_age=g_frame_index>=g_wet_lara.last_water_plane_frame ?
    g_frame_index-g_wet_lara.last_water_plane_frame : 0u;
  if(pose_age>90u || water_age>600u)
    return;

  unsigned int interval=(unsigned int)g_wet_lara.drip_interval_frames;
  interval+=(unsigned int)((1.0f-f_min(wet,1.0f))*
    (float)g_wet_lara.drip_interval_frames*1.4f);
  if(interval<4u) interval=4u;
  if(g_wet_lara.last_drip_frame &&
     g_frame_index-g_wet_lara.last_drip_frame<interval)
    return;

  float seed=(float)((g_frame_index*1103515245u+12345u)&1023u)*
    (1.0f/1024.0f);
  float angle=seed*6.2831853f;
  float spread=38.0f+46.0f*wet;
  float ox=cosf(angle)*spread;
  float oz=sinf(angle)*spread;
  float radius=g_wet_lara.drip_ripple_radius*(0.74f+wet*0.42f);
  float strength=g_wet_lara.drip_ripple_strength*(0.35f+wet*0.85f);
  tr456_water_emit_lara_impulse(g_wet_lara.last_lara_x+ox,
    g_wet_lara.last_water_y,g_wet_lara.last_lara_z+oz,
    radius,strength);
  g_wet_lara.last_drip_frame=g_frame_index;
}

static void tr456_wet_lara_note_underwater_sustain(int joint_count,
                                                   float water_y,
                                                   float min_y,
                                                   float max_y) {
  g_wet_lara.last_underwater_frame=g_frame_index;
  g_wet_lara.last_underwater_ms=tr456_wet_lara_now_ms();
  if((diag_is_active() || g_wet_lara.underwater_log_count<24u) &&
     g_wet_lara.underwater_log_frame!=g_frame_index) {
    char msg[320];
    snprintf(msg,sizeof(msg),
      "wet lara underwater sustain session=%d frame=%u joints=%d waterY=%.1f body=%.1f..%.1f margin=%.1f",
      g_diag_session,g_frame_index,joint_count,(double)water_y,
      (double)min_y,(double)max_y,(double)g_wet_lara.underwater_margin);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.underwater_log_count++;
    g_wet_lara.underwater_log_frame=g_frame_index;
  }
}

static int tr456_wet_lara_update_underwater_sustain_from_joints(
    const GLfloat joints[96][4], const GLfloat view[16],
    int has_water_y, GLfloat fallback_water_y) {
  if(!g_wet_lara.underwater_sustain || !joints || !view)
    return 0;
  if(!g_wet_lara.partial_line_valid && !has_water_y)
    return 0;

  float water_y=has_water_y ? fallback_water_y : g_wet_lara.partial_water_y;
  float dir=g_wet_lara.partial_direction;
  float min_y=1000000000.0f;
  float max_y=-1000000000.0f;
  int submerged_count=0;
  for(int j=0;j<32;j++) {
    float jy=joints[j*3+1][3]+view[7];
    if(jy<min_y) min_y=jy;
    if(jy>max_y) max_y=jy;
    if(dir*(jy-water_y)>=-g_wet_lara.underwater_margin)
      submerged_count++;
  }
  float body_h=max_y-min_y;
  int body_under=dir<0.0f ?
    (max_y<=water_y+g_wet_lara.underwater_margin) :
    (min_y>=water_y-g_wet_lara.underwater_margin);
  if(body_h<32.0f || !body_under ||
     submerged_count<g_wet_lara.underwater_min_joints)
    return 0;

  g_wet_lara.partial_line_dir=dir;
  g_wet_lara.partial_line_y=dir<0.0f ?
    max_y+g_wet_lara.partial_fade : min_y-g_wet_lara.partial_fade;
  g_wet_lara.partial_line_valid=1;
  g_wet_lara.partial_water_y=water_y;
  g_wet_lara.partial_body_min_y=min_y;
  g_wet_lara.partial_body_max_y=max_y;
  tr456_wet_lara_note_underwater_sustain(submerged_count,water_y,min_y,max_y);
  return 1;
}

static float tr456_wet_lara_contact_radius(const GLfloat *c) {
  float encoded=f_abs(c[3]);
  float radius=encoded>=49152.0f ? floorf(encoded*(1.0f/512.0f)) :
    encoded*0.025f;
  return f_min(f_max(radius,90.0f),720.0f);
}

static int tr456_wet_lara_active_contacts(GLfloat contacts[16][4],
                                          GLfloat motions[16][4],
                                          int *source) {
  int count=build_effective_contact_values(contacts,motions,source);
  return count>0 && contact_values_sum_abs(contacts)>0.001f;
}

static int tr456_wet_lara_origin_near_contact(void) {
  GLfloat contacts[16][4];
  GLfloat motions[16][4];
  int source=0;
  if(!tr456_wet_lara_active_contacts(contacts,motions,&source)) return 0;
  GLfloat model[16];
  GLfloat view[16];
  if(!tr456_lab_read_vec4_array("uModelMatrix",4,model)) return 0;
  if(!tr456_lab_read_vec4_array("uViewMatrix",4,view)) return 0;
  float ox=model[3]+view[3];
  float oy=model[7]+view[7];
  float oz=model[11]+view[11];
  for(int i=0;i<16;i++) {
    const GLfloat *c=contacts[i];
    float sum=f_abs(c[0])+f_abs(c[1])+f_abs(c[2])+f_abs(c[3]);
    if(sum<=0.001f) continue;
    if(is_screen_contact_value(c[0],c[1],c[2])) return 1;
    float radius=tr456_wet_lara_contact_radius(c)*g_wet_lara.radius_scale+
      260.0f;
    float dx=ox-c[0];
    float dz=oz-c[2];
    float dy=f_abs(oy-c[1]);
    if(dx*dx+dz*dz<=radius*radius &&
       dy<=g_wet_lara.vertical_radius*1.45f)
      return 1;
  }
  return 0;
}

static int tr456_wet_lara_joints_near_contact(
    const GLfloat contacts[16][4], const GLfloat motions[16][4],
    int *slot_out, int *joint_out, int *joint_count_out,
    float *dist_out, float *dy_out, float *radius_out, float *speed_out) {
  GLfloat joints[96][4];
  GLfloat view[16];
  if(!tr456_lab_read_vec4_array("uJoints",96,&joints[0][0]))
    return 0;
  if(!tr456_lab_read_vec4_array("uViewMatrix",4,view))
    return 0;

  float vx=view[3];
  float vy=view[7];
  float vz=view[11];
  float best_dist=9999999.0f;
  float best_radius=0.0f;
  float best_dy=0.0f;
  float best_speed=0.0f;
  int best_slot=-1;
  int best_joint=-1;
  int best_count=0;
  for(int i=0;i<16;i++) {
    const GLfloat *c=contacts[i];
    float sum=f_abs(c[0])+f_abs(c[1])+f_abs(c[2])+f_abs(c[3]);
    if(sum<=0.001f || is_screen_contact_value(c[0],c[1],c[2]))
      continue;
    float speed=motions ? f_abs(motions[i][3]) : 0.0f;
    if(speed<g_wet_lara.fallback_min_speed)
      continue;
    float radius=tr456_wet_lara_contact_radius(c)*
      g_wet_lara.fallback_radius_scale+80.0f;
    int near_count=0;
    int near_joint=-1;
    float near_dist=9999999.0f;
    float near_dy=0.0f;
    for(int j=0;j<32;j++) {
      float jx=joints[j*3+0][3]+vx;
      float jy=joints[j*3+1][3]+vy;
      float jz=joints[j*3+2][3]+vz;
      float dx=jx-c[0];
      float dz=jz-c[2];
      float dy=f_abs(jy-c[1]);
      float dist=sqrtf(dx*dx+dz*dz);
      float score=(dist-radius)+f_max(dy-g_wet_lara.fallback_vertical,0.0f);
      if(score<best_dist) {
        best_dist=score;
        best_radius=radius;
        best_dy=dy;
        best_speed=speed;
        best_slot=i;
        best_joint=j;
      }
      if(dist<=radius && dy<=g_wet_lara.fallback_vertical) {
        near_count++;
        if(dist<near_dist) {
          near_dist=dist;
          near_dy=dy;
          near_joint=j;
        }
      }
    }
    if(near_count>best_count ||
       (near_count==best_count && near_count>0 && near_dist<best_dist)) {
      best_count=near_count;
      best_slot=i;
      best_joint=near_joint;
      best_dist=near_dist;
      best_dy=near_dy;
      best_radius=radius;
      best_speed=speed;
    }
    if(near_count>=g_wet_lara.water_min_joints) {
      if(slot_out) *slot_out=i;
      if(joint_out) *joint_out=near_joint;
      if(joint_count_out) *joint_count_out=near_count;
      if(dist_out) *dist_out=near_dist;
      if(dy_out) *dy_out=near_dy;
      if(radius_out) *radius_out=radius;
      if(speed_out) *speed_out=speed;
      return 1;
    }
  }
  if((diag_is_active() || g_wet_lara.contact_miss_log_count<12u) &&
     g_wet_lara.contact_miss_log_frame!=g_frame_index && best_slot>=0) {
    char msg[320];
    snprintf(msg,sizeof(msg),
      "wet lara water miss session=%d frame=%u slot=%d joint=%d joints=%d margin=%.1f radius=%.1f dy=%.1f speed=%.2f minSpeed=%.2f needJoints=%d",
      g_diag_session,g_frame_index,best_slot,best_joint,best_count,
      (double)best_dist,(double)best_radius,(double)best_dy,
      (double)best_speed,(double)g_wet_lara.fallback_min_speed,
      g_wet_lara.water_min_joints);
    log_line(msg);
    if(diag_is_active()) diag_consume_line();
    else g_wet_lara.contact_miss_log_count++;
    g_wet_lara.contact_miss_log_frame=g_frame_index;
  }
  return 0;
}

static void tr456_wet_lara_update_water_contact(void) {
  if(!g_wet_lara.use_water_contact || !g_wet_lara.contact_fallback ||
     g_wet_lara.water_eval_frame==g_frame_index)
    return;
  g_wet_lara.water_eval_frame=g_frame_index;

  GLfloat contacts[16][4];
  GLfloat motions[16][4];
  int source=0;
  int slot=-1;
  int joint=-1;
  int joint_count=0;
  float dist=0.0f;
  float dy=0.0f;
  float radius=0.0f;
  float speed=0.0f;
  if(!tr456_wet_lara_active_contacts(contacts,motions,&source) ||
     !tr456_wet_lara_joints_near_contact(
       (const GLfloat (*)[4])contacts,(const GLfloat (*)[4])motions,
       &slot,&joint,&joint_count,&dist,&dy,&radius,&speed)) {
    g_wet_lara.water_streak=0;
    return;
  }

  if(g_wet_lara.water_streak<g_wet_lara.water_enter_frames)
    g_wet_lara.water_streak++;
  if(g_wet_lara.water_streak>=g_wet_lara.water_enter_frames)
    tr456_wet_lara_note_water_contact(slot,joint,joint_count,dist,dy,
      radius,speed);
}

static void tr456_wet_lara_update_synthetic_contact(void) {
  if(!g_wet_lara.use_synthetic_contact ||
     g_wet_lara.synthetic_eval_frame==g_frame_index)
    return;
  g_wet_lara.synthetic_eval_frame=g_frame_index;

  GLfloat joints[96][4];
  GLfloat view[16];
  int surface=-1;
  int joint=-1;
  int joint_count=0;
  GLfloat dist=0.0f;
  GLfloat dy=0.0f;
  GLfloat water_y=0.0f;
  int has_lara_pose=
    tr456_lab_read_vec4_array("uJoints",96,&joints[0][0]) &&
    tr456_lab_read_vec4_array("uViewMatrix",4,view);
  if(has_lara_pose)
    tr456_wet_lara_update_pose_cache((const GLfloat (*)[4])joints,view);
  int has_surface_contact=has_lara_pose &&
    synthetic_surface_lara_contact((const GLfloat (*)[4])joints,view,
      g_wet_lara.synthetic_min_joints,
      g_wet_lara.synthetic_surface_age_frames,
      g_wet_lara.synthetic_margin,g_wet_lara.synthetic_vertical,
      g_wet_lara.synthetic_above_surface,g_wet_lara.partial_direction,
      &surface,&joint,&joint_count,&dist,&dy,&water_y);
  if(!has_surface_contact) {
    g_wet_lara.synthetic_streak=0;
    int has_underwater_plane=surface>=0 &&
      dist<=g_wet_lara.synthetic_margin;
    if(has_underwater_plane) {
      g_wet_lara.last_water_y=water_y;
      g_wet_lara.last_water_plane_frame=g_frame_index;
    }
    if(has_lara_pose && has_underwater_plane &&
       tr456_wet_lara_update_underwater_sustain_from_joints(
          (const GLfloat (*)[4])joints,view,has_underwater_plane,water_y))
      return;
    if(has_lara_pose)
      tr456_wet_lara_carry_partial_line_from_joints(
        (const GLfloat (*)[4])joints,view);
    if((diag_is_active() || g_wet_lara.synthetic_miss_log_count<12u) &&
       g_wet_lara.synthetic_miss_log_frame!=g_frame_index) {
      char msg[320];
      snprintf(msg,sizeof(msg),
        "wet lara synthetic miss session=%d frame=%u surface=%d joint=%d joints=%d dist=%.1f dy=%.1f waterY=%.1f minJoints=%d maxAge=%d margin=%.1f vertical=%.1f above=%.1f",
        g_diag_session,g_frame_index,surface,joint,joint_count,(double)dist,
        (double)dy,(double)water_y,g_wet_lara.synthetic_min_joints,
        g_wet_lara.synthetic_surface_age_frames,
        (double)g_wet_lara.synthetic_margin,
        (double)g_wet_lara.synthetic_vertical,
        (double)g_wet_lara.synthetic_above_surface);
      log_line(msg);
      if(diag_is_active()) diag_consume_line();
      else g_wet_lara.synthetic_miss_log_count++;
      g_wet_lara.synthetic_miss_log_frame=g_frame_index;
    }
    return;
  }

  g_wet_lara.last_water_y=water_y;
  g_wet_lara.last_water_plane_frame=g_frame_index;
  if(g_wet_lara.synthetic_streak<g_wet_lara.synthetic_enter_frames)
    g_wet_lara.synthetic_streak++;
  if(g_wet_lara.synthetic_streak>=g_wet_lara.synthetic_enter_frames) {
    tr456_wet_lara_update_partial_line_from_joints(
      (const GLfloat (*)[4])joints,view,water_y);
    tr456_wet_lara_note_synthetic_contact(surface,joint,joint_count,
      dist,dy,water_y);
    tr456_wet_lara_emit_contact_ripple((const GLfloat (*)[4])joints,view,
      joint,joint_count,water_y);
  }
}

static int tr456_wet_lara_current_compatible(GLint *attr_coord_out,
                                             GLint *attr_normal_out,
                                             GLint *attr_light_out,
                                             GLint *attr_color_out,
                                             int *use_joints_out) {
  PFNGLGETATTRIBLOCATION get_attr=real_get_attrib_location();
  CaptureGL *gl=capture_gl();
  if(!get_attr || !gl || !gl->get_uniform_location || !g_current_program)
    return 0;
  GLint attr_coord=get_attr(g_current_program,"aCoord");
  if(attr_coord<0 || attr_coord>15) return 0;
  GLint attr_normal=get_attr(g_current_program,"aNormal");
  if(g_wet_lara.require_normal && (attr_normal<0 || attr_normal>15))
    return 0;
  if(attr_normal>15) attr_normal=-1;
  GLint attr_light=get_attr(g_current_program,"aLight");
  if(attr_light>15) attr_light=-1;
  GLint attr_color=get_attr(g_current_program,"aColor");
  if(attr_color>15) attr_color=-1;
  if(gl->get_uniform_location(g_current_program,"uProjMatrix")<0) return 0;
  if(gl->get_uniform_location(g_current_program,"uModelMatrix[0]")<0) return 0;
  if(gl->get_uniform_location(g_current_program,"uViewMatrix[0]")<0) return 0;
  int use_joints=0;
  if(g_wet_lara.use_joints && attr_normal>=0 &&
     attr_light>=0 && attr_color>=0 &&
     gl->get_uniform_location(g_current_program,"uJoints[0]")>=0)
    use_joints=1;
  if(g_wet_lara.require_joints && !use_joints) return 0;
  if(attr_coord_out) *attr_coord_out=attr_coord;
  if(attr_normal_out) *attr_normal_out=attr_normal;
  if(attr_light_out) *attr_light_out=attr_light;
  if(attr_color_out) *attr_color_out=attr_color;
  if(use_joints_out) *use_joints_out=use_joints;
  return 1;
}

static int tr456_wet_lara_candidate(const char *call, GLenum mode,
                                     GLsizei count, int count_known,
                                     GLint *attr_coord_out,
                                     GLint *attr_normal_out,
                                     GLint *attr_light_out,
                                     GLint *attr_color_out,
                                     int *use_joints_out) {
  tr456_wet_lara_load_config();
  if(!g_wet_lara.enabled || g_wet_lara.opacity<=0.0f) return 0;
  if(!tr456_wet_lara_triangle_mode(mode)) return 0;
  if(is_tracked_water_shader_type(g_current_program_type)) return 0;
  if(!tr456_wet_lara_draw_count_matches(count,count_known)) return 0;
  if(!g_wet_lara.use_draw_counts) {
    if(count_known &&
       (count<g_wet_lara.min_count || count>g_wet_lara.max_count))
      return 0;
  }
  if(g_wet_lara.frame_draws>=g_wet_lara.max_per_frame) return 0;
  if(!tr456_wet_lara_current_compatible(attr_coord_out,attr_normal_out,
     attr_light_out,attr_color_out,use_joints_out))
    return 0;

  if(g_wet_lara.use_water_contact)
    tr456_wet_lara_update_water_contact();
  if(g_wet_lara.use_synthetic_contact)
    tr456_wet_lara_update_synthetic_contact();
  float wet_amount=tr456_wet_lara_wet_amount();
  if(g_wet_lara.contact_only) {
    GLfloat contacts[16][4];
    GLfloat motions[16][4];
    int source=0;
    if(!tr456_wet_lara_active_contacts(contacts,motions,&source)) return 0;
    if(g_wet_lara.object_gate && !tr456_wet_lara_origin_near_contact())
      return 0;
  }
  if(wet_amount<=0.0f)
    return 0;

  int candidate_index=g_wet_lara.frame_candidates++;
  if(g_wet_lara.frame_start>=0 && candidate_index<g_wet_lara.frame_start)
    return 0;
  if(g_wet_lara.frame_end>=0 && candidate_index>g_wet_lara.frame_end)
    return 0;
  if(diag_is_active() && g_wet_lara.diag_log_frame!=g_frame_index) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "wet lara candidate session=%d frame=%u index=%d call=%s program=%u type=%s mode=0x%X count=%d attr=%d normal=%d light=%d color=%d joints=%d wetAmount=%.3f frameDraws=%d/%d lastCandidates=%d",
      g_diag_session,g_frame_index,candidate_index,call ? call : "draw",
      g_current_program,shader_type_name(g_current_program_type),
      (unsigned int)mode,(int)count,
      attr_coord_out ? (int)*attr_coord_out : -1,
      attr_normal_out ? (int)*attr_normal_out : -1,
      attr_light_out ? (int)*attr_light_out : -1,
      attr_color_out ? (int)*attr_color_out : -1,
      use_joints_out ? *use_joints_out : 0,
      (double)wet_amount,
      g_wet_lara.frame_draws,g_wet_lara.max_per_frame,
      g_wet_lara.last_frame_candidates);
    log_line(msg);
    diag_consume_line();
    g_wet_lara.diag_log_frame=g_frame_index;
  }
  return 1;
}

static Tr456WetLaraProgram *tr456_wet_lara_program_for_attrs(
  GLint attr_coord, GLint attr_normal, GLint attr_light, GLint attr_color,
  int use_joints) {
  if(attr_coord<0 || attr_coord>=16) return 0;
  if(use_joints && (attr_normal<0 || attr_normal>=16 ||
     attr_light<0 || attr_light>=16 || attr_color<0 || attr_color>=16))
    return 0;

  Tr456WetLaraProgram *p=0;
  for(int i=0;i<TR456_WET_LARA_PROGRAM_COUNT;i++) {
    Tr456WetLaraProgram *candidate=&g_wet_lara.programs[i];
    if((candidate->ready || candidate->failed) &&
       candidate->attr_coord==attr_coord &&
       candidate->attr_normal==attr_normal &&
       candidate->attr_light==attr_light &&
       candidate->attr_color==attr_color &&
       candidate->use_joints==(use_joints ? 1 : 0)) {
      p=candidate;
      break;
    }
  }
  if(!p) {
    for(int i=0;i<TR456_WET_LARA_PROGRAM_COUNT;i++) {
      Tr456WetLaraProgram *candidate=&g_wet_lara.programs[i];
      if(!candidate->ready && !candidate->failed && !candidate->program) {
        p=candidate;
        memset(p,0,sizeof(*p));
        p->attr_coord=attr_coord;
        p->attr_normal=attr_normal;
        p->attr_light=attr_light;
        p->attr_color=attr_color;
        p->use_joints=use_joints ? 1 : 0;
        break;
      }
    }
  }
  if(!p) return 0;
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

  static const char *vs_rigid_text=
    "#version 150\n"
    "uniform mat4 uProjMatrix;\n"
    "uniform vec4 uViewMatrix[4];\n"
    "uniform vec4 uModelMatrix[4];\n"
    "uniform vec4 uTrWetLaraInfo;\n"
    "in vec4 aCoord;\n"
    "in vec4 aNormal;\n"
    "out vec3 vWetWorld;\n"
    "out vec3 vWetLocal;\n"
    "out vec3 vWetPos;\n"
    "out vec3 vWetView;\n"
    "out vec3 vWetNormal;\n"
    "void main(){\n"
    " vec4 s=vec4(aCoord.xyz,1.0);\n"
    " vec3 w=vec3(dot(uModelMatrix[0],s),dot(uModelMatrix[1],s),dot(uModelMatrix[2],s));\n"
    " vec3 wp=w+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);\n"
    " vec3 n=aNormal.xyz-vec3(127.0);\n"
    " if(length(n)<0.001) n=vec3(0.0,1.0,0.0);\n"
    " n=normalize(vec3(dot(uModelMatrix[0].xyz,n),dot(uModelMatrix[1].xyz,n),dot(uModelMatrix[2].xyz,n)));\n"
    " vWetWorld=wp;\n"
    " vWetLocal=aCoord.xyz;\n"
    " vWetPos=w;\n"
    " vWetNormal=n;\n"
    " vWetView=vec3(dot(uViewMatrix[0].xyz,w),dot(uViewMatrix[1].xyz,w),dot(uViewMatrix[2].xyz,w));\n"
    " vec4 clip=uProjMatrix*vec4(vWetView,1.0);\n"
    " clip.z-=clip.w*uTrWetLaraInfo.w;\n"
    " gl_Position=clip;\n"
    "}\n";
  static const char *vs_joints_text=
    "#version 150\n"
    "uniform mat4 uProjMatrix;\n"
    "uniform vec4 uViewMatrix[4];\n"
    "uniform vec4 uModelMatrix[4];\n"
    "uniform vec4 uJoints[96];\n"
    "uniform vec4 uTrWetLaraInfo;\n"
    "in vec4 aCoord;\n"
    "in vec4 aNormal;\n"
    "in vec4 aLight;\n"
    "in vec4 aColor;\n"
    "out vec3 vWetWorld;\n"
    "out vec3 vWetLocal;\n"
    "out vec3 vWetPos;\n"
    "out vec3 vWetView;\n"
    "out vec3 vWetNormal;\n"
    "void main(){\n"
    " vec4 s=vec4(aCoord.xyz,1.0);\n"
    " ivec3 idx=ivec3(aLight.xyz)*3;\n"
    " float wt=aColor.x;\n"
    " vec3 p=vec3(dot(uJoints[idx.x+0],s),dot(uJoints[idx.x+1],s),dot(uJoints[idx.x+2],s))*wt;\n"
    " vec3 n=aNormal.xyz-vec3(127.0);\n"
    " if(length(n)<0.001) n=vec3(0.0,1.0,0.0);\n"
    " n=normalize(n);\n"
    " vec3 sn=vec3(dot(uJoints[idx.x+0].xyz,n),dot(uJoints[idx.x+1].xyz,n),dot(uJoints[idx.x+2].xyz,n))*wt;\n"
    " wt=aColor.y;\n"
    " p+=vec3(dot(uJoints[idx.y+0],s),dot(uJoints[idx.y+1],s),dot(uJoints[idx.y+2],s))*wt;\n"
    " sn+=vec3(dot(uJoints[idx.y+0].xyz,n),dot(uJoints[idx.y+1].xyz,n),dot(uJoints[idx.y+2].xyz,n))*wt;\n"
    " wt=aColor.z;\n"
    " p+=vec3(dot(uJoints[idx.z+0],s),dot(uJoints[idx.z+1],s),dot(uJoints[idx.z+2],s))*wt;\n"
    " sn+=vec3(dot(uJoints[idx.z+0].xyz,n),dot(uJoints[idx.z+1].xyz,n),dot(uJoints[idx.z+2].xyz,n))*wt;\n"
    " if(length(sn)<0.001) sn=n;\n"
    " vec3 wp=p+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);\n"
    " vWetWorld=wp;\n"
    " vWetLocal=aCoord.xyz;\n"
    " vWetPos=p;\n"
    " vWetNormal=sn;\n"
    " vWetView=vec3(dot(uViewMatrix[0].xyz,p),dot(uViewMatrix[1].xyz,p),dot(uViewMatrix[2].xyz,p));\n"
    " vec4 clip=uProjMatrix*vec4(vWetView,1.0);\n"
    " clip.z-=clip.w*uTrWetLaraInfo.w;\n"
    " gl_Position=clip;\n"
    "}\n";
  static const char *fs_text=
    "#version 150\n"
    "uniform vec4 uTrWetLaraInfo;\n"
    "uniform vec4 uTrWetLaraTint;\n"
    "uniform vec4 uTrWetLaraTiming;\n"
    "uniform vec4 uTrWetLaraDetail;\n"
    "uniform vec4 uTrWetLaraDrops;\n"
    "uniform vec4 uLightPos[4];\n"
    "uniform vec4 uLightCol[4];\n"
    "in vec3 vWetWorld;\n"
    "in vec3 vWetLocal;\n"
    "in vec3 vWetPos;\n"
    "in vec3 vWetView;\n"
    "in vec3 vWetNormal;\n"
    "out vec4 trshaderFragColor;\n"
    "float sat(float x){ return clamp(x,0.0,1.0); }\n"
    "float h12(vec2 p){ vec3 p3=fract(vec3(p.xyx)*.1031); p3+=dot(p3,p3.yzx+33.33); return fract((p3.x+p3.y)*p3.z); }\n"
    "void main(){\n"
    " float wet=sat(uTrWetLaraTiming.x);\n"
    " if(abs(uTrWetLaraTiming.w)>0.5){\n"
    "  float fade=max(uTrWetLaraTiming.z,1.0);\n"
    "  wet*=smoothstep(-fade,fade,uTrWetLaraTiming.w*(vWetWorld.y-uTrWetLaraTiming.y));\n"
    " }\n"
    " if(wet<=0.002) discard;\n"
    " vec3 n=vWetNormal;\n"
    " if(dot(n,n)<0.0001) n=vec3(0.0,1.0,0.0);\n"
    " n=normalize(n);\n"
    " vec3 vp=-vWetPos;\n"
    " if(dot(vp,vp)<0.0001) vp=vec3(0.0,0.0,1.0);\n"
    " vec3 v=normalize(vp);\n"
    " float ndv=sat(dot(n,v));\n"
    " float rim=pow(1.0-ndv,4.0);\n"
    " float upper=sat(n.y*.45+.55);\n"
    " vec3 key=normalize(vec3(-.34,.82,.45));\n"
    " float shade=sat(dot(n,key)*.54+.46);\n"
    " float nh=sat(dot(n,normalize(key+v)));\n"
    " float nh2=nh*nh;\n"
    " float nh4=nh2*nh2;\n"
    " float nh8=nh4*nh4;\n"
    " float lightSpec=sat((nh8*.26+nh8*nh8*.86)*shade*uTrWetLaraInfo.y+rim*.045);\n"
    " float cloth=sat(1.0-upper*.18);\n"
    " float clothMask=sat(uTrWetLaraDetail.x);\n"
    " float colorSpot=sat(wet*clothMask*(.58+.42*cloth));\n"
    " if(uTrWetLaraDetail.y>0.5){\n"
    "  trshaderFragColor=vec4(.00,.85,1.00,sat(max(wet,.35)*uTrWetLaraInfo.x));\n"
    "  return;\n"
    " }\n"
    " vec2 dropUv=vec2(vWetWorld.x*.033+vWetWorld.z*.011,vWetWorld.y*.026+vWetWorld.x*.006);\n"
    " vec2 cell=floor(dropUv*5.0);\n"
    " vec2 fracp=fract(dropUv*5.0)-.5;\n"
    " float rnd=h12(cell);\n"
    " vec2 jitter=vec2(h12(cell+2.7),h12(cell+8.1))-.5;\n"
    " float bead=smoothstep(.060,.010,length(fracp-jitter*.28));\n"
    " bead*=smoothstep(.54,.98,rnd)*uTrWetLaraDrops.x*wet*clothMask;\n"
    " float trailSeed=h12(floor(vec2(vWetWorld.x*.018+vWetWorld.z*.010,rnd*7.0)));\n"
    " float trailLine=pow(sat(1.0-abs(fract(vWetWorld.x*.020+vWetWorld.z*.015+trailSeed)-.5)*2.0),10.0);\n"
    " float trailBreak=smoothstep(.22,.86,h12(floor(vec2(vWetWorld.y*.014+uTrWetLaraDrops.w*.18,trailSeed*13.0))));\n"
    " float streak=trailLine*trailBreak*smoothstep(.10,.82,wet)*uTrWetLaraDrops.y*clothMask*(.35+.65*cloth);\n"
    " float dropMask=sat(bead+streak*.74);\n"
    " float darkA=wet*uTrWetLaraInfo.x*(.16+.24*cloth)*uTrWetLaraDetail.z*clothMask;\n"
    " darkA+=dropMask*uTrWetLaraInfo.x*(.10+.10*cloth);\n"
    " float shineA=wet*lightSpec*(.130+.105*uTrWetLaraInfo.x);\n"
    " shineA+=dropMask*(.055+.075*lightSpec)*uTrWetLaraInfo.x;\n"
    " float a=sat(darkA+shineA);\n"
    " if(a<0.002) discard;\n"
    " vec3 dark=vec3(.016,.011,.007)*mix(vec3(1.0),uTrWetLaraTint.rgb,.08+.14*colorSpot);\n"
    " vec3 shine=mix(vec3(.33,.32,.30),vec3(.78,.76,.70),sat(lightSpec*.62));\n"
    " vec3 col=mix(dark,shine,sat(shineA/(shineA+darkA*.48+0.0001)));\n"
    " trshaderFragColor=vec4(col,a);\n"
    "}\n";

  GLuint vs=tr456_lab_compile_shader(GL_VERTEX_SHADER,
    use_joints ? "wet lara skinned vertex" : "wet lara vertex",
    use_joints ? vs_joints_text : vs_rigid_text);
  GLuint fs=tr456_lab_compile_shader(GL_FRAGMENT_SHADER,
    "wet lara fragment",fs_text);
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
  if(attr_normal>=0 && attr_normal<16)
    bind_attr(program,(GLuint)attr_normal,"aNormal");
  if(use_joints && attr_light>=0 && attr_light<16)
    bind_attr(program,(GLuint)attr_light,"aLight");
  if(use_joints && attr_color>=0 && attr_color<16)
    bind_attr(program,(GLuint)attr_color,"aColor");
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
    snprintf(msg,sizeof(msg),
      "wet lara link failed attr=%d normal=%d light=%d color=%d joints=%d log=%s",
      (int)attr_coord,(int)attr_normal,(int)attr_light,(int)attr_color,
      use_joints ? 1 : 0,logbuf);
    log_line(msg);
    PFNGLDELETEPROGRAM del_program=real_delete_program();
    if(del_program) del_program(program);
    p->failed=1;
    return 0;
  }

  p->program=program;
  p->loc_proj=gl->get_uniform_location(program,"uProjMatrix");
  p->loc_model=gl->get_uniform_location(program,"uModelMatrix[0]");
  p->loc_view=gl->get_uniform_location(program,"uViewMatrix[0]");
  p->loc_joints=gl->get_uniform_location(program,"uJoints[0]");
  p->loc_info=gl->get_uniform_location(program,"uTrWetLaraInfo");
  p->loc_tint=gl->get_uniform_location(program,"uTrWetLaraTint");
  p->loc_timing=gl->get_uniform_location(program,"uTrWetLaraTiming");
  p->loc_detail=gl->get_uniform_location(program,"uTrWetLaraDetail");
  p->loc_drops=gl->get_uniform_location(program,"uTrWetLaraDrops");
  p->loc_light_pos=gl->get_uniform_location(program,"uLightPos[0]");
  p->loc_light_col=gl->get_uniform_location(program,"uLightCol[0]");
  p->ready=1;
  {
    char msg[192];
    snprintf(msg,sizeof(msg),
      "wet lara linked program=%u attrCoord=%d attrNormal=%d attrLight=%d attrColor=%d joints=%d",
      program,(int)attr_coord,(int)attr_normal,(int)attr_light,
      (int)attr_color,p->use_joints);
    log_line(msg);
  }
  return p;
}

static float tr456_wet_lara_cloth_overlay_scale(GLsizei count) {
  if(count==42735)
    return 1.0f;
  if(count==11016 || count==22140 || count==11904)
    return 0.0f;
  if(count>=30000)
    return 0.45f;
  if(count>=16000)
    return 0.15f;
  return 0.0f;
}

static int tr456_wet_lara_setup_uniforms(const Tr456WetLaraProgram *program,
                                         GLsizei count) {
  CaptureGL *gl=capture_gl();
  PFNGLUNIFORMMATRIX4FV matrix4=real_uniform_matrix_4fv();
  if(!program || !gl || !gl->uniform_4f || !gl->uniform_4fv || !matrix4)
    return 0;

  GLfloat proj[16];
  GLfloat model[16];
  GLfloat view[16];
  GLfloat joints[96][4];
  GLfloat light_pos[4][4];
  GLfloat light_col[4][4];
  int has_light_pos=0;
  int has_light_col=0;
  if(!tr456_lab_read_mat4("uProjMatrix",proj)) return 0;
  if(!tr456_lab_read_vec4_array("uModelMatrix",4,model)) return 0;
  if(!tr456_lab_read_vec4_array("uViewMatrix",4,view)) return 0;
  if(program->use_joints &&
     !tr456_lab_read_vec4_array("uJoints",96,&joints[0][0]))
    return 0;
  if(program->loc_light_pos>=0)
    has_light_pos=tr456_lab_read_vec4_array("uLightPos",4,&light_pos[0][0]);
  if(program->loc_light_col>=0)
    has_light_col=tr456_lab_read_vec4_array("uLightCol",4,&light_col[0][0]);
  if(program->loc_proj>=0) {
    matrix4(program->loc_proj,1,GL_FALSE,proj);
    shadow_note_uniform_matrix4fv(program->loc_proj,1,GL_FALSE,proj);
  }
  if(program->loc_model>=0)
    shadow_call_uniform_4fv(gl,program->loc_model,4,model);
  if(program->loc_view>=0)
    shadow_call_uniform_4fv(gl,program->loc_view,4,view);
  if(program->use_joints && program->loc_joints>=0)
    shadow_call_uniform_4fv(gl,program->loc_joints,96,&joints[0][0]);
  if(program->loc_info>=0)
    shadow_call_uniform_4f(gl,program->loc_info,g_wet_lara.opacity,
      g_wet_lara.specular,g_wet_lara.radius_scale,g_wet_lara.depth_bias);
  if(program->loc_tint>=0)
    shadow_call_uniform_4f(gl,program->loc_tint,g_wet_lara.tint_r,g_wet_lara.tint_g,
      g_wet_lara.tint_b,g_wet_lara.vertical_radius);
  if(program->loc_timing>=0) {
    float wet=tr456_wet_lara_wet_amount();
    float line_y=0.0f;
    float fade=g_wet_lara.partial_fade;
    float dir=0.0f;
    if(g_wet_lara.partial_wet) {
      if(g_wet_lara.partial_line_valid) {
        line_y=g_wet_lara.partial_line_y;
        dir=g_wet_lara.partial_line_dir;
      } else {
        wet=0.0f;
      }
    } else if(!g_wet_lara.full_body_from_timing) {
      wet=0.0f;
    }
    shadow_call_uniform_4f(gl,program->loc_timing,wet,line_y,fade,dir);
  }
  unsigned long long now=tr456_wet_lara_now_ms();
  float time_sec=(float)(now%600000ULL)*0.001f;
  if(program->loc_detail>=0) {
    float cloth_scale=tr456_wet_lara_cloth_overlay_scale(count);
    shadow_call_uniform_4f(gl,program->loc_detail,cloth_scale,
      g_wet_lara.debug_visible ? 1.0f : 0.0f,
      g_wet_lara.cloth_darkening,time_sec);
  }
  if(program->loc_drops>=0)
    shadow_call_uniform_4f(gl,program->loc_drops,g_wet_lara.droplet_strength,
      g_wet_lara.streak_strength,
      (float)g_wet_lara.last_raw_active_mask,time_sec);
  if(program->loc_light_pos>=0 && has_light_pos)
    shadow_call_uniform_4fv(gl,program->loc_light_pos,4,&light_pos[0][0]);
  if(program->loc_light_col>=0 && has_light_col)
    shadow_call_uniform_4fv(gl,program->loc_light_col,4,&light_col[0][0]);
  return 1;
}

static void tr456_wet_lara_begin_state(Tr456WetLaraDrawState *state) {
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
    shadow_get_integer_or_gl(GL_CURRENT_PROGRAM,&state->old_program);
    shadow_get_integer_or_gl(GL_BLEND,&state->old_blend);
    shadow_get_integer_or_gl(GL_DEPTH_TEST,&state->old_depth);
    shadow_get_integer_or_gl(GL_CULL_FACE,&state->old_cull);
    shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,&state->old_depth_mask);
    shadow_get_integer_or_gl(GL_DEPTH_FUNC,&state->old_depth_func);
    shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&state->old_blend_func[0]);
    shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&state->old_blend_func[1]);
    shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&state->old_blend_func[2]);
    shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&state->old_blend_func[3]);
  }

  PFNGLENABLE enable=real_enable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNC blend_func=real_blend_func();
  PFNGLBLENDFUNCSEPARATE blend_func_separate=real_blend_func_separate();
  if(enable) {
    enable(GL_BLEND);
    shadow_note_enable(GL_BLEND,1);
    enable(GL_DEPTH_TEST);
    shadow_note_enable(GL_DEPTH_TEST,1);
  }
  if(depth_func) { depth_func(GL_LEQUAL); shadow_note_depth_func(GL_LEQUAL); }
  if(depth_mask) { depth_mask(GL_FALSE); shadow_note_depth_mask(GL_FALSE); }
  if(blend_func_separate) {
    blend_func_separate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ZERO,GL_ONE);
    shadow_note_blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ZERO,GL_ONE);
  } else if(blend_func) {
    blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    shadow_note_blend_func(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,
      GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }
}

static void tr456_wet_lara_end_state(const Tr456WetLaraDrawState *state) {
  PFNGLUSEPROGRAM use_program=real_use_program();
  PFNGLENABLE enable=real_enable();
  PFNGLDISABLE disable=real_disable();
  PFNGLDEPTHMASK depth_mask=real_depth_mask();
  PFNGLDEPTHFUNC depth_func=real_depth_func();
  PFNGLBLENDFUNCSEPARATE blend_func_separate=real_blend_func_separate();
  PFNGLBLENDFUNC blend_func=real_blend_func();
  if(blend_func_separate) {
    blend_func_separate((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[2],
      (GLenum)state->old_blend_func[3]);
    shadow_note_blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[2],
      (GLenum)state->old_blend_func[3]);
  } else if(blend_func) {
    blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1]);
    shadow_note_blend_func((GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1],
      (GLenum)state->old_blend_func[0],
      (GLenum)state->old_blend_func[1]);
  }
  if(depth_mask) {
    depth_mask((GLboolean)(state->old_depth_mask ? 1 : 0));
    shadow_note_depth_mask((GLboolean)(state->old_depth_mask ? 1 : 0));
  }
  if(depth_func) {
    depth_func((GLenum)state->old_depth_func);
    shadow_note_depth_func((GLenum)state->old_depth_func);
  }
  if(state->old_cull) {
    if(enable) { enable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,1); }
  } else {
    if(disable) { disable(GL_CULL_FACE); shadow_note_enable(GL_CULL_FACE,0); }
  }
  if(state->old_depth) {
    if(enable) { enable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,1); }
  } else {
    if(disable) { disable(GL_DEPTH_TEST); shadow_note_enable(GL_DEPTH_TEST,0); }
  }
  if(state->old_blend) {
    if(enable) { enable(GL_BLEND); shadow_note_enable(GL_BLEND,1); }
  } else {
    if(disable) { disable(GL_BLEND); shadow_note_enable(GL_BLEND,0); }
  }
  if(use_program) {
    use_program((GLuint)state->old_program);
    shadow_note_use_program((GLuint)state->old_program);
  }
}

static int tr456_wet_lara_begin_draw(GLint attr_coord, GLint attr_normal,
                                     GLint attr_light, GLint attr_color,
                                     int use_joints, GLsizei count,
                                     Tr456WetLaraDrawState *state) {
  Tr456WetLaraProgram *program=
    tr456_wet_lara_program_for_attrs(attr_coord,attr_normal,attr_light,
      attr_color,use_joints);
  PFNGLUSEPROGRAM use_program=real_use_program();
  if(!program || !use_program) return 0;
  tr456_wet_lara_begin_state(state);
  use_program(program->program);
  shadow_note_use_program(program->program);
  if(!tr456_wet_lara_setup_uniforms(program,count)) {
    tr456_wet_lara_end_state(state);
    return 0;
  }
  g_wet_lara.frame_draws++;
  return 1;
}

static int tr456_wet_lara_begin_candidate(const char *call, GLenum mode,
                                          GLsizei count, int count_known,
                                          Tr456WetLaraDrawState *state) {
  GLint attr_coord=-1;
  GLint attr_normal=-1;
  GLint attr_light=-1;
  GLint attr_color=-1;
  int use_joints=0;
  if(!tr456_wet_lara_candidate(call,mode,count,count_known,&attr_coord,
      &attr_normal,&attr_light,&attr_color,&use_joints))
    return 0;
  return tr456_wet_lara_begin_draw(attr_coord,attr_normal,attr_light,
    attr_color,use_joints,count,state);
}

static void tr456_wet_lara_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, GLint first, GLsizei count) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,first,count);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_range_elements(const char *call,
  PFNGLDRAWRANGEELEMENTS draw, GLenum mode, GLuint start, GLuint end,
  GLsizei count, GLenum type, const void *indices) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,start,end,count,type,indices);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_base_vertex(const char *call,
  PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLint base_vertex) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices,base_vertex);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_range_elements_base_vertex(
  const char *call, PFNGLDRAWRANGEELEMENTSBASEVERTEX draw, GLenum mode,
  GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices,
  GLint base_vertex) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,start,end,count,type,indices,base_vertex);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_arrays_instanced(const char *call,
  PFNGLDRAWARRAYSINSTANCED draw, GLenum mode, GLint first, GLsizei count,
  GLsizei instance_count) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,first,count,instance_count);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_instanced(const char *call,
  PFNGLDRAWELEMENTSINSTANCED draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLsizei instance_count) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_instanced_base_vertex(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLint base_vertex) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_arrays_instanced_base_instance(
  const char *call, PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLint first, GLsizei count, GLsizei instance_count, GLuint base_instance) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,first,count,instance_count,base_instance);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_instanced_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLuint base_instance) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_instance);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_instanced_base_vertex_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE draw,
  GLenum mode, GLsizei count, GLenum type, const void *indices,
  GLsizei instance_count, GLint base_vertex, GLuint base_instance) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,count,1,&state)) return;
  draw(mode,count,type,indices,instance_count,base_vertex,base_instance);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_multi_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, const GLint *first,
  const GLsizei *count, GLsizei draw_count) {
  if(!first || !count || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_wet_lara_draw_arrays(call,draw,mode,first[i],count[i]);
}

static void tr456_wet_lara_multi_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, const GLsizei *count, GLenum type,
  const void * const *indices, GLsizei draw_count) {
  if(!count || !indices || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_wet_lara_draw_elements(call,draw,mode,count[i],type,indices[i]);
}

static void tr456_wet_lara_multi_draw_elements_base_vertex(
  const char *call, PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode,
  const GLsizei *count, GLenum type, const void * const *indices,
  GLsizei draw_count, const GLint *base_vertex) {
  if(!count || !indices || !base_vertex || draw_count<=0) return;
  for(GLsizei i=0;i<draw_count;i++)
    tr456_wet_lara_draw_elements_base_vertex(call,draw,mode,count[i],
      type,indices[i],base_vertex[i]);
}

static void tr456_wet_lara_draw_arrays_indirect(const char *call,
  PFNGLDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,0,0,&state)) return;
  draw(mode,indirect);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_draw_elements_indirect(const char *call,
  PFNGLDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect) {
  if(!draw) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,0,0,&state)) return;
  draw(mode,type,indirect);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_multi_draw_arrays_indirect(const char *call,
  PFNGLMULTIDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect,
  GLsizei draw_count, GLsizei stride) {
  if(!draw || draw_count<=0) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,draw_count,1,&state)) return;
  draw(mode,indirect,draw_count,stride);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_multi_draw_elements_indirect(const char *call,
  PFNGLMULTIDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect, GLsizei draw_count, GLsizei stride) {
  if(!draw || draw_count<=0) return;
  Tr456WetLaraDrawState state;
  if(!tr456_wet_lara_begin_candidate(call,mode,draw_count,1,&state)) return;
  draw(mode,type,indirect,draw_count,stride);
  tr456_wet_lara_end_state(&state);
}

static void tr456_wet_lara_end_frame(void) {
  tr456_wet_lara_load_config();
  tr456_wet_lara_maybe_emit_drip_ripple(tr456_wet_lara_wet_amount());
  tr456_wet_lara_reset_partial_line_if_dry();
  g_wet_lara.last_frame_draws=g_wet_lara.frame_draws;
  g_wet_lara.last_frame_candidates=g_wet_lara.frame_candidates;
  g_wet_lara.last_frame_timing_draws=g_wet_lara.frame_timing_draws;
  g_wet_lara.frame_draws=0;
  g_wet_lara.frame_candidates=0;
  g_wet_lara.frame_timing_draws=0;
}

static void tr456_wet_lara_diag_begin(const char *where) {
  tr456_wet_lara_load_config();
  char msg[2200];
  snprintf(msg,sizeof(msg),
    "wet lara diag session=%d frame=%u where=%s enabled=%d contactOnly=%d objectGate=%d requireNormal=%d useCounts=%d useTiming=%d useWater=%d useRipple=%d useSynthetic=%d underwater=%d underwaterStart=%d rippleNewOnly=%d rippleMin=%d contactFallback=%d useJoints=%d requireJoints=%d debugVisible=%d timingCount=%d wetAmount=%.3f wetDelay=%.2f wetRamp=%.2f holdFrames=%d dryFrames=%d drySeconds=%.2f water=(streak=%d enter=%d grace=%d joints=%d last=%u) synthetic=(streak=%d enter=%d grace=%d joints=%d maxAge=%d last=%u margin=%.1f vertical=%.1f above=%.1f) underwater=(last=%u joints=%d margin=%.1f) ripple=(last=%u grace=%d) lastTimingDraws=%d opacity=%.3f specular=%.2f detail=(%.2f %.2f %.2f) partial=(%d carry=%d valid=%d line=%.1f dir=%.0f cfgDir=%.0f water=%.1f body=%.1f..%.1f rise=%.1f fade=%.1f full=%.1f carryMax=%.1f) count=%d-%d maxPerFrame=%d lastDraws=%d lastCandidates=%d frameRange=%d-%d radiusScale=%.2f vertical=%.1f fallback=(%.2f %.1f %.2f) depthBias=%.5f tint=(%.2f %.2f %.2f)",
    g_diag_session,g_frame_index,where ? where : "unknown",
    g_wet_lara.enabled,g_wet_lara.contact_only,g_wet_lara.object_gate,
    g_wet_lara.require_normal,g_wet_lara.use_draw_counts,
    g_wet_lara.use_timing_draw,g_wet_lara.use_water_contact,
    g_wet_lara.use_ripple_circle,g_wet_lara.use_synthetic_contact,
    g_wet_lara.underwater_sustain,
    g_wet_lara.underwater_can_start,
    g_wet_lara.ripple_circle_new_only,
    g_wet_lara.ripple_min_count,
    g_wet_lara.contact_fallback,
    g_wet_lara.use_joints,
    g_wet_lara.require_joints,g_wet_lara.debug_visible,
    g_wet_lara.timing_count,
    (double)tr456_wet_lara_wet_amount(),
    (double)g_wet_lara.wet_delay_seconds,
    (double)g_wet_lara.wet_ramp_seconds,
    g_wet_lara.hold_frames,
    g_wet_lara.dry_frames,(double)g_wet_lara.dry_seconds,
    g_wet_lara.water_streak,g_wet_lara.water_enter_frames,
    g_wet_lara.water_grace_frames,g_wet_lara.water_min_joints,
    g_wet_lara.last_water_frame,
    g_wet_lara.synthetic_streak,g_wet_lara.synthetic_enter_frames,
    g_wet_lara.synthetic_grace_frames,g_wet_lara.synthetic_min_joints,
    g_wet_lara.synthetic_surface_age_frames,
    g_wet_lara.last_synthetic_frame,
    (double)g_wet_lara.synthetic_margin,
    (double)g_wet_lara.synthetic_vertical,
    (double)g_wet_lara.synthetic_above_surface,
    g_wet_lara.last_underwater_frame,g_wet_lara.underwater_min_joints,
    (double)g_wet_lara.underwater_margin,
    g_wet_lara.last_ripple_frame,g_wet_lara.ripple_grace_frames,
    g_wet_lara.last_frame_timing_draws,
    (double)g_wet_lara.opacity,(double)g_wet_lara.specular,
    (double)g_wet_lara.droplet_strength,
    (double)g_wet_lara.streak_strength,
    (double)g_wet_lara.cloth_darkening,
    g_wet_lara.partial_wet,g_wet_lara.partial_carry_after_exit,
    g_wet_lara.partial_line_valid,
    (double)g_wet_lara.partial_line_y,
    (double)g_wet_lara.partial_line_dir,
    (double)g_wet_lara.partial_direction,
    (double)g_wet_lara.partial_water_y,
    (double)g_wet_lara.partial_body_min_y,
    (double)g_wet_lara.partial_body_max_y,
    (double)g_wet_lara.partial_rise,
    (double)g_wet_lara.partial_fade,
    (double)g_wet_lara.partial_full_margin,
    (double)g_wet_lara.partial_carry_max_delta,
    g_wet_lara.min_count,g_wet_lara.max_count,g_wet_lara.max_per_frame,
    g_wet_lara.last_frame_draws,g_wet_lara.last_frame_candidates,
    g_wet_lara.frame_start,g_wet_lara.frame_end,
    (double)g_wet_lara.radius_scale,(double)g_wet_lara.vertical_radius,
    (double)g_wet_lara.fallback_radius_scale,
    (double)g_wet_lara.fallback_vertical,
    (double)g_wet_lara.fallback_min_speed,
    (double)g_wet_lara.depth_bias,(double)g_wet_lara.tint_r,
    (double)g_wet_lara.tint_g,(double)g_wet_lara.tint_b);
  log_line(msg);
  if(diag_is_active()) diag_consume_line();
}
