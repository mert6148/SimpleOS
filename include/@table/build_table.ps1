# Build script for PowerShell. Attempts g++ then cl.exe
Set-Location -Path $PSScriptRoot
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    g++ -std=c++17 -O2 -Wall -Wextra -o table_cli TableDB.cpp table_cli.cpp
    g++ -std=c++17 -O2 -Wall -Wextra -o print TableDB.cpp print.cpp
    Write-Host "Built table_cli and print with g++"
    exit 0
}
if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
    cl /nologo /Ox /std:c++17 /Fe:table_cli.exe TableDB.cpp table_cli.cpp
    cl /nologo /Ox /std:c++17 /Fe:print.exe TableDB.cpp print.cpp
    Write-Host "Built table_cli and print with cl.exe"
    exit 0
}
Write-Error "No supported compiler found (g++ or cl.exe)."; exit 1
