'use strict';

// Kernel çalıştırma durumları
const KernelDurum = {
  HAZIR: 'hazir',
  CALISYOR: 'calisyor',
  MESGUL: 'mesgul',
  HATA: 'hata',
  KAPATILDI: 'kapatildi',
};

// Varsayılan kernel yapılandırması
const varsayilanAyarlar = {
  zaman_asimi: 30000,      // ms cinsinden maksimum çalışma süresi
  bellek_siniri: 128,      // MB cinsinden bellek sınırı
  izin_verilen_moduller: [], // izin verilen modüller listesi
  hata_ayiklama: false,    // debug modu
};

class Kernel {
  /**
   * @param {object} ayarlar - Kernel yapılandırma seçenekleri
   */
  constructor(ayarlar = {}) {
    this.ayarlar = { ...varsayilanAyarlar, ...ayarlar };
    this.durum = KernelDurum.HAZIR;
    this.aktif_gorevler = new Map();
    this.olay_dinleyicileri = new Map();
    this._gorev_sayaci = 0;
  }

  // ─── Durum Yönetimi ───────────────────────────────────────────

  /**
   * Mevcut kernel durumunu döner
   */
  durumAl() {
    return this.durum;
  }

  /**
   * Kernel'in çalışır durumda olup olmadığını kontrol eder
   */
  aktifMi() {
    return this.durum !== KernelDurum.KAPATILDI;
  }

  // ─── Görev Yönetimi ───────────────────────────────────────────

  /**
   * Sandboxed ortamda kod çalıştırır
   * @param {Function|string} kod - Çalıştırılacak fonksiyon veya kod dizisi
   * @param {object} [baglam={}] - Koda aktarılacak bağlam değişkenleri
   * @returns {Promise<object>} - Çalıştırma sonucu
   */
  async calistir(kod, baglam = {}) {
    if (!this.aktifMi()) {
      throw new KernelHatasi('Kernel kapatılmış, yeni görev başlatılamaz', 'KAPATILDI');
    }

    const gorev_id = this._gorevIdUret();
    const baslangic = Date.now();

    this.durum = KernelDurum.CALISYOR;
    this._olayCalistir('gorev:basladi', { gorev_id, baslangic });

    const gorev = {
      id: gorev_id,
      baslangic,
      durum: 'calisyor',
      iptal_kontrolu: new AbortController(),
    };

    this.aktif_gorevler.set(gorev_id, gorev);

    try {
      const sonuc = await this._zamanAsimliCalistir(kod, baglam, gorev);
      const sure = Date.now() - baslangic;

      gorev.durum = 'tamamlandi';
      this._olayCalistir('gorev:tamamlandi', { gorev_id, sure, sonuc });

      return {
        basarili: true,
        gorev_id,
        sonuc,
        sure_ms: sure,
        bellek_kullanimi: this._bellekOku(),
      };
    } catch (hata) {
      const sure = Date.now() - baslangic;

      gorev.durum = 'hata';
      this.durum = KernelDurum.HATA;
      this._olayCalistir('gorev:hata', { gorev_id, sure, hata: hata.message });

      return {
        basarili: false,
        gorev_id,
        hata: hata.message,
        hata_turu: hata.constructor.name,
        sure_ms: sure,
      };
    } finally {
      this.aktif_gorevler.delete(gorev_id);
      if (this.durum === KernelDurum.CALISYOR) {
        this.durum = KernelDurum.HAZIR;
      }
    }
  }

  /**
   * Çalışmakta olan bir görevi iptal eder
   * @param {string} gorev_id - İptal edilecek görev ID'si
   */
  iptalEt(gorev_id) {
    const gorev = this.aktif_gorevler.get(gorev_id);
    if (!gorev) {
      throw new KernelHatasi(`Görev bulunamadı: ${gorev_id}`, 'GOREV_YOK');
    }
    gorev.iptal_kontrolu.abort();
    gorev.durum = 'iptal';
    this._olayCalistir('gorev:iptal', { gorev_id });
  }

  /**
   * Tüm aktif görevleri listeler
   */
  aktifGorevler() {
    return Array.from(this.aktif_gorevler.values()).map(({ id, baslangic, durum }) => ({
      id,
      baslangic,
      durum,
      gecen_ms: Date.now() - baslangic,
    }));
  }

  // ─── Olay Sistemi ─────────────────────────────────────────────

  /**
   * Kernel olaylarına dinleyici ekler
   * @param {string} olay - Olay adı
   * @param {Function} geri_cagri - Tetiklenecek fonksiyon
   */
  dinle(olay, geri_cagri) {
    if (!this.olay_dinleyicileri.has(olay)) {
      this.olay_dinleyicileri.set(olay, []);
    }
    this.olay_dinleyicileri.get(olay).push(geri_cagri);
    return () => this.dinlemeyi_birak(olay, geri_cagri); // abonelikten çıkma fonksiyonu
  }

