<?php

declare(strict_types=1);

namespace App\Script\Package;

class Package implements PackageInterface
{
    private string $name;
    private string $version;
    private string $description;
    private array $dependencies;

    public function __construct(string $name, string $version, string $description, array $dependencies = [])
    {
        $this->name = $name;
        $this->version = $version;
        $this->description = $description;
        $this->dependencies = array_values($dependencies);
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
