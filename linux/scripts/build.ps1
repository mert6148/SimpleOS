Param(
    [ValidateSet('all', 'app', 'kernel', 'clean', 'ci')]
    [string]$Target = 'all'
)

$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..')

Push-Location $root
try {
    if (Get-Command make -ErrorAction SilentlyContinue) {
        make $Target
        return
    }

    $cc = Get-Command gcc -ErrorAction SilentlyContinue
    if (-not $cc) {
        throw 'make veya gcc bulunamadi. MSYS2/MinGW, Git Bash veya WSL kurun.'
    }

    $gcc = $cc.Source
    $cflags = @('-Wall', '-Wextra', '-O2', '-std=c11')
    $bin = Join-Path $root 'bin'
    New-Item -ItemType Directory -Force -Path $bin | Out-Null

    if ($Target -eq 'clean') {
        Remove-Item -Recurse -Force $bin -ErrorAction SilentlyContinue
        return
    }

    $doApp = $Target -eq 'all' -or $Target -eq 'app' -or $Target -eq 'ci'
    $doKernel = $Target -eq 'all' -or $Target -eq 'kernel' -or $Target -eq 'ci'

    if ($doApp) {
        & $gcc @cflags -o (Join-Path $bin 'simpleos-app') (Join-Path $root 'app\app.c')
    }
    if ($doKernel) {
        & $gcc @cflags -o (Join-Path $bin 'simpleos-kernel-stub') (Join-Path $root 'kernel\kernel.c')
    }
}
finally {
    Pop-Location
}
