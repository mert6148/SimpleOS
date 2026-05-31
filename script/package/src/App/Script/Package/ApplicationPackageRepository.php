<?php

declare(strict_types=1);

namespace App\Script\Package;

class ApplicationPackageRepository
{
    /** @var array<string, ApplicationPackage> */
    private array $packages = [];

    public function addPackage(ApplicationPackage $package): void
    {
        $this->packages[$package->getName()] = $package;
    }

    public function getPackage(string $name): ?ApplicationPackage
    {
        return $this->packages[$name] ?? null;
    }

    /** @return array<string, ApplicationPackage> */
    public function getAllPackages(): array
    {
        return $this->packages;
    }

    public function loadFromArray(array $definitions): void
    {
        foreach ($definitions as $definition) {
            if (!isset($definition['name'], $definition['version'], $definition['description'])) {
                continue;
            }

            $package = new ApplicationPackage(
                (string)$definition['name'], 
                (string)$definition['version'], 
                (string)$definition['description'], 
                isset($definition['dependencies']) && is_array($definition['dependencies']) ? $definition['dependencies'] : [],
                isset($definition['entryPoint']) ? (string)$definition['entryPoint'] : $definition['name']
            );

            $this->addPackage($package);
        }
    }
}
