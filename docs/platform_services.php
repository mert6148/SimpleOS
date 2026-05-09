<?php
/**
 * SimpleOS Platform Services
 * Google, Windows, Linux platformları için hizmetler
 * 
 * @version 1.0.0
 * @license MIT
 */

/**
 * Platformlar arası uyumlu hizmetler
 */
class PlatformDetector {
    public static function getOS() {
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
            return 'windows';
        } elseif (stripos(PHP_OS, 'Linux') !== false) {
            return 'linux';
        } elseif (stripos(PHP_OS, 'Darwin') !== false) {
            return 'macos';
        }
        return 'unknown';
    }
    
    public static function getEnvironment() {
        return [
            'os' => self::getOS(),
            'php_version' => phpversion(),
            'memory_limit' => ini_get('memory_limit'),
            'max_execution_time' => ini_get('max_execution_time'),
            'temp_dir' => sys_get_temp_dir(),
            'home_dir' => self::getHomeDir(),
            'separators' => [
                'path' => PATH_SEPARATOR,
                'directory' => DIRECTORY_SEPARATOR
            ]
        ];
    }
    
    private static function getHomeDir() {
        if (self::getOS() === 'windows') {
            return getenv('USERPROFILE') ?: getenv('HOME');
        }
        return getenv('HOME') ?: '/root';
    }
}

/**
 * Windows-specific services
 */
class WindowsServices {
    private $system32Path;
    private $programFilesPath;
    
    public function __construct() {
        $this->system32Path = getenv('WINDIR') . '\\System32';
        $this->programFilesPath = getenv('ProgramFiles');
    }
    
    /**
     * Create Windows scheduled task for documentation sync
     */
    public function createScheduledTask($taskName, $scriptPath, $frequency = 'daily') {
        $triggers = [
            'daily' => 'DAILY /EVERY 1',
            'hourly' => 'MINUTE /EVERY 60',
            'weekly' => 'WEEKLY /EVERY 1'
        ];
        
        $trigger = $triggers[$frequency] ?? $triggers['daily'];
        
        $command = "schtasks /create /tn \"SimpleOS\\{$taskName}\" /tr \"{$scriptPath}\" /sc {$trigger} /ru SYSTEM /f";
        
        return [
            'command' => $command,
            'description' => "Scheduled task: {$taskName}",
            'frequency' => $frequency
        ];
    }
    
    /**
     * Install Windows Service
     */
    public function installService($serviceName, $phpPath, $scriptPath) {
        $nssm = $this->system32Path . '\\nssm.exe';
        
        if (!file_exists($nssm)) {
            return ['error' => 'NSSM not found. Download from: https://nssm.cc'];
        }
        
        $command = "{$nssm} install {$serviceName} \"{$phpPath}\" \"{$scriptPath}\"";
        
        return [
            'command' => $command,
            'service_name' => $serviceName,
            'status' => 'pending'
        ];
    }
    
    /**
     * Get Windows registry documentation settings
     */
    public function getRegistrySettings() {
        $regPath = 'HKEY_LOCAL_MACHINE\\SOFTWARE\\SimpleOS\\Docs';
        
        return [
            'registry_path' => $regPath,
            'settings' => [
                'ApiUrl' => 'http://localhost:8000/docs/docsservices.php',
                'CacheDir' => '%APPDATA%\\SimpleOS\\Docs\\Cache',
                'AutoUpdate' => 1,
                'UpdateFrequency' => 3600
            ]
        ];
    }
    
