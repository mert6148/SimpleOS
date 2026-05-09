<?php

declare(strict_types=1);

namespace DocIntegration\Exception;

/** Tüm entegrasyon istisnalarının tabanı */
class EntegrasyonHatasi extends \RuntimeException
{
    public function __construct(
        string $mesaj,
        private readonly string $entegrasyonAdi = '',
        int $kod = 0,
        ?\Throwable $oncekiHata = null
    ) {
        parent::__construct($mesaj, $kod, $oncekiHata);
    }

    public function entegrasyonAdiniAl(): string
    {
        return $this->entegrasyonAdi;
    }
}

/** Endpoint çağrısı başarısız olduğunda */
class EndpointHatasi extends EntegrasyonHatasi
{
    public function __construct(
        string $mesaj,
        private readonly string $endpoint = '',
        string $entegrasyonAdi = '',
        int $httpKod = 0,
        ?\Throwable $oncekiHata = null
    ) {
        parent::__construct($mesaj, $entegrasyonAdi, $httpKod, $oncekiHata);
    }

    public function endpointAl(): string
    {
        return $this->endpoint;
    }
}

/** Entegrasyon başlatılamadığında */
class BaslatmaHatasi extends EntegrasyonHatasi {}

/** Entegrasyon kayıt defterinde bulunamadığında */
class KayitBulunamadiHatasi extends EntegrasyonHatasi {}

/** Geçersiz yapılandırma */
class YapilandirmaHatasi extends EntegrasyonHatasi {}
