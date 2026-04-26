<?php

declare(strict_types=1);

namespace SimpleOS\Docs\Log;

/**
 * SimpleOS Documentation Logger
 *
 * Dokümantasyon değişikliklerini takip eden merkezi logger.
 * Fluent arayüz, çoklu işleyici ve filtre desteği sunar.
 *
 * Kullanım:
 *   $logger = DocLogger::olustur(__DIR__ . '/storage');
 *   $logger->degisiklik('docs/footer.md', 'Mert D.', '1.0.1', 'Footer güncellendi.');
 *   $logger->hata('docs/api.md', 'Parser hatası.', ['satir' => 42]);
 */
class DocLogger
{
    private const SURUM = '2.0.0';

    private LogIsleyiciInterface $isleyici;

    /** @var array<string, mixed> Tüm kayıtlara eklenen varsayılan bağlam */
    private array $varsayilanBaglam = [];

    public function __construct(LogIsleyiciInterface $isleyici)
    {
        $this->isleyici = $isleyici;
    }

    // ─── Fabrika ──────────────────────────────────────────

    /**
     * Hazır yapılandırmayla DocLogger örneği oluşturur.
     * - storage/logs/ → dönen metin log dosyaları
     * - storage/logs/json/ → NDJSON dosyaları
     * - STDOUT → renkli konsol çıktısı
     *
     * @param string      $depolamaKoku  Depolama kök dizini
     * @param LogSeviye   $minimumSeviye Filtrelenecek minimum seviye
     */
    public static function olustur(
        string    $depolamaKoku,
        LogSeviye $minimumSeviye = LogSeviye::DEBUG
    ): self {
        $logDizini  = rtrim($depolamaKoku, '/') . '/logs';
        $jsonDizini = $logDizini . '/json';

        $yigit = (new YigitIsleyici())
            ->ekle(new DosyaIsleyici(
                dizin:           $logDizini,
                onEk:            'simpleos-docs',
                rotasyonFormati: DosyaIsleyici::ROTASYON_GUNLUK,
                minimumSeviye:   $minimumSeviye
            ))
            ->ekle(new JsonIsleyici(
                dosyaYolu:     $jsonDizini . '/simpleos-docs-' . date('Y-m-d') . '.ndjson',
                minimumSeviye: $minimumSeviye
            ))
            ->ekle(new KonsolIsleyici(
                renkli:        PHP_SAPI === 'cli',
                minimumSeviye: LogSeviye::BILGI
            ));

        $instance = new self($yigit);
        $instance->varsayilanBaglam['logger_surumu'] = self::SURUM;

        return $instance;
    }

    /**
     * Sadece HTTP çıktısı veren DocLogger.
     */
    public static function httpIcinOlustur(LogSeviye $minimumSeviye = LogSeviye::BILGI): self
    {
        $http = new HttpIsleyici(tamponla: false, minimumSeviye: $minimumSeviye);
        return new self($http);
    }

    // ─── Varsayılan Bağlam ────────────────────────────────

    public function baglamEkle(string $anahtar, mixed $deger): static
    {
        $this->varsayilanBaglam[$anahtar] = $deger;
        return $this;
    }

    public function baglamKaldir(string $anahtar): static
    {
        unset($this->varsayilanBaglam[$anahtar]);
        return $this;
    }

    // ─── Kısayol Metodlar ─────────────────────────────────

    /**
     * Dokümantasyon değişikliğini kaydeder (en sık kullanılan).
     *
     * @param string               $dosya    Değiştirilen dosya yolu
     * @param string               $yazar    Değişikliği yapan kişi
     * @param string               $surum    Yeni sürüm numarası
     * @param string               $mesaj    Değişiklik açıklaması
     * @param array<string,mixed>  $baglam   Ek meta veri
     */
    public function degisiklik(
        string $dosya,
        string $yazar,
        string $surum,
        string $mesaj,
        array  $baglam = []
    ): void {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::BILGI,
            mesaj:    $mesaj,
            kategori: 'docs.degisiklik',
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
            dosya:    $dosya,
            yazar:    $yazar,
            surum:    $surum,
        ));
    }

    /**
     * Yeni dosya oluşturulduğunda.
     */
    public function olusturuldu(string $dosya, string $yazar, string $surum, array $baglam = []): void
    {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::BILGI,
            mesaj:    "Yeni dokümantasyon dosyası oluşturuldu.",
            kategori: 'docs.olusturuldu',
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
            dosya:    $dosya,
            yazar:    $yazar,
            surum:    $surum,
        ));
    }

    /**
     * Dosya silindiğinde.
     */
    public function silindi(string $dosya, string $yazar, string $surum, array $baglam = []): void
    {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::UYARI,
            mesaj:    "Dokümantasyon dosyası silindi.",
            kategori: 'docs.silindi',
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
            dosya:    $dosya,
            yazar:    $yazar,
            surum:    $surum,
        ));
    }

    /**
     * Doğrulama hatası.
     */
    public function hata(string $dosya, string $mesaj, array $baglam = []): void
    {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::HATA,
            mesaj:    $mesaj,
            kategori: 'docs.hata',
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
            dosya:    $dosya,
        ));
    }

    /**
     * Uyarı seviyesinde kayıt.
     */
    public function uyari(string $mesaj, string $kategori = 'docs.uyari', array $baglam = []): void
    {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::UYARI,
            mesaj:    $mesaj,
            kategori: $kategori,
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
        ));
    }

    /**
     * Debug kaydı.
     */
    public function debug(string $mesaj, array $baglam = []): void
    {
        $this->kaydet(new LogKaydi(
            seviye:   LogSeviye::DEBUG,
            mesaj:    $mesaj,
            kategori: 'docs.debug',
            baglam:   array_merge($this->varsayilanBaglam, $baglam),
        ));
    }

    /**
     * Özel seviyede ham kayıt.
     */
    public function kaydet(LogKaydi $kayit): void
    {
        $this->isleyici->isle($kayit);
    }

    // ─── Toplu İşlem ──────────────────────────────────────

    /**
     * Birden fazla değişikliği tek seferde kaydeder.
     *
     * @param array<array{dosya:string,yazar:string,surum:string,mesaj:string}> $degisiklikler
     */
    public function topluDegisiklik(array $degisiklikler): void
    {
        foreach ($degisiklikler as $d) {
            $this->degisiklik(
                $d['dosya']  ?? 'bilinmiyor',
                $d['yazar']  ?? 'bilinmiyor',
                $d['surum']  ?? '0.0.0',
                $d['mesaj']  ?? '',
                $d['baglam'] ?? [],
            );
        }
    }

    // ─── İşleyici Erişimi ─────────────────────────────────

    public function isleyiciAl(): LogIsleyiciInterface
    {
        return $this->isleyici;
    }

    public function isleyiciDegistir(LogIsleyiciInterface $isleyici): static
    {
        $this->isleyici = $isleyici;
        return $this;
    }
}
