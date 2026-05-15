# TR456 Water Proxy

Experimental OpenGL wrapper for improving water in Tomb Raider Remastered.

The project installs a local `OpenGL32.dll` next to `tomb123.exe` or
`tomb456.exe`. That DLL
chains to the game's previous OpenGL wrapper as `OpenGL32_orig.dll`, intercepts
`wglGetProcAddress("glShaderSource")`, detects known water shader sources,
and replaces them with the GLSL files copied to the `tr456_water` support
directory.

Current behavior:

- forwards the original OpenGL32 exports to `OpenGL32_orig.dll`;
- replaces the water surface shader with `tr456_water\tr456_water_surface.glsl`;
- replaces the water surface vertex shader with `tr456_water\tr456_water_surface_vertex.glsl`;
- attaches `tr456_water\tr456_water_surface_geometry.glsl` when contact mesh
  subdivision is enabled;
- replaces the water reflection shader with `tr456_water\tr456_water_reflect.glsl`;
- replaces the water reflection vertex shader with `tr456_water\tr456_water_reflect_vertex.glsl`;
- replaces the screen-space water/refraction pass with `tr456_water\tr456_water_ssr.glsl`;
- replaces the detected flowing-water shader with `tr456_water\tr456_water_flow.glsl`;
- replaces the detected flowing-water vertex shader with `tr456_water\tr456_water_flow_vertex.glsl`;
- attaches `tr456_water\tr456_water_flow_geometry.glsl` for subdivided flowing
  contact waves when enabled;
- replaces the detected authored ripple sprite shader with `tr456_water\tr456_water_ripple.glsl`;
- preloads and caches replacement shader sources, then injects tuning defines
  from `tr456_water\tr456_water.ini` when each shader is compiled;
- copies the current framebuffer into `uTrWaterScene` before the first tracked
  water draw and binds it to all replacement water shaders;
- writes runtime diagnostics to `tr456_water\tr456_water_proxy.log`;
- does not patch the game executable for the normal proxy flow.

## Paths

Default game directory:

```powershell
D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered
```

Build output:

```powershell
build\OpenGL32.dll
```

Installed runtime files:

```text
OpenGL32.dll
OpenGL32_orig.dll
tr456_water\tr456_water_surface.glsl
tr456_water\tr456_water_surface_vertex.glsl
tr456_water\tr456_water_surface_geometry.glsl
tr456_water\tr456_water_reflect.glsl
tr456_water\tr456_water_reflect_vertex.glsl
tr456_water\tr456_water_ssr.glsl
tr456_water\tr456_water_flow.glsl
tr456_water\tr456_water_flow_vertex.glsl
tr456_water\tr456_water_flow_geometry.glsl
tr456_water\tr456_water_grid_vertex.glsl
tr456_water\tr456_water_grid_geometry.glsl
tr456_water\tr456_water_grid.glsl
tr456_water\tr456_water_synthetic_vertex.glsl
tr456_water\tr456_water_synthetic.glsl
tr456_water\tr456_water_ripple.glsl
tr456_water\tr456_water.ini
tr456_water\tr456_water_proxy.log
```

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

The build script uses Zig and reads the export list from the game's previous
OpenGL DLL. It looks for, in order:

1. `tr456_water\OpenGL32.dll.tr456-prev.bak`;
2. `OpenGL32.dll.tr456-prev.bak`;
3. `OpenGL32_orig.dll`;
4. `OpenGL32.dll`.

You can override paths explicitly:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 `
  -GameDir "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered" `
  -Zig "C:\zig\zig.exe"
```

## Install

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1
```

The installer prepares `OpenGL32_orig.dll` as the forward target, copies the
proxy DLL, creates `tr456_water`, copies the shader/config files there, and
clears the old proxy log. If it has to replace an existing support-directory
INI, it writes a timestamped `.bak` next to that INI first. It leaves
`tomb123.exe`/`tomb456.exe` untouched.

If you still have a `tomb123.exe.tr456-water.bak` or
`tomb456.exe.tr456-water.bak` from an older local
experiment and want to restore it during install, pass:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1 -RestoreExeBackup
```

## Verify

After launching the game once, check:

```powershell
Get-Content "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered\tr456_water\tr456_water_proxy.log"
```

A successful run should include lines like:

```text
tr456 water proxy loaded
loaded OpenGL32_orig.dll
using external surface shader
preloaded water shader sources
patched surface shader
using external reflect shader
patched reflect shader
using external screen-space water shader
patched screen-space water shader
```

## Tuning Shaders

Edit:

```text
shaders\tr456_water_surface.glsl
shaders\tr456_water_surface_vertex.glsl
shaders\tr456_water_surface_geometry.glsl
shaders\tr456_water_reflect.glsl
shaders\tr456_water_reflect_vertex.glsl
shaders\tr456_water_ssr.glsl
shaders\tr456_water_flow.glsl
shaders\tr456_water_flow_vertex.glsl
shaders\tr456_water_flow_geometry.glsl
```

Then reinstall to copy them into the game support directory:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1
```

