/*
 * SimpleOS script/examples - C örneği
 *
 * Derleme (WSL/Linux):
 *   gcc -std=c99 -Wall -Wextra -O2 -I../include hello.c -o hello_c
 */

#include <stdio.h>

#include "../include/hashtable.h"

struct item {
  int key;
  const char *value;
  struct hlist_node node;
};

int main(void)
{
  HASHTABLE_DEFINE(ht, 8);
  hash_init(ht);

  struct item a = { .key = 1, .value = "merhaba", .node = {0} };
  struct item b = { .key = 9, .value = "dunya", .node = {0} };

  INIT_HLIST_NODE(&a.node);
  INIT_HLIST_NODE(&b.node);

  hash_add(ht, &a.node, (unsigned)a.key);
  hash_add(ht, &b.node, (unsigned)b.key);

  struct item *it = NULL;
  hash_for_each(ht, it, node)
  {
    printf("key=%d value=%s\n", it->key, it->value);
  }

  return 0;
}

