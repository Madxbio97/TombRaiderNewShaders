# Shader Define Ownership

This document captures the current shader-facing config contract before deeper
module splits. Shader source factories are already compiled separately, while
shader define generation still lives in the main proxy translation unit.

## Runtime Contract

`src/tr456_proxy_runtime_config.h` is the internal contract exposed to the rest
of the proxy:

| Function | Role |
| --- | --- |
| `ini_int()` | Read an integer value from `[Water]` with a fallback. |
| `ini_float()` | Read a float value from `[Water]` with a fallback. |
| `ini_string()` | Read a string value from `[Water]` with a fallback. |
| `load_runtime_config()` | Refresh and cache runtime feature flags and tuning values. |
| `configured_shader()` | Load external GLSL and inject the generated define block. |

The rest of `src/tr456_proxy_runtime_config.inc` remains private: timestamp
reload checks, clamp helpers, shader define cache, define generation, and define
injection should not be called directly by other modules.

## Define Pipeline

```text
tr456_water.ini
  -> ini_* lookup
  -> load_runtime_config() cached runtime flags
  -> build_shader_defines()
  -> shader_defines() cache
  -> inject_defines()
  -> configured_shader()
  -> shader compile/link in tr456_proxy_programs.inc
```

External GLSL files enter through `configured_shader()`:

| Caller | Shader files |
| --- | --- |
| `synthetic_surface_vertex_shader()` / `synthetic_surface_shader()` | `tr456_water_synthetic_vertex.glsl`, `tr456_water_synthetic.glsl` |
| `flow_lite_vertex_shader()` / `flow_lite_fragment_shader()` | `tr456_water_flow_lite_vertex.glsl`, `tr456_water_flow_lite.glsl` |

`src/tr456_proxy_shader_sources.h` declares the shader source factory surface.
`src/tr456_proxy_shader_sources.c` owns synthetic standing-water, FlowLite, and
diagnostic shader source entry points. It is compiled beside
`src/tr456_water_proxy.c`, so startup shader preloading, program compilation,
and diagnostics share the same entry points without relying on `.inc` include
order.

`configured_shader()` is the tiny internal runtime-config boundary used by this
split module. The shader source factory should not call runtime config internals
directly.

## Define Groups

| Group | Prefix/examples | Owner |
| --- | --- | --- |
| Global water response | `TR456_WATER_REFRACT_STRENGTH`, `TR456_WATER_DEPTH_STRENGTH` | `build_shader_defines()` |
| Standing water | `TR456_WATER_STANDING_*`, `TR456_WATER_STANDING_ORIGINAL_BLEND` | `build_shader_defines()` |
| Contact/ripple/wake | `TR456_WATER_CONTACT_*`, `TR456_WATER_WAKE_*`, `TR456_WATER_RAIN_RIPPLE` | `build_shader_defines()` |
| FlowLite motion and shape | `TR456_WATER_FLOW_*` | `build_shader_defines()` plus `load_runtime_flow_fx_config()` for C-side runtime arrays |
| Reflection/FBO flags | `TR456_WATER_FBO_REFLECTION`, `TR456_WATER_REFLECTION_QUALITY` | `build_shader_defines()` and framebuffer runtime config |
| Synthetic feature toggles | `TR456_WATER_SYNTHETIC_*` | `build_shader_defines()` and synthetic runtime config |

The generated define block is compile-time shader input. Values that must
change without shader recompilation should use uniforms or existing runtime
state instead.

## Change Checklist

When adding a shader-facing setting:

1. Add the INI key to `tr456_water.ini` only if it belongs in the active
   compatibility profile. Add it to `profiles/tr456_water.release.ini` when it
   is release-only or release-critical.
2. Document the key in `INI_SETTINGS.md` if it is user-facing. If it is a hidden
   shader or lab control, classify it in `docs/SETTINGS_OWNERSHIP.csv`.
3. Add C-side cached runtime state only when the CPU path needs it outside
   shader compilation.
4. Add the define in `build_shader_defines()` and keep its name under the
   closest existing prefix.
5. Use the define in `shaders/*.glsl`.
6. Run `tools/audit_ini_settings.ps1 -FailOnUnclassified` and the DLL build.
7. Update release package expectations only for release-critical values, then
   run `tools/audit_ini_settings.ps1 -FailOnPackageDrift`.

Avoid adding new embedded shader strings in C unless they are diagnostics.
Runtime-loaded GLSL files should remain the primary tuning surface.
