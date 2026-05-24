# TombRaiderNewShaders INI Settings Reference

This file documents every setting exposed in the current release
`tr456_water.ini` profile for build 1.2.36.

General rules:

- Boolean switches use `0` for disabled and `1` for enabled.
- Numeric strength values are usually scalar multipliers. Higher values make the
  effect stronger, but some settings are clamped by the runtime.
- Restart the game after editing the INI. The installer syncs the canonical
  `tr456_water.ini` into `tr456_water\tr456_water.ini`, so back up manual edits
  before reinstalling.
- Keep the release compatibility switches as documented unless you are doing a
  controlled diagnostic run.

## Core Runtime And Compatibility

| Setting | Release value | Description |
| --- | ---: | --- |
| `WaterShaderPatching` | `1` | Master switch for shader/program interception and water replacement. Set to `0` for original game water. |
| `ShaderPreload` | `0` | Controls startup shader-source preload. `0` keeps the release path lazy and avoids extra startup work. |
| `CompatMode` | `Auto` | Compatibility policy. `Auto` selects the safe runtime path for the active driver. Other internal modes are for diagnostics. |
| `CompatReport` | `1` | Writes a one-time compatibility report to the runtime log when logging is enabled. |
| `CompatGlErrorCheck` | `0` | Enables heavier GL error checks for troubleshooting synthetic water failures. Keep off for release play. |
| `CompatGlErrorWarmupDraws` | `0` | Draw count to ignore before compatibility GL error checks start. Used only with GL error checking. |
| `CompatMaxSyntheticErrors` | `4` | Number of repeated synthetic-water GL errors tolerated before failing safe to original water. |
| `ShaderBinaryCache` | `1` | Enables shader program binary caching when the active driver is known to be safe for it. |
| `EffectToggleMask` | `4095` | Bitmask for live support toggles. Release value keeps the `Ctrl+J` plus number-key toggles available. |
| `VulkanOnly` | `1` | Requires the Mesa/Zink path and disables silent fallback to the Windows system OpenGL runtime. |
| `MesaZinkChain` | `1` | Treats `OpenGL32_orig.dll` as the Mesa/Zink WGL chain target and applies the Zink environment policy. |
| `ReShadeChain` | `0` | Disables legacy OpenGL ReShade proxy chaining. Use ReShade's Vulkan layer instead. |
| `ReShadeDll` | `OpenGL32_reshade.dll` | Legacy OpenGL ReShade chain DLL name. Ignored on the supported Vulkan-only release path. |

## Framebuffer Capture

| Setting | Release value | Description |
| --- | ---: | --- |
| `FramebufferReflection` | `1` | Enables scene framebuffer capture used by synthetic refraction and reflection. |
| `ReflectionQuality` | `1` | Reflection quality tier. `0` disables reflection sampling, higher values allow stronger capture use. |
| `FramebufferCaptureInterval` | `1` | Captures every N frames. `1` is the most responsive release setting. |
| `FramebufferWarmupFrames` | `0` | Number of frames to wait before framebuffer capture becomes active. |
| `FramebufferScale` | `1` | Capture resolution scale divisor. `1` keeps full capture resolution. |
| `FramebufferErrorCheck` | `0` | Enables extra framebuffer error checks for diagnostics. Keep off for release play. |

## Synthetic Water Routing

| Setting | Release value | Description |
| --- | ---: | --- |
| `SyntheticWaterSurface` | `1` | Master switch for synthetic water surfaces. |
| `SyntheticStandingWaterOnly` | `1` | Limits the standing-water synthetic path to standing-water candidates. |
| `SyntheticStandingReplaceOriginal` | `0` | Experimental profile: keeps the original standing-water pass visible under the synthetic layer. |
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
| `SyntheticReflectOriginalMask` | `1` | Preserves the original water mask in the reflection path. |
| `SyntheticOverlayDepthMode` | `0` | Release depth mode for synthetic overlays. Non-zero values are experimental. |
| `SyntheticCompileDelayFrames` | `0` | Delays synthetic shader compilation by this many frames. |
| `SyntheticContactMaxSamples` | `192` | Maximum number of contact samples used for synthetic water interaction tracking. |
| `SyntheticSurfaceOpacity` | `0.70` | Global opacity multiplier for synthetic water. |
| `SyntheticSurfaceTint` | `0.12` | Global tint strength for synthetic water. |
| `SyntheticSurfaceReflection` | `1.48` | Global reflection multiplier for synthetic water. |

## Standing Water Look

