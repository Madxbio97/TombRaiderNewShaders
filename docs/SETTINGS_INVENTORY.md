# Settings Inventory

This checkpoint keeps the runtime settings surface visible before deeper
refactoring. It compares the active compatibility INI profile, the strict Nexus
release profile, public INI documentation, settings ownership, code references,
and package expectations.

Run the audit from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\audit_ini_settings.ps1
powershell -ExecutionPolicy Bypass -File .\tools\audit_ini_settings.ps1 -Markdown
powershell -ExecutionPolicy Bypass -File .\tools\audit_ini_settings.ps1 -FailOnPackageDrift
powershell -ExecutionPolicy Bypass -File .\tools\audit_ini_settings.ps1 -FailOnPackageDrift -FailOnUnclassified
```

`-FailOnPackageDrift` is intended for release validation. It exits with an error
when `profiles/tr456_water.release.ini` does not match package expectations.
`-FailOnUnclassified` exits when a code-referenced setting is neither exposed in
a profile/documentation nor classified in `docs/SETTINGS_OWNERSHIP.csv`.

## Snapshot

Generated on 2026-05-26 from active `tr456_water.ini`,
`profiles/tr456_water.release.ini`, `INI_SETTINGS.md`,
`docs/SETTINGS_OWNERSHIP.csv`, `src`, and `tools/package_nexus_release.ps1`.

| Source | Count |
| --- | ---: |
| Active INI keys | 158 |
| Release profile keys | 160 |
| Documented keys | 160 |
| Code-referenced keys | 242 |
| Ownership-classified keys | 84 |
| Internal code-only keys | 82 |
| Release-only owned keys | 2 |
| Release package expectations | 17 |

Current active INI, release profile, `INI_SETTINGS.md`, and ownership map are
aligned for exposed settings: every active/release profile key is documented,
every documented key exists in at least one profile, every active key is read by
code, and every hidden code-only key has an explicit owner.

## Active INI Keys Not Found In Code

Current status: none.

The previously stale active keys were wired on 2026-05-26:

| Key | Current value | Notes |
| --- | --- | --- |
| `CompatAllowSystemFull` | `0` | `CompatMode=Auto` now falls back to shader-only behavior on non-Zink OpenGL unless this key is enabled. |
| `SyntheticCompileSync` | `1` | Synthetic standing-water compilation now completes in one ensure pass when enabled; staged compile remains available when disabled. |
| `WetLaraUnderwaterOverlay` | `0` | Wet Lara state can stay alive underwater while the visual overlay is suppressed. |
| `WetLaraUnderwaterOverlayGraceFrames` | `4` | Controls the frame grace for underwater overlay suppression. |

The audit script recognizes dynamic keys such as `WetLaraDrawCount0..3` through
their source pattern `WetLaraDrawCount%d`, so they are not included in this
stale-key list.

## Release Package Drift

Current status: none.

The active profile intentionally remains a compatibility/development install
profile with `VulkanOnly=0` and `SyntheticStandingReplaceOriginal=1`.
`profiles/tr456_water.release.ini` is the strict Nexus profile with
`VulkanOnly=1`, `SyntheticStandingReplaceOriginal=0`,
`StandingOriginalBlend=0.00`, and `StandingRefractionOriginalBlend=0.00`.
`tools/package_nexus_release.ps1` validates and packages that explicit release
profile.

## Code-Only Settings

Current status: none unclassified.

The audit still sees keys used by code defaults but absent from the active INI
and public INI docs. These now live in `docs/SETTINGS_OWNERSHIP.csv` instead of
being a noisy drift list. Main groups:

| Group | Examples |
| --- | --- |
| Legacy standing/volume water tuning | `DepthStrength`, `ShorelineStrength`, `WaterVolumeStrength`, `StandingMicroTrembleStrength` |
| FlowLite internal shaping | `FlowWaveStrength`, `FlowLaneStrength`, `FlowSingleLayer`, `FlowContactDistortion` |
| Wet Lara lab controls | `WetLaraUseJoints`, `WetLaraHoldFrames`, `WetLaraTintR`, `WetLaraWetRampSeconds` |
| Ripple/contact diagnostics | `RippleSpriteCenterMode`, `RippleSpriteMinCount`, `ContactWakeDirectional` |
| Release-only profile guards | `StandingOriginalBlend`, `StandingRefractionOriginalBlend` |

Before adding more options, either document and expose the key in the active or
release profile, or add it to `docs/SETTINGS_OWNERSHIP.csv` with an owner and
visibility.

## Recommended Cleanup Order

1. Decide which classified internal lab controls should become user-facing
   profile keys.
2. Keep `tools/audit_ini_settings.ps1 -FailOnPackageDrift -FailOnUnclassified`
   in the release checklist so profile and ownership drift stay closed.
3. Use the ownership map to drive the next runtime-config split by owner group.
