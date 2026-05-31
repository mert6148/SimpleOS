# SimpleOS PHP Package Manager

Bu klasör sistem paketlerini yönetmek için PHP kaynak dizinleri ve temel paket
yönetimi altyapısı içerir.

## Yapı

- `composer.json` - PSR-4 autoload desteği
- `package.php` - CLI giriş noktası
- `src/bootstrap.php` - sınıf otomatik yükleme
- `src/App/Script/Package/` - paket modeli, depo, yönetici ve kurucu sınıflar

## Kullanım

```bash
cd "script/package"
php package.php list
php package.php install system-packages
php package.php info kernel-headers
```

## Paketler

- `core-utils`
- `kernel-headers`
- `shell-tools`
- `system-packages`

## Uygulama Paketleri

Bu paket sistemi, SimpleOS için kullanıcı uygulamalarını yönetir.

- `text-editor` - Minimal düzenleyici uygulaması
- `file-manager` - Dosya yöneticisi uygulaması
- `process-viewer` - Çalışan uygulamaları izleyen araç

### Kullanım

```bash
php package.php app list
php package.php app install text-editor
php package.php app info file-manager
php package.php app run text-editor -- example.txt
php package.php app remove process-viewer
```
