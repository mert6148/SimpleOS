# Linux alt yapısı: kullanıcı alanı araçları (kernel.c burada çıplak süreç simülasyonudur).

ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BINDIR := $(ROOT)bin

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c11
LDFLAGS ?=

APP := $(BINDIR)/simpleos-app
KSTUB := $(BINDIR)/simpleos-kernel-stub

.PHONY: all dirs app kernel clean ci help lint

dirs:
	mkdir -p $(BINDIR)

all: dirs $(APP) $(KSTUB)

app: dirs $(APP)

kernel: dirs $(KSTUB)

$(APP): $(ROOT)app/app.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(KSTUB): $(ROOT)kernel/kernel.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)

# statik kontroller (splint yoksa atlanır)
lint:
	command -v cppcheck >/dev/null 2>&1 && cppcheck --enable=warning -q "$(ROOT)app/app.c" "$(ROOT)kernel/kernel.c" || true

# CI uyumlu doğrulama
ci: all lint

help:
	@echo "Hedefler: all app kernel clean ci lint"
	@echo "  APP=$(APP)"
	@echo "  KSTUB=$(KSTUB)"
