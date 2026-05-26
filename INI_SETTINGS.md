# TombRaiderNewShaders INI Settings Reference

This file documents every setting exposed in the active compatibility
`tr456_water.ini` profile for build 1.2.36. The strict Nexus profile lives in
`profiles/tr456_water.release.ini` and may override a small number of values
for package validation.

General rules:

- Boolean switches use `0` for disabled and `1` for enabled.
- Numeric strength values are usually scalar multipliers. Higher values make the
  effect stronger, but some settings are clamped by the runtime.
- Restart the game after editing the INI. The installer syncs the canonical
  `tr456_water.ini` into `tr456_water\tr456_water.ini`, so back up manual edits
  before reinstalling.
- Keep compatibility switches as documented unless you are doing a controlled
  diagnostic run.

## Core Runtime And Compatibility

| Setting | Profile value | Description |
| --- | ---: | --- |
| `SafeMode` | `0` | Emergency pass-through mode. Set `1` to disable shader patching, synthetic/FBO water, native overlay suppression, FlowLite, and Wet Lara overlay/contact helpers while keeping the proxy chain loaded. |
| `WaterShaderPatching` | `1` | Master switch for shader/program interception and water replacement. Set to `0` for original game water. |
| `ShaderPreload` | `0` | Controls startup shader-source preload. `0` keeps the release path lazy and avoids extra startup work. |
| `CompatMode` | `Auto` | Compatibility policy. `Auto` uses the full synthetic path on Mesa/Zink, falls back to a safer shader-only path on system OpenGL or unknown wrappers, and uses vanilla pass-through on Microsoft/very old OpenGL drivers. |
| `CompatReport` | `1` | Writes a one-time compatibility report to the runtime log when logging is enabled. |
| `CompatGlErrorCheck` | `0` | Enables heavier GL error checks for troubleshooting synthetic water failures. Keep off for release play. |
| `CompatGlErrorWarmupDraws` | `0` | Draw count to ignore before compatibility GL error checks start. Used only with GL error checking. |
| `CompatMaxSyntheticErrors` | `4` | Number of repeated synthetic-water GL errors tolerated before failing safe to original water. |
| `CompatAllowSystemFull` | `0` | Allows `Auto` to keep full synthetic/FBO water effects on non-Zink system OpenGL drivers. Release value keeps this off for broad compatibility. |
| `ShaderBinaryCache` | `1` | Enables shader program binary caching when the active driver is known to be safe for it. |
| `EffectToggleMask` | `4095` | Bitmask for live support toggles. Release value keeps the `Ctrl+J` plus number-key toggles available. |
| `VulkanOnly` | `0` | Allows fallback to the Windows system OpenGL runtime when `OpenGL32_orig.dll` is missing or rejected. Set `1` for strict Mesa/Zink diagnostics. |
| `MesaZinkChain` | `1` | Treats a Mesa/Zink `OpenGL32_orig.dll` as the preferred full-effects WGL chain target and applies the Zink environment policy. |
| `ReShadeChain` | `0` | Disables legacy OpenGL ReShade proxy chaining. Use ReShade's Vulkan layer instead. |
| `ReShadeDll` | `OpenGL32_reshade.dll` | Legacy OpenGL ReShade chain DLL name. Kept for older/test setups; release builds prefer the Vulkan layer. |

## Framebuffer Capture

| Setting | Profile value | Description |
| --- | ---: | --- |
| `FramebufferReflection` | `1` | Enables scene framebuffer capture used by synthetic refraction and reflection. |
| `ReflectionQuality` | `1` | Reflection quality tier. `0` disables reflection sampling, higher values allow stronger capture use. |
| `FramebufferCaptureInterval` | `1` | Captures every N frames. `1` is the most responsive release setting. |
| `FramebufferWarmupFrames` | `0` | Number of frames to wait before framebuffer capture becomes active. |
| `FramebufferScale` | `1` | Capture resolution scale divisor. `1` keeps full capture resolution. |
| `FramebufferErrorCheck` | `0` | Enables extra framebuffer error checks for diagnostics. Keep off for release play. |

## Synthetic Water Routing

