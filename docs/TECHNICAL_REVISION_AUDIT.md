# Technical Revision Audit

This audit is the first checkpoint for the larger refactor plan. It focuses on
facts that affect sequencing: what is safe to change now, what needs contracts
first, and which roadmap items do not apply to the current architecture.

## Scope

Reviewed areas:

- Proxy bootstrap, WGL/OpenGL chain loading, and exports.
- Shader tracking, runtime GLSL loading, and program compilation.
- Synthetic standing water, FlowLite, framebuffer capture, and draw dispatch.
- Wet Lara contact/ripple/overlay logic.
- Build, install, and Nexus packaging scripts.

Not reviewed yet:

- In-game visual correctness on multiple levels.
- Driver-specific runtime behavior on NVIDIA/AMD/Intel through Mesa/Zink.
- Long-run memory and performance telemetry from real gameplay.

## Key Findings

### 1. Direct Vulkan refactors are not applicable yet

The code does not create Vulkan devices, descriptor sets, command buffers, push
constants, or Vulkan memory allocations. The Vulkan layer is reached through
Mesa/Zink. Vulkan optimization should currently mean:

- Keep the OpenGL call stream Zink-friendly.
- Avoid fragile GL state assumptions.
- Reduce redundant GL queries and state churn.
- Keep shader binary cache policy conservative by driver/vendor.

Native Vulkan resource management only becomes relevant if the project adds a
separate renderer backend.

### 2. Build-time contracts are implicit

The project is organized into `.inc` modules inside one C translation unit. This
is workable, but dependencies are encoded by include order. A helper used in a
later `.inc` file must be declared or defined earlier, otherwise the whole DLL
fails to build. This already surfaced in the current branch around
`NativeWaterOverlay*` and `UnderwaterSurfaceGuard*` glue.

Recommended near-term action:

- Add small internal contract sections before each `.inc` include.
- Prefer narrow forward declarations over relying on accidental order.
- Move shared structs and feature state to small internal headers only after the
  dependencies are documented.

### 3. Release profile drift is now explicit

The branch now has two named profiles:

- `tr456_water.ini` is the active compatibility/development install profile.
- `profiles/tr456_water.release.ini` is the strict Nexus Vulkan/Zink package
  profile.

`tools/package_nexus_release.ps1` validates and packages the explicit release
profile, so local development values such as `VulkanOnly=0` no longer create
release package drift.

Recommended near-term action:

- Keep `tools/audit_ini_settings.ps1 -FailOnPackageDrift` in the release
  checklist.
- Treat future release-critical settings as package-profile changes first, then
  decide separately whether they belong in the active compatibility profile.

### 4. Draw dispatch has high duplication

`tr456_proxy_draw_dispatch.inc` repeats the same flow across many draw entry
points:

1. Note the draw.
2. Optionally suppress native overlay.
3. Prepare `SyntheticDrawDecision`.
4. Draw original pass with optional depth guard.
5. Draw wet Lara overlay.
6. Draw synthetic pass.
7. Restore GL state.

Recommended near-term action:

- Extract common decision/original-pass guard helpers first.
- Keep actual GL function signatures separate until coverage is stronger.
- Avoid a broad "one generic draw wrapper" change until runtime captures are
  available, because each GL draw family has slightly different arguments.

### 5. Runtime config is powerful but loosely typed

INI parsing is simple and robust enough for support use, but settings are read
in several places and documentation/package checks can drift from code defaults.

Recommended near-term action:

- Create a small settings inventory document or generated check.
- For each setting, track: INI key, default, clamp range, shader define mapping,
  release expected value, and owning subsystem.
- Before adding more options, centralize clamp/default logic per feature group.

### 6. Shader tuning is split across C defines and GLSL

External shaders are good for iteration, but many tuning values enter through
generated `#define`s. This is fast and simple, yet it makes shader/API contracts
hard to see.

Recommended near-term action:

