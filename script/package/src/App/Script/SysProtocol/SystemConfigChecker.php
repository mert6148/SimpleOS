<?php

namespace App\Script\SysProtocol;

/**
 * System Configuration Checker
 * 
 * Comprehensive system health and configuration validation tool
 * for SimpleOS kernel and protocol stack.
 */
class SystemConfigChecker
{
    private array $checks = [];
    private array $results = [];
    
    public function __construct()
    {
        $this->registerChecks();
    }
    
    /**
     * Register system checks
     */
    private function registerChecks(): void
    {
        $this->checks = [
            'kernel_version' => 'Check kernel version compatibility',
            'php_version' => 'Verify PHP version requirements',
            'network_interfaces' => 'Validate network interface configuration',
            'protocol_stack' => 'Verify protocol stack is loaded',
            'memory_allocation' => 'Check memory availability',
            'port_availability' => 'Test common service ports',
            'ip_validation' => 'Verify IP validation functionality',
        ];
    }
    
    /**
     * Run all system checks
     */
    public function runAllChecks(): array
    {
        $this->results = [
            'timestamp' => date('Y-m-d H:i:s'),
            'checks' => [],
        ];
        
        foreach ($this->checks as $check => $description) {
            $this->results['checks'][$check] = $this->executeCheck($check);
        }
        
        $this->results['summary'] = $this->generateSummary();
        
        return $this->results;
    }
    
    /**
     * Execute individual system check
     */
    private function executeCheck(string $checkName): array
    {
        $result = [
            'name' => $checkName,
            'status' => 'unknown',
            'message' => '',
        ];
        
        switch ($checkName) {
            case 'kernel_version':
                $result = $this->checkKernelVersion();
                break;
            case 'php_version':
                $result = $this->checkPHPVersion();
                break;
            case 'network_interfaces':
                $result = $this->checkNetworkInterfaces();
                break;
            case 'protocol_stack':
                $result = $this->checkProtocolStack();
                break;
            case 'memory_allocation':
                $result = $this->checkMemoryAllocation();
                break;
            case 'port_availability':
                $result = $this->checkPortAvailability();
                break;
            case 'ip_validation':
                $result = $this->checkIPValidation();
                break;
        }
        
        return $result;
    }
    
    /**
     * Check kernel version
     */
    private function checkKernelVersion(): array
    {
        $kernel = \App\Kernel\Kernel::getInstance();
        $version = $kernel->getConfig('kernel_version');
        
        return [
            'name' => 'kernel_version',
            'status' => 'pass',
            'message' => "Kernel version {$version} detected",
            'details' => [
                'version' => $version,
                'architecture' => $kernel->getConfig('architecture'),
            ],
        ];
    }
    
    /**
     * Check PHP version
     */
    private function checkPHPVersion(): array
    {
        $version = phpversion();
        $required = '7.4.0';
        $status = version_compare($version, $required, '>=') ? 'pass' : 'fail';
        
        return [
            'name' => 'php_version',
            'status' => $status,
            'message' => "PHP {$version} " . ($status === 'pass' ? 'meets' : 'does not meet') . " requirement ({$required})",
            'details' => [
                'current' => $version,
                'required' => $required,
            ],
        ];
    }
    
    /**
     * Check network interfaces
     */
    private function checkNetworkInterfaces(): array
    {
        $protocol = new SysProtocol();
        $interfaces = $protocol->getNetworkInterfaces();
        $upCount = count(array_filter($interfaces, fn($i) => $i['status'] === 'up'));
        
        return [
            'name' => 'network_interfaces',
            'status' => $upCount > 0 ? 'pass' : 'fail',
            'message' => "{$upCount} network interface(s) up",
            'details' => [
                'total' => count($interfaces),
                'up' => $upCount,
                'interfaces' => array_map(fn($i) => [
                    'name' => $i['name'],
                    'type' => $i['type'],
                    'ipv4' => $i['ipv4'],
                ], $interfaces),
            ],
        ];
    }
    
