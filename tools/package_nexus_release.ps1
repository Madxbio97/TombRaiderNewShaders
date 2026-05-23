param(
  [string]$Version = "1.2.14",
  [string]$OutDir = "dist"
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
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
$ini = Join-Path $root "tr456_water.ini"
$shaderDir = Join-Path $root "shaders"

foreach ($required in @($dll, $readme, $ini, $shaderDir)) {
  if (-not (Test-Path $required)) {
    throw "Required release input not found: $required"
  }
}

$iniText = Get-Content -LiteralPath $ini -Raw
foreach ($releaseOffKey in @(
    "VerboseLog",
    "WetLaraTraceLog",
    "PerfTelemetry")) {
  if ($iniText -notmatch "(?m)^\s*$releaseOffKey\s*=\s*0\s*$") {
    throw "Release INI must keep $releaseOffKey=0 for the Nexus package"
  }
}

$shaderFiles = @(Get-ChildItem -LiteralPath $shaderDir -Filter "*.glsl" -File)
if ($shaderFiles.Count -lt 1) {
  throw "No GLSL shaders found in $shaderDir"
}
Copy-Item -LiteralPath $dll -Destination (Join-Path $packageDir "OpenGL32.dll") -Force
Copy-Item -LiteralPath $readme -Destination (Join-Path $packageDir "README.md") -Force
Copy-Item -LiteralPath $ini -Destination (Join-Path $waterDir "tr456_water.ini") -Force
$shaderFiles | Copy-Item -Destination $waterDir -Force

$installText = @'
TombRaiderNewShaders - Nexus runtime package

Manual install:
1. Close Tomb Raider I-III Remastered.
2. Open the game directory that contains tomb123.exe / tomb456.exe.
3. Extract this archive into that directory so OpenGL32.dll is next to the game exe and tr456_water is a subfolder.
4. Launch the game.

Files installed:
- OpenGL32.dll
- tr456_water\tr456_water.ini
- tr456_water\*.glsl

Compatibility note:
This experimental branch is Vulkan-only. OpenGL32_orig.dll must be a Mesa/Zink OpenGL DLL. The proxy will not fall back to the system OpenGL runtime while VulkanOnly=1.

ReShade note:
OpenGL ReShade chaining is disabled for profiling this branch. Keep ReShadeChain=0 while testing WATER-ZINK.

If OpenGL32_orig.dll is an older copy of this mod's proxy or a plain copy of the system OpenGL DLL, replace it with the Mesa/Zink runtime DLL before launching.

Startup/performance note:
This build avoids DllMain disk work, disables background shader preload by default, defers framebuffer capture until synthetic water needs it, and keeps verbose draw/texture logs off unless enabled in tr456_water.ini.
Runtime logs are written only if logs.txt is created manually in the game directory.

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
  foreach ($requiredEntry in @("OpenGL32.dll", "README.md", "NEXUS_INSTALL.txt", "tr456_water\tr456_water.ini")) {
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