- Document shader uniform/define contracts per shader file.
- Keep runtime-loaded shader files as the primary tuning surface.
- Avoid embedding new large shader strings in C unless they are diagnostics.

### 7. Portability is currently limited by design

The proxy uses Win32, WGL, `OpenGL32.dll` naming, SRW locks, and DLL export
forwarding. Linux/Proton support is not a small portability patch; it would need
a separate loader/interposition strategy.

Recommended near-term action:

- Treat Windows native as the supported platform.
- Abstract only where it reduces current risk, for example path/log/file helpers.
- Defer Linux support until the Windows architecture is stabilized.

### 8. Safety mode exists as an umbrella setting

`SafeMode=1` now maps to the existing lower-risk paths instead of adding a
second fallback system. It disables shader patching, synthetic/FBO water,
native overlay suppression, FlowLite, and Wet Lara overlay/contact helpers while
keeping the proxy chain loaded.

Recommended near-term action:

- Keep `SafeMode=0` validated in Nexus release packaging.
- Keep `tools/validate_tr456_runtime.ps1` in the support path so users can
  confirm which profile is active before launching the game.

## Refactor Roadmap

### Structure Phase Exit Criteria

This structural refactor phase is intentionally finite. It is complete when:

- The active compatibility profile, strict release profile, public INI docs, and
  settings ownership map pass `tools/audit_ini_settings.ps1
  -FailOnPackageDrift -FailOnUnclassified`.
- The proxy DLL builds through `tools/build_tr456_water_proxy.ps1`.
- The Nexus package builds through `tools/package_nexus_release.ps1`.
- `git diff --check` reports no whitespace errors.
- At least one low-risk physical module split is proven by the build. Current
  split: `tr456_proxy_shader_sources.c`.
- Remaining `.inc` modules have documented boundaries or are explicitly deferred
  because runtime risk is higher than the cleanup benefit.

Use `tools/check_structure_refactor.ps1` as the single local gate for this
phase. After it passes, stop structural cleanup unless a later feature or bugfix
needs a specific boundary change.

Out of scope for this phase:

- Native Vulkan backend work.
- Broad draw-dispatch rewrites.
- Async shader compilation or resource lifetime rewrites.
- Visual configurator UI.
- Linux/Proton loader support.

### Phase 0: Stabilize the branch

- Keep the current compile fix for missing overlay/underwater helpers.
- Add a repeatable `build.ps1` verification command to the revision checklist.
- Keep packaging validation green for `profiles/tr456_water.release.ini`.

### Phase 1: Document contracts

- Add module contract comments around the `.inc` include sequence.
- Document shader defines/uniforms for synthetic and FlowLite shaders.
- Add a settings inventory so INI, code defaults, docs, and package validation
  can be compared.

### Phase 2: Reduce duplication carefully

- Extract draw-dispatch helper functions with no behavior change.
- Group runtime config by subsystem.
- Introduce small strategy tables only where the code already behaves like a
  strategy, for example water pass selection or shader source loading.

### Phase 3: Separate modules when contracts are clear

- Move stable structs and prototypes into internal headers.
- First split completed: shader source factories now live in
  `tr456_proxy_shader_sources.c` with `tr456_proxy_shader_sources.h` as their
  narrow boundary around external GLSL entry points and embedded diagnostic
  shader text.
- Compile some modules as separate `.c` files only after hidden include-order
  dependencies are eliminated. Shader sources are the pilot; `configured_shader()`
  is now the small internal runtime-config boundary they call into.
- Keep public API surface tiny: this is a proxy DLL, not a general SDK yet.

### Phase 4: UX and support tooling

- Safe mode setting is implemented.
- Config validator script is implemented.
- Later, build a visual configurator on top of the documented settings schema.

## Progress Log

### 2026-05-26

- Added include-order contract comments in `src/tr456_water_proxy.c`, so each
  `.inc` include now states the dependency boundary it relies on.
- Added `tools/audit_ini_settings.ps1` and `docs/SETTINGS_INVENTORY.md` to keep
  INI, docs, code references, and release package expectations comparable.
