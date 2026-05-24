#!/usr/bin/env python3
"""
SimpleOS Architecture Detector - Pratik Örnekler (Examples)
x86 ve x64 sistemi algılama örnekleri
"""

import sys
import os

# SimpleOS modülünü ekle
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from venv.Include import (
    SystemArchitecture,
    get_system_info,
    is_x64,
    is_x86,
    get_architecture,
    print_system_info,
)


# ============================================================================
# ÖRNEK 1: Temel Mimari Kontrolleri
# ============================================================================

def example_1_basic_checks():
    """Temel mimari kontrolleri"""
    print("\n" + "="*70)
    print("ÖRNEK 1: Temel Mimari Kontrolleri")
    print("="*70 + "\n")
    
    print(f"64-bit sistem mi?        : {is_x64()}")
    print(f"32-bit sistem mi?        : {is_x86()}")
    print(f"Sistem mimarisi          : {get_architecture()}")
    print()


# ============================================================================
# ÖRNEK 2: Detaylı Sistem Bilgileri
# ============================================================================

def example_2_detailed_info():
    """Detaylı sistem bilgilerini göster"""
    print("\n" + "="*70)
    print("ÖRNEK 2: Detaylı Sistem Bilgileri")
    print("="*70 + "\n")
    
    info = get_system_info()
    
    print(f"Mimari            : {info.arch.upper()}")
    print(f"Bit Genişliği     : {info.bits}-bit")
    print(f"64-bit mi?        : {info.is_64bit}")
    print(f"32-bit mi?        : {info.is_32bit}")
    print(f"Makine Tipi       : {info.machine}")
    print(f"İşlemci           : {info.processor}")
    print(f"Python Bitleri    : {info.python_bits}")
    print()


# ============================================================================
# ÖRNEK 3: Koşullu Kod Çalıştırma
# ============================================================================

def example_3_conditional_execution():
    """Mimariye göre koşullu kod çalıştırma"""
    print("\n" + "="*70)
    print("ÖRNEK 3: Koşullu Kod Çalıştırma")
    print("="*70 + "\n")
    
    if is_x64():
        print("✓ 64-bit sistem tespit edildi")
        print("  → 64-bit kütüphaneler yüklenecek")
        print("  → 64-bit bileşenler kurulacak")
        print()
    elif is_x86():
        print("✓ 32-bit sistem tespit edildi")
        print("  → 32-bit kütüphaneler yüklenecek")
        print("  → 32-bit bileşenler kurulacak")
        print()


# ============================================================================
# ÖRNEK 4: Sistem Bilgisini Dosyaya Kaydetme
# ============================================================================

def example_4_save_to_file():
    """Sistem bilgisini dosyaya kaydetme"""
    print("\n" + "="*70)
    print("ÖRNEK 4: Sistem Bilgisini Dosyaya Kaydetme")
    print("="*70 + "\n")
    
    import json
    
    info = get_system_info()
    info_dict = {
        "architecture": info.arch,
        "bits": info.bits,
        "is_64bit": info.is_64bit,
        "is_32bit": info.is_32bit,
        "machine": info.machine,
        "processor": info.processor,
    }
    
    filename = "system_info.json"
    with open(filename, "w") as f:
        json.dump(info_dict, f, indent=2)
    
    print(f"✓ Sistem bilgisi '{filename}' dosyasına kaydedildi")
    print(f"\nDosya içeriği:")
    print(json.dumps(info_dict, indent=2))
    print()
    
    # Dosyayı sil (cleanup)
    os.remove(filename)


# ============================================================================
# ÖRNEK 5: Sistem Uyumluluğu Kontrolleri
# ============================================================================

def example_5_compatibility_checks():
    """Sistem uyumluluğu kontrolleri"""
    print("\n" + "="*70)
    print("ÖRNEK 5: Sistem Uyumluluğu Kontrolleri")
    print("="*70 + "\n")
    
    checks = {
        "64-bit OpenSSL": is_x64(),
        "32-bit Legacy App": is_x86(),
        "Multi-platform Support": True,
        "SIMD Instructions": is_x64(),  # SIMD genellikle 64-bit'e özel
        "Large Memory Support": is_x64(),  # 4GB+ RAM 32-bit'te sınırlı
    }
    
    print("Uyumluluk Kontrolleri:")
    print("-" * 50)
    for check, status in checks.items():
        status_str = "✓ Destekleniyor" if status else "✗ Desteklenmiyor"
        print(f"  {check:<30} : {status_str}")
    print()


# ============================================================================
# ÖRNEK 6: Dinamik Kütüphane Yükleme
# ============================================================================

def example_6_dynamic_library_loading():
    """Mimariye göre kütüphane yükleme (simülasyon)"""
    print("\n" + "="*70)
    print("ÖRNEK 6: Dinamik Kütüphane Yükleme")
    print("="*70 + "\n")
    
    arch = get_architecture()
    
    # Hangi kütüphanenin yükleneceğini belirle
    libraries = {
        "x64": [
            "libcrypto_x64.so",
            "libssl_x64.so",
            "zlib_x64.dll"
        ],
        "x86": [
            "libcrypto_x86.so",
            "libssl_x86.so",
            "zlib_x86.dll"
        ]
    }
    
    print(f"Tespit Edilen Mimari: {arch.upper()}")
    print(f"\nYüklenecek Kütüphaneler:")
    for lib in libraries.get(arch, []):
        print(f"  • {lib}")
    print()


