<?php

declare(strict_types=1);

namespace DocIntegration\Http;

use DocIntegration\Exception\EndpointHatasi;

/**
 * cURL tabanlı hafif HTTP istemcisi.
 * Gerçek entegrasyonlarda Guzzle/Symfony HttpClient ile değiştirilebilir.
 */
class HttpIstemci
{
    private array $varsayilanBasliklar = [
        'Content-Type' => 'application/json',
        'Accept'       => 'application/json',
    ];

    public function __construct(
        private readonly int    $zamanAsimi   = 30,
        private readonly bool   $sslDogrulama = true,
        private readonly int    $yeniden_deneme_sayisi = 3,
        private array           $ekBasliklar  = []
    ) {}

    /**
     * GET isteği gönderir.
     *
     * @param  array<string, mixed> $sorguParametreleri
     */
    public function get(
        string $url,
        array $sorguParametreleri = [],
        array $basliklar = []
    ): HttpYanit {
        if (!empty($sorguParametreleri)) {
            $url .= '?' . http_build_query($sorguParametreleri);
        }
        return $this->istek('GET', $url, [], $basliklar);
    }

    /**
     * POST isteği gönderir.
     *
     * @param  array<string, mixed> $govde
     */
    public function post(
        string $url,
        array $govde = [],
        array $basliklar = []
    ): HttpYanit {
        return $this->istek('POST', $url, $govde, $basliklar);
    }

    /**
     * PUT isteği gönderir.
     */
    public function put(
        string $url,
        array $govde = [],
        array $basliklar = []
    ): HttpYanit {
        return $this->istek('PUT', $url, $govde, $basliklar);
    }

    /**
     * DELETE isteği gönderir.
     */
    public function delete(string $url, array $basliklar = []): HttpYanit
    {
        return $this->istek('DELETE', $url, [], $basliklar);
    }

    /**
     * Ham HTTP isteği; yeniden deneme mantığı dahil.
     */
    private function istek(
        string $metot,
        string $url,
        array  $govde    = [],
        array  $basliklar = []
    ): HttpYanit {
        $birlesikBasliklar = array_merge(
            $this->varsayilanBasliklar,
            $this->ekBasliklar,
            $basliklar
        );

        $son_hata = null;

        for ($deneme = 1; $deneme <= $this->yeniden_deneme_sayisi; $deneme++) {
            try {
                return $this->curlIstegi($metot, $url, $govde, $birlesikBasliklar);
            } catch (EndpointHatasi $hata) {
                $son_hata = $hata;
                // Son denemede yeniden deneme yapma
                if ($deneme < $this->yeniden_deneme_sayisi) {
                    usleep(200_000 * $deneme); // 0.2s, 0.4s, ...
                }
            }
        }

        throw $son_hata;
    }

    private function curlIstegi(
        string $metot,
        string $url,
        array  $govde,
        array  $basliklar
    ): HttpYanit {
        // cURL mevcut değilse simüle et (test/sandbox ortamı)
        if (!function_exists('curl_init')) {
            return $this->simulasyonYanit($metot, $url);
        }

        $curl = curl_init();

        $curlBasliklar = array_map(
            fn($k, $v) => "{$k}: {$v}",
            array_keys($basliklar),
            $basliklar
        );

        $secenekler = [
            CURLOPT_URL            => $url,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_TIMEOUT        => $this->zamanAsimi,
            CURLOPT_SSL_VERIFYPEER => $this->sslDogrulama,
            CURLOPT_HTTPHEADER     => $curlBasliklar,
            CURLOPT_CUSTOMREQUEST  => $metot,
        ];

        if (!empty($govde) && in_array($metot, ['POST', 'PUT', 'PATCH'])) {
            $secenekler[CURLOPT_POSTFIELDS] = json_encode($govde);
        }

        curl_setopt_array($curl, $secenekler);

        $icerik   = curl_exec($curl);
        $httpKod  = curl_getinfo($curl, CURLINFO_HTTP_CODE);
        $curlHata = curl_error($curl);
        curl_close($curl);

        if ($curlHata) {
            throw new EndpointHatasi(
                "cURL hatası: {$curlHata}",
                $url
            );
        }

        if ($httpKod >= 400) {
            throw new EndpointHatasi(
                "HTTP {$httpKod}: {$url}",
                $url,
                '',
                $httpKod
            );
        }

        return new HttpYanit($httpKod, (string) $icerik, []);
    }

    /** cURL olmayan ortamlarda simüle edilmiş yanıt */
    private function simulasyonYanit(string $metot, string $url): HttpYanit
    {
        $govde = json_encode([
            'simulasyon' => true,
            'metot'      => $metot,
            'url'        => $url,
            'zaman'      => date('c'),
        ]);

        return new HttpYanit(200, $govde, ['X-Simulated' => 'true']);
    }
}

/**
 * HTTP yanıt değer nesnesi.
 */
final class HttpYanit
{
    public function __construct(
        private readonly int    $httpKodu,
        private readonly string $govde,
        private readonly array  $basliklar
    ) {}

    public function httpKoduAl(): int    { return $this->httpKodu; }
    public function govdeAl(): string    { return $this->govde; }
    public function basliklarAl(): array { return $this->basliklar; }

    public function basariliMi(): bool
    {
        return $this->httpKodu >= 200 && $this->httpKodu < 300;
    }

    /**
     * JSON govdeyi diziye çözer.
     *
     * @return array<string, mixed>
     */
    public function jsonAl(): array
    {
        $veri = json_decode($this->govde, true);
        return is_array($veri) ? $veri : [];
    }
}
