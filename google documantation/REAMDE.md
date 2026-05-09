# ChromiumDocs

Google Docs benzeri, Chromium'da tam çalışan belge editörü.
Mevcut `googledocs.html` + `script.js` + `styles.css` sisteminin üretim kalitesine yükseltilmiş hali.

## Dosya Yapısı

```
chromium-docs/
├── index.html          ← Ana giriş noktası (tarayıcıda aç)
└── src/
    ├── css/
    │   └── styles.css  ← Tam Google Material Design teması
    └── js/
        └── script.js   ← Editör motoru
```

## Özellikler

### Araç Çubuğu
- Yazı tipi & boyutu değiştirici
- Kalın / İtalik / Altı çizili / Üstü çizili
- Metin rengi & vurgu rengi seçici
- Sola / Ortaya / Sağa / İki yana hizalama
- Madde işareti & numaralı liste
- Girinti artır / azalt
- Başlık stilleri (H1–H4, Alıntı, Kod)
- Bağlantı ekleme
- Tablo ekleme (3×3)
- Yatay çizgi
- Biçim temizleme
- Geri al / Yinele
- Zoom +/−/%100

### Menü Çubuğu
- Dosya: Yeni, Aç, Kaydet, HTML indir, Yazdır
- Düzenle: Kes/Kopyala/Yapıştır, Seçtümü, Bul
- Görüntüle: Zoom, Kenar çubuğu
- Ekle: Bağlantı, Tablo, Çizgi, Kod
- Biçim: Tüm metin biçimleri
- Araçlar: Kelime sayısı istatistikleri

### Sol Kenar Çubuğu
- **Anahat** sekmesi: Başlıkları otomatik listeler, tıklayınca kaydır
- **İstatistik** sekmesi: Kelime, karakter, paragraf, okuma süresi

### Bul & Değiştir
- Ctrl+F ile aç/kapat
- Canlı vurgu (sarı), aktif sonuç (turuncu)
- ↑↓ ile gezinme, Enter / Shift+Enter

### Klavye Kısayolları
| Kısayol       | İşlev              |
|---------------|--------------------|
| Ctrl+B        | Kalın              |
| Ctrl+I        | İtalik             |
| Ctrl+U        | Altı çizili        |
| Ctrl+Z        | Geri al            |
| Ctrl+Y        | Yinele             |
| Ctrl+F        | Bul                |
| Ctrl+S        | Kaydet (simülasyon)|
| Ctrl+P        | Yazdır             |
| Ctrl+\        | Kenar çubuğu       |

## Kullanım

```bash
# 1. Klasörü aç
cd chromium-docs

# 2. index.html dosyasını Chromium / Chrome / Edge ile aç
# (Dosya çift tıklama veya)
chromium index.html
# veya
google-chrome index.html
```

> **Not:** `contenteditable` API en iyi Chromium tabanlı tarayıcılarda çalışır.
> Firefox'ta bazı `execCommand` davranışları farklı olabilir.

## Orijinal Dosyalardan Farklar

| Konu | Orijinal | ChromiumDocs |
|------|----------|--------------|
| Araç sayısı | 3 (bold/italic/underline) | 25+ araç |
| CSS | ~40 satır basit stil | ~500 satır Material Design teması |
| JS | Yarım kalmış API tab kodu | ~350 satır tam editör motoru |
| Menü | Yok | 8 menü, 30+ eylem |
| Anahat | Yok | Otomatik güncellenen H1-H4 listesi |
| İstatistik | Yok | Canlı kelime/karakter/okuma süresi |
| Bul | Yok | Canlı vurgulu arama + gezinme |
| Zoom | Yok | %50–%200 arası zoom |
| Dışa aktar | Yok | HTML indirme |