Restart the game after shader changes. The proxy preloads replacement shaders
once per process and then serves cached source when the game compiles the
original shader.

Runtime tuning lives in the support directory:

```text
tr456_water\tr456_water.ini
```

Useful values:

```ini
[Water]
DebugMode=0
GameShaderReplacement=0
ReflectionQuality=1
FramebufferReflection=1
FramebufferCaptureInterval=1
FramebufferWarmupFrames=0
FramebufferScale=1
ShaderPreload=1
ContactMeshSubdivision=0
WaterGridOverlay=0
WaterGridFlowOverlay=0
WaterGridSubdivision=8
WaterGridStrength=0.92
WaterGridOpacity=0.24
WaterGridFlowOpacity=0.18
SyntheticFlowSurface=1
SyntheticFlowOnly=1
SurfaceVertexStrength=0.00
SurfaceVertexWaveStrength=0.00
FlowVertexStrength=0.00
FlowWaveStrength=1.48
FlowVolumeWaveStrength=0.62
FlowVolumeWaveScale=1.00
FlowSpeed=10.00
FlowSingleLayer=0.00
FlowDirectionSign=1.00
Opacity=0.64
WaterTextureStrength=1.00
WaterDetailStrength=0.22
WaterDetailScale=1.00
FlowDetailStrength=0.76
FlowDetailScale=1.00
SyntheticCompileDelayFrames=240
```

Reflection notes:

- `ReflectionQuality` controls extra reflection texture samples: `0` fastest,
  `1` balanced, `2` full rough reflection sampling.
- `Opacity` lowers the strength of the replacement water color so more of the
  original scene shows through.
- `ForceReflection` keeps reflections visible even when the game reports that
  the current water has no authored reflection.
- `SceneReflectionStrength` controls the mirror-sampled scene fallback.
- `ReflectionContrast` lifts the copied scene/reflection source before the
  water tint is applied.
- `RoughReflection` softens and broadens reflected scenery with extra FBO
  samples, especially on disturbed water.
- `FresnelStrength` controls the grazing-angle reflection boost.
- `ContactEdge` adds subtle darkening and wetline energy where water meets
  strong scene edges.
- `DepthAbsorption` makes darker/deeper-looking areas absorb more light without
  adding a milky haze.
- `WallReflectionStretch` vertically stretches reflected walls across corridor
  water.
- `WaterSaturation` and `WaterBrightness` tune the final color grade.
- `WaterTextureStrength` increases visible water texture contrast and fine
  surface patterning without making the whole surface much brighter.
- `WaterDetailStrength` and `FlowDetailStrength` now stay conservative in the
  default profile: enough fine relief for the custom layer, but not enough to
  dominate the game-authored water texture.
- `FramebufferReflection=1` enables the experimental framebuffer copy path used
  by `uTrWaterScene` in `tr456_water_ssr.glsl`.
- `FramebufferScale=1`, `FramebufferCaptureInterval=1`, and
  `FramebufferWarmupFrames=0` keep the scene/refraction source on the original
  full-resolution path. The downsampled path is faster but can introduce camera
  artifacts, so it is not the default profile.
- `ShaderPreload=1` delays lightweight source preloading on a background thread;
  `2` forces the old immediate full preload path for diagnostics.
- `GameShaderReplacement=0` keeps the game's original water shaders at startup
  and uses them as tracked draw sources for the synthetic layer. Set it to `1`
  when debugging the older full GLSL replacement path.
- `SyntheticCompileDelayFrames` postpones the heavy synthetic-water program link
  so the main menu can become responsive before the custom layer is compiled.
- `DiagnosticDumpShaders=1` saves unknown GLSL sources under
  `tr456_water\diagnostics`; `DiagnosticLogShaders=1` also logs unknown shader
  previews. Both default to `0` now for cleaner startup. Press `Insert`
  in-game to log active draw/program info for `DiagnosticFrames` frames.
- `RippleStrength` adds a screen-space wake around the configured center. It is
  a first approximation until we have Lara's real world position and velocity.