    /**
     * Create Windows batch installer
     */
    public function createInstaller($appName, $appVersion, $appPath) {
        $installerBatch = <<<'BATCH'
@echo off
setlocal enabledelayedexpansion

echo Installing {APP_NAME} v{VERSION}...

set INSTALL_DIR=%ProgramFiles%\{APP_NAME}
if not exist "!INSTALL_DIR!" mkdir "!INSTALL_DIR!"

echo Copying files...
xcopy /E /I /Y "%~dp0." "!INSTALL_DIR!"

echo Creating shortcuts...
powershell -Command "$WshShell = New-Object -ComObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%AppData%\Microsoft\Windows\Start Menu\Programs\{APP_NAME}.lnk'); $Shortcut.TargetPath = '!INSTALL_DIR!\simpleos_docs_viewer.exe'; $Shortcut.Save()"

echo Setting registry entries...
reg add "HKLM\SOFTWARE\{APP_NAME}" /v "InstallPath" /d "!INSTALL_DIR!" /f
reg add "HKLM\SOFTWARE\{APP_NAME}" /v "Version" /d "{VERSION}" /f

echo.
echo Installation complete!
echo Launch the application from Start Menu
pause
BATCH;
        
        return str_replace(
            ['{APP_NAME}', '{VERSION}'],
            [$appName, $appVersion],
            $installerBatch
        );
    }
    
    /**
     * Get Windows app configuration paths
     */
    public function getAppPaths() {
        $appData = getenv('APPDATA');
        $localAppData = getenv('LOCALAPPDATA');
        
        return [
            'app_data' => $appData . '\\SimpleOS\\Docs',
            'local_app_data' => $localAppData . '\\SimpleOS\\Docs',
            'documents' => getenv('USERPROFILE') . '\\Documents\\SimpleOS',
            'program_files' => $this->programFilesPath . '\\SimpleOS\\Docs',
            'temp' => sys_get_temp_dir() . '\\simpleos_docs'
        ];
    }
}

/**
 * Linux-specific services
 */
class LinuxServices {
    private $installPrefix = '/opt/simpleos-docs';
    
    /**
     * Create systemd service file
     */
    public function createSystemdService($serviceName = 'simpleos-docs') {
        $serviceContent = <<<'SERVICE'
[Unit]
Description=SimpleOS Documentation Service
After=network.target

[Service]
Type=simple
User=www-data
WorkingDirectory={INSTALL_DIR}
ExecStart=/usr/bin/php -S 0.0.0.0:8000 {INSTALL_DIR}/docsservices.php
Restart=on-failure
RestartSec=10s
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
SERVICE;
        
        return str_replace('{INSTALL_DIR}', $this->installPrefix, $serviceContent);
    }
    
    /**
     * Create systemd timer for periodic sync
     */
    public function createSystemdTimer($timerName = 'simpleos-docs-sync') {
        $timerFile = <<<'TIMER'
[Unit]
Description=SimpleOS Documentation Sync Timer
Requires={TIMER_NAME}.service

[Timer]
OnBootSec=10min
OnUnitActiveSec=1h
Unit={TIMER_NAME}.service

[Install]
WantedBy=timers.target
TIMER;
        
        $serviceFile = <<<'SERVICE'
[Unit]
Description=SimpleOS Documentation Sync
After=network.target

[Service]
Type=oneshot
ExecStart=/usr/bin/php {INSTALL_DIR}/sync.php
User=www-data
SERVICE;
        
        return [
            'timer' => str_replace('{TIMER_NAME}', $timerName, $timerFile),
            'service' => str_replace('{INSTALL_DIR}', $this->installPrefix, $serviceFile)
        ];
    }
    
    /**
     * Create init.d script for older systems
     */
    public function createInitScript($serviceName = 'simpleos-docs') {
        $initScript = <<<'INIT'
#!/bin/bash
### BEGIN INIT INFO
# Provides:          {SERVICE_NAME}
# Required-Start:    $network $remote_fs
# Required-Stop:     $network $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: SimpleOS Documentation Service
# Description:       RESTful API for SimpleOS documentation
### END INIT INFO

DAEMON=/usr/bin/php
DAEMON_ARGS="{INSTALL_DIR}/docsservices.php"
PIDFILE=/var/run/{SERVICE_NAME}.pid
LOGFILE=/var/log/{SERVICE_NAME}.log

case "$1" in
  start)
    echo "Starting {SERVICE_NAME}..."
    $DAEMON $DAEMON_ARGS > $LOGFILE 2>&1 &
    echo $! > $PIDFILE
    ;;
  stop)
    echo "Stopping {SERVICE_NAME}..."
    if [ -f $PIDFILE ]; then
      kill $(cat $PIDFILE)
      rm $PIDFILE
    fi
    ;;
  restart)
    $0 stop
    sleep 1
    $0 start
    ;;
  status)
    if [ -f $PIDFILE ]; then
      echo "{SERVICE_NAME} is running (PID: $(cat $PIDFILE))"
    else
      echo "{SERVICE_NAME} is not running"
    fi
    ;;
  *)
    echo "Usage: $0 {start|stop|restart|status}"
    exit 1
