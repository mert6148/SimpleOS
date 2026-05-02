<?php

namespace Kernel\App\Bootstraps;

/**
 * PHP hata işleme ayarlarını yapar.
 * Öncelik: 20 — ortam yüklendikten hemen sonra çalışır.
 */
class HataYonetimiBootstrap extends AbstractBootstrap
{
    private string $ortam;
    private string $logDosyasi;

    public function __construct(
        string $ortam = 'production',
        string $logDosyasi = '/tmp/kernel-hata.log'
    ) {
        $this->ortam      = $ortam;
        $this->logDosyasi = $logDosyasi;
    }

    public function bootstrap(): void
    {
        if ($this->ortam === 'development') {
            error_reporting(E_ALL);
            ini_set('display_errors', '1');
        } else {
            error_reporting(E_ALL & ~E_DEPRECATED & ~E_STRICT);
            ini_set('display_errors', '0');
            ini_set('log_errors', '1');
            ini_set('error_log', $this->logDosyasi);
        }

        // Yakalanmamış istisnaları ele al
        set_exception_handler(function (\Throwable $hata): void {
            $mesaj = sprintf(
                '[%s] Yakalanmamış istisna: %s in %s:%d',
                date('Y-m-d H:i:s'),
                $hata->getMessage(),
                $hata->getFile(),
                $hata->getLine()
            );
            error_log($mesaj);

            if ($this->ortam !== 'development') {
                http_response_code(500);
                echo json_encode([
                    'hata'   => true,
                    'mesaj'  => 'Sunucu hatası oluştu.',
                    'kod'    => 500,
                ]);
            } else {
                throw $hata;
            }
        });

        // Ölümcül hataları yakala
        register_shutdown_function(function (): void {
            $son = error_get_last();
            if ($son !== null && in_array($son['type'], [E_ERROR, E_PARSE, E_CORE_ERROR, E_COMPILE_ERROR], true)) {
                error_log(sprintf(
                    '[%s] Ölümcül hata: %s in %s:%d',
                    date('Y-m-d H:i:s'),
                    $son['message'],
                    $son['file'],
                    $son['line']
                ));
            }
        });
    }

    public function getOncelik(): int
    {
        return 20;
    }
}
