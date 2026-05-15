<?php

declare(strict_types=1);

namespace SimpleOS\Docs\Log;

/**
 * Kaydedilmiş log dosyalarını okur, sorgular ve filtreler.
 *
 * Kullanım:
 *   $okuyucu = new LogOkuyucu(__DIR__ . '/storage/logs/json');
 *   $sonuclar = $okuyucu
 *       ->tarihAraliginda(new \DateTime('-7 days'), new \DateTime())
 *       ->seviyeyeGore(LogSeviye::HATA)
 *       ->yazaraGore('Mert D.')
 *       ->al();
 */
class LogOkuyucu
{
    /** @var LogKaydi[] */
    private array $kayitlar = [];

    // Filtreler
    private ?LogSeviye $filtreMinSeviye  = null;
    private ?LogSeviye $filtreMaxSeviye  = null;
    private ?string    $filtreKategori   = null;
    private ?string    $filtreYazar      = null;
    private ?string    $filtreDosya      = null;
    private ?\DateTimeInterface $baslangic = null;
    private ?\DateTimeInterface $bitis     = null;
    private ?string    $filtreMetin      = null;
    private int        $limit            = 0;    // 0 = sınırsız
    private int        $offset           = 0;
    private bool       $tersSiralamali   = true; // En yeni önce

    /**
     * @param string $jsonDizini NDJSON dosyalarının bulunduğu dizin
     */
    public function __construct(private readonly string $jsonDizini)
    {
        $this->yukle();
    }

    // ─── Filtreler (Fluent API) ────────────────────────────

    public function seviyeyeGore(LogSeviye $seviye): static
    {
        $this->filtreMinSeviye = $seviye;
        $this->filtreMaxSeviye = $seviye;
        return $this;
    }

    public function minimumSeviye(LogSeviye $seviye): static
    {
        $this->filtreMinSeviye = $seviye;
        return $this;
    }

    public function kategoriyeGore(string $kategori): static
    {
        $this->filtreKategori = $kategori;
        return $this;
    }

    public function yazaraGore(string $yazar): static
    {
        $this->filtreYazar = $yazar;
        return $this;
    }

    public function dosyayaGore(string $dosya): static
    {
        $this->filtreDosya = $dosya;
        return $this;
    }

    public function tarihAraliginda(
        \DateTimeInterface $baslangic,
        \DateTimeInterface $bitis
    ): static {
        $this->baslangic = $baslangic;
        $this->bitis     = $bitis;
        return $this;
    }

    public function metinAra(string $metin): static
    {
        $this->filtreMetin = $metin;
        return $this;
    }

    public function limit(int $limit, int $offset = 0): static
    {
        $this->limit  = $limit;
        $this->offset = $offset;
        return $this;
    }

    public function eskiOnce(): static
    {
        $this->tersSiralamali = false;
        return $this;
    }

    // ─── Sonuçlar ─────────────────────────────────────────

    /** @return LogKaydi[] */
    public function al(): array
    {
        $sonuclar = array_filter($this->kayitlar, fn(LogKaydi $k) => $this->eslesiyor($k));
        $sonuclar = array_values($sonuclar);

        // Sıralama
        usort($sonuclar, function(LogKaydi $a, LogKaydi $b) {
            $fark = $a->zaman <=> $b->zaman;
            return $this->tersSiralamali ? -$fark : $fark;
        });

        // Sayfalama
        if ($this->offset > 0 || $this->limit > 0) {
            $sonuclar = array_slice($sonuclar, $this->offset, $this->limit ?: null);
        }

        return $sonuclar;
    }

    /** Filtrelenmiş toplam kayıt sayısını döner */
    public function say(): int
    {
        return count(array_filter($this->kayitlar, fn(LogKaydi $k) => $this->eslesiyor($k)));
    }

    /** Kayıtları düz metin olarak döner */
    public function metinOlarakAl(): string
    {
        return implode(PHP_EOL, array_map(
            fn(LogKaydi $k) => $k->metinSatiri(),
            $this->al()
        ));
    }

    /** Kayıtları JSON dizisi olarak döner */
    public function jsonOlarakAl(): string
    {
        return json_encode(
            array_map(fn(LogKaydi $k) => $k->diziOlarakAl(), $this->al()),
            JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES
        );
    }

    /**
     * Özet istatistikler döner.
     *
     * @return array{toplam:int, seviyeler:array<string,int>, kategoriler:array<string,int>, yazarlar:array<string,int>}
     */
    public function ozet(): array
    {
        $kayitlar = $this->al();
        $seviyeler = $kategoriler = $yazarlar = [];

        foreach ($kayitlar as $k) {
            $seviyeler[$k->seviye->etiketi()]  = ($seviyeler[$k->seviye->etiketi()]  ?? 0) + 1;
            $kategoriler[$k->kategori]          = ($kategoriler[$k->kategori]          ?? 0) + 1;
            if ($k->yazar) {
                $yazarlar[$k->yazar] = ($yazarlar[$k->yazar] ?? 0) + 1;
            }
        }

        arsort($seviyeler);
        arsort($kategoriler);
        arsort($yazarlar);

        return [
            'toplam'     => count($kayitlar),
            'seviyeler'  => $seviyeler,
            'kategoriler'=> $kategoriler,
            'yazarlar'   => $yazarlar,
        ];
    }

    // ─── Özel ─────────────────────────────────────────────

    private function yukle(): void
    {
        if (!is_dir($this->jsonDizini)) return;

        foreach (glob($this->jsonDizini . '/*.ndjson') ?: [] as $dosya) {
            $satirlar = file($dosya, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) ?: [];
            foreach ($satirlar as $satir) {
                $veri = json_decode($satir, true);
                if (is_array($veri)) {
                    try {
                        $this->kayitlar[] = LogKaydi::diziden($veri);
                    } catch (\Throwable) {
                        // Bozuk satırı atla
                    }
                }
            }
        }
    }

    private function eslesiyor(LogKaydi $k): bool
    {
        if ($this->filtreMinSeviye && !$k->seviye->enAzindan($this->filtreMinSeviye)) return false;
        if ($this->filtreMaxSeviye && $k->seviye->value < $this->filtreMaxSeviye->value) return false;
        if ($this->filtreKategori && !str_starts_with($k->kategori, $this->filtreKategori)) return false;
        if ($this->filtreYazar && $k->yazar !== $this->filtreYazar) return false;
        if ($this->filtreDosya && !str_contains($k->dosya ?? '', $this->filtreDosya)) return false;
        if ($this->baslangic && $k->zaman < $this->baslangic) return false;
        if ($this->bitis   && $k->zaman > $this->bitis)   return false;
        if ($this->filtreMetin
            && !str_contains(strtolower($k->mesaj), strtolower($this->filtreMetin))) {
            return false;
        }
        return true;
    }
}
