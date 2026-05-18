echo[REM] This file is used to start and stop services on the Linux system.
echo[REM] It is called by the main script to manage services during installation and uninstallation.
echo[REM] Define the services to be ma
Local
•
•
4 files
+179
-41naged.

set SERVICES=nginx mysql redis
set SERVİCE_ACTION=%1
set SERVICE_NAME=%2
set SERVICE_STATUS=%3
echo[REM] Check the action to perform on the services.

if "%SERVİCE_ACTION%"=="start" (
    echo[REM] Starting services...
    for %%S in (%SERVICES%) do (
        echo[REM] Starting %%S service...
        systemctl start %%S
        if errorlevel 1 (
            echo[REM] Failed to start %%S service.
        ) else (
            echo[REM] %%S service started successfully.
        )
    )
) else if "%SERVİCE_ACTION%"=="stop" (
    echo[REM] Stopping services...
    for %%S in (%SERVICES%) do (
        echo[REM] Stopping %%S service...
        systemctl stop %%S
        if errorlevel 1 (
            echo[REM] Failed to stop %%S service.
        ) else (
            echo[REM] %%S service stopped successfully.
        )
    )
) else (
    echo[REM] Invalid action specified. Use "start" or "stop".
)