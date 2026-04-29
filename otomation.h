/**
 * SimpleOS Otomasyon Sistemi
 * otomasyon.h — Genel tanımlar, yapılar, sabitler
 *
 * Kaynak dosyaların hatalı kısımlarından türetilen
 * temiz, derlenebilir C99 başlık dosyası.
 *
 * @author  Mert Doğanay
 * @version 2.0.0
 * @license MIT
 */

#ifndef SIMPLEOS_OTOMASYON_H
#define SIMPLEOS_OTOMASYON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
  #include <windows.h>
  #define PLATFORM_UYKU_MS(ms) Sleep(ms)
#else
  #include <unistd.h>
  #define PLATFORM_UYKU_MS(ms) usleep((useconds_t)(ms) * 1000u)
#endif

/* ─── Versiyon ──────────────────────────────────────────── */
#define OT_MAJOR 2
#define OT_MINOR 0
#define OT_YAMA  0

/* ─── Kapasite Sabitleri ────────────────────────────────── */
#define OT_MAKS_KOMUT_UZUNLUGU   256
#define OT_MAKS_DUGUM_SAYISI     64
#define OT_MAKS_KUYRUK_BOYUTU    128
#define OT_MAKS_ISIM_UZUNLUGU    64
#define OT_MAKS_DEGER_UZUNLUGU   256
#define OT_MAKS_LOG_SATIRI       512

/* ─── Hata Kodları ──────────────────────────────────────── */
typedef enum {
    OT_BASARILI           =  0,
    OT_HATA_GENEL         = -1,
    OT_HATA_BELLEK        = -2,
    OT_HATA_GECERSIZ_ARG  = -3,
    OT_HATA_BILINMIYOR    = -4,
    OT_HATA_ZAMAN_ASIMI   = -5,
    OT_HATA_KAPASITE      = -6,
    OT_HATA_IZIN          = -7,
    OT_HATA_IO            = -8,
} OtHataKodu;

/* ─── Komut Tipleri ─────────────────────────────────────── */
typedef enum {
    KOMUT_YAZDIR  = 0,   /* os_docs.c → "print"  */
    KOMUT_CALISTIR= 1,   /* otomation.c → işlem  */
    KOMUT_DUR     = 2,   /* os_docs.c  → "exit"  */
    KOMUT_DURAKLA = 3,
    KOMUT_DEVAM   = 4,
    KOMUT_BILINMIYOR = 99,
} KomutTipi;

/* ─── Değer Düğümü (simple.c → postcss::Value) ─────────── */
typedef enum {
    DUGUM_KELIME  = 0,   /* "word"   */
    DUGUM_METIN   = 1,   /* "string" */
    DUGUM_BOSLUK  = 2,   /* "space"  */
    DUGUM_FUNC    = 3,   /* "func"   */
    DUGUM_BILINMIYOR = 99,
} DugumTipi;

typedef struct {
    DugumTipi   tip;
    char        deger[OT_MAKS_DEGER_UZUNLUGU];
    char        ham[OT_MAKS_DEGER_UZUNLUGU];   /* raw */
    uint32_t    satir;
    uint32_t    sutun;
} DegerDugumu;

/* ─── Basit Değer Kümesi (simple.c → Simple sınıfı) ────── */
typedef struct {
    char         deger[OT_MAKS_DEGER_UZUNLUGU];
    char         metin[OT_MAKS_DEGER_UZUNLUGU];
    char         ham[OT_MAKS_DEGER_UZUNLUGU];
    DegerDugumu  dugumler[OT_MAKS_DUGUM_SAYISI];
    size_t       dugum_sayisi;
} BasitDeger;

/* ─── Kesir Değeri (otomation.c → Fraction sınıfı) ─────── */
typedef struct {
    BasitDeger   temel;
    int32_t      pay;       /* pay   */
    int32_t      payda;     /* payda */
    double       ondalik;   /* hesaplanmış ondalık değer */
} KesirDeger;

/* ─── Komut Yapısı ──────────────────────────────────────── */
typedef struct {
    KomutTipi  tip;
    char       isim[OT_MAKS_KOMUT_UZUNLUGU];
    char       arguman[OT_MAKS_KOMUT_UZUNLUGU];
    uint32_t   oncelik;     /* 0 = en düşük */
    bool       aktif;
} Komut;

