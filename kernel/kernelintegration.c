/**
 * SimpleOS Kernel - Entegrasyon Katmanı
 * kernelintegration.c
 *
 * kernel.h içindeki tüm fonksiyonları uygular.
 * Derleyici: GCC / Clang (C99+)
 * C++ uyumluluğu için extern "C" bloğu kernel.h içinde tanımlıdır.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
  #include <windows.h>
  #define PLATFORM_UYKU(ms) Sleep(ms)
  typedef CRITICAL_SECTION PlatformKilit;
  #define KILIT_BASLAT(k)   InitializeCriticalSection(&(k))
  #define KILIT_AL(k)        EnterCriticalSection(&(k))
  #define KILIT_BIRAK(k)     LeaveCriticalSection(&(k))
  #define KILIT_KALDIR(k)    DeleteCriticalSection(&(k))
#else
  #include <unistd.h>
  #include <pthread.h>
  #define PLATFORM_UYKU(ms) usleep((ms) * 1000)
  typedef pthread_mutex_t PlatformKilit;
  #define KILIT_BASLAT(k)   pthread_mutex_init(&(k), NULL)
  #define KILIT_AL(k)        pthread_mutex_lock(&(k))
  #define KILIT_BIRAK(k)     pthread_mutex_unlock(&(k))
  #define KILIT_KALDIR(k)    pthread_mutex_destroy(&(k))
#endif

#include "kernel.h"

/* ─── Sabitler ────────────────────────────────────────────── */
#define MAKS_GOREV_SAYISI     256
#define VARSAYILAN_IS_PARCACIGI 4
#define VARSAYILAN_BELLEK_KB    4096
#define VARSAYILAN_ZAMAN_ASIMI  5000
#define LOG_TAMPON_BOYUTU       512
#define VERSIYON_DIZISI         "0.1.0"

/* ─── Görev Yapısı ────────────────────────────────────────── */
typedef enum {
    GOREV_BEKLIOR   = 0,
    GOREV_CALISYOR  = 1,
    GOREV_BITTI     = 2,
    GOREV_IPTAL     = 3,
    GOREV_HATA      = 4,
} GorevDurum;

typedef struct {
    KernelGorevId    id;
    KernelGorevFonk  fonk;
    void*            veri;
    uint8_t          oncelik;
    GorevDurum       durum;
    KernelHataKodu   sonuc;
    uint64_t         olusturma_zamani;
    uint64_t         bitis_zamani;
} Gorev;

/* ─── Kernel İç Durumu ────────────────────────────────────── */
typedef struct {
    KernelDurum          durum;
    KernelAyarlar        ayarlar;
    Gorev                gorevler[MAKS_GOREV_SAYISI];
    uint32_t             gorev_sayisi;
    uint32_t             toplam_gorev;
    uint32_t             hata_sayisi;
    uint64_t             baslama_zamani;

    /* Loglama */
    KernelLogGeriCagri   log_geri_cagri;
    void*                log_kullanici_verisi;

    /* Bellek havuzu (basit arena) */
    uint8_t*             bellek_havuzu;
    size_t               bellek_kullanim;
    size_t               bellek_kapasite;

    /* Eşzamanlılık */
    PlatformKilit        kilit;
    int                  kilit_baslandi;
} KernelIcDurum;

/* ─── Küresel Durum ───────────────────────────────────────── */
static KernelIcDurum g_kernel = { 0 };

/* ─── Yardımcı: Zaman ─────────────────────────────────────── */
static uint64_t zaman_ms_al(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
}

/* ─── Yardımcı: Dahili Log ────────────────────────────────── */
static const char* log_seviye_adi(LogSeviye s) {
    switch (s) {
        case LOG_SEVIYE_DEBUG:  return "DEBUG";
        case LOG_SEVIYE_BILGI:  return "BİLGİ";
        case LOG_SEVIYE_UYARI:  return "UYARI";
        case LOG_SEVIYE_HATA:   return "HATA ";
        case LOG_SEVIYE_KRITIK: return "KRİTİK";
        default:                return "?????";
    }
}

void kernelLog(LogSeviye seviye, const char* format, ...) {
    if (seviye < g_kernel.ayarlar.log_seviye) return;

    char tampon[LOG_TAMPON_BOYUTU];
    va_list args;
    va_start(args, format);
    vsnprintf(tampon, sizeof(tampon), format, args);
    va_end(args);

    if (g_kernel.log_geri_cagri) {
        g_kernel.log_geri_cagri(seviye, tampon, g_kernel.log_kullanici_verisi);
    } else {
        uint64_t sure = zaman_ms_al() - g_kernel.baslama_zamani;
        fprintf(
            (seviye >= LOG_SEVIYE_HATA) ? stderr : stdout,
            "[%7llu ms][%s] %s\n",
            (unsigned long long)sure,
            log_seviye_adi(seviye),
            tampon
        );
    }
}

