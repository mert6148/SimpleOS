param()

$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Path -Parent
$buildDir = Join-Path $scriptDir 'build'

if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Push-Location $buildDir
try {
    cmake $scriptDir
    cmake --build . --config Release
    Write-Host "Built x86 and x64 system demos in $buildDir."
}
finally {
    Pop-Location
}
