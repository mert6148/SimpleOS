/**
 * SimpleOS Window Manager Implementation
 * Multi-window management system
 */

#include "window_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * WINDOW STRUCTURE
 * =========================================================================== */

struct window
{
    window_manager_t *manager;
    char title[256];
    rect_t bounds;
    window_state_t state;
    window_flags_t flags;
    window_callback_t callback;
    bool dirty;
    void *content;
    int z_index;
    bool focused;
};

/* ============================================================================
 * WINDOW MANAGER STRUCTURE
 * =========================================================================== */

#define MAX_WINDOWS 64

struct window_manager
{
    int screen_width;
    int screen_height;
    window_t *windows[MAX_WINDOWS];
    int window_count;
    window_t *focused_window;
    bool running;
    int z_counter;
};

/* ============================================================================
 * WINDOW MANAGER LIFECYCLE
 * =========================================================================== */

window_manager_t *window_manager_create(int width, int height)
{
    window_manager_t *wm = (window_manager_t *)malloc(sizeof(window_manager_t));
    if (!wm)
        return NULL;

    memset(wm, 0, sizeof(window_manager_t));
    wm->screen_width = width > 0 ? width : 80;
    wm->screen_height = height > 0 ? height : 24;
    wm->window_count = 0;
    wm->focused_window = NULL;
    wm->running = false;
    wm->z_counter = 0;

    return wm;
}

void window_manager_destroy(window_manager_t *wm)
{
    if (!wm)
        return;

    // Destroy all windows
    for (int i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i])
        {
            free(wm->windows[i]);
        }
    }

    free(wm);
}

int window_manager_init(window_manager_t *wm)
{
    if (!wm)
        return -1;

    wm->running = true;
    wm->window_count = 0;
    wm->z_counter = 0;

    return 0;
}

void window_manager_shutdown(window_manager_t *wm)
{
    if (wm)
    {
        wm->running = false;
    }
}

int window_manager_update(window_manager_t *wm)
{
    if (!wm || !wm->running)
        return -1;

    // Update all windows
    for (int i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i] && wm->windows[i]->dirty)
        {
            // Window needs redrawing
            window_event_t event;
            event.type = WINDOW_EVENT_PAINT;
            event.window = wm->windows[i];
            event.bounds = wm->windows[i]->bounds;

            if (wm->windows[i]->callback)
            {
                wm->windows[i]->callback(&event);
            }

            wm->windows[i]->dirty = false;
        }
    }

    return 0;
}

int window_manager_render(window_manager_t *wm)
{
    if (!wm)
        return -1;

    // Sort windows by z-index
    for (int i = 0; i < wm->window_count - 1; i++)
    {
        for (int j = i + 1; j < wm->window_count; j++)
        {
            if (wm->windows[i]->z_index > wm->windows[j]->z_index)
            {
                window_t *temp = wm->windows[i];
                wm->windows[i] = wm->windows[j];
                wm->windows[j] = temp;
            }
        }
    }

    // Render all visible windows
    for (int i = 0; i < wm->window_count; i++)
    {
        window_t *win = wm->windows[i];
        if (!win || win->state == WINDOW_STATE_HIDDEN)
            continue;

        // Draw window frame
        printf("\x1b[%d;%dH", win->bounds.y, win->bounds.x);
        printf("┌");
        for (int j = 0; j < win->bounds.width - 2; j++)
            printf("─");
        printf("┐");

        // Draw title bar
        if (win->title[0])
        {
            printf("\x1b[%d;%dH", win->bounds.y, win->bounds.x + 2);
            int title_len = strlen(win->title);
            if (title_len > win->bounds.width - 4)
                title_len = win->bounds.width - 4;
            for (int j = 0; j < title_len; j++)
                printf("%c", win->title[j]);
        }

        // Draw vertical borders
        for (int row = 1; row < win->bounds.height - 1; row++)
        {
            printf("\x1b[%d;%dH", win->bounds.y + row, win->bounds.x);
            printf("│");
            printf("\x1b[%d;%dH", win->bounds.y + row, win->bounds.x + win->bounds.width - 1);
            printf("│");
        }

        // Draw bottom border
        printf("\x1b[%d;%dH", win->bounds.y + win->bounds.height - 1, win->bounds.x);
        printf("└");
        for (int j = 0; j < win->bounds.width - 2; j++)
            printf("─");
        printf("┘");
    }

    fflush(stdout);
    return 0;
}

void window_manager_get_screen_size(window_manager_t *wm, int *width, int *height)
{
    if (wm)
    {
        if (width)
            *width = wm->screen_width;
        if (height)
            *height = wm->screen_height;
    }
}

/* ============================================================================
 * WINDOW LIFECYCLE
 * =========================================================================== */

