<?php

declare(strict_types=1);

namespace App\Script\SysProtocol;

class ProjectIpChecker
{
    private string $projectIp;
    private int $port;

    public function __construct(?string $projectIp = null, int $port = 80)
    {
        $this->projectIp = $projectIp ?? getenv('SIMPLEOS_PROJECT_IP') ?: '127.0.0.1';
        $this->port = $port;
    }

    public function getProjectIp(): string
    {
        return $this->projectIp;
    }

    public function getPort(): int
    {
        return $this->port;
    }

    public function isValid(): bool
    {
        return filter_var($this->projectIp, FILTER_VALIDATE_IP) !== false;
    }

    public function getLocalHostname(): string
    {
        return gethostname() ?: 'unknown';
    }

    /** @return string[] */
    public function getLocalAddresses(): array
    {
        $hostname = $this->getLocalHostname();
        $addresses = [];

        $resolved = gethostbynamel($hostname);
        if (is_array($resolved)) {
            $addresses = array_merge($addresses, $resolved);
        }

        if (filter_var($this->projectIp, FILTER_VALIDATE_IP)) {
            $addresses[] = $this->projectIp;
        }

        return array_values(array_unique(array_filter($addresses)));
    }

    public function checkConnectivity(int $timeout = 2): array
    {
        if (!$this->isValid()) {
            return [
                'ip' => $this->projectIp,
                'port' => $this->port,
                'reachable' => false,
                'message' => 'Invalid project IP address.',
            ];
        }

        $address = sprintf('tcp://%s:%d', $this->projectIp, $this->port);
        $connection = @stream_socket_client($address, $errno, $errstr, $timeout);

        if ($connection !== false) {
            fclose($connection);
            return [
                'ip' => $this->projectIp,
                'port' => $this->port,
                'reachable' => true,
                'message' => sprintf('Successfully connected to %s:%d.', $this->projectIp, $this->port),
            ];
        }

        return [
            'ip' => $this->projectIp,
            'port' => $this->port,
            'reachable' => false,
            'message' => sprintf('Could not connect to %s:%d (%s).', $this->projectIp, $this->port, $errstr),
        ];
    }
}