- `SurfaceRelief` makes the water normals/refraction feel less flat.
- `SurfaceVertexStrength` and `SurfaceVertexWaveStrength` add broad geometric
  breathing to the regular water surface before the fragment shader runs.
- `PixelWaveStrength` and `RefractionWaveStrength` make the visible waves and
  screen refraction stronger without relying on coarse vertex displacement.
- `WaterVolumeStrength` adds depth tint and light absorption so water reads as
  a body of water rather than a flat transparent plane; higher values mute the
  bottom before they brighten the surface.
- `ShorelineStrength` boosts the subtle wetline/edge response where the water
  surface meets authored edges and shallower areas.
- `GameRippleStrength` turns the game's own drawn circular water ripples into
  extra local per-pixel distortion and wake energy. This is the preferred
  Lara-linked source for interactive rings; it also controls the replacement
  ripple sprite pass.
- `WakeStrength`, `WakeWidth`, and `WakeLength` shape the animated walking
  wake around the configured screen-space center.
- `ContactWaveStrength`, `ContactWaveRadius`, and `ContactWaveSpeed` control
  the Lara-linked circular waves derived from the game's authored ripple sprite
  pass. The proxy packs the detected sprite radius into `uContacts[16]` so the
  surface wave can match the sprite ring size instead of using a fixed radius.
- `ContactVertexStrength` adds a subtle geometric crest, while
  `ContactNormalStrength` controls the stronger per-pixel normal/refraction
  response that keeps the rings round on coarse water meshes.
- `SafeVolumeStrength` restores visible water relief without moving mesh
  vertices. It bends normals, refraction, reflections, foam, and glints, so the
  water gains volume even when polygon displacement is toggled off.
- `TileSeamSoftening` dampens refraction and reflection offsets near authored
  tile borders, and `TileSeamWidth` controls the guarded border width. This
  hides seams that appear only when strong distortion samples across tile edges.
- `ContactMeshSubdivision` keeps the legacy in-program mesh path off by
  default. `WaterGridOverlay=0` keeps the extra water-grid pass disabled for
  the current near-vanilla baseline. If enabled, it reuses the original water draw as a
  mask, but its own geometry shader builds an 8-step heightfield grid with
  fewer varyings, so it can create smoother moving waves without exposing the
  old authored triangle normals. `WaterGridFlowOverlay=0` keeps that overlay
  off for flowing water by default, because repeated transparent flow tiles can
  expose rectangular seams. `WaterGridStrength` and `WaterGridOpacity` tune the
  overlay.
- `CalmMirrorStrength` creates quieter mirror patches inside standing water,
  `RainRippleStrength` controls the procedural mesh rings, and
  `WetEdgeStrength` boosts the wet reflective meniscus near authored edges.
- `FlowLaneStrength` adds speed lanes and stretched reflection bands to flowing
  water, while `FlowSwirlStrength` adds soft eddies and bubble foam.
- `ContactCoordMode` chooses how contact centers are decoded: `1` uses world
  `x/z`, `2` uses `x/y`, and `0` auto-picks the closer interpretation.
- `PatchRipplePass` enables the experimental authored ripple sprite tracking.
  `RippleSpriteMinCount` is the legacy draw-count gate used by the contact
  detector; accepted circular sprite draws seed the grid deformation in screen
  space, so Lara and rain rings keep the same round footprint on the water.
  `RippleSpriteCenterMode=1` treats ripple sprites as `0..1` quads; set it to
  `0` if the circle center appears shifted by half a sprite.
  `RippleSpriteVisual=0` keeps the shared sprite shader visually original, which
  avoids water-like shading on splashes while still using accepted ring draws as
  contact sources for the live water layer.
- `SyntheticFlowSurface=1` routes detected flowing-water draws through the
  synthetic surface program. `SyntheticFlowOnly=1` skips the original flow draw
  after the synthetic program has linked, so the experiment is owned by the
  custom layer while still falling back safely if the synthetic shader fails.
  Waterfall sheets, rock cascades, and spray/splash materials are classified as
  original-only. The classifier combines draw shape, `uParams`, and the bound
  flow texture material so the bypass follows the same waterfall/spray texture
  across scenes instead of relying on one captured draw count.
- `MicroRippleStrength` and `MicroRippleScale` add constant fine ripples even
  when the game's authored noise looks flat.
- `MirrorRoughness` breaks up mirror-like reflections without disabling them.
- `SwellStrength` and `SwellScale` add a slower final wave layer that keeps the
  surface from looking like a flat sheet.
- `WakeWaveStrength` controls the extra ring waves around the configured Lara
  screen-space center.
