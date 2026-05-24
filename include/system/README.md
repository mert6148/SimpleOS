# SimpleOS System Prototype

Bu klasör, SimpleOS için mimariye duyarlı bir sistem prototipi içerir.

## İçerik

- `simpleos_system_x86` — 32-bit x86 mimarisi için derlenen prototip
- `simpleos_system_x64` — 64-bit x64 mimarisi için derlenen prototip

## Build

### Linux / macOS / WSL

```bash
cd include/system
./build_system.sh
```

### Windows PowerShell

```powershell
cd include\system
.
```

### Elle CMake kullanımı

```bash
mkdir -p include/system/build
cd include/system/build
cmake ..
cmake --build . --config Release
```

## Çalıştırma

```bash
./build/simpleos_system_x86 --help
./build/simpleos_system_x64 --help
```

## Kullanım

- `--info-only`: Sadece sistem meta verisini gösterir
- `--demo-only`: Sadece mimari demoyu çalıştırır
- `--help`: Yardım metnini gösterir

## Amaç

- `include/system` dizinini SimpleOS için doğru prototip alanı haline getirir.
- x86 ve x64 yürütülebilirlerini ayrı tutar.
- Mimariler arası runtime davranışı ve komut satırı seçeneğini gösterir.
