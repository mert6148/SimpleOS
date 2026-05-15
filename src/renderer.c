/**
 * SimpleOS Renderer Implementation
 * Graphics and text rendering engine
 */

#include "renderer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/* ============================================================================
 * RENDERER STRUCTURE
 * =========================================================================== */

typedef struct
{
    char character;
    uint8_t fg_color;
    uint8_t bg_color;
    bool bold;
    bool underline;
} cell_t;

struct renderer
{
    int width;
    int height;
    cell_t *buffer;
    cell_t *back_buffer;
    uint8_t current_fg;
    uint8_t current_bg;
    bool current_bold;
    bool current_underline;
};

/* ============================================================================
 * ANSI COLOR CODES
 * =========================================================================== */

static const char *ansi_color_codes[9] =
    {
        "30", // BLACK
        "31", // RED
        "32", // GREEN
        "33", // YELLOW
        "34", // BLUE
        "35", // MAGENTA
        "36", // CYAN
        "37", // WHITE
        "90"  // GRAY
};

/* ============================================================================
 * RENDERER LIFECYCLE
 * =========================================================================== */

renderer_t *renderer_create(int width, int height)
{
    renderer_t *renderer = (renderer_t *)malloc(sizeof(renderer_t));
    if (!renderer)
        return NULL;

    renderer->width = width > 0 ? width : 80;
    renderer->height = height > 0 ? height : 24;

    size_t buffer_size = renderer->width * renderer->height * sizeof(cell_t);
    renderer->buffer = (cell_t *)malloc(buffer_size);
    renderer->back_buffer = (cell_t *)malloc(buffer_size);

    if (!renderer->buffer || !renderer->back_buffer)
    {
        free(renderer->buffer);
        free(renderer->back_buffer);
        free(renderer);
        return NULL;
    }

    renderer->current_fg = COLOR_WHITE;
    renderer->current_bg = COLOR_BLACK;
    renderer->current_bold = false;
    renderer->current_underline = false;

    memset(renderer->buffer, 0, buffer_size);
    memset(renderer->back_buffer, 0, buffer_size);

    return renderer;
}

void renderer_destroy(renderer_t *renderer)
{
    if (!renderer)
        return;

    free(renderer->buffer);
    free(renderer->back_buffer);
    free(renderer);
}

int renderer_init(renderer_t *renderer)
{
    if (!renderer)
        return -1;

    // Clear screen
    printf("\x1b[2J");
    printf("\x1b[H");
    fflush(stdout);

    return 0;
}

void renderer_clear(renderer_t *renderer, color_t bg_color)
{
    if (!renderer)
        return;

    for (int i = 0; i < renderer->width * renderer->height; i++)
    {
        renderer->buffer[i].character = ' ';
        renderer->buffer[i].fg_color = COLOR_WHITE;
        renderer->buffer[i].bg_color = COLOR_BLACK;
        renderer->buffer[i].bold = false;
        renderer->buffer[i].underline = false;
    }
}

int renderer_present(renderer_t *renderer)
{
    if (!renderer)
        return -1;

    // Simple implementation: redraw entire screen
    printf("\x1b[2J\x1b[H");

    for (int y = 0; y < renderer->height; y++)
    {
        for (int x = 0; x < renderer->width; x++)
        {
            int index = y * renderer->width + x;
            cell_t *cell = &renderer->buffer[index];

            // Set colors and attributes
            if (cell->bold)
                printf("\x1b[1m");
            if (cell->underline)
                printf("\x1b[4m");

            printf("\x1b[%sm", ansi_color_codes[cell->fg_color]);

            putchar(cell->character);
            printf("\x1b[0m");
        }
        printf("\n");
    }

    fflush(stdout);
    return 0;
}

void renderer_get_size(renderer_t *renderer, int *width, int *height)
{
    if (renderer)
    {
        if (width)
            *width = renderer->width;
        if (height)
            *height = renderer->height;
    }
}

/* ============================================================================
 * DRAWING PRIMITIVES
 * =========================================================================== */

void renderer_set_color(renderer_t *renderer, ansi_color_t fg, ansi_color_t bg)
{
    if (renderer)
    {
        renderer->current_fg = fg;
        renderer->current_bg = bg;
    }
}

void renderer_draw_pixel(renderer_t *renderer, int x, int y, char c)
{
    if (!renderer || x < 0 || y < 0 || x >= renderer->width || y >= renderer->height)
        return;

    int index = y * renderer->width + x;
    renderer->buffer[index].character = c;
    renderer->buffer[index].fg_color = renderer->current_fg;
    renderer->buffer[index].bg_color = renderer->current_bg;
    renderer->buffer[index].bold = renderer->current_bold;
    renderer->buffer[index].underline = renderer->current_underline;
}