- Started the draw-dispatch duplication pass with narrow
  `draw_original_elements_pass()` and `draw_original_arrays_pass()` helpers.
  The pilot keeps each GL draw signature explicit while centralizing depth guard
  and wet Lara replay behavior for the two base draw paths.
- Extended the same helper pass across range, base-vertex, instanced, multi-draw,
  and indirect draw paths. Multi/indirect paths keep separate helpers because
  their `skip_original` decision is not identical to the single-draw
  `SyntheticDrawDecision` flow.
- Split the monolithic runtime config load path into named groups:
  core/diagnostics, FlowLite FX, framebuffer capture, ripple/contact,
  synthetic feature gates, native overlay, underwater guard, and synthetic
  surface tuning. The settings still live in the same translation unit, but
  ownership boundaries are now visible before any future file split.
- Added `src/tr456_proxy_runtime_config.h` as the internal runtime-config
  contract. Bootstrap now depends on this contract instead of declaring INI
  readers locally.
- Added `docs/SHADER_DEFINE_OWNERSHIP.md` to document the define generation
  pipeline, external GLSL entry points, define groups, and the checklist for
  new shader-facing settings.
- Added `src/tr456_proxy_shader_sources.h` and moved synthetic standing-water
  source entry points into the shader source factory layer. Shader source
  loading now has one module-level entry surface for preload, program
  compilation, and diagnostics.
- Wired the previously stale active INI keys:
  `CompatAllowSystemFull`, `SyntheticCompileSync`,
  `WetLaraUnderwaterOverlay`, and `WetLaraUnderwaterOverlayGraceFrames`.
  The settings inventory now reports no active INI keys missing from code.
- Added `profiles/tr456_water.release.ini` and switched Nexus packaging and
  package-drift auditing to that explicit release profile. The active
  `tr456_water.ini` remains the compatibility/development install profile.
- Documented the profile split in README, `INI_SETTINGS.md`, and the settings
  inventory. Release package drift now reports none.
- Consolidated original-pass depth guard handling in
  `src/tr456_proxy_draw_dispatch.inc` behind `OriginalPassGuard`. Single,
  multi-draw, and indirect original-pass helpers now share the same begin/end
  guard path while keeping each GL draw signature explicit.
- Split shader source factories into `src/tr456_proxy_shader_sources.c` and
  compile that file beside `src/tr456_water_proxy.c`. This is the first real
  `.c` module split; draw dispatch, flow runtime, and most runtime config remain
  in the main translation unit.
- Added `docs/SETTINGS_OWNERSHIP.csv` and taught
  `tools/audit_ini_settings.ps1` to classify intentional internal code-only
  settings. The audit now reports zero unclassified code keys and can fail on
  future ownership drift with `-FailOnUnclassified`.
- Added `tools/check_structure_refactor.ps1` as the finite gate for the
  structure phase: ownership/package audit, DLL build, Nexus package, and
  whitespace check.
- Ran the structure refactor gate successfully.
- Added `SafeMode=1` as a documented emergency pass-through setting. Runtime
  config and compatibility policy disable shader patching, synthetic/FBO water,
  native overlay suppression, FlowLite, and Wet Lara overlay/contact helpers
  while leaving the proxy chain loaded. Nexus packaging validates `SafeMode=0`.
- Added `tools/validate_tr456_runtime.ps1` for support validation of an
  installed game directory or extracted Nexus package root. The script checks
  the proxy DLL, chain target, runtime INI, `SafeMode`, required GLSL files,
  stale shader experiments, and release-critical INI values. It is included in
  the Nexus package and runs inside `tools/check_structure_refactor.ps1`.

## Immediate Next Tasks

1. Treat the structure phase as closed while the gate stays green.
2. Stop broad structural cleanup and move to targeted runtime behavior work or
   real install validation on the game directory.
3. Re-open structural work only for a specific feature or bugfix boundary.