  /**
   * Belirli bir olayın dinleyicisini kaldırır
   */
  dinlemeyi_birak(olay, geri_cagri) {
    const dinleyiciler = this.olay_dinleyicileri.get(olay);
    if (dinleyiciler) {
      const index = dinleyiciler.indexOf(geri_cagri);
      if (index !== -1) dinleyiciler.splice(index, 1);
    }
  }

  // ─── Yaşam Döngüsü ────────────────────────────────────────────

  /**
   * Kernel'i temiz biçimde kapatır
   * @param {boolean} [zorla=false] - Aktif görevleri zorla sonlandır
   */
  async kapat(zorla = false) {
    if (this.durum === KernelDurum.KAPATILDI) return;

    if (!zorla && this.aktif_gorevler.size > 0) {
      throw new KernelHatasi(
        `${this.aktif_gorevler.size} aktif görev var. Zorla kapatmak için kapat(true) kullanın.`,
        'AKTIF_GOREV'
      );
    }

    // Tüm aktif görevleri iptal et
    for (const [id] of this.aktif_gorevler) {
      this.iptalEt(id);
    }

    this.durum = KernelDurum.KAPATILDI;
    this._olayCalistir('kernel:kapatildi', { zaman: Date.now() });

    if (this.ayarlar.hata_ayiklama) {
      console.log('[Kernel] Kernel başarıyla kapatıldı');
    }
  }

  // ─── Özel Yardımcı Metodlar ───────────────────────────────────

  async _zamanAsimliCalistir(kod, baglam, gorev) {
    const { zaman_asimi } = this.ayarlar;

    const calistirma_vaadi = typeof kod === 'function'
      ? Promise.resolve().then(() => kod(baglam))
      : Promise.resolve().then(() => this._kodDegerlendirmesi(kod, baglam));

    const zaman_asimi_vaadi = new Promise((_, reddet) => {
      const zamanlayici = setTimeout(() => {
        reddet(new KernelHatasi(`Zaman aşımı: ${zaman_asimi}ms`, 'ZAMAN_ASIMI'));
      }, zaman_asimi);

      gorev.iptal_kontrolu.signal.addEventListener('abort', () => {
        clearTimeout(zamanlayici);
        reddet(new KernelHatasi('Görev iptal edildi', 'IPTAL'));
      });
    });

    return Promise.race([calistirma_vaadi, zaman_asimi_vaadi]);
  }

  _kodDegerlendirmesi(kod_dizisi, baglam) {
    // Gerçek ortamda burada güvenli sandbox kurulumu yapılır
    // (örn. vm2, isolated-vm, WebWorker, vs.)
    const parametreler = Object.keys(baglam);
    const degerler = Object.values(baglam);
    // eslint-disable-next-line no-new-func
    const fonksiyon = new Function(...parametreler, kod_dizisi);
    return fonksiyon(...degerler);
  }

  _gorevIdUret() {
    return `gorev_${Date.now()}_${++this._gorev_sayaci}`;
  }

  _bellekOku() {
    if (typeof process !== 'undefined' && process.memoryUsage) {
      const kullanim = process.memoryUsage();
      return Math.round(kullanim.heapUsed / 1024 / 1024 * 100) / 100; // MB
    }
    return null;
  }

  _olayCalistir(olay, veri) {
    const dinleyiciler = this.olay_dinleyicileri.get(olay) || [];
    dinleyiciler.forEach(fn => {
      try { fn(veri); } catch (e) { /* dinleyici hatalarını yutma */ }
    });

    if (this.ayarlar.hata_ayiklama) {
      console.log(`[Kernel:${olay}]`, veri);
    }
  }
}

// ─── Özel Hata Sınıfı ─────────────────────────────────────────

class KernelHatasi extends Error {
  constructor(mesaj, kod = 'BILINMEYEN') {
    super(mesaj);
    this.name = 'KernelHatasi';
    this.kod = kod;
  }
}

// ─── Fabrika Fonksiyonu ───────────────────────────────────────

/**
 * Yeni bir Kernel örneği oluşturur ve hazır hale getirir
 * @param {object} ayarlar - Yapılandırma seçenekleri
 * @returns {Kernel}
 */
function kernelOlustur(ayarlar = {}) {
  return new Kernel(ayarlar);
}

module.exports = {
  Kernel,
  KernelHatasi,
  KernelDurum,
  kernelOlustur,
};