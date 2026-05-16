param(
  [string]$Zig = $env:ZIG,
  [string]$Out = "build\OpenGL32.dll",
  [string]$GameDir = "D:\GTA4\Tomb Raider I-III Remastered (2024)\Tomb Raider I-III Remastered",
  [string]$ForwardSource,
  [string[]]$ExtraCFlags = @()
)

$ErrorActionPreference = "Stop"

if (-not $Zig) {
  foreach ($localZig in @(
    "C:\zig\zig.exe",
    "C:\codex-zig\zig-windows-x86_64-0.13.0\zig.exe"
  )) {
    if (Test-Path $localZig) {
      $Zig = $localZig
      break
    }
  }
  if (-not $Zig -and (Test-Path "C:\codex-zig")) {
    $localZig = Get-ChildItem -LiteralPath "C:\codex-zig" -Filter zig.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($localZig) { $Zig = $localZig.FullName }
  }
  if (-not $Zig) {
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
$stubPath = Join-Path $outDir "tr456_water_forward_stubs.s"
$exports = Get-PeExportNames $ForwardSource
$hookedExports = @(
  "wglGetProcAddress",
  "wglSwapBuffers",
  "wglSwapLayerBuffers",
  "glBindTexture",
  "glViewport",
  "glEnable",
  "glDisable",
  "glDepthMask",
  "glDepthFunc",
  "glBlendFunc",
  "glDrawArrays",
  "glDrawElements",
  "glCompressedTexImage2D",
  "glCompressedTexSubImage2D",
  "glCompressedTexImage3D",
  "glCompressedTexSubImage3D",
  "glCompressedTextureSubImage2D",
  "glCompressedTextureSubImage3D",
  "glCompressedTextureImage2DEXT",
  "glCompressedTextureImage3DEXT",
  "glCompressedTextureSubImage2DEXT",
  "glCompressedTextureSubImage3DEXT",
  "glTexImage3D",
  "glTexSubImage3D",
  "glTextureSubImage3D",
  "glTextureImage3DEXT",
  "glTextureSubImage3DEXT"
)
$def = New-Object System.Collections.Generic.List[string]
$def.Add('LIBRARY "OpenGL32.dll"')
$def.Add('EXPORTS')
foreach ($name in $exports) {
  $def.Add("  $name")
}
[IO.File]::WriteAllLines($defPath, $def.ToArray(), [Text.Encoding]::ASCII)

$stub = New-Object System.Collections.Generic.List[string]
$stub.Add(".text")
$stub.Add(".intel_syntax noprefix")
$stub.Add(".extern old_proc")
foreach ($name in $exports) {
  if ($name -in $hookedExports) {
    continue
  }
  if ($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$') {
    throw "Cannot generate forward stub for unsupported export name: $name"
  }
  $label = "tr456_forward_name_$name"
  $stub.Add(".global $name")
  $stub.Add(".def $name; .scl 2; .type 32; .endef")
  $stub.Add("$name" + ":")
  $stub.Add("  sub rsp, 168")
  $stub.Add("  mov [rsp+32], rcx")
  $stub.Add("  mov [rsp+40], rdx")
  $stub.Add("  mov [rsp+48], r8")
  $stub.Add("  mov [rsp+56], r9")
  $stub.Add("  movdqu [rsp+64], xmm0")
  $stub.Add("  movdqu [rsp+80], xmm1")
  $stub.Add("  movdqu [rsp+96], xmm2")
  $stub.Add("  movdqu [rsp+112], xmm3")
  $stub.Add("  lea rcx, [rip + $label]")
  $stub.Add("  call old_proc")
  $stub.Add("  mov r10, rax")
  $stub.Add("  mov rcx, [rsp+32]")
  $stub.Add("  mov rdx, [rsp+40]")
  $stub.Add("  mov r8, [rsp+48]")
  $stub.Add("  mov r9, [rsp+56]")
  $stub.Add("  movdqu xmm0, [rsp+64]")
  $stub.Add("  movdqu xmm1, [rsp+80]")
  $stub.Add("  movdqu xmm2, [rsp+96]")
  $stub.Add("  movdqu xmm3, [rsp+112]")
  $stub.Add("  add rsp, 168")
  $stub.Add("  test r10, r10")
  $stub.Add("  je tr456_forward_missing_$name")
  $stub.Add("  jmp r10")
  $stub.Add("tr456_forward_missing_$name" + ":")
  $stub.Add("  ret")
  $stub.Add("")
  $stub.Add(".section .rdata," + '"dr"')
  $stub.Add("$label" + ":")
  $stub.Add("  .asciz " + '"' + $name + '"')
  $stub.Add(".text")
}
[IO.File]::WriteAllLines($stubPath, $stub.ToArray(), [Text.Encoding]::ASCII)

& $Zig cc `
  -target x86_64-windows-gnu `
  -O2 `
  -shared `
  -Wall `
  -Wextra `
  @ExtraCFlags `
  -o $outPath `
  $src `
  $stubPath `
  $defPath `
  -lkernel32

if ($LASTEXITCODE -ne 0) {
  throw "zig cc failed with exit code $LASTEXITCODE"
}

Write-Host "Built $outPath"
