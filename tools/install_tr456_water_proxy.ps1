param(
  [string]$GameDir = "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered",
  [string]$ReShadeDll,
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
if (-not (Test-Path $exe)) {
  $tomb123 = Join-Path $GameDir "tomb123.exe"
  if (Test-Path $tomb123) {
    $exe = $tomb123
  }
}
$exeBak = "$exe.tr456-water.bak"
if ($RestoreExeBackup -and (Test-Path $exeBak)) {
  Copy-Item -LiteralPath $exeBak -Destination $exe -Force
  Write-Host "Restored original tomb456.exe from $exeBak"
} elseif ($RestoreExeBackup) {
  Write-Host "No tomb456.exe backup found at $exeBak"
}

$modDir = Join-Path $GameDir "tr456_water"
New-Item -ItemType Directory -Force -Path $modDir | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $modDir "shader_cache") | Out-Null

$dstDll = Join-Path $GameDir "OpenGL32.dll"
$prevDll = Join-Path $modDir "OpenGL32.dll.tr456-prev.bak"
$legacyPrevDll = Join-Path $GameDir "OpenGL32.dll.tr456-prev.bak"
$origDll = Join-Path $GameDir "OpenGL32_orig.dll"
$reshadeChainDll = Join-Path $GameDir "OpenGL32_reshade.dll"
$systemDll = Join-Path $env:WINDIR "System32\opengl32.dll"

function Test-Tr456ProxyDll($Path) {
  if (-not (Test-Path $Path)) {
    return $false
  }
  $bytes = [IO.File]::ReadAllBytes($Path)
  $text = [Text.Encoding]::ASCII.GetString($bytes)
  return $text.Contains("tr456 water proxy loaded") -or
    $text.Contains("tr456_water_proxy.log")
}

function Test-ReShadeDll($Path) {
  if (-not (Test-Path $Path)) {
    return $false
  }
  $bytes = [IO.File]::ReadAllBytes($Path)
  $text = [Text.Encoding]::ASCII.GetString($bytes)
  return $text.Contains("ReShade") -or
    $text.Contains("reshade") -or
    $text.Contains("crosire")
}

function Test-SameFileHash($Left, $Right) {
  if (-not (Test-Path $Left) -or -not (Test-Path $Right)) {
    return $false
  }
  $leftItem = Get-Item -LiteralPath $Left
  $rightItem = Get-Item -LiteralPath $Right
  if ($leftItem.Length -ne $rightItem.Length) {
    return $false
  }
  return (Get-FileHash -Algorithm SHA256 -LiteralPath $Left).Hash -eq
    (Get-FileHash -Algorithm SHA256 -LiteralPath $Right).Hash
}

if ($ReShadeDll) {
  if (-not (Test-Path $ReShadeDll)) {
    throw "ReShade DLL not found: $ReShadeDll"
  }
  if (Test-Tr456ProxyDll $ReShadeDll) {
    throw "Refusing to use this mod's proxy as a ReShade chain DLL: $ReShadeDll"
  }
  if (Test-SameFileHash $ReShadeDll $systemDll) {
    throw "Refusing to use the system OpenGL32.dll as a ReShade chain DLL: $ReShadeDll"
  }
  Copy-Item -LiteralPath $ReShadeDll -Destination $reshadeChainDll -Force
  Write-Host "Prepared ReShade chain target $reshadeChainDll"
}

if ((Test-Path $legacyPrevDll) -and -not (Test-Path $prevDll)) {
  if (-not (Test-Path $origDll)) {
    Copy-Item -LiteralPath $legacyPrevDll -Destination $origDll -Force
  }
  Remove-Item -LiteralPath $legacyPrevDll -Force
  Write-Host "Prepared forward target from legacy backup"
} elseif (Test-Path $legacyPrevDll) {
  Remove-Item -LiteralPath $legacyPrevDll -Force
}

if ((Test-Path $origDll) -and (Test-Tr456ProxyDll $origDll)) {
  Remove-Item -LiteralPath $origDll -Force
  Write-Host "Removed stale TR456 proxy chain target"
}

if ((Test-Path $origDll) -and (Test-SameFileHash $origDll $systemDll)) {
  Remove-Item -LiteralPath $origDll -Force
  Write-Host "Removed redundant system OpenGL32.dll chain target"
}