void kernelLogGeriCagriKaydet(KernelLogGeriCagri geri_cagri,
                               void*              kullanici_verisi) {
    g_kernel.log_geri_cagri      = geri_cagri;
    g_kernel.log_kullanici_verisi = kullanici_verisi;
}

/* ─── Bellek Havuzu ───────────────────────────────────────── */
static KernelHataKodu bellek_havuzu_baslat(size_t kapasite_kb) {
    size_t kapasite = kapasite_kb * 1024;
    g_kernel.bellek_havuzu = (uint8_t*)malloc(kapasite);
    if (!g_kernel.bellek_havuzu) return KERNEL_HATA_BELLEK;
    memset(g_kernel.bellek_havuzu, 0, kapasite);
    g_kernel.bellek_kapasite = kapasite;
    g_kernel.bellek_kullanim = 0;
    return KERNEL_BASARILI;
}

void* kernelBellekAyir(size_t boyut) {
    /* 8-bayt hizalamak için yuvarla */
    size_t hizalanmis = (boyut + 7) & ~(size_t)7;

    if (g_kernel.bellek_kullanim + hizalanmis > g_kernel.bellek_kapasite) {
        kernelLog(LOG_SEVIYE_HATA,
                  "Bellek havuzu dolu: istenen=%zu, kalan=%zu",
                  hizalanmis,
                  g_kernel.bellek_kapasite - g_kernel.bellek_kullanim);
        return NULL;
    }
    void* ptr = g_kernel.bellek_havuzu + g_kernel.bellek_kullanim;
    g_kernel.bellek_kullanim += hizalanmis;
    return ptr;
}

void kernelBellekSerbest(void* ptr) {
    /* Arena tahsisatı; serbest bırakma şu an desteklenmiyor */
    (void)ptr;
    kernelLog(LOG_SEVIYE_DEBUG,
              "kernelBellekSerbest: arena bellekte serbest bırakma göz ardı edildi");
}

/* ─── Yaşam Döngüsü ───────────────────────────────────────── */

KernelHataKodu kernelBaslat(const KernelAyarlar* ayarlar) {
    if (g_kernel.durum != KERNEL_DURUM_BASLATILMADI &&
        g_kernel.durum != KERNEL_DURUM_KAPATILDI) {
        return KERNEL_HATA_ZATEN_BASLADI;
    }

    g_kernel.durum = KERNEL_DURUM_BASLATILIYOR;
    g_kernel.baslama_zamani = zaman_ms_al();

    /* Ayarları uygula */
    if (ayarlar) {
        g_kernel.ayarlar = *ayarlar;
    } else {
        g_kernel.ayarlar.maks_is_parcacigi = VARSAYILAN_IS_PARCACIGI;
        g_kernel.ayarlar.bellek_havuzu_kb  = VARSAYILAN_BELLEK_KB;
        g_kernel.ayarlar.zaman_asimi_ms    = VARSAYILAN_ZAMAN_ASIMI;
        g_kernel.ayarlar.hata_ayiklama_modu= 0;
        g_kernel.ayarlar.log_seviye        = LOG_SEVIYE_BILGI;
    }

    kernelLog(LOG_SEVIYE_BILGI, "Kernel başlatılıyor (v%s)...", VERSIYON_DIZISI);

    /* Bellek havuzunu başlat */
    KernelHataKodu hata = bellek_havuzu_baslat(g_kernel.ayarlar.bellek_havuzu_kb);
    if (hata != KERNEL_BASARILI) {
        kernelLog(LOG_SEVIYE_KRITIK, "Bellek havuzu başlatılamadı!");
        g_kernel.durum = KERNEL_DURUM_HATA;
        return hata;
    }

    /* Kilit başlat */
    KILIT_BASLAT(g_kernel.kilit);
    g_kernel.kilit_baslandi = 1;

    /* Görev kuyruğunu sıfırla */
    memset(g_kernel.gorevler, 0, sizeof(g_kernel.gorevler));
    g_kernel.gorev_sayisi  = 0;
    g_kernel.toplam_gorev  = 0;
    g_kernel.hata_sayisi   = 0;

    g_kernel.durum = KERNEL_DURUM_CALISYOR;
    kernelLog(LOG_SEVIYE_BILGI,
              "Kernel başarıyla başlatıldı. Bellek: %u KB, İş parçacığı: %u",
              g_kernel.ayarlar.bellek_havuzu_kb,
              g_kernel.ayarlar.maks_is_parcacigi);

    return KERNEL_BASARILI;
}

