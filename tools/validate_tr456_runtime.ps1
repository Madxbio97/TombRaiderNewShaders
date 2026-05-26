param(
  [string]$GameDir = ".",
  [string]$IniPath,
  [switch]$ReleaseStrict,
  [switch]$AllowPackageRoot,
  [string]$ReportPath
)

$ErrorActionPreference = "Stop"

$checks = New-Object System.Collections.Generic.List[object]

function Resolve-InputPath($Path, $BasePath) {
  if ([IO.Path]::IsPathRooted($Path)) {
    return $Path
  }
  return Join-Path $BasePath $Path
}

function Add-Check($Name, $Status, $Detail) {
  [void]$checks.Add([pscustomobject]@{
      Name = $Name
      Status = $Status
      Detail = $Detail
    })
}

function Test-ContainsAscii($Path, [string[]]$Needles) {
  if (-not (Test-Path $Path)) {
    return $false
  }
  $bytes = [IO.File]::ReadAllBytes($Path)
  $text = [Text.Encoding]::ASCII.GetString($bytes)
  foreach ($needle in $Needles) {
    if ($text.Contains($needle)) {
      return $true
    }
  }
  return $false
}

function Test-Tr456ProxyDll($Path) {
  return Test-ContainsAscii $Path @(
    "tr456 water proxy loaded",
    "tr456_water_proxy.log")
}

