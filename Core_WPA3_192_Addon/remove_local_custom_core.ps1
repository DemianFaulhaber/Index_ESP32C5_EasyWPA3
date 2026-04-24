param(
    [string]$TargetVendor = "indexesp32",
    [string]$Arduino15 = "$env:LOCALAPPDATA\Arduino15"
)

$ErrorActionPreference = "Stop"
$dstRoot = Join-Path $Arduino15 "packages\$TargetVendor"

if (Test-Path $dstRoot) {
    Remove-Item -Path $dstRoot -Recurse -Force
    Write-Host "Removed custom core package: $dstRoot" -ForegroundColor Green
} else {
    Write-Host "Custom core package not found: $dstRoot" -ForegroundColor Yellow
}
