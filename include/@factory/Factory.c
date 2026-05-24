/*
 * C implementation of a simple Factory module.
 * Provides dynamic product storage and listing.
 * This file accompanies Factory.h and a small CLI program.
 */

#include "Factory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Factory {
    char **products;
    size_t count;
    size_t capacity;
};

/* Portable strdup replacement to avoid platform-specific names */
static char *factory_strdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *d = (char*)malloc(n);
    if (!d) return NULL;
    memcpy(d, s, n);
    return d;
}

Factory *factory_create(void) {
    Factory *f = (Factory*)malloc(sizeof(Factory));
    if (!f) return NULL;
    f->products = NULL;
    f->count = 0;
    f->capacity = 0;
    return f;
}

void factory_destroy(Factory *f) {
    if (!f) return;
    for (size_t i = 0; i < f->count; ++i) {
        free(f->products[i]);
    }
    free(f->products);
    free(f);
}

int factory_add_product(Factory *f, const char *name) {
    if (!f || !name) return -1;
    if (f->count + 1 > f->capacity) {
        size_t newcap = f->capacity == 0 ? 4 : f->capacity * 2;
        char **tmp = (char**)realloc(f->products, newcap * sizeof(char*));
        if (!tmp) return -1;
        f->products = tmp;
        f->capacity = newcap;
    }
    f->products[f->count] = factory_strdup(name);
    if (!f->products[f->count]) return -1;
    f->count += 1;
    return 0;
}

void factory_list_products(const Factory *f) {
    if (!f) return;
    printf("Listing %zu product(s):\n", f->count);
    for (size_t i = 0; i < f->count; ++i) {
        printf("  %zu: %s\n", i + 1, f->products[i]);
    }
}
