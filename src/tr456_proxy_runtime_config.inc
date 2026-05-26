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

static int runtime_file_write_time(const char *file, FILETIME *out) {
  if(!out || !g_dir[0]) return 0;
  char path[MAX_PATH];
  if(!runtime_path(path,file)) return 0;
  WIN32_FILE_ATTRIBUTE_DATA data;
  if(!GetFileAttributesExA(path,GetFileExInfoStandard,&data)) return 0;
  *out=data.ftLastWriteTime;
  return 1;
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
    FILETIME ft;
    g_ini_write_time_valid=runtime_file_write_time("tr456_water.ini",&ft);
    if(g_ini_write_time_valid)
      g_ini_write_time=ft;
    diag_logf("DIAG ini load %s dir=\"%s\" mod=\"%s\"",
      g_ini_text ? "ok" : "missing",g_dir,g_mod_dir);
    g_ini_loaded=1;
  }
  ReleaseSRWLockExclusive(&g_ini_lock);
}

static int reload_ini_if_changed(void) {
  if(!g_dir[0]) return 0;
  FILETIME ft;
  int have_time=runtime_file_write_time("tr456_water.ini",&ft);
  int should_reload=!g_ini_loaded;
  if(!should_reload && have_time && g_ini_write_time_valid &&
     CompareFileTime(&ft,&g_ini_write_time)!=0)
    should_reload=1;
  if(!should_reload && have_time && !g_ini_write_time_valid)
    should_reload=1;
  if(!should_reload) return 0;

  AcquireSRWLockExclusive(&g_ini_lock);
  should_reload=!g_ini_loaded;
  if(!should_reload && have_time && g_ini_write_time_valid &&
     CompareFileTime(&ft,&g_ini_write_time)!=0)
    should_reload=1;
  if(!should_reload && have_time && !g_ini_write_time_valid)
    should_reload=1;
  if(should_reload) {
    char *text=read_text("tr456_water.ini");
    if(g_ini_text)
      free(g_ini_text);
    g_ini_text=text;
    g_ini_loaded=1;
    g_ini_write_time_valid=have_time;
    if(have_time)
      g_ini_write_time=ft;
  }
  ReleaseSRWLockExclusive(&g_ini_lock);
  return should_reload;
}

