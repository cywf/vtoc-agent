[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path

Push-Location $repoRoot
try {
    cargo fmt --manifest-path firmware/Cargo.toml -- --check
    cargo clippy --manifest-path firmware/Cargo.toml -- -D warnings
    cargo test --manifest-path firmware/Cargo.toml --locked
    Get-ChildItem protocol/v1 -Recurse -Filter *.json | ForEach-Object {
        Get-Content -LiteralPath $_.FullName -Raw | ConvertFrom-Json | Out-Null
    }
    & (Join-Path $PSScriptRoot 'build-bringup-images.ps1') -Role all
    $releaseInput = Get-Content -LiteralPath (Join-Path $repoRoot 'artifacts/release-manifest-input.json') -Raw | ConvertFrom-Json
    if ($releaseInput.board -ne 'Heltec WiFi LoRa 32 V3' -or $releaseInput.bundles.Count -ne 2) {
        throw 'Release manifest input is incomplete.'
    }
    foreach ($bundleReference in $releaseInput.bundles) {
        $manifestPath = Join-Path (Join-Path $repoRoot 'artifacts') $bundleReference.manifest
        $bundle = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
        if ($bundle.imageRole -ne $bundleReference.imageRole -or $bundle.files.Count -ne 5) {
            throw "Bundle manifest is incomplete for role $($bundleReference.imageRole)."
        }
        foreach ($file in $bundle.files) {
            $path = Join-Path (Split-Path $manifestPath -Parent) $file.path
            if (-not (Test-Path $path) -or (Get-FileHash -Algorithm SHA256 -LiteralPath $path).Hash -ne $file.sha256) {
                throw "Bundle checksum verification failed for role $($bundleReference.imageRole): $($file.path)."
            }
        }
        Write-Output "Verified $($bundleReference.imageRole) flash bundle and checksums."
    }
} finally {
    Pop-Location
}
