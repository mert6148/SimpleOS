/* Factory.h - C factory interface with header guard */
#ifndef SIMPLEOS_FACTORY_H
#define SIMPLEOS_FACTORY_H

#include <stddef.h>

typedef struct Factory Factory;

/* Create/destroy factory instance */
Factory *factory_create(void);
void factory_destroy(Factory *f);

/* Add a product by name (returns 0 on success) */
int factory_add_product(Factory *f, const char *name);

/* Print products to stdout */
void factory_list_products(const Factory *f);

#endif /* SIMPLEOS_FACTORY_H */
