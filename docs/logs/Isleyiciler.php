<?php

declare(strict_types=1);

namespace SimpleOS\Docs\Log;

// ═══════════════════════════════════════════════════════════
// ARAYÜZ
// ═══════════════════════════════════════════════════════════

/**
 * Tüm log işleyicilerinin uygulaması gereken sözleşme.
 */
interface LogIsleyiciInterface
{
    public function isle(LogKaydi $kayit): void;
    public function minimumSeviyeAyarla(LogSeviye $seviye): static;
}

// ═══════════════════════════════════════════════════════════
// TEMEL İŞLEYİCİ
// ═══════════════════════════════════════════════════════════

abstract class TemelIsleyici implements LogIsleyiciInterface
{
    protected LogSeviye $minimumSeviye;

    public function __construct(LogSeviye $minimumSeviye = LogSeviye::DEBUG)
    {
        $this->minimumSeviye = $minimumSeviye;
    }

    public function minimumSeviyeAyarla(LogSeviye $seviye): static
    {
        $this->minimumSeviye = $seviye;
        return $this;
    }

    final public function isle(LogKaydi $kayit): void
    {
        if (!$kayit->seviye->enAzindan($this->minimumSeviye)) {
            return;
        }
        $this->yaz($kayit);
    }

    abstract protected function yaz(LogKaydi $kayit): void;
}

// ═══════════════════════════════════════════════════════════
// DOSYA İŞLEYİCİSİ
// ═══════════════════════════════════════════════════════════

/**
 * Log kayıtlarını dönen dosyalara yazar.
 */
class DosyaIsleyici extends TemelIsleyici
{
    public const ROTASYON_GUNLUK   = 'Y-m-d';
    public const ROTASYON_HAFTALIK = 'Y-\WW';
    public const ROTASYON_AYLIK    = 'Y-m';
    public const ROTASYON_YOK      = '';

    private ?resource $akis = null;
    private string $mevcutDosyaYolu = '';

    public function __construct(
        private readonly string $dizin,
        private readonly string $onEk            = 'simpleos-docs',
        private readonly string $uzanti           = '.log',
        private readonly string $rotasyonFormati  = self::ROTASYON_GUNLUK,
        private readonly int    $maksBoyutBayt    = 10 * 1024 * 1024,
        LogSeviye $minimumSeviye = LogSeviye::DEBUG
    ) {
        parent::__construct($minimumSeviye);
        if (!is_dir($dizin) && !mkdir($dizin, 0755, true)) {
            throw new \RuntimeException("Log dizini oluşturulamadı: {$dizin}");
        }
    }

    protected function yaz(LogKaydi $kayit): void
    {
        $yol = $this->dosyaYoluHesapla();
        $this->akisAc($yol);
        fwrite($this->akis, $kayit->metinSatiri() . PHP_EOL);
    }

    public function __destruct()
    {
        if (is_resource($this->akis)) fclose($this->akis);
    }

    private function dosyaYoluHesapla(): string
    {
        $sonek = $this->rotasyonFormati ? '-' . date($this->rotasyonFormati) : '';
        return $this->dizin . DIRECTORY_SEPARATOR . $this->onEk . $sonek . $this->uzanti;
    }

    private function akisAc(string $yol): void
    {
        if ($this->mevcutDosyaYolu !== $yol
            || (is_resource($this->akis) && $this->boyutAsimi($yol))
        ) {
            if (is_resource($this->akis)) fclose($this->akis);
            $this->akis = null;
        }
        if (!is_resource($this->akis)) {
            $this->akis = fopen($yol, 'a');
            if (!$this->akis) {
                throw new \RuntimeException("Log dosyası açılamadı: {$yol}");
            }
            $this->mevcutDosyaYolu = $yol;
        }
    }

    private function boyutAsimi(string $yol): bool
    {
        return file_exists($yol) && filesize($yol) >= $this->maksBoyutBayt;
    }
}

// ═══════════════════════════════════════════════════════════
// JSON İŞLEYİCİSİ (NDJSON)
// ═══════════════════════════════════════════════════════════

class JsonIsleyici extends TemelIsleyici
{
    private ?resource $akis = null;

