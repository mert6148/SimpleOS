<?php

declare(strict_types=1);

namespace App\Script\Package;

class ApplicationPackageInstaller
{
    private PackageInstaller $systemInstaller;
    private ApplicationPackageRepository $repository;
    private array $installed = [];
    private string $storageFile;

    public function __construct(ApplicationPackageRepository $repository, PackageInstaller $systemInstaller)
    {
        $this->repository = $repository;
        $this->systemInstaller = $systemInstaller;
        $this->storageFile = dirname(__DIR__, 4) . '/installed_apps.json';
        $this->loadInstalledApplications();
    }

    private function loadInstalledApplications(): void
    {
        if (!file_exists($this->storageFile)) {
            $this->installed = [];
            return;
        }

        $decoded = json_decode((string) file_get_contents($this->storageFile), true);
        $this->installed = is_array($decoded) ? array_values($decoded) : [];
    }

    private function saveInstalledApplications(): void
    {
        file_put_contents($this->storageFile, json_encode($this->installed, JSON_PRETTY_PRINT));
    }

    public function installApplication(string $name): void
    {
        if ($this->isInstalled($name)) {
            echo "Application {$name} is already installed.\n";
            return;
        }

        $package = $this->repository->getPackage($name);
        if ($package === null) {
            echo "Application {$name} not found.\n";
            return;
        }

        foreach ($package->getDependencies() as $dependency) {
            $this->systemInstaller->installPackage($dependency);
        }

        $this->installed[] = $package->getName();
        $this->saveInstalledApplications();
        echo "Installing application: {$package->getName()} ({$package->getVersion()})\n";
        echo "Entry point: {$package->getEntryPoint()}\n";
    }

    public function removeApplication(string $name): void
    {
        if (!$this->isInstalled($name)) {
            echo "Application {$name} is not installed.\n";
            return;
        }

        $this->installed = array_values(array_filter($this->installed, static fn(string $installed) => $installed !== $name));
        $this->saveInstalledApplications();
        echo "Removed application: {$name}\n";
    }

    public function runApplication(string $name, array $args = []): void
    {
        if (!$this->isInstalled($name)) {
            echo "Application {$name} is not installed. Install it first.\n";
            return;
        }

        $package = $this->repository->getPackage($name);
        if ($package === null) {
            echo "Application {$name} not found.\n";
            return;
        }

        echo "Launching application: {$package->getName()}\n";
        echo "Entry point: {$package->getEntryPoint()}\n";
        if (!empty($args)) {
            echo "Arguments: " . implode(' ', $args) . "\n";
        }
    }

    public function isInstalled(string $name): bool
    {
        return in_array($name, $this->installed, true);
    }

    /** @return string[] */
    public function getInstalledApplications(): array
    {
        return $this->installed;
    }
}