/* Orijinal API uyumluluğu */
void initializeKernel(void) {
    KernelHataKodu hata = kernelBaslat(NULL);
    if (hata != KERNEL_BASARILI) {
        fprintf(stderr, "[Kernel] HATA: Başlatma başarısız, kod=%d\n", hata);
    }
}

KernelHataKodu kernelKapat(int zorla) {
    if (g_kernel.durum == KERNEL_DURUM_KAPATILDI) return KERNEL_BASARILI;
    if (g_kernel.durum == KERNEL_DURUM_BASLATILMADI) return KERNEL_HATA_BASLAMADI;

    g_kernel.durum = KERNEL_DURUM_KAPATILIYOR;
    kernelLog(LOG_SEVIYE_BILGI, "Kernel kapatılıyor (zorla=%d)...", zorla);

    /* Bekleyen görevleri iptal et */
    if (g_kernel.kilit_baslandi) KILIT_AL(g_kernel.kilit);
    for (uint32_t i = 0; i < g_kernel.gorev_sayisi; i++) {
        if (g_kernel.gorevler[i].durum == GOREV_BEKLIOR ||
            g_kernel.gorevler[i].durum == GOREV_CALISYOR) {
            g_kernel.gorevler[i].durum = GOREV_IPTAL;
        }
    }
    if (g_kernel.kilit_baslandi) KILIT_BIRAK(g_kernel.kilit);

    /* Bellek havuzunu serbest bırak */
    if (g_kernel.bellek_havuzu) {
        free(g_kernel.bellek_havuzu);
        g_kernel.bellek_havuzu  = NULL;
        g_kernel.bellek_kullanim = 0;
        g_kernel.bellek_kapasite = 0;
    }

    if (g_kernel.kilit_baslandi) {
        KILIT_KALDIR(g_kernel.kilit);
        g_kernel.kilit_baslandi = 0;
    }

    uint64_t sure = zaman_ms_al() - g_kernel.baslama_zamani;
    kernelLog(LOG_SEVIYE_BILGI,
              "Kernel kapatıldı. Çalışma süresi: %llu ms, "
              "Toplam görev: %u, Hata: %u",
              (unsigned long long)sure,
              g_kernel.toplam_gorev,
              g_kernel.hata_sayisi);

    g_kernel.durum = KERNEL_DURUM_KAPATILDI;
    return KERNEL_BASARILI;
}

/* Orijinal API uyumluluğu */
void shutdownKernel(void) {
    kernelKapat(0);
}

/* ─── Ana Döngü ───────────────────────────────────────────── */

void kernelMain(void) {
    kernelLog(LOG_SEVIYE_BILGI, "Kernel ana döngüsü başladı.");

    while (g_kernel.durum == KERNEL_DURUM_CALISYOR) {
        int gorev_calistirildi = 0;

        /* En yüksek öncelikli bekleyen görevi bul */
        KILIT_AL(g_kernel.kilit);
        Gorev* secilen = NULL;
        for (uint32_t i = 0; i < g_kernel.gorev_sayisi; i++) {
            Gorev* g = &g_kernel.gorevler[i];
            if (g->durum == GOREV_BEKLIOR) {
                if (!secilen || g->oncelik > secilen->oncelik) {
                    secilen = g;
                }
            }
        }
        if (secilen) secilen->durum = GOREV_CALISYOR;
        KILIT_BIRAK(g_kernel.kilit);

        if (secilen) {
            kernelLog(LOG_SEVIYE_DEBUG,
                      "Görev #%u çalıştırılıyor (öncelik=%u)...",
                      secilen->id, secilen->oncelik);

            secilen->sonuc = secilen->fonk(secilen->veri);
            secilen->bitis_zamani = zaman_ms_al();

            if (secilen->sonuc == KERNEL_BASARILI) {
                secilen->durum = GOREV_BITTI;
                kernelLog(LOG_SEVIYE_DEBUG,
                          "Görev #%u tamamlandı (%llu ms)",
                          secilen->id,
                          (unsigned long long)(secilen->bitis_zamani -
                                               secilen->olusturma_zamani));
            } else {
                secilen->durum = GOREV_HATA;
                g_kernel.hata_sayisi++;
                kernelLog(LOG_SEVIYE_HATA,
                          "Görev #%u hata ile bitti, kod=%d",
                          secilen->id, secilen->sonuc);
            }
            gorev_calistirildi = 1;
        }

        if (!gorev_calistirildi) {
            /* Bekleyen görev yok — kısa süre bekle */
            PLATFORM_UYKU(1);
        }
    }

    kernelLog(LOG_SEVIYE_BILGI, "Kernel ana döngüsü bitti.");
    kernelKapat(0);
}

/* ─── Görev Yönetimi ──────────────────────────────────────── */

