param(
  [string]$IniPath = "tr456_water.ini",
  [string]$PackageIniPath = "profiles\tr456_water.release.ini",
  [string]$DocsPath = "INI_SETTINGS.md",
  [string]$OwnershipPath = "docs\SETTINGS_OWNERSHIP.csv",
  [switch]$Markdown,
  [switch]$FailOnPackageDrift,
  [switch]$FailOnUnclassified
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot

function Resolve-RepoPath($Path) {
  if ([IO.Path]::IsPathRooted($Path)) {
    return $Path
  }
  return Join-Path $root $Path
}

function New-KeyRecord($Key) {
  [pscustomobject]@{
    Key = $Key
    IniValue = ""
    IniLine = 0
    PackageIniValue = ""
    PackageIniLine = 0
    DocsValue = ""
    DocsLine = 0
    CodeTypes = New-Object System.Collections.Generic.List[string]
    CodeDefaults = New-Object System.Collections.Generic.List[string]
    CodeRefs = New-Object System.Collections.Generic.List[string]
    CodePattern = ""
    PackageValue = ""
    OwnershipVisibility = ""
    OwnershipOwner = ""
    OwnershipNotes = ""
  }
}

function Get-Record($Map, $Key) {
  if (-not $Map.Contains($Key)) {
    $Map[$Key] = New-KeyRecord $Key
  }
  return $Map[$Key]
}

function Add-Unique($List, $Value) {
  if ($Value -and -not $List.Contains($Value)) {
    [void]$List.Add($Value)
  }
}

$iniFull = Resolve-RepoPath $IniPath
$packageIniFull = Resolve-RepoPath $PackageIniPath
$docsFull = Resolve-RepoPath $DocsPath
$ownershipFull = Resolve-RepoPath $OwnershipPath
$packageFull = Join-Path $root "tools\package_nexus_release.ps1"
$srcDir = Join-Path $root "src"

foreach ($required in @($iniFull, $packageIniFull, $docsFull, $ownershipFull, $packageFull, $srcDir)) {
  if (-not (Test-Path $required)) {
    throw "Required input not found: $required"
  }
}

$records = [ordered]@{}
$codePatterns = New-Object System.Collections.Generic.List[object]

$lineNo = 0
foreach ($line in Get-Content -LiteralPath $iniFull) {
  $lineNo++
  if ($line -match '^\s*([A-Za-z0-9_]+)\s*=\s*(.*?)\s*(?:[;#].*)?$') {
    $rec = Get-Record $records $Matches[1]
    $rec.IniValue = $Matches[2].Trim()
    $rec.IniLine = $lineNo
  }
}

$lineNo = 0
foreach ($line in Get-Content -LiteralPath $packageIniFull) {
  $lineNo++
  if ($line -match '^\s*([A-Za-z0-9_]+)\s*=\s*(.*?)\s*(?:[;#].*)?$') {
    $rec = Get-Record $records $Matches[1]
    $rec.PackageIniValue = $Matches[2].Trim()
    $rec.PackageIniLine = $lineNo
  }
}

$lineNo = 0
foreach ($line in Get-Content -LiteralPath $docsFull) {
  $lineNo++
  if ($line -match '^\|\s*`([^`]+)`\s*\|\s*`?([^|`]+?)`?\s*\|') {
    $key = $Matches[1].Trim()
    if ($key -eq "---") {
      continue
    }
    $rec = Get-Record $records $key
    $rec.DocsValue = $Matches[2].Trim()
    $rec.DocsLine = $lineNo
  }
}

foreach ($row in Import-Csv -LiteralPath $ownershipFull) {
  if (-not $row.Key) {
    continue
  }
  $key = $row.Key.Trim()
  if (-not $key) {
    continue
  }
  $rec = Get-Record $records $key
  if ($row.Visibility) {
    $rec.OwnershipVisibility = $row.Visibility.Trim()
  }
  if ($row.Owner) {
    $rec.OwnershipOwner = $row.Owner.Trim()
  }
  if ($row.Notes) {
    $rec.OwnershipNotes = $row.Notes.Trim()
  }
}

$sourceFiles = Get-ChildItem -LiteralPath $srcDir -Recurse -File |
  Where-Object { $_.Name -match '\.(c|h|inc)$' } |
  Sort-Object FullName

foreach ($file in $sourceFiles) {
  $relative = Resolve-Path -LiteralPath $file.FullName -Relative
  $lineNo = 0
  foreach ($line in Get-Content -LiteralPath $file.FullName) {
    $lineNo++
    foreach ($match in [regex]::Matches($line, 'ini_(int|float|string)\s*\(\s*"([^"]+)"')) {
      $kind = $match.Groups[1].Value
      $key = $match.Groups[2].Value
      $default = ""
      $escapedKey = [regex]::Escape($key)
      $defaultPattern = 'ini_' + $kind + '\s*\(\s*"' + $escapedKey +
        '"\s*,\s*([^,\)\r\n]+)'
      $defaultMatch = [regex]::Match($line, $defaultPattern)
      if ($defaultMatch.Success) {
        $default = $defaultMatch.Groups[1].Value.Trim()
      }

      if ($key -like "*%d*") {
        [void]$codePatterns.Add([pscustomobject]@{
          Pattern = $key
          Kind = $kind
          Default = $default
          Ref = "${relative}:$lineNo"
        })
        continue
      }

      $rec = Get-Record $records $key
      Add-Unique $rec.CodeTypes $kind
      Add-Unique $rec.CodeDefaults $default
      Add-Unique $rec.CodeRefs "${relative}:$lineNo"
    }
    foreach ($match in [regex]::Matches($line, '"([A-Za-z0-9_]+%d[A-Za-z0-9_]*)"')) {
      [void]$codePatterns.Add([pscustomobject]@{
        Pattern = $match.Groups[1].Value
        Kind = "pattern"
        Default = ""
        Ref = "${relative}:$lineNo"
      })
    }
  }
}

foreach ($pattern in $codePatterns) {
  $regexPattern = '^' + [regex]::Escape($pattern.Pattern).Replace('%d', '\d+') + '$'
  foreach ($rec in $records.Values) {
    if ($rec.Key -match $regexPattern) {
      Add-Unique $rec.CodeTypes $pattern.Kind
      Add-Unique $rec.CodeDefaults $pattern.Default
      Add-Unique $rec.CodeRefs $pattern.Ref
      $rec.CodePattern = $pattern.Pattern
    }
  }
}

$inReleaseOff = $false
foreach ($line in Get-Content -LiteralPath $packageFull) {
  if ($line -match 'foreach \(\$releaseOffKey in @\(') {
    $inReleaseOff = $true
    continue
  }
  if ($inReleaseOff -and $line -match '\)\) \{') {
    $inReleaseOff = $false
  }
  if ($inReleaseOff -and $line -match '"([^"]+)"') {
    $rec = Get-Record $records $Matches[1]
    $rec.PackageValue = "0"
  }
  if ($line -match '@\{\s*Name\s*=\s*"([^"]+)"\s*;\s*Value\s*=\s*"([^"]+)"\s*\}') {
    $rec = Get-Record $records $Matches[1]
    $rec.PackageValue = $Matches[2]
  }
}

$rows = @($records.Values | Sort-Object Key)
$activeIni = @($rows | Where-Object { $_.IniLine -gt 0 })
$releaseProfile = @($rows | Where-Object { $_.PackageIniLine -gt 0 })
$documented = @($rows | Where-Object { $_.DocsLine -gt 0 })
$codeUsed = @($rows | Where-Object { $_.CodeRefs.Count -gt 0 })
$packageExpected = @($rows | Where-Object { $_.PackageValue })
$ownershipClassified = @($rows | Where-Object { $_.OwnershipVisibility })
$internalCodeOnly = @($rows | Where-Object {
    $_.CodeRefs.Count -gt 0 -and $_.IniLine -eq 0 -and
    $_.PackageIniLine -eq 0 -and $_.OwnershipVisibility -eq "internal"
  })
$releaseOnlyOwned = @($rows | Where-Object {
    $_.OwnershipVisibility -eq "release-only"
  })

$iniNotDocumented = @($rows | Where-Object { $_.IniLine -gt 0 -and $_.DocsLine -eq 0 })
$releaseNotDocumented = @($rows | Where-Object {
    $_.PackageIniLine -gt 0 -and $_.DocsLine -eq 0
  })
$docsNotProfile = @($rows | Where-Object {
    $_.DocsLine -gt 0 -and $_.IniLine -eq 0 -and $_.PackageIniLine -eq 0
  })
$iniNotCode = @($rows | Where-Object {
    $_.IniLine -gt 0 -and $_.CodeRefs.Count -eq 0 -and -not $_.PackageValue
  })
$codeNotDocs = @($rows | Where-Object {
    $_.CodeRefs.Count -gt 0 -and $_.DocsLine -eq 0 -and
    -not $_.OwnershipVisibility
  })
$codeNotProfile = @($rows | Where-Object {
    $_.CodeRefs.Count -gt 0 -and $_.IniLine -eq 0 -and
    $_.PackageIniLine -eq 0 -and -not $_.OwnershipVisibility
  })
$ownershipNotUsed = @($rows | Where-Object {
    $_.OwnershipVisibility -and $_.CodeRefs.Count -eq 0 -and
    $_.IniLine -eq 0 -and $_.PackageIniLine -eq 0 -and $_.DocsLine -eq 0
  })
$packageDrift = @($packageExpected | Where-Object {
    $_.PackageIniLine -eq 0 -or $_.PackageIniValue -ne $_.PackageValue
  })

function Join-Keys($Items) {
  if (-not $Items -or $Items.Count -eq 0) {
    return "_none_"
  }
  return ($Items | ForEach-Object { "``$($_.Key)``" }) -join ", "
}

function Escape-MarkdownCell($Text) {
  if ($null -eq $Text -or $Text -eq "") {
    return ""
  }
  return (($Text -replace '\|', '\|') -replace "`r?`n", " ")
}

if ($Markdown) {
  Write-Output "# INI Settings Inventory Audit"
  Write-Output ""
  Write-Output ('Generated from active INI `{0}`, release profile `{1}`, `{2}`, ownership `{3}`, `src`, and `tools/package_nexus_release.ps1`.' -f $IniPath,$PackageIniPath,$DocsPath,$OwnershipPath)
  Write-Output ""
  Write-Output "## Summary"
  Write-Output ""
  Write-Output "| Source | Count |"
  Write-Output "| --- | ---: |"
  Write-Output "| Active INI keys | $($activeIni.Count) |"
  Write-Output "| Release profile keys | $($releaseProfile.Count) |"
  Write-Output "| Documented keys | $($documented.Count) |"
  Write-Output "| Code-referenced keys | $($codeUsed.Count) |"
  Write-Output "| Ownership-classified keys | $($ownershipClassified.Count) |"
  Write-Output "| Internal code-only keys | $($internalCodeOnly.Count) |"
  Write-Output "| Release-only owned keys | $($releaseOnlyOwned.Count) |"
  Write-Output "| Release package expectations | $($packageExpected.Count) |"
  Write-Output ""
  Write-Output "## Drift"
  Write-Output ""
  Write-Output "- INI keys not documented: $(Join-Keys $iniNotDocumented)"
  Write-Output "- Release profile keys not documented: $(Join-Keys $releaseNotDocumented)"
  Write-Output "- Documented keys missing from active/release profiles: $(Join-Keys $docsNotProfile)"
  Write-Output "- Active INI keys not found in code references: $(Join-Keys $iniNotCode)"
  Write-Output "- Unclassified code keys not documented: $(Join-Keys $codeNotDocs)"
  Write-Output "- Unclassified code keys missing from active/release profiles: $(Join-Keys $codeNotProfile)"
  Write-Output "- Ownership entries not found anywhere: $(Join-Keys $ownershipNotUsed)"
  Write-Output ""
  Write-Output "## Ownership"
  Write-Output ""
  Write-Output "| Visibility | Count |"
  Write-Output "| --- | ---: |"
  foreach ($group in ($ownershipClassified | Group-Object OwnershipVisibility | Sort-Object Name)) {
    Write-Output "| $($group.Name) | $($group.Count) |"
  }
  Write-Output ""
  Write-Output "## Release Package Drift"
  Write-Output ""
  if ($packageDrift.Count -eq 0) {
    Write-Output "_none_"
  } else {
    Write-Output "| Key | Release profile | Package expects |"
    Write-Output "| --- | --- | --- |"
    foreach ($rec in $packageDrift) {
      $release = if ($rec.PackageIniLine -gt 0) { $rec.PackageIniValue } else { "_missing_" }
      Write-Output ('| `{0}` | `{1}` | `{2}` |' -f
        $rec.Key,(Escape-MarkdownCell $release),
        (Escape-MarkdownCell $rec.PackageValue))
    }
  }
  Write-Output ""
  Write-Output "## Inventory"
  Write-Output ""
  Write-Output "| Key | Active INI | Release profile | Docs | Code default(s) | Package | Ownership | Notes |"
  Write-Output "| --- | --- | --- | --- | --- | --- | --- | --- |"
  foreach ($rec in $rows) {
    $ini = if ($rec.IniLine -gt 0) { ('`{0}`' -f $rec.IniValue) } else { "" }
    $release = if ($rec.PackageIniLine -gt 0) { ('`{0}`' -f $rec.PackageIniValue) } else { "" }
    $docs = if ($rec.DocsLine -gt 0) { ('`{0}`' -f $rec.DocsValue) } else { "" }
    $defaults = if ($rec.CodeDefaults.Count -gt 0) {
      ($rec.CodeDefaults | ForEach-Object { "``$_``" }) -join ", "
    } else {
      ""
    }
    $package = if ($rec.PackageValue) { ('`{0}`' -f $rec.PackageValue) } else { "" }
    $ownership = ""
    if ($rec.OwnershipVisibility) {
      $ownership = ('`{0}`' -f $rec.OwnershipVisibility)
      if ($rec.OwnershipOwner) {
        $ownership = ('{0} / `{1}`' -f $ownership,$rec.OwnershipOwner)
      }
    }
    $notes = New-Object System.Collections.Generic.List[string]
    if ($rec.CodePattern) { [void]$notes.Add(('code pattern `{0}`' -f $rec.CodePattern)) }
    if ($rec.CodeRefs.Count -gt 0) {
      $firstRef = $rec.CodeRefs[0]
      [void]$notes.Add(('code: `{0}`' -f $firstRef))
    }
    if ($rec.OwnershipNotes) {
      [void]$notes.Add((Escape-MarkdownCell $rec.OwnershipNotes))
    }
    Write-Output ('| `{0}` | {1} | {2} | {3} | {4} | {5} | {6} | {7} |' -f
      $rec.Key,$ini,$release,$docs,$defaults,$package,$ownership,
      ($notes -join '; '))
  }
} else {
  Write-Output "INI settings inventory audit"
  Write-Output "Active INI: $IniPath"
  Write-Output "Release profile: $PackageIniPath"
  Write-Output "Ownership map: $OwnershipPath"
  Write-Output "Active INI keys: $($activeIni.Count)"
  Write-Output "Release profile keys: $($releaseProfile.Count)"
  Write-Output "Documented keys: $($documented.Count)"
  Write-Output "Code-referenced keys: $($codeUsed.Count)"
  Write-Output "Ownership-classified keys: $($ownershipClassified.Count)"
  Write-Output "Internal code-only keys: $($internalCodeOnly.Count)"
  Write-Output "Release-only owned keys: $($releaseOnlyOwned.Count)"
  Write-Output "Release package expectations: $($packageExpected.Count)"
  Write-Output ""
  Write-Output "INI keys not documented: $(($iniNotDocumented | ForEach-Object Key) -join ', ')"
  Write-Output "Release profile keys not documented: $(($releaseNotDocumented | ForEach-Object Key) -join ', ')"
  Write-Output "Documented keys missing from active/release profiles: $(($docsNotProfile | ForEach-Object Key) -join ', ')"
  Write-Output "Active INI keys not found in code references: $(($iniNotCode | ForEach-Object Key) -join ', ')"
  Write-Output "Unclassified code keys not documented: $(($codeNotDocs | ForEach-Object Key) -join ', ')"
  Write-Output "Unclassified code keys missing from active/release profiles: $(($codeNotProfile | ForEach-Object Key) -join ', ')"
  Write-Output "Ownership entries not found anywhere: $(($ownershipNotUsed | ForEach-Object Key) -join ', ')"
  Write-Output ""
  if ($packageDrift.Count -gt 0) {
    Write-Output "Release package drift:"
    foreach ($rec in $packageDrift) {
      $release = if ($rec.PackageIniLine -gt 0) { $rec.PackageIniValue } else { "<missing>" }
      Write-Output "  $($rec.Key): release profile=$release package=$($rec.PackageValue)"
    }
  } else {
    Write-Output "Release package drift: none"
  }
}

if ($FailOnPackageDrift -and $packageDrift.Count -gt 0) {
  throw "Package expectations differ from release profile ($($packageDrift.Count) key(s))."
}

if ($FailOnUnclassified -and
    ($codeNotDocs.Count -gt 0 -or $codeNotProfile.Count -gt 0 -or
     $ownershipNotUsed.Count -gt 0)) {
  throw "Unclassified or stale setting ownership entries found."
}
