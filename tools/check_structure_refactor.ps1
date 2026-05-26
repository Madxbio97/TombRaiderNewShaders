param(
  [string]$Zig = $env:ZIG,
  [string]$ForwardSource = (Join-Path $env:WINDIR "System32\opengl32.dll"),
  [switch]$SkipBuild,
  [switch]$SkipPackage
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot

function Invoke-CheckedStep($Name, [scriptblock]$Step) {
  Write-Host "==> $Name"
  & $Step
  if ($null -ne $LASTEXITCODE -and $LASTEXITCODE -ne 0) {
    throw "$Name failed with exit code $LASTEXITCODE"
  }
}

Invoke-CheckedStep "settings/package ownership audit" {
  & (Join-Path $PSScriptRoot "audit_ini_settings.ps1") `
    -FailOnPackageDrift `
    -FailOnUnclassified
}

if (-not $SkipBuild) {
  $buildArgs = @{
    ForwardSource = $ForwardSource
  }
  if ($Zig) {
    $buildArgs.Zig = $Zig
  }
  Invoke-CheckedStep "proxy DLL build" {
    & (Join-Path $PSScriptRoot "build_tr456_water_proxy.ps1") @buildArgs
  }
}

if (-not $SkipPackage) {
  Invoke-CheckedStep "Nexus release package" {
    & (Join-Path $PSScriptRoot "package_nexus_release.ps1")
  }
  Invoke-CheckedStep "Nexus package runtime validation" {
    & (Join-Path $PSScriptRoot "validate_tr456_runtime.ps1") `
      -GameDir (Join-Path $root "dist\TombRaiderNewShaders-Nexus") `
      -ReleaseStrict `
      -AllowPackageRoot
  }
}

$git = Get-Command git -ErrorAction SilentlyContinue
if ($git) {
  Invoke-CheckedStep "git diff whitespace check" {
    git -C $root diff --check
  }
} else {
  Write-Host "git not found; skipped diff whitespace check"
}

Write-Host "Structure refactor gate passed."
