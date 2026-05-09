/**
 * SimpleOS - IPv4 helpers (host-side utility)
 *
 * This file intentionally contains no OS/kernel dependencies; it's a small
 * helper that can be used by tooling or a future userspace/terminal demo.
 */

#include "ip.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool parse_u8_dec(const char **p, uint8_t *out)
{
    unsigned value = 0;
    unsigned digits = 0;

    while (**p && isdigit((unsigned char)**p))
    {
        value = (value * 10u) + (unsigned)(**p - '0');
        digits++;
        if (value > 255u)
        {
            return false;
        }
        (*p)++;
    }

    if (digits == 0)
    {
        return false;
    }

    *out = (uint8_t)value;
    return true;
}

bool ip_parse_ipv4(const char *s, ipv4_addr_t *out)
{
    if (!s || !out)
    {
        return false;
    }

    const char *p = s;
    uint8_t b0, b1, b2, b3;

    if (!parse_u8_dec(&p, &b0) || *p++ != '.')
        return false;
    if (!parse_u8_dec(&p, &b1) || *p++ != '.')
        return false;
    if (!parse_u8_dec(&p, &b2) || *p++ != '.')
        return false;
    if (!parse_u8_dec(&p, &b3))
        return false;

    if (*p != '\0')
    {
        return false;
    }

    out->bytes[0] = b0;
    out->bytes[1] = b1;
    out->bytes[2] = b2;
    out->bytes[3] = b3;
    return true;
}

bool ip_format_ipv4(const ipv4_addr_t *addr, char *dst, size_t dst_size)
{
    if (!addr || !dst || dst_size == 0)
    {
        return false;
    }

    int written = snprintf(dst, dst_size, "%u.%u.%u.%u",
                           (unsigned)addr->bytes[0],
                           (unsigned)addr->bytes[1],
                           (unsigned)addr->bytes[2],
                           (unsigned)addr->bytes[3]);

    if (written < 0)
    {
        return false;
    }

    return (size_t)written < dst_size;
}