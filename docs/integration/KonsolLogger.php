<?php

declare(strict_types=1);

namespace DocIntegration\Logger;

use DocIntegration\Contracts\LoggerInterface;

/**
 * Renkli konsol çıktısı veren basit logger.
 */
class KonsolLogger implements LoggerInterface
{
    private const RENKLER = [
        'sifirla'  => "\033[0m",
        'bilgi'    => "\033[36m",   // Cyan
        'hata'     => "\033[31m",   // Kırmızı
        'uyari'    => "\033[33m",   // Sarı
        'debug'    => "\033[90m",   // Gri
        'basari'   => "\033[32m",   // Yeşil
    ];

    public function __construct(
        private readonly bool $renkli = true,
        private readonly bool $zamanDamgasi = true
    ) {}

    public function bilgi(string $mesaj, array $baglam = []): void
    {
        $this->yaz('BİLGİ', $mesaj, 'bilgi', $baglam);
    }

    public function hata(string $mesaj, array $baglam = []): void
    {
        $this->yaz('HATA', $mesaj, 'hata', $baglam, STDERR);
    }

    public function uyari(string $mesaj, array $baglam = []): void
    {
        $this->yaz('UYARI', $mesaj, 'uyari', $baglam);
    }

    public function debug(string $mesaj, array $baglam = []): void
    {
        $this->yaz('DEBUG', $mesaj, 'debug', $baglam);
    }

    public function basari(string $mesaj, array $baglam = []): void
    {
        $this->yaz('BASARI', $mesaj, 'basari', $baglam);
    }

    private function yaz(
        string $seviye,
        string $mesaj,
        string $renk,
        array $baglam = [],
        mixed $akis = STDOUT
    ): void {
        $zaman   = $this->zamanDamgasi ? '[' . date('H:i:s') . '] ' : '';
        $seviyeStr = sprintf('[%-6s]', $seviye);

        if ($this->renkli) {
            $r = self::RENKLER[$renk] ?? self::RENKLER['sifirla'];
            $s = self::RENKLER['sifirla'];
            $satir = "{$zaman}{$r}{$seviyeStr}{$s} {$mesaj}";
        } else {
            $satir = "{$zaman}{$seviyeStr} {$mesaj}";
        }

        if (!empty($baglam)) {
            $satir .= ' ' . json_encode($baglam, JSON_UNESCAPED_UNICODE);
        }

        fwrite($akis, $satir . PHP_EOL);
    }
}