    public function __construct(
        private readonly string $dosyaYolu,
        LogSeviye $minimumSeviye = LogSeviye::DEBUG
    ) {
        parent::__construct($minimumSeviye);
        $dizin = dirname($dosyaYolu);
        if (!is_dir($dizin) && !mkdir($dizin, 0755, true)) {
            throw new \RuntimeException("Dizin oluşturulamadı: {$dizin}");
        }
        $this->akis = fopen($dosyaYolu, 'a');
        if (!$this->akis) {
            throw new \RuntimeException("JSON log dosyası açılamadı: {$dosyaYolu}");
        }
    }

    protected function yaz(LogKaydi $kayit): void
    {
        fwrite(
            $this->akis,
            json_encode(
                $kayit->diziOlarakAl(),
                JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES
            ) . PHP_EOL
        );
    }

    public function __destruct()
    {
        if (is_resource($this->akis)) fclose($this->akis);
    }
}

// ═══════════════════════════════════════════════════════════
// KONSOL İŞLEYİCİSİ
// ═══════════════════════════════════════════════════════════

class KonsolIsleyici extends TemelIsleyici
{
    public function __construct(
        private readonly bool $renkli = true,
        LogSeviye $minimumSeviye = LogSeviye::DEBUG
    ) {
        parent::__construct($minimumSeviye);
    }

    protected function yaz(LogKaydi $kayit): void
    {
        $akis = $kayit->seviye->value <= LogSeviye::HATA->value ? STDERR : STDOUT;
        fwrite($akis, $kayit->metinSatiri($this->renkli) . PHP_EOL);
    }
}

// ═══════════════════════════════════════════════════════════
// HTTP (text/plain) İŞLEYİCİSİ
// ═══════════════════════════════════════════════════════════

/**
 * Orijinal log.php'deki header() mantığının temiz uygulaması.
 */
class HttpIsleyici extends TemelIsleyici
{
    private bool $baslikGonderildi = false;

    /** @var LogKaydi[] */
    private array $tampon = [];

    public function __construct(
        private readonly bool $tamponla = true,
        LogSeviye $minimumSeviye = LogSeviye::BILGI
    ) {
        parent::__construct($minimumSeviye);
    }

    protected function yaz(LogKaydi $kayit): void
    {
        if ($this->tamponla) {
            $this->tampon[] = $kayit;
        } else {
            $this->basliklarGonder();
            echo $kayit->metinSatiri() . PHP_EOL;
        }
    }

    /** Tampondaki tüm kayıtları HTTP yanıtı olarak yayar. */
    public function gonder(): void
    {
        $this->basliklarGonder();
        foreach ($this->tampon as $kayit) {
            echo $kayit->metinSatiri() . PHP_EOL;
        }
        $this->tampon = [];
    }

    private function basliklarGonder(): void
    {
        if ($this->baslikGonderildi || headers_sent()) return;
        header('Content-Type: text/plain; charset=utf-8');
        header('Cache-Control: no-cache, no-store, must-revalidate');
        header('Pragma: no-cache');
        header('Expires: 0');
        header('X-Log-Generator: SimpleOS-DocLogger/2.0');
        $this->baslikGonderildi = true;
    }
}

// ═══════════════════════════════════════════════════════════
// YIĞIT İŞLEYİCİSİ (Çoklu Hedef)
// ═══════════════════════════════════════════════════════════

class YigitIsleyici implements LogIsleyiciInterface
{
    /** @var LogIsleyiciInterface[] */
    private array $isleyiciler = [];

    public function ekle(LogIsleyiciInterface $isleyici): static
    {
        $this->isleyiciler[] = $isleyici;
        return $this;
    }

    public function isle(LogKaydi $kayit): void
    {
        foreach ($this->isleyiciler as $isleyici) {
            try {
                $isleyici->isle($kayit);
            } catch (\Throwable) {
                // Bir işleyicinin hatası diğerlerini durdurmaz
            }
        }
    }

    public function minimumSeviyeAyarla(LogSeviye $seviye): static
    {
        foreach ($this->isleyiciler as $i) $i->minimumSeviyeAyarla($seviye);
        return $this;
    }
}
