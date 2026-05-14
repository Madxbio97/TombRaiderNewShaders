param(
  [string]$GameDir = "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered",
  [int]$DebugMode = 0,
  [int]$WaterGridOverlay = -1,
  [int]$WaterGridFlowOverlay = -1,
  [switch]$DumpShaders,
  [switch]$LogShaders,
  [switch]$ClearLog
)

$ErrorActionPreference = "Stop"

$modDir = Join-Path $GameDir "tr456_water"
$ini = Join-Path $modDir "tr456_water.ini"
if (-not (Test-Path $ini)) {
  throw "tr456_water.ini not found: $ini"
}

function Set-IniEntry {
  param(
    [string]$Path,
    [string]$Key,
    [string]$Value
  )
  $text = Get-Content -Raw -LiteralPath $Path
  $entry = "$Key=$Value"
  $pattern = "(?m)^\s*$([regex]::Escape($Key))\s*=.*$"
  if ($text -match $pattern) {
    $text = [regex]::Replace($text, $pattern, $entry)
  } else {
    $text = $text.TrimEnd() + "`r`n$entry`r`n"
  }
  Set-Content -LiteralPath $Path -Encoding ASCII -Value $text
}

Set-IniEntry -Path $ini -Key "DebugMode" -Value ([string]$DebugMode)
if ($WaterGridOverlay -ge 0) {
  Set-IniEntry -Path $ini -Key "WaterGridOverlay" -Value ([string]$WaterGridOverlay)
}
if ($WaterGridFlowOverlay -ge 0) {
  Set-IniEntry -Path $ini -Key "WaterGridFlowOverlay" -Value ([string]$WaterGridFlowOverlay)
}
if ($DumpShaders) {
  Set-IniEntry -Path $ini -Key "DiagnosticDumpShaders" -Value "1"
}
if ($LogShaders) {
  Set-IniEntry -Path $ini -Key "DiagnosticLogShaders" -Value "1"
}
if ($ClearLog) {
  Remove-Item -LiteralPath (Join-Path $modDir "tr456_water_proxy.log") -Force -ErrorAction SilentlyContinue
  Remove-Item -LiteralPath (Join-Path $modDir "diagnostics") -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "Set DebugMode=$DebugMode in $ini"
if ($WaterGridOverlay -ge 0) {
  Write-Host "Set WaterGridOverlay=$WaterGridOverlay"
}
if ($WaterGridFlowOverlay -ge 0) {
  Write-Host "Set WaterGridFlowOverlay=$WaterGridFlowOverlay"
}
if ($DumpShaders) { Write-Host "DiagnosticDumpShaders=1" }
if ($LogShaders) { Write-Host "DiagnosticLogShaders=1" }
if ($ClearLog) { Write-Host "Cleared water log and diagnostics directory" }
Write-Host "Restart the game after changing DebugMode, because shaders are compiled at startup."
