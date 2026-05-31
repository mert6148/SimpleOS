<?php

declare(strict_types=1);

namespace App\Script\Package;

class PackageInstaller
{
    private PackageManager $packageManager;
    private array $installed = [];

    public function __construct(PackageManager $packageManager)
    {
        $this->packageManager = $packageManager;
    }

    public function installPackage(string $name): void
    {
        if ($this->isInstalled($name)) {
            echo "Package {$name} is already installed.\n";
            return;
        }

        $package = $this->packageManager->getPackage($name);
        if ($package === null) {
            echo "Package {$name} not found.\n";
            return;
        }

        foreach ($package->getDependencies() as $dependency) {
            $this->installPackage($dependency);
        }

        $this->installed[] = $package->getName();
        echo "Installing package: {$package->getName()} ({$package->getVersion()})\n";
    }

    public function isInstalled(string $name): bool
    {
        return in_array($name, $this->installed, true);
    }

    /** @return string[] */
    public function getInstalledPackages(): array
    {
        return $this->installed;
    }
}
