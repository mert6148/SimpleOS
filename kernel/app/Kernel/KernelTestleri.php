<?php

namespace Kernel\Tests;

use Kernel\App\App;
use Kernel\App\Bootstraps\AbstractBootstrap;
use Kernel\App\Interfaces\BootstrapInterface;
use Kernel\Exceptions\BootstrapException;
use Kernel\Exceptions\KernelException;
use Kernel\Kernel\Kernel;
use PHPUnit\Framework\TestCase;

/**
 * Kernel sistem testleri.
 */
class KernelTestleri extends TestCase
{
    // ─────────────────────────────────────────────────────────────
    // App testleri
    // ─────────────────────────────────────────────────────────────

    public function test_bos_app_olusturulabilir(): void
    {
        $app = new App();
        $this->assertEmpty($app->getBootstraps());
        $this->assertFalse($app->calisiyorMu());
    }

    public function test_bootstrap_eklenir(): void
    {
        $bootstrap = $this->gecerliBootstrapOlustur();
        $app = new App([$bootstrap]);

        $this->assertCount(1, $app->getBootstraps());
    }

    public function test_ayni_bootstrap_iki_kez_eklenemez(): void
    {
        $this->expectException(KernelException::class);

        $bootstrap = $this->gecerliBootstrapOlustur();
        $app = new App();
        $app->addBootstrap($bootstrap);
        $app->addBootstrap($bootstrap); // İstisna bekleniyor
    }

    public function test_bootstrap_kaldirilir(): void
    {
        $bootstrap = $this->gecerliBootstrapOlustur();
        $app = new App([$bootstrap]);

        $sonuc = $app->removeBootstrap($bootstrap);

        $this->assertTrue($sonuc);
        $this->assertEmpty($app->getBootstraps());
    }

    public function test_app_run_sonrasi_calisor_olarak_isaretlenir(): void
    {
        $bootstrap = $this->gecerliBootstrapOlustur();
        $app = new App([$bootstrap]);
        $app->run();

        $this->assertTrue($app->calisiyorMu());
    }

    public function test_app_run_iki_kez_cagrilamaz(): void
    {
        $this->expectException(KernelException::class);

        $app = new App();
        $app->run();
        $app->run(); // İstisna bekleniyor
    }

    // ─────────────────────────────────────────────────────────────
    // Bootstrap öncelik testleri
    // ─────────────────────────────────────────────────────────────

    public function test_bootstraplar_oncelik_sirasina_gore_calisir(): void
    {
        $calistirmaSirasi = [];

        $bootstrap50 = $this->oncelikliBootstrapOlustur(50, $calistirmaSirasi, 'B');
        $bootstrap10 = $this->oncelikliBootstrapOlustur(10, $calistirmaSirasi, 'A');
        $bootstrap99 = $this->oncelikliBootstrapOlustur(99, $calistirmaSirasi, 'C');

        $app = new App([$bootstrap50, $bootstrap10, $bootstrap99]);
        $app->run();

        $this->assertSame(['A', 'B', 'C'], $calistirmaSirasi);
    }

    // ─────────────────────────────────────────────────────────────
    // Kernel testleri
    // ─────────────────────────────────────────────────────────────

    public function test_kernel_basarili_baslatilir(): void
    {
        $kernel = new Kernel(new App());
        $kernel->run();

        $this->assertTrue($kernel->baslatildiMi());
        $this->assertNotNull($kernel->getBaslatmaSuresi());
    }

    public function test_kernel_iki_kez_cagrilamaz(): void
    {
        $this->expectException(KernelException::class);

        $kernel = new Kernel(new App());
        $kernel->run();
        $kernel->run(); // İstisna bekleniyor
    }

    public function test_hata_veren_bootstrap_exception_firlatir(): void
    {
        $this->expectException(KernelException::class);

        $hataliBootstrap = new class extends AbstractBootstrap {
            public function bootstrap(): void
            {
                throw new \RuntimeException('Bootstrap kasıtlı hatası');
            }
        };

        $kernel = new Kernel(new App([$hataliBootstrap]));
        $kernel->run();
    }

    public function test_kernel_bootstrap_sayisini_dogru_doner(): void
    {
        $b1 = $this->gecerliBootstrapOlustur();
        $b2 = $this->gecerliBootstrapOlustur('IkinciBootstrap');

        $kernel = new Kernel(new App([$b1, $b2]));

        $this->assertSame(2, $kernel->getBootstrapSayisi());
    }

    // ─────────────────────────────────────────────────────────────
    // Yardımcı metodlar
    // ─────────────────────────────────────────────────────────────

    private function gecerliBootstrapOlustur(string $ad = 'TestBootstrap'): BootstrapInterface
    {
        return new class($ad) extends AbstractBootstrap {
            public function __construct(private readonly string $isim) {}

            public function bootstrap(): void
            {
                // Test: herhangi bir işlem yok
            }

            public function getAd(): string
            {
                return $this->isim;
            }
        };
    }

    private function oncelikliBootstrapOlustur(
        int    $oncelik,
        array  &$sira,
        string $etiket
    ): BootstrapInterface {
        return new class($oncelik, $sira, $etiket) extends AbstractBootstrap {
            public function __construct(
                private readonly int    $oncelikDegeri,
                private array           &$sira,
                private readonly string $etiket
            ) {}

            public function bootstrap(): void
            {
                $this->sira[] = $this->etiket;
            }

            public function getOncelik(): int
            {
                return $this->oncelikDegeri;
            }
        };
    }
}
