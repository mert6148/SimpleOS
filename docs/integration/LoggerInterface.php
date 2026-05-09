<?php

declare(strict_types=1);

namespace DocIntegration\Contracts;

interface LoggerInterface
{
    public function bilgi(string $mesaj, array $baglam = []): void;
    public function hata(string $mesaj, array $baglam = []): void;
    public function uyari(string $mesaj, array $baglam = []): void;
    public function debug(string $mesaj, array $baglam = []): void;
}