esac
INIT;
        
        return str_replace(
            ['{SERVICE_NAME}', '{INSTALL_DIR}'],
            [$serviceName, $this->installPrefix],
            $initScript
        );
    }
    
    /**
     * Create AppImage desktop entry
     */
    public function createAppImageEntry() {
        $desktopEntry = <<<'ENTRY'
[Desktop Entry]
Type=Application
Name=SimpleOS Documentation Viewer
Comment=View SimpleOS documentation offline
Exec=simpleos-docs-viewer
Icon=simpleos-docs-viewer
Categories=Documentation;Development;
Terminal=false
StartupNotify=true
ENTRY;
        
        return $desktopEntry;
    }
    
    /**
     * Get Linux installation paths
     */
    public function getInstallPaths() {
        return [
            'bin' => '/usr/local/bin',
            'opt' => '/opt/simpleos-docs',
            'etc' => '/etc/simpleos-docs',
            'var' => '/var/lib/simpleos-docs',
            'log' => '/var/log/simpleos-docs',
            'systemd' => '/etc/systemd/system',
            'home_config' => getenv('HOME') . '/.config/simpleos-docs'
        ];
    }
    
    /**
     * Create Dockerfile for containerization
     */
    public function createDockerfile($phpVersion = '8.1') {
        $dockerfile = <<<'DOCKERFILE'
FROM php:{VERSION}-cli

LABEL maintainer="Mert Doğan"
LABEL description="SimpleOS Documentation Services"

# Install required extensions
RUN apt-get update && apt-get install -y \
    curl \
    libcurl4-openssl-dev \
    && docker-php-ext-install curl \
    && rm -rf /var/lib/apt/lists/*

# Create application directory
WORKDIR /app

# Copy application files
COPY docsservices.php .
COPY google_integration.php .
COPY config.json .

# Create cache and log directories
RUN mkdir -p cache logs backups && \
    chmod 755 cache logs backups

# Expose port
EXPOSE 8000

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8000/api/health || exit 1

# Run PHP development server
CMD ["php", "-S", "0.0.0.0:8000"]
DOCKERFILE;
        
        return str_replace('{VERSION}', $phpVersion, $dockerfile);
    }
    
    /**
     * Create docker-compose file
     */
    public function createDockerCompose() {
        $compose = <<<'COMPOSE'
version: '3.8'

services:
  docs-api:
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "8000:8000"
    volumes:
      - ./docs:/app
      - ./cache:/app/cache
      - ./logs:/app/logs
    environment:
      - PHP_MEMORY_LIMIT=256M
      - GOOGLE_CLIENT_ID=${GOOGLE_CLIENT_ID}
      - GOOGLE_CLIENT_SECRET=${GOOGLE_CLIENT_SECRET}
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8000/api/health"]
      interval: 30s
      timeout: 10s
      retries: 3
    restart: unless-stopped

volumes:
  docs:
  cache:
  logs:
COMPOSE;
        
        return $compose;
    }
}

/**
 * Google Cloud Services
 */
class GoogleCloudServices {
    private $projectId;
    private $bucketName;
    
    public function __construct($projectId = '', $bucketName = '') {
        $this->projectId = $projectId ?? getenv('GOOGLE_CLOUD_PROJECT');
        $this->bucketName = $bucketName ?? getenv('GOOGLE_CLOUD_BUCKET');
    }
    