    /**
     * Check protocol stack
     */
    private function checkProtocolStack(): array
    {
        $protocol = new SysProtocol();
        $protocols = $protocol->getProtocols();
        $activeCount = count(array_filter($protocols, fn($p) => $p['status'] === 'active'));
        
        return [
            'name' => 'protocol_stack',
            'status' => $activeCount > 0 ? 'pass' : 'fail',
            'message' => "{$activeCount} protocol(s) active",
            'details' => [
                'total' => count($protocols),
                'active' => $activeCount,
                'protocols' => array_keys($protocols),
            ],
        ];
    }
    
    /**
     * Check memory allocation
     */
    private function checkMemoryAllocation(): array
    {
        $used = memory_get_usage(true);
        $peak = memory_get_peak_usage(true);
        $limit = ini_get('memory_limit');
        
        return [
            'name' => 'memory_allocation',
            'status' => 'pass',
            'message' => "Memory usage: {$used} bytes",
            'details' => [
                'used' => $used,
                'peak' => $peak,
                'limit' => $limit,
            ],
        ];
    }
    
    /**
     * Check port availability
     */
    private function checkPortAvailability(): array
    {
        $testPorts = [22, 80, 443, 3306, 5432];
        $available = 0;
        
        foreach ($testPorts as $port) {
            $fp = @fsockopen('127.0.0.1', $port, $errno, $errstr, 1);
            if ($fp) {
                fclose($fp);
                $available++;
            }
        }
        
        return [
            'name' => 'port_availability',
            'status' => 'pass',
            'message' => "{$available} of " . count($testPorts) . " test ports available",
            'details' => [
                'tested_ports' => $testPorts,
                'available' => $available,
            ],
        ];
    }
    
    /**
     * Check IP validation
     */
    private function checkIPValidation(): array
    {
        $helper = new NetworkHelper();
        
        $testIPs = [
            '127.0.0.1' => 'loopback',
            '192.168.1.1' => 'private',
            '8.8.8.8' => 'public',
        ];
        
        $validCount = 0;
        foreach ($testIPs as $ip => $expected) {
            if ($helper->validateIP($ip) && $helper->classifyIP($ip) === $expected) {
                $validCount++;
            }
        }
        
        return [
            'name' => 'ip_validation',
            'status' => $validCount === count($testIPs) ? 'pass' : 'fail',
            'message' => "{$validCount} of " . count($testIPs) . " IP validation tests passed",
            'details' => [
                'passed' => $validCount,
                'total' => count($testIPs),
            ],
        ];
    }
    
    /**
     * Generate summary report
     */
    private function generateSummary(): array
    {
        $checks = $this->results['checks'] ?? [];
        $passed = count(array_filter($checks, fn($c) => $c['status'] === 'pass'));
        $failed = count(array_filter($checks, fn($c) => $c['status'] === 'fail'));
        $unknown = count(array_filter($checks, fn($c) => $c['status'] === 'unknown'));
        
        return [
            'total_checks' => count($checks),
            'passed' => $passed,
            'failed' => $failed,
            'unknown' => $unknown,
            'overall_status' => $failed === 0 ? 'healthy' : 'needs_attention',
            'success_rate' => count($checks) > 0 ? round(($passed / count($checks)) * 100, 2) : 0,
        ];
    }
    
    /**
     * Get check results
     */
    public function getResults(): array
    {
        return $this->results;
    }
    
    /**
     * Format results as report
     */
    public function getReport(): string
    {
        $report = "=== SimpleOS System Configuration Report ===\n";
        $report .= "Timestamp: {$this->results['timestamp']}\n\n";
        
        foreach ($this->results['checks'] as $check) {
            $icon = $check['status'] === 'pass' ? '[✓]' : '[✗]';
            $report .= "{$icon} {$check['name']}: {$check['message']}\n";
        }
        
        $summary = $this->results['summary'];
        $report .= "\n=== Summary ===\n";
        $report .= "Total Checks: {$summary['total_checks']}\n";
        $report .= "Passed: {$summary['passed']}\n";
        $report .= "Failed: {$summary['failed']}\n";
        $report .= "Overall Status: {$summary['overall_status']}\n";
        $report .= "Success Rate: {$summary['success_rate']}%\n";
        
        return $report;
    }
}
