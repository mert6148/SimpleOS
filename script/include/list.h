/* SPDX-License-Identifier: MIT */
#ifndef SIMPLEOS_LIST_H
#define SIMPLEOS_LIST_H

#include <stddef.h>

/*
 * Bu dosya, Linux kernel'deki hlist API'sinin minimal bir alt kümesini sağlar.
 * `script/include/hashtable.h` ile birlikte host-side yardımcılar için yeterlidir.
 */

struct hlist_node {
  struct hlist_node *next;
  struct hlist_node **pprev;
};

struct hlist_head {
  struct hlist_node *first;
};

#define HLIST_HEAD_INIT \
  { NULL }

#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
  h->next = NULL;
  h->pprev = NULL;
}

static inline int hlist_unhashed(const struct hlist_node *h)
{
  return !h->pprev;
}

static inline void hlist_del_init(struct hlist_node *n)
{
  if (!hlist_unhashed(n)) {
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
      next->pprev = pprev;
  }
  INIT_HLIST_NODE(n);
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
  struct hlist_node *first = h->first;
  n->next = first;
  if (first)
    first->pprev = &n->next;
  h->first = n;
  n->pprev = &h->first;
}

#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr) - offsetof(type, member)))

#define hlist_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each_entry(pos, head, member)                      \
  for (struct hlist_node *_node = (head)->first;                     \
       _node && ((pos) = hlist_entry(_node, __typeof__(*(pos)), member), 1); \
       _node = _node->next)

#define hlist_for_each_entry_safe(pos, tmp, head, member)            \
  for (struct hlist_node *_node = (head)->first, *_next;             \
       _node && ((_next) = _node->next,                              \
                 (pos) = hlist_entry(_node, __typeof__(*(pos)), member), 1); \
       _node = _next)

#endif /* SIMPLEOS_LIST_H */

