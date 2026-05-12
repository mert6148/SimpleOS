/**
 * SimpleOS UI Components Implementation
 * Terminal UI rendering and interaction
 */

#include "ui_components.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

/* ============================================================================
 * ANSI COLOR CODES
 * =========================================================================== */

#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_DIM "\x1b[2m"
#define ANSI_UNDERLINE "\x1b[4m"

#define ANSI_FG_BLACK "\x1b[30m"
#define ANSI_FG_RED "\x1b[31m"
#define ANSI_FG_GREEN "\x1b[32m"
#define ANSI_FG_YELLOW "\x1b[33m"
#define ANSI_FG_BLUE "\x1b[34m"
#define ANSI_FG_MAGENTA "\x1b[35m"
#define ANSI_FG_CYAN "\x1b[36m"
#define ANSI_FG_WHITE "\x1b[37m"

#define ANSI_BG_BLACK "\x1b[40m"
#define ANSI_BG_RED "\x1b[41m"
#define ANSI_BG_GREEN "\x1b[42m"
#define ANSI_BG_YELLOW "\x1b[43m"
#define ANSI_BG_BLUE "\x1b[44m"
#define ANSI_BG_MAGENTA "\x1b[45m"
#define ANSI_BG_CYAN "\x1b[46m"
#define ANSI_BG_WHITE "\x1b[47m"

#define ANSI_CURSOR_HIDE "\x1b[?25l"
#define ANSI_CURSOR_SHOW "\x1b[?25h"
#define ANSI_CURSOR_HOME "\x1b[H"
#define ANSI_CLEAR_SCREEN "\x1b[2J"

/* ============================================================================
 * GLOBAL STATE
 * =========================================================================== */

static int g_screen_width = 80;
static int g_screen_height = 24;

/* ============================================================================
 * PANEL FUNCTIONS
 * =========================================================================== */

ui_panel_t ui_panel_create(const char *title, int x, int y, int width, int height)
{
    ui_panel_t panel;

    memset(&panel, 0, sizeof(ui_panel_t));
    if (title)
    {
        strncpy(panel.title, title, sizeof(panel.title) - 1);
    }

    panel.x = x;
    panel.y = y;
    panel.width = width;
    panel.height = height;
    panel.visible = true;
    panel.border = true;

    return panel;
}

void ui_panel_set_content(ui_panel_t *panel, const char *content)
{
    if (!panel || !content)
        return;

    memset(panel->content, 0, sizeof(panel->content));
    strncpy(panel->content, content, sizeof(panel->content) - 1);
}

void ui_panel_append(ui_panel_t *panel, const char *text)
{
    if (!panel || !text)
        return;

    size_t current_len = strlen(panel->content);
    size_t space_left = sizeof(panel->content) - current_len - 1;

    if (space_left > 0)
    {
        strncat(panel->content, text, space_left);
    }
}

void ui_panel_draw(const ui_panel_t *panel)
{
    if (!panel || !panel->visible)
        return;

    // Move cursor to position
    printf("\x1b[%d;%dH", panel->y, panel->x);

    // Draw top border if enabled
    if (panel->border)
    {
        printf("┌");
        for (int i = 0; i < panel->width - 2; i++)
            printf("─");
        printf("┐");

        // Draw title
        if (panel->title[0])
        {
            printf("\x1b[%d;%dH", panel->y, panel->x + 2);
            printf(" %s ", panel->title);
        }
    }

    // Draw content
    int content_y = panel->y + (panel->border ? 1 : 0);
    const char *line = panel->content;
    int line_count = 0;

    while (*line && line_count < panel->height - (panel->border ? 2 : 0))
    {
        printf("\x1b[%d;%dH", content_y + line_count, panel->x + (panel->border ? 1 : 0));

        // Print up to panel width characters
        int col = 0;
        while (*line && *line != '\n' && col < panel->width - (panel->border ? 2 : 0))
        {
            putchar(*line);
            line++;
            col++;
        }

        if (*line == '\n')
            line++;

        line_count++;
    }

    // Draw bottom border if enabled
    if (panel->border)
    {
        printf("\x1b[%d;%dH", panel->y + panel->height - 1, panel->x);
        printf("└");
        for (int i = 0; i < panel->width - 2; i++)
            printf("─");
        printf("┘");
    }
}

void ui_panel_clear(ui_panel_t *panel)
{
    if (panel)
    {
        memset(panel->content, 0, sizeof(panel->content));
    }
}

