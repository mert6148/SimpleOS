<?php

require_once __DIR__ . '/vendor/autoload.php';

use Kernel\App\App;
use Kernel\App\Bootstraps\HataYonetimiBootstrap;
use Kernel\App\Bootstraps\OrtamBootstrap;
use Kernel\Kernel\Kernel;
use Kernel\Kernel\KernelFactory;
use Kernel\ServiceContainer\Container;

// ─────────────────────────────────────────────────────────────
// YÖNTEM 1: KernelFactory ile hızlı başlatma
// ─────────────────────────────────────────────────────────────

$kernel = KernelFactory::olustur(
    ortam: 'development',
    projeDizini: __DIR__
);

$kernel->run();

printf(
    "✓ Kernel başlatıldı | Bootstrap sayısı: %d | Süre: %.4f sn\n",
    $kernel->getBootstrapSayisi(),
    $kernel->getBaslatmaSuresi() ?? 0.0
);

// ─────────────────────────────────────────────────────────────
// YÖNTEM 2: Manuel, tam kontrollü kurulum
// ─────────────────────────────────────────────────────────────

$konteyner = new Container();
$app = new App(konteyner: $konteyner);

// Bootstrap'leri tek tek ekle
$app->addBootstrap(new OrtamBootstrap('.env'));
$app->addBootstrap(new HataYonetimiBootstrap('development'));

$kernel2 = new Kernel($app);
$kernel2->run();

printf(
    "✓ Kernel-2 başlatıldı | Bootstrap sayısı: %d | Süre: %.4f sn\n",
    $kernel2->getBootstrapSayisi(),
    $kernel2->getBaslatmaSuresi() ?? 0.0
);

// ─────────────────────────────────────────────────────────────
// YÖNTEM 3: Özel bootstrap ile genişletme
// ─────────────────────────────────────────────────────────────

use Kernel\App\Bootstraps\AbstractBootstrap;

$ozelBootstrap = new class extends AbstractBootstrap {
    public function bootstrap(): void
    {
        // Örnek: Veritabanı bağlantısını kur
        echo "✓ Veritabanı bootstrap çalıştı\n";
    }

    public function getOncelik(): int
    {
        return 50; // Hata yönetiminden sonra, servislerden önce
    }

    public function getAd(): string
    {
        return 'VeritabaniBootstrap';
    }
};

$app3 = new App([$ozelBootstrap]);
$kernel3 = new Kernel($app3);
$kernel3->run();
