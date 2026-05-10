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
- replaces the water reflection shader with `tr456_water\tr456_water_reflect.glsl`;
- replaces the screen-space water/refraction pass with `tr456_water\tr456_water_ssr.glsl`;
- injects tuning defines from `tr456_water\tr456_water.ini` when each shader is compiled;
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
tr456_water\tr456_water_reflect.glsl
tr456_water\tr456_water_ssr.glsl
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
shaders\tr456_water_reflect.glsl
shaders\tr456_water_ssr.glsl
```

Then reinstall to copy them into the game support directory:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1
```

Restart the game after shader changes. The proxy reads each replacement shader
when the game compiles the original shader.

Runtime tuning lives in the support directory:

```text
tr456_water\tr456_water.ini
```

Useful values:

```ini
[Water]
DebugMode=0
SurfaceWave=1.15
RefractStrength=1.20
ReflectStrength=1.76
SSRStrength=1.28
GlintStrength=0.85
FoamStrength=0.75
ChromaStrength=0.55
TintStrength=0.74
CausticsStrength=0.95
DepthStrength=0.75
RippleStrength=0.85
RippleCenterX=0.50
RippleCenterY=0.62
RoughReflection=0.88
FresnelStrength=1.18
BottomCaustics=0.82
ContactEdge=0.72
DepthAbsorption=0.88
WallReflectionStretch=0.84
WaterSaturation=1.08
WaterBrightness=0.80
Opacity=0.60
ForceReflection=0.95
SceneReflectionStrength=1.00
ReflectionContrast=1.42
FramebufferReflection=1
```

Reflection notes:

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
- `BottomCaustics` adds thin moving caustic lines to the refracted floor layer.
- `ContactEdge` adds subtle darkening and wetline energy where water meets
  strong scene edges.
- `DepthAbsorption` makes darker/deeper-looking areas absorb more light.
- `WallReflectionStretch` vertically stretches reflected walls across corridor
  water.
- `WaterSaturation` and `WaterBrightness` tune the final color grade.
- `FramebufferReflection=1` enables the experimental framebuffer copy path used
  by `uTrWaterScene` in `tr456_water_ssr.glsl`.
- `RippleStrength` adds a screen-space wake around the configured center. It is
  a first approximation until we have Lara's real world position and velocity.
- `CausticsStrength` and `DepthStrength` are inspired by OpenLara's water
  composition pass: height normals, Fresnel, caustic line energy, and underwater
  color absorption.

`DebugMode` values:

- `0`: normal rendering;
- `1`: normals/waves;
- `2`: fresnel;
- `3`: SSR fade or foam, depending on the pass;
- `4`: screen-space water mask in the SSR pass;
- `5`: mirrored reflection source;
- `6`: raw framebuffer source;
- `7`: reflection blend mask;
- `8`: pass id colors: surface cyan, reflection yellow, SSR magenta.

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
