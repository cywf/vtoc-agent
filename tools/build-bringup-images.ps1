[CmdletBinding()]
param(
    [ValidateSet('base', 'recovery', 'all')]
    [string]$Role = 'all'
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$project = '/workspace/firmware/esp-idf'
$image = (Get-Content (Join-Path $repoRoot 'toolchain.lock.json') -Raw | ConvertFrom-Json).espIdf.image
$artifacts = Join-Path $repoRoot 'artifacts'
New-Item -ItemType Directory -Force -Path $artifacts | Out-Null
$script:bundleIndex = @()

function Build-Image([string]$imageRole) {
    $buildDir = "build-$imageRole"
    $defaults = "sdkconfig.defaults;sdkconfig.$imageRole.defaults"
    $sdkconfig = "$buildDir/sdkconfig"
    $idfArgs = "-B $buildDir -D SDKCONFIG=$sdkconfig -D SDKCONFIG_DEFAULTS='$defaults'"
    docker run --rm -v "${repoRoot}:/workspace" -w $project $image bash -lc "idf.py $idfArgs fullclean && idf.py $idfArgs build"
    $buildRoot = Join-Path $repoRoot "firmware/esp-idf/$buildDir"
    $bundleRoot = Join-Path $artifacts "vtoc-agent-0.1.0-bringup-$imageRole"
    if (Test-Path $bundleRoot) {
        Remove-Item -LiteralPath $bundleRoot -Recurse -Force
    }
    New-Item -ItemType Directory -Force -Path (Join-Path $bundleRoot 'bootloader'), (Join-Path $bundleRoot 'partition_table') | Out-Null

    $bundleFiles = @(
        [ordered]@{ offset = '0x0'; path = 'bootloader/bootloader.bin'; source = (Join-Path $buildRoot 'bootloader/bootloader.bin') },
        [ordered]@{ offset = '0x8000'; path = 'partition_table/partition-table.bin'; source = (Join-Path $buildRoot 'partition_table/partition-table.bin') },
        [ordered]@{ offset = '0x10000'; path = 'vtoc_agent_bringup.bin'; source = (Join-Path $buildRoot 'vtoc_agent_bringup.bin') },
        [ordered]@{ offset = $null; path = 'flash_args.generated'; source = (Join-Path $buildRoot 'flash_args') },
        [ordered]@{ offset = $null; path = 'flasher_args.generated.json'; source = (Join-Path $buildRoot 'flasher_args.json') }
    )
    foreach ($bundleFile in $bundleFiles) {
        if (-not (Test-Path $bundleFile.source)) {
            throw "Expected bundle file was not produced for role ${imageRole}: $($bundleFile.path)."
        }
        Copy-Item -LiteralPath $bundleFile.source -Destination (Join-Path $bundleRoot $bundleFile.path) -Force
        $bundleFile.Remove('source')
        $bundleFile.sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $bundleRoot $bundleFile.path)).Hash
    }

    $bundleManifest = [ordered]@{
        schemaVersion = 1
        board = 'Heltec WiFi LoRa 32 V3'
        imageRole = $imageRole
        imageVersion = '0.1.0-bringup'
        espIdfVersion = 'v5.3.1'
        flashArguments = 'flash_args.generated'
        esptoolArguments = 'flasher_args.generated.json'
        files = $bundleFiles
    }
    $bundleManifestPath = Join-Path $bundleRoot 'bundle-manifest.json'
    $bundleManifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $bundleManifestPath -Encoding UTF8
    $script:bundleIndex += [ordered]@{ imageRole = $imageRole; manifest = "$(Split-Path $bundleRoot -Leaf)/bundle-manifest.json" }
    Write-Output "Created deterministic $imageRole flash bundle at $bundleRoot"
}

if ($Role -in @('base', 'all')) { Build-Image 'base' }
if ($Role -in @('recovery', 'all')) { Build-Image 'recovery' }

if ($Role -eq 'all') {
    [ordered]@{
        schemaVersion = 1
        board = 'Heltec WiFi LoRa 32 V3'
        imageVersion = '0.1.0-bringup'
        bundles = $bundleIndex
    } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $artifacts 'release-manifest-input.json') -Encoding UTF8
}
