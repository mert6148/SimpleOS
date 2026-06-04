@echo off
REM SimpleOS GitHub helper script
REM Usage:
REM   github-commands.bat help
REM   github-commands.bat init <remote-url> [main|master]
REM   github-commands.bat status
REM   github-commands.bat add
REM   github-commands.bat commit "Your commit message"
REM   github-commands.bat push [main|master]
REM   github-commands.bat force-push [main|master]
REM   github-commands.bat set-remote <remote-url>
REM   github-commands.bat log
REM   github-commands.bat branch

setlocal enabledelayedexpansion
set "REMOTE_URL=%~2"
set "BRANCH=%~3"
if "%BRANCH%"=="" set "BRANCH=main"

if "%~1"=="" goto usage

if /I "%~1"=="help" goto usage
if /I "%~1"=="status" goto status
if /I "%~1"=="init" goto init
if /I "%~1"=="add" goto add
if /I "%~1"=="commit" goto commit
if /I "%~1"=="push" goto push
if /I "%~1"=="force-push" goto force_push
if /I "%~1"=="set-remote" goto set_remote
if /I "%~1"=="log" goto log
if /I "%~1"=="branch" goto branch
if /I "%~1"=="pull-rebase" goto pull_rebase
if /I "%~1"=="remote" goto remote

necho Unknown command: %~1
echo.
goto usage

:init
if "%REMOTE_URL%"=="" (
    echo Usage: github-commands.bat init ^<remote-url^> [main^|master]
    goto end
)
git init
ngit branch -M %BRANCH%
ngit add .
ngit commit -m "Initial commit" 2>nul || echo No changes to commit.
ngit remote add origin %REMOTE_URL% 2>nul || git remote set-url origin %REMOTE_URL%
ngit push -u origin %BRANCH%
goto end

:status
git status
goto end

:add
git add .
goto end

:commit
 if "%~2"=="" (
    echo Usage: github-commands.bat commit "Your commit message"
    goto end
)
git commit -m "%~2"
goto end

:push
ngit push -u origin %BRANCH%
goto end

:force_push
ngit push --force-with-lease origin %BRANCH%
goto end

:set_remote
if "%REMOTE_URL%"=="" (
    echo Usage: github-commands.bat set-remote ^<remote-url^>
    goto end
)
git remote remove origin 2>nul
ngit remote add origin %REMOTE_URL%
goto end

:log
ngit log --oneline -5
goto end

:branch
ngit branch -avv
goto end

:pull_rebase
ngit pull --rebase origin %BRANCH%
goto end

:remote
ngit remote -v
goto end

:usage
echo GitHub command helper for SimpleOS repository.
echo.
echo Usage:
echo   github-commands.bat help
echo   github-commands.bat init ^<remote-url^> [main^|master]
echo   github-commands.bat status
echo   github-commands.bat add
echo   github-commands.bat commit "Your commit message"
echo   github-commands.bat push [main^|master]
echo   github-commands.bat force-push [main^|master]
echo   github-commands.bat set-remote ^<remote-url^>
echo   github-commands.bat remote
echo   github-commands.bat log
echo   github-commands.bat branch
echo   github-commands.bat pull-rebase [main^|master]
echo.
echo Examples:
echo   github-commands.bat init https://github.com/mert6148/SimpleOS.git main
echo   github-commands.bat commit "Update README"
echo   github-commands.bat push
echo   github-commands.bat force-push master
goto end

:end
endlocal