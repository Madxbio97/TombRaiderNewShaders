# TR456 Water Proxy

OpenGL32 proxy for Tomb Raider I-III Remastered water rendering.

The proxy installs a local `OpenGL32.dll` next to `tomb123.exe` or
`tomb456.exe`, chains to the previous/system OpenGL runtime, tracks the game's
known water shader programs, and draws one synthetic water pass over the
original standing/flowing water layers. It no longer replaces the game's water,
ripple, environment, grid, geometry, caustic, or debug-picker shaders.

## Current Scope

- standing water keeps the authored game layer and blends
  `tr456_water_synthetic.glsl` over it;
- flowing water is enabled only by DDS texture signatures plus authored water
  shader parameters, then blended over the authored game layer;
- waterfalls, splashes, fire/fx sprites, seam/mix/overlay layers, and special
  non-water layers remain original;
- original ripple sprite draws are tracked only to feed contact/ripple data;
- logs are written only when `logs.txt` exists in the game root.

Installed support files:

```text
OpenGL32.dll
OpenGL32_orig.dll                  (optional chain target)
tr456_water\tr456_water_synthetic_vertex.glsl
tr456_water\tr456_water_synthetic.glsl
tr456_water\tr456_water.ini
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

## Runtime Tuning

Edit `tr456_water.ini` in the repo, reinstall, then restart the game.

Important defaults:

```ini
WaterShaderPatching=1
ShaderPreload=0
SyntheticWaterSurface=1
SyntheticStandingWaterOnly=0
SyntheticFlowSurface=1
FramebufferReflection=1
BumpMappingStrength=0.00
FlowBumpMappingStrength=0.00
SyntheticBumpMappingStrength=0.00
DebugMode=0
VerboseLog=0
```

For diagnostics, create `logs.txt` in the game root and use `Insert` in game.
Remove `logs.txt` again when you are done.