| Setting | Profile value | Description |
| --- | ---: | --- |
| `SyntheticWaterSurface` | `1` | Master switch for synthetic water surfaces. |
| `SyntheticStandingWaterOnly` | `1` | Limits the standing-water synthetic path to standing-water candidates. |
| `SyntheticStandingReplaceOriginal` | `1` | Skips the original standing-water pass and lets the synthetic standing-water layer fully own the final composite. |
| `SyntheticStandingBoundsGuard` | `1` | Keeps synthetic standing water clipped to the original draw bounds to prevent spillover. |
| `SyntheticStandingPreserveMask` | `1` | Preserves the original standing-water mask when compositing the synthetic layer. |
| `SyntheticFlowSurface` | `1` | Enables synthetic processing for flowing-water candidates. |
| `SyntheticFlowReplaceOriginal` | `1` | Replaces the original flowing-water pass when FlowLite is ready. |
| `FlowLiteSurface` | `1` | Uses the current fast FlowLite shader for flowing water. |
| `FlowLiteDebugMode` | `0` | FlowLite-only diagnostic overlay. `0` is off, `1` paints FlowLite surfaces cyan, `2` shows refraction/capture delta heat. Keep `0` for normal play. |
| `FlowInPlacePatch` | `0` | Disables the older in-place original flow shader patch path. |
| `FlowFastPath` | `1` | Enables the release fast path for synthetic flow. |
| `FlowTextureFallback` | `1` | Allows material/parameter fallback classification when precise flow texture signatures are unavailable. |
| `SyntheticReflectSurface` | `1` | Enables synthetic reflection support for water surfaces. |
| `NativeWaterOverlaySuppress` | `1` | Suppresses small leftover native blended water overlays after a synthetic/FlowLite water draw on the same water plane. |
| `NativeWaterOverlayMaxCount` | `384` | Maximum draw count eligible for native water overlay suppression. Keeps large level geometry and Lara draws untouched. |
| `NativeWaterOverlayYMargin` | `96` | World-space Y tolerance used to match a leftover native overlay to the most recent synthetic water plane. |
| `UnderwaterSurfaceGuard` | `0` | Leaves standing-water refraction and reflection active even when Lara's underwater detector recently fired. |
| `UnderwaterSurfaceGuardGraceFrames` | `30` | Frame grace for the underwater surface guard so the water layer does not flicker during intermittent detection. |
| `UnderwaterSurfaceRefractionScale` | `1.00` | Neutral refraction scale used if the underwater guard is re-enabled. |
| `UnderwaterSurfaceReflectionScale` | `1.00` | Neutral reflection scale used if the underwater guard is re-enabled. |
| `UnderwaterSurfaceOpacityScale` | `1.00` | Neutral opacity scale used if the underwater guard is re-enabled. |
| `SyntheticReflectOriginalMask` | `0` | Disables the reflection-path original-water mask because the original standing-water layer is no longer drawn. |
| `SyntheticOverlayDepthMode` | `0` | Release depth mode for synthetic overlays. Non-zero values are experimental. |
| `SyntheticCompileSync` | `1` | Compiles the synthetic standing-water program in one pass when first needed, preventing the original standing layer from flashing during staged compile warmup. |
| `SyntheticCompileDelayFrames` | `0` | Delays synthetic shader compilation by this many frames. |
| `SyntheticContactMaxSamples` | `192` | Maximum number of contact samples used for synthetic water interaction tracking. |
| `SyntheticSurfaceOpacity` | `0.66` | Global opacity multiplier for synthetic water. |
| `SyntheticSurfaceTint` | `0.12` | Global tint strength for synthetic water. |
| `SyntheticSurfaceReflection` | `1.48` | Global reflection multiplier for synthetic water. |

## Standing Water Look