KernelHataKodu kernelGorevPlanla(KernelGorevFonk fonk,
                                  void*           veri,
                                  uint8_t         oncelik,
                                  KernelGorevId*  gorev_id) {
    KERNEL_NULL_KONTROL(fonk);
    KERNEL_NULL_KONTROL(gorev_id);

    if (g_kernel.durum != KERNEL_DURUM_CALISYOR) return KERNEL_HATA_BASLAMADI;

    KILIT_AL(g_kernel.kilit);

    if (g_kernel.gorev_sayisi >= MAKS_GOREV_SAYISI) {
        KILIT_BIRAK(g_kernel.kilit);
        return KERNEL_HATA_BELLEK;
    }

    Gorev* g = &g_kernel.gorevler[g_kernel.gorev_sayisi++];
    g->id               = ++g_kernel.toplam_gorev;
    g->fonk             = fonk;
    g->veri             = veri;
    g->oncelik          = oncelik;
    g->durum            = GOREV_BEKLIOR;
    g->sonuc            = KERNEL_BASARILI;
    g->olusturma_zamani = zaman_ms_al();
    g->bitis_zamani     = 0;

    *gorev_id = g->id;
    KILIT_BIRAK(g_kernel.kilit);

    kernelLog(LOG_SEVIYE_DEBUG,
              "Görev #%u planlandı (öncelik=%u)", *gorev_id, oncelik);
    return KERNEL_BASARILI;
}

KernelHataKodu kernelGorevIptal(KernelGorevId gorev_id) {
    KILIT_AL(g_kernel.kilit);
    for (uint32_t i = 0; i < g_kernel.gorev_sayisi; i++) {
        if (g_kernel.gorevler[i].id == gorev_id) {
            GorevDurum d = g_kernel.gorevler[i].durum;
            if (d == GOREV_BEKLIOR || d == GOREV_CALISYOR) {
                g_kernel.gorevler[i].durum = GOREV_IPTAL;
                KILIT_BIRAK(g_kernel.kilit);
                kernelLog(LOG_SEVIYE_BILGI, "Görev #%u iptal edildi.", gorev_id);
                return KERNEL_BASARILI;
            }
            KILIT_BIRAK(g_kernel.kilit);
            return KERNEL_HATA_GENEL; /* zaten bitti/iptal */
        }
    }
    KILIT_BIRAK(g_kernel.kilit);
    return KERNEL_HATA_GECERSIZ_PARAM;
}

KernelHataKodu kernelGorevBekle(KernelGorevId gorev_id,
                                 uint32_t      zaman_asimi) {
    uint64_t bitis = zaman_ms_al() + (zaman_asimi ? zaman_asimi : UINT64_MAX);

    while (zaman_ms_al() < bitis) {
        KILIT_AL(g_kernel.kilit);
        for (uint32_t i = 0; i < g_kernel.gorev_sayisi; i++) {
            if (g_kernel.gorevler[i].id == gorev_id) {
                GorevDurum d = g_kernel.gorevler[i].durum;
                KILIT_BIRAK(g_kernel.kilit);
                if (d == GOREV_BITTI) return KERNEL_BASARILI;
                if (d == GOREV_HATA)  return KERNEL_HATA_GENEL;
                if (d == GOREV_IPTAL) return KERNEL_HATA_GENEL;
                break;
            }
        }
        KILIT_BIRAK(g_kernel.kilit);
        PLATFORM_UYKU(1);
    }
    return KERNEL_HATA_ZAMAN_ASIMI;
}

/* ─── Durum Sorgulama ─────────────────────────────────────── */

KernelDurum kernelDurumAl(void) {
    return g_kernel.durum;
}

KernelHataKodu kernelDurumBilgisiAl(KernelDurumBilgisi* bilgi) {
    KERNEL_NULL_KONTROL(bilgi);

    bilgi->durum              = g_kernel.durum;
    bilgi->baslama_zamani_ms  = g_kernel.baslama_zamani;
    bilgi->calisma_suresi_ms  = zaman_ms_al() - g_kernel.baslama_zamani;
    bilgi->kullanilan_bellek  = g_kernel.bellek_kullanim;
    bilgi->toplam_gorev_sayisi= g_kernel.toplam_gorev;
    bilgi->hata_sayisi        = g_kernel.hata_sayisi;

    uint32_t aktif = 0;
    KILIT_AL(g_kernel.kilit);
    for (uint32_t i = 0; i < g_kernel.gorev_sayisi; i++) {
        GorevDurum d = g_kernel.gorevler[i].durum;
        if (d == GOREV_BEKLIOR || d == GOREV_CALISYOR) aktif++;
    }
    KILIT_BIRAK(g_kernel.kilit);
    bilgi->aktif_gorev_sayisi = aktif;

    return KERNEL_BASARILI;
}

const char* kernelVersiyonAl(void) {
    return VERSIYON_DIZISI;
}