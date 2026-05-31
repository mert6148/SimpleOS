<?php

namespace App\Script\SysProtocol;

/**
 * Network Helper
 * 
 * Provides network utilities for IP validation, endpoint management,
 * and network configuration validation.
 */
class NetworkHelper
{
    private array $validatedIPs = [];
    private array $cachedEndpoints = [];
    private array $networkRules = [];
    
    public function __construct()
    {
        $this->loadNetworkRules();
    }
    
    /**
     * Load network validation rules
     */
    private function loadNetworkRules(): void
    {
        $this->networkRules = [
            'reserved_ports' => range(0, 1023),
            'dynamic_ports' => range(1024, 65535),
            'well_known_ports' => [
                22 => 'SSH',
                25 => 'SMTP',
                53 => 'DNS',
                80 => 'HTTP',
                443 => 'HTTPS',
                3306 => 'MySQL',
                5432 => 'PostgreSQL',
                6379 => 'Redis',
                8080 => 'Proxy',
            ],
        ];
    }
    
    /**
     * Validate complete IP address
     */
    public function validateIP(string $ip): bool
    {
        // Check cache
        if (isset($this->validatedIPs[$ip])) {
            return $this->validatedIPs[$ip];
        }
        
        // Basic IPv4 validation
        $parts = explode('.', $ip);
        if (count($parts) !== 4) {
            $this->validatedIPs[$ip] = false;
            return false;
        }
        
        foreach ($parts as $part) {
            if (!is_numeric($part) || (int)$part < 0 || (int)$part > 255) {
                $this->validatedIPs[$ip] = false;
                return false;
            }
        }
        
        $this->validatedIPs[$ip] = true;
        return true;
    }
    
    /**
     * Get IP classification
     */
    public function classifyIP(string $ip): string
    {
        if (!$this->validateIP($ip)) {
            return 'invalid';
        }
        
        $parts = explode('.', $ip);
        $firstOctet = (int)$parts[0];
        
        if ($firstOctet === 127) return 'loopback';
        if ($firstOctet >= 224 && $firstOctet <= 239) return 'multicast';
        if ($firstOctet === 255) return 'broadcast';
        if ($firstOctet === 10) return 'private';
        if ($firstOctet === 172) {
            $secondOctet = (int)$parts[1];
            if ($secondOctet >= 16 && $secondOctet <= 31) return 'private';
        }
        if ($firstOctet === 192 && (int)$parts[1] === 168) return 'private';
        
        return 'public';
    }
    
    /**
     * Validate network endpoint
     */
    public function validateEndpoint(string $ip, int $port): bool
    {
        if (!$this->validateIP($ip)) {
            return false;
        }
        
        if ($port < 0 || $port > 65535) {
            return false;
        }
        
        return true;
    }
    
    /**
     * Cache and get endpoint info
     */
    public function getEndpointInfo(string $ip, int $port): ?array
    {
        $key = "{$ip}:{$port}";
        
        if (isset($this->cachedEndpoints[$key])) {
            return $this->cachedEndpoints[$key];
        }
        
        if (!$this->validateEndpoint($ip, $port)) {
            return null;
        }
        
        $info = [
            'ip' => $ip,
            'port' => $port,
            'endpoint' => $key,
            'ip_type' => $this->classifyIP($ip),
            'port_type' => $this->classifyPort($port),
            'is_privileged' => $port < 1024,
            'service' => $this->networkRules['well_known_ports'][$port] ?? null,
        ];
        
        $this->cachedEndpoints[$key] = $info;
        return $info;
    }
    
    /**
     * Classify port
     */
    private function classifyPort(int $port): string
    {
        if ($port < 1024) return 'reserved';
        if ($port <= 49151) return 'registered';
        return 'dynamic';
    }
    
    /**
     * Format endpoint for display
     */
    public function formatEndpoint(string $ip, int $port): string
    {
        $info = $this->getEndpointInfo($ip, $port);
        if ($info === null) {
            return "Invalid endpoint: {$ip}:{$port}";
        }
        
        $formatted = "Endpoint: {$info['endpoint']}\n";
        $formatted .= "IP Type: {$info['ip_type']}\n";
        $formatted .= "Port Type: {$info['port_type']}\n";
        $formatted .= "Privileged: " . ($info['is_privileged'] ? "Yes" : "No") . "\n";
        if ($info['service']) {
            $formatted .= "Service: {$info['service']}\n";
        }
        
        return $formatted;
    }
    
    /**
     * Get network status
     */
    public function getNetworkStatus(): array
    {
        return [
            'ips_validated' => count($this->validatedIPs),
            'endpoints_cached' => count($this->cachedEndpoints),
            'well_known_ports' => count($this->networkRules['well_known_ports']),
        ];
    }
}
