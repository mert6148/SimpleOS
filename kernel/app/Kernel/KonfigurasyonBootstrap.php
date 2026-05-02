<?php

namespace Kernel\App\Bootstraps;

use Kernel\Config\Konfigurasyon;

/**
 * Konfigürasyon dosyalarını yükler.
 * Öncelik: 30.
 */
class KonfigurasyonBootstrap extends AbstractBootstrap
{
    private Konfigurasyon $konfigurasyon;
    private string $configDizini;

    public function __construct(Konfigurasyon $konfigurasyon, string $configDizini = 'config')
    {
        $this->konfigurasyon = $konfigurasyon;
        $this->configDizini  = rtrim($configDizini, '/');
    }

    public function bootstrap(): void
    {
        if (!is_dir($this->configDizini)) {
            return;
        }

        $dosyalar = glob($this->configDizini . '/*.php');

        if ($dosyalar === false) {
            return;
        }

        foreach ($dosyalar as $dosya) {
            $anahtar = basename($dosya, '.php');
            $veri    = require $dosya;

            if (is_array($veri)) {
                $this->konfigurasyon->yukle($anahtar, $veri);
            }
        }
    }

    public function getOncelik(): int
    {
        return 30;
    }
}