if ((Test-Path $prevDll) -and -not (Test-Tr456ProxyDll $prevDll)) {
  Copy-Item -LiteralPath $prevDll -Destination $origDll -Force
  Remove-Item -LiteralPath $prevDll -Force
  Write-Host "Prepared forward target $origDll"
} elseif (Test-Path $origDll) {
  Write-Host "Prepared forward target $origDll"
} elseif ((Test-Path $dstDll) -and -not (Test-Tr456ProxyDll $dstDll)) {
  if (Test-ReShadeDll $dstDll) {
    Copy-Item -LiteralPath $dstDll -Destination $reshadeChainDll -Force
    Write-Host "Prepared ReShade chain target $reshadeChainDll"
  } else {
    Copy-Item -LiteralPath $dstDll -Destination $origDll -Force
    Write-Host "Prepared forward target $origDll"
  }
} elseif (Test-Path $systemDll) {
  Write-Host "No previous OpenGL wrapper found; proxy will use system OpenGL32.dll"
} else {
  throw "No OpenGL32.dll found to chain."
}

Copy-Item -LiteralPath $dll -Destination $dstDll -Force

$shaderFiles = @(
  "tr456_scene_post_vertex.glsl",
  "tr456_scene_post.glsl",
  "tr456_water_synthetic_vertex.glsl",
  "tr456_water_synthetic.glsl",
  "tr456_water_contact_ssgi.glsl"
)

$staleShaderFiles = @(
  "tr456_water_surface.glsl",
  "tr456_water_surface_vertex.glsl",
  "tr456_water_surface_geometry.glsl",
  "tr456_water_reflect.glsl",
  "tr456_water_reflect_geometry.glsl",
  "tr456_water_reflect_vertex.glsl",
  "tr456_water_ssr.glsl",
  "tr456_water_flow.glsl",
  "tr456_water_flow_vertex.glsl",
  "tr456_water_flow_geometry.glsl",
  "tr456_water_grid_vertex.glsl",
  "tr456_water_grid_geometry.glsl",
  "tr456_water_grid.glsl",
  "tr456_water_synthetic_geometry.glsl",
  "tr456_water_ripple.glsl",
  "tr456_water_flow_foam.glsl",
  "tr456_water_room.glsl",
  "tr456_water_room_vertex.glsl",
  "tr456_water_detail.bmp"
)

foreach ($shader in $shaderFiles) {
  Copy-Item -LiteralPath (Join-Path $root "shaders\$shader") -Destination (Join-Path $modDir $shader) -Force
}

Remove-Item -LiteralPath (Join-Path $modDir "Water.dds") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_overlay_bump.dds") -Force -ErrorAction SilentlyContinue

foreach ($shader in $staleShaderFiles) {
  Remove-Item -LiteralPath (Join-Path $modDir $shader) -Force -ErrorAction SilentlyContinue
}

foreach ($shader in ($shaderFiles + $staleShaderFiles)) {
  Remove-Item -LiteralPath (Join-Path $GameDir $shader) -Force -ErrorAction SilentlyContinue
}

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
  $canonicalIni = Get-Content -Raw -LiteralPath (Join-Path $root "tr456_water.ini")
  $iniText = Get-Content -Raw -LiteralPath $ini
  $canonicalNormalized = ($canonicalIni -replace "`r`n", "`n").TrimEnd("`n")
  $iniNormalized = ($iniText -replace "`r`n", "`n").TrimEnd("`n")
  if ($iniNormalized -ne $canonicalNormalized) {
    $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
    Copy-Item -LiteralPath $ini -Destination "$ini.before-installer-sync-$stamp.bak" -Force
    $canonicalOut = ($canonicalNormalized -replace "`n", "`r`n") + "`r`n"
    Set-Content -LiteralPath $ini -Encoding ASCII -NoNewline -Value $canonicalOut
    Write-Host "Synced canonical tr456_water.ini"
  }
}

$log = Join-Path $modDir "tr456_water_proxy.log"
$legacyLog = Join-Path $GameDir "tr456_water_proxy.log"
$diagDir = Join-Path $modDir "diagnostics"
Remove-Item -LiteralPath $log -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $legacyLog -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $diagDir -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $prevDll -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $legacyPrevDll -Force -ErrorAction SilentlyContinue

Write-Host "Installed TR456 water proxy DLL."
Write-Host "Support directory: $modDir"
$shaderList = ($shaderFiles | ForEach-Object { "tr456_water\$_" }) -join ", "
Write-Host "Shader files: $shaderList"
