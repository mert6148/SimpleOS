<?php

declare(strict_types=1);

namespace App\Script\Package;

class ApplicationPackage extends Package
{
    private string $entryPoint;

    public function __construct(
        string $name,
        string $version,
        string $description,
        array $dependencies = [],
        string $entryPoint = ''
    ) {
        parent::__construct($name, $version, $description, $dependencies);
        $this->entryPoint = $entryPoint;
    }

    public function getEntryPoint(): string
    {
        return $this->entryPoint;
    }
}
