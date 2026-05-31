<?php

declare(strict_types=1);

namespace App\Script\Package;

class PackageRepository
{
    private PackageManager $packageManager;

    public function __construct(PackageManager $packageManager)
    {
        $this->packageManager = $packageManager;
    }

    public function addPackage(PackageInterface $package): void
    {
        $this->packageManager->addPackage($package);
    }

    public function getPackage(string $name): ?PackageInterface
    {
        return $this->packageManager->getPackage($name);
    }

    /** @return array<string, PackageInterface> */
    public function getAllPackages(): array
    {
        return $this->packageManager->getAllPackages();
    }

    public function loadFromArray(array $definitions): void
    {
        foreach ($definitions as $definition) {
            if (!isset($definition['name'], $definition['version'], $definition['description'])) {
                continue;
            }

            $package = new Package(
                (string) $definition['name'],
                (string) $definition['version'],
                (string) $definition['description'],
                isset($definition['dependencies']) && is_array($definition['dependencies'])
                    ? $definition['dependencies']
                    : []
            );

            $this->addPackage($package);
        }
    }
}
