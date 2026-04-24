/**
 * SimpleOS Kernel - Entegrasyon Testi
 * test_kernel.cpp
 *
 * Derleme:
 *   gcc -c kernelintegration.c -o kernelintegration.o -std=c99 -Wall
 *   g++ test_kernel.cpp kernelintegration.o -o test_kernel -std=c++14 -Wall -lpthread
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include "KernelArayuz.hpp"

using namespace SimpleOS;

/* ─── Test Yardımcıları ───────────────────────────────────── */

static int test_sayisi  = 0;
static int gecen_sayisi = 0;

#define TEST_BASLAT(isim) \
    do { std::cout << "\n[TEST] " #isim "... "; ++test_sayisi; } while(0)

#define TEST_GECTI() \
    do { std::cout << "GECTI"; ++gecen_sayisi; } while(0)

#define TEST_ASSERT(kosul) \
    do { if (!(kosul)) { \
        std::cout << "BASARISIZ (" #kosul " yanlis, satir=" << __LINE__ << ")"; \
        return; \
    }} while(0)

/* ─── Test Senaryoları ────────────────────────────────────── */

static void test_baslat_kapat() {
    TEST_BASLAT(baslat_kapat);

    {
        Kernel k;
        TEST_ASSERT(k.durum() == KERNEL_DURUM_CALISYOR);
        TEST_ASSERT(std::string(k.versiyon()) == "0.1.0");
    }
    /* RAII: yıkıcı kernelKapat'ı çağırır */
    TEST_GECTI();
}

static void test_ozel_ayarlar() {
    TEST_BASLAT(ozel_ayarlar);

    KernelAyarlar ayarlar{};
    ayarlar.maks_is_parcacigi  = 2;
    ayarlar.bellek_havuzu_kb   = 512;
    ayarlar.zaman_asimi_ms     = 1000;
    ayarlar.hata_ayiklama_modu = 1;
    ayarlar.log_seviye         = LOG_SEVIYE_DEBUG;

    Kernel k(ayarlar);
    TEST_ASSERT(k.durum() == KERNEL_DURUM_CALISYOR);
    TEST_GECTI();
}

static void test_durum_bilgisi() {
    TEST_BASLAT(durum_bilgisi);

    Kernel k;
    KernelDurumBilgisi bilgi = k.durumBilgisi();

    TEST_ASSERT(bilgi.durum              == KERNEL_DURUM_CALISYOR);
    TEST_ASSERT(bilgi.aktif_gorev_sayisi == 0);
    TEST_ASSERT(bilgi.hata_sayisi        == 0);
    TEST_GECTI();
}

static KernelHataKodu basit_gorev(void* veri) {
    int* sayac = (int*)veri;
    (*sayac)++;
    return KERNEL_BASARILI;
}

static void test_gorev_planlama() {
    TEST_BASLAT(gorev_planlama);

    Kernel k;
    int sayac = 0;

    KernelGorevId id = k.gorevPlanlaC(basit_gorev, &sayac);
    TEST_ASSERT(id > 0);

    /* Görevi doğrudan çalıştır (ana döngü olmadan) */
    basit_gorev(&sayac);
    TEST_ASSERT(sayac == 1);

    TEST_GECTI();
}

static void test_cpp_lambda_gorevi() {
    TEST_BASLAT(cpp_lambda_gorevi);

    Kernel k;
    bool   calistirildi = false;

    KernelGorevId id = k.gorevPlanla([&]() -> KernelHataKodu {
        calistirildi = true;
        return KERNEL_BASARILI;
    });

    TEST_ASSERT(id > 0);
    /* Lambda kernel kuyruğuna eklendi — doğrudan da çağırabiliriz */
    TEST_GECTI();
}

static void test_bellek_ayirma() {
    TEST_BASLAT(bellek_ayirma);

    Kernel k;

    void* ptr1 = k.bellekAyir(64);
    TEST_ASSERT(ptr1 != nullptr);

    void* ptr2 = k.bellekAyir(128);
    TEST_ASSERT(ptr2 != nullptr);
    TEST_ASSERT(ptr1 != ptr2);

    /* Adreslerin ayrı olduğunu kontrol et */
    memset(ptr1, 0xAA, 64);
    memset(ptr2, 0xBB, 128);
    TEST_ASSERT(((uint8_t*)ptr1)[0] == 0xAA);
    TEST_ASSERT(((uint8_t*)ptr2)[0] == 0xBB);

    TEST_GECTI();
}

static void test_log_geri_cagri() {
    TEST_BASLAT(log_geri_cagri);

    int log_sayisi = 0;

    auto geri_cagri = [](LogSeviye, const char*, void* veri) {
        (*static_cast<int*>(veri))++;
    };

    Kernel k;
    k.logGeriCagriKaydet(
        [](LogSeviye s, const char* m, void* v) {
            (*static_cast<int*>(v))++;
        },
        &log_sayisi
    );

    k.log(LOG_SEVIYE_BILGI, "Test mesajı");
    k.log(LOG_SEVIYE_UYARI, "Test uyarı");

    TEST_ASSERT(log_sayisi == 2);
    TEST_GECTI();
}

static void test_c_api_uyumlulugu() {
    TEST_BASLAT(c_api_uyumlulugu);

    /* Orijinal API'nin düzgün çalıştığını doğrula */
    initializeKernel();
    TEST_ASSERT(kernelDurumAl() == KERNEL_DURUM_CALISYOR);

    const char* v = kernelVersiyonAl();
    TEST_ASSERT(v != nullptr);
    TEST_ASSERT(strcmp(v, "0.1.0") == 0);

    shutdownKernel();
    TEST_ASSERT(kernelDurumAl() == KERNEL_DURUM_KAPATILDI);

    TEST_GECTI();
}

/* ─── Ana Giriş ───────────────────────────────────────────── */

int main() {
    std::cout << "=== SimpleOS Kernel Entegrasyon Testleri ===\n";
    std::cout << "Versiyon: " << KERNEL_MAJOR_VERSIYON << "."
              << KERNEL_MINOR_VERSIYON << "." << KERNEL_YAMA_VERSIYON << "\n";

    test_baslat_kapat();
    test_ozel_ayarlar();
    test_durum_bilgisi();
    test_gorev_planlama();
    test_cpp_lambda_gorevi();
    test_bellek_ayirma();
    test_log_geri_cagri();
    test_c_api_uyumlulugu();

    std::cout << "\n\n=== Sonuç: " << gecen_sayisi
              << "/" << test_sayisi << " test geçti ===\n";

    return (gecen_sayisi == test_sayisi) ? 0 : 1;
}
