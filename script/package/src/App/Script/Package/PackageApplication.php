<?php

declare(strict_types=1);

namespace App\Script\Package;

use App\Kernel\Kernel;
use App\Script\SysProtocol\SysProtocol;
use App\Script\SysProtocol\NetworkHelper;
use App\Script\SysProtocol\SystemConfigChecker;
use App\Script\Package\ApplicationPackageRepository;
use App\Script\Package\ApplicationPackageInstaller;

class PackageApplication
{
    private PackageRepository $repository;
    private PackageInstaller $installer;
    private ApplicationPackageRepository $appRepository;
    private ApplicationPackageInstaller $appInstaller;
    private Kernel $kernel;
    private SysProtocol $sysProtocol;
    private NetworkHelper $networkHelper;

    public function __construct(PackageRepository $repository, PackageInstaller $installer, ApplicationPackageRepository $appRepository, ApplicationPackageInstaller $appInstaller)
    {
        $this->repository = $repository;
        $this->installer = $installer;
        $this->appRepository = $appRepository;
        $this->appInstaller = $appInstaller;
        $this->kernel = Kernel::getInstance();
        $this->sysProtocol = new SysProtocol();
        $this->networkHelper = new NetworkHelper();
    }

    public static function run(array $argv): void
    {
        $packageManager = new PackageManager();
        $repository = new PackageRepository($packageManager);
        $installer = new PackageInstaller($packageManager);
        $appRepository = new ApplicationPackageRepository();
        $appInstaller = new ApplicationPackageInstaller($appRepository, $installer);
        $app = new self($repository, $installer, $appRepository, $appInstaller);

        $repository->loadFromArray([
            [
                'name' => 'core-utils',
                'version' => '1.0.0',
                'description' => 'Core system utilities for SimpleOS.',
                'dependencies' => [],
            ],
            [
                'name' => 'kernel-headers',
                'version' => '1.0.1',
                'description' => 'Kernel header files for module compilation.',
                'dependencies' => ['core-utils'],
            ],
            [
                'name' => 'shell-tools',
                'version' => '0.3.2',
                'description' => 'Shell utilities and command-line helpers.',
                'dependencies' => ['core-utils'],
            ],
            [
                'name' => 'system-packages',
                'version' => '0.1.0',
                'description' => 'Package manager runtime for system packages.',
                'dependencies' => ['kernel-headers', 'shell-tools'],
            ],
        ]);

        $appRepository->loadFromArray([
            [
                'name' => 'text-editor',
                'version' => '0.1.0',
                'description' => 'Minimal text editor for SimpleOS applications.',
                'dependencies' => ['shell-tools'],
                'entryPoint' => 'text-editor',
            ],
            [
                'name' => 'file-manager',
                'version' => '0.2.0',
                'description' => 'Graphical file manager for navigating SimpleOS files.',
                'dependencies' => ['shell-tools'],
                'entryPoint' => 'file-manager',
            ],
            [
                'name' => 'process-viewer',
                'version' => '0.1.1',
                'description' => 'Process viewer for monitoring running applications.',
                'dependencies' => ['shell-tools'],
                'entryPoint' => 'process-viewer',
            ],
        ]);

        $command = $argv[1] ?? 'help';
        if ($command === 'app') {
            $app->runAppCommand(array_slice($argv, 2));
            return;
        }

        $argument = $argv[2] ?? null;

        switch ($command) {
            case 'list':
                $app->listPackages();
                break;
            case 'install':
                if ($argument === null) {
                    $app->printHelp();
                    break;
                }
                $app->installPackage($argument);
                break;
            case 'info':
                if ($argument === null) {
                    $app->printHelp();
                    break;
                }
                $app->printPackageInfo($argument);
                break;
            case 'version':
                $app->showPackageVersion();
                break;
            case 'prompt':
                $app->runPromptCommand(array_slice($argv, 2));
                break;
            case 'kernel':
                $app->showKernelInfo();
                break;
            case 'kernel-init':
                $app->initializeKernel();
                break;
            case 'kernel-status':
                $app->showKernelStatus();
                break;
            case 'protocol':
                $app->showProtocolInfo();
                break;
            case 'network':
                $app->showNetworkStatus();
                break;
            case 'validate-ip':
                if ($argument === null) {
                    $app->printHelp();
                    break;
                }
                $app->validateIP($argument);
                break;
            case 'endpoint':
                if ($argument === null || !isset($argv[3])) {
                    $app->printHelp();
                    break;
                }
                $app->showEndpointInfo($argument, (int)$argv[3]);
                break;
            case 'system-check':
                $app->runSystemCheck();
                break;
            default:
                $app->printHelp();
                break;
        }
    }

