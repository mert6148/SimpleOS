<?php

declare(strict_types=1);

namespace DocIntegration\Contracts;

/**
 * Tüm entegrasyon sürücülerinin uygulaması gereken temel sözleşme.
 */
interface IntegrationInterface
{
    /**
     * Entegrasyona ait endpoint listesini döner.
     *
     * @return string[]
     */
    public function getEndpoints(): array;

    /**
     * Belirtilen endpoint'i çağırır ve ham yanıt döner.
     *
     * @param  string               $endpoint
     * @param  array<string, mixed> $parametreler
     * @return string
     *
     * @throws \DocIntegration\Exception\EndpointHatasi
     */
    public function callEndpoint(string $endpoint, array $parametreler = []): string;

    /**
     * Entegrasyonun bağlantı durumunu döner.
     */
    public function bagliMi(): bool;

    /**
     * Entegrasyon adını döner (loglama/kayıt için).
     */
    public function adAl(): string;
}
