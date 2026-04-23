# SimpleOS Chromium Dokümantasyon Sistemi - Mimari

## 🏗️ Sistem Mimarisi

```
┌─────────────────────────────────────────────────────────────┐
│                    Electron Main Process                     │
│           (electron_main.js - Node.js Ortamı)               │
├─────────────────────────────────────────────────────────────┤
│  • PHP Sunucusu Yönetimi (127.0.0.1:8000)                  │
│  • Pencere ve Uygulama Yaşam Döngüsü                        │
│  • Sistem Entegrasyonu (Dosya, Diyalog, Bildirim)          │
│  • IPC İletişim Koordinatörü                               │
└────────────┬────────────────────────────────┬───────────────┘
             │                                │
     ┌───────▼────────┐            ┌──────────▼──────────┐
     │  IPC Bridge    │            │  PHP API Server     │
     │ (preload.js)   │            │ (127.0.0.1:8000)    │
     │  Güvenlik     │            │                     │
     │  Köprüsü      │            │ • docsservices.php  │
     └───────┬────────┘            │ • google_docs_*.php │
             │                     │ • platform_*.php    │
     ┌───────▼─────────────────────▼──────────────────┐
     │   Renderer Process (Chromium)                  │
     │          (index.html)                          │
     ├──────────────────────────────────────────────┤
     │                                               │
     │  ┌─────────────────────────────────────────┐ │
     │  │      chromium_app.js (Frontend)         │ │
     │  │                                         │ │
     │  │  • DocumentViewer                       │ │
     │  │  • GoogleIntegrationPanel               │ │
     │  │  • DocumentManager                      │ │
     │  │  • ChromiumAPIClient                    │ │
     │  └─────────────────────────────────────────┘ │
     │                    ▲                          │
     │                    │                          │
     │         Fetch API / XHR (HTTP)               │
     │                    │                          │
     └────────────────────┼──────────────────────────┘
                          │
              ┌───────────▼────────────┐
              │   Google APIs          │
              ├────────────────────────┤
              │ • Google Docs API      │
              │ • Google Drive API     │
              │ • Google Sheets API    │
              │ • Google Slides API    │
              └────────────────────────┘
```

## 📦 Bileşenler

### 1. **electron_main.js** (Electron Ana İşlem)

#### Sorumluluğu
- Chromium penceresini oluşturma ve yönetme
- PHP sunucusunu başlatma/durdurma
- IPC olaylarını işleme
- Sistem dosya operasyonları
- Menü ve kısayol tuşları

#### Önemli Fonksiyonlar
```javascript
createWindow()              // Uygulama penceresini oluştur
startPHPServer()           // PHP API sunucusu başlat
stopPHPServer()            // PHP sunucuyu kapat
handleImportDocs()         // Doku içe aktarma diyaloğu
handleExportDocs(format)   // Doku ihraç işlemi
```

#### IPC Endpoints
```
import-google-docs         → Google Docs'dan dönüştür
sync-google-drive          → Drive klasörünü senkronize et
sync-google-sheet          → Sheet'i senkronize et
search-docs                → Belgeler içinde ara
export-docs                → Dosya olarak dışa aktar
get-config / update-config → Yapılandırma yönetimi
check-doc-update           → Belge güncelleme kontrolü
get-revision-history       → Sürüm geçmişi
```

### 2. **preload.js** (IPC Güvenlik Köprüsü)

#### Amaç
- Renderer işlemini Main işlemden izole etme
- contextBridge ile güvenli API sunma
- Node.js risklerinden koruma

#### Sunulan API'lar
```javascript
window.electron.ipcRenderer   // IPC iletişimi
window.app                    // Uygulama meta verileri
window.electronUtils          // Sistem araçları
window.clipboard              // Pano işlemleri
window.storage               // Güvenli depolama
window.notify               // Bildirim sistemi
window.files                // Dosya operasyonları
window.network              // Ağ durumu
```

### 3. **chromium_app.js** (Frontend İşlemi)

#### Bileşenleri

**ChromiumAPIClient**
- HTTP isteklerini yönetme
- Yanıt önbellekleme
- Hata yönetimi

**DocumentViewer**
- Belge yükleme ve görüntüleme
- HTML/Markdown/PDF işleme
- Sözdizimi vurgulama

**GoogleIntegrationPanel**
- İçe aktarma diyaloğu
- Senkronizasyon diyaloğu
- Arama diyaloğu

**DocumentManager**
- Belge listesi yönetimi
- Yerel depolama (localStorage)
- Güncelleme kontrolü

#### Veri Akışı
```
User Action (Button Click)
         ↓
Diyalog Oluştur (createModal)
         ↓
Form Gönder (form submit)
         ↓
API İsteği (ChromiumAPIClient.request)
         ↓
PHP Sunucusuna HTTP (fetch)
         ↓
Google API'lara İstek (PHP'de)
         ↓
Sonuç İşleme
         ↓
UI Güncelleme (render)
         ↓
Bildirim Göster (showNotification)
```

### 4. **google_docs_integration.php** (Google Docs API)

