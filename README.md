# TR456 Water Proxy

Experimental OpenGL wrapper for improving water in Tomb Raider IV-VI Remastered.

The project installs a local `OpenGL32.dll` next to `tomb456.exe`. That DLL
chains to the game's previous OpenGL wrapper as `OpenGL32_orig.dll`, intercepts
`wglGetProcAddress("glShaderSource")`, detects known water shader sources,
and replaces them with the GLSL files copied to the `tr456_water` support
directory.

Current behavior:

- forwards the original OpenGL32 exports to `OpenGL32_orig.dll`;
- replaces the water surface shader with `tr456_water\tr456_water_surface.glsl`;
- replaces the water surface vertex shader with `tr456_water\tr456_water_surface_vertex.glsl`;
- replaces the water reflection shader with `tr456_water\tr456_water_reflect.glsl`;
- replaces the water reflection vertex shader with `tr456_water\tr456_water_reflect_vertex.glsl`;
- replaces the screen-space water/refraction pass with `tr456_water\tr456_water_ssr.glsl`;
- replaces the detected flowing-water shader with `tr456_water\tr456_water_flow.glsl`;
- replaces the detected flowing-water vertex shader with `tr456_water\tr456_water_flow_vertex.glsl`;
- replaces the detected authored ripple sprite shader with `tr456_water\tr456_water_ripple.glsl`;
- preloads and caches replacement shader sources, then injects tuning defines
  from `tr456_water\tr456_water.ini` when each shader is compiled;
- copies the current framebuffer into `uTrWaterScene` before the first tracked
  water draw and binds it to all replacement water shaders;
- writes runtime diagnostics to `tr456_water\tr456_water_proxy.log`;
- does not patch `tomb456.exe` for the normal proxy flow.

## Paths

Default game directory:

```powershell
G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered
```

Build output:

```powershell
build\OpenGL32.dll
```

Installed runtime files:

```text
OpenGL32.dll
OpenGL32_orig.dll
tr456_water\OpenGL32.dll.tr456-prev.bak
tr456_water\tr456_water_surface.glsl
tr456_water\tr456_water_surface_vertex.glsl
tr456_water\tr456_water_reflect.glsl
tr456_water\tr456_water_reflect_vertex.glsl
tr456_water\tr456_water_ssr.glsl
tr456_water\tr456_water_flow.glsl
tr456_water\tr456_water_flow_vertex.glsl
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
  -GameDir "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered" `
  -Zig "C:\zig\zig.exe"
```

## Install

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1
```

The installer backs up the previous `OpenGL32.dll`, prepares
`OpenGL32_orig.dll`, copies the proxy DLL, creates `tr456_water`, copies the
shader/config files there, and clears the old proxy log. It leaves
`tomb456.exe` untouched.

If you still have a `tomb456.exe.tr456-water.bak` from an older local
experiment and want to restore it during install, pass:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1 -RestoreExeBackup
```

## Verify

After launching the game once, check:

```powershell
Get-Content "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered\tr456_water\tr456_water_proxy.log"
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
shaders\tr456_water_reflect.glsl
shaders\tr456_water_reflect_vertex.glsl
shaders\tr456_water_ssr.glsl
shaders\tr456_water_flow.glsl
shaders\tr456_water_flow_vertex.glsl
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
ReflectionQuality=1
SurfaceWave=1.12
SurfaceVertexStrength=0.46
SurfaceVertexWaveStrength=1.05
PixelWaveStrength=1.62
RefractionWaveStrength=1.52
DeepCausticsStrength=0.86
WaterVolumeStrength=1.18
ShorelineStrength=0.78
GameRippleStrength=1.45
RefractStrength=1.18
ReflectStrength=1.48
SSRStrength=1.00
GlintStrength=0.56
FoamStrength=0.54
ChromaStrength=0.36
TintStrength=0.74
CausticsStrength=0.46
DepthStrength=0.84
RippleStrength=0.62
RippleCenterX=0.50
RippleCenterY=0.38
SurfaceRelief=1.08
WakeStrength=0.95
WakeWidth=0.58
WakeLength=0.84
ContactWaveStrength=1.35
ContactWaveRadius=1.00
ContactWaveSpeed=1.34
ContactVertexStrength=0.30
ContactNormalStrength=1.35
ContactCoordMode=1
PatchRipplePass=1
RippleSpriteMinCount=96
MicroRippleStrength=0.36
MicroRippleScale=0.72
MirrorRoughness=1.02
SwellStrength=0.72
SwellScale=0.70
WakeWaveStrength=0.88
EdgeWaveStrength=0.42
EdgeWaveWidth=0.085
RoughReflection=0.90
FresnelStrength=1.05
BottomCaustics=0.82
ContactEdge=0.72
DepthAbsorption=1.08
WallReflectionStretch=0.84
WaterSaturation=1.02
WaterBrightness=0.82
WaterTextureStrength=0.92
FlowWaterStrength=1.04
FlowReflectionStrength=0.68
FlowOpacity=0.82
FlowChromaStrength=0.28
FlowCausticsStrength=0.18
FlowVertexStrength=0.68
FlowWaveStrength=1.18
FlowSpeed=1.75
FlowStreakFoam=0.82
Opacity=0.58
ForceReflection=0.82
SceneReflectionStrength=0.92
ReflectionContrast=1.32
FramebufferReflection=1
DiagnosticDumpShaders=0
DiagnosticLogShaders=0
DiagnosticFrames=150
DiagnosticMaxLines=420
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
- `BottomCaustics` adds soft broken caustic light only to the refracted floor
  layer.
- `ContactEdge` adds subtle darkening and wetline energy where water meets
  strong scene edges.
- `DepthAbsorption` makes darker/deeper-looking areas absorb more light without
  adding a milky haze.
- `WallReflectionStretch` vertically stretches reflected walls across corridor
  water.
- `WaterSaturation` and `WaterBrightness` tune the final color grade.
- `WaterTextureStrength` increases visible water texture contrast and fine
  surface patterning without making the whole surface much brighter.
- `FramebufferReflection=1` enables the experimental framebuffer copy path used
  by `uTrWaterScene` in `tr456_water_ssr.glsl`.
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
- `DeepCausticsStrength` keeps visible caustic energy in the refracted
  depth/bottom layer instead of drawing a bright pattern on the water surface.
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
- `ContactCoordMode` chooses how contact centers are decoded: `1` uses world
  `x/z`, `2` uses `x/y`, and `0` auto-picks the closer interpretation.
- `PatchRipplePass` enables the experimental authored ripple sprite replacement.
  `RippleSpriteMinCount` gates it to larger ring draw calls so smaller sprites
  such as fire keep the original shader behavior.
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
  `FlowChromaStrength`, `FlowCausticsStrength`, `FlowVertexStrength`, and
  `FlowWaveStrength` control the detected flowing-water pass separately from
  still water. `FlowSpeed` scales the animated current without changing color
  opacity. `FlowStreakFoam` adds thin directional foam streaks along the
  current.
- `CausticsStrength` and `DepthStrength` are inspired by OpenLara's water
  composition pass: height normals, Fresnel, submerged caustic energy, and
  underwater color absorption.

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
- `10`: contact wave debug: height red, X slope green, Y slope blue.

## Uninstall

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\uninstall_tr456_water_proxy.ps1
```

This restores the previous `OpenGL32.dll` from
`tr456_water\OpenGL32.dll.tr456-prev.bak` and removes the proxy support files.

## Notes

This project is now trimmed to the reversible OpenGL proxy path. The old
binary-patching and unrelated overlay experiments were removed from the
workspace.
