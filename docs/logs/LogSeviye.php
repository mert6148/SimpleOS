<?php

declare(strict_types=1);

namespace SimpleOS\Docs\Log;

/**
 * Log seviyelerini tanımlayan enum.
 * RFC 5424 (Syslog) standartlarına uygun.
 */
enum LogSeviye: int
{
    case ACIL    = 0;  // Sistem kullanılamaz
    case ALERT   = 1;  // Hemen müdahale gerekli
    case KRITIK  = 2;  // Kritik durum
    case HATA    = 3;  // Hata
    case UYARI   = 4;  // Uyarı
    case BILGI   = 5;  // Bilgilendirme (varsayılan)
    case DEBUG   = 6;  // Hata ayıklama
    case DETAY   = 7;  // Ayrıntılı izleme

    /** Okunabilir etiket döner */
    public function etiketi(): string
    {
        return match($this) {
            self::ACIL   => 'ACİL',
            self::ALERT  => 'ALERT',
            self::KRITIK => 'KRİTİK',
            self::HATA   => 'HATA',
            self::UYARI  => 'UYARI',
            self::BILGI  => 'BİLGİ',
            self::DEBUG  => 'DEBUG',
            self::DETAY  => 'DETAY',
        };
    }

    /** ANSI renk kodu döner (terminal çıktısı için) */
    public function rengi(): string
    {
        return match($this) {
            self::ACIL,
            self::KRITIK => "\033[1;31m",  // Kalın kırmızı
            self::ALERT  => "\033[0;31m",  // Kırmızı
            self::HATA   => "\033[0;91m",  // Açık kırmızı
            self::UYARI  => "\033[0;33m",  // Sarı
            self::BILGI  => "\033[0;36m",  // Cyan
            self::DEBUG  => "\033[0;90m",  // Gri
            self::DETAY  => "\033[0;37m",  // Beyaz
        };
    }

    /** Seviyenin belirtilenden daha önemli olup olmadığını kontrol et */
    public function enAzindan(self $minimum): bool
    {
        return $this->value <= $minimum->value;
    }
}
