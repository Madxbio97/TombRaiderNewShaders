param(
  [string]$Zig = $env:ZIG,
  [string]$Out = "build\OpenGL32.dll",
  [string]$GameDir = "G:\SteamLibrary\steamapps\common\Tomb Raider IV-VI Remastered",
  [string]$ForwardSource
)

$ErrorActionPreference = "Stop"

if (-not $Zig) {
  $localZig = "C:\zig\zig.exe"
  if (Test-Path $localZig) {
    $Zig = $localZig
  } else {
    $cmd = Get-Command zig -ErrorAction SilentlyContinue
    if ($cmd) { $Zig = $cmd.Source }
  }
}

if (-not $Zig -or -not (Test-Path $Zig)) {
  throw "Zig compiler not found. Pass -Zig path\to\zig.exe or set ZIG."
}

$root = Split-Path -Parent $PSScriptRoot
$src = Join-Path $root "src\tr456_water_proxy.c"
$outPath = Join-Path $root $Out
$outDir = Split-Path -Parent $outPath
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

function Read-U16($Bytes, $Offset) { [BitConverter]::ToUInt16($Bytes, $Offset) }
function Read-U32($Bytes, $Offset) { [BitConverter]::ToUInt32($Bytes, $Offset) }
function Get-PeExportNames($Path) {
  $bytes = [IO.File]::ReadAllBytes($Path)
  $pe = Read-U32 $bytes 0x3c
  $optional = $pe + 24
  $magic = Read-U16 $bytes $optional
  $dataDirectory = $optional + $(if ($magic -eq 0x20b) { 112 } else { 96 })
  $exportRva = Read-U32 $bytes $dataDirectory
  if ($exportRva -eq 0) { throw "No export table in $Path" }
  $sectionCount = Read-U16 $bytes ($pe + 6)
  $sectionOffset = $optional + (Read-U16 $bytes ($pe + 20))
  $sections = @()
  for ($i = 0; $i -lt $sectionCount; $i++) {
    $o = $sectionOffset + $i * 40
    $sections += [pscustomobject]@{
      VirtualAddress = Read-U32 $bytes ($o + 12)
      VirtualSize = Read-U32 $bytes ($o + 8)
      RawPointer = Read-U32 $bytes ($o + 20)
      RawSize = Read-U32 $bytes ($o + 16)
    }
  }
  function RvaToOffset($Rva) {
    foreach ($s in $sections) {
      $size = [Math]::Max($s.VirtualSize, $s.RawSize)
      if ($Rva -ge $s.VirtualAddress -and $Rva -lt ($s.VirtualAddress + $size)) {
        return $s.RawPointer + ($Rva - $s.VirtualAddress)
      }
    }
    throw "RVA not mapped: 0x$($Rva.ToString('x'))"
  }
  $exportOffset = RvaToOffset $exportRva
  $nameCount = Read-U32 $bytes ($exportOffset + 24)
  $namesOffset = RvaToOffset (Read-U32 $bytes ($exportOffset + 32))
  $names = @()
  for ($i = 0; $i -lt $nameCount; $i++) {
    $nameOffset = RvaToOffset (Read-U32 $bytes ($namesOffset + $i * 4))
    $chars = New-Object System.Collections.Generic.List[byte]
    for ($p = $nameOffset; $bytes[$p] -ne 0; $p++) { $chars.Add($bytes[$p]) }
    $names += [Text.Encoding]::ASCII.GetString($chars.ToArray())
  }
  $names | Sort-Object -Unique
}

if (-not $ForwardSource -or -not (Test-Path $ForwardSource)) {
  if (-not (Test-Path $GameDir)) {
    throw "Game directory not found: $GameDir"
  }

  $forwardCandidates = @(
    (Join-Path $GameDir "tr456_water\OpenGL32.dll.tr456-prev.bak"),
    (Join-Path $GameDir "OpenGL32.dll.tr456-prev.bak"),
    (Join-Path $GameDir "OpenGL32_orig.dll"),
    (Join-Path $GameDir "OpenGL32.dll"),
    (Join-Path $env:WINDIR "System32\opengl32.dll")
  )

  foreach ($candidate in $forwardCandidates) {
    if (Test-Path $candidate) {
      $ForwardSource = $candidate
      break
    }
  }
}

if (-not $ForwardSource -or -not (Test-Path $ForwardSource)) {
  throw "Forward source DLL not found. Pass -ForwardSource path\to\OpenGL32.dll or set -GameDir."
}

Write-Host "Using export source $ForwardSource"

$defPath = Join-Path $outDir "tr456_water_proxy.def"
$exports = Get-PeExportNames $ForwardSource
$def = New-Object System.Collections.Generic.List[string]
$def.Add('LIBRARY "OpenGL32.dll"')
$def.Add('EXPORTS')
foreach ($name in $exports) {
  if ($name -in @("wglGetProcAddress", "wglSwapBuffers", "wglSwapLayerBuffers", "glDrawArrays", "glDrawElements")) {
    $def.Add("  $name")
  } else {
    $def.Add("  $name=OpenGL32_orig.$name")
  }
}
[IO.File]::WriteAllLines($defPath, $def.ToArray(), [Text.Encoding]::ASCII)

& $Zig cc `
  -target x86_64-windows-gnu `
  -O2 `
  -shared `
  -Wall `
  -Wextra `
  -o $outPath `
  $src `
  $defPath `
  -lkernel32

if ($LASTEXITCODE -ne 0) {
  throw "zig cc failed with exit code $LASTEXITCODE"
}

Write-Host "Built $outPath"
