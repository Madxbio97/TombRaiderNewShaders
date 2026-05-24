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

## Source Layout

- `src/tr456_water_proxy.c` owns process/bootstrap state, shared GL typedefs,
  shader/program tracking, diagnostics, contacts, and geometry capture.
- `src/tr456_proxy_*.inc` modules hold the proxy subsystems that were split out
  of the original single-file implementation: bootstrap, runtime config, shadow
  state, flow runtime, program compilation, GL hooks, draw dispatch, and WGL
  exports.
- `shaders/*.glsl` contains runtime-loaded GLSL. Synthetic standing water and
  FlowLite flowing water now use the same external shader loading path, so
  visual tuning can happen in shader files instead of embedded C strings.
- `tools/*.ps1` contains the repeatable build, install, uninstall, and Nexus
  packaging entry points.

## Main Changes In 1.2.34

- Vulkan/Zink is the primary release path for NVIDIA, AMD, and Intel Vulkan
  drivers.
- FlowLite flowing water now varies subtly by world location, keeps the softer
  non-scaly texture pass, and uses stronger refraction, distortion, glints, and
  reflection.
- FlowLite vertex and fragment shaders now live in external GLSL files next to
  the synthetic water shaders, which makes future water tuning much easier and
  keeps the C proxy focused on routing/runtime work.
- FlowLite now has separate bottom/scene refraction controls, so the floor
  below flowing water bends more visibly without turning the surface into noise.
- FlowLite reflection now uses a view-angle Fresnel balance: more bottom
  refraction from above, stronger reflection at grazing angles.
- FlowLite bottom refraction is more visible through a stronger multi-sample
  floor lens pass.
- FlowLite now actually wires the release INI controls for speed, secondary
  motion, breakup, bump/detail response, chromatic refraction, and specular
  streaks into the active FlowLite shader instead of leaving them on the old
  path.
- FlowLite now amplifies the difference between the captured scene and the
  refracted scene, making bottom distortion easier to see while keeping the
  release water translucent.
- Added a FlowLite-only diagnostic mode and a low-noise draw probe so we can
  tell whether a specific flowing-water surface is using the synthetic layer.
- FlowLite refraction now compensates for final alpha blending, so the captured
  scene/bottom distortion remains visible in normal rendering instead of being
  blended back into the original frame.
- FlowLite lens compensation is now luminance-biased and clamped to avoid
  colorful/inverted refraction artifacts.
- FlowLite opacity now responds to `FlowOpacity`, so flowing water can stay more
  transparent without losing its refraction/reflective response.
- Standing water keeps the bounds guard, original-mask preservation, layer
  offset, calmer tremble, and breathing motion. The release profile now
  replaces the original standing layer to avoid visible layer separation.
- Release packaging now validates the Vulkan/Zink and ReShade-safe settings
  before creating the Nexus archive.

## Requirements

Hard requirements:

- Windows x64.
- Tomb Raider I-III Remastered PC release. Install into the directory that
  contains `tomb123.exe` and `tomb456.exe`.
- A Vulkan-capable GPU with a current vendor driver. The release target is
  NVIDIA, AMD, or Intel hardware through Mesa/Zink.
- Mesa/Zink WGL runtime installed as `OpenGL32_orig.dll` next to the game exe.
  The proxy is Vulkan-only by default and will not silently fall back to the
  system OpenGL runtime while `VulkanOnly=1`.
- No OpenGL ReShade proxy chain in front of the mod. Use ReShade's Vulkan layer
  instead if ReShade is needed.

Not supported as a release target:

- Microsoft Basic Render Driver or machines without a working Vulkan driver.
- Plain system `opengl32.dll` as `OpenGL32_orig.dll`.
- Proton/Wine/Steam Deck without a native `opengl32` DLL override.

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

OpenGL ReShade proxy chaining is disabled for this Vulkan-only package.
`ReShadeChain=0` is the supported release setting. Do not install ReShade as
the OpenGL API for this game, and do not rename `ReShade64.dll` to
`OpenGL32.dll`.

Use ReShade through its Vulkan layer:

1. Keep this mod's `OpenGL32.dll` next to `tomb123.exe` / `tomb456.exe`.
2. Keep the Mesa/Zink WGL runtime as `OpenGL32_orig.dll` in the same directory.
3. Run the official ReShade setup tool and select `tomb123.exe` or
   `tomb456.exe`.
4. Select `Vulkan` when the setup tool asks for the rendering API, or enable
   ReShade's global Vulkan layer if the setup tool presents that option.
5. Install the desired ReShade effect packages or point the setup tool at an
   existing preset.
6. Launch the game. ReShade should appear as a Vulkan layer after the
   Mesa/Zink path is active.

The supported runtime path is:

```text
Game -> water proxy -> Mesa/Zink -> Vulkan -> ReShade Vulkan layer
```

If ReShade does not appear, reinstall it as Vulkan, verify that the Vulkan
layer is enabled, and make sure `OpenGL32_orig.dll` is not a ReShade DLL or the
Windows system `opengl32.dll`. Disable ReShade for clean benchmark captures or
when collecting support diagnostics.

## Installed Files

```text
OpenGL32.dll                         water proxy
OpenGL32_orig.dll                    Mesa/Zink OpenGL chain target
INI_SETTINGS.md                      release INI tuning reference
tr456_water\tr456_water.ini          release profile
tr456_water\tr456_water_flow_lite.glsl
tr456_water\tr456_water_flow_lite_vertex.glsl
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

The installer copies the proxy DLL, syncs `tr456_water.ini`, installs the
current GLSL shader files, and removes stale shader experiments from older
builds.

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
