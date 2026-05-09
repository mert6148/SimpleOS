# SimpleOS Build Script for Windows
# This script tries to build using available tools (WSL, MinGW, MSYS2)
# 
# Usage: .\build.ps1 [target]
#        .\build.ps1 qemu
#        .\build.ps1 clean

param(
    [string]$Target = "all"
)

Write-Host "SimpleOS Build Script for Windows" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

# Try WSL first
function Test-WSL {
    try {
        $version = wsl --version 2>$null
        if ($?) { return $true }
    } catch {}
    return $false
}

# Try MinGW/MSYS2
function Test-MinGW {
    try {
        $version = gcc --version 2>$null
        if ($?) { return $true }
    } catch {}
    return $false
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

if (Test-WSL) {
    Write-Host "✓ WSL found - using WSL for build" -ForegroundColor Green
    Write-Host ""
    Write-Host "Running: wsl make $Target" -ForegroundColor Yellow
    Write-Host ""
    
    $wslPath = wsl wslpath -a "$scriptDir"
    wsl -e bash -c "cd $wslPath && make $Target"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "✓ Build successful!" -ForegroundColor Green
        Write-Host ""
        if ($Target -eq "qemu") {
            Write-Host "Kernel running in QEMU..." -ForegroundColor Yellow
            Write-Host "Press Ctrl+A then X to exit QEMU" -ForegroundColor Gray
        }
    } else {
        Write-Host ""
        Write-Host "✗ Build failed" -ForegroundColor Red
        exit 1
    }
} elseif (Test-MinGW) {
    Write-Host "✓ MinGW/MSYS2 found - using native compiler" -ForegroundColor Green
    Write-Host ""
    Write-Host "Running: make $Target" -ForegroundColor Yellow
    Write-Host ""
    
    cd $scriptDir
    & make $Target
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "✓ Build successful!" -ForegroundColor Green
    } else {
        Write-Host ""
        Write-Host "✗ Build failed" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "✗ No build tools found" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install one of:" -ForegroundColor Yellow
    Write-Host "  1. WSL2 with Ubuntu (recommended)"
    Write-Host "  2. MinGW/MSYS2"
    Write-Host "  3. Docker"
    Write-Host ""
    Write-Host "See WINDOWS_SETUP.md for detailed instructions"
    exit 1
}