window_t *window_create(window_manager_t *wm, const char *title,
                        int x, int y, int width, int height, window_flags_t flags)
{
    if (!wm || wm->window_count >= MAX_WINDOWS)
        return NULL;

    window_t *window = (window_t *)malloc(sizeof(window_t));
    if (!window)
        return NULL;

    memset(window, 0, sizeof(window_t));
    window->manager = wm;

    if (title)
    {
        strncpy(window->title, title, sizeof(window->title) - 1);
    }

    window->bounds.x = x;
    window->bounds.y = y;
    window->bounds.width = width > 0 ? width : 20;
    window->bounds.height = height > 0 ? height : 10;
    window->flags = flags;
    window->state = WINDOW_STATE_VISIBLE;
    window->z_index = wm->z_counter++;
    window->focused = false;
    window->dirty = true;

    // Add to window list
    wm->windows[wm->window_count++] = window;

    // Fire create event
    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_CREATE;
        event.window = window;
        event.bounds = window->bounds;
        window->callback(&event);
    }

    return window;
}

void window_destroy(window_t *window)
{
    if (!window || !window->manager)
        return;

    window_manager_t *wm = window->manager;

    // Fire destroy event
    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_DESTROY;
        event.window = window;
        window->callback(&event);
    }

    // Remove from window list
    for (int i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i] == window)
        {
            for (int j = i; j < wm->window_count - 1; j++)
            {
                wm->windows[j] = wm->windows[j + 1];
            }
            wm->window_count--;
            break;
        }
    }

    if (wm->focused_window == window)
    {
        wm->focused_window = NULL;
    }

    free(window);
}

void window_show(window_t *window)
{
    if (!window)
        return;

    window->state = WINDOW_STATE_VISIBLE;
    window->dirty = true;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_SHOW;
        event.window = window;
        window->callback(&event);
    }
}

void window_hide(window_t *window)
{
    if (!window)
        return;

    window->state = WINDOW_STATE_HIDDEN;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_HIDE;
        event.window = window;
        window->callback(&event);
    }
}

void window_set_title(window_t *window, const char *title)
{
    if (!window || !title)
        return;

    strncpy(window->title, title, sizeof(window->title) - 1);
    window->dirty = true;
}

const char *window_get_title(window_t *window)
{
    if (!window)
        return "";

    return window->title;
}

void window_set_position(window_t *window, int x, int y)
{
    if (!window)
        return;

    window->bounds.x = x;
    window->bounds.y = y;
    window->dirty = true;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_MOVE;
        event.window = window;
        event.bounds = window->bounds;
        window->callback(&event);
    }
}

void window_set_size(window_t *window, int width, int height)
{
    if (!window)
        return;

    window->bounds.width = width;
    window->bounds.height = height;
    window->dirty = true;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_RESIZE;
        event.window = window;
        event.bounds = window->bounds;
        window->callback(&event);
    }
}

rect_t window_get_bounds(window_t *window)
{
    rect_t empty = {0, 0, 0, 0};
    if (!window)
        return empty;

    return window->bounds;
}

void window_set_state(window_t *window, window_state_t state)
{
    if (!window)
        return;

    window->state = state;
    window->dirty = true;
}

window_state_t window_get_state(window_t *window)
{
    if (!window)
        return WINDOW_STATE_HIDDEN;

    return window->state;
}

void window_focus(window_t *window)
{
    if (!window || !window->manager)
        return;

    window_manager_t *wm = window->manager;

    // Unfocus previous window
    if (wm->focused_window && wm->focused_window != window)
    {
        wm->focused_window->focused = false;
        window_event_t event;
        event.type = WINDOW_EVENT_BLUR;
        event.window = wm->focused_window;
        if (wm->focused_window->callback)
            wm->focused_window->callback(&event);
    }

    // Focus new window
    wm->focused_window = window;
    window->focused = true;
    window->z_index = wm->z_counter++;
    window->dirty = true;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_FOCUS;
        event.window = window;
        window->callback(&event);
    }
}

bool window_is_focused(window_t *window)
{
    if (!window)
        return false;

    return window->focused;
}

void window_set_callback(window_t *window, window_callback_t callback)
{
    if (window)
    {
        window->callback = callback;
    }
}

void window_request_close(window_t *window)
{
    if (!window)
        return;

    if (window->callback)
    {
        window_event_t event;
        event.type = WINDOW_EVENT_CLOSE;
        event.window = window;
        window->callback(&event);
    }

    window_destroy(window);
}

void window_invalidate(window_t *window)
{
    if (window)
    {
        window->dirty = true;
    }
}

void window_clear(window_t *window)
{
    if (!window)
        return;

    // Clear window content area
    for (int row = 1; row < window->bounds.height - 1; row++)
    {
        printf("\x1b[%d;%dH", window->bounds.y + row, window->bounds.x + 1);
        for (int col = 0; col < window->bounds.width - 2; col++)
        {
            putchar(' ');
        }
    }

    window->dirty = false;
}
