# SimpleOS Chromium Dokümantasyon Sistemi
# Google Dokümanlara Entegrasyon

## 📋 Genel Bakış

SimpleOS Chromium Dokümantasyon Sistemi, Google Docs, Sheets ve Slides ile tam entegrasyon sunan modern bir masaüstü uygulamasıdır. Electron ve Chromium üzerine kurulu bu sistem, çevrimdışı çalışmayı, gerçek zamanlı senkronizasyonu ve kapsamlı dökümantasyon yönetimini destekler.

### 🎯 Temel Özellikleri

- **Google Docs Entegrasyonu**: Belgelerinizi doğrudan Google Drive'dan içe aktarın
- **Çoklu Format Desteği**: HTML, Markdown, PDF, JSON formatlarında ihracat
- **Gerçek Zamanlı Senkronizasyon**: Google Drive klasörlerini otomatik senkronize edin
- **Google Sheets Desteği**: Spreadsheet verilerini JSON olarak eşitleyin
- **Google Slides Yönetimi**: Sunuları görüntülerin ve dışa aktarın
- **Çevrimdışı Modu**: Önbelleklenmiş belgelerle internet olmadan çalışın
- **Koyu Tema**: Gece modu desteği
- **Arama Fonksiyonu**: Tüm senkronize belgeler içinde arama yapın
- **Sürüm Geçmişi**: Google Drive revizyon geçmişini görüntüleyin

## 🚀 Kurulum

### Sistem Gereksinimleri

- Node.js 14.0+ ve npm
- PHP 7.4+ (API sunucusu için)
- Windows 10+, macOS 10.15+ veya Ubuntu 20.04+
- Google API anahtarları

### 1. Temel Kurulum

```bash
# Depo klonlayın
git clone https://github.com/yourusername/simpleos-docs.git
cd simpleos-docs

# Bağımlılıkları yükleyin
npm install

# PHP API sunucusu başlatın (ayrı terminal)
npm run php-server

# Electron uygulamasını geliştirme modunda çalıştırın
npm run electron-start
```

### 2. Google API Konfigürasyonu

#### Google Cloud Console'da Proje Oluşturun

