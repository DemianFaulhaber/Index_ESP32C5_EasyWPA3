param(
    [string]$SourceVendor = "esp32",
    [string]$TargetVendor = "indexesp32",
    [string]$CoreName = "esp32",
    [string]$CoreVersion = "3.3.8",
    [string]$Arduino15 = "$env:LOCALAPPDATA\Arduino15"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$msg) {
    Write-Host "[STEP] $msg" -ForegroundColor Cyan
}

function Ensure-Exists([string]$path) {
    if (-not (Test-Path $path)) {
        throw "Path not found: $path"
    }
}

$srcRoot = Join-Path $Arduino15 "packages\$SourceVendor"
$dstRoot = Join-Path $Arduino15 "packages\$TargetVendor"

$srcHw = Join-Path $srcRoot "hardware\$CoreName\$CoreVersion"
$srcTools = Join-Path $srcRoot "tools"
$dstHw = Join-Path $dstRoot "hardware\$CoreName\$CoreVersion"
$dstTools = Join-Path $dstRoot "tools"

Write-Step "Validating source core"
Ensure-Exists $srcHw
Ensure-Exists $srcTools

Write-Step "Creating target package root"
New-Item -ItemType Directory -Force -Path $dstRoot | Out-Null

Write-Step "Copying hardware core to custom vendor package"
New-Item -ItemType Directory -Force -Path (Split-Path $dstHw -Parent) | Out-Null
Copy-Item -Path $srcHw -Destination (Split-Path $dstHw -Parent) -Recurse -Force

Write-Step "Copying tools folder to custom vendor package"
Copy-Item -Path $srcTools -Destination $dstRoot -Recurse -Force

$libsRoot = Join-Path $dstTools "esp32c5-libs\$CoreVersion"
$sdkconfig = Join-Path $libsRoot "sdkconfig"
$headers = @(
    (Join-Path $libsRoot "dio_qspi\include\sdkconfig.h"),
    (Join-Path $libsRoot "qio_qspi\include\sdkconfig.h")
)

Write-Step "Patching sdkconfig for Suite-B 192"
Ensure-Exists $sdkconfig
$raw = Get-Content $sdkconfig -Raw
if ($raw -match "(?m)^# CONFIG_ESP_WIFI_SUITE_B_192 is not set$") {
    $raw = $raw -replace "(?m)^# CONFIG_ESP_WIFI_SUITE_B_192 is not set$", "CONFIG_ESP_WIFI_SUITE_B_192=y"
} elseif ($raw -notmatch "(?m)^CONFIG_ESP_WIFI_SUITE_B_192=y$") {
    $raw += "`r`nCONFIG_ESP_WIFI_SUITE_B_192=y`r`n"
}
Set-Content -Path $sdkconfig -Value $raw -Encoding ascii

Write-Step "Patching generated sdkconfig headers"
foreach ($hdr in $headers) {
    Ensure-Exists $hdr
    $h = Get-Content $hdr -Raw
    if ($h -notmatch "(?m)^#define CONFIG_ESP_WIFI_SUITE_B_192 1$") {
        $h = $h.TrimEnd() + "`r`n#define CONFIG_ESP_WIFI_SUITE_B_192 1`r`n"
        Set-Content -Path $hdr -Value $h -Encoding ascii
    }
}

Write-Step "Adjusting package name in boards metadata"
$boardsTxt = Join-Path $dstHw "boards.txt"
if (Test-Path $boardsTxt) {
    $b = Get-Content $boardsTxt -Raw
    if ($b -match "(?m)^name=.*$") {
        $b = $b -replace "(?m)^name=.*$", "name=ESP32 Boards (Index WPA3 192 Custom Core)"
    } else {
        $b = "name=ESP32 Boards (Index WPA3 192 Custom Core)`r`n" + $b
    }
    Set-Content -Path $boardsTxt -Value $b -Encoding ascii
}

Write-Step "Done"
Write-Host "Custom core created at: $dstHw" -ForegroundColor Green
Write-Host "Tools copied at:       $dstTools" -ForegroundColor Green
Write-Host "Restart Arduino IDE and select the board from vendor '$TargetVendor'." -ForegroundColor Yellow
