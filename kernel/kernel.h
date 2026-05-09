#pragma region File Header
/**
 * SimpleOS Kernel
 * Copyright (C) 2024 Mert D.
 * GNU General Public License v3 veya üstü
 */
#pragma endregion

#ifndef KERNEL_H
#define KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ─── Versiyon Sabitleri ───────────────────────────────────── */
#define KERNEL_MAJOR_VERSIYON   0
#define KERNEL_MINOR_VERSIYON   1
#define KERNEL_YAMA_VERSIYON    0

/* ─── Hata Kodları ────────────────────────────────────────── */
typedef enum {
    KERNEL_BASARILI            =  0,
    KERNEL_HATA_GENEL          = -1,
    KERNEL_HATA_BELLEK         = -2,
    KERNEL_HATA_ZATEN_BASLADI  = -3,
    KERNEL_HATA_BASLAMADI      = -4,
    KERNEL_HATA_GECERSIZ_PARAM = -5,
    KERNEL_HATA_ZAMAN_ASIMI    = -6,
    KERNEL_HATA_IZIN_REDDEDILDI= -7,
} KernelHataKodu;

/* ─── Kernel Durumları ────────────────────────────────────── */
typedef enum {
    KERNEL_DURUM_BASLATILMADI  = 0,
    KERNEL_DURUM_BASLATILIYOR  = 1,
    KERNEL_DURUM_CALISYOR      = 2,
    KERNEL_DURUM_DURAKLATILDI  = 3,
    KERNEL_DURUM_KAPATILIYOR   = 4,
    KERNEL_DURUM_KAPATILDI     = 5,
    KERNEL_DURUM_HATA          = 6,
} KernelDurum;

/* ─── Log Seviyeleri ──────────────────────────────────────── */
typedef enum {
    LOG_SEVIYE_DEBUG   = 0,
    LOG_SEVIYE_BILGI   = 1,
    LOG_SEVIYE_UYARI   = 2,
    LOG_SEVIYE_HATA    = 3,
    LOG_SEVIYE_KRITIK  = 4,
} LogSeviye;

/* ─── Yapı Tanımları ──────────────────────────────────────── */

/** Kernel yapılandırma yapısı */
typedef struct {
    uint32_t  maks_is_parcacigi;   /**< Maksimum iş parçacığı sayısı */
    uint32_t  bellek_havuzu_kb;    /**< Bellek havuzu boyutu (KB) */
    uint32_t  zaman_asimi_ms;      /**< Varsayılan zaman aşımı (ms) */
    uint8_t   hata_ayiklama_modu;  /**< 1 = aktif, 0 = pasif */
    uint8_t   log_seviye;          /**< Minimum log seviyesi */
} KernelAyarlar;

/** Kernel durum bilgisi yapısı */
typedef struct {
    KernelDurum  durum;
    uint32_t     aktif_gorev_sayisi;
    uint64_t     baslama_zamani_ms;
    uint64_t     calisma_suresi_ms;
    size_t       kullanilan_bellek;
    uint32_t     toplam_gorev_sayisi;
    uint32_t     hata_sayisi;
} KernelDurumBilgisi;

/** Görev tanımlayıcısı */
typedef uint32_t KernelGorevId;

/** Görev fonksiyonu imzası */
typedef KernelHataKodu (*KernelGorevFonk)(void* veri);

/** Log geri çağırma imzası */
typedef void (*KernelLogGeriCagri)(LogSeviye seviye,
                                    const char* mesaj,
                                    void* kullanici_verisi);

/* ─── Çekirdek Yaşam Döngüsü ─────────────────────────────── */

/**
 * Kernel'i varsayılan ayarlarla başlatır.
 * @return KERNEL_BASARILI veya hata kodu
 */
KernelHataKodu initializeKernel(void);

/**
 * Kernel'i özel ayarlarla başlatır.
 * @param ayarlar  Yapılandırma yapısı (NULL = varsayılan)
 * @return KERNEL_BASARILI veya hata kodu
 */
