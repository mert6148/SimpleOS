/**
 * SimpleOS Otomasyon Sistemi — uygulama
 * otomation.h ile eşleşen motor, kuyruk, kesir ve basit değer işlemleri.
 */

#include "otomation.h"
#include <ctype.h>
#include <stddef.h>
#include <string.h>

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
        strncpy(bd->deger, deger, OT_MAKS_DEGER_UZUNLUGU - 1u);
        strncpy(bd->metin, deger, OT_MAKS_DEGER_UZUNLUGU - 1u);
        strncpy(bd->ham, deger, OT_MAKS_DEGER_UZUNLUGU - 1u);
    }
}

void basit_deger_ata_ham(BasitDeger *bd, const char *deger, const char *metin,
                         const char *ham)
{
    if (!bd)
        return;
    basit_deger_baslat(bd);
    if (deger)
        strncpy(bd->deger, deger, OT_MAKS_DEGER_UZUNLUGU - 1u);
    if (metin)
        strncpy(bd->metin, metin, OT_MAKS_DEGER_UZUNLUGU - 1u);
    if (ham)
        strncpy(bd->ham, ham, OT_MAKS_DEGER_UZUNLUGU - 1u);
}

OtHataKodu basit_deger_dugum_ekle(BasitDeger *bd, DegerDugumu dugum)
{
    OT_NULL_KONTROL(bd);
    if (bd->dugum_sayisi >= OT_MAKS_DUGUM_SAYISI)
        return OT_HATA_KAPASITE;
    bd->dugumler[bd->dugum_sayisi++] = dugum;
    return OT_BASARILI;
}

void basit_deger_isle(const BasitDeger *bd)
{
    FILE *out = stdout;
    size_t i;
    if (!bd)
        return;
    fprintf(out, "[BasitDeger] deger=%s metin=%s\n", bd->deger, bd->metin);
    for (i = 0; i < bd->dugum_sayisi; i++) {
        fprintf(out, "  dugum[%zu] tip=%d deger=%s\n", i,
                (int)bd->dugumler[i].tip, bd->dugumler[i].deger);
    }
}

void kesir_baslat(KesirDeger *kd, int32_t pay, int32_t payda)
{
    if (!kd)
        return;
    memset(kd, 0, sizeof(*kd));
    kd->pay = pay;
    kd->payda = payda;
    if (payda != 0)
        kd->ondalik = (double)pay / (double)payda;
    else
        kd->ondalik = 0.0;
}

static const char *atla_bosluk(const char *s)
{
    if (!s)
        return s;
    while (*s && isspace((unsigned char)*s))
        s++;
    return s;
}

OtHataKodu kesir_ata_metin(KesirDeger *kd, const char *metin)
{
    int32_t pay = 0;
    int32_t payda = 1;
    char buf[OT_MAKS_DEGER_UZUNLUGU];
    const char *p;
    char *end;
    long lp;
    long lq;

    OT_NULL_KONTROL(kd);
    OT_NULL_KONTROL(metin);
    strncpy(buf, metin, sizeof(buf) - 1u);
    buf[sizeof(buf) - 1u] = '\0';
    p = atla_bosluk(buf);
    if (*p == '\0')
        return OT_HATA_GECERSIZ_ARG;
    lp = strtol(p, &end, 10);
    if (end == p)
        return OT_HATA_GECERSIZ_ARG;
    pay = (int32_t)lp;
    p = atla_bosluk(end);
    if (*p == '/') {
        p++;
        p = atla_bosluk(p);
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
    if (!kd)
        return 0.0;
    return kd->ondalik;
}

void kesir_yazdir(const KesirDeger *kd, FILE *akis)
{
    FILE *out = akis ? akis : stdout;
    if (!kd)
        return;
    fprintf(out, "%d/%d = %.6f\n", (int)kd->pay, (int)kd->payda, kd->ondalik);
}

KomutTipi komut_cozumle(const char *metin)
{
    if (!metin || metin[0] == '\0')
        return KOMUT_BILINMIYOR;
    if (ot_str_ieq(metin, "print") || ot_str_ieq(metin, "yazdir"))
        return KOMUT_YAZDIR;
    if (ot_str_ieq(metin, "exit") || ot_str_ieq(metin, "cikis"))
        return KOMUT_DUR;
    if (ot_str_ieq(metin, "start") || ot_str_ieq(metin, "basla"))
        return KOMUT_CALISTIR;
    if (ot_str_ieq(metin, "stop") || ot_str_ieq(metin, "dur"))
        return KOMUT_DURAKLA;
    if (ot_str_ieq(metin, "pause") || ot_str_ieq(metin, "durakla"))
        return KOMUT_DURAKLA;
    if (ot_str_ieq(metin, "resume") || ot_str_ieq(metin, "devam"))
        return KOMUT_DEVAM;
    return KOMUT_BILINMIYOR;
}

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
    return OT_BASARILI;
}