/* ============================================================================
 * PROGRESS BAR FUNCTIONS
 * =========================================================================== */

ui_progress_bar_t ui_progress_bar_create(const char *label, int width)
{
    ui_progress_bar_t bar;

    memset(&bar, 0, sizeof(ui_progress_bar_t));
    if (label)
    {
        strncpy(bar.label, label, sizeof(bar.label) - 1);
    }

    bar.width = width > 0 ? width : 40;
    bar.fill_char = '=';
    bar.empty_char = '-';
    bar.value = 0.0f;

    return bar;
}

void ui_progress_bar_set_value(ui_progress_bar_t *bar, float value)
{
    if (bar)
    {
        bar->value = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
    }
}

void ui_progress_bar_draw(const ui_progress_bar_t *bar)
{
    if (!bar)
        return;

    printf("%s [", bar->label);

    int filled = (int)(bar->value * bar->width);
    for (int i = 0; i < bar->width; i++)
    {
        putchar(i < filled ? bar->fill_char : bar->empty_char);
    }

    printf("] %.1f%%\n", bar->value * 100.0f);
}

/* ============================================================================
 * TABLE FUNCTIONS
 * =========================================================================== */

ui_table_t ui_table_create(const char *title)
{
    ui_table_t table;

    memset(&table, 0, sizeof(ui_table_t));
    if (title)
    {
        strncpy(table.title, title, sizeof(table.title) - 1);
    }

    table.row_count = 0;
    table.total_width = 0;

    return table;
}

void ui_table_add_header(ui_table_t *table, const char *header, int width)
{
    if (!table || !header || table->headers.cell_count >= 10)
        return;

    int idx = table->headers.cell_count;
    strncpy(table->headers.cells[idx].value, header,
            sizeof(table->headers.cells[idx].value) - 1);
    table->headers.cells[idx].width = width;
    table->headers.cells[idx].right_align = false;

    table->headers.cell_count++;
    table->total_width += width + 3; // +3 for spacing
}

int ui_table_add_row(ui_table_t *table, const char **values, int count)
{
    if (!table || !values || count > 10 || table->row_count >= 32)
        return -1;

    int row_idx = table->row_count;
    ui_table_row_t *row = &table->rows[row_idx];

    for (int i = 0; i < count && i < 10; i++)
    {
        if (values[i])
        {
            strncpy(row->cells[i].value, values[i],
                    sizeof(row->cells[i].value) - 1);
        }
    }

    row->cell_count = count;
    table->row_count++;

    return row_idx;
}

void ui_table_set_cell(ui_table_t *table, int row, int col, const char *value)
{
    if (!table || row < 0 || row >= table->row_count || col < 0 || col >= 10 || !value)
        return;

    strncpy(table->rows[row].cells[col].value, value,
            sizeof(table->rows[row].cells[col].value) - 1);
}

void ui_table_draw(const ui_table_t *table)
{
    if (!table)
        return;

    if (table->title[0])
    {
        printf("\n%s\n", table->title);
    }

    // Draw header
    if (table->headers.cell_count > 0)
    {
        for (int i = 0; i < table->headers.cell_count; i++)
        {
            if (i > 0)
                printf("  ");
            printf("%-*s", table->headers.cells[i].width, table->headers.cells[i].value);
        }
        printf("\n");

        // Draw separator
        for (int i = 0; i < table->headers.cell_count; i++)
        {
            if (i > 0)
                printf("  ");
            for (int j = 0; j < table->headers.cells[i].width; j++)
                printf("-");
        }
        printf("\n");
    }

    // Draw rows
    for (int row = 0; row < table->row_count; row++)
    {
        for (int col = 0; col < table->rows[row].cell_count; col++)
        {
            if (col > 0)
                printf("  ");

            int width = table->headers.cells[col].width;
            printf("%-*s", width, table->rows[row].cells[col].value);
        }
        printf("\n");
    }
}

void ui_table_clear(ui_table_t *table)
{
    if (table)
    {
        table->row_count = 0;
    }
}

/* ============================================================================
 * STATUS FUNCTIONS
 * =========================================================================== */

