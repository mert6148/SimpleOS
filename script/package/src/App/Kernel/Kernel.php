<?php

namespace App\Kernel;

/**
 * Kernel System Manager
 * 
 * Manages kernel configuration, system information, and operational state
 * for SimpleOS.
 */
class Kernel
{
    private static ?Kernel $instance = null;
    
    private array $config = [];
    private array $systemInfo = [];
    private array $modules = [];
    private array $drivers = [];
    private bool $initialized = false;
    private string $rootPath = '';
    
    private function __construct()
    {
        $this->rootPath = dirname(dirname(dirname(dirname(__DIR__))));
        $this->loadConfig();
        $this->discoverSystemInfo();
    }
    
    /**
     * Get singleton instance
     */
    public static function getInstance(): self
    {
        if (self::$instance === null) {
            self::$instance = new self();
        }
        return self::$instance;
    }
    
    /**
     * Initialize kernel systems
     */
    public function initialize(): bool
    {
        if ($this->initialized) {
            return true;
        }
        
        // Load kernel modules
        $this->loadModules();
        
        // Initialize drivers
        $this->initializeDrivers();
        
        $this->initialized = true;
        return true;
    }
    
    /**
     * Load kernel configuration
     */
    private function loadConfig(): void
    {
        $this->config = [
            'kernel_version' => '1.0.0',
            'build_date' => '2026-05-30',
            'architecture' => 'x86_64',
            'memory_model' => 'buddy_allocator',
            'scheduler' => 'priority_round_robin',
            'max_tasks' => 256,
            'max_priority_levels' => 6,
            'page_size' => 4096,
            'max_memory' => 2147483648, // 2GB
            'enable_paging' => true,
            'enable_interrupt_handling' => true,
            'tick_rate' => 100,
        ];
    }
    
    /**
     * Discover system information
     */
    private function discoverSystemInfo(): void
    {
        $this->systemInfo = [
            'os_name' => 'SimpleOS',
            'os_version' => $this->config['kernel_version'],
            'architecture' => $this->config['architecture'],
            'boot_time' => time(),
            'uptime_seconds' => 0,
            'hostname' => gethostname() ?: 'simpleos-kernel',
            'platform' => php_uname('s'),
            'php_version' => phpversion(),
            'memory_total' => function_exists('memory_get_peak_usage') 
                ? memory_get_peak_usage(true) 
                : 0,
        ];
    }
    
    /**
     * Load kernel modules
     */
    private function loadModules(): void
    {
        $this->modules = [
            'memory_manager' => [
                'name' => 'Memory Manager',
                'version' => '1.0.0',
                'status' => 'loaded',
                'type' => 'core',
            ],
            'scheduler' => [
                'name' => 'Task Scheduler',
                'version' => '1.0.0',
                'status' => 'loaded',
                'type' => 'core',
            ],
            'vfs' => [
                'name' => 'Virtual File System',
                'version' => '1.0.0',
                'status' => 'loaded',
                'type' => 'core',
            ],
            'interrupt_handler' => [
                'name' => 'Interrupt Handler',
                'version' => '1.0.0',
                'status' => 'loaded',
                'type' => 'core',
            ],
            'device_manager' => [
                'name' => 'Device Manager',
                'version' => '1.0.0',
                'status' => 'loaded',
                'type' => 'subsystem',
            ],
        ];
    }
    
    /**
     * Initialize drivers
     */
    private function initializeDrivers(): void
    {
        $this->drivers = [
            'pic' => [
                'name' => 'Programmable Interrupt Controller',
                'version' => '1.0.0',
                'status' => 'initialized',
                'irq_count' => 16,
            ],
            'pit' => [
                'name' => 'Programmable Interval Timer',
                'version' => '1.0.0',
                'status' => 'initialized',
                'tick_rate' => $this->config['tick_rate'],
            ],
            'serial' => [
                'name' => 'Serial Port Driver',
                'version' => '1.0.0',
                'status' => 'initialized',
                'port' => '0x3F8',
            ],
        ];
    }
    
    /**
     * Get kernel configuration
     */
    public function getConfig(?string $key = null): mixed
    {
        if ($key === null) {
            return $this->config;
        }
        return $this->config[$key] ?? null;
    }
    
    /**
     * Get system information
     */
    public function getSystemInfo(?string $key = null): mixed
    {
        if ($key === null) {
            return $this->systemInfo;
        }
        return $this->systemInfo[$key] ?? null;
    }
    
    /**
     * Get loaded modules
     */
    public function getModules(): array
    {
        return $this->modules;
    }
    
    /**
     * Get initialized drivers
     */
    public function getDrivers(): array
    {
        return $this->drivers;
    }
    
    /**
     * Get kernel status
     */
    public function getStatus(): array
    {
        return [
            'initialized' => true,
            'kernel_version' => $this->config['kernel_version'],
            'uptime' => time() - $this->systemInfo['boot_time'],
            'modules_loaded' => count($this->modules),
            'drivers_initialized' => count($this->drivers),
            'tasks_running' => 0,
            'memory_used' => memory_get_usage(true),
            'memory_available' => $this->config['max_memory'],
        ];
    }
    
    /**
     * Get kernel information as formatted string
     */
    public function getKernelInfo(): string
    {
        $info = "=== SimpleOS Kernel Information ===\n";
        $info .= sprintf("Kernel Version: %s\n", $this->config['kernel_version']);
        $info .= sprintf("Build Date: %s\n", $this->config['build_date']);
        $info .= sprintf("Architecture: %s\n", $this->config['architecture']);
        $info .= sprintf("Boot Time: %s\n", date('Y-m-d H:i:s', $this->systemInfo['boot_time']));
        $info .= sprintf("Hostname: %s\n", $this->systemInfo['hostname']);
        $info .= sprintf("Modules Loaded: %d\n", count($this->modules));
        $info .= sprintf("Drivers Initialized: %d\n", count($this->drivers));
        
        return $info;
    }
    
    /**
     * Check if kernel is initialized
     */
    public function isInitialized(): bool
    {
        return $this->initialized;
    }
    
    /**
     * Get root path
     */
    public function getRootPath(): string
    {
        return $this->rootPath;
    }
}