    private function runAppCommand(array $argv): void
    {
        $command = $argv[0] ?? 'help';
        $argument = $argv[1] ?? null;

        switch ($command) {
            case 'list':
                $this->listApplications();
                break;
            case 'install':
                if ($argument === null) {
                    $this->printHelp();
                    break;
                }
                $this->appInstaller->installApplication($argument);
                break;
            case 'remove':
                if ($argument === null) {
                    $this->printHelp();
                    break;
                }
                $this->appInstaller->removeApplication($argument);
                break;
            case 'info':
                if ($argument === null) {
                    $this->printHelp();
                    break;
                }
                $this->printApplicationInfo($argument);
                break;
            case 'run':
                if ($argument === null) {
                    $this->printHelp();
                    break;
                }
                $appArgs = array_slice($argv, 2);
                if (isset($appArgs[0]) && $appArgs[0] === '--') {
                    array_shift($appArgs);
                }
                $this->appInstaller->runApplication($argument, $appArgs);
                break;
            default:
                $this->printHelp();
                break;
        }
    }

    private function runPromptCommand(array $argv): void
    {
        $command = $argv[0] ?? 'help';
        $name = $argv[1] ?? null;
        $options = array_slice($argv, 2);

        switch ($command) {
            case 'list':
                $this->listPromptTemplates();
                break;
            case 'info':
                if ($name === null) {
                    $this->printHelp();
                    break;
                }
                $this->printPromptInfo($name);
                break;
            case 'create':
                if ($name === null) {
                    $this->printHelp();
                    break;
                }
                $this->createPromptTemplate($name, $options);
                break;
            default:
                $this->printHelp();
                break;
        }
    }

    private function listPromptTemplates(): void
    {
        foreach ($this->getPromptRepository() as $prompt) {
            echo sprintf("- %s\n  %s\n", $prompt['name'], $prompt['description']);
        }
    }

    private function printPromptInfo(string $name): void
    {
        $prompt = $this->getPromptByName($name);
        if ($prompt === null) {
            echo "Prompt template {$name} not found.\n";
            return;
        }

        echo sprintf("Name: %s\nDescription: %s\nCategory: %s\nDefault fields: %s\n",
            $prompt['name'],
            $prompt['description'],
            $prompt['category'],
            implode(', ', $prompt['fields'])
        );
    }

    private function createPromptTemplate(string $name, array $options): void
    {
        $optionsText = !empty($options) ? implode(' ', $options) : 'none';
        echo "Creating system prompt template: {$name}\n";
        echo "Options: {$optionsText}\n";
        echo "Prompt template {$name} has been registered for future use.\n";
    }

    private function getPromptByName(string $name): ?array
    {
        foreach ($this->getPromptRepository() as $prompt) {
            if ($prompt['name'] === $name) {
                return $prompt;
            }
        }

        return null;
    }

    private function getPromptRepository(): array
    {
        return [
            [
                'name' => 'kernel-init-prompt',
                'description' => 'Kernel initialization workflow prompt for system boot and module load.',
                'category' => 'system',
                'fields' => ['boot_mode', 'module_list', 'driver_hints'],
            ],
            [
                'name' => 'network-check-prompt',
                'description' => 'Network and protocol validation prompt for endpoint checks.',
                'category' => 'network',
                'fields' => ['ip_address', 'port', 'validation_level'],
            ],
            [
                'name' => 'package-install-prompt',
                'description' => 'Package installation prompt for dependency resolution and status reporting.',
                'category' => 'package',
                'fields' => ['package_name', 'dependencies', 'confirmation'],
            ],
        ];
    }

    private function showPackageVersion(): void
    {
        $packageJson = dirname(__DIR__, 4) . '/package.json';
        $version = 'unknown';

        if (file_exists($packageJson)) {
            $content = json_decode((string) file_get_contents($packageJson), true);
            if (is_array($content) && isset($content['version'])) {
                $version = (string) $content['version'];
            }
        }

        echo "SimpleOS Package Manager version: {$version}\n";
    }

    private function listPackages(): void
    {
        foreach ($this->repository->getAllPackages() as $package) {
            echo sprintf("- %s (%s)\n  %s\n", $package->getName(), $package->getVersion(), $package->getDescription());
        }
    }

    private function installPackage(string $name): void
    {
        $this->installer->installPackage($name);
    }

    private function printPackageInfo(string $name): void
    {
        $package = $this->repository->getPackage($name);
        if ($package === null) {
            echo "Package {$name} not found.\n";
            return;
        }

        echo sprintf("Name: %s\nVersion: %s\nDescription: %s\nDependencies: %s\n",
            $package->getName(),
            $package->getVersion(),
            $package->getDescription(),
            implode(', ', $package->getDependencies()) ?: 'none'
        );
    }