void ui_draw_status(const char *label, ui_status_t status)
{
    if (!label)
        return;

    const char *status_str;
    const char *color;

    switch (status)
    {
    case UI_STATUS_OK:
        status_str = "✓";
        color = ANSI_FG_GREEN;
        break;
    case UI_STATUS_WARNING:
        status_str = "⚠";
        color = ANSI_FG_YELLOW;
        break;
    case UI_STATUS_ERROR:
        status_str = "✗";
        color = ANSI_FG_RED;
        break;
    case UI_STATUS_INFO:
        status_str = "ℹ";
        color = ANSI_FG_BLUE;
        break;
    default:
        status_str = "?";
        color = ANSI_FG_WHITE;
    }

    printf("%s%s%s %s\n", color, status_str, ANSI_RESET, label);
}

void ui_draw_status_line(const char *message, ui_status_t status)
{
    if (!message)
        return;

    const char *color;

    switch (status)
    {
    case UI_STATUS_OK:
        color = ANSI_BG_GREEN ANSI_FG_BLACK;
        break;
    case UI_STATUS_WARNING:
        color = ANSI_BG_YELLOW ANSI_FG_BLACK;
        break;
    case UI_STATUS_ERROR:
        color = ANSI_BG_RED ANSI_FG_WHITE;
        break;
    case UI_STATUS_INFO:
        color = ANSI_BG_BLUE ANSI_FG_WHITE;
        break;
    default:
        color = ANSI_BG_WHITE ANSI_FG_BLACK;
    }

    printf("%s %s %s\n", color, message, ANSI_RESET);
}

/* ============================================================================
 * MENU FUNCTIONS
 * =========================================================================== */

ui_menu_t ui_menu_create(const char *title)
{
    ui_menu_t menu;

    memset(&menu, 0, sizeof(ui_menu_t));
    if (title)
    {
        strncpy(menu.title, title, sizeof(menu.title) - 1);
    }

    menu.item_count = 0;
    menu.selected = 0;

    return menu;
}

int ui_menu_add_item(ui_menu_t *menu, const char *label, int id)
{
    if (!menu || !label || menu->item_count >= 20)
        return -1;

    int idx = menu->item_count;
    menu->items[idx].id = id;
    menu->items[idx].enabled = true;
    strncpy(menu->items[idx].label, label, sizeof(menu->items[idx].label) - 1);

    menu->item_count++;
    return idx;
}

void ui_menu_draw(const ui_menu_t *menu)
{
    if (!menu)
        return;

    if (menu->title[0])
    {
        printf("\n%s%s%s\n", ANSI_BOLD, menu->title, ANSI_RESET);
        printf("─────────────────\n");
    }

    for (int i = 0; i < menu->item_count; i++)
    {
        if (i == menu->selected)
        {
            printf("%s→ %s%s\n", ANSI_FG_CYAN, menu->items[i].label, ANSI_RESET);
        }
        else
        {
            printf("  %s\n", menu->items[i].label);
        }
    }
    printf("\n");
}

void ui_menu_select_prev(ui_menu_t *menu)
{
    if (menu && menu->selected > 0)
    {
        menu->selected--;
    }
}

void ui_menu_select_next(ui_menu_t *menu)
{
    if (menu && menu->selected < menu->item_count - 1)
    {
        menu->selected++;
    }
}

int ui_menu_get_selected(const ui_menu_t *menu)
{
    if (!menu || menu->selected < 0 || menu->selected >= menu->item_count)
        return -1;

    return menu->items[menu->selected].id;
}

/* ============================================================================
 * LAYOUT FUNCTIONS
 * =========================================================================== */

void ui_clear_screen(void)
{
    printf("%s%s", ANSI_CLEAR_SCREEN, ANSI_CURSOR_HOME);
    fflush(stdout);
}

void ui_cursor_move(int x, int y)
{
    printf("\x1b[%d;%dH", y, x);
    fflush(stdout);
}

void ui_cursor_show(void)
{
    printf("%s", ANSI_CURSOR_SHOW);
    fflush(stdout);
}

void ui_cursor_hide(void)
{
    printf("%s", ANSI_CURSOR_HIDE);
    fflush(stdout);
}

void ui_get_screen_size(int *width, int *height)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
#endif
    g_screen_width = *width;
    g_screen_height = *height;
}

/* ============================================================================
 * COLOR FUNCTIONS
 * =========================================================================== */

void ui_set_color(int fg, int bg)
{
    printf("\x1b[%d;%dm", fg, bg);
    fflush(stdout);
}

void ui_reset_color(void)
{
    printf("%s", ANSI_RESET);
    fflush(stdout);
}

void ui_set_bold(bool bold)
{
    printf("%s", bold ? ANSI_BOLD : ANSI_RESET);
    fflush(stdout);
}
