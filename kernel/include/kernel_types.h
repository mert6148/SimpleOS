/**
 * SimpleOS Kernel - Common Type Definitions and Macros
 * Provides fundamental data types and utility macros used throughout the kernel
 */

#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ============================================================================
 * FUNDAMENTAL TYPES
 * =========================================================================== */

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef uintptr_t   addr_t;
typedef uintptr_t   pid_t;
typedef uintptr_t   dev_t;
typedef uintptr_t   ino_t;
typedef uint32_t    mode_t;
typedef uint32_t    uid_t;
typedef uint32_t    gid_t;

/* ============================================================================
 * STATUS CODES
 * =========================================================================== */

#define KERNEL_SUCCESS       0
#define KERNEL_ERROR        -1
#define KERNEL_ENOENT       -2    /* File not found */
#define KERNEL_EACCES       -3    /* Permission denied */
#define KERNEL_ENOMEM       -4    /* Out of memory */
#define KERNEL_EAGAIN       -5    /* Try again */
#define KERNEL_EBUSY        -6    /* Device busy */
#define KERNEL_EINVAL       -7    /* Invalid argument */

/* ============================================================================
 * UTILITY MACROS
 * =========================================================================== */

#define ALIGN(x, a)         (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a)    ((x) & ~((a) - 1))
#define IS_ALIGNED(x, a)    (((x) & ((a) - 1)) == 0)

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define ARRAY_SIZE(a)       (sizeof(a) / sizeof((a)[0]))
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)

#define UNUSED              __attribute__((unused))
#define PACKED              __attribute__((packed))
#define NORETURN            __attribute__((noreturn))
#define SECTION(s)          __attribute__((section(s)))
#define INLINE              inline __attribute__((always_inline))

/* ============================================================================
 * LIST MANAGEMENT
 * =========================================================================== */

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct {
    list_node_t head;
} list_t;

INLINE void list_init(list_t *list) {
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

INLINE void list_add(list_t *list, list_node_t *node) {
    node->next = list->head.next;
    node->prev = &list->head;
    list->head.next->prev = node;
    list->head.next = node;
}

INLINE void list_remove(list_node_t *node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = node->prev = NULL;
}

INLINE bool list_empty(list_t *list) {
    return list->head.next == &list->head;
}

#define list_for_each(pos, list) \
    for ((pos) = (list)->head.next; (pos) != &(list)->head; (pos) = (pos)->next)

#define list_entry(ptr, type, member) \
    CONTAINER_OF(ptr, type, member)

#endif /* KERNEL_TYPES_H */
