# Include / venv Demo

Bu klasörde Python ve C ile venv desteği gösteren örnek programlar bulunur.

## Dosyalar

- `venv_demo.py`: Python `venv` modülü kullanarak bir sanal ortam oluşturur ve doğrular.
- `venv_helper.c`: Ortam değişkenlerini (`VIRTUAL_ENV`, `PATH`) gösterir ve Python yüklemesini kontrol eder.

## Kullanım

Python ile çalıştırmak için:

```powershell
cd "c:\Users\mertd\OneDrive\Masaüstü\SimpleOS"
python Include/venv_demo.py --path venv
```

C programını derlemek için:

```powershell
gcc Include/venv_helper.c -o Include/venv_helper.exe
Include\venv_helper.exe
```

Windows ortamında `python` komutu kullanılabilir olmalıdır.

## Açıklama

`venv_demo.py`:
- `venv` modülü ile belirtilen klasörde bir venv oluşturur.
- `python --version` ve `pip --version` ile sanal ortamı doğrular.

`venv_helper.c`:
- `VIRTUAL_ENV` ve `PATH` ortam değişkenlerini okur.
- Python çalışabilirliğini `system("python --version")` ile kontrol eder.
