<?php

namespace Kernel\App\Interfaces;

interface AppInterface
{
    /**
     * Uygulamayı başlatır — tüm bootstrap'leri sırayla çalıştırır.
     */
    public function run(): void;

    /**
     * Bootstrap bileşeni ekler.
     */
    public function addBootstrap(BootstrapInterface $bootstrap): void;

    /**
     * Kayıtlı tüm bootstrap'leri döner.
     *
     * @return BootstrapInterface[]
     */
    public function getBootstraps(): array;
}