static void invalidate_runtime_config_caches(void) {
  g_runtime_config_loaded=0;
  tr456_wet_lara_invalidate_config();

  AcquireSRWLockExclusive(&g_shader_defines_lock);
  g_shader_defines_ready=0;
  g_shader_defines_logged=0;
  g_shader_defines_cache[0]=0;
  ReleaseSRWLockExclusive(&g_shader_defines_lock);

  AcquireSRWLockExclusive(&g_shader_text_lock);
  for(size_t i=0;i<sizeof(g_shader_text_cache)/sizeof(g_shader_text_cache[0]);i++) {
    free(g_shader_text_cache[i].text);
    g_shader_text_cache[i].text=0;
    g_shader_text_cache[i].loaded=0;
  }
  ReleaseSRWLockExclusive(&g_shader_text_lock);
  invalidate_synthetic_surface_program();
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

static float trshader_clamp_float(float v, float lo, float hi) {
  return v<lo ? lo : (v>hi ? hi : v);
}

static int trshader_clamp_int(int v, int lo, int hi) {
  return v<lo ? lo : (v>hi ? hi : v);
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

static void load_runtime_core_config(void) {
  g_runtime_safe_mode=ini_int("SafeMode",0) ? 1 : 0;
  g_runtime_shader_patching=ini_int("WaterShaderPatching",0);
  g_runtime_verbose_log=ini_int("VerboseLog",0);
  g_runtime_perf_telemetry=trshader_clamp_int(
    ini_int("PerfTelemetry",0),0,1);
  trshader_compat_read_config();
  g_runtime_shader_binary_cache=ini_int("ShaderBinaryCache",1);
  g_runtime_dump_flow_shader_source=ini_int("DumpFlowShaderSource",0) ? 1 : 0;
  g_runtime_refresh_flow_texture_signatures=
    ini_int("RefreshFlowTextureSignatures",0);
  g_effect_toggle_mask=(unsigned int)ini_int("EffectToggleMask",
    TR456_EFFECT_TOGGLE_MASK)&TR456_EFFECT_TOGGLE_MASK;
}

static void load_runtime_flow_fx_config(void) {
  g_runtime_flow_fx0[0]=trshader_clamp_float(
    ini_float("FlowReflectionStrength",0.45f),0.0f,2.0f);
  g_runtime_flow_fx0[1]=trshader_clamp_float(
    ini_float("FlowRefractionWarp",0.70f),0.0f,3.5f);
  g_runtime_flow_fx0[2]=trshader_clamp_float(
    ini_float("FlowSurfaceTension",0.0f),0.0f,2.5f);
  g_runtime_flow_fx0[3]=trshader_clamp_float(
    ini_float("FlowCrossDistortion",0.60f),0.0f,3.0f);
  g_runtime_flow_fx1[0]=trshader_clamp_float(
    ini_float("FlowGlintStrength",0.35f),0.0f,2.0f);
  g_runtime_flow_fx1[1]=trshader_clamp_float(
    ini_float("FlowSpecularStreakStrength",0.30f),0.0f,2.0f);
  g_runtime_flow_fx1[2]=trshader_clamp_float(
    ini_float("FlowStreakFoam",0.0f),0.0f,2.0f);
  g_runtime_flow_fx1[3]=trshader_clamp_float(
    ini_float("FlowSurfaceDistortion",0.75f),0.0f,3.5f);
  g_runtime_flow_fx2[0]=trshader_clamp_float(
    ini_float("FlowWaterStrength",0.85f),0.0f,2.0f);
  g_runtime_flow_fx2[1]=trshader_clamp_float(
    ini_float("FlowBodyStrength",0.12f),0.0f,2.0f);
  g_runtime_flow_fx2[2]=trshader_clamp_float(
    ini_float("FlowRidgeStrength",0.20f),0.0f,2.0f);
  g_runtime_flow_fx2[3]=trshader_clamp_float(
    ini_float("FlowDetailStrength",0.0f),0.0f,2.0f);
  g_runtime_flow_fx3[0]=trshader_clamp_float(
    ini_float("TileSeamSoftening",1.0f),0.0f,2.0f);
  g_runtime_flow_fx3[1]=trshader_clamp_float(
    ini_float("TileSeamWidth",0.035f),0.004f,0.18f);
  g_runtime_flow_fx3[2]=trshader_clamp_float(
    ini_float("FlowInPlaceRefractionBoost",1.20f),0.25f,3.0f);
  g_runtime_flow_fx3[3]=trshader_clamp_float(
    ini_float("FlowInPlaceReflectionBoost",1.55f),0.25f,3.0f);
}

static void load_runtime_framebuffer_config(void) {
  g_runtime_fbo_reflection=ini_int("FramebufferReflection",0);
  g_runtime_fbo_capture_interval=trshader_clamp_int(
    ini_int("FramebufferCaptureInterval",1),1,8);
  g_runtime_fbo_warmup_frames=trshader_clamp_int(
    ini_int("FramebufferWarmupFrames",0),0,600);
  g_runtime_fbo_scale=trshader_clamp_int(ini_int("FramebufferScale",1),1,4);
  g_runtime_fbo_error_check=ini_int("FramebufferErrorCheck",0) ? 1 : 0;
}

static void load_runtime_ripple_contact_config(void) {
  g_runtime_ripple_min_count=ini_int("RippleSpriteMinCount",192);
  g_runtime_ripple_center_mode=trshader_clamp_int(
    ini_int("RippleSpriteCenterMode",1),0,1);
  g_runtime_synthetic_contact_max_samples=
    trshader_clamp_int(ini_int("SyntheticContactMaxSamples",192),32,512);
}

static void load_runtime_synthetic_flags_config(void) {
  g_runtime_synthetic_surface=ini_int("SyntheticWaterSurface",0);
  g_runtime_synthetic_standing_only=ini_int("SyntheticStandingWaterOnly",0);
  g_runtime_synthetic_standing_replace_original=
    ini_int("SyntheticStandingReplaceOriginal",
      g_runtime_synthetic_standing_only);
  g_runtime_synthetic_standing_bounds_guard=
    ini_int("SyntheticStandingBoundsGuard",1) ? 1 : 0;
  g_runtime_synthetic_standing_preserve_mask=
    ini_int("SyntheticStandingPreserveMask",1) ? 1 : 0;
  g_runtime_synthetic_flow_surface=ini_int("SyntheticFlowSurface",0);
  g_runtime_synthetic_flow_replace_original=
    ini_int("SyntheticFlowReplaceOriginal",0) ? 1 : 0;
  g_runtime_flow_lite_surface=ini_int("FlowLiteSurface",0) ? 1 : 0;
  g_runtime_flow_inplace_patch=ini_int("FlowInPlacePatch",1) ? 1 : 0;
  g_runtime_synthetic_reflect_surface=ini_int("SyntheticReflectSurface",1);
}

static void load_runtime_native_overlay_config(void) {
  g_runtime_native_water_overlay_suppress=
    ini_int("NativeWaterOverlaySuppress",1) ? 1 : 0;
  g_runtime_native_water_overlay_max_count=trshader_clamp_int(
    ini_int("NativeWaterOverlayMaxCount",384),0,4096);
  g_runtime_native_water_overlay_y_margin=trshader_clamp_float(
    ini_float("NativeWaterOverlayYMargin",96.0f),0.0f,4096.0f);
}

static void load_runtime_underwater_guard_config(void) {
  g_runtime_underwater_surface_guard=ini_int("UnderwaterSurfaceGuard",0) ? 1 : 0;
  g_runtime_underwater_surface_guard_grace_frames=trshader_clamp_int(
    ini_int("UnderwaterSurfaceGuardGraceFrames",30),0,600);
  g_runtime_underwater_surface_refraction_scale=trshader_clamp_float(
    ini_float("UnderwaterSurfaceRefractionScale",1.0f),0.0f,2.0f);
  g_runtime_underwater_surface_reflection_scale=trshader_clamp_float(
    ini_float("UnderwaterSurfaceReflectionScale",1.0f),0.0f,2.0f);
  g_runtime_underwater_surface_opacity_scale=trshader_clamp_float(
    ini_float("UnderwaterSurfaceOpacityScale",1.0f),0.0f,2.0f);
}

static void load_runtime_synthetic_surface_config(void) {
  g_runtime_synthetic_overlay_depth_mode=trshader_clamp_int(
    ini_int("SyntheticOverlayDepthMode",0),0,2);
  g_runtime_synthetic_reflect_original_mask=trshader_clamp_float(
    ini_float("SyntheticReflectOriginalMask",1.0f),0.0f,1.0f);
  g_runtime_synthetic_debug_solid=ini_int("SyntheticDebugSolid",0) ? 1 : 0;
  g_runtime_flow_texture_fallback=ini_int("FlowTextureFallback",1);
  g_runtime_underlay_pattern=ini_int("WaterUnderlayPattern",0);
  g_runtime_underlay_flow_pattern=ini_int("WaterUnderlayPatternFlow",0);
  g_runtime_synthetic_opacity=trshader_clamp_float(
    ini_float("SyntheticSurfaceOpacity",0.58f),0.0f,1.0f);
  g_runtime_synthetic_tint=trshader_clamp_float(
    ini_float("SyntheticSurfaceTint",1.0f),0.0f,2.0f);
  g_runtime_synthetic_reflection=trshader_clamp_float(
    ini_float("SyntheticSurfaceReflection",0.76f),0.0f,2.0f);
  g_runtime_underlay_pattern_strength=trshader_clamp_float(
    ini_float("WaterUnderlayPatternStrength",0.0f),0.0f,2.0f);
  g_runtime_synthetic_compile_delay_frames=trshader_clamp_int(
    ini_int("SyntheticCompileDelayFrames",0),0,600);
  g_runtime_synthetic_compile_sync=ini_int("SyntheticCompileSync",1) ? 1 : 0;
}

static void load_runtime_config(void) {
  if(reload_ini_if_changed())
    invalidate_runtime_config_caches();
  if(g_runtime_config_loaded) return;
  load_runtime_core_config();
  load_runtime_flow_fx_config();
  load_runtime_framebuffer_config();
  load_runtime_ripple_contact_config();
  load_runtime_synthetic_flags_config();
  load_runtime_native_overlay_config();
  load_runtime_underwater_guard_config();
  load_runtime_synthetic_surface_config();
  trshader_compat_apply_runtime_policy();
  g_runtime_config_loaded=1;
}

static void build_shader_defines(char *out, size_t out_size) {
  const float refract_wave=ini_float("RefractionWaveStrength",0.90f);
  const float water_volume=ini_float("WaterVolumeStrength",0.45f);
  const float shoreline=ini_float("ShorelineStrength",0.35f);
  const float refract=ini_float("RefractStrength",0.75f);
  const float reflect=ini_float("ReflectStrength",0.85f);
  const float glint=ini_float("GlintStrength",0.22f);
  const float sparkle=ini_float("SurfaceSparkleStrength",0.58f);
  const float foam=ini_float("FoamStrength",0.20f);
  const float chroma=ini_float("ChromaStrength",0.10f);
  const float surface_caustic=ini_float("SurfaceCausticStrength",0.0f);
  const float surface_blue_stripe=ini_float("SurfaceBlueStripeStrength",0.0f);
  const float standing_life=ini_float("StandingLifeStrength",0.82f);
  const float standing_micro=ini_float("StandingMicroChopStrength",0.58f);
  const float standing_micro_tremble=
    ini_float("StandingMicroTrembleStrength",1.15f);
  const float standing_tremble=ini_float("StandingTrembleStrength",1.65f);
  const float standing_breath=ini_float("StandingBreathStrength",1.0f);
  const float standing_tension=ini_float("StandingTensionStrength",0.66f);
  const float standing_drift=ini_float("StandingDriftSpeed",0.74f);
  float standing_layer_y_offset=ini_float("StandingLayerYOffset",0.0f);
  if(standing_layer_y_offset<-256.0f) standing_layer_y_offset=-256.0f;
  if(standing_layer_y_offset>256.0f) standing_layer_y_offset=256.0f;
  const float standing_original_blend=ini_float("StandingOriginalBlend",0.0f);
  const float standing_refraction_original_blend=
    ini_float("StandingRefractionOriginalBlend",0.0f);
  const float reflect_original_mask=
    ini_float("SyntheticReflectOriginalMask",1.0f);
  const float depth=ini_float("DepthStrength",0.45f);
  const float surface_relief=ini_float("SurfaceRelief",0.85f);
  const float wake_strength=ini_float("WakeStrength",1.0f);
  const float wake_width=ini_float("WakeWidth",0.42f);
  const float wake_length=ini_float("WakeLength",0.58f);
  const float contact_ripple_decay=ini_float("ContactRippleDecay",1.10f);
  const float contact_wake_directional=ini_float("ContactWakeDirectional",0.72f);
  const float lara_splash=ini_float("LaraSplashStrength",1.15f);
  const float flow_contact=ini_float("FlowContactStrength",1.0f);
  const float flow_contact_normal=ini_float("FlowContactNormalStrength",1.0f);
  const float flow_contact_ripples=ini_float("FlowContactRipples",0.0f);
  const float flow_contact_distortion=ini_float("FlowContactDistortion",1.0f);
  const float underlay_pattern=ini_float("WaterUnderlayPatternStrength",0.0f);
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
  const float bump_mapping=ini_float("BumpMappingStrength",0.42f);
  const float bump_scale=ini_float("BumpMappingScale",0.92f);
  const float flow_bump=ini_float("FlowBumpMappingStrength",0.82f);
  const float synthetic_bump=ini_float("SyntheticBumpMappingStrength",0.72f);
  const float flow_detail=ini_float("FlowDetailStrength",0.0f);
  const float flow_strength=ini_float("FlowWaterStrength",0.85f);
  const float flow_location_variation=
    ini_float("FlowLocationVariation",1.0f);
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
  const float flow_bottom_refraction=
    ini_float("FlowBottomRefractionStrength",1.35f);
  const float flow_bottom_refraction_scale=
    ini_float("FlowBottomRefractionScale",1.18f);
  const float flow_bottom_visibility=ini_float("FlowBottomVisibility",0.72f);
  const float flow_fresnel_reflection=ini_float("FlowFresnelReflection",1.18f);
  const float flow_surface_tension=ini_float("FlowSurfaceTension",0.0f);
  const float flow_cross_distortion=ini_float("FlowCrossDistortion",0.60f);
  const float flow_direction_sign=ini_float("FlowDirectionSign",1.0f);
  const float flow_original_deformation=ini_float("FlowOriginalDeformation",1.0f);
  const float flow_original_sync=ini_float("FlowOriginalSync",1.0f);
  const float flow_body=ini_float("FlowBodyStrength",0.12f);
  const float flow_ridge=ini_float("FlowRidgeStrength",0.20f);
  const float flow_edge_foam=ini_float("FlowEdgeFoamStrength",0.0f);
  const float flow_ribbon=ini_float("FlowRibbonStrength",0.25f);
  const float flow_eddy_foam=ini_float("FlowEddyFoamStrength",0.0f);
  const float flow_depth_body=ini_float("FlowDepthBodyStrength",0.12f);
  const float flow_specular_streak=ini_float("FlowSpecularStreakStrength",0.30f);
  const float flow_cross_wave=ini_float("FlowCrossWaveStrength",0.25f);
  const float flow_breakup=ini_float("FlowBreakupStrength",0.20f);
  const int flow_fast_path=ini_int("FlowFastPath",0) ? 1 : 0;
  int flow_debug_mode=ini_int("FlowLiteDebugMode",0);
  if(flow_debug_mode<0) flow_debug_mode=0;
  if(flow_debug_mode>2) flow_debug_mode=2;
  int contact_max_active=ini_int("ContactMaxActive",6);
  if(contact_max_active<1) contact_max_active=1;
  if(contact_max_active>16) contact_max_active=16;
  const int fbo_reflection=ini_int("FramebufferReflection",1);
  int reflection_quality=ini_int("ReflectionQuality",1);
  if(reflection_quality<0) reflection_quality=0;
  if(reflection_quality>2) reflection_quality=2;
  const float synthetic_reflection=ini_float("SyntheticSurfaceReflection",0.76f);
  const int synthetic_bump_enabled=(bump_mapping*synthetic_bump)>0.001f;
  const int synthetic_reflection_enabled=
    (fbo_reflection && reflection_quality>0 &&
     synthetic_reflection*reflect>0.001f);
  const int synthetic_flow_reflection_enabled=
    (fbo_reflection && reflection_quality>0 &&
     synthetic_reflection*flow_reflection*reflect>0.001f);
  const int synthetic_debug_solid=g_runtime_synthetic_debug_solid;
  snprintf(out,out_size,
    "#define TR456_WATER_REFRACTION_WAVE_STRENGTH %.6f\n"
    "#define TR456_WATER_VOLUME_STRENGTH %.6f\n"
    "#define TR456_WATER_SHORELINE_STRENGTH %.6f\n"
    "#define TR456_WATER_REFRACT_STRENGTH %.6f\n"
    "#define TR456_WATER_REFLECT_STRENGTH %.6f\n"
    "#define TR456_WATER_GLINT_STRENGTH %.6f\n"
    "#define TR456_WATER_SPARKLE_STRENGTH %.6f\n"
    "#define TR456_WATER_FOAM_STRENGTH %.6f\n"
    "#define TR456_WATER_CHROMA_STRENGTH %.6f\n"
    "#define TR456_WATER_SURFACE_CAUSTIC %.6f\n"
    "#define TR456_WATER_BLUE_STRIPE %.6f\n"
    "#define TR456_WATER_STANDING_LIFE %.6f\n"
    "#define TR456_WATER_STANDING_MICRO_CHOP %.6f\n"
    "#define TR456_WATER_STANDING_MICRO_TREMBLE %.6f\n"
    "#define TR456_WATER_STANDING_TREMBLE %.6f\n"
    "#define TR456_WATER_STANDING_BREATH %.6f\n"
    "#define TR456_WATER_STANDING_TENSION %.6f\n"
    "#define TR456_WATER_STANDING_DRIFT_SPEED %.6f\n"
    "#define TR456_WATER_STANDING_LAYER_Y_OFFSET %.6f\n"
    "#define TR456_WATER_STANDING_ORIGINAL_BLEND %.6f\n"
    "#define TR456_WATER_STANDING_REFRACTION_ORIGINAL_BLEND %.6f\n"
    "#define TR456_WATER_REFLECT_ORIGINAL_MASK %.6f\n"
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
    "#define TR456_WATER_LARA_SPLASH_STRENGTH %.6f\n"
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
    "#define TR456_WATER_FLOW_LOCATION_VARIATION %.6f\n"
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
    "#define TR456_WATER_FLOW_BOTTOM_REFRACTION_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_BOTTOM_REFRACTION_SCALE %.6f\n"
    "#define TR456_WATER_FLOW_BOTTOM_VISIBILITY %.6f\n"
    "#define TR456_WATER_FLOW_FRESNEL_REFLECTION %.6f\n"
    "#define TR456_WATER_FLOW_SURFACE_TENSION %.6f\n"
    "#define TR456_WATER_FLOW_CROSS_DISTORTION %.6f\n"
    "#define TR456_WATER_FLOW_DIRECTION_SIGN %.6f\n"
    "#define TR456_WATER_FLOW_ORIGINAL_DEFORMATION %.6f\n"
    "#define TR456_WATER_FLOW_ORIGINAL_SYNC %.6f\n"
    "#define TR456_WATER_FLOW_BODY %.6f\n"
    "#define TR456_WATER_FLOW_RIDGE %.6f\n"
    "#define TR456_WATER_FLOW_EDGE_FOAM %.6f\n"
    "#define TR456_WATER_FLOW_RIBBON %.6f\n"
    "#define TR456_WATER_FLOW_EDDY_FOAM %.6f\n"
    "#define TR456_WATER_FLOW_DEPTH_BODY %.6f\n"
    "#define TR456_WATER_FLOW_SPECULAR_STREAK %.6f\n"
    "#define TR456_WATER_FLOW_CROSS_WAVE %.6f\n"
    "#define TR456_WATER_FLOW_BREAKUP %.6f\n"
    "#define TR456_WATER_FLOW_FAST_PATH %d\n"
    "#define TR456_WATER_FLOW_DEBUG_MODE %d\n"
    "#define TR456_WATER_FLOW_CONTACT_STRENGTH %.6f\n"
    "#define TR456_WATER_FLOW_CONTACT_NORMAL %.6f\n"
    "#define TR456_WATER_FLOW_CONTACT_RIPPLES %.6f\n"
    "#define TR456_WATER_FLOW_CONTACT_DISTORTION %.6f\n"
    "#define TR456_WATER_CONTACT_MAX_ACTIVE %d\n"
    "#define TR456_WATER_UNDERLAY_PATTERN_STRENGTH %.6f\n"
    "#define TR456_WATER_REFLECTION_QUALITY %d\n"
    "#define TR456_WATER_FBO_REFLECTION %d\n"
    "#define TR456_WATER_SYNTHETIC_BUMP_ENABLED %d\n"
    "#define TR456_WATER_SYNTHETIC_REFLECTION_ENABLED %d\n"
    "#define TR456_WATER_SYNTHETIC_FLOW_REFLECTION_ENABLED %d\n"
    "#define TR456_WATER_SYNTHETIC_DEBUG_SOLID %d\n",
    (double)refract_wave,
    (double)water_volume,(double)shoreline,
    (double)refract,(double)reflect,
    (double)glint,(double)sparkle,(double)foam,(double)chroma,
    (double)surface_caustic,(double)surface_blue_stripe,
    (double)standing_life,(double)standing_micro,
    (double)standing_micro_tremble,(double)standing_tremble,
    (double)standing_breath,
    (double)standing_tension,(double)standing_drift,
    (double)standing_layer_y_offset,
    (double)standing_original_blend,
    (double)standing_refraction_original_blend,
    (double)reflect_original_mask,
    (double)depth,(double)surface_relief,
    (double)wake_strength,(double)wake_width,(double)wake_length,
    (double)rain_ripple,(double)wet_edge,
    (double)micro_ripple,
    (double)swell_strength,(double)wake_wave,
    (double)contact_ripple_decay,(double)contact_wake_directional,
    (double)lara_splash,
    (double)edge_wave,
    (double)reflection_contrast,(double)reflection_shimmer,
    (double)fresnel_strength,
    (double)contact_edge,(double)depth_absorption,
    (double)water_saturation,(double)water_brightness,(double)water_texture,
    (double)bump_mapping,(double)bump_scale,
    (double)flow_bump,(double)synthetic_bump,(double)flow_detail,
    (double)flow_strength,(double)flow_location_variation,
    (double)flow_reflection,(double)flow_opacity,
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
    (double)flow_bottom_refraction,
    (double)flow_bottom_refraction_scale,
    (double)flow_bottom_visibility,
    (double)flow_fresnel_reflection,
    (double)flow_surface_tension,
    (double)flow_cross_distortion,
    (double)flow_direction_sign,
    (double)flow_original_deformation,
    (double)flow_original_sync,
    (double)flow_body,
    (double)flow_ridge,
    (double)flow_edge_foam,
    (double)flow_ribbon,
    (double)flow_eddy_foam,
    (double)flow_depth_body,
    (double)flow_specular_streak,
    (double)flow_cross_wave,
    (double)flow_breakup,
    flow_fast_path,
    flow_debug_mode,
    (double)flow_contact,
    (double)flow_contact_normal,
    (double)flow_contact_ripples,
    (double)flow_contact_distortion,
    contact_max_active,
    (double)underlay_pattern,
    reflection_quality,
    fbo_reflection,
    synthetic_bump_enabled,
    synthetic_reflection_enabled,
    synthetic_flow_reflection_enabled,
    synthetic_debug_solid);
  if(!g_shader_defines_logged) {
    char msg[1024];
    snprintf(msg,sizeof(msg),
      "shader defines flow strength=%.3f location=%.3f opacity=%.3f speed=%.3f dir=%.0f chroma=%.3f standing=%.3f standingLife=%.3f/%.3f/%.3f/%.3f/%.3f/%.3f/%.3f yOffset=%.1f originalBlend=%.3f refrOriginal=%.3f reflectMask=%.3f vertex=%.3f wave=%.3f volumeWave=%.3f/%.3f warp=%.3f surfaceDist=%.3f tension=%.3f crossDist=%.3f fast=%d flowDebug=%d contact=%.3f/%.3f ripples=%.3f distort=%.3f wakeDir=%.3f rippleDecay=%.3f reflectionShimmer=%.3f reflectionQ=%d originalDeform=%.3f detail=%.3f/%.3f fbo=%d debugSolid=%d toggles=0x%03X",
      (double)flow_strength,(double)flow_location_variation,
      (double)flow_opacity,(double)flow_speed,
      (double)flow_direction_sign,(double)flow_chroma,
       (double)flow_standing_blend,
       (double)standing_life,(double)standing_micro,
       (double)standing_micro_tremble,
       (double)standing_tremble,(double)standing_breath,
       (double)standing_tension,
       (double)standing_drift,(double)standing_layer_y_offset,
       (double)standing_original_blend,
       (double)standing_refraction_original_blend,
       (double)reflect_original_mask,
       (double)flow_vertex,(double)flow_wave,
       (double)flow_volume_wave,(double)flow_volume_wave_scale,
       (double)flow_refraction_warp,
      (double)flow_surface_distortion,(double)flow_surface_tension,
      (double)flow_cross_distortion,
      flow_fast_path,flow_debug_mode,
      (double)flow_contact,(double)flow_contact_normal,
      (double)flow_contact_ripples,(double)flow_contact_distortion,
      (double)contact_wake_directional,(double)contact_ripple_decay,
      (double)reflection_shimmer,
      reflection_quality,
      (double)flow_original_deformation,
      0.0,(double)flow_detail,
      fbo_reflection,synthetic_debug_solid,
      g_effect_toggle_mask&TR456_EFFECT_TOGGLE_MASK);
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

char *configured_shader(const char *file, const char *label) {
  ShaderTextCache *entry=shader_text_cache_entry(file);
  if(!entry) {
    char *text=read_text(file);
    if(!text) return 0;
    char *out=inject_defines(text);
    free(text);
    return out;
  }

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
