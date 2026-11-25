param(
    [Parameter(Mandatory = $true)]
    [string]$Root,
    [Parameter(Mandatory = $true)]
    [ValidateSet("ARM64", "AMD64", "I386", "ARM64EC")]
    [string]$Expected
)

$machines = @{
    ARM64   = [uint16]0xAA64
    AMD64   = [uint16]0x8664
    I386    = [uint16]0x014C
    ARM64EC = [uint16]0xA641
}
$expectedMachine = $machines[$Expected]

function Read-PeIdentity([string]$Path) {
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 0x40) { throw "File is too small to be a PE." }
    $peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
    if ($peOffset -le 0 -or ($peOffset + 24) -gt $bytes.Length) { throw "PE header is outside the file." }
    $signature = [BitConverter]::ToUInt32($bytes, $peOffset)
    if ($signature -ne 0x00004550) { throw "Not a PE file." }
    $machine = [BitConverter]::ToUInt16($bytes, $peOffset + 4)
    $sectionCount = [BitConverter]::ToUInt16($bytes, $peOffset + 6)
    $optionalSize = [BitConverter]::ToUInt16($bytes, $peOffset + 20)
    $optional = $peOffset + 24
    $magic = [BitConverter]::ToUInt16($bytes, $optional)
    $dataDir = if ($magic -eq 0x20B) { $optional + 112 } else { $optional + 96 }
    # IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG is directory 10.
    $loadRva = [BitConverter]::ToUInt32($bytes, $dataDir + (10 * 8))
    $chpe = [uint64]0
    if ($loadRva -ne 0) {
        $sectionTable = $optional + $optionalSize
        $loadOff = 0
        for ($i = 0; $i -lt $sectionCount; $i++) {
            $section = $sectionTable + ($i * 40)
            $virtualAddress = [BitConverter]::ToUInt32($bytes, $section + 12)
            $virtualSize = [BitConverter]::ToUInt32($bytes, $section + 8)
            $rawPointer = [BitConverter]::ToUInt32($bytes, $section + 20)
            if ($loadRva -ge $virtualAddress -and $loadRva -lt ($virtualAddress + $virtualSize)) {
                $loadOff = $rawPointer + ($loadRva - $virtualAddress)
                break
            }
        }
        # CHPEMetadataPointer is at offset 200 of IMAGE_LOAD_CONFIG_DIRECTORY64.
        if ($loadOff -gt 0 -and ($loadOff + 208) -le $bytes.Length) {
            $configSize = [BitConverter]::ToUInt32($bytes, $loadOff)
            if ($configSize -ge 208) {
                $chpe = [BitConverter]::ToUInt64($bytes, $loadOff + 200)
            }
        }
    }
    return @{ Machine = $machine; Chpe = $chpe }
}

if (-not (Test-Path -LiteralPath $Root)) {
    throw "Output directory does not exist: $Root"
}

$files = @(Get-ChildItem -LiteralPath $Root -Recurse -File | Where-Object { $_.Extension -in ".exe", ".dll" })
if ($files.Count -eq 0) {
    throw "No exe or dll files under $Root"
}

$failed = $false
foreach ($file in $files) {
    $id = Read-PeIdentity $file.FullName
    $machine = [uint16]$id.Machine
    $name = $machines.GetEnumerator() | Where-Object { $_.Value -eq $machine } | Select-Object -ExpandProperty Key -First 1
    if (-not $name) { $name = ("0x{0:X4}" -f $machine) }
    $isArm64X = ($machine -eq $machines.AMD64 -and $id.Chpe -ne 0)
    $ok = $false
    if ($Expected -eq "ARM64") {
        $ok = ($machine -eq $machines.ARM64)
    }
    elseif ($Expected -eq "ARM64EC") {
        # The linker emits an ARM64X image. Its COFF machine is AMD64 and
        # IMAGE_LOAD_CONFIG_DIRECTORY64.CHPEMetadataPointer is set.
        # Object files use 0xA641, but a linked exe/dll does not.
        $ok = $isArm64X -or ($machine -eq $machines.ARM64EC)
    }
    elseif ($Expected -eq "AMD64") {
        $ok = ($machine -eq $machines.AMD64 -and -not $isArm64X)
    }
    else {
        $ok = ($machine -eq $expectedMachine)
    }
    $suffix = ""
    if ($isArm64X) { $suffix = " ARM64X" }
    if (-not $ok) {
        Write-Host "FAIL $($file.FullName) machine=$name$suffix"
        $failed = $true
    }
    else {
        Write-Host "OK   $($file.Name) machine=$name$suffix"
    }
}

if ($failed) {
    throw "One or more binaries are not $Expected."
}