OtHataKodu komut_kuyrugu_al(KomutKuyrugu *kq, Komut *hedef)
{
    OT_NULL_KONTROL(kq);
    OT_NULL_KONTROL(hedef);
    if (kq->boyut == 0u)
        return OT_HATA_GENEL;
    *hedef = kq->ogeler[kq->bas];
    kq->bas = (kq->bas + 1u) % OT_MAKS_KUYRUK_BOYUTU;
    kq->boyut--;
    return OT_BASARILI;
}

bool komut_kuyrugu_bos_mu(const KomutKuyrugu *kq)
{
    if (!kq)
        return true;
    return kq->boyut == 0u;
}

size_t komut_kuyrugu_boyut(const KomutKuyrugu *kq)
{
    if (!kq)
        return 0u;
    return kq->boyut;
}

static OtHataKodu tek_komut_isle(OtomasyonMotoru *mot, const Komut *k)
{
    FILE *log = mot->ayarlar.log_akisi;
    if (!log)
        log = stderr;

    switch (k->tip) {
    case KOMUT_YAZDIR:
        if (k->arguman[0] != '\0')
            fprintf(stdout, "%s\n", k->arguman);
        else
            os_docs_yazdir_demo(stdout);
        return OT_BASARILI;
    case KOMUT_CALISTIR:
        fprintf(stdout, "Otomasyon calisiyor: %s\n", k->isim);
        if (mot->ayarlar.hata_ayiklama)
            ot_log(mot, "INFO", "KOMUT_CALISTIR %s", k->isim);
        return OT_BASARILI;
    case KOMUT_DUR:
        fprintf(stdout, "Cikis (exit).\n");
        return OT_BASARILI;
    case KOMUT_DURAKLA:
        fprintf(stdout, "Duraklat / stop.\n");
        PLATFORM_UYKU_MS(mot->ayarlar.tekrar_araligi_ms ?
                             mot->ayarlar.tekrar_araligi_ms : 1u);
        return OT_BASARILI;
    case KOMUT_DEVAM:
        fprintf(stdout, "Devam.\n");
        return OT_BASARILI;
    default:
        fprintf(log, "Bilinmeyen komut tipi\n");
        return OT_HATA_BILINMIYOR;
    }
}

OtHataKodu otomasyon_baslat(OtomasyonMotoru *mot,
                            const OtomasyonAyarlari *ayarlar)
{
    OT_NULL_KONTROL(mot);
    memset(mot, 0, sizeof(*mot));
    if (ayarlar)
        mot->ayarlar = *ayarlar;
    else {
        mot->ayarlar.zaman_asimi_ms = 0u;
        mot->ayarlar.tekrar_araligi_ms = 50u;
        mot->ayarlar.hata_ayiklama = false;
        mot->ayarlar.hata_durumunda_dur = true;
        mot->ayarlar.log_akisi = NULL;
    }
    OT_HATA_KONTROL(komut_kuyrugu_baslat(&mot->kuyruk));
    mot->calisyor = true;
    mot->baslama_zamani = time(NULL);
    return OT_BASARILI;
}

