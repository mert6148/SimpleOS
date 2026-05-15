<?php

/**
 * SimpleOS Documentation Log
 *
 * @version  1.0.0
 * @author   Mert Doğanay
 * @license  MIT
 * @link     https://github.com/mertdoganay/SimpleOS/blob/main/docs/logs/log.php
 *
 * Dahili kullanım içindir. Bu dosyayı yetkisiz kişilerle paylaşmayın.
 *
 * Değişiklik Günlüğü:
 *   2026-04-21  İlk oluşturma.
 *   2026-04-21  Footer ve yazar bilgisi değişiklikleri eklendi.
 *   2026-04-21  Log biçimi güncellendi, ek meta veri desteği eklendi.
 *   2026-04-26  Tam log motoru yeniden yazıldı: LogDepo, LogFiltre,
 *               DocLog, LogKaydi, LogSeviye sınıfları eklendi.
 */

declare(strict_types=1);

// ─── Otomatik Yükleme ────────────────────────────────────────
// Composer yoksa sınıfları elle yükle
$siniflar = [
    __DIR__ . '/LogSeviye.php',
    __DIR__ . '/LogKaydi.php',
    __DIR__ . '/LogFiltre.php',
    __DIR__ . '/LogDepo.php',
    __DIR__ . '/DocLog.php',
];
foreach ($siniflar as $dosya) {
    require_once $dosya;
}

use SimpleOS\Docs\Log\DocLog;
use SimpleOS\Docs\Log\LogFiltre;
use SimpleOS\Docs\Log\LogSeviye;
use SimpleOS\Docs\Log\LogKategori;

// ─── Yapılandırma ─────────────────────────────────────────────
$depoKlasoru    = __DIR__ . '/../storage';
$varsayilanYazar = 'Mert Doğanay';
$limitVar        = (int) ($_GET['limit']  ?? 50);
$formatVar       = in_array($_GET['format'] ?? '', ['json', 'text']) ? $_GET['format'] : 'text';
$eylem           = $_GET['eylem'] ?? '';

// ─── Log Örneği ───────────────────────────────────────────────
$log = DocLog::olustur($depoKlasoru, $varsayilanYazar);

// ─── HTTP Eylemleri ───────────────────────────────────────────
//   ?eylem=yukle     : Başlangıç kayıtlarını yükle (demo)
//   ?eylem=temizle   : Eski kayıtları temizle (30 günden eski)
//   ?format=json     : JSON çıktısı
//   ?limit=N         : Son N kaydı göster

switch ($eylem) {
    case 'yukle':
        // Başlangıç demo kayıtları — gerçek ortamda bu blok kaldırılır
        $log->olusturuldu('README.md',                $varsayilanYazar);
        $log->olusturuldu('docs/api/reference.md',    $varsayilanYazar);
        $log->olusturuldu('docs/kernel/kernel.md',    $varsayilanYazar);
        $log->duzenlendi ('docs/kernel/kernel.md',    $varsayilanYazar, ['satir_sayisi' => 312]);
        $log->duzenlendi ('docs/api/reference.md',    $varsayilanYazar, ['bolum' => 'endpoint listesi']);
        $log->yazarGuncellendi('README.md',           'Bilinmiyor', $varsayilanYazar);
        $log->versiyonEtiketi('v0.1.0',               $varsayilanYazar, ['not' => 'İlk sürüm']);
        $log->uyari(
            'kernelintegration.c içinde eksik yorum satırı tespit edildi',
            'kernelintegration.c', $varsayilanYazar, LogKategori::HATA->value
        );
        $log->duzenlendi('docs/logs/log.php',         $varsayilanYazar, ['sebep' => 'Sistem yeniden yazıldı']);
        $log->bilgi('Dokümantasyon sistemi başarıyla başlatıldı');

        header('Location: log.php?format=' . $formatVar);
        exit;

    case 'temizle':
        // 30 günden eski kayıtları sil (sadece POST ile izin ver)
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            $tarih   = new \DateTimeImmutable('-30 days');
            $silinen = (new \SimpleOS\Docs\Log\LogDepo($depoKlasoru))->eskiyiTemizle($tarih);
            header('Content-Type: text/plain; charset=utf-8');
            echo "{$silinen} kayıt silindi (30 günden eski).\n";
        } else {
            header('HTTP/1.1 405 Method Not Allowed');
            echo "Bu eylem yalnızca POST isteğiyle çalışır.\n";
        }
        exit;
}

// ─── Filtreleme (GET parametrelerinden) ──────────────────────
$filtre = null;
if (!empty($_GET['seviye'])) {
    try {
        $filtre = LogFiltre::seviyeIle(LogSeviye::from($_GET['seviye']));
    } catch (\ValueError) {}
}
if (!empty($_GET['kategori'])) {
    $filtre ??= new LogFiltre();
    $filtre->kategori = $_GET['kategori'];
}
if (!empty($_GET['yazar'])) {
    $filtre ??= new LogFiltre();
    $filtre->yazar = $_GET['yazar'];
}
if (!empty($_GET['ara'])) {
    $filtre ??= new LogFiltre();
    $filtre->aramaMetni = $_GET['ara'];
}

// ─── HTTP Yanıtı ─────────────────────────────────────────────
// Tek seferde header gönder — orijinaldeki duplicate sorunu çözülüyor
$log->httpYaniti($limitVar, $formatVar);