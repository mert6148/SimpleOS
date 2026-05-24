# SimpleOS Architecture Detector

## Genel Bakış (Overview)

Bu proje, hem x86 (32-bit) hem de x64 (64-bit) sistemlerini destekleyen bir Python mimarisi algılama aracıdır. Windows ve Linux platformlarında çalışır.

**Features:**
- ✅ x86 ve x64 sistem tespiti
- ✅ Detaylı sistem bilgilerine erişim
- ✅ JSON çıkışı desteği
- ✅ İşletim Sistemi uyumluluğu (Windows, Linux, macOS)
- ✅ Python 3.6+ uyumlu

---

## Dosya Yapısı (File Structure)

```
SimpleOS/
├── venv/Include/__init__.py      # Temel modül (Module)
├── arch_detector.py              # Standalone program
└── ARCHITECTURE_DETECTOR.md       # Bu dosya
```

---

## Kurulum (Installation)

### 1. Gereklilikler (Requirements)
- Python 3.6 veya üzeri
- Windows, Linux veya macOS

### 2. Kurulum Adımları

**Seçenek A: Modül olarak kullanma**
```bash
# Projeyi Python path'ine ekleyin
cd SimpleOS
python3 -c "from venv.Include import get_system_info; print(get_system_info())"
```

**Seçenek B: Standalone program olarak çalıştırma**
```bash
cd SimpleOS
python3 arch_detector.py
```

---

## Kullanım Örnekleri (Usage Examples)

### Python Modül Olarak

```python
from venv.Include import (
    get_system_info,
    is_x64,
    is_x86,
    get_architecture,
    SystemArchitecture,
    print_system_info
)

# Basit kontroller
if is_x64():
    print("64-bit sistem üzerinde çalışıyor")

if is_x86():
    print("32-bit sistem üzerinde çalışıyor")

# Mimarisiyi al
arch = get_architecture()
print(f"Sistem mimarisi: {arch}")

# Detaylı bilgi
info = get_system_info()
print(f"Bit sayısı: {info.bits}")
print(f"İşlemci: {info.processor}")
print(f"Platform: {info.machine}")

# Ekrana yazdır
print_system_info()
```

### Standalone Program

```bash
# Temel bilgi göster
python3 arch_detector.py

# Detaylı bilgi
python3 arch_detector.py --verbose

# JSON formatında çıkış
python3 arch_detector.py --json

# Compact JSON
python3 arch_detector.py --json --compact

# x64 üzerinde olup olmadığını kontrol et
python3 arch_detector.py --check x64

# x86 üzerinde olup olmadığını kontrol et
python3 arch_detector.py --check x86

# Banner olmadan çalıştır
python3 arch_detector.py --no-banner
```

---

## Çıktı Örnekleri (Output Examples)

### Normal Çıkış
```
============================================================
SimpleOS Architecture Detector v1.0
============================================================

────────────────────────────────────────────────────────────
System Architecture Information
────────────────────────────────────────────────────────────
Architecture:        X64
Bit Width:           64-bit
Is 64-bit (x64):     True
Is 32-bit (x86):     False
Machine:             AMD64
Processor:           Intel64
Platform:            win32
System:              Windows
Release:             10
────────────────────────────────────────────────────────────
```

### JSON Çıkışı
```json
{
  "timestamp": "2026-05-22T10:30:45.123456",
  "architecture": "x64",
  "bits": 64,
  "is_64bit": true,
  "is_32bit": false,
  "machine": "AMD64",
  "processor": "Intel64",
  "platform": "win32",
  "python_version": "3.9.7",
  "python_build": "CPython",
  "os_name": "nt",
  "system": "Windows",
  "release": "10",
  "version": "10.0.19041"
}
```

---

## API Referansı (API Reference)

### `__init__.py` Modülü

#### Sınıflar (Classes)