    /**
     * Create Google Cloud Storage configuration
     */
    public function createGCSConfig() {
        return [
            'project_id' => $this->projectId,
            'bucket_name' => $this->bucketName,
            'upload_path' => 'gs://' . $this->bucketName . '/docs',
            'gcs_key_file' => getenv('GOOGLE_APPLICATION_CREDENTIALS')
        ];
    }
    
    /**
     * Create Cloud Run deployment configuration
     */
    public function createCloudRunConfig($serviceName = 'simpleos-docs') {
        $config = [
            'service' => $serviceName,
            'image' => 'gcr.io/' . $this->projectId . '/' . $serviceName . ':latest',
            'memory' => '256Mi',
            'cpu' => '1',
            'port' => 8000,
            'timeout' => 3600,
            'min_instances' => 1,
            'max_instances' => 10,
            'allow_unauthenticated' => true
        ];
        
        return $config;
    }
    
    /**
     * Create gcloud deploy command
     */
    public function getDeployCommand($serviceName = 'simpleos-docs', $region = 'europe-west1') {
        $command = <<<CMD
gcloud run deploy {SERVICE_NAME} \
  --source . \
  --region {REGION} \
  --platform managed \
  --memory 256Mi \
  --cpu 1 \
  --allow-unauthenticated \
  --set-env-vars GOOGLE_CLOUD_PROJECT={PROJECT_ID} \
  --port 8000
CMD;
        
        return str_replace(
            ['{SERVICE_NAME}', '{REGION}', '{PROJECT_ID}'],
            [$serviceName, $region, $this->projectId],
            $command
        );
    }
    
    /**
     * Create Firestore backup configuration
     */
    public function createFirestoreBackup() {
        return [
            'database_id' => '(default)',
            'collection' => 'documentation',
            'backup_location' => 'europe-west1',
            'retention_days' => 30,
            'enabled' => true
        ];
    }
    
    /**
     * Create Cloud Pub/Sub configuration for sync notifications
     */
    public function createPubSubConfig() {
        return [
            'topic' => 'projects/' . $this->projectId . '/topics/simpleos-docs-sync',
            'subscription' => 'simpleos-docs-sync-sub',
            'ack_deadline_seconds' => 60,
            'message_retention_duration' => 604800  // 7 days
        ];
    }
    
    /**
     * Create App Engine configuration
     */
    public function createAppEngineConfig($runtime = 'php81') {
        $appYaml = <<<'YAML'
runtime: {RUNTIME}
env: standard
entrypoint: serve /app/docsservices.php

handlers:
  - url: /.*
    script: auto
    secure: always

env_variables:
  GOOGLE_CLOUD_PROJECT: "{PROJECT_ID}"

inbound_services:
  - warmup
YAML;
        
        return str_replace(
            ['{RUNTIME}', '{PROJECT_ID}'],
            [$runtime, $this->projectId],
            $appYaml
        );
    }
}

/**
 * Cross-platform service manager
 */
class CrossPlatformServiceManager {
    private $platform;
    private $windows;
    private $linux;
    private $google;
    
    public function __construct() {
        $this->platform = PlatformDetector::getOS();
        $this->windows = new WindowsServices();
        $this->linux = new LinuxServices();
        $this->google = new GoogleCloudServices();
    }
    
    /**
     * Install service for current platform
     */
    public function installService($config = []) {
        $result = [
            'platform' => $this->platform,
            'status' => 'pending',
            'instructions' => []
        ];
        
        switch ($this->platform) {
            case 'windows':
                $result['type'] = 'Windows Service';
                $result['installer'] = $this->windows->createInstaller(
                    $config['app_name'] ?? 'SimpleOS Docs',
                    $config['app_version'] ?? '1.0.0',
                    $config['app_path'] ?? __DIR__
                );
                $result['instructions'] = [
                    'Save the batch script as install.bat',
                    'Run as Administrator',
                    'Service will be installed automatically'
                ];
                break;
                
            case 'linux':
                $result['type'] = 'Systemd Service';
                $result['service_file'] = $this->linux->createSystemdService();
                $result['timer_files'] = $this->linux->createSystemdTimer();
                $result['instructions'] = [
                    'sudo cp simpleos-docs.service /etc/systemd/system/',
                    'sudo cp simpleos-docs-sync.service /etc/systemd/system/',
                    'sudo cp simpleos-docs-sync.timer /etc/systemd/system/',
                    'sudo systemctl daemon-reload',
                    'sudo systemctl enable simpleos-docs.service',
                    'sudo systemctl start simpleos-docs.service'
                ];
                break;
                
            default:
                $result['status'] = 'unsupported';
                $result['message'] = 'Platform not supported';
        }
        
        return $result;
    }
    