| Setting | Profile value | Description |
| --- | ---: | --- |
| `SurfaceRelief` | `2.96` | Overall standing-water surface relief and visible wave height response. |
| `RefractionWaveStrength` | `2.54` | Strength of wave-driven refraction in standing water. |
| `RefractStrength` | `1.74` | Main standing-water refraction multiplier. |
| `ReflectStrength` | `1.34` | Main standing-water reflection multiplier. |
| `ReflectionContrast` | `1.56` | Contrast applied to reflected standing-water detail. |
| `ReflectionShimmer` | `0.34` | Shimmer applied to standing-water reflection sampling. |
| `FresnelStrength` | `1.16` | View-angle reflection boost for standing water. |
| `ChromaStrength` | `0.00` | Chromatic refraction strength. Release profile keeps it disabled. |
| `GlintStrength` | `0.24` | Standing-water glint strength. |
| `SurfaceSparkleStrength` | `0.72` | Fine highlight sparkle on standing water. |
| `FoamStrength` | `0.00` | Standing-water foam strength. Release profile keeps it disabled. |
| `SurfaceCausticStrength` | `0.30` | Subtle caustic-like surface light response. |
| `SurfaceBlueStripeStrength` | `0.00` | Experimental blue stripe coloration. Release profile keeps it disabled. |
| `StandingLifeStrength` | `0.88` | Broad standing-water movement/liveliness. |
| `StandingMicroChopStrength` | `1.18` | Small standing-water chop and micro variation. |
| `StandingTrembleStrength` | `1.72` | High-frequency standing-water tremble. |
| `StandingBreathStrength` | `1.10` | Slow standing-water breathing motion. |
| `StandingTensionStrength` | `0.96` | Surface-tension style shaping for standing-water highlights and ridges. |
| `StandingDriftSpeed` | `1.02` | Drift speed for standing-water animated detail. |
| `StandingLayerYOffset` | `24.0` | Vertical layer offset used to keep standing-water overlays better aligned with the original surface. |
| `StandingOriginalBlend` | `0.00` | Strict Nexus profile guard that prevents the restored original standing-water layer from being blended into synthetic refraction color. |
| `StandingRefractionOriginalBlend` | `0.00` | Strict Nexus profile guard that keeps synthetic standing-water refraction driven by our waves, underlay, and micro tremble instead of warped original color. |

## FlowLite Water Look

FlowLite affects flowing water. Waterfalls, splashes, and unknown flow materials
stay on the original game path unless they are classified as FlowLite surfaces.

