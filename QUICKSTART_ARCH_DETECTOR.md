# SimpleOS Architecture Detector - Hızlı Başlangıç

## 📋 Dosyalar

| Dosya | Açıklama |
|-------|----------|
| `__init__.py` | Python modülü (import kullanarak) |
| `arch_detector.py` | Standalone program (CLI) |
| `examples_arch_detector.py` | Pratik örnekler |
| `ARCHITECTURE_DETECTOR.md` | Detaylı dokümantasyon |
| `QUICKSTART.md` | Bu dosya |

---

## 🚀 Hızlı Başlangıç

### 1. Basit Kullanım

```bash
# Sistem bilgisini göster
python3 arch_detector.py

# JSON formatında
python3 arch_detector.py --json

# x64 üzerinde mi diye kontrol et
python3 arch_detector.py --check x64
```

### 2. Python Kodunda Kullanım

```python
from venv.Include import is_x64, is_x86, get_system_info

if is_x64():
    print("64-bit sistem")

info = get_system_info()
print(f"Mimari: {info.arch}, Bit: {info.bits}")
```

### 3. Örnekleri Çalıştırma

```bash
python3 examples_arch_detector.py
```

---

## 🎯 Yaygın Kullanım Durumları

### Derleme Yapılandırması
```bash
ARCH=$(python3 arch_detector.py --check x64 && echo "x64" || echo "x86")
./configure --arch=$ARCH
```

### Deployment Kontrol
```python
if is_x64():
    deploy_64bit_version()
else:
    deploy_32bit_version()
```

### Test Ortamı
```bash
python3 arch_detector.py --json > test_env.json
```

---

## 📖 Belgeleme

- **ARCHITECTURE_DETECTOR.md** - Tam belgeleme
- **examples_arch_detector.py** - 10 pratik örnek
- **Inline Kod Yorumları** - Tüm fonksiyonlar açıklanmış

---

## ⚡ Komut Referansı

```bash
# Temel
python3 arch_detector.py

# Verbose (detaylı)
python3 arch_detector.py --verbose

# JSON çıkış
python3 arch_detector.py --json

# Kontrol
python3 arch_detector.py --check x64
python3 arch_detector.py --check x86

# Kombinasyonlar
python3 arch_detector.py --verbose --json
python3 arch_detector.py --json --compact
```

---

## 🔧 Python API

```python
from venv.Include import (
    get_architecture,        # Mimari adı
    is_x64,                  # 64-bit mi
    is_x86,                  # 32-bit mi  
    get_system_info,         # Detaylı bilgi
    SystemArchitecture,      # Sınıf
    ArchitectureInfo,        # Data class
)

# Kullanım
arch = get_architecture()      # "x64" veya "x86"
is_64 = is_x64()              # True/False
info = get_system_info()      # ArchitectureInfo nesnesi
```

---

## ✅ Kontrol Listesi

- [ ] Python 3.6+ kurulu mu?
- [ ] SimpleOS klasörüne erişiminiz var mı?
- [ ] `arch_detector.py` çalışıyor mu?
- [ ] `examples_arch_detector.py` çalışıyor mu?
- [ ] Modül içe aktarılıyor mu?

---

## 🆘 Sorun Giderme

| Sorun | Çözüm |
|-------|-------|
| `ModuleNotFoundError` | PYTHONPATH'e dizin ekleyin |
| `python: not found` | `python3` kullanın |
| İzin hatası | Dosyalara izin verin (`chmod +x`) |
| Yanlış mimari | `--verbose` ile kontrol edin |

---

## 📚 Daha Fazla Bilgi

Detaylı dokümantasyon için **ARCHITECTURE_DETECTOR.md** dosyasını okuyun.

**Pratik örnekler için** `examples_arch_detector.py` dosyasını çalıştırın.

---

**Hazır mısınız? Başlamak için:** `python3 arch_detector.py`