**`SystemArchitecture`**
- `get_architecture()`: Mimarisiyi döndür ("x86" veya "x64")
- `get_bits()`: Bit genişliğini döndür (32 veya 64)
- `is_x64()`: 64-bit olup olmadığını kontrol et
- `is_x86()`: 32-bit olup olmadığını kontrol et
- `get_full_info()`: ArchitectureInfo nesnesi döndür

**`ArchitectureInfo` (dataclass)**
- `arch`: Mimari türü (str)
- `bits`: Bit genişliği (int)
- `is_64bit`: 64-bit kontrol (bool)
- `is_32bit`: 32-bit kontrol (bool)
- `machine`: Makine tipi (str)
- `processor`: İşlemci adı (str)
- `python_bits`: Python bit genişliği (int)

#### Fonksiyonlar (Functions)

```python
def get_architecture() -> str
    """Sistem mimarisini döndür"""

def is_x64() -> bool
    """64-bit sistem olup olmadığını kontrol et"""

def is_x86() -> bool
    """32-bit sistem olup olmadığını kontrol et"""

def get_system_info() -> ArchitectureInfo
    """Detaylı sistem bilgisi döndür"""

def print_system_info() -> None
    """Sistem bilgisini ekrana yazdır"""
```

### `arch_detector.py` Program

#### Komut Satırı Argümanları

| Argüman | Açıklama |
|---------|----------|
| `--verbose, -v` | Detaylı bilgi göster |
| `--json, -j` | JSON formatında çıkış |
| `--compact` | Sıkıştırılmış JSON (boşluksuz) |
| `--check {x86,x64}` | Belirli mimaride olup olmadığını kontrol et |
| `--no-banner` | Banner gösterme |

---

## Uygulamalar (Use Cases)

### 1. Sistem Uyumluluğu Kontrolü
```python
if is_x64():
    # 64-bit kütüphaneleri yükle
    import my_64bit_lib
else:
    # 32-bit kütüphaneleri yükle
    import my_32bit_lib
```

### 2. Deployment Doğrulaması
```bash
#!/bin/bash
python3 arch_detector.py --check x64
if [ $? -eq 0 ]; then
    echo "x64 sistem tespit edildi, 64-bit bileşenleri yükle"
else
    echo "x86 sistem tespit edildi, 32-bit bileşenleri yükle"
fi
```

### 3. Sistem Bilgisini Dosyaya Kaydetme
```bash
python3 arch_detector.py --json > system_info.json
```

### 4. Birden Fazla Sistemde Kontrol
```python
from venv.Include import get_system_info

systems = [
    "192.168.1.100",
    "192.168.1.101",
    "192.168.1.102"
]

for host in systems:
    # SSH üzerinden sistemi kontrol et
    info = get_system_info()  # Remote olarak çalıştır
    print(f"{host}: {info.arch}")
```

---

## Sorun Giderme (Troubleshooting)

### Modül İçe Aktarma Sorunu

```python
# Hata: ModuleNotFoundError: No module named 'venv'
# Çözüm: PYTHONPATH'e dizini ekle
import sys
sys.path.insert(0, 'c:\\Users\\mertd\\OneDrive\\Masaüstü\\SimpleOS')
from venv.Include import get_system_info
```

### Program Çalıştırma Sorunu

```bash
# Hata: python: command not found
# Çözüm: python3 kullan
python3 arch_detector.py

# Windows'ta
python arch_detector.py
```

---

## Destek ve Kaynaklar (Support & Resources)

- **SimpleOS Projesi**: [ARCHITECTURE.md](ARCHITECTURE.md)
- **Python Dokümantasyonu**: https://docs.python.org/3/
- **Platform Modülü**: https://docs.python.org/3/library/platform.html

---

## Lisans (License)

Bu proje SimpleOS projesinin bir parçasıdır.

---

## Sürüm Tarihi (Changelog)

### v1.0.0 (2026-05-22)
- ✨ İlk sürüm
- ✅ x86/x64 algılama
- ✅ Standalone program
- ✅ JSON desteği
- ✅ Komut satırı arayüzü

---

**Son Güncelleme**: 22 Mayıs 2026