# ============================================================================
# ÖRNEK 7: Sistem Raporu Oluşturma
# ============================================================================

def example_7_system_report():
    """Detaylı sistem raporu oluşturma"""
    print("\n" + "="*70)
    print("ÖRNEK 7: Sistem Raporu Oluşturma")
    print("="*70 + "\n")
    
    print_system_info()


# ============================================================================
# ÖRNEK 8: Değişken Mimarilere Göre Farklı Ayarlar
# ============================================================================

def example_8_arch_specific_settings():
    """Mimariye özel ayarlar"""
    print("\n" + "="*70)
    print("ÖRNEK 8: Mimariye Özel Ayarlar")
    print("="*70 + "\n")
    
    # Mimariye göre bellek ayarları
    memory_settings = {
        "x64": {
            "heap_size": "8GB",
            "stack_size": "8MB",
            "buffer_size": "64KB",
            "max_processes": 512,
        },
        "x86": {
            "heap_size": "2GB",
            "stack_size": "2MB",
            "buffer_size": "16KB",
            "max_processes": 256,
        }
    }
    
    arch = get_architecture()
    settings = memory_settings.get(arch, {})
    
    print(f"Sistem Mimarisi: {arch.upper()}")
    print(f"\nMimari Özel Ayarlar:")
    print("-" * 50)
    for key, value in settings.items():
        print(f"  {key:<20} : {value}")
    print()


# ============================================================================
# ÖRNEK 9: Mimariye Göre Dosya Seçimi
# ============================================================================

def example_9_architecture_specific_files():
    """Mimariye göre farklı dosya seçimi"""
    print("\n" + "="*70)
    print("ÖRNEK 9: Mimariye Göre Dosya Seçimi")
    print("="*70 + "\n")
    
    arch = get_architecture()
    
    files = {
        "x64": {
            "kernel": "kernel_x64.bin",
            "bootloader": "boot_x64.bin",
            "drivers": "drivers_x64/",
        },
        "x86": {
            "kernel": "kernel_x86.bin",
            "bootloader": "boot_x86.bin",
            "drivers": "drivers_x86/",
        }
    }
    
    print(f"Tespit Edilen Mimari: {arch.upper()}")
    print(f"\nYüklenecek Dosyalar:")
    print("-" * 50)
    for component, filepath in files.get(arch, {}).items():
        status = "✓ Bulundu" if True else "✗ Bulunamadı"
        print(f"  {component:<15} : {filepath:<30} {status}")
    print()


# ============================================================================
# ÖRNEK 10: Performans Optimizasyonu
# ============================================================================

def example_10_performance_optimization():
    """Mimariye göre performans optimizasyonu"""
    print("\n" + "="*70)
    print("ÖRNEK 10: Performans Optimizasyonu")
    print("="*70 + "\n")
    
    optimizations = {
        "x64": [
            "SIMD optimizasyonu (SSE4.2, AVX2)",
            "64-bit pointer optimizasyonu",
            "64-bit aritmetik işlemleri",
            "Geniş hafıza erişimi (>4GB)",
            "64-bit thread lokal depolama",
        ],
        "x86": [
            "SIMD optimizasyonu (SSE2)",
            "32-bit pointer optimizasyonu",
            "32-bit aritmetik işlemleri",
            "Sınırlı hafıza erişimi (<4GB)",
            "32-bit thread lokal depolama",
        ]
    }
    
    arch = get_architecture()
    
    print(f"Sistem Mimarisi: {arch.upper()}")
    print(f"\nTatbik Edilen Optimizasyonlar:")
    print("-" * 50)
    for opt in optimizations.get(arch, []):
        print(f"  ✓ {opt}")
    print()


# ============================================================================
# ANA MENÜ VE ÇALIŞTIRMA
# ============================================================================

def main():
    """Ana fonksiyon - tüm örnekleri çalıştır"""
    
    print("\n")
    print("╔" + "═"*68 + "╗")
    print("║" + " "*15 + "SimpleOS Architecture Detector - Örnekler" + " "*11 + "║")
    print("║" + " "*12 + "x86 ve x64 Sistemleri Algılama Pratik Örnekleri" + " "*8 + "║")
    print("╚" + "═"*68 + "╝")
    
    examples = [
        ("Temel Kontroller", example_1_basic_checks),
        ("Detaylı Bilgi", example_2_detailed_info),
        ("Koşullu Çalıştırma", example_3_conditional_execution),
        ("Dosyaya Kaydetme", example_4_save_to_file),
        ("Uyumluluk Kontrolleri", example_5_compatibility_checks),
        ("Kütüphane Yükleme", example_6_dynamic_library_loading),
        ("Sistem Raporu", example_7_system_report),
        ("Mimari Özel Ayarlar", example_8_arch_specific_settings),
        ("Dosya Seçimi", example_9_architecture_specific_files),
        ("Performans Optimizasyonu", example_10_performance_optimization),
    ]
    
    try:
        for i, (name, func) in enumerate(examples, 1):
            func()
            if i < len(examples):
                input("Sonraki örneği görmek için Enter'e basın...")
    
    except KeyboardInterrupt:
        print("\n\n✗ Program kullanıcı tarafından durduruldu")
        return 1
    except Exception as e:
        print(f"\n✗ Hata: {e}")
        return 1
    
    print("\n" + "="*70)
    print("Tüm örnekler tamamlandı!")
    print("="*70 + "\n")
    return 0


if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