    private function printHelp(): void
    {
        echo "=== SimpleOS System Manager ===\n\n";
        echo "Application commands:\n";
        echo "  php package.php app list                 - List available applications\n";
        echo "  php package.php app install <app>        - Install an application package\n";
        echo "  php package.php app remove <app>         - Remove an installed application\n";
        echo "  php package.php app info <app>           - Show application details\n";
        echo "  php package.php app run <app> [args...]  - Run an installed application\n\n";

        echo "Package commands:\n";
        echo "  php package.php list                    - List all packages\n";
        echo "  php package.php install <package>      - Install a package\n";
        echo "  php package.php info <package>         - Show package info\n";
        echo "  php package.php version                 - Show package manager version\n\n";
        
        echo "Prompt commands:\n";
        echo "  php package.php prompt list             - List registered system prompt templates\n";
        echo "  php package.php prompt info <prompt>    - Show prompt template details\n";
        echo "  php package.php prompt create <prompt> [options] - Create or register a prompt template\n\n";
        
        echo "Kernel commands:\n";
        echo "  php package.php kernel                 - Show kernel information\n";
        echo "  php package.php kernel-init            - Initialize kernel systems\n";
        echo "  php package.php kernel-status          - Show kernel status\n\n";
        
        echo "Protocol commands:\n";
        echo "  php package.php protocol               - Show protocol information\n";
        echo "  php package.php network                - Show network status\n\n";
        
        echo "Network commands:\n";
        echo "  php package.php validate-ip <ip>       - Validate IP address\n";
        echo "  php package.php endpoint <ip> <port>   - Show endpoint information\n\n";
        
        echo "System commands:\n";
        echo "  php package.php system-check           - Run comprehensive system check\n";
    }

    private function listApplications(): void
    {
        foreach ($this->appRepository->getAllPackages() as $package) {
            echo sprintf("- %s (%s)\n  %s\n", $package->getName(), $package->getVersion(), $package->getDescription());
        }
    }

    private function printApplicationInfo(string $name): void
    {
        $package = $this->appRepository->getPackage($name);
        if ($package === null) {
            echo "Application {$name} not found.\n";
            return;
        }

        echo sprintf("Name: %s\nVersion: %s\nDescription: %s\nDependencies: %s\nEntry point: %s\n",
            $package->getName(),
            $package->getVersion(),
            $package->getDescription(),
            implode(', ', $package->getDependencies()) ?: 'none',
            $package->getEntryPoint()
        );
    }

    private function showKernelInfo(): void
    {
        if (!$this->kernel->isInitialized()) {
            $this->kernel->initialize();
        }
        echo $this->kernel->getKernelInfo();
    }

    private function initializeKernel(): void
    {
        if ($this->kernel->initialize()) {
            echo "[✓] Kernel initialized successfully\n";
            echo "Modules loaded: " . count($this->kernel->getModules()) . "\n";
            echo "Drivers initialized: " . count($this->kernel->getDrivers()) . "\n";
        } else {
            echo "[✗] Failed to initialize kernel\n";
        }
    }

    private function showKernelStatus(): void
    {
        $status = $this->kernel->getStatus();
        echo "=== Kernel Status ===\n";
        echo "Initialized: " . ($status['initialized'] ? "Yes" : "No") . "\n";
        echo "Kernel Version: " . $status['kernel_version'] . "\n";
        echo "Uptime: " . $status['uptime'] . " seconds\n";
        echo "Modules Loaded: " . $status['modules_loaded'] . "\n";
        echo "Drivers Initialized: " . $status['drivers_initialized'] . "\n";
        echo "Memory Used: " . number_format($status['memory_used']) . " bytes\n";
    }

    private function showProtocolInfo(): void
    {
        $status = $this->sysProtocol->getStatus();
        echo "=== System Protocols ===\n";
        echo "Active Protocols: " . $status['protocols_active'] . "\n";
        echo "Network Interfaces Up: " . $status['interfaces_up'] . "\n";
        echo "Total Interfaces: " . $status['total_interfaces'] . "\n";
        echo "IPv4 Rules Loaded: " . $status['ipv4_rules_loaded'] . "\n\n";
        
        echo "=== Protocol Details ===\n";
        foreach ($this->sysProtocol->getProtocols() as $name => $proto) {
            echo "Protocol: {$proto['name']}\n";
            echo "  Version: {$proto['version']}\n";
            echo "  Status: {$proto['status']}\n\n";
        }
    }

    private function showNetworkStatus(): void
    {
        $status = $this->networkHelper->getNetworkStatus();
        echo "=== Network Status ===\n";
        echo "IPs Validated: " . $status['ips_validated'] . "\n";
        echo "Endpoints Cached: " . $status['endpoints_cached'] . "\n";
        echo "Well-known Ports: " . $status['well_known_ports'] . "\n\n";
        
        echo "=== Network Interfaces ===\n";
        foreach ($this->sysProtocol->getNetworkInterfaces() as $iface) {
            echo "Interface: {$iface['name']}\n";
            echo "  Type: {$iface['type']}\n";
            echo "  IPv4: {$iface['ipv4']}\n";
            echo "  Status: {$iface['status']}\n\n";
        }
    }

    private function validateIP(string $ip): void
    {
        echo "=== IP Validation ===\n";
        if ($this->networkHelper->validateIP($ip)) {
            echo "IP: {$ip}\n";
            echo "Valid: Yes\n";
            echo "Classification: " . $this->networkHelper->classifyIP($ip) . "\n";
        } else {
            echo "IP: {$ip}\n";
            echo "Valid: No\n";
        }
    }

    private function showEndpointInfo(string $ip, int $port): void
    {
        echo $this->networkHelper->formatEndpoint($ip, $port);
    }

    private function runSystemCheck(): void
    {
        $checker = new SystemConfigChecker();
        $checker->runAllChecks();
        echo $checker->getReport();
    }
}
