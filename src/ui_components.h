/**
 * SimpleOS UI Components Header
 * UI elements for frontend dashboard and terminal
 */

#ifndef SIMPLEOS_UI_COMPONENTS_H
#define SIMPLEOS_UI_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * UI COMPONENT TYPES
     * =========================================================================== */

    /**
     * Panel component for displaying information
     */
    typedef struct
    {
        char title[128];
        char content[2048];
        int x;
        int y;
        int width;
        int height;
        bool visible;
        bool border;
    } ui_panel_t;

    /**
     * Progress bar component
     */
    typedef struct
    {
        char label[64];
        float value; // 0.0 to 1.0
        int width;
        char fill_char;
        char empty_char;
    } ui_progress_bar_t;

    /**
     * Table cell
     */
    typedef struct
    {
        char value[256];
        int width;
        bool right_align;
    } ui_cell_t;

    /**
     * Table row
     */
    typedef struct
    {
        ui_cell_t cells[10];
        int cell_count;
    } ui_table_row_t;

    /**
     * Table component
     */
    typedef struct
    {
        char title[128];
        ui_table_row_t headers;
        ui_table_row_t rows[32];
        int row_count;
        int total_width;
    } ui_table_t;

    /**
     * Status indicator
     */
    typedef enum
    {
        UI_STATUS_OK = 0,
        UI_STATUS_WARNING = 1,
        UI_STATUS_ERROR = 2,
        UI_STATUS_INFO = 3
    } ui_status_t;

    /**
     * Menu item
     */
    typedef struct
    {
        char label[64];
        int id;
        bool enabled;
    } ui_menu_item_t;

    /**
     * Menu component
     */
    typedef struct
    {
        char title[128];
        ui_menu_item_t items[20];
        int item_count;
        int selected;
    } ui_menu_t;

    /* ============================================================================
     * PANEL FUNCTIONS
     * =========================================================================== */

    /**
     * Create panel
     * @param title Panel title
     * @param x X position
     * @param y Y position
     * @param width Width in characters
     * @param height Height in lines
     * @return Initialized panel
     */
    ui_panel_t ui_panel_create(const char *title, int x, int y, int width, int height);

    /**
     * Set panel content
     * @param panel Panel pointer
     * @param content Content string
     */
    void ui_panel_set_content(ui_panel_t *panel, const char *content);

    /**
     * Append to panel content
     * @param panel Panel pointer
     * @param text Text to append
     */
    void ui_panel_append(ui_panel_t *panel, const char *text);

    /**
     * Draw panel
     * @param panel Panel pointer
     */
    void ui_panel_draw(const ui_panel_t *panel);

    /**
     * Clear panel
     * @param panel Panel pointer
     */
    void ui_panel_clear(ui_panel_t *panel);

    /* ============================================================================
     * PROGRESS BAR FUNCTIONS
     * =========================================================================== */

    /**
     * Create progress bar
     * @param label Progress bar label
     * @param width Width in characters
     * @return Initialized progress bar
     */
    ui_progress_bar_t ui_progress_bar_create(const char *label, int width);

    /**
     * Set progress value
     * @param bar Progress bar pointer
     * @param value Value 0.0-1.0
     */
    void ui_progress_bar_set_value(ui_progress_bar_t *bar, float value);

    /**
     * Draw progress bar
     * @param bar Progress bar pointer
     */
    void ui_progress_bar_draw(const ui_progress_bar_t *bar);

    /* ============================================================================
     * TABLE FUNCTIONS
     * =========================================================================== */

    /**
     * Create table
     * @param title Table title
     * @return Initialized table
     */
    ui_table_t ui_table_create(const char *title);

    /**
     * Add column header
     * @param table Table pointer
     * @param header Header text
     * @param width Column width
     */
    void ui_table_add_header(ui_table_t *table, const char *header, int width);

    /**
     * Add table row
     * @param table Table pointer
     * @param values Array of cell values
     * @param count Number of cells
     * @return Row index or -1 on error
     */
    int ui_table_add_row(ui_table_t *table, const char **values, int count);

    /**
     * Update table cell
     * @param table Table pointer
     * @param row Row index
     * @param col Column index
     * @param value New value
     */
    void ui_table_set_cell(ui_table_t *table, int row, int col, const char *value);

    /**
     * Draw table
     * @param table Table pointer
     */
    void ui_table_draw(const ui_table_t *table);

    /**
     * Clear table
     * @param table Table pointer
     */
    void ui_table_clear(ui_table_t *table);

    /* ============================================================================
     * STATUS FUNCTIONS
     * =========================================================================== */

    /**
     * Draw status indicator
     * @param label Status label
     * @param status Status type
     */
    void ui_draw_status(const char *label, ui_status_t status);

    /**
     * Draw status line at bottom
     * @param message Status message
     * @param status Status type
     */
    void ui_draw_status_line(const char *message, ui_status_t status);

    /* ============================================================================
     * MENU FUNCTIONS
     * =========================================================================== */

    /**
     * Create menu
     * @param title Menu title
     * @return Initialized menu
     */
    ui_menu_t ui_menu_create(const char *title);

    /**
     * Add menu item
     * @param menu Menu pointer
     * @param label Item label
     * @param id Item ID
     * @return Item index or -1 on error
     */
    int ui_menu_add_item(ui_menu_t *menu, const char *label, int id);

    /**
     * Draw menu
     * @param menu Menu pointer
     */
    void ui_menu_draw(const ui_menu_t *menu);

    /**
     * Move selection up
     * @param menu Menu pointer
     */
    void ui_menu_select_prev(ui_menu_t *menu);

    /**
     * Move selection down
     * @param menu Menu pointer
     */
    void ui_menu_select_next(ui_menu_t *menu);

    /**
     * Get selected item ID
     * @param menu Menu pointer
     * @return Selected item ID
     */
    int ui_menu_get_selected(const ui_menu_t *menu);

    /* ============================================================================
     * LAYOUT FUNCTIONS
     * =========================================================================== */

    /**
     * Clear screen
     */
    void ui_clear_screen(void);

    /**
     * Move cursor
     * @param x X position
     * @param y Y position
     */
    void ui_cursor_move(int x, int y);

    /**
     * Show cursor
     */
    void ui_cursor_show(void);

    /**
     * Hide cursor
     */
    void ui_cursor_hide(void);

    /**
     * Get screen dimensions
     * @param width Pointer to receive width
     * @param height Pointer to receive height
     */
    void ui_get_screen_size(int *width, int *height);

    /* ============================================================================
     * COLOR FUNCTIONS
     * =========================================================================== */

    /**
     * Set text color (ANSI)
     * @param fg Foreground color (30-37)
     * @param bg Background color (40-47)
     */
    void ui_set_color(int fg, int bg);

    /**
     * Reset colors to default
     */
    void ui_reset_color(void);

    /**
     * Set bold attribute
     * @param bold True to enable bold
     */
    void ui_set_bold(bool bold);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_UI_COMPONENTS_H */