#### Sınıflar

**GoogleDocsIntegration**
```php
// Başlıca metodlar
exportGoogleDoc($docId, $format)        // Belgeyi dışa aktar
getDocMetadata($docId)                  // Belge bilgisi
listDocsInFolder($folderId)             // Klasördeki belgeler
batchExportDocs($folderId, $format)     // Toplu dışa aktarım
searchInDocs($query, $folderId)         // Belge içinde arama
checkDocumentUpdates($docId)            // Güncelleme kontrolü
getRevisionHistory($docId, $max)        // Sürüm geçmişi
```

**GoogleDocsAPIHandler**
```php
// REST endpoint yönlendiricisi
handleRequest($action, $params)
// Aksiyon örnekleri: export, metadata, list, batch-export, etc.
```

### 5. **docsservices.php** (REST API)

#### Endpointler

| Endpoint | Metod | İşlev |
|----------|-------|-------|
| `/api/sections` | GET | Tüm bölümleri listele |
| `/api/section/{id}` | GET | Bölüm içeriğini getir |
| `/api/section/{id}` | PUT | Bölümü güncelle |
| `/api/search` | GET | Belgeler içinde ara |
| `/api/health` | GET | Sistem sağlığı |
| `/api/export/{format}` | GET | Dışa aktar |

#### Veri Yapısı
```json
{
  "version": "1.0.0",
  "lastUpdated": "2024-04-22T10:00:00Z",
  "sections": {
    "overview": {
      "title": "Genel Bakış",
      "content": "...",
      "lastModified": "2024-04-22T10:00:00Z"
    }
  }
}
```

## 🔄 İşlem Akışları

### Akış 1: Google Docs İçe Aktarma

```
Kullanıcı: "İçe Aktar" Butonuna Tıkla
    ↓
chromium_app.js: openImportDialog()
    ↓
UI: Modal Diyalog Göster (Doc ID Input)
    ↓
Kullanıcı: Doc ID Gir ve Gönder
    ↓
chromium_app.js: documentViewer.loadDocument(docId, format)
    ↓
ShowLoadingSpinner(true)
    ↓
ChromiumAPIClient.request('/google-docs/export?doc_id=...')
    ↓
HTTP GET isteği gönder
    ↓
electron_main.js: Fetch API → PHP Sunucusuna
    ↓
google_docs_integration.php: exportGoogleDoc()
    ↓
Google Docs API: OAuth Token ile istek
    ↓
Google: Belgeyi dönüştür → HTML/Markdown
    ↓
PHP: Sonucu JSON olarak döndür
    ↓
chromium_app.js: Sonuç işle
    ↓
DocumentViewer.render(content, format)
    ↓
ShowNotification("Document loaded", "success")
    ↓
addToRecentDocs({ id, title })
    ↓
Kullanıcı: Belgeyi Görüntüle
```

### Akış 2: Google Drive Senkronizasyonu

```
Kullanıcı: "Senkronize Et" Butonuna Tıkla
    ↓
chromium_app.js: openSyncDialog()
    ↓
UI: Modal Diyalog Göster (Folder ID Input)
    ↓
Kullanıcı: Folder ID Gir ve Gönder
    ↓
GoogleIntegrationPanel.syncFolder(folderId, format)
    ↓
ShowLoadingSpinner(true)
    ↓
ChromiumAPIClient.request('/google-docs/batch-export', POST)
    ↓
POST isteği gönder: { folder_id: ..., format: ... }
    ↓
google_docs_integration.php: batchExportDocs()
    ↓
listDocsInFolder() → Google Drive API
    ↓
Her belge için:
  - exportGoogleDoc() → Google Docs API
  - Dosya olarak kaydet → cache/
    ↓
Sonuçları JSON döndür
    ↓
chromium_app.js: displaySyncResults(results)
    ↓
ShowNotification("Synced: X documents", "success")
    ↓
UI: Senkronize edilen dosyaları göster
```

### Akış 3: Arama İşlemi

```
Kullanıcı: "Ara" Butonuna Tıkla
    ↓
chromium_app.js: openSearchDialog()
    ↓
UI: Arama Terimi Gir
    ↓
Kullanıcı: Arama Formu Gönder
    ↓
GoogleIntegrationPanel.performSearch(query, folderId)
    ↓
ChromiumAPIClient.request('/google-docs/search?query=...')
    ↓
HTTP GET isteği
    ↓
google_docs_integration.php: searchInDocs(query, folderId)
    ↓
Google Drive API: Full-text search
    ↓
Sonuçları işle ve JSON döndür
    ↓
chromium_app.js: displaySearchResults(results)
    ↓
UI: Sonuçları tablo olarak göster
    ↓
Kullanıcı: Sonuçlardan birini seç
    ↓
documentViewer.loadDocument(docId)
    ↓
(Akış 1'e geri dön)
```

## 📊 Veri Modelleri

### Belge Nesnesi (Document Object)

