# TR456 Water Proxy

OpenGL32 proxy for Tomb Raider I-III Remastered water rendering.

The proxy installs a local `OpenGL32.dll` next to `tomb123.exe` or
`tomb456.exe`, chains to the previous/system OpenGL runtime, tracks the game's
known water shader programs, and draws one synthetic water pass through the
tracked water geometry. Standing water can replace the old authored surface;
ripple, environment, grid, geometry, caustic, and debug shaders remain original.

## Compatibility Fix

Some Intel/Epic startup paths route `wglGetProcAddress` queries back through
the local `OpenGL32.dll` proxy. The proxy now resolves GL/WGL extension entry
points through the active ICD driver's `DrvGetProcAddress` path before falling
back to system WGL, and guards re-entrant WGL context teardown. This fixes early
launch failures where core startup queries such as `wglChoosePixelFormatARB`,
`wglDeleteContext`, or `glShaderSource` could recurse through the proxy or
return null extension pointers.

## Current Scope

- standing water can replace the authored game layer so the older blue surface
  does not wash out `tr456_water_synthetic.glsl`;
- flowing water uses DDS texture signatures first, then the optional
  `FlowTextureFallback` authored-parameter gate for repacked/retextured mods;
- waterfalls, splashes, fire/fx sprites, seam/mix/overlay layers, and special
  non-water layers remain original;
- original ripple sprite draws are tracked only to feed contact/ripple data;
- Wet Lara uses Lara's skinned joint uniforms against the synthetic water
  surface bounds, then dries back to the original material over time;
- logs are written only when `logs.txt` exists in the game root.

Installed support files:

```text
OpenGL32.dll
OpenGL32_reshade.dll               (optional ReShade chain target)
OpenGL32_orig.dll                  (optional chain target)
tr456_water\tr456_water_synthetic_vertex.glsl
tr456_water\tr456_water_synthetic.glsl
tr456_water\tr456_water.ini
tr456_water\shader_cache\*.bin       (local driver shader cache)
tr456_water\tr456_water_proxy.log  (only when logs.txt enables logging)
```

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -ForwardSource "$env:WINDIR\System32\opengl32.dll"
```

The build script uses Zig and reads the OpenGL export list from the forward
source DLL.

## Install

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1
```

The installer copies the proxy DLL, syncs `tr456_water.ini`, installs the two
synthetic shader files, and removes stale shader experiments from older builds.
`ShaderBinaryCache=1` keeps a local driver-specific OpenGL program binary in
`tr456_water\shader_cache` after the first successful synthetic shader link.
The cache is validated against the current GLSL text, driver strings, and
attribute layout before use. AMD/Radeon and Mesa/RADV-style OpenGL drivers
automatically bypass this cache and use the GLSL compile path, which is slower
only on first link but more reliable on those drivers.

`CompatMode=Auto` is the default runtime safety profile. It logs the active
OpenGL vendor/renderer/version, chooses driver-specific safeguards, bypasses
fragile program binaries on AMD/Mesa/RADV-style drivers, and disables only the
synthetic pass if repeated GL errors or shader compile/link failures are seen.
`CompatGlErrorWarmupDraws` keeps per-draw GL error checks only through the
initial successful synthetic draws, then retires them to avoid steady-state
`glGetError` stalls. Manual modes are available for support builds: `Full`
requests the full effect stack, `ShaderOnly` keeps tracking while disabling
synthetic passes, and `Vanilla` passes the game's original rendering through.
Hard shader failures still fail safe to original water.

## ReShade

OpenGL ReShade also wants to be named `OpenGL32.dll`, so this proxy must stay as
the first DLL and ReShade must be chained behind it. Put the ReShade OpenGL DLL
next to the game as `OpenGL32_reshade.dll`, or let the installer prepare it:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1 -ReShadeDll "C:\path\to\ReShade\OpenGL32.dll"
```

If ReShade is already installed as the game's `OpenGL32.dll`, run this
installer after ReShade; it will preserve that DLL as `OpenGL32_reshade.dll`
when it can identify it. `ReShadeChain=1` enables this lookup, and
`ReShadeChain=0` skips `OpenGL32_reshade.dll` while keeping the normal
`OpenGL32_orig.dll` fallback.

## Runtime Tuning

Edit `tr456_water.ini` in the repo, reinstall, then restart the game.

Important defaults:

```ini
WaterShaderPatching=1
ShaderPreload=0
CompatMode=Auto
CompatReport=1
CompatGlErrorCheck=1
CompatGlErrorWarmupDraws=180
CompatMaxSyntheticErrors=4
ShaderBinaryCache=1
ReShadeChain=1
ReShadeDll=OpenGL32_reshade.dll
SyntheticWaterSurface=1
SyntheticStandingWaterOnly=1
SyntheticStandingReplaceOriginal=1
SyntheticFlowSurface=1
SyntheticReflectSurface=1
FlowTextureFallback=1
FramebufferReflection=1
ChromaStrength=0.00
GlintStrength=0.16
FoamStrength=0.00
SurfaceCausticStrength=0.22
SurfaceBlueStripeStrength=0.06
SyntheticSurfaceOpacity=0.62
SyntheticSurfaceReflection=0.76
WaterUnderlayPattern=0
WaterUnderlayPatternFlow=0
WaterUnderlayPatternStrength=0.00
ReflectStrength=1.34
FlowReflectionStrength=0.42
FlowDetailStrength=0.90
FlowVertexStrength=0.10
FlowLaneStrength=0.18
FlowStreakFoam=0.035
FlowGlintStrength=0.18
SurfaceSparkleStrength=0.58
StandingLifeStrength=0.82
StandingMicroChopStrength=0.58
StandingTensionStrength=0.66
StandingDriftSpeed=0.74
LaraSplashStrength=1.15
WetLara=1
WetLaraUseSyntheticContact=1
WetLaraUnderwaterSustain=1
WetLaraUnderwaterCanStart=0
WetLaraUnderwaterMinJoints=8
WetLaraUnderwaterMargin=96.0
WetLaraSyntheticGraceFrames=30
WetLaraSyntheticVertical=192.0
WetLaraDryFrames=2400
WetLaraDrySeconds=20.0
WetLaraExitGraceFrames=45
WetLaraPartialWet=1
WetLaraWetDelaySeconds=1.00
WetLaraWetRampSeconds=1.25
WetLaraTraceLog=1
WetLaraTraceIntervalFrames=30
WetLaraOpacity=0.56
WetLaraSpecular=4.00
WetLaraDropletStrength=0.42
WetLaraStreakStrength=0.34
WetLaraClothDarkening=2.00
WetLaraContactRipples=1
WetLaraContactRippleRadius=185.0
WetLaraContactRippleStrength=1.18
WetLaraDripRipples=1
WetLaraDripIntervalFrames=18
WetLaraDripRippleRadius=86.0
WetLaraDripRippleStrength=0.38
WetLaraPartialRise=80.0
WetLaraPartialFade=90.0
WetLaraPartialDirection=-1.0
BumpMappingStrength=0.00
FlowBumpMappingStrength=0.00
SyntheticBumpMappingStrength=0.00
VerboseLog=0
```

For runtime logs, create `logs.txt` in the game root. Remove it again when you
are done.