| Setting | Release value | Description |
| --- | ---: | --- |
| `SurfaceRelief` | `2.72` | Overall standing-water surface relief and visible wave height response. |
| `SafeVolumeStrength` | `0.38` | Legacy profile key retained for compatibility with older tuning sets. The current runtime does not read it directly. |
| `PixelWaveStrength` | `1.70` | Legacy profile key retained for compatibility with older tuning sets. The current runtime does not read it directly. |
| `RefractionWaveStrength` | `2.40` | Strength of wave-driven refraction in standing water. |
| `RefractStrength` | `1.68` | Main standing-water refraction multiplier. |
| `ReflectStrength` | `1.34` | Main standing-water reflection multiplier. |
| `SSRStrength` | `0.66` | Legacy screen-space reflection tuning key retained for older profiles. The current runtime does not read it directly. |
| `ReflectionContrast` | `1.56` | Contrast applied to reflected standing-water detail. |
| `RoughReflection` | `0.62` | Legacy rough-reflection profile key retained for older tuning sets. The current runtime does not read it directly. |
| `MirrorRoughness` | `0.72` | Legacy mirror-roughness profile key retained for older tuning sets. The current runtime does not read it directly. |
| `CalmMirrorStrength` | `0.72` | Legacy calm-mirror profile key retained for older tuning sets. The current runtime does not read it directly. |
| `ReflectionShimmer` | `0.28` | Shimmer applied to standing-water reflection sampling. |
| `FresnelStrength` | `1.16` | View-angle reflection boost for standing water. |
| `Opacity` | `0.72` | Standing-water opacity baseline. |
| `TintStrength` | `0.46` | Standing-water tint strength. |
| `ChromaStrength` | `0.00` | Chromatic refraction strength. Release profile keeps it disabled. |
| `GlintStrength` | `0.16` | Standing-water glint strength. |
| `SurfaceSparkleStrength` | `0.58` | Fine highlight sparkle on standing water. |
| `FoamStrength` | `0.00` | Standing-water foam strength. Release profile keeps it disabled. |
| `SurfaceCausticStrength` | `0.22` | Subtle caustic-like surface light response. |
| `SurfaceBlueStripeStrength` | `0.00` | Experimental blue stripe coloration. Release profile keeps it disabled. |
| `StandingLifeStrength` | `0.82` | Broad standing-water movement/liveliness. |
| `StandingMicroChopStrength` | `0.96` | Small standing-water chop and micro variation. |
| `StandingMicroTrembleStrength` | `1.25` | Extra high-frequency standing-water micro tremble. Keep this moderate if the surface starts to shimmer too aggressively. |
| `StandingTrembleStrength` | `1.90` | High-frequency standing-water tremble. |
| `StandingBreathStrength` | `1.35` | Slow standing-water breathing motion. |
| `StandingTensionStrength` | `0.92` | Surface-tension style shaping for standing-water highlights and ridges. |
| `StandingDriftSpeed` | `0.74` | Drift speed for standing-water animated detail. |
| `StandingLayerYOffset` | `24.0` | Vertical layer offset used to keep standing-water overlays better aligned with the original surface. |
| `StandingOriginalBlend` | `0.00` | Blend from the independent synthetic standing-water body toward the unwarped captured/original standing color. Kept at `0.00` while the actual original draw layer is restored. |
| `StandingRefractionOriginalBlend` | `0.00` | Blend from synthetic-only refraction/lens distortion toward warped captured/original color. `0.00` keeps refraction detached from the original layer. |

## FlowLite Water Look

FlowLite affects flowing water. Waterfalls, splashes, and unknown flow materials
stay on the original game path unless they are classified as FlowLite surfaces.

