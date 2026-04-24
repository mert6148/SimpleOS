<?php

declare(strict_types=1);

namespace DocIntegration\Integration;

use DocIntegration\Exception\EndpointHatasi;
use DocIntegration\Exception\BaslatmaHatasi;
use DocIntegration\Http\HttpYanit;

/**
 * Genel REST API entegrasyon sürücüsü.
 *
 * Yapılandırma:
 *   - temel_url      : string  (zorunlu)
 *   - api_anahtari   : string  (zorunlu)
 *   - versiyon       : string  (isteğe bağlı, varsayılan "v1")
 *   - zaman_asimi    : int     (isteğe bağlı, varsayılan 30)
 *
 * Örnek kullanım:
 *   $entegrasyon = new RestApiEntegrasyon([
 *       'temel_url'    => 'https://api.ornek.com',
 *       'api_anahtari' => 'abc123',
 *   ]);
 */
class RestApiEntegrasyon extends TemelEntegrasyon
{
    private const ENDPOINT_LISTESI = [
        'saglik'     => '/health',
        'kullanicilar' => '/users',
        'belgeler'   => '/documents',
        'durum'      => '/status',
    ];

    public function adAl(): string
    {
        return 'REST API';
    }

    public function getEndpoints(): array
    {
        return array_keys(self::ENDPOINT_LISTESI);
    }

    protected function temelUrlAl(): string
    {
        $versiyon = $this->yapilandirmaAl('versiyon', 'v1');
        return rtrim((string) $this->yapilandirmaAl('temel_url'), '/') . "/{$versiyon}";
    }

    protected function zorunluAnahtarlar(): array
    {
        return ['temel_url', 'api_anahtari'];
    }

    protected function baslatmaDogrula(): void
    {
        $url = $this->yapilandirmaAl('temel_url', '');
        if (!filter_var($url, FILTER_VALIDATE_URL)) {
            throw new BaslatmaHatasi(
                "Geçersiz temel URL: '{$url}'",
                $this->adAl()
            );
        }
    }

    protected function endpointCagir(string $endpoint, array $parametreler): HttpYanit
    {
        $yol = self::ENDPOINT_LISTESI[$endpoint]
            ?? throw new EndpointHatasi("Eşleme bulunamadı: {$endpoint}", $endpoint, $this->adAl());

        $basliklar = [
            'Authorization' => 'Bearer ' . $this->yapilandirmaAl('api_anahtari'),
            'X-Client'      => 'DocIntegration/1.0',
        ];

        return match (true) {
            in_array($endpoint, ['kullanicilar', 'belgeler']) && !empty($parametreler)
                => $this->http->post($this->urlOlustur($yol), $parametreler, $basliklar),
            default
                => $this->http->get($this->urlOlustur($yol), $parametreler, $basliklar),
        };
    }
}