    /**
     * Get installation instructions for platform
     */
    public function getInstallationGuide() {
        $guide = [
            'platform' => $this->platform,
            'environment' => PlatformDetector::getEnvironment(),
            'steps' => []
        ];
        
        switch ($this->platform) {
            case 'windows':
                $guide['steps'] = [
                    '1. Download simpleos_docs_viewer.exe from releases',
                    '2. Run the installer',
                    '3. Automatically adds to Start Menu',
                    '4. Launch from Start Menu or create desktop shortcut',
                    '5. Configure API URL if needed'
                ];
                $guide['paths'] = $this->windows->getAppPaths();
                break;
                
            case 'linux':
                $guide['steps'] = [
                    '1. Download simpleos-docs-viewer.AppImage',
                    '2. chmod +x simpleos-docs-viewer.AppImage',
                    '3. ./simpleos-docs-viewer.AppImage',
                    '4. Or use: sudo apt install ./simpleos-docs-viewer.deb (if available)',
                    '5. Launch from Applications menu'
                ];
                $guide['paths'] = $this->linux->getInstallPaths();
                break;
        }
        
        return $guide;
    }
    
    /**
     * Get platform-specific configuration
     */
    public function getPlatformConfig() {
        $config = [
            'platform' => $this->platform,
            'general' => PlatformDetector::getEnvironment()
        ];
        
        switch ($this->platform) {
            case 'windows':
                $config['windows'] = [
                    'registry_settings' => $this->windows->getRegistrySettings(),
                    'app_paths' => $this->windows->getAppPaths(),
                    'scheduled_tasks' => $this->windows->createScheduledTask(
                        'DocSync',
                        'php sync.php',
                        'daily'
                    )
                ];
                break;
                
            case 'linux':
                $config['linux'] = [
                    'install_paths' => $this->linux->getInstallPaths(),
                    'service_config' => $this->linux->createSystemdService(),
                    'docker_config' => $this->linux->createDockerfile()
                ];
                break;
        }
        
        // Google Cloud always available
        $config['google_cloud'] = [
            'cloud_run' => $this->google->createCloudRunConfig(),
            'app_engine' => $this->google->createAppEngineConfig(),
            'cloud_storage' => $this->google->createGCSConfig(),
            'pub_sub' => $this->google->createPubSubConfig()
        ];
        
        return $config;
    }
}

// ============================================================
// API Endpoint Handler
// ============================================================
class PlatformServicesAPI {
    private $manager;
    
    public function __construct() {
        $this->manager = new CrossPlatformServiceManager();
    }
    
    public function handleRequest() {
        $method = $_SERVER['REQUEST_METHOD'];
        $path = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
        $action = basename($path);
        
        header('Content-Type: application/json; charset=utf-8');
        
        switch ($action) {
            case 'platform-info':
                echo json_encode($this->manager->getInstallationGuide(), JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
                break;
                
            case 'platform-config':
                echo json_encode($this->manager->getPlatformConfig(), JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
                break;
                
            case 'install-service':
                $config = json_decode(file_get_contents('php://input'), true) ?? [];
                echo json_encode($this->manager->installService($config), JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
                break;
                
            default:
                http_response_code(404);
                echo json_encode(['error' => 'Unknown action']);
        }
    }
}

// Execute if called directly
if (basename($_SERVER['PHP_SELF']) === 'platform_services.php') {
    $api = new PlatformServicesAPI();
    $api->handleRequest();
}
