<?php

namespace Kernel\App;

use Kernel\App\Interfaces\AppInterface;
use Kernel\App\Interfaces\BootstrapInterface;
use Kernel\Exceptions\BootstrapException;
use Kernel\Exceptions\KernelException;
use Kernel\ServiceContainer\Container;

/**
 * Uygulama çekirdeği.
 * Bootstrap bileşenlerini öncelik sırasına göre yönetir ve çalıştırır.
 */
class App implements AppInterface
{
    /** @var BootstrapInterface[] Kayıtlı bootstrap bileşenleri */
    private array $bootstraps = [];

    /** @var bool Uygulamanın çalışıp çalışmadığı */
    private bool $calisiyorMu = false;

    /**
     * @param BootstrapInterface[] $bootstraps Başlangıç bootstrap listesi
     * @param Container|null       $konteyner  Servis konteyneri (opsiyonel)
     */
    public function __construct(
        array $bootstraps = [],
        private readonly ?Container $konteyner = null
    ) {
        foreach ($bootstraps as $bootstrap) {
            $this->addBootstrap($bootstrap);
        }
    }

    /**
     * {@inheritDoc}
     *
     * Bootstrap'leri öncelik sırasına göre sıralayıp çalıştırır.
     *
     * @throws KernelException  Uygulama zaten çalışıyorsa
     * @throws BootstrapException Bootstrap başlatma sırasında hata oluşursa
     */
    public function run(): void
    {
        if ($this->calisiyorMu) {
            throw new KernelException('Uygulama zaten çalışıyor. run() birden fazla çağrılamaz.');
        }

        $this->calisiyorMu = true;

        // Önceliğe göre sırala (düşük sayı = önce çalışır)
        $sirali = $this->bootstraps;
        usort($sirali, fn(BootstrapInterface $a, BootstrapInterface $b)
            => $a->getOncelik() <=> $b->getOncelik()
        );

        foreach ($sirali as $bootstrap) {
            $this->bootstrapCalistir($bootstrap);
        }
    }

    /**
     * {@inheritDoc}
     *
     * Aynı bootstrap iki kez eklenemez (nesne kimliği kontrolü).
     *
     * @throws KernelException Bootstrap zaten eklenmişse
     */
    public function addBootstrap(BootstrapInterface $bootstrap): void
    {
        if ($this->bootstrapVarMi($bootstrap)) {
            throw new KernelException(
                sprintf(
                    "'%s' bootstrap zaten eklenmiş.",
                    $bootstrap->getAd()
                )
            );
        }

        $this->bootstraps[] = $bootstrap;
    }

    /**
     * {@inheritDoc}
     */
    public function getBootstraps(): array
    {
        return $this->bootstraps;
    }

    /**
     * Bootstrap'i kaldırır.
     */
    public function removeBootstrap(BootstrapInterface $hedef): bool
    {
        foreach ($this->bootstraps as $anahtar => $bootstrap) {
            if ($bootstrap === $hedef) {
                unset($this->bootstraps[$anahtar]);
                $this->bootstraps = array_values($this->bootstraps);
                return true;
            }
        }
        return false;
    }

    /**
     * Konteyneri döner (varsa).
     */
    public function getKonteyner(): ?Container
    {
        return $this->konteyner;
    }

    /**
     * Uygulamanın çalışıp çalışmadığını döner.
     */
    public function calisiyorMu(): bool
    {
        return $this->calisiyorMu;
    }

    // -------------------------------------------------------------------------
    // Yardımcı (özel) metodlar
    // -------------------------------------------------------------------------

    /**
     * Tek bir bootstrap'i güvenli şekilde çalıştırır.
     *
     * @throws BootstrapException
     */
    private function bootstrapCalistir(BootstrapInterface $bootstrap): void
    {
        try {
            $bootstrap->bootstrap();
        } catch (\Throwable $hata) {
            throw new BootstrapException(
                get_class($bootstrap),
                $hata->getMessage(),
                (int) $hata->getCode(),
                $hata
            );
        }
    }

    /**
     * Bootstrap'in daha önce eklenip eklenmediğini kontrol eder.
     */
    private function bootstrapVarMi(BootstrapInterface $aranan): bool
    {
        foreach ($this->bootstraps as $bootstrap) {
            if ($bootstrap === $aranan) {
                return true;
            }
        }
        return false;
    }
}