param(
  [string]$GameDir = "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered"
)

$ErrorActionPreference = "Stop"

$proc = Get-Process -Name tomb456,tomb123 -ErrorAction SilentlyContinue
if ($proc) {
  throw "Close tomb456.exe/tomb123.exe before uninstalling the proxy DLL."
}

$modDir = Join-Path $GameDir "tr456_water"
$dstDll = Join-Path $GameDir "OpenGL32.dll"
$prevDll = Join-Path $modDir "OpenGL32.dll.tr456-prev.bak"
$legacyPrevDll = Join-Path $GameDir "OpenGL32.dll.tr456-prev.bak"
$origDll = Join-Path $GameDir "OpenGL32_orig.dll"
$reshadeChainDll = Join-Path $GameDir "OpenGL32_reshade.dll"

$shaderFiles = @(
  "tr456_water_synthetic_vertex.glsl",
  "tr456_water_synthetic.glsl"
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

$supportFiles = $shaderFiles + $staleShaderFiles + @(
  "tr456_water.ini",
  "tr456_water_proxy.log"
)

if (Test-Path $prevDll) {
  Copy-Item -LiteralPath $prevDll -Destination $dstDll -Force
  Write-Host "Restored previous OpenGL32.dll."
} elseif (Test-Path $legacyPrevDll) {
  Copy-Item -LiteralPath $legacyPrevDll -Destination $dstDll -Force
  Write-Host "Restored previous OpenGL32.dll from legacy backup."
} elseif (Test-Path $origDll) {
  Copy-Item -LiteralPath $origDll -Destination $dstDll -Force
  Write-Host "Restored previous OpenGL32.dll from OpenGL32_orig.dll."
} elseif (Test-Path $reshadeChainDll) {
  Copy-Item -LiteralPath $reshadeChainDll -Destination $dstDll -Force
  Write-Host "Restored ReShade OpenGL32.dll from OpenGL32_reshade.dll."
} else {
  Remove-Item -LiteralPath $dstDll -Force -ErrorAction SilentlyContinue
  Write-Host "Removed proxy OpenGL32.dll."
}

foreach ($file in $supportFiles) {
  Remove-Item -LiteralPath (Join-Path $modDir $file) -Force -ErrorAction SilentlyContinue
}
Remove-Item -LiteralPath $prevDll -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $modDir -Force -Recurse -ErrorAction SilentlyContinue

foreach ($file in $supportFiles) {
  Remove-Item -LiteralPath (Join-Path $GameDir $file) -Force -ErrorAction SilentlyContinue
}
Remove-Item -LiteralPath $legacyPrevDll -Force -ErrorAction SilentlyContinue
if (Test-Path $origDll) {
  Write-Host "Left OpenGL32_orig.dll in place. Rename it back to OpenGL32.dll if it is another wrapper you chained manually."
}
if (Test-Path $reshadeChainDll) {
  Write-Host "Left OpenGL32_reshade.dll in place. Delete it if you no longer use ReShade."
}
