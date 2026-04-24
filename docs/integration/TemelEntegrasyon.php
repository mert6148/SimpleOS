<?php

declare(strict_types=1);

namespace DocIntegration\Integration;

use DocIntegration\Contracts\IntegrationInterface;
use DocIntegration\Contracts\LoggerInterface;
use DocIntegration\Exception\EndpointHatasi;
use DocIntegration\Exception\BaslatmaHatasi;
use DocIntegration\Exception\YapilandirmaHatasi;
use DocIntegration\Http\HttpIstemci;
use DocIntegration\Http\HttpYanit;
use DocIntegration\Logger\KonsolLogger;

/**
 * Tüm entegrasyon sürücülerinin türetildiği temel sınıf.
 *
 * Alt sınıflar şunları uygulamalıdır:
 *   - adAl(): string
 *   - getEndpoints(): array
 *   - temelUrlAl(): string
 *
 * İsteğe bağlı geçersiz kılma:
 *   - baslatmaDogrula(): void  — kurulum sonrası özel doğrulama
 */
abstract class TemelEntegrasyon implements IntegrationInterface
{
    protected HttpIstemci  $http;
    protected LoggerInterface $logger;
    private   bool         $bagliMiDurumu = false;

    /** @var array<string, mixed> */
    protected array $yapilandirma = [];

    public function __construct(
        array $yapilandirma = [],
        ?HttpIstemci $http = null,
        ?LoggerInterface $logger = null
    ) {
        $this->yapilandirmayiDogrula($yapilandirma);
        $this->yapilandirma = $yapilandirma;
        $this->http         = $http   ?? new HttpIstemci();
        $this->logger       = $logger ?? new KonsolLogger();

        $this->baslatmaDogrula();
        $this->bagliMiDurumu = true;

        $this->logger->bilgi(
            "{$this->adAl()} entegrasyonu başlatıldı.",
            ['endpoints' => count($this->getEndpoints())]
        );
    }

    // ─── IntegrationInterface ────────────────────────────────

    public function bagliMi(): bool
    {
        return $this->bagliMiDurumu;
    }

    /**
     * Endpoint'i çağırır; loglama ve hata sarmalama dahil.
     */
    public function callEndpoint(string $endpoint, array $parametreler = []): string
    {
        if (!$this->bagliMiDurumu) {
            throw new EndpointHatasi(
                "Entegrasyon bağlı değil.",
                $endpoint,
                $this->adAl()
            );
        }

        if (!in_array($endpoint, $this->getEndpoints(), true)) {
            throw new EndpointHatasi(
                "Bilinmeyen endpoint: {$endpoint}",
                $endpoint,
                $this->adAl()
            );
        }

        $this->logger->debug("Endpoint çağrılıyor: {$endpoint}", $parametreler);

        try {
            $yanit = $this->endpointCagir($endpoint, $parametreler);
            $this->logger->bilgi(
                "Endpoint başarılı: {$endpoint}",
                ['http_kod' => $yanit->httpKoduAl()]
            );
            return $yanit->govdeAl();
        } catch (EndpointHatasi $hata) {
            $this->bagliMiDurumu = false;
            $this->logger->hata("Endpoint hatası: {$endpoint}", ['hata' => $hata->getMessage()]);
            throw $hata;
        }
    }

    // ─── Soyut ve Geçersiz Kılınabilir ───────────────────────

    /**
     * Alt sınıf burada gerçek HTTP çağrısını yapar.
     */
    abstract protected function endpointCagir(
        string $endpoint,
        array  $parametreler
    ): HttpYanit;

    /**
     * Temel URL'yi döner (ör. "https://api.ornek.com/v1").
     */
    abstract protected function temelUrlAl(): string;

    /**
     * Alt sınıf özel başlatma doğrulaması için geçersiz kılabilir.
     *
     * @throws BaslatmaHatasi
     */
    protected function baslatmaDogrula(): void {}

    /**
     * Zorunlu yapılandırma anahtarlarını doğrular.
     *
     * @throws YapilandirmaHatasi
     */
    protected function yapilandirmayiDogrula(array $yapilandirma): void
    {
        foreach ($this->zorunluAnahtarlar() as $anahtar) {
            if (empty($yapilandirma[$anahtar])) {
                throw new YapilandirmaHatasi(
                    "Zorunlu yapılandırma anahtarı eksik: '{$anahtar}'",
                    $this->adAl()
                );
            }
        }
    }

    /**
     * Zorunlu yapılandırma anahtarlarını döner.
     * Alt sınıflar geçersiz kılabilir.
     *
     * @return string[]
     */
    protected function zorunluAnahtarlar(): array
    {
        return [];
    }

    // ─── Yardımcılar ─────────────────────────────────────────

    protected function yapilandirmaAl(string $anahtar, mixed $varsayilan = null): mixed
    {
        return $this->yapilandirma[$anahtar] ?? $varsayilan;
    }

    protected function urlOlustur(string $yol): string
    {
        return rtrim($this->temelUrlAl(), '/') . '/' . ltrim($yol, '/');
    }
}
