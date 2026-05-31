<?php

declare(strict_types=1);

namespace App\Script\Package;

class PackageManager
{
    /** @var array<string, PackageInterface> */
    private array $packages = [];

    public function addPackage(PackageInterface $package): void
    {
        $this->packages[$package->getName()] = $package;
    }

    public function getPackage(string $name): ?PackageInterface
    {
        return $this->packages[$name] ?? null;
    }

    /** @return array<string, PackageInterface> */
    public function getAllPackages(): array
    {
        return $this->packages;
    }

    public function hasPackage(string $name): bool
    {
        return isset($this->packages[$name]);
    }
}
