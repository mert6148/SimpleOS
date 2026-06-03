/**
 * =============================================================================
 *  SimpleOS — Otomasyon Motoru  |  otomation.c
 *  Sürüm   : 3.0.0
 *  Açıklama: Komut motoru, kuyruk yönetimi, kesir işlemleri, olay sistemi,
 *             zamanlayıcı, boru hattı, güvenlik katmanı, istatistik motoru,
 *             günlük sistemi, serileştirme ve platform soyutlama katmanı.
 * =============================================================================
 */

#include "otomation.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Platform uyumluluk ─────────────────────────────────────────────────── */
#if defined(_WIN32) || defined(_WIN64)
#  define OT_WINDOWS 1
#  include <windows.h>
#  define PLATFORM_UYKU_MS(ms)  Sleep((DWORD)(ms))
#  define OT_PATH_SEP           '\\'
#else
#  include <unistd.h>
#  define PLATFORM_UYKU_MS(ms)  usleep((useconds_t)((ms) * 1000u))
#  define OT_PATH_SEP           '/'
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 1 — SABİTLER VE MAKROLAR
 * ═══════════════════════════════════════════════════════════════════════════*/

#define OT_SURUM_MAJOR          3
#define OT_SURUM_MINOR          0
#define OT_SURUM_YAMA           0
#define OT_SURUM_DIZI           "3.0.0"

#define OT_MAKS_DEGER_UZUNLUGU  256u
#define OT_MAKS_KOMUT_UZUNLUGU  128u
#define OT_MAKS_KUYRUK_BOYUTU   512u
#define OT_MAKS_DUGUM_SAYISI    64u
#define OT_MAKS_BORU_ASAMASI    32u
#define OT_MAKS_OLAY_DINLEYICI  64u
#define OT_MAKS_ZAMANLAYICI     128u
#define OT_MAKS_IS_PARCACIGI    32u
#define OT_MAKS_LOG_SATIRI      1024u
#define OT_MAKS_HATA_YIGINI     16u
#define OT_MAKS_ENV_DEGISKEN    128u
#define OT_MAKS_FILTRE_KURAL    64u
#define OT_MAKS_EKLENTI         32u
#define OT_MAKS_METAVERI        48u

#define OT_NULL_KONTROL(p)                              \
    do {                                                \
        if (!(p)) return OT_HATA_GECERSIZ_ARG;         \
    } while (0)

#define OT_NULL_KONTROL_VOID(p)                         \
    do {                                                \
        if (!(p)) return;                               \
    } while (0)

#define OT_HATA_KONTROL(expr)                           \
    do {                                                \
        OtHataKodu _kod = (expr);                       \
        if (_kod != OT_BASARILI) return _kod;           \
    } while (0)

#define OT_DIZI_BOYUTU(a)   (sizeof(a) / sizeof((a)[0]))
#define OT_MIN(a, b)        ((a) < (b) ? (a) : (b))
#define OT_MAX(a, b)        ((a) > (b) ? (a) : (b))
#define OT_KISIT(v, mn, mx) OT_MIN(OT_MAX((v), (mn)), (mx))
#define OT_KULLANILMADI(x)  ((void)(x))

/* Derleme zamanı doğrulama */
#define OT_STATIK_ASSERT(kosul, mesaj) \
    typedef char ot_statik_assert_##mesaj[(kosul) ? 1 : -1]

OT_STATIK_ASSERT(sizeof(uint32_t) == 4, uint32_boyut_4_olmali);
OT_STATIK_ASSERT(sizeof(uint64_t) == 8, uint64_boyut_8_olmali);

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 2 — TİP TANIMLAMALARI
 * ═══════════════════════════════════════════════════════════════════════════*/

/* Hata kodları */
typedef enum {
    OT_BASARILI            =  0,
    OT_HATA_GENEL          = -1,
    OT_HATA_BELLEK         = -2,
    OT_HATA_GECERSIZ_ARG   = -3,
    OT_HATA_BILINMIYOR     = -4,
    OT_HATA_ZAMAN_ASIMI    = -5,
    OT_HATA_KAPASITE       = -6,
    OT_HATA_IZIN           = -7,
    OT_HATA_IO             = -8,
    OT_HATA_DURUMU         = -9,
    OT_HATA_VAROLAN        = -10,
    OT_HATA_BULUNAMADI     = -11,
    OT_HATA_AKIS           = -12,
    OT_HATA_SIRALAMA       = -13,
    OT_HATA_DOGRULAMA      = -14,
    OT_HATA_IPTAL          = -15,
} OtHataKodu;

/* Komut tipleri */
typedef enum {
    KOMUT_BILINMIYOR =  0,
    KOMUT_YAZDIR     =  1,
    KOMUT_DUR        =  2,
    KOMUT_CALISTIR   =  3,
    KOMUT_DURAKLA    =  4,
    KOMUT_DEVAM      =  5,
    KOMUT_YUKLE      =  6,
    KOMUT_KAYDET     =  7,
    KOMUT_TEMIZLE    =  8,
    KOMUT_LISTELE    =  9,
    KOMUT_FILTRELE   = 10,
    KOMUT_SIRALA     = 11,
    KOMUT_AKTAR      = 12,
    KOMUT_DOGRULA    = 13,
    KOMUT_SIFIRLA    = 14,
    KOMUT_DURUM      = 15,
    KOMUT_YARDIM     = 16,
    KOMUT_SURUM      = 17,
    KOMUT_GECMIS     = 18,
    KOMUT_BEKLE      = 19,
    KOMUT_TEKRAR     = 20,
    KOMUT_KOSULLU    = 21,
    KOMUT_DONGU      = 22,
    KOMUT_KESINTI    = 23,
    KOMUT_ATLA       = 24,
} KomutTipi;

/* Olay tipleri */
typedef enum {
    OLAY_BILINMIYOR         = 0,
    OLAY_MOTOR_BASLADI      = 1,
    OLAY_MOTOR_DURDU        = 2,
    OLAY_KOMUT_EKLENDI      = 3,
    OLAY_KOMUT_TAMAMLANDI   = 4,
    OLAY_KOMUT_BASARISIZ    = 5,
    OLAY_KUYRUK_DOLU        = 6,
    OLAY_KUYRUK_BOSTA       = 7,
    OLAY_HATA               = 8,
    OLAY_UYARI              = 9,
    OLAY_BILGI              = 10,
    OLAY_ZAMANLAYICI_ATTI   = 11,
    OLAY_BORU_TAMAMLANDI    = 12,
    OLAY_KAYNAK_AZALDI      = 13,
    OLAY_DOGRULAMA_HATASI   = 14,
} OlayTipi;

/* Günlük seviyeleri */
typedef enum {
    GUNLUK_HATA    = 0,
    GUNLUK_UYARI   = 1,
    GUNLUK_BILGI   = 2,
    GUNLUK_HATA_AY = 3,
    GUNLUK_IZLEME  = 4,
} GunlukSeviyesi;

/* Boru hattı aşama durumu */
typedef enum {
    ASAMA_BEKLIYOR    = 0,
    ASAMA_CALISIYOR   = 1,
    ASAMA_TAMAMLANDI  = 2,
    ASAMA_BASARISIZ   = 3,
    ASAMA_ATLANDI     = 4,
    ASAMA_IPTAL       = 5,
} AsamaDurumu;

/* Zamanlayıcı tipi */
typedef enum {
    ZAMANLAYICI_TEK_SEFER   = 0,
    ZAMANLAYICI_TEKRARLI     = 1,
    ZAMANLAYICI_CRON_BENZERI = 2,
} ZamanLayiciTipi;

/* Düğüm tipi */
typedef enum {
    DUGUM_METIN    = 0,
    DUGUM_SAYI     = 1,
    DUGUM_KESIR    = 2,
    DUGUM_MANTIKSAL= 3,
    DUGUM_LISTE    = 4,
    DUGUM_NESNE    = 5,
    DUGUM_BOŞ      = 6,
} DugumTipi;

/* Güvenlik seviyesi */
typedef enum {
    GUVENLIK_KAPALTI  = 0,
    GUVENLIK_DUSUK    = 1,
    GUVENLIK_ORTA     = 2,
    GUVENLIK_YUKSEK   = 3,
    GUVENLIK_PARANOYA = 4,
} GuvenlikSeviyesi;

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 3 — VERİ YAPISI TANIMLAMALARI
 * ═══════════════════════════════════════════════════════════════════════════*/

typedef struct {
    DugumTipi   tip;
    char        deger[OT_MAKS_DEGER_UZUNLUGU];
} DegerDugumu;

typedef struct {
    char        deger[OT_MAKS_DEGER_UZUNLUGU];
    char        metin[OT_MAKS_DEGER_UZUNLUGU];
    char        ham[OT_MAKS_DEGER_UZUNLUGU];
    DegerDugumu dugumler[OT_MAKS_DUGUM_SAYISI];
    size_t      dugum_sayisi;
} BasitDeger;

typedef struct {
    BasitDeger  temel;
    int32_t     pay;
    int32_t     payda;
    double      ondalik;
    bool        indirgenmi;
} KesirDeger;

typedef struct {
    KomutTipi   tip;
    char        isim[OT_MAKS_KOMUT_UZUNLUGU];
    char        arguman[OT_MAKS_KOMUT_UZUNLUGU];
    char        hedef[OT_MAKS_KOMUT_UZUNLUGU];
    uint32_t    oncelik;
    uint32_t    tekrar_sayisi;
    uint32_t    zaman_asimi_ms;
    bool        aktif;
    bool        engelleme;
    time_t      olusturma_zamani;
    uint64_t    komut_id;
} Komut;

typedef struct {
    Komut       ogeler[OT_MAKS_KUYRUK_BOYUTU];
    size_t      bas;
    size_t      son;
    size_t      boyut;
    uint64_t    toplam_eklenen;
    uint64_t    toplam_alinan;
} KomutKuyrugu;

/* Öncelik kuyruğu öğesi */
typedef struct {
    Komut       komut;
    uint32_t    efektif_oncelik;
    time_t      ekleme_zamani;
} OncelikliKomut;

typedef struct {
    OncelikliKomut  ogeler[OT_MAKS_KUYRUK_BOYUTU];
    size_t          boyut;
    uint64_t        islem_sayisi;
} OncelikliKuyruk;

typedef struct {
    FILE       *akis;
    GunlukSeviyesi seviye;
    bool        renkli;
    bool        zaman_damgali;
    bool        seviye_yazili;
    char        dosya_yolu[512];
    uint64_t    satir_sayisi;
    size_t      max_dosya_boyutu;
} GunlukAyarlari;

typedef struct {
    OlayTipi    tip;
    char        kaynak[64];
    char        mesaj[OT_MAKS_DEGER_UZUNLUGU];
    time_t      zaman;
    uint64_t    olay_id;
    int32_t     hata_kodu;
    void       *kullanici_verisi;
} OlayVeri;

typedef void (*OlayDinleyiciFonk)(const OlayVeri *olay, void *baglan);

typedef struct {
    OlayTipi            hedef_tip;
    OlayDinleyiciFonk   fonk;
    void               *baglan;
    bool                aktif;
    uint64_t            cagri_sayisi;
} OlayDinleyici;

typedef struct {
    OlayDinleyici   dinleyiciler[OT_MAKS_OLAY_DINLEYICI];
    size_t          dinleyici_sayisi;
    OlayVeri        gecmis[256];
    size_t          gecmis_bas;
    size_t          gecmis_boyutu;
    uint64_t        toplam_olay;
} OlaySistemi;

typedef OtHataKodu (*ZamanLayiciFonk)(void *baglan);

typedef struct {
    ZamanLayiciTipi tip;
    ZamanLayiciFonk fonk;
    void           *baglan;
    uint32_t        aralik_ms;
    time_t          son_atis;
    time_t          ilk_atis;
    uint64_t        atis_sayisi;
    uint32_t        max_atis;
    bool            aktif;
    char            isim[64];
    uint32_t        id;
} Zamanlayici;

typedef struct {
    Zamanlayici     liste[OT_MAKS_ZAMANLAYICI];
    size_t          boyut;
    uint32_t        siradaki_id;
} ZamanLayiciYoneticisi;

typedef OtHataKodu (*BoruAsamaFonk)(BasitDeger *giris, BasitDeger *cikis,
                                    void *baglan);

typedef struct {
    char            isim[64];
    BoruAsamaFonk   fonk;
    void           *baglan;
    AsamaDurumu     durum;
    uint64_t        islem_suresi_us;
    bool            atlama_aktif;
    bool            hata_toleransi;
} BoruAsamasi;

typedef struct {
    BoruAsamasi     asamalar[OT_MAKS_BORU_ASAMASI];
    size_t          asama_sayisi;
    char            isim[64];
    bool            paralel_mi;
    uint64_t        tamamlanan;
    uint64_t        basarisiz;
} BoruHatti;