| Setting | Profile value | Description |
| --- | ---: | --- |
| `FlowWaterStrength` | `1.08` | Overall FlowLite water body strength. |
| `FlowLocationVariation` | `1.00` | Amount of procedural world-location variation for flow speed, detail, tint, and reflection. |
| `FlowReflectionStrength` | `1.42` | Main FlowLite reflection multiplier. Runtime clamp is `0.0` to `2.0`. |
| `FlowInPlaceRefractionBoost` | `1.42` | Refraction boost used by the older in-place flow patch path. Kept documented for diagnostics. |
| `FlowInPlaceReflectionBoost` | `2.62` | Reflection boost used by the older in-place flow patch path. Kept documented for diagnostics. |
| `FlowOpacity` | `0.24` | FlowLite opacity multiplier. Lower values make flowing water more transparent while retaining refraction. |
| `FlowChromaStrength` | `0.025` | Small FlowLite chromatic refraction split used to make screen-space bending easier to read. |
| `FlowStandingBlend` | `0.86` | Blend amount between flow shading and standing-water style compositing. |
| `FlowVertexStrength` | `0.16` | Flow vertex deformation strength. Adds a synced up/down flow bob in FlowLite. |
| `FlowSpeed` | `1.10` | Animation speed multiplier for FlowLite movement. Lower release value gives the flow a slightly heavier, more viscous feel. |
| `FlowStreakFoam` | `0.045` | Thin foam/streak contribution in FlowLite. |
| `FlowSecondaryMotion` | `0.96` | Strength for secondary FlowLite drift, tremble, shimmer, and side-motion layers. |
| `FlowSecondaryOpacity` | `0.32` | Visibility/opacity support for secondary FlowLite refraction and detail layers. |
| `FlowSecondaryReflection` | `0.42` | Reflection support for secondary FlowLite streaks and shimmer layers. |
| `FlowGlintStrength` | `1.20` | FlowLite glint/highlight strength. |
| `FlowRefractionWarp` | `1.60` | Strength of FlowLite screen-space refraction warp. Runtime clamp is `0.0` to `3.5`. |
| `FlowSurfaceDistortion` | `1.72` | Strength of FlowLite surface shimmer/distortion. Runtime clamp is `0.0` to `3.5`. |
| `FlowBottomRefractionStrength` | `0.72` | Separate bottom/scene refraction strength for FlowLite. Higher values make the floor below flowing water bend more visibly. |
| `FlowBottomRefractionScale` | `1.00` | Scale of the bottom-refraction warp relative to the surface warp. |
| `FlowBottomVisibility` | `0.68` | Amount of refracted scene/floor color mixed into FlowLite. Higher values make the bottom read through the water more strongly. |
| `FlowFresnelReflection` | `0.96` | View-angle reflection balance for FlowLite. Higher values increase reflection at grazing angles while preserving bottom visibility from above. |
| `FlowSurfaceTension` | `1.16` | Surface-tension style shaping for flow ridges and highlights. |
| `FlowCrossDistortion` | `2.58` | Cross-flow distortion and side-motion strength. |
| `FlowDirectionSign` | `-1.00` | Direction sign shared by synthetic flow and FlowLite. Release value flips the procedural flow to match the game-authored visual flow direction. |
| `FlowOriginalDeformation` | `0.00` | Amount of original authored flow deformation to preserve. Release FlowLite disables it. |
| `FlowOriginalSync` | `1.00` | Synchronization amount with original authored flow timing. |
| `FlowBodyStrength` | `1.08` | FlowLite body/density contribution. |
| `FlowRidgeStrength` | `1.08` | Flow ridge and crest strength. |
| `FlowSpecularStreakStrength` | `0.38` | Long FlowLite specular streak strength. |
| `FlowBreakupStrength` | `0.86` | Breakup amount for flow lanes, crests, refraction breakup, and surface variation. |
| `FlowDetailStrength` | `0.98` | Fine FlowLite detail amount. Lower values reduce the scaly/repeating texture feel. |
| `TileSeamSoftening` | `0.52` | Softens visible flow texture tile seams. |
| `TileSeamWidth` | `0.044` | Width used when detecting and softening flow texture tile seams. |

## Contact, Ripple, And Wet Lara

