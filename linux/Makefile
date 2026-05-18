# Linux alt yapısı: kullanıcı alanı araçları (kernel.c burada çıplak süreç simülasyonudur).

ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BINDIR := $(ROOT)bin
ARCH ?= x64
BUILD_DIR := $(ROOT)build/linux/$(ARCH)

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c11
LDFLAGS ?=

ifeq ($(ARCH),x86)
	CFLAGS += -m32
	LDFLAGS += -m32
	PACKAGE_ARCH := i386
else ifeq ($(ARCH),x64)
	CFLAGS += -m64
	LDFLAGS += -m64
	PACKAGE_ARCH := amd64
else
	PACKAGE_ARCH := $(ARCH)
endif

APP := $(BINDIR)/simpleos-app
KSTUB := $(BINDIR)/simpleos-kernel-stub
KERNEL_BIN := $(BUILD_DIR)/kernel

.PHONY: all build run stop clean install deps install-x86 install-x64 kernel php csharp python lint ci help

all: deps build

deps:
	@echo "[SimpleOS] Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y gcc g++ make python3 python3-pip php apache2 dotnet-sdk-6.0
ifeq ($(ARCH),x86)
	sudo apt-get install -y gcc-multilib g++-multilib libc6-dev-i386
endif
	pip3 install flask flask-cors psutil

install: deps build
	@echo "[SimpleOS] Installing PHP packages..."
	if [ -d services/php ]; then cd services/php && composer install; fi
	@echo "[SimpleOS] Installing .NET packages..."
	if [ -d services/csharp ]; then cd services/csharp && dotnet restore; fi

install-x86: ARCH := x86
install-x86: install

install-x64: ARCH := x64
install-x64: install

build: kernel php csharp python

kernel: | $(BUILD_DIR)
	@echo "[SimpleOS] Building C/C++ kernel for $(ARCH)..."
	$(CC) $(CFLAGS) -o $(KERNEL_BIN) kernel/kernel.c -lpthread

$(BUILD_DIR):
	mkdir -p "$@"

php:
	@echo "[SimpleOS] PHP service ready..."

csharp:
	@echo "[SimpleOS] Building C# ASP.NET..."
	if [ -d services/csharp ]; then cd services/csharp && dotnet build; fi

python:
	@if [ -d services/python ]; then \
		echo "[SimpleOS] Python service ready..."; \
	else \
		echo "[SimpleOS] No Python service found, skipping."; \
	fi

run:
	@echo "[SimpleOS] Starting all services..."
	"$(KERNEL_BIN)" &
	@if [ -d services/php ]; then php -S 0.0.0.0:8080 -t services/php & fi
	@if [ -d services/csharp ]; then cd services/csharp && dotnet run & cd - >/dev/null; fi
	@if [ -d services/python ]; then python3 services/python/kernel_manager.py & fi

stop:
	@echo "[SimpleOS] Stopping all services..."
	pkill -f kernel
	pkill -f "php -S"
	pkill -f "dotnet run"
	pkill -f kernel_manager.py

clean:
	@echo "[SimpleOS] Cleaning build artifacts..."
	rm -f "$(KERNEL_BIN)"
	rm -rf "$(BUILD_DIR)"
	if [ -d services/csharp ]; then cd services/csharp && dotnet clean; fi
	rm -rf __pycache__
	find . -name "*.pyc" -delete

# statik kontroller (splint yoksa atlanır)
lint:
	command -v cppcheck >/dev/null 2>&1 && cppcheck --enable=warning -q "$(ROOT)app/app.c" "$(ROOT)kernel/kernel.c" || true

# CI uyumlu doğrulama
ci: all lint

help:
	@echo "Hedefler: all install install-x86 install-x64 build run stop clean ci lint"
	@echo "  ARCH=$(ARCH)"
	@echo "  KERNEL_BIN=$(KERNEL_BIN)"
