<?php

declare(strict_types=1);

namespace App\Script\SysProtocol;

class SysProtocol
{
    /** @var array<string, string> */
    private array $protocols = [
        'simpleos-kernel' => 'Kernel control protocol for boot, module runtime and metadata.',
        'simpleos-system' => 'System protocol for package discovery, lifecycle and service registration.',
        'simpleos-ip-check' => 'Project IP validation and network connectivity probe.',
    ];

    /** @return array<string, string> */
    public function listProtocolDetails(): array
    {
        return $this->protocols;
    }

    public function getProtocolDescription(string $name): ?string
    {
        return $this->protocols[$name] ?? null;
    }

    /** @return string[] */
    public function getSupportedProtocols(): array
    {
        return array_keys($this->protocols);
    }
}
