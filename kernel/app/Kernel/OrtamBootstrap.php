<?php

namespace Kernel\App\Bootstraps;

/**
 * Ortam değişkenlerini yükler (.env dosyasından veya sistem ortamından).
 * Öncelik: 10 — ilk çalışması gereken bootstrap.
 */
class OrtamBootstrap extends AbstractBootstrap
{
    private string $envDosyasi;

    public function __construct(string $envDosyasi = '.env')
    {
        $this->envDosyasi = $envDosyasi;
    }

    public function bootstrap(): void
    {
        if (!file_exists($this->envDosyasi)) {
            // Dosya yoksa sessizce devam et — production ortamda sistem env kullanılır
            return;
        }

        $satirlar = file($this->envDosyasi, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

        foreach ($satirlar as $satir) {
            // Yorum satırlarını atla
            if (str_starts_with(trim($satir), '#')) {
                continue;
            }

            if (!str_contains($satir, '=')) {
                continue;
            }

            [$anahtar, $deger] = explode('=', $satir, 2);
            $anahtar = trim($anahtar);
            $deger   = trim($deger, " \t\n\r\0\x0B\"'");

            // Sadece henüz tanımlı değilse ayarla
            if (!array_key_exists($anahtar, $_ENV)) {
                $_ENV[$anahtar]  = $deger;
                putenv("{$anahtar}={$deger}");
            }
        }
    }

    public function getOncelik(): int
    {
        return 10;
    }
}