function Test-ReShadeDll($Path) {
  return Test-ContainsAscii $Path @(
    "ReShade",
    "reshade",
    "crosire")
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

function Read-IniMap($Path) {
  $map = @{}
  foreach ($line in Get-Content -LiteralPath $Path) {
    if ($line -match '^\s*([A-Za-z0-9_]+)\s*=\s*(.*?)\s*(?:[;#].*)?$') {
      $map[$Matches[1]] = $Matches[2].Trim()
    }
  }
  return $map
}

function Get-IniValue($Map, $Key) {
  if ($Map -and $Map.ContainsKey($Key)) {
    return $Map[$Key]
  }
  return ""
}

function Add-IniExpectation($Map, $Key, $Expected, $FailureStatus) {
  $actual = Get-IniValue $Map $Key
  if ($actual -eq "") {
    Add-Check "INI $Key" $FailureStatus "Missing; expected $Expected."
  } elseif ($actual -ne $Expected) {
    Add-Check "INI $Key" $FailureStatus "Expected $Expected, found $actual."
  } else {
    Add-Check "INI $Key" "PASS" "Value is $Expected."
  }
}

function Format-Report($GameFull, $IniFull) {
  $failCount = @($checks | Where-Object { $_.Status -eq "FAIL" }).Count
  $warnCount = @($checks | Where-Object { $_.Status -eq "WARN" }).Count
  $passCount = @($checks | Where-Object { $_.Status -eq "PASS" }).Count

  $lines = New-Object System.Collections.Generic.List[string]
  [void]$lines.Add("TR456 runtime validation")
  [void]$lines.Add("GameDir: $GameFull")
  if ($IniFull) {
    [void]$lines.Add("INI: $IniFull")
  }
  [void]$lines.Add("ReleaseStrict: $([bool]$ReleaseStrict)")
  [void]$lines.Add("AllowPackageRoot: $([bool]$AllowPackageRoot)")
  [void]$lines.Add("")
  foreach ($check in $checks) {
    [void]$lines.Add(("[{0}] {1} - {2}" -f
        $check.Status,$check.Name,$check.Detail))
  }
  [void]$lines.Add("")
  [void]$lines.Add("Summary: PASS=$passCount WARN=$warnCount FAIL=$failCount")
  return ($lines -join [Environment]::NewLine)
}

function Write-ValidationReport($GameFull, $IniFull) {
  $report = Format-Report $GameFull $IniFull
  Write-Output $report
  if ($ReportPath) {
    $reportFull = Resolve-InputPath $ReportPath (Get-Location)
    $reportDir = Split-Path -Parent $reportFull
    if ($reportDir -and -not (Test-Path $reportDir)) {
      New-Item -ItemType Directory -Force -Path $reportDir | Out-Null
    }
    Set-Content -LiteralPath $reportFull -Encoding ASCII -Value $report
  }
}

$gameFull = Resolve-InputPath $GameDir (Get-Location)
if (-not (Test-Path $gameFull)) {
  Add-Check "Game directory" "FAIL" "Not found: $gameFull"
  Write-ValidationReport $gameFull $null
  exit 1
}
$gameFull = (Resolve-Path -LiteralPath $gameFull).Path

$supportDir = Join-Path $gameFull "tr456_water"
$dll = Join-Path $gameFull "OpenGL32.dll"
$forwardDll = Join-Path $gameFull "OpenGL32_orig.dll"
$reshadeDll = Join-Path $gameFull "OpenGL32_reshade.dll"
$systemDll = Join-Path $env:WINDIR "System32\opengl32.dll"
$legacyIni = Join-Path $gameFull "tr456_water.ini"

if ($AllowPackageRoot) {
  Add-Check "Package root mode" "PASS" "Game executables and OpenGL32_orig.dll are not required."
} else {
  $exes = @("tomb456.exe", "tomb123.exe") |
    ForEach-Object { Join-Path $gameFull $_ } |
    Where-Object { Test-Path $_ }
  if ($exes.Count -gt 0) {
    Add-Check "Game executable" "PASS" (($exes | ForEach-Object {
          Split-Path -Leaf $_
        }) -join ", ")
  } else {
    Add-Check "Game executable" "FAIL" "Expected tomb456.exe or tomb123.exe in the game directory."
  }
}

if (Test-Path $dll) {
  if (Test-Tr456ProxyDll $dll) {
    Add-Check "OpenGL32.dll" "PASS" "TR456 proxy signature found."
  } else {
    Add-Check "OpenGL32.dll" "FAIL" "File exists, but does not look like this mod's proxy DLL."
  }
} else {
  Add-Check "OpenGL32.dll" "FAIL" "Missing proxy DLL."
}

if (Test-Path $supportDir) {
  Add-Check "tr456_water directory" "PASS" "Support directory exists."
} else {
  Add-Check "tr456_water directory" "FAIL" "Missing support directory."
}

$iniFull = $null
if ($IniPath) {
  $iniFull = Resolve-InputPath $IniPath (Get-Location)
} else {
  $iniFull = Join-Path $supportDir "tr456_water.ini"
}

$iniMap = @{}
if (Test-Path $iniFull) {
  $iniFull = (Resolve-Path -LiteralPath $iniFull).Path
  $iniMap = Read-IniMap $iniFull
  Add-Check "Runtime INI" "PASS" "Loaded $iniFull"
} else {
  Add-Check "Runtime INI" "FAIL" "Missing $iniFull"
}

if (Test-Path $legacyIni) {
  Add-Check "Legacy root INI" "WARN" "Found $legacyIni; current installs use tr456_water\tr456_water.ini."
}

$safeMode = Get-IniValue $iniMap "SafeMode"
if ($safeMode -eq "0") {
  Add-Check "SafeMode" "PASS" "Normal rendering mode."
} elseif ($safeMode -eq "1") {
  $status = if ($ReleaseStrict) { "FAIL" } else { "WARN" }
  Add-Check "SafeMode" $status "Emergency pass-through mode is active."
} else {
  Add-Check "SafeMode" "FAIL" "Expected 0 or 1, found '$safeMode'."
}

$requiresForward = $ReleaseStrict -or ((Get-IniValue $iniMap "VulkanOnly") -eq "1")
if (Test-Path $forwardDll) {
  if (Test-Tr456ProxyDll $forwardDll) {
    Add-Check "OpenGL32_orig.dll" "FAIL" "Forward target is another TR456 proxy DLL."
  } elseif (Test-SameFileHash $forwardDll $systemDll) {
    $status = if ($ReleaseStrict) { "FAIL" } else { "WARN" }
    Add-Check "OpenGL32_orig.dll" $status "Forward target matches the Windows system OpenGL DLL."
  } elseif (Test-ReShadeDll $forwardDll) {
    Add-Check "OpenGL32_orig.dll" "FAIL" "Forward target looks like an OpenGL ReShade DLL; use ReShade's Vulkan layer instead."
  } else {
    Add-Check "OpenGL32_orig.dll" "PASS" "Forward target exists and is not a known bad chain target."
  }
} elseif ($AllowPackageRoot) {
  Add-Check "OpenGL32_orig.dll" "PASS" "Not included in the Nexus package; user installs Mesa/Zink next to the game executable."
} elseif ($requiresForward) {
  Add-Check "OpenGL32_orig.dll" "FAIL" "Required when ReleaseStrict or VulkanOnly=1 is active."
} else {
  Add-Check "OpenGL32_orig.dll" "WARN" "Missing; Auto mode may fall back to the system OpenGL runtime."
}

if (Test-Path $reshadeDll) {
  if (Test-Tr456ProxyDll $reshadeDll) {
    Add-Check "OpenGL32_reshade.dll" "FAIL" "ReShade chain target is this mod's proxy."
  } elseif (Test-SameFileHash $reshadeDll $systemDll) {
    Add-Check "OpenGL32_reshade.dll" "FAIL" "ReShade chain target matches the Windows system OpenGL DLL."
  } elseif (Test-ReShadeDll $reshadeDll) {
    Add-Check "OpenGL32_reshade.dll" "WARN" "OpenGL ReShade chaining is not the supported release path."
  } else {
    Add-Check "OpenGL32_reshade.dll" "WARN" "Unexpected chain DLL present."
  }
}

$requiredShaders = @(
  "tr456_water_synthetic_vertex.glsl",
  "tr456_water_synthetic.glsl",
  "tr456_water_flow_lite_vertex.glsl",
  "tr456_water_flow_lite.glsl")
foreach ($shader in $requiredShaders) {
  $path = Join-Path $supportDir $shader
  if (Test-Path $path) {
    Add-Check "Shader $shader" "PASS" "Present."
  } else {
    Add-Check "Shader $shader" "FAIL" "Missing from tr456_water."
  }
}

$staleShaders = @(
  "tr456_scene_post_vertex.glsl",
  "tr456_scene_post.glsl",
  "tr456_scene_ssgi.glsl",
  "tr456_scene_bump.glsl",
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
  "tr456_water_contact_ssgi.glsl",
  "tr456_water_ripple.glsl",
  "tr456_water_flow_foam.glsl",
  "tr456_water_room.glsl",
  "tr456_water_room_vertex.glsl")
foreach ($shader in $staleShaders) {
  foreach ($dir in @($gameFull, $supportDir)) {
    $path = Join-Path $dir $shader
    if (Test-Path $path) {
      Add-Check "Stale shader $shader" "WARN" "Old shader experiment remains at $path."
    }
  }
}

$releaseOffKeys = @(
  "SafeMode",
  "VerboseLog",
  "WetLaraTraceLog",
  "WetLaraDebugVisible",
  "SyntheticDebugSolid",
  "FlowLiteDebugMode",
  "SwapDebugOverlay",
  "PerfTelemetry",
  "DumpFlowShaderSource")
foreach ($key in $releaseOffKeys) {
  $actual = Get-IniValue $iniMap $key
  if ($ReleaseStrict) {
    Add-IniExpectation $iniMap $key "0" "FAIL"
  } elseif ($actual -and $actual -ne "0") {
    Add-Check "INI $key" "WARN" "Diagnostic/support setting is enabled: $actual."
  }
}

if ($ReleaseStrict) {
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
    Add-IniExpectation $iniMap $releaseKey.Name $releaseKey.Value "FAIL"
  }
}

foreach ($logToggle in @(
    (Join-Path $gameFull "logs.txt"),
    (Join-Path $supportDir "logs.txt"))) {
  if (Test-Path $logToggle) {
    Add-Check "logs.txt" "WARN" "Runtime logging toggle exists at $logToggle."
  }
}

Write-ValidationReport $gameFull $iniFull

$failCount = @($checks | Where-Object { $_.Status -eq "FAIL" }).Count
if ($failCount -gt 0) {
  exit 1
}
