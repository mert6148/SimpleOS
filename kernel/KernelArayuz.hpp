/**
 * SimpleOS Kernel - C++ Sarmalayıcı
 * KernelArayuz.hpp
 *
 * Saf C kernel API'sini RAII prensipleriyle C++ sınıfına sarar.
 * Başlık sadece C++ modunda dahil edilebilir.
 */

#ifndef KERNEL_ARAYUZ_HPP
#define KERNEL_ARAYUZ_HPP

#include <stdexcept>
#include <string>
#include <functional>
#include <vector>
#include <memory>

extern "C" {
    #include "kernel.h"
}

namespace SimpleOS {

/* ─── Hata Sınıfı ─────────────────────────────────────────── */

class KernelIstisnasi : public std::runtime_error {
public:
    explicit KernelIstisnasi(KernelHataKodu kod, const std::string& mesaj = "")
        : std::runtime_error("[Kernel] " + mesaj + " (kod=" + std::to_string(kod) + ")")
        , hata_kodu_(kod) {}

    KernelHataKodu hataKoduAl() const { return hata_kodu_; }

private:
    KernelHataKodu hata_kodu_;
};

/* ─── Yardımcı: Hata Kontrolü ─────────────────────────────── */

inline void hataKontrolEt(KernelHataKodu kod, const std::string& islem) {
    if (kod != KERNEL_BASARILI) {
        throw KernelIstisnasi(kod, islem + " başarısız");
    }
}

/* ─── Görev Sarmalayıcı ───────────────────────────────────── */

/**
 * std::function görevini C uyumlu wrapper'a köprüler.
 * Heap üzerinde yaratılır; görev tamamlandığında silinir.
 */
struct GorevKoprusu {
    std::function<KernelHataKodu()> fonk;
};

inline KernelHataKodu _gorev_kopru_calistir(void* veri) {
    auto* kopru = static_cast<GorevKoprusu*>(veri);
    KernelHataKodu sonuc = kopru->fonk();
    delete kopru;   /* tek seferlik: tamamlandı, sil */
    return sonuc;
}

/* ─── Ana Kernel Sınıfı ───────────────────────────────────── */

class Kernel {
public:
    /* Varsayılan ayarlarla başlat */
    Kernel() {
        hataKontrolEt(kernelBaslat(nullptr), "kernelBaslat");
    }

    /* Özel ayarlarla başlat */
    explicit Kernel(const KernelAyarlar& ayarlar) {
        hataKontrolEt(kernelBaslat(&ayarlar), "kernelBaslat");
    }

    /* RAII: yıkıcı kernel'i kapatır */
    ~Kernel() noexcept {
        kernelKapat(1);   /* zorla kapat — istisnayı yok say */
    }

    /* Kopyalamaya ve atamaya izin verme (tekil kaynak) */
    Kernel(const Kernel&)            = delete;
    Kernel& operator=(const Kernel&) = delete;

    /* Taşımaya izin ver */
    Kernel(Kernel&&)            = default;
    Kernel& operator=(Kernel&&) = default;

    /* ─── Ana Döngü ─────────────────────────────────────── */

    /**
     * Bloklanarak kernel ana döngüsünü çalıştırır.
     * İçinden kernelKapat çağrısı yapıldığında döner.
     */
    void calistir() {
        kernelMain();
    }

    /* ─── Görev Yönetimi ────────────────────────────────── */

    /**
     * Lambda/fonksiyon nesnesini görev olarak planlar.
     * @param fonk      Çalıştırılacak std::function
     * @param oncelik   0-255 (255 = en yüksek)
     * @return Görev ID'si
     */
    KernelGorevId gorevPlanla(std::function<KernelHataKodu()> fonk,
                               uint8_t oncelik = 128) {
        auto* kopru = new GorevKoprusu{ std::move(fonk) };
        KernelGorevId id = 0;
        KernelHataKodu hata = kernelGorevPlanla(
            _gorev_kopru_calistir, kopru, oncelik, &id
        );
        if (hata != KERNEL_BASARILI) {
            delete kopru;
            throw KernelIstisnasi(hata, "gorevPlanla");
        }
        return id;
    }

    /**
     * Ham C fonksiyonunu görev olarak planlar.
     */
    KernelGorevId gorevPlanlaC(KernelGorevFonk fonk,
                                void*          veri     = nullptr,
                                uint8_t        oncelik  = 128) {
        KernelGorevId id = 0;
        hataKontrolEt(
            kernelGorevPlanla(fonk, veri, oncelik, &id),
            "gorevPlanlaC"
        );
        return id;
    }

    void gorevIptal(KernelGorevId id) {
        hataKontrolEt(kernelGorevIptal(id), "gorevIptal");
    }

    void gorevBekle(KernelGorevId id, uint32_t zaman_asimi = 0) {
        hataKontrolEt(kernelGorevBekle(id, zaman_asimi), "gorevBekle");
    }

    /* ─── Bellek ────────────────────────────────────────── */

    void* bellekAyir(size_t boyut) {
        void* ptr = kernelBellekAyir(boyut);
        if (!ptr) throw KernelIstisnasi(KERNEL_HATA_BELLEK, "bellekAyir");
        return ptr;
    }

    void bellekSerbest(void* ptr) noexcept {
        kernelBellekSerbest(ptr);
    }

    /* ─── Durum ─────────────────────────────────────────── */

    KernelDurum          durum()    const { return kernelDurumAl(); }
    std::string          versiyon() const { return kernelVersiyonAl(); }

    KernelDurumBilgisi   durumBilgisi() const {
        KernelDurumBilgisi bilgi{};
        hataKontrolEt(kernelDurumBilgisiAl(&bilgi), "durumBilgisiAl");
        return bilgi;
    }

    /* ─── Loglama ───────────────────────────────────────── */

    void logGeriCagriKaydet(KernelLogGeriCagri geri_cagri,
                             void*              kullanici_verisi = nullptr) {
        kernelLogGeriCagriKaydet(geri_cagri, kullanici_verisi);
    }

    void log(LogSeviye seviye, const std::string& mesaj) {
        kernelLog(seviye, "%s", mesaj.c_str());
    }

    /* ─── Kapatma ───────────────────────────────────────── */

    void kapat(bool zorla = false) {
        hataKontrolEt(kernelKapat(zorla ? 1 : 0), "kapat");
    }
};

/* ─── Kolaylık: Paylaşılan Kernel ─────────────────────────── */

using KernelPtr = std::unique_ptr<Kernel>;

inline KernelPtr kernelOlustur() {
    return std::make_unique<Kernel>();
}

inline KernelPtr kernelOlustur(const KernelAyarlar& ayarlar) {
    return std::make_unique<Kernel>(ayarlar);
}

} /* namespace SimpleOS */

#endif /* KERNEL_ARAYUZ_HPP */
