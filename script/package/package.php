<?php

namespace App\Script\Package;

use App\Script\Package\PackageInterface;

class Package implements PackageInterface
{
    protected string $name;

    protected string $version;

    protected string $description;

    protected array $dependencies = [];

    public function __construct(string $name, string $version, string $description, array $dependencies = [])
    {
        $this->name = $name;
        $this->version = $version;
        $this->description = $description;
        $this->dependencies = $dependencies;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function getVersion(): string
    {
        return $this->version;
    }

    public function getDescription(): string
    {
        return $this->description;
    }

    public function getDependencies(): array
    {
        return $this->dependencies;
    }
}

class PackageManager
{
    protected array $packages = [];

    public function addPackage(PackageInterface $package): void
    {
        $this->packages[$package->getName()] = $package;
    }

    public function getPackage(string $name): ?PackageInterface
    {
        return $this->packages[$name] ?? null;
    }

    public function getAllPackages(): array
    {
        return $this->packages;
    }
}

class PackageInstaller
{
    protected PackageManager $packageManager;

    public function __construct(PackageManager $packageManager)
    {
        $this->packageManager = $packageManager;
    }

    public function installPackage(string $name): void
    {
        $package = $this->packageManager->getPackage($name);
        if (!$package) {
            echo "Package {$name} not found.\n";
            return;
        }

        foreach ($package->getDependencies() as $dependency) {
            $this->installPackage($dependency);
        }

        echo "Installing package: {$package->getName()} ({$package->getVersion()})\n";
    }
}