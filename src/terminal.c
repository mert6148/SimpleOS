/**
 * SimpleOS Terminal Emulator
 * A simple terminal emulator for interacting with the SimpleOS kernel
 *
 * Provides:
 * - Serial console communication
 * - Command parsing
 * - Output buffering
 * - History management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>

#include "terminal.h"

/* ============================================================================
 * TERMINAL CONFIGURATION
 * =========================================================================== */

#define TERMINAL_MAX_COLS 80
#define TERMINAL_MAX_ROWS 24
#define TERMINAL_MAX_CMDLINE 256
#define TERMINAL_MAX_HISTORY 100
#define TERMINAL_MAX_OUTPUT_LINES 1000

/* ============================================================================
 * COLOR CODES
 * =========================================================================== */

typedef enum
{
    TERM_COLOR_BLACK = 0,
    TERM_COLOR_RED = 1,
    TERM_COLOR_GREEN = 2,
    TERM_COLOR_YELLOW = 3,
    TERM_COLOR_BLUE = 4,
    TERM_COLOR_MAGENTA = 5,
    TERM_COLOR_CYAN = 6,
    TERM_COLOR_WHITE = 7
} terminal_color_t;

typedef struct
{
    terminal_color_t foreground;
    terminal_color_t background;
    bool bold;
    bool underline;
} terminal_attribute_t;

/* ============================================================================
 * TERMINAL STATE
 * =========================================================================== */

typedef struct
{
    char buffer[TERMINAL_MAX_CMDLINE];
    size_t pos;
    size_t len;
    bool dirty;
} terminal_line_t;

typedef struct
{
    char history[TERMINAL_MAX_HISTORY][TERMINAL_MAX_CMDLINE];
    size_t count;
    size_t current;
} terminal_history_t;

typedef struct
{
    char lines[TERMINAL_MAX_OUTPUT_LINES][TERMINAL_MAX_COLS];
    size_t count;
    size_t scroll_pos;
    terminal_attribute_t attr;
} terminal_output_t;

struct terminal
{
    terminal_line_t cmdline;
    terminal_history_t history;
    terminal_output_t output;
    bool running;
    time_t start_time;
};

/* ============================================================================
 * TERMINAL INITIALIZATION
 * =========================================================================== */

terminal_t *terminal_create(void)
{
    terminal_t *term = (terminal_t *)malloc(sizeof(terminal_t));

    if (!term)
    {
        return NULL;
    }

    memset(term, 0, sizeof(terminal_t));

    // Initialize command line
    term->cmdline.buffer[0] = '\0';
    term->cmdline.pos = 0;
    term->cmdline.len = 0;
    term->cmdline.dirty = true;

    // Initialize history
    term->history.count = 0;
    term->history.current = 0;

    // Initialize output
    term->output.count = 0;
    term->output.scroll_pos = 0;
    memset(term->output.lines[0], 0, TERMINAL_MAX_COLS);
    term->output.attr.foreground = TERM_COLOR_WHITE;
    term->output.attr.background = TERM_COLOR_BLACK;
    term->output.attr.bold = false;
    term->output.attr.underline = false;

    // Terminal state
    term->running = true;
    term->start_time = time(NULL);

    return term;
}

/* ============================================================================
 * TERMINAL OUTPUT
 * =========================================================================== */

void terminal_putchar(terminal_t *term, char c)
{
    if (!term || term->output.count >= TERMINAL_MAX_OUTPUT_LINES)
    {
        return;
    }

    if (c == '\n')
    {
        if (term->output.count + 1 >= TERMINAL_MAX_OUTPUT_LINES)
        {
            return;
        }
        term->output.count++;
        memset(term->output.lines[term->output.count], 0, TERMINAL_MAX_COLS);
    }
    else
    {
        size_t line = term->output.count;
        size_t col = strlen(term->output.lines[line]);

        if (col < TERMINAL_MAX_COLS - 1)
        {
            term->output.lines[line][col] = c;
            term->output.lines[line][col + 1] = '\0';
        }
    }
}

void terminal_puts(terminal_t *term, const char *str)
{
    if (!term || !str)
    {
        return;
    }

    for (const char *p = str; *p; p++)
    {
        terminal_putchar(term, *p);
    }
}

void terminal_printf(terminal_t *term, const char *fmt, ...)
{
    char buffer[512];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    terminal_puts(term, buffer);
}

/* ============================================================================
 * COMMAND LINE EDITING
 * =========================================================================== */

void terminal_cmd_insert(terminal_t *term, char c)
{
    if (!term)
        return;

    if (term->cmdline.len >= TERMINAL_MAX_CMDLINE - 1)
    {
        return;
    }

    // Insert at cursor position
    for (size_t i = term->cmdline.len; i > term->cmdline.pos; i--)
    {
        term->cmdline.buffer[i] = term->cmdline.buffer[i - 1];
    }

    term->cmdline.buffer[term->cmdline.pos] = c;
    term->cmdline.pos++;
    term->cmdline.len++;
    term->cmdline.buffer[term->cmdline.len] = '\0';
    term->cmdline.dirty = true;
}

void terminal_cmd_backspace(terminal_t *term)
{
    if (!term || term->cmdline.pos == 0)
    {
        return;
    }

    // Delete character before cursor
    for (size_t i = term->cmdline.pos - 1; i < term->cmdline.len; i++)
    {
        term->cmdline.buffer[i] = term->cmdline.buffer[i + 1];
    }

    term->cmdline.pos--;
    term->cmdline.len--;
    term->cmdline.buffer[term->cmdline.len] = '\0';
    term->cmdline.dirty = true;
}

