# Tomb Raider New Shaders

Tomb Raider I-III Remastered did a strong job preserving the original games, but
some interactive effects still feel simpler than the environments around them.
Water, reflections, refraction, fire, rain, fog, and lighting all have room for
more depth and motion.

Tomb Raider New Shaders is an experimental visual mod that focuses on improving
those effects while keeping the original style and atmosphere intact. The
current alpha release is centered on water rendering: more dynamic surfaces,
stronger refraction, real-time reflections, depth response, and improved surface
motion.

## Main Features

- Improved water rendering for standing and flowing water.
- Dynamic, mobile water surfaces with more natural motion.
- Refraction and screen-space deformation effects.
- Real-time reflection support.
- Improved depth and underwater surface response.
- Standing water now has a fine micro-tremble layer and an experimental cleaner
  profile that returns the original standing-water draw layer while keeping the
  synthetic refraction/lens distortion detached from that original layer.
- A new synthetic rendering layer for water and water surfaces, replacing or
  augmenting the original game water where supported.
- FlowLite flowing water with softer texture response, stronger refraction, and
  location-based variation.
- Separate FlowLite bottom refraction and Fresnel-balanced reflection, so the
  floor is more visible from above while grazing angles remain reflective.
- FlowLite speed, secondary motion, breakup, bump/detail, chromatic refraction,
  and specular-streak settings are wired into the active shader path.
- Wet Lara/contact support for water interaction effects.

## Requirements

- Windows x64.
- Tomb Raider I-III Remastered PC release.
- Install into the game directory that contains `tomb123.exe` and `tomb456.exe`.

For version 1.2.2 and newer:

- A Vulkan-capable GPU with a current vendor driver.
- Target hardware: NVIDIA, AMD, or Intel through Mesa/Zink.
- Mesa/Zink WGL runtime installed as `OpenGL32_orig.dll` next to the game exe.
- The proxy is Vulkan-only by default and will not silently fall back to the
  system OpenGL runtime while `VulkanOnly=1`.
- Do not place an OpenGL ReShade proxy in front of this mod. Use ReShade through
  its Vulkan layer instead.

Not supported as a release target:

- Microsoft Basic Render Driver.
- Systems without a working Vulkan driver.
- Plain Windows `opengl32.dll` copied as `OpenGL32_orig.dll`.
- Proton, Wine, or Steam Deck unless a native `opengl32` DLL override is
  configured.

## Installation

1. Close Tomb Raider I-III Remastered.
2. Open the game directory that contains `tomb123.exe` and `tomb456.exe`.
3. Unpack the archive into that directory.
4. Make sure this mod's `OpenGL32.dll` is next to the game exe.
5. For version 1.2.2 and newer, make sure the Mesa/Zink WGL runtime is present
   as `OpenGL32_orig.dll`.
6. Launch the game.

## ReShade

### ReShade With Version 1.2.0 And Older

Older OpenGL-chain builds expect OpenGL ReShade behind this mod's proxy:

- This mod remains `OpenGL32.dll`.
- Put the ReShade OpenGL DLL next to it as `OpenGL32_reshade.dll`.
- `ReShadeChain=1` is enabled by default in those older builds.
- To disable ReShade, set `ReShadeChain=0` or remove `OpenGL32_reshade.dll`.

### ReShade With Version 1.2.2 And Newer

OpenGL ReShade proxy chaining is disabled for the Vulkan-only package.
`ReShadeChain=0` is the supported release setting.

Do not install ReShade as the OpenGL API for this game, and do not rename
`ReShade64.dll` to `OpenGL32.dll`.

Use ReShade through its Vulkan layer:

1. Keep this mod's `OpenGL32.dll` next to `tomb123.exe` / `tomb456.exe`.
2. Keep the Mesa/Zink WGL runtime as `OpenGL32_orig.dll` in the same directory.
3. Run the official ReShade setup tool and select `tomb123.exe` or
   `tomb456.exe`.
4. Select `Vulkan` when the setup tool asks for the rendering API, or enable
   ReShade's global Vulkan layer if the setup tool presents that option.
5. Install the desired ReShade effects or point the setup tool at an existing
   preset.
6. Launch the game. ReShade should appear after the Mesa/Zink path is active.

Expected runtime path for 1.2.2 and newer:

```text
Game -> water proxy -> Mesa/Zink -> Vulkan -> ReShade Vulkan layer
```

## AMD, Radeon, And EGS Notes

Leave these settings in `tr456_water.ini`:

```ini
CompatMode=Auto
ShaderBinaryCache=1
```

On AMD/Radeon and fragile Mesa/RADV-style paths, the runtime can bypass the
program binary cache automatically. The first run may take a little longer while
shaders compile, but this avoids crashes during binary shader recovery.

## Performance Notes

The shader effects increase system requirements somewhat, but the base game
requirements are generally low. Most modern Vulkan-capable GPUs should be able
to run the mod, but heavier water scenes can cost more than the original water.

## Known Issues

- Shader compilation can cause a small stutter in the main menu on first launch.
- Flowing water may still have minor performance issues in some scenes.
- Some effects are still experimental and will be improved in later releases.

## Support

If you enjoy the project and want to support development:

https://boosty.to/nikizhy
