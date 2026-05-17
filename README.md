# TR456 Water Proxy

OpenGL32 proxy for Tomb Raider I-III Remastered water rendering.

The proxy installs a local `OpenGL32.dll` next to `tomb123.exe` or
`tomb456.exe`, chains to the previous/system OpenGL runtime, tracks the game's
known water shader programs, and draws one synthetic water pass over the
original standing/flowing water layers. It no longer replaces the game's water,
ripple, environment, grid, geometry, caustic, or debug shaders.

## Current Scope

- standing water keeps the authored game layer and blends
  `tr456_water_synthetic.glsl` over it;
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
attribute layout before use.

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
ShaderBinaryCache=1
ReShadeChain=1
ReShadeDll=OpenGL32_reshade.dll
SyntheticWaterSurface=1
SyntheticStandingWaterOnly=0
SyntheticFlowSurface=1
SyntheticReflectSurface=1
FlowTextureFallback=1
FramebufferReflection=1
ChromaStrength=0.00
GlintStrength=0.00
FoamStrength=0.00
SurfaceCausticStrength=0.00
SurfaceBlueStripeStrength=0.00
WetLara=1
WetLaraUseSyntheticContact=1
WetLaraUnderwaterSustain=1
WetLaraUnderwaterMinJoints=8
WetLaraUnderwaterMargin=96.0
WetLaraPartialWet=1
WetLaraWetDelaySeconds=1.00
WetLaraWetRampSeconds=1.25
WetLaraOpacity=0.56
WetLaraSpecular=4.00
WetLaraDropletStrength=0.00
WetLaraStreakStrength=0.00
WetLaraClothDarkening=2.00
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
