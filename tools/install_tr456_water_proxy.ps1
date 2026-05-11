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
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_surface_vertex.glsl") -Destination (Join-Path $modDir "tr456_water_surface_vertex.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_reflect.glsl") -Destination (Join-Path $modDir "tr456_water_reflect.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_reflect_vertex.glsl") -Destination (Join-Path $modDir "tr456_water_reflect_vertex.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_ssr.glsl") -Destination (Join-Path $modDir "tr456_water_ssr.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_flow.glsl") -Destination (Join-Path $modDir "tr456_water_flow.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_flow_vertex.glsl") -Destination (Join-Path $modDir "tr456_water_flow_vertex.glsl") -Force
Copy-Item -LiteralPath (Join-Path $root "shaders\tr456_water_ripple.glsl") -Destination (Join-Path $modDir "tr456_water_ripple.glsl") -Force

foreach ($staleShader in @("tr456_water_flow_foam.glsl", "tr456_water_room.glsl", "tr456_water_room_vertex.glsl")) {
  Remove-Item -LiteralPath (Join-Path $modDir $staleShader) -Force -ErrorAction SilentlyContinue
}

Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_surface.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_surface_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_reflect.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_reflect_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_ssr.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_ripple.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow_foam.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_room.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_room_vertex.glsl") -Force -ErrorAction SilentlyContinue

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
    "SurfaceWave=1.12",
    "SurfaceVertexStrength=0.46",
    "SurfaceVertexWaveStrength=1.05",
    "PixelWaveStrength=1.62",
    "RefractionWaveStrength=1.52",
    "DeepCausticsStrength=0.86",
    "WaterVolumeStrength=1.18",
    "ShorelineStrength=0.78",
    "GameRippleStrength=1.45",
    "RefractStrength=1.18",
    "ReflectStrength=1.48",
    "SSRStrength=1.00",
    "GlintStrength=0.56",
    "FoamStrength=0.54",
    "ChromaStrength=0.36",
    "TintStrength=0.74",
    "CausticsStrength=0.46",
    "DepthStrength=0.84",
    "RippleStrength=0.62",
    "RippleCenterX=0.50",
    "RippleCenterY=0.38",
    "SurfaceRelief=1.08",
    "WakeStrength=0.95",
    "WakeWidth=0.58",
    "WakeLength=0.84",
    "ContactWaveStrength=1.35",
    "ContactWaveRadius=1.00",
    "ContactWaveSpeed=1.34",
    "ContactVertexStrength=0.30",
    "ContactNormalStrength=1.35",
    "ContactCoordMode=1",
    "PatchRipplePass=1",
    "RippleSpriteMinCount=96",
    "MicroRippleStrength=0.36",
    "MicroRippleScale=0.72",
    "MirrorRoughness=1.02",
    "SwellStrength=0.72",
    "SwellScale=0.70",
    "WakeWaveStrength=0.88",
    "EdgeWaveStrength=0.42",
    "EdgeWaveWidth=0.085",
    "RoughReflection=0.90",
    "FresnelStrength=1.05",
    "BottomCaustics=0.82",
    "ContactEdge=0.72",
    "DepthAbsorption=1.08",
    "WallReflectionStretch=0.84",
    "WaterSaturation=1.02",
    "WaterBrightness=0.82",
    "WaterTextureStrength=0.92",
    "FlowWaterStrength=1.04",
    "FlowReflectionStrength=0.68",
    "FlowOpacity=0.82",
    "FlowChromaStrength=0.28",
    "FlowCausticsStrength=0.18",
    "FlowVertexStrength=0.68",
    "FlowWaveStrength=1.18",
    "FlowSpeed=1.75",
    "FlowStreakFoam=0.82",
    "Opacity=0.58",
    "ForceReflection=0.82",
    "SceneReflectionStrength=0.92",
    "ReflectionContrast=1.32",
    "FramebufferReflection=1",
    "DiagnosticDumpShaders=0",
    "DiagnosticLogShaders=0",
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
Write-Host "Shader files: tr456_water\tr456_water_surface.glsl, tr456_water\tr456_water_surface_vertex.glsl, tr456_water\tr456_water_reflect.glsl, tr456_water\tr456_water_reflect_vertex.glsl, tr456_water\tr456_water_ssr.glsl, tr456_water\tr456_water_flow.glsl, tr456_water\tr456_water_flow_vertex.glsl, tr456_water\tr456_water_ripple.glsl"
