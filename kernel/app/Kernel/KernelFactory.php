<?php

namespace Kernel\Kernel;

use Kernel\App\App;
use Kernel\App\Bootstraps\HataYonetimiBootstrap;
use Kernel\App\Bootstraps\KonfigurasyonBootstrap;
use Kernel\App\Bootstraps\OrtamBootstrap;
use Kernel\Config\Konfigurasyon;
use Kernel\ServiceContainer\Container;

/**
 * Kernel fabrikası.
 * Standart bir Kernel örneği oluşturmak için kullanım kolaylığı sağlar.
 */
class KernelFactory
{
    /**
     * Varsayılan bootstrap'lerle tam donanımlı Kernel oluşturur.
     *
     * @param string $ortam      'development' | 'production'
     * @param string $projeDizini Proje kök dizini
     */
    public static function olustur(
        string $ortam = 'production',
        string $projeDizini = __DIR__
    ): Kernel {
        $konteyner    = new Container();
        $konfigurasyon = new Konfigurasyon();

        // Servisleri konteynere kaydet
        $konteyner->ornek(Konfigurasyon::class, $konfigurasyon);
        $konteyner->ornek(Container::class, $konteyner);

        // Bootstrap listesi
        $bootstraplar = [
            new OrtamBootstrap($projeDizini . '/.env'),
            new HataYonetimiBootstrap($ortam, $projeDizini . '/logs/hata.log'),
            new KonfigurasyonBootstrap($konfigurasyon, $projeDizini . '/config'),
        ];

        $app    = new App($bootstraplar, $konteyner);
        return  new Kernel($app);
    }

    /**
     * Minimal (bootstrap'siz) boş Kernel oluşturur.
     */
    public static function minimal(): Kernel
    {
        return new Kernel(new App());
    }
}
