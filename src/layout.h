/**
 * SimpleOS Layout Management System Header
 * Automatic widget positioning and sizing
 */

#ifndef SIMPLEOS_LAYOUT_H
#define SIMPLEOS_LAYOUT_H

#include <stdint.h>
#include <stdbool.h>
#include "widget.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * LAYOUT TYPES
     * =========================================================================== */

    /**
     * Layout manager handle
     */
    typedef struct layout_manager layout_manager_t;

    /**
     * Layout constraints
     */
    typedef enum
    {
        LAYOUT_CONSTRAINT_FIXED = 0,     // Fixed size
        LAYOUT_CONSTRAINT_FILL = 1,      // Fill available space
        LAYOUT_CONSTRAINT_PREFERRED = 2, // Preferred size
        LAYOUT_CONSTRAINT_WRAP = 3       // Wrap content
    } layout_constraint_t;

    /**
     * Layout orientation
     */
    typedef enum
    {
        LAYOUT_ORIENTATION_VERTICAL = 0,
        LAYOUT_ORIENTATION_HORIZONTAL = 1
    } layout_orientation_t;

    /**
     * Box layout directions
     */
    typedef enum
    {
        LAYOUT_ALIGN_START = 0,
        LAYOUT_ALIGN_CENTER = 1,
        LAYOUT_ALIGN_END = 2,
        LAYOUT_ALIGN_STRETCH = 3
    } layout_align_t;

    /**
     * Size constraint
     */
    typedef struct
    {
        layout_constraint_t type;
        int size;
        int min_size;
        int max_size;
    } size_constraint_t;

    /**
     * Widget layout info
     */
    typedef struct
    {
        int x;
        int y;
        int width;
        int height;
        size_constraint_t width_constraint;
        size_constraint_t height_constraint;
        int margin_top;
        int margin_right;
        int margin_bottom;
        int margin_left;
        int padding;
    } widget_layout_t;

    /* ============================================================================
     * LAYOUT MANAGER
     * =========================================================================== */

    /**
     * Create layout manager
     * @param parent Parent container
     * @return Layout manager handle or NULL on error
     */
    layout_manager_t *layout_manager_create(widget_container_t *parent);

    /**
     * Destroy layout manager
     * @param manager Layout manager handle
     */
    void layout_manager_destroy(layout_manager_t *manager);

    /**
     * Set layout type
     * @param manager Layout manager handle
     * @param orientation Layout orientation
     */
    void layout_manager_set_orientation(layout_manager_t *manager, layout_orientation_t orientation);

    /**
     * Set layout alignment
     * @param manager Layout manager handle
     * @param align Main alignment
     * @param cross_align Cross alignment
     */
    void layout_manager_set_alignment(layout_manager_t *manager, layout_align_t align,
                                      layout_align_t cross_align);

    /**
     * Set layout spacing
     * @param manager Layout manager handle
     * @param spacing Spacing between widgets
     */
    void layout_manager_set_spacing(layout_manager_t *manager, int spacing);

    /**
     * Calculate layout
     * @param manager Layout manager handle
     * @return 0 on success, -1 on error
     */
    int layout_manager_calculate(layout_manager_t *manager);

    /**
     * Apply layout to widgets
     * @param manager Layout manager handle
     * @return 0 on success, -1 on error
     */
    int layout_manager_apply(layout_manager_t *manager);

    /* ============================================================================
     * WIDGET CONSTRAINTS
     * =========================================================================== */

    /**
     * Set widget width constraint
     * @param widget Widget handle
     * @param constraint Width constraint
     */
    void layout_set_width_constraint(widget_t *widget, size_constraint_t constraint);

    /**
     * Set widget height constraint
     * @param widget Widget handle
     * @param constraint Height constraint
     */
    void layout_set_height_constraint(widget_t *widget, size_constraint_t constraint);

    /**
     * Set widget margins
     * @param widget Widget handle
     * @param top Top margin
     * @param right Right margin
     * @param bottom Bottom margin
     * @param left Left margin
     */
    void layout_set_margins(widget_t *widget, int top, int right, int bottom, int left);

    /**
     * Set widget padding
     * @param widget Widget handle
     * @param padding Padding
     */
    void layout_set_padding(widget_t *widget, int padding);

    /* ============================================================================
     * BOX LAYOUT
     * =========================================================================== */

    /**
     * Create box layout
     * @param container Container widget
     * @param orientation Layout orientation
     * @return Layout manager handle
     */
    layout_manager_t *layout_box_create(widget_container_t *container, layout_orientation_t orientation);

    /* ============================================================================
     * GRID LAYOUT
     * =========================================================================== */

    /**
     * Create grid layout
     * @param container Container widget
     * @param rows Number of rows
     * @param cols Number of columns
     * @return Layout manager handle
     */
    layout_manager_t *layout_grid_create(widget_container_t *container, int rows, int cols);

    /**
     * Set widget grid position
     * @param widget Widget handle
     * @param row Row index
     * @param col Column index
     * @param row_span Row span
     * @param col_span Column span
     */
    void layout_grid_set_position(widget_t *widget, int row, int col, int row_span, int col_span);

    /* ============================================================================
     * DOCK LAYOUT
     * =========================================================================== */

    /**
     * Dock position
     */
    typedef enum
    {
        DOCK_TOP = 0,
        DOCK_BOTTOM = 1,
        DOCK_LEFT = 2,
        DOCK_RIGHT = 3,
        DOCK_CENTER = 4
    } dock_position_t;

    /**
     * Create dock layout
     * @param container Container widget
     * @return Layout manager handle
     */
    layout_manager_t *layout_dock_create(widget_container_t *container);

    /**
     * Set widget dock position
     * @param widget Widget handle
     * @param position Dock position
     */
    void layout_dock_set_position(widget_t *widget, dock_position_t position);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_LAYOUT_H */
