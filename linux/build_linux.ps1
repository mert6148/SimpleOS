param()

$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Path -Parent

Push-Location "$scriptDir\x86"
try {
    & make
}
finally {
    Pop-Location
}

Push-Location "$scriptDir\x64"
try {
    & make
}
finally {
    Pop-Location
}

Push-Location "$scriptDir\distribution"
try {
    dotnet run --project SimpleOSLinuxDist.csproj -- --root "$scriptDir" --package
}
finally {
    Pop-Location
}

Write-Host "Linux distribution build complete."
