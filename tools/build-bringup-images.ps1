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

function Build-Image([string]$imageRole) {
    $buildDir = "build-$imageRole"
    $defaults = "sdkconfig.defaults;sdkconfig.$imageRole.defaults"
    $sdkconfig = "$buildDir/sdkconfig"
    $idfArgs = "-B $buildDir -D SDKCONFIG=$sdkconfig -D SDKCONFIG_DEFAULTS='$defaults'"
    docker run --rm -v "${repoRoot}:/workspace" -w $project $image bash -lc "idf.py $idfArgs fullclean && idf.py $idfArgs build"
    $binary = Join-Path $repoRoot "firmware/esp-idf/$buildDir/vtoc_agent_bringup.bin"
    if (-not (Test-Path $binary)) {
        throw "Expected image was not produced for role $imageRole."
    }
    $destination = Join-Path $artifacts "vtoc-agent-0.1.0-bringup-$imageRole.bin"
    Copy-Item -LiteralPath $binary -Destination $destination -Force
    Get-FileHash -Algorithm SHA256 -LiteralPath $destination
}

if ($Role -in @('base', 'all')) { Build-Image 'base' }
if ($Role -in @('recovery', 'all')) { Build-Image 'recovery' }
