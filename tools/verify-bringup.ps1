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
    Get-ChildItem artifacts -Filter 'vtoc-agent-0.1.0-bringup-*.bin' | ForEach-Object {
        Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName
    }
} finally {
    Pop-Location
}
