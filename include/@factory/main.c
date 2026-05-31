/*
 * main.c - CLI for Factory demo
 * Protection: requires FACTORY_SECRET env var and matching --key argument
 */

#include "Factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *prog) {
    printf("Usage: %s --key <secret> [product ...]\n", prog);
    printf("Requires environment variable FACTORY_SECRET to be set to the same secret.\n");
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    const char *env_secret = getenv("FACTORY_SECRET");
    if (!env_secret) {
        fprintf(stderr, "Protected: FACTORY_SECRET environment variable is not set.\n");
        fprintf(stderr, "Set FACTORY_SECRET before running to allow product creation.\n");
        return 2;
    }

    const char *provided = NULL;
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "--key") == 0 && i + 1 < argc) {
            provided = argv[i + 1];
            i += 2;
            break;
        }
        ++i;
    }

    if (!provided || strcmp(provided, env_secret) != 0) {
        fprintf(stderr, "Authentication failed: provided key is missing or incorrect.\n");
        return 3;
    }

    /* remaining args after --key are product names */
    int first_product = -1;
    for (int j = 1; j < argc; ++j) {
        if (strcmp(argv[j], "--key") == 0) {
            first_product = j + 2; /* skip key value */
            break;
        }
    }

    Factory *f = factory_create();
    if (!f) {
        fprintf(stderr, "Out of memory\n");
        return 4;
    }

    if (first_product > 0 && first_product <= argc) {
        for (int k = first_product; k < argc; ++k) {
            if (factory_add_product(f, argv[k]) != 0) {
                fprintf(stderr, "Failed to add product: %s\n", argv[k]);
            }
        }
    }

    factory_list_products(f);

    factory_destroy(f);
    return 0;
}