| Setting | Profile value | Description |
| --- | ---: | --- |
| `ContactMaxActive` | `6` | Maximum active contact ripples evaluated by the synthetic water shaders. |
| `ContactRippleDecay` | `0.86` | Decay speed/shape for contact ripples. |
| `LaraSplashStrength` | `1.24` | Splash contribution from Lara-related water interaction. |
| `FlowContactStrength` | `1.34` | Flow-specific contact response strength. |
| `FlowContactNormalStrength` | `1.22` | Flow-specific contact normal/distortion strength. |
| `FlowContactRipples` | `0` | Enables additional FlowLite contact ripples. Release value keeps them disabled. |
| `WetLara` | `1` | Enables the wet-Lara rendering helper. |
| `WetLaraContactOnly` | `0` | If enabled, wetness only starts from confirmed water contacts. Release allows broader synthetic/contact detection. |
| `WetLaraUseTimingDraw` | `0` | Uses timing draw detection as a wetness trigger. Release profile keeps it disabled. |
| `WetLaraUseWaterContact` | `1` | Allows direct water-contact detection to drive wetness. |
| `WetLaraUseRippleCircle` | `1` | Allows ripple-circle evidence to help detect water contact. |
| `WetLaraUseSyntheticContact` | `1` | Allows synthetic water contact tracking to drive wetness. |
| `WetLaraRequireJoints` | `0` | If enabled, wetness requires enough Lara joint/contact samples. Release keeps this permissive. |
| `WetLaraRenderOverlay` | `1` | Enables the wetness overlay rendering pass on Lara. |
| `WetLaraUnderwaterOverlay` | `0` | Keeps wetness/contact state alive underwater but skips the visual overlay while Lara is fully submerged, avoiding underwater color-state artifacts. |
| `WetLaraUnderwaterOverlayGraceFrames` | `4` | Small frame grace used to suppress the overlay across intermittent underwater-detection gaps. |
| `WetLaraDrawCount0` | `42735` | Lara draw-count signature slot 0 used by the wet-Lara detector. |
| `WetLaraDrawCount1` | `11016` | Lara draw-count signature slot 1 used by the wet-Lara detector. |
| `WetLaraDrawCount2` | `22140` | Lara draw-count signature slot 2 used by the wet-Lara detector. |
| `WetLaraDrawCount3` | `11904` | Lara draw-count signature slot 3 used by the wet-Lara detector. |
| `WetLaraDryFrames` | `2400` | Maximum dry-out duration in frames. |
| `WetLaraDrySeconds` | `20.0` | Dry-out duration in seconds. |
| `WetLaraExitGraceFrames` | `45` | Grace period after leaving water before wetness begins fading aggressively. |
| `WetLaraWaterMinJoints` | `1` | Minimum joint/contact count for water-contact wetness evidence. |
| `WetLaraSyntheticGraceFrames` | `30` | Grace period for synthetic-water contact evidence. |
| `WetLaraSyntheticMinJoints` | `1` | Minimum joint/contact count for synthetic wetness evidence. |
| `WetLaraSyntheticSurfaceAgeFrames` | `45` | Maximum age of synthetic surface evidence used for wetness. |
| `WetLaraSyntheticMargin` | `192.0` | Horizontal/planar margin used when matching Lara to synthetic water surfaces. |
| `WetLaraSyntheticVertical` | `520.0` | Vertical range used when matching Lara to synthetic water surfaces. |
| `WetLaraSyntheticAboveSurface` | `64.0` | Allowed height above the synthetic surface for wetness/contact matching. |
| `WetLaraOpacity` | `0.38` | Strength of the wetness overlay on Lara. |
| `WetLaraSpecular` | `1.45` | Specular shine strength for wet Lara. |
| `WetLaraContactRipples` | `1` | Enables ripples triggered by Lara contact. |
| `WetLaraContactRippleRadius` | `240.0` | Radius for Lara contact ripples. |
| `WetLaraContactRippleStrength` | `1.46` | Strength for Lara contact ripples. |
| `WetLaraDripRipples` | `1` | Enables drip ripples while Lara is wet. |
| `WetLaraPartialWet` | `1` | Enables partial-body wetness instead of only full-body wetness. |
| `WetLaraRadiusScale` | `8.00` | Horizontal radius multiplier for wet-Lara contact matching. |
| `WetLaraVerticalRadius` | `4096.0` | Vertical radius for wet-Lara contact matching. |
| `WetLaraWaterContactRadius` | `2.50` | Radius used by the water-contact detector. |
| `WetLaraWaterContactVertical` | `520.0` | Vertical range used by the water-contact detector. |

## Shared Fine Detail

| Setting | Profile value | Description |
| --- | ---: | --- |
| `WaterSaturation` | `1.06` | Global water color saturation multiplier. |
| `WaterBrightness` | `0.86` | Global water brightness multiplier. |
| `WaterTextureStrength` | `1.42` | Strength of sampled water texture detail. |
| `BumpMappingStrength` | `0.28` | Base bump/normal strength for water. |
| `BumpMappingScale` | `0.82` | Scale of the bump/detail domain. |
| `FlowBumpMappingStrength` | `0.66` | Flow-specific bump/detail multiplier. Lower values reduce scaly texture response. |
| `SyntheticBumpMappingStrength` | `0.58` | Synthetic-water bump/detail multiplier. |

## Release Diagnostics

These settings should stay `0` in Nexus/release builds unless you are collecting
support data.

| Setting | Profile value | Description |
| --- | ---: | --- |
| `SyntheticDebugSolid` | `0` | Renders synthetic water as a solid debug color when enabled. |
| `SwapDebugOverlay` | `0` | Shows the swap/debug overlay when enabled. |
| `WetLaraDebugVisible` | `0` | Forces visible wet-Lara debug output. |
| `WetLaraTraceLog` | `0` | Writes verbose wet-Lara trace logs. |
| `VerboseLog` | `0` | Enables verbose runtime logging. |
| `PerfTelemetry` | `0` | Enables periodic performance telemetry logging. |
| `DumpFlowShaderSource` | `0` | Dumps original flow shader source for diagnostics. |
