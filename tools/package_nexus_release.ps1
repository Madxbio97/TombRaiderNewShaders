param(
  [string]$Version = "1.2.36",
  [string]$OutDir = "dist",
  [string]$ReleaseIni = "profiles\tr456_water.release.ini"
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot

function Resolve-RepoPath($Path) {
  if ([IO.Path]::IsPathRooted($Path)) {
    return $Path
  }
  return Join-Path $root $Path
}

$outRoot = Join-Path $root $OutDir
New-Item -ItemType Directory -Force -Path $outRoot | Out-Null

$resolvedRoot = (Resolve-Path -LiteralPath $root).Path
$resolvedOut = (Resolve-Path -LiteralPath $outRoot).Path
if (-not $resolvedOut.StartsWith($resolvedRoot, [StringComparison]::OrdinalIgnoreCase)) {
  throw "Output directory escaped repository root: $resolvedOut"
}

$packageName = "TombRaiderNewShaders-Nexus"
$packageDir = Join-Path $outRoot $packageName
$zipPath = Join-Path $outRoot "$packageName-$Version.zip"

if (Test-Path $packageDir) {
  $resolvedPackage = (Resolve-Path -LiteralPath $packageDir).Path
  if (-not $resolvedPackage.StartsWith($resolvedOut, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Package directory escaped output root: $resolvedPackage"
  }
  Remove-Item -LiteralPath $packageDir -Recurse -Force
}

$waterDir = Join-Path $packageDir "tr456_water"
New-Item -ItemType Directory -Force -Path $waterDir | Out-Null

$dll = Join-Path $root "build\OpenGL32.dll"
$readme = Join-Path $root "README.md"
$iniReference = Join-Path $root "INI_SETTINGS.md"
$ini = Resolve-RepoPath $ReleaseIni
$shaderDir = Join-Path $root "shaders"

foreach ($required in @($dll, $readme, $iniReference, $ini, $shaderDir)) {
  if (-not (Test-Path $required)) {
    throw "Required release input not found: $required"
  }
}

$iniText = Get-Content -LiteralPath $ini -Raw
foreach ($releaseOffKey in @(
    "SafeMode",
    "VerboseLog",
    "WetLaraTraceLog",
    "WetLaraDebugVisible",
    "SyntheticDebugSolid",
    "FlowLiteDebugMode",
    "SwapDebugOverlay",
    "PerfTelemetry",
    "DumpFlowShaderSource")) {
  if ($iniText -notmatch "(?m)^\s*$releaseOffKey\s*=\s*0\s*$") {
    throw "Release INI must keep $releaseOffKey=0 for the Nexus package"
  }
}

foreach ($releaseKey in @(
    @{ Name = "CompatMode"; Value = "Auto" },
    @{ Name = "VulkanOnly"; Value = "1" },
    @{ Name = "MesaZinkChain"; Value = "1" },
    @{ Name = "ReShadeChain"; Value = "0" },
    @{ Name = "SyntheticStandingReplaceOriginal"; Value = "0" },
    @{ Name = "StandingOriginalBlend"; Value = "0.00" },
    @{ Name = "StandingRefractionOriginalBlend"; Value = "0.00" },
    @{ Name = "FlowLiteSurface"; Value = "1" },
    @{ Name = "SyntheticFlowReplaceOriginal"; Value = "1" })) {
  if ($iniText -notmatch "(?m)^\s*$($releaseKey.Name)\s*=\s*$($releaseKey.Value)\s*$") {
    throw "Release INI must keep $($releaseKey.Name)=$($releaseKey.Value) for the Nexus package"
  }
}

$shaderFiles = @(Get-ChildItem -LiteralPath $shaderDir -Filter "*.glsl" -File)
if ($shaderFiles.Count -lt 1) {
  throw "No GLSL shaders found in $shaderDir"
}
foreach ($requiredShader in @(
    "tr456_water_synthetic_vertex.glsl",
    "tr456_water_synthetic.glsl",
    "tr456_water_flow_lite_vertex.glsl",
    "tr456_water_flow_lite.glsl")) {
  if (-not ($shaderFiles | Where-Object { $_.Name -eq $requiredShader })) {
    throw "Required shader file not found: $shaderDir\$requiredShader"
  }
}
Copy-Item -LiteralPath $dll -Destination (Join-Path $packageDir "OpenGL32.dll") -Force
Copy-Item -LiteralPath $readme -Destination (Join-Path $packageDir "README.md") -Force
Copy-Item -LiteralPath $iniReference -Destination (Join-Path $packageDir "INI_SETTINGS.md") -Force
Copy-Item -LiteralPath $ini -Destination (Join-Path $waterDir "tr456_water.ini") -Force
$shaderFiles | Copy-Item -Destination $waterDir -Force

$packageToolsDir = Join-Path $packageDir "tools"
New-Item -ItemType Directory -Force -Path $packageToolsDir | Out-Null
Copy-Item -LiteralPath (Join-Path $root "tools\validate_tr456_runtime.ps1") `
  -Destination (Join-Path $packageToolsDir "validate_tr456_runtime.ps1") `
  -Force

$installText = @'
TombRaiderNewShaders - Nexus runtime package

Manual install:
1. Close Tomb Raider I-III Remastered.
2. Open the game directory that contains tomb123.exe / tomb456.exe.
3. Extract this archive into that directory so OpenGL32.dll is next to the game exe and tr456_water is a subfolder.
4. Launch the game.

Files installed:
- OpenGL32.dll
- INI_SETTINGS.md
- tools\validate_tr456_runtime.ps1
- tr456_water\tr456_water.ini
- tr456_water\*.glsl

Main changes in 1.2.36:
- Vulkan/Zink is the primary release path for NVIDIA, AMD, and Intel Vulkan drivers.
- FlowLite flowing water varies subtly by world location and keeps the softer non-scaly texture pass.
- FlowLite shaders now live as external GLSL files next to the synthetic water shaders, so future water tuning no longer requires editing huge C string literals.
- Standing water keeps the new micro-tremble layer, restores the original standing draw layer, and keeps synthetic refraction/lens distortion detached from the original layer through `StandingRefractionOriginalBlend=0.00`.
- FlowLite bottom refraction is more visible through a multi-sample floor lens pass plus Fresnel-balanced reflections.
- FlowLite now wires speed, secondary motion, breakup, bump/detail response, chromatic refraction, and specular streak INI controls into the active FlowLite shader.
- FlowLite now amplifies captured-scene deltas so bottom refraction is visible while the water stays translucent.
- FlowLite now has an off-by-default diagnostic mode and a low-noise draw probe for support captures.
- FlowLite refraction now compensates for alpha blending so the captured scene/bottom distortion survives the final overlay composite.
- FlowLite lens compensation is now luminance-biased and clamped to avoid colorful/inverted refraction artifacts.
- Standing water tremble, breath, and micro chop are slightly calmer.
- Standing water keeps the bounds guard, original-mask preservation, layer offset, calmer broad motion, and micro tremble while the original standing draw layer is visible for this experiment.
- SafeMode=1 is available as an emergency pass-through support switch; the Nexus package validates SafeMode=0 for normal release play.
- The Nexus packager validates Vulkan/Zink, safe-mode, and ReShade-safe settings before creating the archive.

Requirements:
- Windows x64.
- Tomb Raider I-III Remastered PC release.
- A Vulkan-capable NVIDIA, AMD, or Intel GPU with a current vendor driver.
- Mesa/Zink WGL runtime installed as OpenGL32_orig.dll next to tomb123.exe / tomb456.exe before launching.
- ReShade OpenGL proxy chaining must stay disabled. Use ReShade's Vulkan layer instead if ReShade is needed.

Compatibility note:
This package is Vulkan-only through Mesa/Zink. OpenGL32_orig.dll must be the Mesa/Zink OpenGL DLL. The proxy will not fall back to the system OpenGL runtime while VulkanOnly=1.

Unsupported release targets:
- Microsoft Basic Render Driver or machines without a working Vulkan driver.
- Plain system opengl32.dll copied as OpenGL32_orig.dll.
- Proton/Wine/Steam Deck unless a native opengl32 DLL override is configured.

ReShade note:
OpenGL ReShade proxy chaining is disabled in this Vulkan-only package. Keep ReShadeChain=0. Do not install ReShade as OpenGL for this game, and do not rename ReShade64.dll to OpenGL32.dll.

Supported ReShade setup:
1. Keep this mod's OpenGL32.dll next to tomb123.exe / tomb456.exe.
2. Keep the Mesa/Zink WGL runtime as OpenGL32_orig.dll in the same directory.
3. Run the official ReShade setup tool and select tomb123.exe or tomb456.exe.
4. Select Vulkan when the setup tool asks for the rendering API, or enable ReShade's global Vulkan layer if the setup tool presents that option.
5. Launch the game. The expected path is Game -> water proxy -> Mesa/Zink -> Vulkan -> ReShade Vulkan layer.

Disable ReShade for clean benchmark captures or support diagnostics.

If OpenGL32_orig.dll is an older copy of this mod's proxy or a plain copy of the system OpenGL DLL, replace it with the Mesa/Zink runtime DLL before launching.

Startup/performance note:
This build uses the FlowLite Zink profile by default, avoids DllMain disk work, disables background shader preload by default, defers framebuffer capture until synthetic water needs it, and keeps verbose draw/texture logs off unless enabled in tr456_water.ini.
Runtime logs are written only if logs.txt is created manually in the game directory.

Safe mode:
If the game launches but water effects are suspected to cause a problem, set SafeMode=1 in tr456_water\tr456_water.ini. This keeps the proxy chain loaded but disables shader patching, synthetic/FBO water, native overlay suppression, FlowLite, and Wet Lara helpers. Set SafeMode=0 again for normal release play.

Support validation:
Run this from the game directory after installing the package:
powershell -ExecutionPolicy Bypass -File .\tools\validate_tr456_runtime.ps1 -GameDir . -ReleaseStrict

This package targets the Windows x64 release. On Proton/Wine/Steam Deck, copying the files may not be enough; configure a native DLL override for opengl32 if the proxy is not loaded.

Uninstall:
Delete this mod's OpenGL32.dll, OpenGL32_orig.dll, the Mesa/Zink runtime DLLs, and the tr456_water folder from the game directory.
'@
Set-Content -LiteralPath (Join-Path $packageDir "NEXUS_INSTALL.txt") -Encoding ASCII -Value $installText

if (Test-Path $zipPath) {
  Remove-Item -LiteralPath $zipPath -Force
}

Compress-Archive -Path (Join-Path $packageDir "*") -DestinationPath $zipPath -Force

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($zipPath)
try {
  $entries = @($archive.Entries)
  $glslEntries = @($entries | Where-Object { $_.FullName -like "tr456_water\*.glsl" })
  if ($glslEntries.Count -ne $shaderFiles.Count) {
    throw "Archive GLSL count mismatch: expected $($shaderFiles.Count), found $($glslEntries.Count)"
  }
  foreach ($requiredEntry in @(
      "OpenGL32.dll",
      "README.md",
      "INI_SETTINGS.md",
      "NEXUS_INSTALL.txt",
      "tools\validate_tr456_runtime.ps1",
      "tr456_water\tr456_water.ini",
      "tr456_water\tr456_water_synthetic_vertex.glsl",
      "tr456_water\tr456_water_synthetic.glsl",
      "tr456_water\tr456_water_flow_lite_vertex.glsl",
      "tr456_water\tr456_water_flow_lite.glsl")) {
    if (-not ($entries | Where-Object { $_.FullName -eq $requiredEntry })) {
      throw "Archive missing required entry: $requiredEntry"
    }
  }
} finally {
  $archive.Dispose()
}

$hash = Get-FileHash -Algorithm SHA256 -LiteralPath $zipPath
$item = Get-Item -LiteralPath $zipPath
Write-Host "Built $($item.FullName)"
Write-Host "Bytes $($item.Length)"
Write-Host "SHA256 $($hash.Hash)"
