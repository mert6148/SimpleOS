/**
 * SimpleOS - IPv4 helpers (host-side utility)
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t bytes[4];
} ipv4_addr_t;

bool ip_parse_ipv4(const char *s, ipv4_addr_t *out);
bool ip_format_ipv4(const ipv4_addr_t *addr, char *dst, size_t dst_size);