typedef struct {
    char    anahtar[64];
    char    deger[OT_MAKS_DEGER_UZUNLUGU];
    bool    salt_okunur;
    time_t  son_guncelleme;
} OrtamDegiskeni;

typedef struct {
    OrtamDegiskeni  degiskenler[OT_MAKS_ENV_DEGISKEN];
    size_t          boyut;
} OrtamTablo;

typedef struct {
    GuvenlikSeviyesi    seviye;
    bool                komut_beyaz_liste_aktif;
    char                beyaz_liste[OT_MAKS_FILTRE_KURAL][64];
    size_t              beyaz_liste_boyutu;
    char                kara_liste[OT_MAKS_FILTRE_KURAL][64];
    size_t              kara_liste_boyutu;
    uint32_t            max_komut_saniyede;
    uint32_t            mevcut_komut_sayisi;
    time_t              pencere_basi;
    uint64_t            engellenen_komut;
    bool                denetim_aktif;
} GuvenlikKatmani;

typedef struct {
    uint64_t    toplam_komut;
    uint64_t    basarili_komut;
    uint64_t    basarisiz_komut;
    uint64_t    iptal_komut;
    uint64_t    zaman_asimi_komut;
    double      ortalama_islem_suresi_ms;
    double      en_uzun_islem_ms;
    double      en_kisa_islem_ms;
    uint64_t    toplam_olay;
    uint64_t    hata_sayisi;
    uint64_t    uyari_sayisi;
    time_t      baslangic;
    time_t      son_guncelleme;
    double      calisma_suresi_sn;
    uint64_t    kuyruk_dolu_sayisi;
    uint64_t    zamanlayici_atis;
    uint64_t    boru_hatti_tamamlanan;
} IstatistikMotoru;

typedef struct {
    char    isim[64];
    char    surum[32];
    char    aciklama[OT_MAKS_DEGER_UZUNLUGU];
    bool    aktif;
    void   *baglan;
    OtHataKodu (*baslat)(void *baglan);
    OtHataKodu (*durdur)(void *baglan);
    OtHataKodu (*komut_isle)(void *baglan, const Komut *k);
} Eklenti;

typedef struct {
    Eklenti     liste[OT_MAKS_EKLENTI];
    size_t      boyut;
} EklentiYoneticisi;

typedef struct {
    char    anahtar[64];
    char    deger[OT_MAKS_DEGER_UZUNLUGU];
} MetaVeriGirisi;

typedef struct {
    MetaVeriGirisi  girdi[OT_MAKS_METAVERI];
    size_t          boyut;
} MetaVeri;

typedef struct {
    uint32_t            zaman_asimi_ms;
    uint32_t            tekrar_araligi_ms;
    bool                hata_ayiklama;
    bool                hata_durumunda_dur;
    FILE               *log_akisi;
    GunlukSeviyesi      log_seviyesi;
    bool                istatistik_aktif;
    bool                olay_sistemi_aktif;
    bool                guvenlik_aktif;
    GuvenlikSeviyesi    guvenlik_seviyesi;
    bool                paralel_boru;
    uint32_t            max_tekrar;
    bool                renkli_log;
} OtomasyonAyarlari;

typedef struct {
    OtomasyonAyarlari   ayarlar;
    KomutKuyrugu        kuyruk;
    OncelikliKuyruk     oncelikli_kuyruk;
    OlaySistemi         olay;
    ZamanLayiciYoneticisi zamanlayici;
    IstatistikMotoru    istatistik;
    GuvenlikKatmani     guvenlik;
    EklentiYoneticisi   eklentiler;
    OrtamTablo          ortam;
    MetaVeri            meta;
    GunlukAyarlari      gunluk;
    bool                calisyor;
    bool                duraklatildi;
    time_t              baslama_zamani;
    uint64_t            calistirilan_komut;
    uint64_t            basarili_komut;
    uint64_t            basarisiz_komut;
    uint64_t            siradaki_komut_id;
    char                surum[32];
    char                motor_id[64];
} OtomasyonMotoru;

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 4 — DAHİLİ YARDIMCI FONKSİYONLAR
 * ═══════════════════════════════════════════════════════════════════════════*/

