<?php

declare(strict_types=1);

namespace App\Script\Package;

interface PackageInterface
{
    public function getName(): string;

    public function getVersion(): string;

    public function getDescription(): string;

    public function getDependencies(): array;
}
