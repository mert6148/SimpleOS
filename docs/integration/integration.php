<?php

declare(strict_types=1);

namespace DocIntegration;

use DocIntegration\Contracts\IntegrationInterface;
use DocIntegration\Contracts\LoggerInterface;
use DocIntegration\Exception\BaslatmaHatasi;
use DocIntegration\Exception\EntegrasyonHatasi;
use DocIntegration\Exception\EndpointHatasi;
use DocIntegration\Logger\KonsolLogger;

/**
 * Entegrasyon çalıştırıcısı.
 *
 * Orijinal koddan yapısal farklılıklar:
 *  - try/catch/else bloğu kaldırıldı (PHP'de geçersiz sözdizimi)
 *  - echo yerine yapılandırılabilir logger
 *  - run() toplu çalıştırma sonuç özeti döner
 *  - setIntegration() ile çalışma zamanı sürücü değişimi desteklenir
 */
class Integration
{
    private IntegrationInterface $integration;

    public function __construct(
        IntegrationInterface $integration,
        private readonly LoggerInterface $logger = new KonsolLogger()
    ) {
        try {
            $this->integration = $integration;
            $this->logger->bilgi(
                "Entegrasyon başlatıldı: {$integration->adAl()}"
            );
        } catch (\Throwable $hata) {
            throw new BaslatmaHatasi(
                'Entegrasyon başlatılamadı: ' . $hata->getMessage(),
                $integration->adAl(),
                0,
                $hata
            );
        }
    }

    // ─── Toplu Çalıştırma ─────────────────────────────────────

    /**
     * Tüm endpoint'leri sırayla çalıştırır.
     *
     * @param  array<string, mixed> $parametreler Tüm endpoint'lere aktarılır
     * @return array{
     *   basarili: int,
     *   basarisiz: int,
     *   sonuclar: array<string, string>,
     *   hatalar: array<string, string>
     * }
     */
    public function run(array $parametreler = []): array
    {
        $sonuclar  = [];
        $hatalar   = [];
        $basarili  = 0;
        $basarisiz = 0;

        $endpointler = $this->integration->getEndpoints();
        $this->logger->bilgi(
            "Çalıştırma başlıyor.",
            ['toplam_endpoint' => count($endpointler)]
        );

        foreach ($endpointler as $endpoint) {
            try {
                $yanit = $this->integration->callEndpoint($endpoint, $parametreler);
                $sonuclar[$endpoint] = $yanit;
                $basarili++;
            } catch (EndpointHatasi $hata) {
                $hatalar[$endpoint] = $hata->getMessage();
                $this->logger->hata(
                    "Endpoint başarısız: {$endpoint}",
                    ['hata' => $hata->getMessage()]
                );
                $basarisiz++;
            }
        }

        $this->logger->bilgi(
            "Çalıştırma tamamlandı.",
            ['basarili' => $basarili, 'basarisiz' => $basarisiz]
        );

        return compact('basarili', 'basarisiz', 'sonuclar', 'hatalar');
    }

    /**
     * Tek bir endpoint'i çalıştırır.
     *
     * @param  array<string, mixed> $parametreler
     * @throws EndpointHatasi
     */
    public function endpointCalistir(string $endpoint, array $parametreler = []): string
    {
        return $this->integration->callEndpoint($endpoint, $parametreler);
    }

    // ─── Getter / Setter ──────────────────────────────────────

    public function getIntegration(): IntegrationInterface
    {
        return $this->integration;
    }

    /**
     * Çalışma zamanında entegrasyon sürücüsünü değiştirir.
     *
     * @throws BaslatmaHatasi
     */
    public function setIntegration(IntegrationInterface $integration): void
    {
        try {
            $eskiAd = $this->integration->adAl();
            $this->integration = $integration;
            $this->logger->bilgi(
                "Entegrasyon değiştirildi.",
                ['eski' => $eskiAd, 'yeni' => $integration->adAl()]
            );
        } catch (\Throwable $hata) {
            throw new BaslatmaHatasi(
                'Entegrasyon değiştirilemedi: ' . $hata->getMessage(),
                $integration->adAl(),
                0,
                $hata
            );
        }
    }

    // ─── Durum ────────────────────────────────────────────────

    public function bagliMi(): bool
    {
        return $this->integration->bagliMi();
    }
}

/**
 * Callable sınıf: Integration nesnesinden IntegrationInterface çeker.
 * Orijinal getIntegration sınıfının düzeltilmiş hali.
 */
final class EntegrasyonAl
{
    public function __invoke(Integration $integration): IntegrationInterface
    {
        try {
            return $integration->getIntegration();
        } catch (EntegrasyonHatasi $hata) {
            throw new EntegrasyonHatasi(
                'Entegrasyon alınamadı: ' . $hata->getMessage(),
                '',
                0,
                $hata
            );
        }
    }
}