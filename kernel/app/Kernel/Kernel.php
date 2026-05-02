<?php

namespace Kernel\Kernel;

use Kernel\App\App;
use Kernel\Exceptions\KernelException;

/**
 * Sistem çekirdeği.
 * App örneğini alır, doğrular ve uygulamayı başlatır.
 */
class Kernel
{
    private App $app;

    /** @var bool Kernel'in başlatılıp başlatılmadığı */
    private bool $baslatildi = false;

    /** @var float|null Başlatma süresi (saniye) */
    private ?float $baslatmaSuresi = null;

    /**
     * @throws KernelException Geçersiz bootstrap tespit edilirse
     */
    public function __construct(App $app)
    {
        $this->app = $app;
        $this->bootstraplarDogrula();
    }

    /**
     * Uygulamayı başlatır.
     *
     * @throws KernelException Kernel zaten başlatıldıysa veya App yoksa
     */
    public function run(): void
    {
        if ($this->baslatildi) {
            throw new KernelException('Kernel zaten başlatıldı. run() yalnızca bir kez çağrılabilir.');
        }

        $baslangic = microtime(true);

        try {
            $this->app->run();
            $this->baslatildi = true;
            $this->baslatmaSuresi = microtime(true) - $baslangic;
        } catch (\Throwable $hata) {
            $this->baslatmaSuresi = microtime(true) - $baslangic;
            throw new KernelException(
                'Kernel çalışırken hata oluştu: ' . $hata->getMessage(),
                (int) $hata->getCode(),
                $hata
            );
        }
    }

    /**
     * App örneğini döner.
     */
    public function getApp(): App
    {
        return $this->app;
    }

    /**
     * Kernel'in başlatılıp başlatılmadığını döner.
     */
    public function baslatildiMi(): bool
    {
        return $this->baslatildi;
    }

    /**
     * Başlatma süresini saniye cinsinden döner (henüz çalışmadıysa null).
     */
    public function getBaslatmaSuresi(): ?float
    {
        return $this->baslatmaSuresi;
    }

    /**
     * Kayıtlı bootstrap sayısını döner.
     */
    public function getBootstrapSayisi(): int
    {
        return count($this->app->getBootstraps());
    }

    // -------------------------------------------------------------------------
    // Yardımcı (özel) metodlar
    // -------------------------------------------------------------------------

    /**
     * Tüm bootstrap'lerin BootstrapInterface'i implement ettiğini doğrular.
     *
     * @throws KernelException Geçersiz bootstrap bulunursa
     */
    private function bootstraplarDogrula(): void
    {
        foreach ($this->app->getBootstraps() as $bootstrap) {
            if (!$bootstrap instanceof \Kernel\App\Interfaces\BootstrapInterface) {
                throw new KernelException(
                    sprintf(
                        "Geçersiz bootstrap tespit edildi: '%s' sınıfı BootstrapInterface'i implement etmiyor.",
                        get_class($bootstrap)
                    )
                );
            }
        }
    }
}