| Setting | Release value | Description |
| --- | ---: | --- |
| `FlowWaterStrength` | `0.88` | Overall FlowLite water body strength. |
| `FlowLocationVariation` | `1.00` | Amount of procedural world-location variation for flow speed, detail, tint, and reflection. |
| `FlowReflectionStrength` | `1.86` | Main FlowLite reflection multiplier. Runtime clamp is `0.0` to `2.0`. |
| `FlowInPlaceRefractionBoost` | `1.42` | Refraction boost used by the older in-place flow patch path. Kept documented for diagnostics. |
| `FlowInPlaceReflectionBoost` | `2.62` | Reflection boost used by the older in-place flow patch path. Kept documented for diagnostics. |
| `FlowOpacity` | `0.20` | FlowLite opacity multiplier. Lower values make flowing water more transparent while retaining refraction. |
| `FlowChromaStrength` | `0.025` | Small FlowLite chromatic refraction split used to make screen-space bending easier to read. |
| `FlowStandingBlend` | `0.86` | Blend amount between flow shading and standing-water style compositing. |
| `FlowVertexStrength` | `0.00` | Flow vertex deformation strength. Release FlowLite keeps geometry deformation disabled. |
| `FlowSpeed` | `1.25` | Animation speed multiplier for FlowLite movement. |
| `FlowStreakFoam` | `0.045` | Thin foam/streak contribution in FlowLite. |
| `FlowSecondaryMotion` | `1.20` | Strength for secondary FlowLite drift, tremble, shimmer, and side-motion layers. |
| `FlowSecondaryOpacity` | `0.32` | Visibility/opacity support for secondary FlowLite refraction and detail layers. |
| `FlowSecondaryReflection` | `0.42` | Reflection support for secondary FlowLite streaks and shimmer layers. |
| `FlowGlintStrength` | `1.36` | FlowLite glint/highlight strength. |
| `FlowRefractionWarp` | `3.48` | Strength of FlowLite screen-space refraction warp. Runtime clamp is `0.0` to `3.5`. |
| `FlowSurfaceDistortion` | `3.48` | Strength of FlowLite surface shimmer/distortion. Runtime clamp is `0.0` to `3.5`. |
| `FlowBottomRefractionStrength` | `2.00` | Separate bottom/scene refraction strength for FlowLite. Higher values make the floor below flowing water bend more visibly. |
| `FlowBottomRefractionScale` | `2.05` | Scale of the bottom-refraction warp relative to the surface warp. |
| `FlowBottomVisibility` | `1.35` | Amount of refracted scene/floor color mixed into FlowLite. Higher values make the bottom read through the water more strongly. |
| `FlowFresnelReflection` | `1.28` | View-angle reflection balance for FlowLite. Higher values increase reflection at grazing angles while preserving bottom visibility from above. |
| `FlowSurfaceTension` | `1.16` | Surface-tension style shaping for flow ridges and highlights. |
| `FlowCrossDistortion` | `2.58` | Cross-flow distortion and side-motion strength. |
| `FlowOriginalDeformation` | `0.00` | Amount of original authored flow deformation to preserve. Release FlowLite disables it. |
| `FlowOriginalSync` | `1.00` | Synchronization amount with original authored flow timing. |
| `FlowBodyStrength` | `0.82` | FlowLite body/density contribution. |
| `FlowRidgeStrength` | `1.12` | Flow ridge and crest strength. |
| `FlowSpecularStreakStrength` | `0.52` | Long FlowLite specular streak strength. |
| `FlowBreakupStrength` | `0.92` | Breakup amount for flow lanes, crests, refraction breakup, and surface variation. |
| `FlowDetailStrength` | `1.48` | Fine FlowLite detail amount. Lower values reduce the scaly/repeating texture feel. |
| `TileSeamSoftening` | `0.52` | Softens visible flow texture tile seams. |
| `TileSeamWidth` | `0.044` | Width used when detecting and softening flow texture tile seams. |

## Contact, Ripple, And Wet Lara

| Setting | Release value | Description |
| --- | ---: | --- |
| `ContactMaxActive` | `6` | Maximum active contact ripples evaluated by the synthetic water shaders. |
| `ContactWaveStrength` | `1.55` | Legacy contact-wave tuning key retained for older profiles. The current runtime does not read it directly. |
| `ContactNormalStrength` | `1.62` | Legacy contact-normal tuning key retained for older profiles. The current runtime does not read it directly. |
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

| Setting | Release value | Description |
| --- | ---: | --- |
| `WaterSaturation` | `1.06` | Global water color saturation multiplier. |
| `WaterBrightness` | `0.86` | Global water brightness multiplier. |
| `WaterTextureStrength` | `1.34` | Strength of sampled water texture detail. |
| `BumpMappingStrength` | `0.42` | Base bump/normal strength for water. |
| `BumpMappingScale` | `0.92` | Scale of the bump/detail domain. |
| `FlowBumpMappingStrength` | `0.82` | Flow-specific bump/detail multiplier. Lower values reduce scaly texture response. |
| `SyntheticBumpMappingStrength` | `0.72` | Synthetic-water bump/detail multiplier. |

## Release Diagnostics

These settings should stay `0` in Nexus/release builds unless you are collecting
support data.

| Setting | Release value | Description |
| --- | ---: | --- |
| `SyntheticDebugSolid` | `0` | Renders synthetic water as a solid debug color when enabled. |
| `SwapDebugOverlay` | `0` | Shows the swap/debug overlay when enabled. |
| `WetLaraDebugVisible` | `0` | Forces visible wet-Lara debug output. |
| `WetLaraTraceLog` | `0` | Writes verbose wet-Lara trace logs. |
| `VerboseLog` | `0` | Enables verbose runtime logging. |
| `PerfTelemetry` | `0` | Enables periodic performance telemetry logging. |
| `DumpFlowShaderSource` | `0` | Dumps original flow shader source for diagnostics. |
