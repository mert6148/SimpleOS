/**
 * SimpleOS Renderer Header
 * Graphics and text rendering engine
 */

#ifndef SIMPLEOS_RENDERER_H
#define SIMPLEOS_RENDERER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * RENDERER TYPES
     * =========================================================================== */

    /**
     * Renderer handle
     */
    typedef struct renderer renderer_t;

    /**
     * Color definition (RGB or ANSI)
     */
    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } color_t;

    /**
     * Predefined colors
     */
    typedef enum
    {
        COLOR_BLACK = 0,
        COLOR_RED = 1,
        COLOR_GREEN = 2,
        COLOR_YELLOW = 3,
        COLOR_BLUE = 4,
        COLOR_MAGENTA = 5,
        COLOR_CYAN = 6,
        COLOR_WHITE = 7,
        COLOR_GRAY = 8
    } ansi_color_t;

    /**
     * Text alignment
     */
    typedef enum
    {
        ALIGN_LEFT = 0,
        ALIGN_CENTER = 1,
        ALIGN_RIGHT = 2
    } text_align_t;

    /**
     * Line style
     */
    typedef enum
    {
        LINE_SOLID = 0,
        LINE_DASHED = 1,
        LINE_DOTTED = 2,
        LINE_DOUBLE = 3
    } line_style_t;

    /**
     * Point structure
     */
    typedef struct
    {
        int x;
        int y;
    } point_t;

    /**
     * Pixel structure
     */
    typedef struct
    {
        int x;
        int y;
        color_t color;
        char character;
        bool bold;
    } pixel_t;

    /* ============================================================================
     * RENDERER FUNCTIONS
     * =========================================================================== */

    /**
     * Create renderer
     * @param width Screen width
     * @param height Screen height
     * @return Renderer handle or NULL on error
     */
    renderer_t *renderer_create(int width, int height);

    /**
     * Destroy renderer
     * @param renderer Renderer handle
     */
    void renderer_destroy(renderer_t *renderer);

    /**
     * Initialize renderer
     * @param renderer Renderer handle
     * @return 0 on success, -1 on error
     */
    int renderer_init(renderer_t *renderer);

    /**
     * Clear screen
     * @param renderer Renderer handle
     * @param bg_color Background color
     */
    void renderer_clear(renderer_t *renderer, color_t bg_color);

    /**
     * Present/flush rendered content to display
     * @param renderer Renderer handle
     * @return 0 on success, -1 on error
     */
    int renderer_present(renderer_t *renderer);

    /**
     * Get renderer size
     * @param renderer Renderer handle
     * @param width Pointer to receive width
     * @param height Pointer to receive height
     */
    void renderer_get_size(renderer_t *renderer, int *width, int *height);

    /* ============================================================================
     * DRAWING PRIMITIVES
     * =========================================================================== */

    /**
     * Set drawing color
     * @param renderer Renderer handle
     * @param fg Foreground color (ANSI)
     * @param bg Background color (ANSI)
     */
    void renderer_set_color(renderer_t *renderer, ansi_color_t fg, ansi_color_t bg);

    /**
     * Draw pixel
     * @param renderer Renderer handle
     * @param x X coordinate
     * @param y Y coordinate
     * @param c Character to draw
     */
    void renderer_draw_pixel(renderer_t *renderer, int x, int y, char c);

    /**
     * Draw line
     * @param renderer Renderer handle
     * @param x1 Start X
     * @param y1 Start Y
     * @param x2 End X
     * @param y2 End Y
     * @param c Character to use
     */
    void renderer_draw_line(renderer_t *renderer, int x1, int y1, int x2, int y2, char c);

    /**
     * Draw rectangle (filled)
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param c Character to fill with
     */
    void renderer_draw_rect_filled(renderer_t *renderer, int x, int y, int width, int height, char c);

    /**
     * Draw rectangle (outline)
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     */
    void renderer_draw_rect_outline(renderer_t *renderer, int x, int y, int width, int height);

    /**
     * Draw box with borders
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param title Optional title
     */
    void renderer_draw_box(renderer_t *renderer, int x, int y, int width, int height, const char *title);

    /* ============================================================================
     * TEXT RENDERING
     * =========================================================================== */

    /**
     * Draw text
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param text Text to draw
     */
    void renderer_draw_text(renderer_t *renderer, int x, int y, const char *text);

    /**
     * Draw text with alignment
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param width Width area
     * @param align Alignment
     * @param text Text to draw
     */
    void renderer_draw_text_aligned(renderer_t *renderer, int x, int y, int width,
                                    text_align_t align, const char *text);

    /**
     * Draw formatted text (printf-style)
     * @param renderer Renderer handle
     * @param x X position
     * @param y Y position
     * @param fmt Format string
     * @param ... Variable arguments
     */
    void renderer_draw_printf(renderer_t *renderer, int x, int y, const char *fmt, ...);

    /**
     * Measure text width
     * @param text Text to measure
     * @return Width in characters
     */
    int renderer_text_width(const char *text);

    /* ============================================================================
     * STYLES & ATTRIBUTES
     * =========================================================================== */

    /**
     * Set text bold
     * @param renderer Renderer handle
     * @param bold Enable bold
     */
    void renderer_set_bold(renderer_t *renderer, bool bold);

    /**
     * Set text underline
     * @param renderer Renderer handle
     * @param underline Enable underline
     */
    void renderer_set_underline(renderer_t *renderer, bool underline);

    /**
     * Reset text attributes
     * @param renderer Renderer handle
     */
    void renderer_reset_attributes(renderer_t *renderer);

    /* ============================================================================
     * BUFFER OPERATIONS
     * =========================================================================== */

    /**
     * Get pixel at position
     * @param renderer Renderer handle
     * @param x X coordinate
     * @param y Y coordinate
     * @return Character at position
     */
    char renderer_get_char(renderer_t *renderer, int x, int y);

    /**
     * Save renderer state to file
     * @param renderer Renderer handle
     * @param filename Output filename
     * @return 0 on success, -1 on error
     */
    int renderer_save(renderer_t *renderer, const char *filename);

    /**
     * Load renderer state from file
     * @param renderer Renderer handle
     * @param filename Input filename
     * @return 0 on success, -1 on error
     */
    int renderer_load(renderer_t *renderer, const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_RENDERER_H */
