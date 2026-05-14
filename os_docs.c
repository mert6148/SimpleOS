/**
 * os_docs — otomasyon motoru ile entegre örnek komut hattı aracı.
 * Eski hatalı C++ benzeri taslak yerine C99; otomation.h API kullanır.
 */

#include "otomation.h"
#include <stdio.h>
#include <string.h>

static const char *const g_os_docs_demo[] = {
    "apple",
    "banana",
    "cherry",
};

void os_docs_yazdir_demo(FILE *akis)
{
    FILE *out = akis ? akis : stdout;
    size_t i;
    size_t n = sizeof(g_os_docs_demo) / sizeof(g_os_docs_demo[0]);
    for (i = 0; i < n; i++)
        fprintf(out, "%s\n", g_os_docs_demo[i]);
}

static void kullanim(FILE *out, const char *prog)
{
    fprintf(out,
            "Kullanim: %s <komut> [arguman]\n"
            "  Komutlar: print yazdir | exit cikis | start basla | stop dur | "
            "pause durakla | resume devam\n"
            "Ornek: %s print\n",
            prog, prog);
}

static OtHataKodu tek_satir_calistir(const char *komut, const char *arg)
{
    OtomasyonAyarlari ay = {0u, 10u, false, false, NULL};
    OtomasyonMotoru mot;
    OtHataKodu st;

    st = otomasyon_baslat(&mot, &ay);
    if (st != OT_BASARILI)
        return st;
    st = otomasyon_komut_ekle(&mot, komut, arg ? arg : "", 0u);
    if (st != OT_BASARILI) {
        otomasyon_temizle(&mot);
        return st;
    }
    st = otomasyon_calistir(&mot);
    otomasyon_temizle(&mot);
    return st;
}

int main(int argc, char **argv)
{
    const char *prog = (argc > 0 && argv[0]) ? argv[0] : "os_docs";

    if (argc < 2) {
        kullanim(stderr, prog);
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        kullanim(stdout, prog);
        return 0;
    }

    {
        const char *arg = (argc >= 3) ? argv[2] : "";
        OtHataKodu st = tek_satir_calistir(argv[1], arg);
        if (st != OT_BASARILI) {
            fprintf(stderr, "Hata: %s\n", hata_acikla(st));
            return 1;
        }
    }

    return 0;
}