/* Büyük/küçük harf duyarsız string karşılaştırma */
static int ot_str_ieq(const char *a, const char *b)
{
    if (!a || !b)
        return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

/* Prefix kontrolü */
static int ot_str_baslangic(const char *s, const char *prefix)
{
    if (!s || !prefix)
        return 0;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

/* Güvenli string kopyalama */
static void ot_str_kopyala(char *hedef, const char *kaynak, size_t boyut)
{
    if (!hedef || !kaynak || boyut == 0)
        return;
    strncpy(hedef, kaynak, boyut - 1u);
    hedef[boyut - 1u] = '\0';
}

/* Baştaki/sondaki boşlukları kırp */
static void ot_str_kirp(char *s)
{
    char *bas, *son;
    size_t uzunluk;
    if (!s || *s == '\0')
        return;
    bas = s;
    while (isspace((unsigned char)*bas))
        bas++;
    uzunluk = strlen(bas);
    if (uzunluk == 0) {
        *s = '\0';
        return;
    }
    son = bas + uzunluk - 1u;
    while (son > bas && isspace((unsigned char)*son))
        son--;
    *(son + 1) = '\0';
    if (bas != s)
        memmove(s, bas, (size_t)(son - bas + 2u));
}

/* Boşluk atlama */
static const char *ot_atla_bosluk(const char *s)
{
    if (!s)
        return s;
    while (*s && isspace((unsigned char)*s))
        s++;
    return s;
}

/* Sayıyı min-max aralığına kısıtla */
static double ot_kisit_double(double deger, double min, double max)
{
    if (deger < min) return min;
    if (deger > max) return max;
    return deger;
}

/* EBOB (Öklid algoritması) */
static int32_t ot_ebob(int32_t a, int32_t b)
{
    int32_t t;
    a = (a < 0) ? -a : a;
    b = (b < 0) ? -b : b;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return (a == 0) ? 1 : a;
}

/* Basit hash (djb2) */
static uint32_t ot_hash(const char *s)
{
    uint32_t h = 5381u;
    int c;
    while ((c = (unsigned char)*s++) != 0)
        h = ((h << 5u) + h) + (uint32_t)c;
    return h;
}

/* Zaman damgası oluştur */
static void ot_zaman_damgasi(char *tampon, size_t boyut)
{
    time_t simdi = time(NULL);
    struct tm *tm_bilgi;
    if (!tampon || boyut < 20u)
        return;
#if defined(OT_WINDOWS)
    {
        struct tm yerel;
        localtime_s(&yerel, &simdi);
        tm_bilgi = &yerel;
        strftime(tampon, boyut, "%Y-%m-%d %H:%M:%S", tm_bilgi);
    }
#else
    {
        struct tm yerel;
        localtime_r(&simdi, &yerel);
        tm_bilgi = &yerel;
        strftime(tampon, boyut, "%Y-%m-%d %H:%M:%S", tm_bilgi);
    }
#endif
}

/* Milisaniye cinsinden geçen süre */
static double ot_sure_ms(time_t baslangic)
{
    return difftime(time(NULL), baslangic) * 1000.0;
}

/* Çift sayı güvenli karşılaştırma */
static bool ot_double_esit(double a, double b, double epsilon)
{
    double fark = a - b;
    return (fark < epsilon) && (fark > -epsilon);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 5 — GÜNLÜK SİSTEMİ
 * ═══════════════════════════════════════════════════════════════════════════*/

static const char *gunluk_seviye_metin(GunlukSeviyesi s)
{
    switch (s) {
    case GUNLUK_HATA:    return "HATA ";
    case GUNLUK_UYARI:   return "UYARI";
    case GUNLUK_BILGI:   return "BİLGİ";
    case GUNLUK_HATA_AY: return "HATA_AY";
    case GUNLUK_IZLEME:  return "İZLEME";
    default:             return "?????";
    }
}

static const char *gunluk_renk_kodu(GunlukSeviyesi s)
{
    switch (s) {
    case GUNLUK_HATA:    return "\033[31m";  /* Kırmızı */
    case GUNLUK_UYARI:   return "\033[33m";  /* Sarı    */
    case GUNLUK_BILGI:   return "\033[32m";  /* Yeşil   */
    case GUNLUK_HATA_AY: return "\033[36m";  /* Cyan    */
    case GUNLUK_IZLEME:  return "\033[35m";  /* Mor     */
    default:             return "\033[0m";
    }
}

static void gunluk_yaz(
    const GunlukAyarlari *ayarlar,
    GunlukSeviyesi seviye,
    const char *kaynak,
    const char *fmt,
    va_list ap)
{
    char tampon[OT_MAKS_LOG_SATIRI];
    char zaman_tampon[32];
    FILE *cikis;

    if (!ayarlar || seviye > ayarlar->seviye)
        return;

    cikis = ayarlar->akis ? ayarlar->akis : stderr;

    if (ayarlar->zaman_damgali)
        ot_zaman_damgasi(zaman_tampon, sizeof(zaman_tampon));
    else
        zaman_tampon[0] = '\0';

    vsnprintf(tampon, sizeof(tampon) - 1u, fmt, ap);
    tampon[sizeof(tampon) - 1u] = '\0';

    if (ayarlar->renkli) {
        fprintf(cikis, "%s[%s][%s][%s] %s\033[0m\n",
                gunluk_renk_kodu(seviye),
                zaman_tampon,
                gunluk_seviye_metin(seviye),
                kaynak ? kaynak : "motor",
                tampon);
    } else {
        fprintf(cikis, "[%s][%s][%s] %s\n",
                zaman_tampon,
                gunluk_seviye_metin(seviye),
                kaynak ? kaynak : "motor",
                tampon);
    }
    fflush(cikis);
}

static void ot_gunluk(
    const OtomasyonMotoru *mot,
    GunlukSeviyesi seviye,
    const char *kaynak,
    const char *fmt, ...)
{
    va_list ap;
    if (!mot)
        return;
    va_start(ap, fmt);
    gunluk_yaz(&mot->gunluk, seviye, kaynak, fmt, ap);
    va_end(ap);
}

/* Eski API uyumluluğu */
void ot_log(const OtomasyonMotoru *mot, const char *seviye,
            const char *fmt, ...)
{
    va_list ap;
    FILE *cikis = stderr;
    if (mot && mot->ayarlar.log_akisi)
        cikis = mot->ayarlar.log_akisi;
    fprintf(cikis, "[%s] ", seviye ? seviye : "?");
    va_start(ap, fmt);
    vfprintf(cikis, fmt, ap);
    va_end(ap);
    fputc('\n', cikis);
    fflush(cikis);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 6 — BasitDeger İŞLEMLERİ
 * ═══════════════════════════════════════════════════════════════════════════*/

void basit_deger_baslat(BasitDeger *bd)
{
    if (!bd)
        return;
    memset(bd, 0, sizeof(*bd));
}

void basit_deger_ata(BasitDeger *bd, const char *deger)
{
    if (!bd)
        return;
    basit_deger_baslat(bd);
    if (deger) {
        ot_str_kopyala(bd->deger, deger, OT_MAKS_DEGER_UZUNLUGU);
        ot_str_kopyala(bd->metin, deger, OT_MAKS_DEGER_UZUNLUGU);
        ot_str_kopyala(bd->ham,   deger, OT_MAKS_DEGER_UZUNLUGU);
    }
}

void basit_deger_ata_ham(BasitDeger *bd, const char *deger, const char *metin,
                         const char *ham)
{
    if (!bd)
        return;
    basit_deger_baslat(bd);
    if (deger)
        ot_str_kopyala(bd->deger, deger, OT_MAKS_DEGER_UZUNLUGU);
    if (metin)
        ot_str_kopyala(bd->metin, metin, OT_MAKS_DEGER_UZUNLUGU);
    if (ham)
        ot_str_kopyala(bd->ham,   ham,   OT_MAKS_DEGER_UZUNLUGU);
}

OtHataKodu basit_deger_dugum_ekle(BasitDeger *bd, DegerDugumu dugum)
{
    OT_NULL_KONTROL(bd);
    if (bd->dugum_sayisi >= OT_MAKS_DUGUM_SAYISI)
        return OT_HATA_KAPASITE;
    bd->dugumler[bd->dugum_sayisi++] = dugum;
    return OT_BASARILI;
}

OtHataKodu basit_deger_dugum_sil(BasitDeger *bd, size_t indeks)
{
    OT_NULL_KONTROL(bd);
    if (indeks >= bd->dugum_sayisi)
        return OT_HATA_GECERSIZ_ARG;
    memmove(&bd->dugumler[indeks],
            &bd->dugumler[indeks + 1u],
            (bd->dugum_sayisi - indeks - 1u) * sizeof(DegerDugumu));
    bd->dugum_sayisi--;
    return OT_BASARILI;
}

OtHataKodu basit_deger_dugum_al(const BasitDeger *bd, size_t indeks,
                                DegerDugumu *hedef)
{
    OT_NULL_KONTROL(bd);
    OT_NULL_KONTROL(hedef);
    if (indeks >= bd->dugum_sayisi)
        return OT_HATA_GECERSIZ_ARG;
    *hedef = bd->dugumler[indeks];
    return OT_BASARILI;
}

bool basit_deger_esit_mi(const BasitDeger *a, const BasitDeger *b)
{
    if (!a || !b)
        return false;
    return strcmp(a->deger, b->deger) == 0 &&
           a->dugum_sayisi == b->dugum_sayisi;
}

void basit_deger_isle(const BasitDeger *bd)
{
    size_t i;
    if (!bd)
        return;
    fprintf(stdout, "[BasitDeger] deger=%s metin=%s\n", bd->deger, bd->metin);
    for (i = 0; i < bd->dugum_sayisi; i++) {
        fprintf(stdout, "  dugum[%zu] tip=%d deger=%s\n",
                i, (int)bd->dugumler[i].tip, bd->dugumler[i].deger);
    }
}

OtHataKodu basit_deger_json_yaz(const BasitDeger *bd, FILE *akis)
{
    size_t i;
    OT_NULL_KONTROL(bd);
    if (!akis)
        akis = stdout;
    fprintf(akis, "{\n");
    fprintf(akis, "  \"deger\": \"%s\",\n", bd->deger);
    fprintf(akis, "  \"metin\": \"%s\",\n", bd->metin);
    fprintf(akis, "  \"ham\": \"%s\",\n",   bd->ham);
    fprintf(akis, "  \"dugumler\": [\n");
    for (i = 0; i < bd->dugum_sayisi; i++) {
        fprintf(akis, "    {\"tip\": %d, \"deger\": \"%s\"}%s\n",
                (int)bd->dugumler[i].tip,
                bd->dugumler[i].deger,
                (i + 1u < bd->dugum_sayisi) ? "," : "");
    }
    fprintf(akis, "  ]\n}\n");
    return OT_BASARILI;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 7 — KESİR İŞLEMLERİ
 * ═══════════════════════════════════════════════════════════════════════════*/

void kesir_baslat(KesirDeger *kd, int32_t pay, int32_t payda)
{
    if (!kd)
        return;
    memset(kd, 0, sizeof(*kd));
    kd->pay   = pay;
    kd->payda = payda;
    kd->ondalik = (payda != 0) ? (double)pay / (double)payda : 0.0;
    kd->indirgenmi = false;
}

OtHataKodu kesir_indirge(KesirDeger *kd)
{
    int32_t ebob;
    OT_NULL_KONTROL(kd);
    if (kd->payda == 0)
        return OT_HATA_GECERSIZ_ARG;
    ebob = ot_ebob(kd->pay, kd->payda);
    kd->pay    /= ebob;
    kd->payda  /= ebob;
    if (kd->payda < 0) {
        kd->pay   = -kd->pay;
        kd->payda = -kd->payda;
    }
    kd->ondalik    = (double)kd->pay / (double)kd->payda;
    kd->indirgenmi = true;
    return OT_BASARILI;
}

OtHataKodu kesir_topla(const KesirDeger *a, const KesirDeger *b, KesirDeger *s)
{
    int32_t yeni_payda;
    int32_t yeni_pay;
    OT_NULL_KONTROL(a);
    OT_NULL_KONTROL(b);
    OT_NULL_KONTROL(s);
    if (a->payda == 0 || b->payda == 0)
        return OT_HATA_GECERSIZ_ARG;
    yeni_payda = a->payda * b->payda;
    yeni_pay   = a->pay * b->payda + b->pay * a->payda;
    kesir_baslat(s, yeni_pay, yeni_payda);
    return kesir_indirge(s);
}

OtHataKodu kesir_cikar(const KesirDeger *a, const KesirDeger *b, KesirDeger *s)
{
    int32_t yeni_payda;
    int32_t yeni_pay;
    OT_NULL_KONTROL(a);
    OT_NULL_KONTROL(b);
    OT_NULL_KONTROL(s);
    if (a->payda == 0 || b->payda == 0)
        return OT_HATA_GECERSIZ_ARG;
    yeni_payda = a->payda * b->payda;
    yeni_pay   = a->pay * b->payda - b->pay * a->payda;
    kesir_baslat(s, yeni_pay, yeni_payda);
    return kesir_indirge(s);
}

OtHataKodu kesir_carp(const KesirDeger *a, const KesirDeger *b, KesirDeger *s)
{
    OT_NULL_KONTROL(a);
    OT_NULL_KONTROL(b);
    OT_NULL_KONTROL(s);
    if (a->payda == 0 || b->payda == 0)
        return OT_HATA_GECERSIZ_ARG;
    kesir_baslat(s, a->pay * b->pay, a->payda * b->payda);
    return kesir_indirge(s);
}

OtHataKodu kesir_bol(const KesirDeger *a, const KesirDeger *b, KesirDeger *s)
{
    OT_NULL_KONTROL(a);
    OT_NULL_KONTROL(b);
    OT_NULL_KONTROL(s);
    if (b->pay == 0)
        return OT_HATA_GECERSIZ_ARG;
    if (a->payda == 0 || b->payda == 0)
        return OT_HATA_GECERSIZ_ARG;
    kesir_baslat(s, a->pay * b->payda, a->payda * b->pay);
    return kesir_indirge(s);
}

int kesir_karsilastir(const KesirDeger *a, const KesirDeger *b)
{
    double da, db;
    if (!a || !b)
        return 0;
    da = a->ondalik;
    db = b->ondalik;
    if (ot_double_esit(da, db, 1e-12))
        return 0;
    return (da < db) ? -1 : 1;
}

OtHataKodu kesir_ata_metin(KesirDeger *kd, const char *metin)
{
    char buf[OT_MAKS_DEGER_UZUNLUGU];
    const char *p;
    char *end;
    long lp, lq;
    int32_t pay = 0, payda = 1;

    OT_NULL_KONTROL(kd);
    OT_NULL_KONTROL(metin);

    ot_str_kopyala(buf, metin, sizeof(buf));
    p = ot_atla_bosluk(buf);
    if (*p == '\0')
        return OT_HATA_GECERSIZ_ARG;

    lp = strtol(p, &end, 10);
    if (end == p)
        return OT_HATA_GECERSIZ_ARG;
    pay = (int32_t)lp;
    p   = ot_atla_bosluk(end);

    if (*p == '/') {
        p++;
        p = ot_atla_bosluk(p);
        lq = strtol(p, &end, 10);
        if (p == end)
            return OT_HATA_GECERSIZ_ARG;
        payda = (int32_t)lq;
    }
    kesir_baslat(kd, pay, payda);
    basit_deger_ata(&kd->temel, metin);
    return OT_BASARILI;
}

double kesir_ondalik_al(const KesirDeger *kd)
{
    return kd ? kd->ondalik : 0.0;
}

OtHataKodu kesir_metin_al(const KesirDeger *kd, char *tampon, size_t boyut)
{
    OT_NULL_KONTROL(kd);
    OT_NULL_KONTROL(tampon);
    if (boyut < 16u)
        return OT_HATA_KAPASITE;
    snprintf(tampon, boyut, "%d/%d", (int)kd->pay, (int)kd->payda);
    return OT_BASARILI;
}

void kesir_yazdir(const KesirDeger *kd, FILE *akis)
{
    if (!kd)
        return;
    if (!akis)
        akis = stdout;
    fprintf(akis, "%d/%d = %.6f\n",
            (int)kd->pay, (int)kd->payda, kd->ondalik);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 8 — KOMUT ÇÖZÜMLEYİCİ
 * ═══════════════════════════════════════════════════════════════════════════*/

/* Komut eşleme tablosu */
typedef struct {
    const char  *metin;
    const char  *takma;
    KomutTipi   tip;
} KomutEsleme;

static const KomutEsleme KOMUT_TABLOSU[] = {
    { "print",    "yazdir",   KOMUT_YAZDIR   },
    { "exit",     "cikis",    KOMUT_DUR      },
    { "quit",     "bitir",    KOMUT_DUR      },
    { "start",    "basla",    KOMUT_CALISTIR },
    { "run",      "calistir", KOMUT_CALISTIR },
    { "stop",     "dur",      KOMUT_DURAKLA  },
    { "pause",    "durakla",  KOMUT_DURAKLA  },
    { "resume",   "devam",    KOMUT_DEVAM    },
    { "continue", "surdur",   KOMUT_DEVAM    },
    { "load",     "yukle",    KOMUT_YUKLE    },
    { "save",     "kaydet",   KOMUT_KAYDET   },
    { "clear",    "temizle",  KOMUT_TEMIZLE  },
    { "list",     "listele",  KOMUT_LISTELE  },
    { "filter",   "filtrele", KOMUT_FILTRELE },
    { "sort",     "sirala",   KOMUT_SIRALA   },
    { "export",   "aktar",    KOMUT_AKTAR    },
    { "validate", "dogrula",  KOMUT_DOGRULA  },
    { "reset",    "sifirla",  KOMUT_SIFIRLA  },
    { "status",   "durum",    KOMUT_DURUM    },
    { "help",     "yardim",   KOMUT_YARDIM   },
    { "version",  "surum",    KOMUT_SURUM    },
    { "history",  "gecmis",   KOMUT_GECMIS   },
    { "wait",     "bekle",    KOMUT_BEKLE    },
    { "repeat",   "tekrar",   KOMUT_TEKRAR   },
    { "if",       "eger",     KOMUT_KOSULLU  },
    { "loop",     "dongu",    KOMUT_DONGU    },
    { "break",    "kesinti",  KOMUT_KESINTI  },
    { "skip",     "atla",     KOMUT_ATLA     },
    { NULL,       NULL,       KOMUT_BILINMIYOR }
};

KomutTipi komut_cozumle(const char *metin)
{
    const KomutEsleme *e;
    if (!metin || metin[0] == '\0')
        return KOMUT_BILINMIYOR;
    for (e = KOMUT_TABLOSU; e->metin != NULL; e++) {
        if (ot_str_ieq(metin, e->metin) || ot_str_ieq(metin, e->takma))
            return e->tip;
    }
    return KOMUT_BILINMIYOR;
}

const char *komut_tip_metin(KomutTipi tip)
{
    const KomutEsleme *e;
    for (e = KOMUT_TABLOSU; e->metin != NULL; e++) {
        if (e->tip == tip)
            return e->metin;
    }
    return "bilinmiyor";
}

OtHataKodu komut_olustur(Komut *k, const char *metin, const char *arguman,
                          uint32_t oncelik)
{
    KomutTipi tip;
    OT_NULL_KONTROL(k);
    OT_NULL_KONTROL(metin);
    memset(k, 0, sizeof(*k));
    tip = komut_cozumle(metin);
    if (tip == KOMUT_BILINMIYOR)
        return OT_HATA_GECERSIZ_ARG;
    k->tip      = tip;
    k->oncelik  = oncelik;
    k->aktif    = true;
    k->olusturma_zamani = time(NULL);
    ot_str_kopyala(k->isim,    metin,   OT_MAKS_KOMUT_UZUNLUGU);
    if (arguman)
        ot_str_kopyala(k->arguman, arguman, OT_MAKS_KOMUT_UZUNLUGU);
    return OT_BASARILI;
}

void komut_yazdir(const Komut *k, FILE *akis)
{
    if (!k)
        return;
    if (!akis)
        akis = stdout;
    fprintf(akis, "Komut[id=%llu tip=%s oncelik=%u aktif=%s arguman='%s']\n",
            (unsigned long long)k->komut_id,
            komut_tip_metin(k->tip),
            k->oncelik,
            k->aktif ? "evet" : "hayir",
            k->arguman);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 9 — KOMUT KUYRUĞU
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu komut_kuyrugu_baslat(KomutKuyrugu *kq)
{
    OT_NULL_KONTROL(kq);
    memset(kq, 0, sizeof(*kq));
    return OT_BASARILI;
}

OtHataKodu komut_kuyrugu_ekle(KomutKuyrugu *kq, const Komut *komut)
{
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(komut);
    if (kq->boyut >= OT_MAKS_KUYRUK_BOYUTU)
        return OT_HATA_KAPASITE;
    kq->ogeler[kq->son] = *komut;
    kq->son = (kq->son + 1u) % OT_MAKS_KUYRUK_BOYUTU;
    kq->boyut++;
    kq->toplam_eklenen++;
    return OT_BASARILI;
}

OtHataKodu komut_kuyrugu_al(KomutKuyrugu *kq, Komut *hedef)
{
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(hedef);
    if (kq->boyut == 0u)
        return OT_HATA_GENEL;
    *hedef = kq->ogeler[kq->bas];
    memset(&kq->ogeler[kq->bas], 0, sizeof(Komut));
    kq->bas = (kq->bas + 1u) % OT_MAKS_KUYRUK_BOYUTU;
    kq->boyut--;
    kq->toplam_alinan++;
    return OT_BASARILI;
}

OtHataKodu komut_kuyrugu_one_bak(const KomutKuyrugu *kq, Komut *hedef)
{
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(hedef);
    if (kq->boyut == 0u)
        return OT_HATA_GENEL;
    *hedef = kq->ogeler[kq->bas];
    return OT_BASARILI;
}

OtHataKodu komut_kuyrugu_temizle(KomutKuyrugu *kq)
{
    OT_NULL_KONTROL(kq);
    memset(kq, 0, sizeof(*kq));
    return OT_BASARILI;
}

bool komut_kuyrugu_bos_mu(const KomutKuyrugu *kq)
{
    return (!kq || kq->boyut == 0u);
}

bool komut_kuyrugu_dolu_mu(const KomutKuyrugu *kq)
{
    return (kq && kq->boyut >= OT_MAKS_KUYRUK_BOYUTU);
}

size_t komut_kuyrugu_boyut(const KomutKuyrugu *kq)
{
    return kq ? kq->boyut : 0u;
}

void komut_kuyrugu_yazdir(const KomutKuyrugu *kq, FILE *akis)
{
    size_t i, indeks;
    if (!kq || !akis)
        return;
    fprintf(akis, "=== KomutKuyrugu boyut=%zu ===\n", kq->boyut);
    for (i = 0; i < kq->boyut; i++) {
        indeks = (kq->bas + i) % OT_MAKS_KUYRUK_BOYUTU;
        komut_yazdir(&kq->ogeler[indeks], akis);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 10 — ÖNCELİKLİ KUYRUK (MIN-HEAP)
 * ═══════════════════════════════════════════════════════════════════════════*/

static void oncelikli_yukari_yuksel(OncelikliKuyruk *kq, size_t i)
{
    OncelikliKomut tmp;
    size_t ust;
    while (i > 0) {
        ust = (i - 1u) / 2u;
        if (kq->ogeler[i].efektif_oncelik < kq->ogeler[ust].efektif_oncelik) {
            tmp            = kq->ogeler[i];
            kq->ogeler[i]  = kq->ogeler[ust];
            kq->ogeler[ust]= tmp;
            i              = ust;
        } else {
            break;
        }
    }
}

static void oncelikli_asagi_in(OncelikliKuyruk *kq, size_t i)
{
    size_t sol, sag, en_kucuk;
    OncelikliKomut tmp;
    for (;;) {
        sol       = 2u * i + 1u;
        sag       = 2u * i + 2u;
        en_kucuk  = i;
        if (sol < kq->boyut &&
            kq->ogeler[sol].efektif_oncelik < kq->ogeler[en_kucuk].efektif_oncelik)
            en_kucuk = sol;
        if (sag < kq->boyut &&
            kq->ogeler[sag].efektif_oncelik < kq->ogeler[en_kucuk].efektif_oncelik)
            en_kucuk = sag;
        if (en_kucuk == i)
            break;
        tmp              = kq->ogeler[i];
        kq->ogeler[i]    = kq->ogeler[en_kucuk];
        kq->ogeler[en_kucuk] = tmp;
        i                = en_kucuk;
    }
}

OtHataKodu oncelikli_kuyruk_baslat(OncelikliKuyruk *kq)
{
    OT_NULL_KONTROL(kq);
    memset(kq, 0, sizeof(*kq));
    return OT_BASARILI;
}

OtHataKodu oncelikli_kuyruk_ekle(OncelikliKuyruk *kq, const Komut *komut)
{
    OncelikliKomut yeni;
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(komut);
    if (kq->boyut >= OT_MAKS_KUYRUK_BOYUTU)
        return OT_HATA_KAPASITE;
    memset(&yeni, 0, sizeof(yeni));
    yeni.komut             = *komut;
    yeni.efektif_oncelik   = komut->oncelik;
    yeni.ekleme_zamani     = time(NULL);
    kq->ogeler[kq->boyut]  = yeni;
    oncelikli_yukari_yuksel(kq, kq->boyut);
    kq->boyut++;
    kq->islem_sayisi++;
    return OT_BASARILI;
}

OtHataKodu oncelikli_kuyruk_al(OncelikliKuyruk *kq, Komut *hedef)
{
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(hedef);
    if (kq->boyut == 0u)
        return OT_HATA_GENEL;
    *hedef = kq->ogeler[0].komut;
    kq->boyut--;
    if (kq->boyut > 0u) {
        kq->ogeler[0] = kq->ogeler[kq->boyut];
        oncelikli_asagi_in(kq, 0);
    }
    kq->islem_sayisi++;
    return OT_BASARILI;
}

bool oncelikli_kuyruk_bos_mu(const OncelikliKuyruk *kq)
{
    return (!kq || kq->boyut == 0u);
}

size_t oncelikli_kuyruk_boyut(const OncelikliKuyruk *kq)
{
    return kq ? kq->boyut : 0u;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 11 — OLAY SİSTEMİ
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu olay_sistemi_baslat(OlaySistemi *os)
{
    OT_NULL_KONTROL(os);
    memset(os, 0, sizeof(*os));
    return OT_BASARILI;
}

OtHataKodu olay_dinleyici_ekle(OlaySistemi *os, OlayTipi tip,
                               OlayDinleyiciFonk fonk, void *baglan)
{
    OlayDinleyici *d;
    size_t i;
    OT_NULL_KONTROL(os);
    OT_NULL_KONTROL(fonk);

    /* Boş slot bul */
    for (i = 0; i < OT_MAKS_OLAY_DINLEYICI; i++) {
        if (!os->dinleyiciler[i].aktif) {
            d              = &os->dinleyiciler[i];
            d->hedef_tip   = tip;
            d->fonk        = fonk;
            d->baglan      = baglan;
            d->aktif       = true;
            d->cagri_sayisi = 0u;
            if (i >= os->dinleyici_sayisi)
                os->dinleyici_sayisi = i + 1u;
            return OT_BASARILI;
        }
    }
    return OT_HATA_KAPASITE;
}

OtHataKodu olay_dinleyici_kaldir(OlaySistemi *os, OlayDinleyiciFonk fonk)
{
    size_t i;
    OT_NULL_KONTROL(os);
    OT_NULL_KONTROL(fonk);
    for (i = 0; i < os->dinleyici_sayisi; i++) {
        if (os->dinleyiciler[i].fonk == fonk) {
            os->dinleyiciler[i].aktif = false;
            return OT_BASARILI;
        }
    }
    return OT_HATA_BULUNAMADI;
}

OtHataKodu olay_yayinla(OlaySistemi *os, OlayTipi tip, const char *kaynak,
                        const char *mesaj, int32_t hata_kodu, void *kullanici)
{
    OlayVeri veri;
    size_t   i;
    OT_NULL_KONTROL(os);

    memset(&veri, 0, sizeof(veri));
    veri.tip           = tip;
    veri.hata_kodu     = hata_kodu;
    veri.kullanici_verisi = kullanici;
    veri.zaman         = time(NULL);
    veri.olay_id       = ++os->toplam_olay;

    if (kaynak)
        ot_str_kopyala(veri.kaynak, kaynak, sizeof(veri.kaynak));
    if (mesaj)
        ot_str_kopyala(veri.mesaj,  mesaj,  sizeof(veri.mesaj));

    /* Geçmişe ekle (dairesel tampon) */
    os->gecmis[os->gecmis_bas] = veri;
    os->gecmis_bas = (os->gecmis_bas + 1u) % OT_DIZI_BOYUTU(os->gecmis);
    if (os->gecmis_boyutu < OT_DIZI_BOYUTU(os->gecmis))
        os->gecmis_boyutu++;

    /* Dinleyicileri çağır */
    for (i = 0; i < os->dinleyici_sayisi; i++) {
        OlayDinleyici *d = &os->dinleyiciler[i];
        if (d->aktif && (d->hedef_tip == OLAY_BILINMIYOR || d->hedef_tip == tip)) {
            d->fonk(&veri, d->baglan);
            d->cagri_sayisi++;
        }
    }
    return OT_BASARILI;
}

void olay_gecmis_yazdir(const OlaySistemi *os, FILE *akis, size_t max_satir)
{
    size_t i, gosterilen;
    char tampon[32];
    struct tm *tm_bilgi;
    if (!os || !akis)
        return;
    gosterilen = OT_MIN(max_satir, os->gecmis_boyutu);
    fprintf(akis, "=== Olay Geçmişi (%zu kayıt) ===\n", gosterilen);
    for (i = 0; i < gosterilen; i++) {
        size_t indeks = (os->gecmis_bas + OT_DIZI_BOYUTU(os->gecmis) -
                        os->gecmis_boyutu + i) % OT_DIZI_BOYUTU(os->gecmis);
        const OlayVeri *v = &os->gecmis[indeks];
        tm_bilgi = localtime(&v->zaman);
        strftime(tampon, sizeof(tampon), "%H:%M:%S", tm_bilgi);
        fprintf(akis, "[%s] #%llu tip=%d kaynak=%s msg=%s\n",
                tampon,
                (unsigned long long)v->olay_id,
                (int)v->tip,
                v->kaynak,
                v->mesaj);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 12 — ZAMANLAYICI YÖNETİCİSİ
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu zamanlayici_yoneticisi_baslat(ZamanLayiciYoneticisi *zy)
{
    OT_NULL_KONTROL(zy);
    memset(zy, 0, sizeof(*zy));
    zy->siradaki_id = 1u;
    return OT_BASARILI;
}

OtHataKodu zamanlayici_ekle(ZamanLayiciYoneticisi *zy,
                            const char *isim,
                            ZamanLayiciTipi tip,
                            uint32_t aralik_ms,
                            ZamanLayiciFonk fonk,
                            void *baglan,
                            uint32_t *cikis_id)
{
    Zamanlayici *z;
    size_t i;
    OT_NULL_KONTROL(zy);
    OT_NULL_KONTROL(fonk);
    if (zy->boyut >= OT_MAKS_ZAMANLAYICI)
        return OT_HATA_KAPASITE;

    /* Boş slot bul */
    for (i = 0; i < OT_MAKS_ZAMANLAYICI; i++) {
        if (!zy->liste[i].aktif)
            break;
    }
    z              = &zy->liste[i];
    memset(z, 0, sizeof(*z));
    z->tip         = tip;
    z->fonk        = fonk;
    z->baglan      = baglan;
    z->aralik_ms   = aralik_ms;
    z->aktif       = true;
    z->ilk_atis    = time(NULL);
    z->son_atis    = z->ilk_atis;
    z->id          = zy->siradaki_id++;
    if (isim)
        ot_str_kopyala(z->isim, isim, sizeof(z->isim));
    if (i >= zy->boyut)
        zy->boyut = i + 1u;
    if (cikis_id)
        *cikis_id = z->id;
    return OT_BASARILI;
}

OtHataKodu zamanlayici_kaldir(ZamanLayiciYoneticisi *zy, uint32_t id)
{
    size_t i;
    OT_NULL_KONTROL(zy);
    for (i = 0; i < zy->boyut; i++) {
        if (zy->liste[i].id == id) {
            zy->liste[i].aktif = false;
            return OT_BASARILI;
        }
    }
    return OT_HATA_BULUNAMADI;
}

OtHataKodu zamanlayici_guncelle(ZamanLayiciYoneticisi *zy)
{
    size_t i;
    time_t simdi;
    OT_NULL_KONTROL(zy);
    simdi = time(NULL);
    for (i = 0; i < zy->boyut; i++) {
        Zamanlayici *z = &zy->liste[i];
        double gecen_ms;
        if (!z->aktif)
            continue;
        gecen_ms = difftime(simdi, z->son_atis) * 1000.0;
        if (gecen_ms >= (double)z->aralik_ms) {
            OtHataKodu sonuc = z->fonk(z->baglan);
            z->son_atis = simdi;
            z->atis_sayisi++;
            if (z->tip == ZAMANLAYICI_TEK_SEFER ||
                (z->max_atis > 0u && z->atis_sayisi >= z->max_atis))
                z->aktif = false;
            OT_KULLANILMADI(sonuc);
        }
    }
    return OT_BASARILI;
}

void zamanlayici_listele(const ZamanLayiciYoneticisi *zy, FILE *akis)
{
    size_t i;
    if (!zy || !akis)
        return;
    fprintf(akis, "=== Zamanlayıcılar (%zu) ===\n", zy->boyut);
    for (i = 0; i < zy->boyut; i++) {
        const Zamanlayici *z = &zy->liste[i];
        if (!z->aktif)
            continue;
        fprintf(akis, "  [%u] isim=%-20s aralik=%u ms atis=%llu\n",
                z->id, z->isim, z->aralik_ms,
                (unsigned long long)z->atis_sayisi);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 13 — BORU HATTI
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu boru_hatti_baslat(BoruHatti *bh, const char *isim, bool paralel)
{
    OT_NULL_KONTROL(bh);
    memset(bh, 0, sizeof(*bh));
    if (isim)
        ot_str_kopyala(bh->isim, isim, sizeof(bh->isim));
    bh->paralel_mi = paralel;
    return OT_BASARILI;
}

OtHataKodu boru_hatti_asama_ekle(BoruHatti *bh, const char *isim,
                                  BoruAsamaFonk fonk, void *baglan,
                                  bool hata_toleransi)
{
    BoruAsamasi *a;
    OT_NULL_KONTROL(bh);
    OT_NULL_KONTROL(fonk);
    if (bh->asama_sayisi >= OT_MAKS_BORU_ASAMASI)
        return OT_HATA_KAPASITE;
    a = &bh->asamalar[bh->asama_sayisi++];
    memset(a, 0, sizeof(*a));
    if (isim)
        ot_str_kopyala(a->isim, isim, sizeof(a->isim));
    a->fonk           = fonk;
    a->baglan         = baglan;
    a->hata_toleransi = hata_toleransi;
    a->durum          = ASAMA_BEKLIYOR;
    return OT_BASARILI;
}

OtHataKodu boru_hatti_calistir(BoruHatti *bh, BasitDeger *giris)
{
    size_t i;
    BasitDeger ara;
    OtHataKodu sonuc;
    OT_NULL_KONTROL(bh);
    OT_NULL_KONTROL(giris);

    basit_deger_baslat(&ara);
    for (i = 0; i < bh->asama_sayisi; i++) {
        BoruAsamasi *a = &bh->asamalar[i];
        if (!a->fonk) {
            a->durum = ASAMA_ATLANDI;
            continue;
        }
        a->durum = ASAMA_CALISIYOR;
        basit_deger_baslat(&ara);
        sonuc = a->fonk(giris, &ara, a->baglan);
        if (sonuc == OT_BASARILI) {
            a->durum = ASAMA_TAMAMLANDI;
            bh->tamamlanan++;
            *giris = ara;
        } else {
            a->durum = ASAMA_BASARISIZ;
            bh->basarisiz++;
            if (!a->hata_toleransi)
                return sonuc;
        }
    }
    return OT_BASARILI;
}

void boru_hatti_durum_yazdir(const BoruHatti *bh, FILE *akis)
{
    size_t i;
    const char *durum_metin[] = {
        "Bekliyor", "Çalışıyor", "Tamamlandı", "Başarısız", "Atlandı", "İptal"
    };
    if (!bh || !akis)
        return;
    fprintf(akis, "=== BoruHattı '%s' (%zu aşama) ===\n",
            bh->isim, bh->asama_sayisi);
    for (i = 0; i < bh->asama_sayisi; i++) {
        const BoruAsamasi *a = &bh->asamalar[i];
        fprintf(akis, "  [%zu] %-20s → %s\n", i, a->isim,
                (int)a->durum < 6 ? durum_metin[a->durum] : "?");
    }
    fprintf(akis, "  Tamamlanan: %llu  Başarısız: %llu\n",
            (unsigned long long)bh->tamamlanan,
            (unsigned long long)bh->basarisiz);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 14 — ORTAM DEĞİŞKENLERİ TABLOSU
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu ortam_baslat(OrtamTablo *ot)
{
    OT_NULL_KONTROL(ot);
    memset(ot, 0, sizeof(*ot));
    return OT_BASARILI;
}

OtHataKodu ortam_ayarla(OrtamTablo *ot, const char *anahtar, const char *deger,
                        bool salt_okunur)
{
    size_t i;
    OT_NULL_KONTROL(ot);
    OT_NULL_KONTROL(anahtar);

    /* Var mı güncelle */
    for (i = 0; i < ot->boyut; i++) {
        if (strcmp(ot->degiskenler[i].anahtar, anahtar) == 0) {
            if (ot->degiskenler[i].salt_okunur)
                return OT_HATA_IZIN;
            ot_str_kopyala(ot->degiskenler[i].deger,
                           deger ? deger : "",
                           OT_MAKS_DEGER_UZUNLUGU);
            ot->degiskenler[i].son_guncelleme = time(NULL);
            return OT_BASARILI;
        }
    }

    /* Yeni ekle */
    if (ot->boyut >= OT_MAKS_ENV_DEGISKEN)
        return OT_HATA_KAPASITE;

    {
        OrtamDegiskeni *d = &ot->degiskenler[ot->boyut++];
        memset(d, 0, sizeof(*d));
        ot_str_kopyala(d->anahtar, anahtar, sizeof(d->anahtar));
        ot_str_kopyala(d->deger,   deger ? deger : "", OT_MAKS_DEGER_UZUNLUGU);
        d->salt_okunur    = salt_okunur;
        d->son_guncelleme = time(NULL);
    }
    return OT_BASARILI;
}

const char *ortam_al(const OrtamTablo *ot, const char *anahtar,
                     const char *varsayilan)
{
    size_t i;
    if (!ot || !anahtar)
        return varsayilan;
    for (i = 0; i < ot->boyut; i++) {
        if (strcmp(ot->degiskenler[i].anahtar, anahtar) == 0)
            return ot->degiskenler[i].deger;
    }
    return varsayilan;
}

OtHataKodu ortam_sil(OrtamTablo *ot, const char *anahtar)
{
    size_t i;
    OT_NULL_KONTROL(ot);
    OT_NULL_KONTROL(anahtar);
    for (i = 0; i < ot->boyut; i++) {
        if (strcmp(ot->degiskenler[i].anahtar, anahtar) == 0) {
            if (ot->degiskenler[i].salt_okunur)
                return OT_HATA_IZIN;
            memmove(&ot->degiskenler[i], &ot->degiskenler[i + 1u],
                    (ot->boyut - i - 1u) * sizeof(OrtamDegiskeni));
            ot->boyut--;
            return OT_BASARILI;
        }
    }
    return OT_HATA_BULUNAMADI;
}

void ortam_yazdir(const OrtamTablo *ot, FILE *akis)
{
    size_t i;
    if (!ot || !akis)
        return;
    fprintf(akis, "=== Ortam Değişkenleri (%zu) ===\n", ot->boyut);
    for (i = 0; i < ot->boyut; i++) {
        fprintf(akis, "  %s=%s%s\n",
                ot->degiskenler[i].anahtar,
                ot->degiskenler[i].deger,
                ot->degiskenler[i].salt_okunur ? " [RO]" : "");
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 15 — GÜVENLİK KATMANI
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu guvenlik_baslat(GuvenlikKatmani *gk, GuvenlikSeviyesi seviye)
{
    OT_NULL_KONTROL(gk);
    memset(gk, 0, sizeof(*gk));
    gk->seviye              = seviye;
    gk->max_komut_saniyede  = (seviye == GUVENLIK_YUKSEK)  ? 10u
                            : (seviye == GUVENLIK_ORTA)    ? 50u
                            :                               1000u;
    gk->pencere_basi        = time(NULL);
    gk->denetim_aktif       = (seviye >= GUVENLIK_ORTA);
    return OT_BASARILI;
}

OtHataKodu guvenlik_beyaz_liste_ekle(GuvenlikKatmani *gk, const char *komut)
{
    OT_NULL_KONTROL(gk);
    OT_NULL_KONTROL(komut);
    if (gk->beyaz_liste_boyutu >= OT_MAKS_FILTRE_KURAL)
        return OT_HATA_KAPASITE;
    ot_str_kopyala(gk->beyaz_liste[gk->beyaz_liste_boyutu++],
                   komut, 64u);
    gk->komut_beyaz_liste_aktif = true;
    return OT_BASARILI;
}

OtHataKodu guvenlik_kara_liste_ekle(GuvenlikKatmani *gk, const char *komut)
{
    OT_NULL_KONTROL(gk);
    OT_NULL_KONTROL(komut);
    if (gk->kara_liste_boyutu >= OT_MAKS_FILTRE_KURAL)
        return OT_HATA_KAPASITE;
    ot_str_kopyala(gk->kara_liste[gk->kara_liste_boyutu++],
                   komut, 64u);
    return OT_BASARILI;
}

OtHataKodu guvenlik_komut_kontrol(GuvenlikKatmani *gk, const Komut *k)
{
    size_t i;
    time_t simdi;
    double pencere_ms;
    OT_NULL_KONTROL(gk);
    OT_NULL_KONTROL(k);

    if (gk->seviye == GUVENLIK_KAPALTI)
        return OT_BASARILI;

    /* Kara liste kontrolü */
    for (i = 0; i < gk->kara_liste_boyutu; i++) {
        if (ot_str_ieq(k->isim, gk->kara_liste[i])) {
            gk->engellenen_komut++;
            return OT_HATA_IZIN;
        }
    }

    /* Beyaz liste kontrolü */
    if (gk->komut_beyaz_liste_aktif) {
        bool izinli = false;
        for (i = 0; i < gk->beyaz_liste_boyutu; i++) {
            if (ot_str_ieq(k->isim, gk->beyaz_liste[i])) {
                izinli = true;
                break;
            }
        }
        if (!izinli) {
            gk->engellenen_komut++;
            return OT_HATA_IZIN;
        }
    }

    /* Hız sınırı */
    simdi      = time(NULL);
    pencere_ms = difftime(simdi, gk->pencere_basi) * 1000.0;
    if (pencere_ms >= 1000.0) {
        gk->mevcut_komut_sayisi = 0u;
        gk->pencere_basi        = simdi;
    }
    if (gk->max_komut_saniyede > 0u &&
        gk->mevcut_komut_sayisi >= gk->max_komut_saniyede) {
        gk->engellenen_komut++;
        return OT_HATA_ZAMAN_ASIMI;
    }
    gk->mevcut_komut_sayisi++;
    return OT_BASARILI;
}

void guvenlik_raporu_yazdir(const GuvenlikKatmani *gk, FILE *akis)
{
    const char *seviye_metin[] = {
        "Kapalı", "Düşük", "Orta", "Yüksek", "Paranoya"
    };
    if (!gk || !akis)
        return;
    fprintf(akis, "=== Güvenlik Raporu ===\n");
    fprintf(akis, "  Seviye        : %s\n",
            (int)gk->seviye < 5 ? seviye_metin[gk->seviye] : "?");
    fprintf(akis, "  Engellenen    : %llu\n",
            (unsigned long long)gk->engellenen_komut);
    fprintf(akis, "  Hız Limiti    : %u/sn\n", gk->max_komut_saniyede);
    fprintf(akis, "  Denetim Aktif : %s\n",
            gk->denetim_aktif ? "evet" : "hayır");
    fprintf(akis, "  Kara Liste    : %zu kural\n", gk->kara_liste_boyutu);
    fprintf(akis, "  Beyaz Liste   : %zu kural\n", gk->beyaz_liste_boyutu);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 16 — EKLENTİ YÖNETİCİSİ
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu eklenti_yoneticisi_baslat(EklentiYoneticisi *ey)
{
    OT_NULL_KONTROL(ey);
    memset(ey, 0, sizeof(*ey));
    return OT_BASARILI;
}

OtHataKodu eklenti_kaydet(EklentiYoneticisi *ey, const Eklenti *eklenti)
{
    OT_NULL_KONTROL(ey);
    OT_NULL_KONTROL(eklenti);
    if (ey->boyut >= OT_MAKS_EKLENTI)
        return OT_HATA_KAPASITE;
    /* İsim çakışması kontrolü */
    {
        size_t i;
        for (i = 0; i < ey->boyut; i++) {
            if (ot_str_ieq(ey->liste[i].isim, eklenti->isim))
                return OT_HATA_VAROLAN;
        }
    }
    ey->liste[ey->boyut++] = *eklenti;
    return OT_BASARILI;
}

OtHataKodu eklenti_kaldir(EklentiYoneticisi *ey, const char *isim)
{
    size_t i;
    OT_NULL_KONTROL(ey);
    OT_NULL_KONTROL(isim);
    for (i = 0; i < ey->boyut; i++) {
        if (ot_str_ieq(ey->liste[i].isim, isim)) {
            if (ey->liste[i].durdur)
                ey->liste[i].durdur(ey->liste[i].baglan);
            memmove(&ey->liste[i], &ey->liste[i + 1u],
                    (ey->boyut - i - 1u) * sizeof(Eklenti));
            ey->boyut--;
            return OT_BASARILI;
        }
    }
    return OT_HATA_BULUNAMADI;
}

OtHataKodu eklenti_tum_baslat(EklentiYoneticisi *ey)
{
    size_t i;
    OT_NULL_KONTROL(ey);
    for (i = 0; i < ey->boyut; i++) {
        Eklenti *e = &ey->liste[i];
        if (e->baslat && !e->aktif) {
            OtHataKodu r = e->baslat(e->baglan);
            if (r == OT_BASARILI)
                e->aktif = true;
        }
    }
    return OT_BASARILI;
}

OtHataKodu eklenti_komut_yayinla(EklentiYoneticisi *ey, const Komut *k)
{
    size_t i;
    OT_NULL_KONTROL(ey);
    OT_NULL_KONTROL(k);
    for (i = 0; i < ey->boyut; i++) {
        Eklenti *e = &ey->liste[i];
        if (e->aktif && e->komut_isle)
            e->komut_isle(e->baglan, k);
    }
    return OT_BASARILI;
}

void eklenti_listele(const EklentiYoneticisi *ey, FILE *akis)
{
    size_t i;
    if (!ey || !akis)
        return;
    fprintf(akis, "=== Eklentiler (%zu) ===\n", ey->boyut);
    for (i = 0; i < ey->boyut; i++) {
        fprintf(akis, "  %-20s v%-10s %s  — %s\n",
                ey->liste[i].isim,
                ey->liste[i].surum,
                ey->liste[i].aktif ? "[AKTİF]" : "[PASIF]",
                ey->liste[i].aciklama);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 17 — META VERİ
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu meta_veri_baslat(MetaVeri *mv)
{
    OT_NULL_KONTROL(mv);
    memset(mv, 0, sizeof(*mv));
    return OT_BASARILI;
}

OtHataKodu meta_veri_ayarla(MetaVeri *mv, const char *anahtar, const char *deger)
{
    size_t i;
    OT_NULL_KONTROL(mv);
    OT_NULL_KONTROL(anahtar);
    /* Güncelle */
    for (i = 0; i < mv->boyut; i++) {
        if (strcmp(mv->girdi[i].anahtar, anahtar) == 0) {
            ot_str_kopyala(mv->girdi[i].deger, deger ? deger : "",
                           OT_MAKS_DEGER_UZUNLUGU);
            return OT_BASARILI;
        }
    }
    /* Yeni */
    if (mv->boyut >= OT_MAKS_METAVERI)
        return OT_HATA_KAPASITE;
    ot_str_kopyala(mv->girdi[mv->boyut].anahtar, anahtar, 64u);
    ot_str_kopyala(mv->girdi[mv->boyut].deger,   deger ? deger : "",
                   OT_MAKS_DEGER_UZUNLUGU);
    mv->boyut++;
    return OT_BASARILI;
}

const char *meta_veri_al(const MetaVeri *mv, const char *anahtar,
                          const char *varsayilan)
{
    size_t i;
    if (!mv || !anahtar)
        return varsayilan;
    for (i = 0; i < mv->boyut; i++) {
        if (strcmp(mv->girdi[i].anahtar, anahtar) == 0)
            return mv->girdi[i].deger;
    }
    return varsayilan;
}

void meta_veri_yazdir(const MetaVeri *mv, FILE *akis)
{
    size_t i;
    if (!mv || !akis)
        return;
    fprintf(akis, "=== Meta Veri (%zu) ===\n", mv->boyut);
    for (i = 0; i < mv->boyut; i++)
        fprintf(akis, "  %s = %s\n", mv->girdi[i].anahtar, mv->girdi[i].deger);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 18 — İSTATİSTİK MOTORU
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu istatistik_baslat(IstatistikMotoru *ist)
{
    OT_NULL_KONTROL(ist);
    memset(ist, 0, sizeof(*ist));
    ist->baslangic        = time(NULL);
    ist->en_kisa_islem_ms = DBL_MAX;
    ist->en_uzun_islem_ms = 0.0;
    return OT_BASARILI;
}

void istatistik_komut_kaydet(IstatistikMotoru *ist, bool basarili,
                              double sure_ms)
{
    if (!ist)
        return;
    ist->toplam_komut++;
    if (basarili)
        ist->basarili_komut++;
    else
        ist->basarisiz_komut++;
    if (sure_ms > 0.0) {
        double toplam = ist->ortalama_islem_suresi_ms *
                        (double)(ist->toplam_komut - 1u) + sure_ms;
        ist->ortalama_islem_suresi_ms = toplam / (double)ist->toplam_komut;
        if (sure_ms > ist->en_uzun_islem_ms)
            ist->en_uzun_islem_ms = sure_ms;
        if (sure_ms < ist->en_kisa_islem_ms)
            ist->en_kisa_islem_ms = sure_ms;
    }
    ist->son_guncelleme    = time(NULL);
    ist->calisma_suresi_sn = difftime(ist->son_guncelleme, ist->baslangic);
}

void istatistik_yazdir(const IstatistikMotoru *ist, FILE *akis)
{
    double basari_orani;
    if (!ist || !akis)
        return;
    basari_orani = (ist->toplam_komut > 0u)
        ? 100.0 * (double)ist->basarili_komut / (double)ist->toplam_komut
        : 0.0;
    fprintf(akis, "╔══════════════════════════════════════╗\n");
    fprintf(akis, "║        SimpleOS İstatistik Raporu    ║\n");
    fprintf(akis, "╠══════════════════════════════════════╣\n");
    fprintf(akis, "║  Toplam Komut       : %-15llu║\n",
            (unsigned long long)ist->toplam_komut);
    fprintf(akis, "║  Başarılı           : %-15llu║\n",
            (unsigned long long)ist->basarili_komut);
    fprintf(akis, "║  Başarısız          : %-15llu║\n",
            (unsigned long long)ist->basarisiz_komut);
    fprintf(akis, "║  İptal              : %-15llu║\n",
            (unsigned long long)ist->iptal_komut);
    fprintf(akis, "║  Başarı Oranı       : %-14.2f%%║\n", basari_orani);
    fprintf(akis, "║  Ort. İşlem Süresi  : %-12.3f ms║\n",
            ist->ortalama_islem_suresi_ms);
    fprintf(akis, "║  En Uzun İşlem      : %-12.3f ms║\n",
            ist->en_uzun_islem_ms);
    fprintf(akis, "║  Çalışma Süresi     : %-12.1f  s║\n",
            ist->calisma_suresi_sn);
    fprintf(akis, "║  Toplam Olay        : %-15llu║\n",
            (unsigned long long)ist->toplam_olay);
    fprintf(akis, "║  Zamanlayıcı Atışı  : %-15llu║\n",
            (unsigned long long)ist->zamanlayici_atis);
    fprintf(akis, "╚══════════════════════════════════════╝\n");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 19 — KOMUT İŞLEYİCİ (TEK KOMUT)
 * ═══════════════════════════════════════════════════════════════════════════*/

static void os_docs_yazdir_demo(FILE *akis)
{
    if (!akis)
        akis = stdout;
    fprintf(akis, "╔══════════════════════════════════╗\n");
    fprintf(akis, "║  SimpleOS Otomasyon Motoru       ║\n");
    fprintf(akis, "║  Sürüm: " OT_SURUM_DIZI "                   ║\n");
    fprintf(akis, "║  Komutlar:                       ║\n");
    fprintf(akis, "║    print/yazdir  — Mesaj yazar   ║\n");
    fprintf(akis, "║    start/basla   — Çalıştırır    ║\n");
    fprintf(akis, "║    stop/dur      — Durdurur      ║\n");
    fprintf(akis, "║    pause/durakla — Duraklatır    ║\n");
    fprintf(akis, "║    resume/devam  — Devam eder    ║\n");
    fprintf(akis, "║    status/durum  — Durum göster  ║\n");
    fprintf(akis, "║    help/yardim   — Yardım        ║\n");
    fprintf(akis, "║    exit/cikis    — Çıkar         ║\n");
    fprintf(akis, "╚══════════════════════════════════╝\n");
}

static OtHataKodu tek_komut_isle(OtomasyonMotoru *mot, const Komut *k)
{
    FILE *log;
    if (!mot || !k)
        return OT_HATA_GECERSIZ_ARG;
    log = mot->ayarlar.log_akisi ? mot->ayarlar.log_akisi : stderr;

    switch (k->tip) {
    case KOMUT_YAZDIR:
        if (k->arguman[0] != '\0')
            fprintf(stdout, "%s\n", k->arguman);
        else
            os_docs_yazdir_demo(stdout);
        return OT_BASARILI;

    case KOMUT_CALISTIR:
        fprintf(stdout, "[ÇALIŞTIR] %s\n", k->isim);
        if (mot->ayarlar.hata_ayiklama)
            ot_gunluk(mot, GUNLUK_HATA_AY, "motor",
                      "KOMUT_CALISTIR: %s arg=%s", k->isim, k->arguman);
        return OT_BASARILI;

    case KOMUT_DUR:
        fprintf(stdout, "[ÇIKİŞ] Motor durduruluyor.\n");
        mot->calisyor = false;
        return OT_BASARILI;

    case KOMUT_DURAKLA:
        fprintf(stdout, "[DURAKT] %s\n", k->arguman[0] ? k->arguman : "bekleniyor");
        mot->duraklatildi = true;
        PLATFORM_UYKU_MS(mot->ayarlar.tekrar_araligi_ms ?
                         mot->ayarlar.tekrar_araligi_ms : 1u);
        return OT_BASARILI;

    case KOMUT_DEVAM:
        fprintf(stdout, "[DEVAM]\n");
        mot->duraklatildi = false;
        return OT_BASARILI;

    case KOMUT_DURUM:
        fprintf(stdout, "[DURUM] Çalışıyor=%s Duraklatıldı=%s "
                "Komut=%llu Başarılı=%llu Başarısız=%llu\n",
                mot->calisyor      ? "evet" : "hayır",
                mot->duraklatildi  ? "evet" : "hayır",
                (unsigned long long)mot->calistirilan_komut,
                (unsigned long long)mot->basarili_komut,
                (unsigned long long)mot->basarisiz_komut);
        return OT_BASARILI;

    case KOMUT_YARDIM:
        os_docs_yazdir_demo(stdout);
        return OT_BASARILI;

    case KOMUT_SURUM:
        fprintf(stdout, "SimpleOS Otomasyon Motoru v%s\n", OT_SURUM_DIZI);
        return OT_BASARILI;

    case KOMUT_TEMIZLE:
        otomasyon_temizle(mot);
        fprintf(stdout, "[TEMİZLE] Kuyruk temizlendi.\n");
        return OT_BASARILI;

    case KOMUT_LISTELE:
        komut_kuyrugu_yazdir(&mot->kuyruk, stdout);
        return OT_BASARILI;

    case KOMUT_BEKLE: {
        uint32_t bekle_ms = (k->arguman[0] != '\0')
            ? (uint32_t)atoi(k->arguman) : 1000u;
        fprintf(stdout, "[BEKLE] %u ms\n", bekle_ms);
        PLATFORM_UYKU_MS(bekle_ms);
        return OT_BASARILI;
    }

    case KOMUT_SIFIRLA:
        istatistik_baslat(&mot->istatistik);
        fprintf(stdout, "[SIFIRLA] İstatistikler sıfırlandı.\n");
        return OT_BASARILI;

    default:
        fprintf(log, "[UYARI] Bilinmeyen komut tipi: %d\n", (int)k->tip);
        return OT_HATA_BILINMIYOR;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 20 — OTOMASYON MOTORU — ANA API
 * ═══════════════════════════════════════════════════════════════════════════*/

OtHataKodu otomasyon_baslat(OtomasyonMotoru *mot,
                            const OtomasyonAyarlari *ayarlar)
{
    OT_NULL_KONTROL(mot);
    memset(mot, 0, sizeof(*mot));

    if (ayarlar) {
        mot->ayarlar = *ayarlar;
    } else {
        mot->ayarlar.zaman_asimi_ms      = 0u;
        mot->ayarlar.tekrar_araligi_ms   = 50u;
        mot->ayarlar.hata_ayiklama       = false;
        mot->ayarlar.hata_durumunda_dur  = true;
        mot->ayarlar.log_akisi           = NULL;
        mot->ayarlar.log_seviyesi        = GUNLUK_BILGI;
        mot->ayarlar.istatistik_aktif    = true;
        mot->ayarlar.olay_sistemi_aktif  = true;
        mot->ayarlar.guvenlik_aktif      = false;
        mot->ayarlar.guvenlik_seviyesi   = GUVENLIK_KAPALTI;
        mot->ayarlar.max_tekrar          = 100u;
        mot->ayarlar.renkli_log          = false;
    }

    /* Alt sistemleri başlat */
    OT_HATA_KONTROL(komut_kuyrugu_baslat(&mot->kuyruk));
    OT_HATA_KONTROL(oncelikli_kuyruk_baslat(&mot->oncelikli_kuyruk));
    OT_HATA_KONTROL(olay_sistemi_baslat(&mot->olay));
    OT_HATA_KONTROL(zamanlayici_yoneticisi_baslat(&mot->zamanlayici));
    OT_HATA_KONTROL(istatistik_baslat(&mot->istatistik));
    OT_HATA_KONTROL(eklenti_yoneticisi_baslat(&mot->eklentiler));
    OT_HATA_KONTROL(ortam_baslat(&mot->ortam));
    OT_HATA_KONTROL(meta_veri_baslat(&mot->meta));

    if (mot->ayarlar.guvenlik_aktif)
        OT_HATA_KONTROL(guvenlik_baslat(&mot->guvenlik,
                                        mot->ayarlar.guvenlik_seviyesi));

    /* Günlük ayarları */
    mot->gunluk.akis          = mot->ayarlar.log_akisi;
    mot->gunluk.seviye        = mot->ayarlar.log_seviyesi;
    mot->gunluk.renkli        = mot->ayarlar.renkli_log;
    mot->gunluk.zaman_damgali = true;
    mot->gunluk.seviye_yazili = true;

    /* Motor kimliği ve sürümü */
    ot_str_kopyala(mot->surum, OT_SURUM_DIZI, sizeof(mot->surum));
    snprintf(mot->motor_id, sizeof(mot->motor_id),
             "ot-motor-%u", ot_hash(OT_SURUM_DIZI) % 100000u);

    /* Varsayılan meta veriler */
    meta_veri_ayarla(&mot->meta, "surum", OT_SURUM_DIZI);
    meta_veri_ayarla(&mot->meta, "motor_id", mot->motor_id);

    mot->calisyor      = true;
    mot->duraklatildi  = false;
    mot->baslama_zamani= time(NULL);

    olay_yayinla(&mot->olay, OLAY_MOTOR_BASLADI, "motor",
                 "Otomasyon motoru başlatıldı", 0, NULL);
    ot_gunluk(mot, GUNLUK_BILGI, "motor",
              "Motor başlatıldı: %s", mot->motor_id);

    return OT_BASARILI;
}

OtHataKodu otomasyon_komut_ekle(OtomasyonMotoru *mot, const char *komut_metni,
                                const char *arguman, uint32_t oncelik)
{
    Komut k;
    OtHataKodu sonuc;
    OT_NULL_KONTROL(mot);
    OT_NULL_KONTROL(komut_metni);

    sonuc = komut_olustur(&k, komut_metni, arguman, oncelik);
    if (sonuc != OT_BASARILI)
        return sonuc;
    k.komut_id = ++mot->siradaki_komut_id;

    /* Güvenlik kontrolü */
    if (mot->ayarlar.guvenlik_aktif) {
        sonuc = guvenlik_komut_kontrol(&mot->guvenlik, &k);
        if (sonuc != OT_BASARILI) {
            ot_gunluk(mot, GUNLUK_UYARI, "guvenlik",
                      "Komut engellendi: %s (kod=%d)", komut_metni, sonuc);
            return sonuc;
        }
    }

    /* Öncelik kuyruğuna ekle */
    sonuc = oncelikli_kuyruk_ekle(&mot->oncelikli_kuyruk, &k);
    if (sonuc != OT_BASARILI) {
        mot->istatistik.kuyruk_dolu_sayisi++;
        olay_yayinla(&mot->olay, OLAY_KUYRUK_DOLU, "kuyruk",
                     "Öncelik kuyruğu dolu", sonuc, NULL);
        return sonuc;
    }

    /* Normal kuyruğa da ekle */
    sonuc = komut_kuyrugu_ekle(&mot->kuyruk, &k);
    if (sonuc != OT_BASARILI && sonuc != OT_HATA_KAPASITE)
        return sonuc;

    olay_yayinla(&mot->olay, OLAY_KOMUT_EKLENDI, "motor",
                 komut_metni, 0, NULL);
    ot_gunluk(mot, GUNLUK_HATA_AY, "motor",
              "Komut eklendi: %s [id=%llu]",
              komut_metni, (unsigned long long)k.komut_id);

    /* Eklenti yayını */
    eklenti_komut_yayinla(&mot->eklentiler, &k);

    return OT_BASARILI;
}

OtHataKodu otomasyon_calistir(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->calisyor = true;

    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Çalışma döngüsü başlıyor...");

    while (mot->calisyor && !oncelikli_kuyruk_bos_mu(&mot->oncelikli_kuyruk)) {
        Komut k;
        OtHataKodu al, is;
        time_t t_baslangic;
        double sure_ms;

        if (mot->duraklatildi) {
            PLATFORM_UYKU_MS(100u);
            zamanlayici_guncelle(&mot->zamanlayici);
            continue;
        }

        al = oncelikli_kuyruk_al(&mot->oncelikli_kuyruk, &k);
        if (al != OT_BASARILI)
            break;

        t_baslangic = time(NULL);
        mot->calistirilan_komut++;

        is    = tek_komut_isle(mot, &k);
        sure_ms = ot_sure_ms(t_baslangic);

        if (is == OT_BASARILI) {
            mot->basarili_komut++;
            istatistik_komut_kaydet(&mot->istatistik, true,  sure_ms);
            olay_yayinla(&mot->olay, OLAY_KOMUT_TAMAMLANDI, "motor",
                         k.isim, 0, NULL);
        } else {
            mot->basarisiz_komut++;
            istatistik_komut_kaydet(&mot->istatistik, false, sure_ms);
            olay_yayinla(&mot->olay, OLAY_KOMUT_BASARISIZ, "motor",
                         k.isim, (int32_t)is, NULL);
            ot_gunluk(mot, GUNLUK_UYARI, "motor",
                      "Komut başarısız: %s (kod=%s)",
                      k.isim, hata_acikla(is));
            if (mot->ayarlar.hata_durumunda_dur)
                return is;
        }

        if (k.tip == KOMUT_DUR)
            break;

        zamanlayici_guncelle(&mot->zamanlayici);

        if (mot->ayarlar.tekrar_araligi_ms > 0u)
            PLATFORM_UYKU_MS(mot->ayarlar.tekrar_araligi_ms);
    }

    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Çalışma döngüsü tamamlandı.");
    return OT_BASARILI;
}

OtHataKodu otomasyon_durdur(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->calisyor = false;
    olay_yayinla(&mot->olay, OLAY_MOTOR_DURDU, "motor",
                 "Motor durduruldu", 0, NULL);
    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Motor durduruldu.");
    return OT_BASARILI;
}

OtHataKodu otomasyon_durakla(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->duraklatildi = true;
    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Motor duraklatıldı.");
    return OT_BASARILI;
}

OtHataKodu otomasyon_devam_et(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->duraklatildi = false;
    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Motor devam ettiriliyor.");
    return OT_BASARILI;
}

void otomasyon_istatistik_yazdir(const OtomasyonMotoru *mot, FILE *akis)
{
    FILE *cikis = akis ? akis : stdout;
    if (!mot)
        return;
    fprintf(cikis, "=== Motor İstatistikleri ===\n");
    fprintf(cikis, "  Çalıştırılan : %llu\n",
            (unsigned long long)mot->calistirilan_komut);
    fprintf(cikis, "  Başarılı     : %llu\n",
            (unsigned long long)mot->basarili_komut);
    fprintf(cikis, "  Başarısız    : %llu\n",
            (unsigned long long)mot->basarisiz_komut);
    fprintf(cikis, "  Kuyruk boyut : %zu\n",
            komut_kuyrugu_boyut(&mot->kuyruk));
    istatistik_yazdir(&mot->istatistik, cikis);
}

void otomasyon_temizle(OtomasyonMotoru *mot)
{
    if (!mot)
        return;
    komut_kuyrugu_baslat(&mot->kuyruk);
    oncelikli_kuyruk_baslat(&mot->oncelikli_kuyruk);
    mot->calistirilan_komut = 0u;
    mot->basarili_komut     = 0u;
    mot->basarisiz_komut    = 0u;
    mot->calisyor           = false;
    mot->duraklatildi       = false;
    ot_gunluk(mot, GUNLUK_BILGI, "motor", "Motor temizlendi.");
}

void otomasyon_kapat(OtomasyonMotoru *mot)
{
    if (!mot)
        return;
    otomasyon_durdur(mot);
    eklenti_tum_baslat(&mot->eklentiler); /* Eklentiler zaten kapatılacak */
    memset(mot, 0, sizeof(*mot));
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 21 — HATA YÖNETİMİ
 * ═══════════════════════════════════════════════════════════════════════════*/

const char *hata_acikla(OtHataKodu kod)
{
    switch (kod) {
    case OT_BASARILI:          return "basarili";
    case OT_HATA_GENEL:        return "genel hata";
    case OT_HATA_BELLEK:       return "bellek hatasi";
    case OT_HATA_GECERSIZ_ARG: return "gecersiz arguman";
    case OT_HATA_BILINMIYOR:   return "bilinmiyor";
    case OT_HATA_ZAMAN_ASIMI:  return "zaman asimi";
    case OT_HATA_KAPASITE:     return "kapasite asimi";
    case OT_HATA_IZIN:         return "erisim izni yok";
    case OT_HATA_IO:           return "girdi/cikti hatasi";
    case OT_HATA_DURUMU:       return "gecersiz durum";
    case OT_HATA_VAROLAN:      return "zaten var";
    case OT_HATA_BULUNAMADI:   return "bulunamadi";
    case OT_HATA_AKIS:         return "akis hatasi";
    case OT_HATA_SIRALAMA:     return "siralama hatasi";
    case OT_HATA_DOGRULAMA:    return "dogrulama hatasi";
    case OT_HATA_IPTAL:        return "islem iptal edildi";
    default:                   return "tanimlanmamis hata kodu";
    }
}

bool hata_kritik_mi(OtHataKodu kod)
{
    return (kod == OT_HATA_BELLEK || kod == OT_HATA_IO);
}

void hata_yazdir(OtHataKodu kod, const char *baglan, FILE *akis)
{
    if (!akis)
        akis = stderr;
    fprintf(akis, "[HATA %d] %s%s%s\n",
            (int)kod,
            hata_acikla(kod),
            baglan ? ": " : "",
            baglan ? baglan : "");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 22 — SERİLEŞTİRME (JSON ÇIKTI)
 * ═══════════════════════════════════════════════════════════════════════════*/

static void json_metin_yaz(FILE *akis, const char *s)
{
    if (!s) {
        fprintf(akis, "null");
        return;
    }
    fputc('"', akis);
    while (*s) {
        unsigned char c = (unsigned char)*s;
        switch (c) {
        case '"':  fputs("\\\"", akis); break;
        case '\\': fputs("\\\\", akis); break;
        case '\n': fputs("\\n",  akis); break;
        case '\r': fputs("\\r",  akis); break;
        case '\t': fputs("\\t",  akis); break;
        default:
            if (c < 0x20)
                fprintf(akis, "\\u%04x", (unsigned)c);
            else
                fputc(c, akis);
        }
        s++;
    }
    fputc('"', akis);
}

OtHataKodu motor_json_yaz(const OtomasyonMotoru *mot, FILE *akis)
{
    OT_NULL_KONTROL(mot);
    if (!akis)
        akis = stdout;

    fprintf(akis, "{\n");
    fprintf(akis, "  \"surum\": \"%s\",\n", mot->surum);
    fprintf(akis, "  \"motor_id\": \"%s\",\n", mot->motor_id);
    fprintf(akis, "  \"calisyor\": %s,\n",
            mot->calisyor ? "true" : "false");
    fprintf(akis, "  \"duraklatildi\": %s,\n",
            mot->duraklatildi ? "true" : "false");
    fprintf(akis, "  \"istatistik\": {\n");
    fprintf(akis, "    \"toplam_komut\": %llu,\n",
            (unsigned long long)mot->istatistik.toplam_komut);
    fprintf(akis, "    \"basarili\": %llu,\n",
            (unsigned long long)mot->istatistik.basarili_komut);
    fprintf(akis, "    \"basarisiz\": %llu,\n",
            (unsigned long long)mot->istatistik.basarisiz_komut);
    fprintf(akis, "    \"ort_ms\": %.3f,\n",
            mot->istatistik.ortalama_islem_suresi_ms);
    fprintf(akis, "    \"calisma_sn\": %.1f\n",
            mot->istatistik.calisma_suresi_sn);
    fprintf(akis, "  },\n");
    fprintf(akis, "  \"kuyruk_boyut\": %zu,\n",
            komut_kuyrugu_boyut(&mot->kuyruk));
    fprintf(akis, "  \"meta\": {\n");
    {
        size_t i;
        for (i = 0; i < mot->meta.boyut; i++) {
            fprintf(akis, "    ");
            json_metin_yaz(akis, mot->meta.girdi[i].anahtar);
            fprintf(akis, ": ");
            json_metin_yaz(akis, mot->meta.girdi[i].deger);
            if (i + 1u < mot->meta.boyut)
                fputc(',', akis);
            fputc('\n', akis);
        }
    }
    fprintf(akis, "  }\n");
    fprintf(akis, "}\n");
    return OT_BASARILI;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 23 — YARDIMCI DEMO FONKSİYONLARI
 * ═══════════════════════════════════════════════════════════════════════════*/

/* Demo boru hattı aşamaları */
static OtHataKodu demo_normalles(BasitDeger *giris, BasitDeger *cikis,
                                  void *baglan)
{
    char tampon[OT_MAKS_DEGER_UZUNLUGU];
    OT_KULLANILMADI(baglan);
    if (!giris || !cikis)
        return OT_HATA_GECERSIZ_ARG;
    ot_str_kopyala(tampon, giris->deger, sizeof(tampon));
    ot_str_kirp(tampon);
    basit_deger_ata(cikis, tampon);
    return OT_BASARILI;
}

static OtHataKodu demo_buyuk_harf(BasitDeger *giris, BasitDeger *cikis,
                                   void *baglan)
{
    size_t i;
    char tampon[OT_MAKS_DEGER_UZUNLUGU];
    OT_KULLANILMADI(baglan);
    if (!giris || !cikis)
        return OT_HATA_GECERSIZ_ARG;
    ot_str_kopyala(tampon, giris->deger, sizeof(tampon));
    for (i = 0; tampon[i]; i++)
        tampon[i] = (char)toupper((unsigned char)tampon[i]);
    basit_deger_ata(cikis, tampon);
    return OT_BASARILI;
}

static OtHataKodu demo_uzunluk_kontrol(BasitDeger *giris, BasitDeger *cikis,
                                        void *baglan)
{
    OT_KULLANILMADI(baglan);
    if (!giris || !cikis)
        return OT_HATA_GECERSIZ_ARG;
    if (strlen(giris->deger) == 0)
        return OT_HATA_DOGRULAMA;
    basit_deger_ata(cikis, giris->deger);
    return OT_BASARILI;
}

/* Demo zamanlayıcı */
static OtHataKodu demo_zamanlayici_fonk(void *baglan)
{
    static uint32_t sayac = 0u;
    OT_KULLANILMADI(baglan);
    fprintf(stdout, "[ZAMANLAYICI] Atış #%u\n", ++sayac);
    return OT_BASARILI;
}

/* Demo olay dinleyici */
static void demo_olay_dinleyici(const OlayVeri *olay, void *baglan)
{
    OT_KULLANILMADI(baglan);
    if (!olay)
        return;
    fprintf(stdout, "[OLAY #%llu] tip=%d kaynak=%s\n",
            (unsigned long long)olay->olay_id,
            (int)olay->tip,
            olay->kaynak);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 24 — TAM ENTEGRASYON TESTİ
 * ═══════════════════════════════════════════════════════════════════════════*/

static void test_basit_deger(void)
{
    BasitDeger bd;
    DegerDugumu dugum;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: BasitDeger ─────────────────────\n");
    basit_deger_baslat(&bd);
    basit_deger_ata(&bd, "merhaba dunya");
    basit_deger_isle(&bd);

    memset(&dugum, 0, sizeof(dugum));
    dugum.tip = DUGUM_METIN;
    ot_str_kopyala(dugum.deger, "test-dugum", sizeof(dugum.deger));
    sonuc = basit_deger_dugum_ekle(&bd, dugum);
    fprintf(stdout, "Düğüm ekleme: %s\n", hata_acikla(sonuc));
    basit_deger_json_yaz(&bd, stdout);
}

static void test_kesir(void)
{
    KesirDeger kd_a, kd_b, kd_s;
    OtHataKodu sonuc;
    char tampon[32];

    fprintf(stdout, "\n── Test: KesirDeger ─────────────────────\n");

    kesir_baslat(&kd_a, 3, 4);
    kesir_baslat(&kd_b, 1, 6);
    kesir_yazdir(&kd_a, stdout);
    kesir_yazdir(&kd_b, stdout);

    sonuc = kesir_topla(&kd_a, &kd_b, &kd_s);
    fprintf(stdout, "3/4 + 1/6 = ");
    kesir_yazdir(&kd_s, stdout);

    sonuc = kesir_carp(&kd_a, &kd_b, &kd_s);
    fprintf(stdout, "3/4 * 1/6 = ");
    kesir_yazdir(&kd_s, stdout);

    sonuc = kesir_ata_metin(&kd_a, "7 / 8");
    fprintf(stdout, "Metin ayrıştırma '7 / 8': %s\n", hata_acikla(sonuc));
    kesir_metin_al(&kd_a, tampon, sizeof(tampon));
    fprintf(stdout, "Metin çıktı: %s = %.4f\n", tampon, kd_a.ondalik);

    OT_KULLANILMADI(sonuc);
}

static void test_kuyruk(void)
{
    KomutKuyrugu kq;
    Komut k_giris, k_cikis;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: KomutKuyrugu ───────────────────\n");
    komut_kuyrugu_baslat(&kq);

    sonuc = komut_olustur(&k_giris, "print", "Merhaba SimpleOS!", 10u);
    komut_kuyrugu_ekle(&kq, &k_giris);

    sonuc = komut_olustur(&k_giris, "status", NULL, 5u);
    komut_kuyrugu_ekle(&kq, &k_giris);

    fprintf(stdout, "Kuyruk boyutu: %zu\n", komut_kuyrugu_boyut(&kq));

    while (!komut_kuyrugu_bos_mu(&kq)) {
        sonuc = komut_kuyrugu_al(&kq, &k_cikis);
        if (sonuc == OT_BASARILI)
            komut_yazdir(&k_cikis, stdout);
    }
    OT_KULLANILMADI(sonuc);
}

static void test_oncelikli_kuyruk(void)
{
    OncelikliKuyruk ok;
    Komut k_giris, k_cikis;
    static const struct { const char *isim; uint32_t oncelik; } testler[] = {
        { "print",  50u },
        { "start",   1u },
        { "status", 25u },
        { "stop",   10u },
        { NULL, 0u }
    };
    int i;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: OncelikliKuyruk (MinHeap) ─────\n");
    oncelikli_kuyruk_baslat(&ok);

    for (i = 0; testler[i].isim != NULL; i++) {
        sonuc = komut_olustur(&k_giris, testler[i].isim, NULL, testler[i].oncelik);
        if (sonuc == OT_BASARILI)
            oncelikli_kuyruk_ekle(&ok, &k_giris);
    }

    fprintf(stdout, "Öncelik sırası (küçük=yüksek öncelik):\n");
    while (!oncelikli_kuyruk_bos_mu(&ok)) {
        sonuc = oncelikli_kuyruk_al(&ok, &k_cikis);
        if (sonuc == OT_BASARILI)
            fprintf(stdout, "  [%3u] %s\n", k_cikis.oncelik, k_cikis.isim);
    }
    OT_KULLANILMADI(sonuc);
}

static void test_boru_hatti(void)
{
    BoruHatti bh;
    BasitDeger giris;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: BoruHattı ──────────────────────\n");
    boru_hatti_baslat(&bh, "test-hatti", false);

    boru_hatti_asama_ekle(&bh, "normalles",       demo_normalles,       NULL, false);
    boru_hatti_asama_ekle(&bh, "uzunluk-kontrol", demo_uzunluk_kontrol, NULL, false);
    boru_hatti_asama_ekle(&bh, "buyuk-harf",      demo_buyuk_harf,      NULL, false);

    basit_deger_ata(&giris, "  simpleos boru hatti testi  ");
    sonuc = boru_hatti_calistir(&bh, &giris);
    fprintf(stdout, "Sonuç: %s\nÇıktı: %s\n",
            hata_acikla(sonuc), giris.deger);
    boru_hatti_durum_yazdir(&bh, stdout);
}

static void test_guvenlik(void)
{
    GuvenlikKatmani gk;
    Komut k;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: GuvenlikKatmanı ────────────────\n");
    guvenlik_baslat(&gk, GUVENLIK_YUKSEK);

    guvenlik_beyaz_liste_ekle(&gk, "print");
    guvenlik_beyaz_liste_ekle(&gk, "status");

    komut_olustur(&k, "print", "izinli", 1u);
    sonuc = guvenlik_komut_kontrol(&gk, &k);
    fprintf(stdout, "'print' izni: %s\n", hata_acikla(sonuc));

    komut_olustur(&k, "exit", NULL, 1u);
    sonuc = guvenlik_komut_kontrol(&gk, &k);
    fprintf(stdout, "'exit' izni : %s\n", hata_acikla(sonuc));

    guvenlik_raporu_yazdir(&gk, stdout);
}

static void test_olay_sistemi(void)
{
    OlaySistemi os;

    fprintf(stdout, "\n── Test: OlaySistemi ────────────────────\n");
    olay_sistemi_baslat(&os);
    olay_dinleyici_ekle(&os, OLAY_BILINMIYOR, demo_olay_dinleyici, NULL);

    olay_yayinla(&os, OLAY_MOTOR_BASLADI,    "test", "Motor başladı",    0, NULL);
    olay_yayinla(&os, OLAY_KOMUT_EKLENDI,    "test", "Komut eklendi",    0, NULL);
    olay_yayinla(&os, OLAY_KOMUT_TAMAMLANDI, "test", "Komut tamamlandı", 0, NULL);
    olay_yayinla(&os, OLAY_HATA,             "test", "Örnek hata",      -1, NULL);

    fprintf(stdout, "Toplam olay: %llu\n",
            (unsigned long long)os.toplam_olay);
    olay_gecmis_yazdir(&os, stdout, 10u);
}

static void test_tam_motor(void)
{
    OtomasyonMotoru mot;
    OtomasyonAyarlari ayarlar;
    OtHataKodu sonuc;

    fprintf(stdout, "\n── Test: Tam Motor Entegrasyonu ─────────\n");

    memset(&ayarlar, 0, sizeof(ayarlar));
    ayarlar.tekrar_araligi_ms  = 0u;
    ayarlar.hata_ayiklama      = true;
    ayarlar.hata_durumunda_dur = false;
    ayarlar.istatistik_aktif   = true;
    ayarlar.olay_sistemi_aktif = true;
    ayarlar.log_akisi          = stdout;
    ayarlar.log_seviyesi       = GUNLUK_BILGI;

    sonuc = otomasyon_baslat(&mot, &ayarlar);
    fprintf(stdout, "Motor başlatma: %s\n", hata_acikla(sonuc));

    /* Olay dinleyici bağla */
    olay_dinleyici_ekle(&mot.olay, OLAY_BILINMIYOR, demo_olay_dinleyici, NULL);

    /* Zamanlayıcı ekle */
    {
        uint32_t zid;
        zamanlayici_ekle(&mot.zamanlayici, "demo-zam",
                         ZAMANLAYICI_TEK_SEFER, 1u,
                         demo_zamanlayici_fonk, NULL, &zid);
        fprintf(stdout, "Zamanlayıcı ID: %u\n", zid);
    }

    /* Komutlar ekle */
    otomasyon_komut_ekle(&mot, "print",   "SimpleOS v3.0 çalışıyor!", 10u);
    otomasyon_komut_ekle(&mot, "status",  NULL,                        5u);
    otomasyon_komut_ekle(&mot, "version", NULL,                        3u);
    otomasyon_komut_ekle(&mot, "print",   "Ortam: production",        10u);

    /* Çalıştır */
    sonuc = otomasyon_calistir(&mot);
    fprintf(stdout, "\nMotor çıkış kodu: %s\n", hata_acikla(sonuc));

    /* Raporlar */
    otomasyon_istatistik_yazdir(&mot, stdout);
    guvenlik_raporu_yazdir(&mot.guvenlik, stdout);
    meta_veri_yazdir(&mot.meta, stdout);

    fprintf(stdout, "\n── JSON Çıktı ───────────────────────────\n");
    motor_json_yaz(&mot, stdout);

    otomasyon_kapat(&mot);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BÖLÜM 25 — ANA GİRİŞ NOKTASI
 * ═══════════════════════════════════════════════════════════════════════════*/

int main(void)
{
    fprintf(stdout,
            "╔══════════════════════════════════════════════════╗\n"
            "║   SimpleOS Otomasyon Motoru  v%-19s║\n"
            "║   Kapsamlı Entegrasyon Testi                     ║\n"
            "╚══════════════════════════════════════════════════╝\n\n",
            OT_SURUM_DIZI);

    test_basit_deger();
    test_kesir();
    test_kuyruk();
    test_oncelikli_kuyruk();
    test_boru_hatti();
    test_guvenlik();
    test_olay_sistemi();
    test_tam_motor();

    fprintf(stdout,
            "\n╔══════════════════════════════════╗\n"
            "║  Tüm testler tamamlandı. ✓       ║\n"
            "╚══════════════════════════════════╝\n");
    return 0;
}

/* ════════════════════════════════════════════════════════════════════════════
 *  EOF — otomation.c  |  SimpleOS v3.0.0
 * ════════════════════════════════════════════════════════════════════════════*/