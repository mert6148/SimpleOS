<?php

declare(strict_types=1);

namespace DocIntegration\Registry;

use DocIntegration\Contracts\IntegrationInterface;
use DocIntegration\Contracts\LoggerInterface;
use DocIntegration\Exception\KayitBulunamadiHatasi;
use DocIntegration\Exception\EntegrasyonHatasi;
use DocIntegration\Logger\KonsolLogger;

/**
 * Tüm kayıtlı entegrasyonları yöneten merkezi kayıt defteri.
 *
 * Kullanım:
 *   $kayit = new EntegrasyonKayitDefteri();
 *   $kayit->kaydet('rest', new RestApiEntegrasyon([...]));
 *   $entegrasyon = $kayit->al('rest');
 */
class EntegrasyonKayitDefteri
{
    /** @var array<string, IntegrationInterface> */
    private array $entegrasyonlar = [];

    /** @var string|null Varsayılan entegrasyon anahtarı */
    private ?string $varsayilanAnahtar = null;

    public function __construct(
        private readonly LoggerInterface $logger = new KonsolLogger()
    ) {}

    // ─── Kayıt İşlemleri ─────────────────────────────────────

    /**
     * Yeni bir entegrasyon kaydeder.
     *
     * @throws EntegrasyonHatasi Aynı anahtar zaten kayıtlıysa
     */
    public function kaydet(
        string $anahtar,
        IntegrationInterface $entegrasyon,
        bool $varsayilanOlarakAyarla = false
    ): static {
        if ($this->varMi($anahtar)) {
            throw new EntegrasyonHatasi("'{$anahtar}' anahtarı zaten kayıtlı.");
        }

        $this->entegrasyonlar[$anahtar] = $entegrasyon;

        if ($varsayilanOlarakAyarla || $this->varsayilanAnahtar === null) {
            $this->varsayilanAnahtar = $anahtar;
        }

        $this->logger->bilgi(
            "Entegrasyon kaydedildi: '{$anahtar}'",
            ['ad' => $entegrasyon->adAl()]
        );

        return $this;
    }

    /**
     * Kayıtlı entegrasyonu günceller (yoksa hata verir).
     *
     * @throws KayitBulunamadiHatasi
     */
    public function guncelle(string $anahtar, IntegrationInterface $entegrasyon): static
    {
        $this->varligiBekle($anahtar);
        $this->entegrasyonlar[$anahtar] = $entegrasyon;
        $this->logger->bilgi("Entegrasyon güncellendi: '{$anahtar}'");
        return $this;
    }

    /**
     * Varsa günceller, yoksa kaydeder.
     */
    public function kaydetVeyaGuncelle(
        string $anahtar,
        IntegrationInterface $entegrasyon
    ): static {
        return $this->varMi($anahtar)
            ? $this->guncelle($anahtar, $entegrasyon)
            : $this->kaydet($anahtar, $entegrasyon);
    }

    /**
     * Kayıtlı entegrasyonu kaldırır.
     */
    public function kaldir(string $anahtar): static
    {
        $this->varligiBekle($anahtar);
        unset($this->entegrasyonlar[$anahtar]);

        if ($this->varsayilanAnahtar === $anahtar) {
            $this->varsayilanAnahtar = array_key_first($this->entegrasyonlar);
        }

        $this->logger->uyari("Entegrasyon kaldırıldı: '{$anahtar}'");
        return $this;
    }

    // ─── Sorgulama ────────────────────────────────────────────

    /**
     * Anahtara göre entegrasyon döner.
     *
     * @throws KayitBulunamadiHatasi
     */
    public function al(string $anahtar): IntegrationInterface
    {
        $this->varligiBekle($anahtar);
        return $this->entegrasyonlar[$anahtar];
    }

    /**
     * Varsayılan entegrasyonu döner.
     *
     * @throws KayitBulunamadiHatasi
     */
    public function varsayilaniAl(): IntegrationInterface
    {
        if ($this->varsayilanAnahtar === null) {
            throw new KayitBulunamadiHatasi("Kayıtlı entegrasyon yok.");
        }
        return $this->entegrasyonlar[$this->varsayilanAnahtar];
    }

    public function varMi(string $anahtar): bool
    {
        return isset($this->entegrasyonlar[$anahtar]);
    }

    /**
     * Tüm kayıtlı entegrasyon anahtarlarını döner.
     *
     * @return string[]
     */
    public function anahtarlariAl(): array
    {
        return array_keys($this->entegrasyonlar);
    }

    /**
     * Tüm entegrasyonları filtreli olarak döner.
     *
     * @return array<string, IntegrationInterface>
     */
    public function tumunuAl(?callable $filtre = null): array
    {
        if ($filtre === null) {
            return $this->entegrasyonlar;
        }

        return array_filter($this->entegrasyonlar, $filtre, ARRAY_FILTER_USE_BOTH);
    }

    /**
     * Sadece bağlı entegrasyonları döner.
     *
     * @return array<string, IntegrationInterface>
     */
    public function bagliEntegrasyonlariAl(): array
    {
        return $this->tumunuAl(
            fn(IntegrationInterface $e) => $e->bagliMi()
        );
    }

    public function sayiAl(): int
    {
        return count($this->entegrasyonlar);
    }

    // ─── Özel ────────────────────────────────────────────────

    /**
     * @throws KayitBulunamadiHatasi
     */
    private function varligiBekle(string $anahtar): void
    {
        if (!$this->varMi($anahtar)) {
            throw new KayitBulunamadiHatasi(
                "Entegrasyon bulunamadı: '{$anahtar}'. "
                . "Kayıtlılar: [" . implode(', ', $this->anahtarlariAl()) . "]"
            );
        }
    }
}