OtHataKodu otomasyon_komut_ekle(OtomasyonMotoru *mot, const char *komut_metni,
                                const char *arguman, uint32_t oncelik)
{
    Komut k;
    OT_NULL_KONTROL(mot);
    OT_NULL_KONTROL(komut_metni);
    memset(&k, 0, sizeof(k));
    k.tip = komut_cozumle(komut_metni);
    if (k.tip == KOMUT_BILINMIYOR)
        return OT_HATA_GECERSIZ_ARG;
    strncpy(k.isim, komut_metni, OT_MAKS_KOMUT_UZUNLUGU - 1u);
    k.isim[OT_MAKS_KOMUT_UZUNLUGU - 1u] = '\0';
    if (arguman)
        strncpy(k.arguman, arguman, OT_MAKS_KOMUT_UZUNLUGU - 1u);
    k.arguman[OT_MAKS_KOMUT_UZUNLUGU - 1u] = '\0';
    k.oncelik = oncelik;
    k.aktif = true;
    return komut_kuyrugu_ekle(&mot->kuyruk, &k);
}

OtHataKodu otomasyon_calistir(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->calisyor = true;
    while (mot->calisyor && !komut_kuyrugu_bos_mu(&mot->kuyruk)) {
        Komut k;
        OtHataKodu al;
        OtHataKodu is;

        al = komut_kuyrugu_al(&mot->kuyruk, &k);
        if (al != OT_BASARILI)
            break;
        mot->calistirilan_komut++;
        is = tek_komut_isle(mot, &k);
        if (is == OT_BASARILI)
            mot->basarili_komut++;
        else {
            mot->basarisiz_komut++;
            if (mot->ayarlar.hata_durumunda_dur)
                return is;
        }
        if (k.tip == KOMUT_DUR) {
            mot->calisyor = false;
            break;
        }
        if (mot->ayarlar.tekrar_araligi_ms > 0u)
            PLATFORM_UYKU_MS(mot->ayarlar.tekrar_araligi_ms);
    }
    return OT_BASARILI;
}

OtHataKodu otomasyon_durdur(OtomasyonMotoru *mot)
{
    OT_NULL_KONTROL(mot);
    mot->calisyor = false;
    return OT_BASARILI;
}

void otomasyon_istatistik_yazdir(const OtomasyonMotoru *mot, FILE *akis)
{
    FILE *out = akis ? akis : stdout;
    if (!mot)
        return;
    fprintf(out, "calistirilan=%llu basarili=%llu basarisiz=%llu kuyruk=%zu\n",
            (unsigned long long)mot->calistirilan_komut,
            (unsigned long long)mot->basarili_komut,
            (unsigned long long)mot->basarisiz_komut,
            (size_t)komut_kuyrugu_boyut(&mot->kuyruk));
}

void otomasyon_temizle(OtomasyonMotoru *mot)
{
    if (!mot)
        return;
    komut_kuyrugu_baslat(&mot->kuyruk);
    mot->calistirilan_komut = 0u;
    mot->basarili_komut = 0u;
    mot->basarisiz_komut = 0u;
    mot->calisyor = false;
}

const char *hata_acikla(OtHataKodu kod)
{
    switch (kod) {
    case OT_BASARILI:
        return "basarili";
    case OT_HATA_GENEL:
        return "genel hata";
    case OT_HATA_BELLEK:
        return "bellek";
    case OT_HATA_GECERSIZ_ARG:
        return "gecersiz arguman";
    case OT_HATA_BILINMIYOR:
        return "bilinmiyor";
    case OT_HATA_ZAMAN_ASIMI:
        return "zaman asimi";
    case OT_HATA_KAPASITE:
        return "kapasite";
    case OT_HATA_IZIN:
        return "izin";
    case OT_HATA_IO:
        return "girdi/cikti";
    default:
        return "tanimlanmamis kod";
    }
}

void ot_log(const OtomasyonMotoru *mot, const char *seviye, const char *fmt,
            ...)
{
    va_list ap;
    FILE *out = stderr;
    if (mot && mot->ayarlar.log_akisi)
        out = mot->ayarlar.log_akisi;
    fprintf(out, "[%s] ", seviye ? seviye : "?");
    va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);
    fputc('\n', out);
}