KernelHataKodu kernelBaslat(const KernelAyarlar* ayarlar);

/**
 * Kernel ana döngüsünü çalıştırır (bloklanır).
 */
void kernelMain(void);

/**
 * Kernel'i temiz biçimde kapatır.
 * @param zorla  0 = bekleyerek kapat, 1 = zorla kapat
 * @return KERNEL_BASARILI veya hata kodu
 */
KernelHataKodu shutdownKernel(void);
KernelHataKodu kernelKapat(int zorla);

/* ─── Durum Sorgulama ─────────────────────────────────────── */

/**
 * Mevcut kernel durumunu döner.
 */
KernelDurum       kernelDurumAl(void);

/**
 * Ayrıntılı durum bilgisini doldurur.
 * @param bilgi  Doldurulacak yapı (NULL olamaz)
 */
KernelHataKodu    kernelDurumBilgisiAl(KernelDurumBilgisi* bilgi);

/**
 * Kernel versiyon dizisini döner ("0.1.0" formatında).
 */
const char*       kernelVersiyonAl(void);

/* ─── Görev Yönetimi ──────────────────────────────────────── */

/**
 * Yeni bir görev planlar.
 * @param fonk      Çalıştırılacak fonksiyon
 * @param veri      Fonksiyona aktarılacak veri (NULL olabilir)
 * @param oncelik   0-255 arası öncelik (255 = en yüksek)
 * @param gorev_id  Çıkış: atanan görev ID'si
 * @return KERNEL_BASARILI veya hata kodu
 */
KernelHataKodu kernelGorevPlanla(KernelGorevFonk fonk,
                                  void*           veri,
                                  uint8_t         oncelik,
                                  KernelGorevId*  gorev_id);

/**
 * Bir görevi iptal eder.
 * @param gorev_id  İptal edilecek görev
 */
KernelHataKodu kernelGorevIptal(KernelGorevId gorev_id);

/**
 * Belirli bir görevin bitmesini bekler.
 * @param gorev_id     Beklenen görev
 * @param zaman_asimi  Maksimum bekleme süresi ms (0 = sonsuz)
 */
KernelHataKodu kernelGorevBekle(KernelGorevId gorev_id,
                                 uint32_t      zaman_asimi);

/* ─── Bellek Yönetimi ─────────────────────────────────────── */

/**
 * Kernel bellek havuzundan bellek ayırır.
 * @param boyut  Ayrılacak bayt sayısı
 * @return Ayrılan bellek adresi, başarısızsa NULL
 */
void* kernelBellekAyir(size_t boyut);

/**
 * Kernel bellek havuzuna bellek serbest bırakır.
 * @param ptr  Serbest bırakılacak adres
 */
void  kernelBellekSerbest(void* ptr);

/* ─── Loglama ─────────────────────────────────────────────── */

/**
 * Özel log geri çağırma fonksiyonu kaydeder.
 * @param geri_cagri   Çağrılacak fonksiyon (NULL = varsayılan stdout)
 * @param kullanici_verisi  Geri çağrıya aktarılacak veri
 */
void kernelLogGeriCagriKaydet(KernelLogGeriCagri geri_cagri,
                               void*              kullanici_verisi);

/**
 * Kernel log mesajı yazar.
 */
void kernelLog(LogSeviye seviye, const char* format, ...);

/* ─── Yardımcı Makrolar ───────────────────────────────────── */

#define KERNEL_HATA_KONTROL(ifade)                          \
    do {                                                     \
        KernelHataKodu _hata = (ifade);                     \
        if (_hata != KERNEL_BASARILI) return _hata;          \
    } while (0)

#define KERNEL_NULL_KONTROL(ptr)                            \
    do {                                                     \
        if ((ptr) == NULL) return KERNEL_HATA_GECERSIZ_PARAM;\
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_H */