1. [Google Cloud Console](https://console.cloud.google.com) ziyaret edin
2. Yeni bir proje oluşturun
3. Aşağıdaki API'ları etkinleştirin:
   - Google Drive API
   - Google Docs API
   - Google Sheets API
   - Google Slides API

#### OAuth 2.0 Kimlik Bilgileri

1. **Kimlik Bilgileri** > **Oluştur** > **OAuth 2.0 istemci kimliği**
2. **Masaüstü uygulaması** seçin
3. İndirilen JSON dosyasından şunları kopyalayın:
   - `client_id`
   - `client_secret`

#### Ortam Değişkenlerini Ayarlayın

**.env dosyası oluşturun:**

```bash
GOOGLE_CLIENT_ID=your_client_id_here
GOOGLE_CLIENT_SECRET=your_client_secret_here
GOOGLE_CLOUD_PROJECT=your_project_id
```

### 3. Uygulamayı Derleyin

```bash
# Windows
npm run build-windows

# macOS
npm run build-mac

# Linux
npm run build-linux

# Tüm platformlar
npm run build
```

## 📁 Dosya Yapısı

```
simpleos-docs/
├── google_docs_integration.php      # Google Docs PHP API
├── google_integration.php           # Google Drive PHP API
├── docsservices.php                 # REST API sunucusu
├── platform_services.php            # Platform spesifik hizmetler
├── electron_main.js                 # Electron ana işlem
├── chromium_app.js                  # Chromium uygulaması
├── preload.js                       # IPC güvenlik köprüsü
├── electron_config.json             # Electron yapılandırması
├── index.html                       # Ana sayfa
├── package.json                     # Node bağımlılıkları
├── public/
│   ├── electron_main.js             # Paketlenmiş ana işlem
│   └── assets/
│       └── icon.png                 # Uygulama ikonu
├── build/                           # Derlenmiş uygulamayı
├── docs/                            # Dökümantasyon dosyaları
└── cache/                           # Önbellek klasörü
```

## 🔧 API Referansı

### Google Docs İçe Aktarma

```javascript
// Elektrondan
const result = await window.electron.ipcRenderer.importGoogleDocs(docId, 'html');

// Doğrudan API'dan
fetch('http://localhost:8000/api/google-docs/export?doc_id=ABC123&format=html')
  .then(res => res.json())
  .then(data => console.log(data.content));
```

### Google Drive Senkronizasyonu

```javascript
// Klasörü senkronize et
const result = await window.electron.ipcRenderer.syncGoogleDrive(folderId, 'html');

// Sonuç:
{
  total: 10,
  exported: 10,
  failed: 0,
  files: [
    { name: 'Dokument1', file: 'dokument1.html', size: 25600 }
  ]
}
```

### Google Sheets Senkronizasyonu

```javascript
// Spreadsheet'i senkronize et
const result = await window.electron.ipcRenderer.syncGoogleSheet(sheetId, 'Sheet1');

// Sonuç:
{
  success: true,
  rows: 100,
  data: [
    { name: 'Item1', value: 'Data1' },
    { name: 'Item2', value: 'Data2' }
  ]
}
```

### Dokümanda Arama

```javascript
// Belgeler içinde ara
const results = await window.electron.ipcRenderer.searchDocs('arama terimi', folderId);

// Sonuç:
{
  query: 'arama terimi',
  count: 5,
  results: [
    { id: 'doc1', name: 'Sonuç 1', snippet: '...' }
  ]
}
```

### İhracat

```javascript
// HTML olarak dışa aktar
const result = await window.electron.ipcRenderer.exportDocs(content, 'MyDocument', 'html');

// Markdown olarak dışa aktar
const result = await window.electron.ipcRenderer.exportDocs(content, 'MyDocument', 'markdown');

// PDF olarak dışa aktar
const result = await window.electron.ipcRenderer.exportDocs(content, 'MyDocument', 'pdf');
```

## 🎨 Tema Özelleştirmesi

### Koyu Mod Etkinleştirme

```javascript
// JavaScript'ten
document.documentElement.setAttribute('data-theme', 'dark');
localStorage.setItem('darkMode', 'true');

// HTML'den
<html data-theme="dark">
```

### CSS Değişkenlerini Özelleştirme

```css
:root {
    --primary: #667eea;
    --secondary: #764ba2;
    --success: #48bb78;
    --danger: #f56565;
}
```

## 🔐 Güvenlik

### IPC Güvenliği

Tüm IPC iletişimi `contextIsolation: true` ile korunur:

```javascript
// Preload.js tarafından güvenli şekilde sunulur
contextBridge.exposeInMainWorld('electron', {
    ipcRenderer: {
        importGoogleDocs: (docId, format) =>
            ipcRenderer.invoke('import-google-docs', { docId, format })
    }
});
```

### Veri Şifreleme

Depolanan veriler:

```javascript
// Yapılandırmada
"security": {
    "sandbox": true,
    "contentSecurityPolicy": true,
    "blockExternalResources": false
}
```

## 📊 Gelişmiş Kullanım

### Yapılandırma Dosyası (electron_config.json)

```json
{
  "cache": {
    "enabled": true,
    "maxSize": 104857600,
    "ttl": 3600000
  },
  "integrations": {
    "googleDocs": {
      "enabled": true,
      "autoFetch": true,
      "batchSize": 10
    }
  },
  "export": {
    "formats": {
      "html": { "minify": true, "includeCss": true },
      "pdf": { "format": "A4", "margin": "1cm" }
    }
  }
}
```

### Toplu Senkronizasyon

```javascript
// Otomatik senkronizasyon ayarlayın
const syncInterval = setInterval(async () => {
    const result = await window.electron.ipcRenderer.syncGoogleDrive(folderId, 'html');
    console.log(`Senkronize: ${result.exported} belge`);
}, 300000); // 5 dakikada bir
```

### Revizyon Geçmişi

```javascript
// Belgenin revizyon geçmişini al
const history = await window.electron.ipcRenderer.getRevisionHistory(docId, 10);

// Sonuç:
{
  success: true,
  revisions: [
    { id: 'rev1', modifiedTime: '2024-01-01T12:00:00Z' }
  ]
}
```

## 🐛 Sorun Giderme

### PHP Sunucusu Başlamıyor

```bash
# PHP yolunu kontrol et
which php  # macOS/Linux
where php  # Windows

# PHP sürümünü kontrol et (7.4+ gerekli)
php -v

# Port 8000 meşgul olabilir
php -S 127.0.0.1:8001
```

### Google API Kimlik Bilgileri Hatası

```bash
# .env dosyasını kontrol et
cat .env

# Ortam değişkenlerini ayarla
export GOOGLE_CLIENT_ID=your_id
export GOOGLE_CLIENT_SECRET=your_secret
```

### Önbellek Sorunları

```javascript
// Önbelleği temizle
localStorage.clear();
apiClient.clearCache();

// Yeniden yükle
location.reload();
```

## 📚 Örnekler

### Örnek 1: Belgelerden Veri İçe Aktarma

```javascript
// Google Drive klasöründeki tüm belgeleri indir
const result = await documentManager.syncFolder(folderId, 'html');

// Senkronize belgeler üzerinde işlem yap
documentManager.documents.forEach(doc => {
    console.log(`Belge: ${doc.title}`);
});
```

### Örnek 2: Arama ve Açma

```javascript
// Belgeleri ara
const results = await googlePanel.performSearch('API referansı', folderId);

// Sonuçlardan birini aç
results.forEach(doc => {
    documentViewer.loadDocument(doc.id, 'html');
});
```

### Örnek 3: Dönem Aralığında İhracat

```javascript
// Tüm belgeleri HTML olarak dışa aktar
documentManager.documents.forEach(doc => {
    documentViewer.loadDocument(doc.id, 'html').then(() => {
        documentViewer.export('html');
    });
});
```

## 🔄 Sürüm Yönetimi

```bash
# Güncelleme kontrol et
npm run check-updates

# Bağımlılıkları güncelle
npm update

# Electron'u güncelle
npm install electron@latest
```

## 📝 Kayıt

Uygulamanın logs klasöründe kayıtlar bulunur:

```
logs/
├── app.log           # Genel uygulama kayıtları
├── api.log           # API isteği kayıtları
├── sync.log          # Senkronizasyon kayıtları
└── error.log         # Hata kayıtları
```

## 🤝 Katkı

Sorunları bildirin veya özellik isteği gönderin:

1. GitHub'da issue açın
2. Değişikliklerinizi test edin
3. Pull request gönderin

## 📄 Lisans

MIT Lisansı - [LICENSE](LICENSE) dosyasına bakın

## 📞 İletişim

- **E-posta**: mert@example.com
- **Web**: https://simpleos.example.com
- **GitHub**: https://github.com/yourusername/simpleos-docs

---

**Sürüm**: 1.0.0  
**Son Güncelleme**: 22 Nisan 2026  
**Durum**: ✅ Üretim Hazır
