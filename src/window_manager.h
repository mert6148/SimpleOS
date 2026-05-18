/**
 * SimpleOS Window Manager Header
 * Multi-window management system for UI framework
 */

#ifndef SIMPLEOS_WINDOW_MANAGER_H
#define SIMPLEOS_WINDOW_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * WINDOW TYPES
     * =========================================================================== */

    /**
     * Window handle
     */
    typedef struct window_manager window_manager_t;
    typedef struct window window_t;

    /**
     * Window flags
     */
    typedef enum
    {
        WINDOW_NORMAL = 0,
        WINDOW_MODAL = 1,
        WINDOW_FULLSCREEN = 2,
        WINDOW_BORDERLESS = 4,
        WINDOW_RESIZABLE = 8,
        WINDOW_ALWAYS_ON_TOP = 16
    } window_flags_t;

    /**
     * Window state
     */
    typedef enum
    {
        WINDOW_STATE_HIDDEN = 0,
        WINDOW_STATE_VISIBLE = 1,
        WINDOW_STATE_MINIMIZED = 2,
        WINDOW_STATE_MAXIMIZED = 3,
        WINDOW_STATE_FOCUSED = 4
    } window_state_t;

    /**
     * Rectangle structure
     */
    typedef struct
    {
        int x;
        int y;
        int width;
        int height;
    } rect_t;

    /**
     * Window event types
     */
    typedef enum
    {
        WINDOW_EVENT_CREATE = 0,
        WINDOW_EVENT_DESTROY = 1,
        WINDOW_EVENT_SHOW = 2,
        WINDOW_EVENT_HIDE = 3,
        WINDOW_EVENT_FOCUS = 4,
        WINDOW_EVENT_BLUR = 5,
        WINDOW_EVENT_RESIZE = 6,
        WINDOW_EVENT_MOVE = 7,
        WINDOW_EVENT_CLOSE = 8,
        WINDOW_EVENT_PAINT = 9,
        WINDOW_EVENT_INPUT = 10
    } window_event_type_t;

    /**
     * Window event
     */
    typedef struct
    {
        window_event_type_t type;
        window_t *window;
        rect_t bounds;
        void *data;
    } window_event_t;

    /**
     * Window callback function
     */
    typedef void (*window_callback_t)(const window_event_t *event);

    /* ============================================================================
     * WINDOW MANAGER FUNCTIONS
     * =========================================================================== */

    /**
     * Create window manager
     * @param width Screen width
     * @param height Screen height
     * @return Window manager handle or NULL on error
     */
    window_manager_t *window_manager_create(int width, int height);

    /**
     * Destroy window manager
     * @param wm Window manager handle
     */
    void window_manager_destroy(window_manager_t *wm);

    /**
     * Initialize window manager
     * @param wm Window manager handle
     * @return 0 on success, -1 on error
     */
    int window_manager_init(window_manager_t *wm);

    /**
     * Shutdown window manager
     * @param wm Window manager handle
     */
    void window_manager_shutdown(window_manager_t *wm);

    /**
     * Process events and update windows
     * @param wm Window manager handle
     * @return 0 on success, -1 on error
     */
    int window_manager_update(window_manager_t *wm);

    /**
     * Render all windows
     * @param wm Window manager handle
     * @return 0 on success, -1 on error
     */
    int window_manager_render(window_manager_t *wm);

    /**
     * Get window manager screen size
     * @param wm Window manager handle
     * @param width Pointer to receive width
     * @param height Pointer to receive height
     */
    void window_manager_get_screen_size(window_manager_t *wm, int *width, int *height);

    /* ============================================================================
     * WINDOW FUNCTIONS
     * =========================================================================== */

    /**
     * Create window
     * @param wm Window manager handle
     * @param title Window title
     * @param x X position
     * @param y Y position
     * @param width Window width
     * @param height Window height
     * @param flags Window flags
     * @return Window handle or NULL on error
     */
    window_t *window_create(window_manager_t *wm, const char *title,
                            int x, int y, int width, int height, window_flags_t flags);

    /**
     * Destroy window
     * @param window Window handle
     */
    void window_destroy(window_t *window);

    /**
     * Show window
     * @param window Window handle
     */
    void window_show(window_t *window);

    /**
     * Hide window
     * @param window Window handle
     */
    void window_hide(window_t *window);

    /**
     * Set window title
     * @param window Window handle
     * @param title New title
     */
    void window_set_title(window_t *window, const char *title);

    /**
     * Get window title
     * @param window Window handle
     * @return Window title
     */
    const char *window_get_title(window_t *window);

    /**
     * Set window position
     * @param window Window handle
     * @param x X position
     * @param y Y position
     */
    void window_set_position(window_t *window, int x, int y);

    /**
     * Set window size
     * @param window Window handle
     * @param width Width
     * @param height Height
     */
    void window_set_size(window_t *window, int width, int height);

    /**
     * Get window bounds
     * @param window Window handle
     * @return Window bounds rectangle
     */
    rect_t window_get_bounds(window_t *window);

    /**
     * Set window state
     * @param window Window handle
     * @param state New state
     */
    void window_set_state(window_t *window, window_state_t state);

    /**
     * Get window state
     * @param window Window handle
     * @return Current window state
     */
    window_state_t window_get_state(window_t *window);

    /**
     * Focus window
     * @param window Window handle
     */
    void window_focus(window_t *window);

    /**
     * Check if window is focused
     * @param window Window handle
     * @return true if focused, false otherwise
     */
    bool window_is_focused(window_t *window);

    /**
     * Set window callback
     * @param window Window handle
     * @param callback Callback function
     */
    void window_set_callback(window_t *window, window_callback_t callback);

    /**
     * Request window close
     * @param window Window handle
     */
    void window_request_close(window_t *window);

    /**
     * Redraw window
     * @param window Window handle
     */
    void window_invalidate(window_t *window);

    /**
     * Clear window content
     * @param window Window handle
     */
    void window_clear(window_t *window);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_WINDOW_MANAGER_H */
