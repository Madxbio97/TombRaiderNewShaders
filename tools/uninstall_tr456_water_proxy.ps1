param(
  [string]$GameDir = "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered"
)

$ErrorActionPreference = "Stop"

$proc = Get-Process -Name tomb456 -ErrorAction SilentlyContinue
if ($proc) {
  throw "Close tomb456.exe before uninstalling the proxy DLL."
}

$modDir = Join-Path $GameDir "tr456_water"
$dstDll = Join-Path $GameDir "OpenGL32.dll"
$prevDll = Join-Path $modDir "OpenGL32.dll.tr456-prev.bak"
$legacyPrevDll = Join-Path $GameDir "OpenGL32.dll.tr456-prev.bak"
if (Test-Path $prevDll) {
  Copy-Item -LiteralPath $prevDll -Destination $dstDll -Force
  Write-Host "Restored previous OpenGL32.dll."
} elseif (Test-Path $legacyPrevDll) {
  Copy-Item -LiteralPath $legacyPrevDll -Destination $dstDll -Force
  Write-Host "Restored previous OpenGL32.dll from legacy backup."
} else {
  Remove-Item -LiteralPath $dstDll -Force -ErrorAction SilentlyContinue
  Write-Host "Removed proxy OpenGL32.dll."
}

Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_surface.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_surface_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_reflect.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_reflect_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_ssr.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_flow.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_flow_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_flow_foam.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_room.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_room_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water.ini") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_proxy.log") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $prevDll -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $modDir -Force -ErrorAction SilentlyContinue

Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_surface.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_surface_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_reflect.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_reflect_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_ssr.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_flow_foam.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_room.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_room_vertex.glsl") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water.ini") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "tr456_water_proxy.log") -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $legacyPrevDll -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $GameDir "OpenGL32_orig.dll") -Force -ErrorAction SilentlyContinue