void terminal_cmd_delete(terminal_t *term)
{
    if (!term || term->cmdline.pos >= term->cmdline.len)
    {
        return;
    }

    // Delete character at cursor
    for (size_t i = term->cmdline.pos; i < term->cmdline.len; i++)
    {
        term->cmdline.buffer[i] = term->cmdline.buffer[i + 1];
    }

    term->cmdline.len--;
    term->cmdline.buffer[term->cmdline.len] = '\0';
    term->cmdline.dirty = true;
}

void terminal_cmd_clear(terminal_t *term)
{
    if (!term)
        return;

    memset(term->cmdline.buffer, 0, TERMINAL_MAX_CMDLINE);
    term->cmdline.pos = 0;
    term->cmdline.len = 0;
    term->cmdline.dirty = true;
}

/* ============================================================================
 * COMMAND EXECUTION
 * =========================================================================== */

typedef struct
{
    const char *name;
    const char *description;
    void (*handler)(terminal_t *term, int argc, char **argv);
} terminal_command_t;

void cmd_help(terminal_t *term, int argc, char **argv)
{
    terminal_puts(term, "\nSimpleOS Terminal - Available Commands:\n");
    terminal_puts(term, "  help       - Show this help message\n");
    terminal_puts(term, "  clear      - Clear terminal\n");
    terminal_puts(term, "  echo       - Echo text\n");
    terminal_puts(term, "  status     - Show kernel status\n");
    terminal_puts(term, "  exit       - Exit terminal\n");
    terminal_puts(term, "\n");
}

void cmd_clear(terminal_t *term, int argc, char **argv)
{
    // Clear output lines
    for (size_t i = 0; i < TERMINAL_MAX_OUTPUT_LINES; i++)
    {
        memset(term->output.lines[i], 0, TERMINAL_MAX_COLS);
    }
    term->output.count = 0;
    term->output.scroll_pos = 0;
}

void cmd_echo(terminal_t *term, int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        terminal_puts(term, argv[i]);
        if (i < argc - 1)
            terminal_putchar(term, ' ');
    }
    terminal_putchar(term, '\n');
}

void cmd_status(terminal_t *term, int argc, char **argv)
{
    time_t now = time(NULL);
    time_t uptime = now - term->start_time;

    terminal_printf(term, "\n[KERNEL STATUS]\n");
    terminal_printf(term, "  Uptime: %ld seconds\n", uptime);
    terminal_printf(term, "  Terminal: Active\n");
    terminal_printf(term, "  Output Lines: %zu\n", term->output.count);
    terminal_printf(term, "  Command History: %zu\n", term->history.count);
    terminal_puts(term, "\n");
}

void cmd_exit(terminal_t *term, int argc, char **argv)
{
    terminal_puts(term, "Exiting terminal...\n");
    term->running = false;
}

terminal_command_t commands[] = {
    {"help", "Show help message", cmd_help},
    {"clear", "Clear terminal", cmd_clear},
    {"echo", "Echo text", cmd_echo},
    {"status", "Show kernel status", cmd_status},
    {"exit", "Exit terminal", cmd_exit},
    {NULL, NULL, NULL}};

/* ============================================================================
 * COMMAND PARSING
 * =========================================================================== */

void terminal_execute_command(terminal_t *term, const char *cmdline)
{
    if (!term || !cmdline || cmdline[0] == '\0')
    {
        return;
    }

    // Copy command line for parsing
    char buffer[TERMINAL_MAX_CMDLINE];
    strncpy(buffer, cmdline, TERMINAL_MAX_CMDLINE - 1);
    buffer[TERMINAL_MAX_CMDLINE - 1] = '\0';

    // Parse arguments
    char *argv[32];
    int argc = 0;
    char *token = strtok(buffer, " \t");

    while (token && argc < 31)
    {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }

    if (argc == 0)
        return;

    // Add to history
    if (term->history.count < TERMINAL_MAX_HISTORY)
    {
        strncpy(term->history.history[term->history.count], cmdline,
                TERMINAL_MAX_CMDLINE - 1);
        term->history.count++;
    }

    // Find and execute command
    for (int i = 0; commands[i].name; i++)
    {
        if (strcmp(argv[0], commands[i].name) == 0)
        {
            terminal_printf(term, "> %s\n", cmdline);
            commands[i].handler(term, argc, argv);
            return;
        }
    }

    // Unknown command
    terminal_printf(term, "> %s\n", cmdline);
    terminal_printf(term, "Unknown command: %s\n", argv[0]);
}

/* ============================================================================
 * TERMINAL CLEANUP
 * =========================================================================== */

void terminal_destroy(terminal_t *term)
{
    if (term)
    {
        free(term);
    }
}

/* ============================================================================
 * MAIN / TEST FUNCTION
 * =========================================================================== */

#ifdef TERMINAL_STANDALONE
int main(void)
{
    terminal_t *term = terminal_create();

    if (!term)
    {
        fprintf(stderr, "Failed to create terminal\n");
        return 1;
    }

    terminal_puts(term, "\n╔════════════════════════════════════════════════════════════╗\n");
    terminal_puts(term, "║         SimpleOS Terminal Emulator v1.0.0                 ║\n");
    terminal_puts(term, "║              Type 'help' for commands                     ║\n");
    terminal_puts(term, "╚════════════════════════════════════════════════════════════╝\n\n");

    // Simulate some test commands
    terminal_execute_command(term, "help");
    terminal_execute_command(term, "echo Hello SimpleOS");
    terminal_execute_command(term, "status");

    // Print output
    printf("\n=== Terminal Output ===\n");
    for (size_t i = 0; i <= term->output.count && i < TERMINAL_MAX_OUTPUT_LINES; i++)
    {
        if (term->output.lines[i][0] != '\0')
        {
            printf("%s\n", term->output.lines[i]);
        }
    }

    terminal_destroy(term);
    return 0;
}
#endif