- `EdgeWaveStrength` and `EdgeWaveWidth` add small animated waterline ripples
  along shoreline/contact edges so they do not look perfectly straight.
- `FlowWaterStrength`, `FlowReflectionStrength`, `FlowOpacity`,
  `FlowChromaStrength`, `FlowVertexStrength`, and `FlowWaveStrength` control
  the detected flowing-water pass separately from
  still water. The main current direction follows the game's authored
  `uParams.xy` UV scroll. Procedural flow streaks, foam, and refraction offsets
  use that same game-authored direction, but their animation
  clock comes from the proxy draw-frame counter so the replacement layer keeps
  moving even when the original flow matrix exposes only a tiny scroll value.
  `FlowVolumeWaveStrength` adds a broad non-geometric pressure wave along that
  current, and `FlowVolumeWaveScale` changes its wavelength. `FlowStreakFoam`
  adds thin directional foam streaks along that current.
- `DepthStrength` is inspired by OpenLara's water composition pass: height
  normals, Fresnel, and underwater color absorption.
- The depth extinction and procedural shore foam shaping also adapt ideas from
  tuxalin's MIT-licensed `water-shader` project, without requiring its external
  foam, normal, height, reflection, or sky textures at runtime.

`DebugMode` values:

- `0`: normal rendering;
- `1`: normals/waves;
- `2`: fresnel;
- `3`: SSR fade or foam, depending on the pass;
- `4`: screen-space water mask in the SSR pass;
- `5`: mirrored reflection source;
- `6`: raw framebuffer source;
- `7`: reflection blend mask;
- `8`: pass id colors: surface cyan, reflection yellow, SSR magenta, flow green;
- `9`: surface uses authored ripples red, shoreline green, volume blue; flow
  uses streak foam red, current strands green, waves blue;
- `10`: contact wave debug: height red, X slope green, Y slope blue;
- `11`: alpha coverage;
- `12`: surface base alpha red, depth green, material edge blue; flow base alpha
  red, depth green, wave blue;
- `13`: mesh/world texture mismatch red/green, generated edge or streak mask
  blue;
- `14`: draw-call id color. If every visible rectangle gets a different color,
  the seam is caused by draw-call/mesh chunk boundaries or transparent sorting;
- `15`: UV tile id and red tile borders. If the seam matches this, it is an
  authored UV/tile boundary;
- `16`: local vertex light/color;
- `17`: authored mesh texture sample;
- `18`: procedural/world texture sample;
- `19`: source alpha: mesh alpha red, world alpha green, final base alpha blue;
- `20`: final water color forced opaque, useful for separating color seams from
  alpha blending seams;
- `21`: seam guard mask. Debug modes disable the water-grid overlay so the base
  pass can be inspected cleanly.
- `22`: flow runtime inputs: red means the game supplied a flow vector, green
  shows its strength, and blue animates from the shader fallback travel clock.
- `23`: procedural detail mask and its contribution to wave/glint.

To switch diagnostic modes without hand-editing the INI:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\set_tr456_water_debug.ps1 -DebugMode 14 -ClearLog
```

Restart the game after changing `DebugMode`; the shaders are compiled at
startup. Press `Insert` in-game to record a draw/program snapshot to
`tr456_water\tr456_water_proxy.log`.

Runtime artifact isolation hotkeys use `Ctrl+J+1..9,0,-,=`:

- `Ctrl+J+1`: flow foam/streaks;
- `Ctrl+J+2`: flow chroma;
- `Ctrl+J+3`: reserved flow-caustics toggle, currently disabled;
- `Ctrl+J+4`: flow lanes/swirl;
- `Ctrl+J+5`: flow refraction warp;
- `Ctrl+J+6`: flow reflection;
- `Ctrl+J+7`: surface refraction warp;
- `Ctrl+J+8`: reserved surface-caustics toggle, currently disabled;
- `Ctrl+J+9`: surface foam/glint;
- `Ctrl+J+0`: surface reflection;
- `Ctrl+J+-`: water-grid displacement. The authored base water mesh is no
  longer moved by this toggle, because that path exposes remastered tile seams;
- `Ctrl+J+=`: game/contact ripples.

Each press toggles the effect immediately and writes the new state to
`tr456_water_proxy.log`.

## Uninstall

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\uninstall_tr456_water_proxy.ps1
```

This restores the previous `OpenGL32.dll` from an old backup if present, or
from `OpenGL32_orig.dll`, then removes the proxy support files.

## Notes

This project is now trimmed to the reversible OpenGL proxy path. The old
binary-patching and unrelated overlay experiments were removed from the
workspace.
