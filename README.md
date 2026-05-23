# TR456 Water Proxy

OpenGL32 proxy for Tomb Raider I-III Remastered water rendering.

This branch is the Vulkan/Zink release path:

```text
Game -> OpenGL32.dll water proxy -> OpenGL32_orig.dll Mesa/Zink -> Vulkan driver
```

The proxy keeps the game's original rendering pipeline, patches the known water
programs, and adds synthetic standing water, FlowLite flowing water, wet Lara,
contact ripples, refraction, reflection, bump/detail response, and shader-cache
compatibility fixes. It does not ship a custom Vulkan renderer.

## Release Profile

`tr456_water.ini` is intentionally short. Rare lab switches still exist as code
defaults, but the release file exposes only the settings expected for normal
play and support:

- `VulkanOnly=1` and `MesaZinkChain=1` require Mesa/Zink as `OpenGL32_orig.dll`.
- `FlowLiteSurface=1` and `SyntheticFlowReplaceOriginal=1` use the current fast
  synthetic flow path.
- `FlowInPlacePatch=0` keeps the older authored-flow patch path disabled.
- `SyntheticDebugSolid=0`, `SwapDebugOverlay=0`, `WetLaraDebugVisible=0`,
  `VerboseLog=0`, `PerfTelemetry=0`, and `DumpFlowShaderSource=0` are release
  defaults.
- `EffectToggleMask=4095` keeps the live flow toggles available for support:
  hold `Ctrl+J` and press `1`..`9`.

The installer syncs the canonical INI into `tr456_water\tr456_water.ini`, so
manual INI experiments should be backed up before reinstalling.

## Compatibility

The Zink compatibility fixes are part of the proxy, not external scripts:

- Forces Mesa to use Zink when the chain target is a Mesa WGL runtime:
  `GALLIUM_DRIVER=zink`, `MESA_LOADER_DRIVER_OVERRIDE=zink`, and
  `LIBGL_ALWAYS_SOFTWARE=0`.
- Disables system OpenGL fallback while `VulkanOnly=1`, so a missing or wrong
  `OpenGL32_orig.dll` fails visibly instead of silently changing backend.
- Resolves WGL/GL extension entry points through the active ICD path and guards
  re-entrant context teardown.
- Replays Zink-sensitive uniform and vertex attribute state for the synthetic
  water path.
- Keeps shader program binaries guarded by driver/vendor checks; fragile
  AMD/Mesa/RADV-style paths fall back to GLSL compilation.
- Fails safe to original water if repeated synthetic GL errors or shader
  compile/link failures are detected.

Known target vendors are NVIDIA, AMD, and Intel Vulkan drivers through
Mesa/Zink. Always verify the active provider before benchmarking:

```powershell
powershell -ExecutionPolicy Bypass -File ..\VulkanTR\tools\test_opengl_provider.ps1 -GameDir "G:\SteamLibrary\steamapps\common\Tomb Raider I-III Remastered" -RequireZink
```

Expected renderer text contains `GL_VENDOR=Mesa` and `GL_RENDERER=zink Vulkan`.

## ReShade

OpenGL ReShade proxy chaining is disabled for this Vulkan-only package:
`ReShadeChain=0` is the supported release setting. If ReShade is needed, use the
ReShade Vulkan layer outside the OpenGL proxy chain and disable it for clean
benchmark captures. The installer still avoids overwriting detected ReShade
wrappers, but the release path is:

```text
Game -> water proxy -> Mesa/Zink -> Vulkan
```

## Installed Files

```text
OpenGL32.dll                         water proxy
OpenGL32_orig.dll                    Mesa/Zink OpenGL chain target
tr456_water\tr456_water.ini          release profile
tr456_water\tr456_water_synthetic.glsl
tr456_water\tr456_water_synthetic_vertex.glsl
tr456_water\shader_cache\*.bin       optional local program cache
tr456_water\tr456_water_proxy.log    only when logs.txt enables logging
```

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build_tr456_water_proxy.ps1 -GameDir "G:\SteamLibrary\steamapps\common\Tomb Raider I-III Remastered"
```

The build script uses Zig and reads the OpenGL export list from the configured
forward DLL.

## Install

Close the game first, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_tr456_water_proxy.ps1 -GameDir "G:\SteamLibrary\steamapps\common\Tomb Raider I-III Remastered"
```

The installer copies the proxy DLL, syncs `tr456_water.ini`, installs the two
synthetic shader files, and removes stale shader experiments from older builds.

## Package

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\package_nexus_release.ps1
```

The packager verifies that release diagnostics are off, copies the built proxy
and support files into `dist\TombRaiderNewShaders-Nexus`, and creates a Nexus
ZIP with SHA-256 output.

## Logging And Support

Logs are off by default. To enable runtime logs for a support run, create
`logs.txt` in the game directory, reproduce the issue, then remove it again.

Use diagnostic settings only temporarily:

```ini
VerboseLog=0
PerfTelemetry=0
DumpFlowShaderSource=0
WetLaraTraceLog=0
```

## Rollback

Close the game and delete this package's `OpenGL32.dll`, `OpenGL32_orig.dll`,
Mesa/Zink runtime DLLs, and `tr456_water` folder from the game directory. If you
were chaining another wrapper manually, restore that wrapper as `OpenGL32.dll`.
