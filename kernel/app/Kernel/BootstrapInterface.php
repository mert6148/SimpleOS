<?php

namespace Kernel\App\Interfaces;

interface BootstrapInterface
{
    /**
     * Bootstrap bileşenini başlatır.
     * Uygulama yüklenirken bir kez çağrılır.
     */
    public function bootstrap(): void;

    /**
     * Bootstrap'in öncelik sırasını döner.
     * Düşük sayı = erken çalışır.
     */
    public function getOncelik(): int;

    /**
     * Bootstrap'in insan okunabilir adını döner.
     */
    public function getAd(): string;
}