void renderer_draw_line(renderer_t *renderer, int x1, int y1, int x2, int y2, char c)
{
    if (!renderer)
        return;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    int x = x1, y = y1;

    while (true)
    {
        renderer_draw_pixel(renderer, x, y, c);

        if (x == x2 && y == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void renderer_draw_rect_filled(renderer_t *renderer, int x, int y, int width, int height, char c)
{
    if (!renderer)
        return;

    for (int row = y; row < y + height && row < renderer->height; row++)
    {
        for (int col = x; col < x + width && col < renderer->width; col++)
        {
            renderer_draw_pixel(renderer, col, row, c);
        }
    }
}

void renderer_draw_rect_outline(renderer_t *renderer, int x, int y, int width, int height)
{
    if (!renderer)
        return;

    // Top and bottom
    for (int col = x; col < x + width && col < renderer->width; col++)
    {
        renderer_draw_pixel(renderer, col, y, '─');
        renderer_draw_pixel(renderer, col, y + height - 1, '─');
    }

    // Left and right
    for (int row = y; row < y + height && row < renderer->height; row++)
    {
        renderer_draw_pixel(renderer, x, row, '│');
        renderer_draw_pixel(renderer, x + width - 1, row, '│');
    }

    // Corners
    renderer_draw_pixel(renderer, x, y, '┌');
    renderer_draw_pixel(renderer, x + width - 1, y, '┐');
    renderer_draw_pixel(renderer, x, y + height - 1, '└');
    renderer_draw_pixel(renderer, x + width - 1, y + height - 1, '┘');
}

void renderer_draw_box(renderer_t *renderer, int x, int y, int width, int height, const char *title)
{
    if (!renderer)
        return;

    renderer_draw_rect_outline(renderer, x, y, width, height);

    if (title && strlen(title) > 0)
    {
        int title_x = x + 2;
        int title_y = y;
        renderer_draw_text(renderer, title_x, title_y, title);
    }
}

/* ============================================================================
 * TEXT RENDERING
 * =========================================================================== */

void renderer_draw_text(renderer_t *renderer, int x, int y, const char *text)
{
    if (!renderer || !text)
        return;

    for (int i = 0; text[i] != '\0' && x + i < renderer->width; i++)
    {
        renderer_draw_pixel(renderer, x + i, y, text[i]);
    }
}

void renderer_draw_text_aligned(renderer_t *renderer, int x, int y, int width,
                                text_align_t align, const char *text)
{
    if (!renderer || !text)
        return;

    int text_len = strlen(text);

    switch (align)
    {
    case ALIGN_LEFT:
        renderer_draw_text(renderer, x, y, text);
        break;

    case ALIGN_CENTER:
    {
        int offset = (width - text_len) / 2;
        renderer_draw_text(renderer, x + offset, y, text);
        break;
    }

    case ALIGN_RIGHT:
    {
        int offset = width - text_len;
        renderer_draw_text(renderer, x + offset, y, text);
        break;
    }
    }
}

void renderer_draw_printf(renderer_t *renderer, int x, int y, const char *fmt, ...)
{
    if (!renderer || !fmt)
        return;

    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    renderer_draw_text(renderer, x, y, buffer);
}

int renderer_text_width(const char *text)
{
    if (!text)
        return 0;

    return strlen(text);
}

/* ============================================================================
 * STYLES & ATTRIBUTES
 * =========================================================================== */

void renderer_set_bold(renderer_t *renderer, bool bold)
{
    if (renderer)
    {
        renderer->current_bold = bold;
    }
}

void renderer_set_underline(renderer_t *renderer, bool underline)
{
    if (renderer)
    {
        renderer->current_underline = underline;
    }
}

void renderer_reset_attributes(renderer_t *renderer)
{
    if (renderer)
    {
        renderer->current_bold = false;
        renderer->current_underline = false;
        renderer->current_fg = COLOR_WHITE;
        renderer->current_bg = COLOR_BLACK;
    }
}

/* ============================================================================
 * BUFFER OPERATIONS
 * =========================================================================== */

char renderer_get_char(renderer_t *renderer, int x, int y)
{
    if (!renderer || x < 0 || y < 0 || x >= renderer->width || y >= renderer->height)
        return ' ';

    int index = y * renderer->width + x;
    return renderer->buffer[index].character;
}

int renderer_save(renderer_t *renderer, const char *filename)
{
    if (!renderer || !filename)
        return -1;

    FILE *f = fopen(filename, "wb");
    if (!f)
        return -1;

    if (fwrite(&renderer->width, sizeof(int), 1, f) != 1)
    {
        fclose(f);
        return -1;
    }

    if (fwrite(&renderer->height, sizeof(int), 1, f) != 1)
    {
        fclose(f);
        return -1;
    }

    size_t size = renderer->width * renderer->height * sizeof(cell_t);
    if (fwrite(renderer->buffer, 1, size, f) != size)
    {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

int renderer_load(renderer_t *renderer, const char *filename)
{
    if (!renderer || !filename)
        return -1;

    FILE *f = fopen(filename, "rb");
    if (!f)
        return -1;

    int width, height;
    if (fread(&width, sizeof(int), 1, f) != 1)
    {
        fclose(f);
        return -1;
    }

    if (fread(&height, sizeof(int), 1, f) != 1)
    {
        fclose(f);
        return -1;
    }

    if (width != renderer->width || height != renderer->height)
    {
        fclose(f);
        return -1;
    }

    size_t size = renderer->width * renderer->height * sizeof(cell_t);
    if (fread(renderer->buffer, 1, size, f) != size)
    {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}
