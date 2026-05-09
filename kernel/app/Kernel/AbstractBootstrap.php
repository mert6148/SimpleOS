<?php

namespace Kernel\App\Bootstraps;

use Kernel\App\Interfaces\BootstrapInterface;

/**
 * Tüm bootstrap bileşenleri için temel sınıf.
 * Varsayılan öncelik ve ad davranışı sağlar.
 */
abstract class AbstractBootstrap implements BootstrapInterface
{
    /**
     * Varsayılan öncelik: 100.
     * Alt sınıflar override ederek sırayı değiştirebilir.
     */
    public function getOncelik(): int
    {
        return 100;
    }

    /**
     * Varsayılan ad: sınıf adı (namespace olmadan).
     */
    public function getAd(): string
    {
        $parcalar = explode('\\', static::class);
        return end($parcalar);
    }
}
