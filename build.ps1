param(
  [string]$Zig = $env:ZIG,
  [string]$Out = "build\OpenGL32.dll",
  [string]$GameDir = "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered",
  [string]$ForwardSource
)

$ErrorActionPreference = "Stop"

$argsForBuild = @{
  Zig = $Zig
  Out = $Out
  GameDir = $GameDir
}

if ($ForwardSource) {
  $argsForBuild.ForwardSource = $ForwardSource
}

& (Join-Path $PSScriptRoot "tools\build_tr456_water_proxy.ps1") @argsForBuild