```javascript
{
    id: "string",              // Google Doc ID
    title: "string",           // Belge başlığı
    timestamp: number,         // Eklenme zamanı
    lastModified: number,      // Son değişiklik zamanı
    hasUpdate: boolean,        // Güncelleme var mı?
    content: "string",         // Belge içeriği (yüklendikten sonra)
    format: "html|markdown"    // İçerik formatı
}
```

### Senkronizasyon Sonucu (Sync Result)

```javascript
{
    total: number,             // Toplam belge sayısı
    exported: number,          // İhraç edilen sayı
    failed: number,            // Başarısız sayı
    files: [
        {
            name: "string",    // Belge adı
            file: "string",    // Dosya adı
            size: number       // Dosya boyutu (byte)
        }
    ]
}
```

### Arama Sonucu (Search Result)

```javascript
{
    query: "string",           // Arama terimi
    count: number,             // Sonuç sayısı
    results: [
        {
            id: "string",      // Doc ID
            name: "string",    // Belge adı
            snippet: "string"  // İçerik parçacığı
        }
    ]
}
```

## 🔐 Güvenlik Modeli

### IPC İletişim Güvenliği

```javascript
// Main Process → Renderer
ipcMain.handle('import-google-docs', async (event, params) => {
    // Parametre doğrulaması
    if (!params.docId || typeof params.docId !== 'string') {
        throw new Error('Invalid docId');
    }
    
    // İş mantığı
    // ...
    
    return result;
});

// Renderer → Main
window.electron.ipcRenderer.invoke('import-google-docs', { 
    docId: 'validated-id',
    format: 'html'
})
```

### OAuth 2.0 Akışı

```
Kullanıcı: Oturum Aç
    ↓
electron_main.js: getAuthUrl()
    ↓
Tarayıcıya Gönder: Google OAuth URL
    ↓
Google: Oturum Aç ve Yetkilendir
    ↓
Geri Yönlendirme: localhost:8000/callback?code=...
    ↓
google_integration.php: handleCallback()
    ↓
Token Değişim: code → access_token
    ↓
Token Kaydetme: Güvenli konuma
    ↓
Sonraki İstekler: Token ile Google API çağrıları
```

### Veri Şifreleme

- Depolanan tokenler: Şifrelenmemiş (üretim için düzeltilmesi gerekir)
- İletişim: HTTPS (üretimde zorunlu)
- Cache: Yerel dosyalar (güvenlidir)

## ⚡ Performans Optimizasyonları

### 1. Önbellekleme Stratejisi

```javascript
class ChromiumAPIClient {
    cache: Map<string, CachedData>
    cacheExpiry: 3600000  // 1 saat
    
    async request(endpoint, options) {
        const cached = this.cache.get(key);
        if (cached && !expired) {
            return cached.data;  // Önbellekten döndür
        }
        
        const data = await fetch(...)
        this.cache.set(key, { data, timestamp });
        return data;
    }
}
```

### 2. Toplu İşlemler

```php
// batchExportDocs: Paralel dışa aktarım
foreach ($docs as $doc) {
    // Eşzamanlı işlemler
    curl_multi_* işlevleri
}
```

### 3. Lazy Loading

```javascript
// Belgeler sadece açıldığında yüklenir
DocumentViewer.loadDocument(docId)
// Diğer belgeler arka planda hazırlanabilir
```

## 🛠️ Geliştirme Rehberi

### Yeni Özellik Ekleme

1. **API Endpoint Ekle**
   - `google_docs_integration.php` → Yeni metod
   - `docsservices.php` → Route tanımlama

2. **IPC Handler Ekle**
   - `electron_main.js` → ipcMain.handle()

3. **UI Bileşeni Ekle**
   - `chromium_app.js` → Yeni sınıf/metod
   - `index.html` → Öğe ekleme

4. **Test Et**
   - `npm run electron-start`
   - Tarayıcı DevTools

### Hata Ayıklama

```bash
# Verbose logging
export DEBUG=simpleos:*
npm run electron-start

# DevTools aç
Ctrl+Shift+I (Windows/Linux)
Cmd+Option+I (macOS)

# PHP hataları
tail -f logs/php.log

# API yanıtları kontrol et
curl http://127.0.0.1:8000/api/health
```

## 📈 İzleme ve Telemetri

Yapılandırma dosyasında `/analytics` bölümü:

```json
{
  "analytics": {
    "enabled": false,
    "trackingId": ""
  }
}
```

Üretim için etkinleştirilebilir.

## 🚀 Dağıtım

### Electron Builder Konfigürasyonu

```json
"build": {
  "appId": "com.simpleos.docs",
  "productName": "SimpleOS Docs Viewer",
  "files": ["..."],
  "win": { "target": ["nsis", "portable"] },
  "linux": { "target": ["AppImage", "deb"] },
  "mac": { "target": ["dmg", "zip"] }
}
```

### Kurulum Paketleri

```bash
npm run build-windows   # → simpleos-docs-setup.exe
npm run build-linux     # → simpleos-docs.AppImage
npm run build-mac       # → SimpleOS Docs Viewer.dmg
```

---

**Doküman Sürümü**: 1.0.0  
**Son Güncelleme**: 22 Nisan 2026
