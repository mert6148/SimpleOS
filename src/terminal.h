/**
 * SimpleOS Terminal Emulator (host-side helper)
 *
 * This module provides a small in-memory terminal model (output buffer,
 * command line editing, history) useful for demos/tests.
 */

#pragma once

#include <stddef.h>

typedef struct terminal terminal_t;

terminal_t *terminal_create(void);
void terminal_destroy(terminal_t *term);

void terminal_putchar(terminal_t *term, char c);
void terminal_puts(terminal_t *term, const char *str);
void terminal_printf(terminal_t *term, const char *fmt, ...);

void terminal_cmd_insert(terminal_t *term, char c);
void terminal_cmd_backspace(terminal_t *term);
void terminal_cmd_delete(terminal_t *term);
void terminal_cmd_clear(terminal_t *term);

void terminal_execute_command(terminal_t *term, const char *cmdline);