/* ─── FIFO Komut Kuyruğu ────────────────────────────────── */
typedef struct {
    Komut    ogeler[OT_MAKS_KUYRUK_BOYUTU];
    size_t   bas;
    size_t   son;
    size_t   boyut;
    bool     kilitli;       /* iş parçacığı güvenliği için basit kilit */
} KomutKuyrugu;

/* ─── Otomasyon Motoru Yapılandırması ───────────────────── */
typedef struct {
    uint32_t zaman_asimi_ms;      /* her komut için azami süre (ms)  */
    uint32_t tekrar_araligi_ms;   /* komutlar arası bekleme süresi    */
    bool     hata_ayiklama;       /* ayrıntılı log açık/kapalı        */
    bool     hata_durumunda_dur;  /* hata alınca işlemi durdur        */
    FILE    *log_akisi;           /* NULL → stderr                    */
} OtomasyonAyarlari;

/* ─── Otomasyon Motoru ──────────────────────────────────── */
typedef struct {
    OtomasyonAyarlari ayarlar;
    KomutKuyrugu      kuyruk;
    uint64_t          calistirilan_komut;
    uint64_t          basarili_komut;
    uint64_t          basarisiz_komut;
    bool              calisyor;
    time_t            baslama_zamani;
} OtomasyonMotoru;

/* ─── Fonksiyon Bildirimleri ────────────────────────────── */

/* simple.c → BasitDeger işlemleri */
void basit_deger_baslat(BasitDeger *bd);
void basit_deger_ata(BasitDeger *bd, const char *deger);
void basit_deger_ata_ham(BasitDeger *bd, const char *deger,
                          const char *metin, const char *ham);
OtHataKodu basit_deger_dugum_ekle(BasitDeger *bd, DegerDugumu dugum);
void basit_deger_isle(const BasitDeger *bd);

/* otomation.c → KesirDeger işlemleri */
void kesir_baslat(KesirDeger *kd, int32_t pay, int32_t payda);
OtHataKodu kesir_ata_metin(KesirDeger *kd, const char *metin);
double kesir_ondalik_al(const KesirDeger *kd);
void kesir_yazdir(const KesirDeger *kd, FILE *akis);

/* os_docs.c → komut sistemi */
KomutTipi komut_cozumle(const char *metin);
OtHataKodu komut_kuyrugu_baslat(KomutKuyrugu *kq);
OtHataKodu komut_kuyrugu_ekle(KomutKuyrugu *kq, const Komut *komut);
OtHataKodu komut_kuyrugu_al(KomutKuyrugu *kq, Komut *hedef);
bool komut_kuyrugu_bos_mu(const KomutKuyrugu *kq);
size_t komut_kuyrugu_boyut(const KomutKuyrugu *kq);

/* Otomasyon motoru */
OtHataKodu otomasyon_baslat(OtomasyonMotoru *mot, const OtomasyonAyarlari *ayarlar);
OtHataKodu otomasyon_komut_ekle(OtomasyonMotoru *mot, const char *komut_metni,
                                  const char *arguman, uint32_t oncelik);
OtHataKodu otomasyon_calistir(OtomasyonMotoru *mot);
OtHataKodu otomasyon_durdur(OtomasyonMotoru *mot);
void otomasyon_istatistik_yazdir(const OtomasyonMotoru *mot, FILE *akis);
void otomasyon_temizle(OtomasyonMotoru *mot);

/* Yardımcı */
const char *hata_acikla(OtHataKodu kod);
void ot_log(const OtomasyonMotoru *mot, const char *seviye, const char *fmt, ...);

/* Makrolar */
#define OT_HATA_KONTROL(ifade)                                     \
    do {                                                            \
        OtHataKodu _k = (ifade);                                   \
        if (_k != OT_BASARILI) return _k;                          \
    } while (0)

#define OT_NULL_KONTROL(ptr)                                        \
    do {                                                            \
        if ((ptr) == NULL) return OT_HATA_GECERSIZ_ARG;            \
    } while (0)

#endif /* SIMPLEOS_OTOMASYON_H */