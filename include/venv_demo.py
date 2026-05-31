"""
SimpleOS venv demo script.

Bu betik:
- Belirtilen klasörde bir Python virtual environment (venv) oluşturur.
- Oluşturulan venv içindeki Python ve pip sürümünü doğrular.

Kullanım:
    python Include/venv_demo.py
    python Include/venv_demo.py --path ./venv
"""

import argparse
import os
import subprocess
import sys
import venv


def create_venv(path: str) -> None:
    builder = venv.EnvBuilder(with_pip=True)
    builder.create(path)
    print(f"Created virtual environment at: {path}")


def verify_venv(path: str) -> None:
    python_exe = os.path.join(path, "Scripts", "python.exe") if os.name == "nt" else os.path.join(path, "bin", "python")
    if not os.path.exists(python_exe):
        raise FileNotFoundError(f"Cannot find Python executable in {python_exe}")

    print(f"Virtual environment python: {python_exe}")
    result = subprocess.run([python_exe, "--version"], capture_output=True, text=True)
    print(result.stdout.strip() or result.stderr.strip())

    result = subprocess.run([python_exe, "-m", "pip", "--version"], capture_output=True, text=True)
    print(result.stdout.strip() or result.stderr.strip())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create and verify a Python virtual environment.")
    parser.add_argument("--path", default="venv", help="Virtual environment target folder")
    args = parser.parse_args()

    venv_path = os.path.abspath(args.path)
    if os.path.exists(venv_path) and os.listdir(venv_path):
        print(f"Existing environment detected at {venv_path}. Verifying...")
        verify_venv(venv_path)
    else:
        print(f"Creating venv at {venv_path}...")
        create_venv(venv_path)
        verify_venv(venv_path)
