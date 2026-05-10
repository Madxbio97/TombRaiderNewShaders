param(
  [string]$GameDir = "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered",
  [switch]$RestoreExeBackup
)

$ErrorActionPreference = "Stop"

$proc = Get-Process -Name tomb456,tomb123 -ErrorAction SilentlyContinue
if ($proc) {
  throw "Close tomb456.exe/tomb123.exe before installing the proxy DLL."
}

$root = Split-Path -Parent $PSScriptRoot
$dll = Join-Path $root "build\OpenGL32.dll"
if (-not (Test-Path $dll)) {
  throw "Build output not found: $dll"
}
if (-not (Test-Path $GameDir)) {
  throw "Game directory not found: $GameDir"
}

$exe = Join-Path $GameDir "tomb456.exe"
$exeBak = "$exe.tr456-water.bak"
if ($RestoreExeBackup -and (Test-Path $exeBak)) {
  Copy-Item -LiteralPath $exeBak -Destination $exe -Force
  Write-Host "Restored original tomb456.exe from $exeBak"
} elseif ($RestoreExeBackup) {
  Write-Host "No tomb456.exe backup found at $exeBak"
}

$modDir = Join-Path $GameDir "tr456_water"
New-Item -ItemType Directory -Force -Path $modDir | Out-Null

$dstDll = Join-Path $GameDir "OpenGL32.dll"
$prevDll = Join-Path $modDir "OpenGL32.dll.tr456-prev.bak"
$legacyPrevDll = Join-Path $GameDir "OpenGL32.dll.tr456-prev.bak"
$origDll = Join-Path $GameDir "OpenGL32_orig.dll"
$systemDll = Join-Path $env:WINDIR "System32\opengl32.dll"

if ((Test-Path $legacyPrevDll) -and -not (Test-Path $prevDll)) {
  Move-Item -LiteralPath $legacyPrevDll -Destination $prevDll -Force
  Write-Host "Moved legacy backup to $prevDll"
} elseif (Test-Path $legacyPrevDll) {
  Remove-Item -LiteralPath $legacyPrevDll -Force
}

if (Test-Path $prevDll) {
  Copy-Item -LiteralPath $prevDll -Destination $origDll -Force
  Write-Host "Prepared forward target $origDll"
} elseif (Test-Path $origDll) {
  Copy-Item -LiteralPath $origDll -Destination $prevDll -Force
  Write-Host "Backed up forward target to $prevDll"
} elseif (Test-Path $dstDll) {
  Copy-Item -LiteralPath $dstDll -Destination $prevDll -Force
  Copy-Item -LiteralPath $prevDll -Destination $origDll -Force
  Write-Host "Prepared forward target $origDll"
} elseif (Test-Path $systemDll) {
  Copy-Item -LiteralPath $systemDll -Destination $prevDll -Force
  Copy-Item -LiteralPath $prevDll -Destination $origDll -Force
  Write-Host "Prepared forward target from system OpenGL32.dll"
} else {
  throw "No OpenGL32.dll found to chain."
}

Copy-Item -LiteralPath $dll -Destination $dstDll -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_surface.glsl") -Destination (Join-Path $modDir "tr456_water_surface.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_reflect.glsl") -Destination (Join-Path $modDir "tr456_water_reflect.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_ssr.glsl") -Destination (Join-Path $modDir "tr456_water_ssr.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_flow.glsl") -Destination (Join-Path $modDir "tr456_water_flow.glsl") -Force

Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_surface.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_reflect.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_ssr.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow.glsl") -Force -ErrorAction SilentlyContinue

$ini = Join-Path $modDir "tr456_water.ini"
$legacyIni = Join-Path $GameDir "tr456_water.ini"
if ((Test-Path $legacyIni) -and -not (Test-Path $ini)) {
  Move-Item -LiteralPath $legacyIni -Destination $ini -Force
  Write-Host "Moved legacy tr456_water.ini to $ini"
} elseif (Test-Path $legacyIni) {
  Remove-Item -LiteralPath $legacyIni -Force
}

if (-not (Test-Path $ini)) {
  Copy-Item -LiteralPath (Join-Path $root "tr456_water.ini") -Destination $ini -Force
  Write-Host "Created tr456_water.ini"
} else {
  $iniText = Get-Content -Raw -LiteralPath $ini
  $missing = New-Object System.Collections.Generic.List[string]
  foreach ($entry in @(
    "ReflectionQuality=1",
    "SurfaceWave=1.14",
    "RefractStrength=1.05",
    "ReflectStrength=1.76",
    "SSRStrength=1.16",
    "GlintStrength=0.88",
    "FoamStrength=0.82",
    "CausticsStrength=0.95",
    "DepthStrength=0.75",
    "RippleStrength=0.85",
    "RippleCenterX=0.50",
    "RippleCenterY=0.38",
    "SurfaceRelief=1.18",
    "WakeStrength=1.55",
    "WakeWidth=0.58",
    "WakeLength=0.84",
    "MicroRippleStrength=0.78",
    "MicroRippleScale=0.95",
    "MirrorRoughness=1.18",
    "SwellStrength=1.18",
    "SwellScale=0.82",
    "WakeWaveStrength=1.65",
    "EdgeWaveStrength=0.90",
    "EdgeWaveWidth=0.085",
    "RoughReflection=1.04",
    "FresnelStrength=1.18",
    "BottomCaustics=0.82",
    "ContactEdge=0.72",
    "DepthAbsorption=0.88",
    "WallReflectionStretch=0.84",
    "WaterSaturation=1.12",
    "WaterBrightness=0.79",
    "WaterTextureStrength=1.22",
    "FlowWaterStrength=1.00",
    "FlowReflectionStrength=1.08",
    "FlowOpacity=1.03",
    "Opacity=0.68",
    "ForceReflection=0.95",
    "SceneReflectionStrength=1.00",
    "ReflectionContrast=1.42",
    "FramebufferReflection=1",
    "DiagnosticDumpShaders=1",
    "DiagnosticFrames=150",
    "DiagnosticMaxLines=420"
  )) {
    $key = ($entry -split "=", 2)[0]
    if ($iniText -notmatch "(?m)^\s*$([regex]::Escape($key))\s*=") {
      $missing.Add($entry)
    }
  }
  if ($missing.Count -gt 0) {
    Add-Content -LiteralPath $ini -Encoding ASCII -Value ""
    Add-Content -LiteralPath $ini -Encoding ASCII -Value "; Added by installer for water reflection tuning."
    foreach ($entry in $missing) {
      Add-Content -LiteralPath $ini -Encoding ASCII -Value $entry
    }
    Write-Host "Updated tr456_water.ini with new reflection settings"
  }
}

$log = Join-Path $modDir "tr456_water_proxy.log"
$legacyLog = Join-Path $GameDir "tr456_water_proxy.log"
$diagDir = Join-Path $modDir "diagnostics"
Remove-Item -LiteralPath $log -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $legacyLog -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $diagDir -Recurse -Force -ErrorAction SilentlyContinue

Write-Host "Installed TR456 water proxy DLL."
Write-Host "Support directory: $modDir"
Write-Host "Shader files: tr456_water\tr456_water_surface.glsl, tr456_water\tr456_water_reflect.glsl, tr456_water\tr456_water_ssr.glsl, tr456_water\tr456_water_flow.glsl"
