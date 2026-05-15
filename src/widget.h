/**
 * SimpleOS Widget System Header
 * Interactive UI components
 */

#ifndef SIMPLEOS_WIDGET_H
#define SIMPLEOS_WIDGET_H

#include <stdint.h>
#include <stdbool.h>
#include "window_manager.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * WIDGET TYPES
     * =========================================================================== */

    /**
     * Widget handle
     */
    typedef struct widget widget_t;

    /**
     * Widget container/panel
     */
    typedef struct widget_container widget_container_t;

    /**
     * Widget type
     */
    typedef enum
    {
        WIDGET_TYPE_CONTAINER = 0,
        WIDGET_TYPE_BUTTON = 1,
        WIDGET_TYPE_LABEL = 2,
        WIDGET_TYPE_TEXT_INPUT = 3,
        WIDGET_TYPE_CHECKBOX = 4,
        WIDGET_TYPE_RADIO = 5,
        WIDGET_TYPE_PROGRESS_BAR = 6,
        WIDGET_TYPE_SLIDER = 7,
        WIDGET_TYPE_LIST_BOX = 8,
        WIDGET_TYPE_COMBO_BOX = 9,
        WIDGET_TYPE_PANEL = 10
    } widget_type_t;

    /**
     * Widget state
     */
    typedef enum
    {
        WIDGET_STATE_NORMAL = 0,
        WIDGET_STATE_FOCUSED = 1,
        WIDGET_STATE_DISABLED = 2,
        WIDGET_STATE_PRESSED = 3,
        WIDGET_STATE_HOVERED = 4
    } widget_state_t;

    /**
     * Widget event types
     */
    typedef enum
    {
        WIDGET_EVENT_CLICK = 0,
        WIDGET_EVENT_FOCUS = 1,
        WIDGET_EVENT_BLUR = 2,
        WIDGET_EVENT_CHANGE = 3,
        WIDGET_EVENT_KEY_PRESS = 4,
        WIDGET_EVENT_MOUSE_ENTER = 5,
        WIDGET_EVENT_MOUSE_LEAVE = 6
    } widget_event_type_t;

    /**
     * Widget event
     */
    typedef struct
    {
        widget_event_type_t type;
        widget_t *widget;
        int data; // Generic data (key code, etc.)
    } widget_event_t;

    /**
     * Widget callback function
     */
    typedef void (*widget_callback_t)(const widget_event_t *event);

    /* ============================================================================
     * BUTTON WIDGET
     * =========================================================================== */

    /**
     * Create button widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param label Button label
     * @return Widget handle or NULL on error
     */
    widget_t *widget_button_create(widget_container_t *parent, int x, int y,
                                   int width, int height, const char *label);

    /**
     * Set button label
     * @param widget Button widget
     * @param label Label text
     */
    void widget_button_set_label(widget_t *widget, const char *label);

    /**
     * Get button label
     * @param widget Button widget
     * @return Label text
     */
    const char *widget_button_get_label(widget_t *widget);

    /* ============================================================================
     * LABEL WIDGET
     * =========================================================================== */

    /**
     * Create label widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param text Label text
     * @return Widget handle or NULL on error
     */
    widget_t *widget_label_create(widget_container_t *parent, int x, int y, const char *text);

    /**
     * Set label text
     * @param widget Label widget
     * @param text Text
     */
    void widget_label_set_text(widget_t *widget, const char *text);

    /**
     * Get label text
     * @param widget Label widget
     * @return Text
     */
    const char *widget_label_get_text(widget_t *widget);

    /* ============================================================================
     * TEXT INPUT WIDGET
     * =========================================================================== */

    /**
     * Create text input widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height (usually 1)
     * @return Widget handle or NULL on error
     */
    widget_t *widget_text_input_create(widget_container_t *parent, int x, int y,
                                       int width, int height);

    /**
     * Set text input text
     * @param widget Text input widget
     * @param text Text
     */
    void widget_text_input_set_text(widget_t *widget, const char *text);

    /**
     * Get text input text
     * @param widget Text input widget
     * @return Text
     */
    const char *widget_text_input_get_text(widget_t *widget);

    /**
     * Insert character at cursor
     * @param widget Text input widget
     * @param c Character to insert
     */
    void widget_text_input_insert_char(widget_t *widget, char c);

    /**
     * Delete character at cursor
     * @param widget Text input widget
     */
    void widget_text_input_delete_char(widget_t *widget);

    /**
     * Set cursor position
     * @param widget Text input widget
     * @param pos Position
     */
    void widget_text_input_set_cursor(widget_t *widget, int pos);

    /* ============================================================================
     * CHECKBOX WIDGET
     * =========================================================================== */

    /**
     * Create checkbox widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param label Label text
     * @return Widget handle or NULL on error
     */
    widget_t *widget_checkbox_create(widget_container_t *parent, int x, int y, const char *label);

    /**
     * Set checkbox state
     * @param widget Checkbox widget
     * @param checked Checked state
     */
    void widget_checkbox_set_checked(widget_t *widget, bool checked);

    /**
     * Get checkbox state
     * @param widget Checkbox widget
     * @return Checked state
     */
    bool widget_checkbox_is_checked(widget_t *widget);

    /* ============================================================================
     * SLIDER WIDGET
     * =========================================================================== */

    /**
     * Create slider widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param min_value Minimum value
     * @param max_value Maximum value
     * @return Widget handle or NULL on error
     */
    widget_t *widget_slider_create(widget_container_t *parent, int x, int y, int width,
                                   int min_value, int max_value);

    /**
     * Set slider value
     * @param widget Slider widget
     * @param value Value
     */
    void widget_slider_set_value(widget_t *widget, int value);

    /**
     * Get slider value
     * @param widget Slider widget
     * @return Value
     */
    int widget_slider_get_value(widget_t *widget);

    /* ============================================================================
     * PROGRESS BAR WIDGET
     * =========================================================================== */

    /**
     * Create progress bar widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @return Widget handle or NULL on error
     */
    widget_t *widget_progress_bar_create(widget_container_t *parent, int x, int y,
                                         int width, int height);

    /**
     * Set progress value
     * @param widget Progress bar widget
     * @param value Value (0-100)
     */
    void widget_progress_bar_set_value(widget_t *widget, int value);

    /**
     * Get progress value
     * @param widget Progress bar widget
     * @return Value
     */
    int widget_progress_bar_get_value(widget_t *widget);

    /* ============================================================================
     * CONTAINER WIDGET
     * =========================================================================== */

    /**
     * Create container widget
     * @param parent Parent widget/container
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @return Widget container handle or NULL on error
     */
    widget_container_t *widget_container_create(widget_container_t *parent, int x, int y,
                                                int width, int height);

    /**
     * Destroy container and all children
     * @param container Container handle
     */
    void widget_container_destroy(widget_container_t *container);

    /**
     * Draw container and children
     * @param container Container handle
     * @param renderer Renderer handle
     */
    void widget_container_draw(widget_container_t *container, renderer_t *renderer);

    /**
     * Get child widget by index
     * @param container Container handle
     * @param index Child index
     * @return Widget handle or NULL
     */
    widget_t *widget_container_get_child(widget_container_t *container, int index);

    /* ============================================================================
     * GENERIC WIDGET FUNCTIONS
     * =========================================================================== */

    /**
     * Destroy widget
     * @param widget Widget handle
     */
    void widget_destroy(widget_t *widget);

    /**
     * Set widget position
     * @param widget Widget handle
     * @param x X position
     * @param y Y position
     */
    void widget_set_position(widget_t *widget, int x, int y);

    /**
     * Set widget size
     * @param widget Widget handle
     * @param width Width
     * @param height Height
     */
    void widget_set_size(widget_t *widget, int width, int height);

    /**
     * Set widget state
     * @param widget Widget handle
     * @param state Widget state
     */
    void widget_set_state(widget_t *widget, widget_state_t state);

    /**
     * Get widget state
     * @param widget Widget handle
     * @return Widget state
     */
    widget_state_t widget_get_state(widget_t *widget);

    /**
     * Enable widget
     * @param widget Widget handle
     */
    void widget_enable(widget_t *widget);

    /**
     * Disable widget
     * @param widget Widget handle
     */
    void widget_disable(widget_t *widget);

    /**
     * Show widget
     * @param widget Widget handle
     */
    void widget_show(widget_t *widget);

    /**
     * Hide widget
     * @param widget Widget handle
     */
    void widget_hide(widget_t *widget);

    /**
     * Set widget callback
     * @param widget Widget handle
     * @param callback Callback function
     */
    void widget_set_callback(widget_t *widget, widget_callback_t callback);

    /**
     * Get widget type
     * @param widget Widget handle
     * @return Widget type
     */
    widget_type_t widget_get_type(widget_t *widget);

    /**
     * Draw widget
     * @param widget Widget handle
     * @param renderer Renderer handle
     */
    void widget_draw(widget_t *widget, renderer_t *renderer);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_WIDGET_H */
