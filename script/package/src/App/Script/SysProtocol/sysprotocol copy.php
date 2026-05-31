<?php

namespace App\Script\Sys;

/**
 * System Protocol Handler
 * 
 * Manages system-level protocols including network communication,
 * IP validation, and inter-process messaging.
 */
class SysProtocol
{
    private array $protocols = [];
    private array $ipRules = [];
    private array $networkInterfaces = [];
    
    public function __construct()
    {
        $this->initializeProtocols();
        $this->loadIPRules();
        $this->discoverNetworkInterfaces();
    }
    
    /**
     * Initialize system protocols
     */
    private function initializeProtocols(): void
    {
        $this->protocols = [
            'tcp' => [
                'name' => 'Transmission Control Protocol',
                'version' => 4,
                'port_range' => [0, 65535],
                'status' => 'active',
            ],
            'udp' => [
                'name' => 'User Datagram Protocol',
                'version' => 4,
                'port_range' => [0, 65535],
                'status' => 'active',
            ],
            'icmp' => [
                'name' => 'Internet Control Message Protocol',
                'version' => 4,
                'status' => 'active',
            ],
            'ipv4' => [
                'name' => 'Internet Protocol v4',
                'version' => 4,
                'address_bits' => 32,
                'status' => 'active',
            ],
        ];
    }
    
    /**
     * Load IP validation rules
     */
    private function loadIPRules(): void
    {
        $this->ipRules = [
            'ipv4_private_ranges' => [
                '10.0.0.0/8',
                '172.16.0.0/12',
                '192.168.0.0/16',
            ],
            'ipv4_loopback' => '127.0.0.0/8',
            'ipv4_broadcast' => '255.255.255.255',
            'ipv4_multicast' => '224.0.0.0/4',
        ];
    }
    
    /**
     * Discover network interfaces
     */
    private function discoverNetworkInterfaces(): void
    {
        if (function_exists('gethostbyname')) {
            $hostname = gethostname() ?: 'localhost';
            $ipaddr = gethostbyname($hostname);
            
            $this->networkInterfaces[] = [
                'name' => 'lo0',
                'type' => 'loopback',
                'ipv4' => '127.0.0.1',
                'netmask' => '255.0.0.0',
                'status' => 'up',
            ];
            
            if ($ipaddr !== $hostname && $ipaddr !== '127.0.0.1') {
                $this->networkInterfaces[] = [
                    'name' => 'eth0',
                    'type' => 'ethernet',
                    'ipv4' => $ipaddr,
                    'hostname' => $hostname,
                    'status' => 'up',
                ];
            }
        }
    }
    
    /**
     * Validate IPv4 address
     */
    public function validateIPv4(string $ip): bool
    {
        if (!filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4)) {
            return false;
        }
        
        // Ensure it's not in invalid ranges
        return !$this->isBroadcast($ip);
    }
    
    /**
     * Check if IP is private
     */
    public function isPrivateIP(string $ip): bool
    {
        if (!$this->validateIPv4($ip)) {
            return false;
        }
        
        foreach ($this->ipRules['ipv4_private_ranges'] as $range) {
            if ($this->ipInRange($ip, $range)) {
                return true;
            }
        }
        
        return $this->ipInRange($ip, $this->ipRules['ipv4_loopback']);
    }
    
    /**
     * Check if IP is broadcast
     */
    public function isBroadcast(string $ip): bool
    {
        return $ip === $this->ipRules['ipv4_broadcast'];
    }
    
    /**
     * Check if IP is in CIDR range
     */
    private function ipInRange(string $ip, string $range): bool
    {
        if (strpos($range, '/') === false) {
            return $ip === $range;
        }
        
        [$subnet, $bits] = explode('/', $range);
        $ip_long = ip2long($ip);
        $subnet_long = ip2long($subnet);
        $mask = -1 << (32 - (int)$bits);
        
        return ($ip_long & $mask) === ($subnet_long & $mask);
    }
    
    /**
     * Validate port
     */
    public function validatePort(int $port): bool
    {
        return $port >= 0 && $port <= 65535;
    }
    
    /**
     * Check if port is privileged (< 1024)
     */
    public function isPrivilegedPort(int $port): bool
    {
        return $port < 1024;
    }
    
    /**
     * Validate IP and port combination
     */
    public function validateEndpoint(string $ip, int $port): bool
    {
        return $this->validateIPv4($ip) && $this->validatePort($port);
    }
    
    /**
     * Get network interfaces
     */
    public function getNetworkInterfaces(): array
    {
        return $this->networkInterfaces;
    }
    
    /**
     * Get active protocols
     */
    public function getProtocols(): array
    {
        return $this->protocols;
    }
    
    /**
     * Get protocol info
     */
    public function getProtocolInfo(string $protocol): ?array
    {
        return $this->protocols[strtolower($protocol)] ?? null;
    }
    
    /**
     * Format IP info for display
     */
    public function formatIPInfo(string $ip): string
    {
        $info = "IP Address: {$ip}\n";
        $info .= "Valid: " . ($this->validateIPv4($ip) ? "Yes" : "No") . "\n";
        $info .= "Private: " . ($this->isPrivateIP($ip) ? "Yes" : "No") . "\n";
        $info .= "Broadcast: " . ($this->isBroadcast($ip) ? "Yes" : "No") . "\n";
        
        return $info;
    }
    
    /**
     * Get system protocol status
     */
    public function getStatus(): array
    {
        return [
            'protocols_active' => count(array_filter($this->protocols, 
                fn($p) => $p['status'] === 'active')),
            'interfaces_up' => count(array_filter($this->networkInterfaces,
                fn($i) => $i['status'] === 'up')),
            'total_interfaces' => count($this->networkInterfaces),
            'ipv4_rules_loaded' => count($this->ipRules),
        ];
    }
}
