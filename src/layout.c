/**
 * SimpleOS Layout Management Implementation
 * Automatic widget positioning and sizing
 */

#include "layout.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * LAYOUT STRUCTURE
 * =========================================================================== */

typedef struct
{
    int row;
    int col;
    int row_span;
    int col_span;
    dock_position_t dock_pos;
} grid_info_t;

struct layout_manager
{
    widget_container_t *parent;
    layout_orientation_t orientation;
    layout_align_t align;
    layout_align_t cross_align;
    int spacing;
    int rows;
    int cols;
    widget_layout_t *widget_layouts;
    grid_info_t *grid_info;
    int max_widgets;
};

/* ============================================================================
 * LAYOUT MANAGER LIFECYCLE
 * =========================================================================== */

layout_manager_t *layout_manager_create(widget_container_t *parent)
{
    if (!parent)
        return NULL;

    layout_manager_t *manager = (layout_manager_t *)malloc(sizeof(layout_manager_t));
    if (!manager)
        return NULL;

    manager->parent = parent;
    manager->orientation = LAYOUT_ORIENTATION_VERTICAL;
    manager->align = LAYOUT_ALIGN_START;
    manager->cross_align = LAYOUT_ALIGN_START;
    manager->spacing = 0;
    manager->rows = 1;
    manager->cols = 1;
    manager->max_widgets = 256;

    manager->widget_layouts = (widget_layout_t *)malloc(sizeof(widget_layout_t) * manager->max_widgets);
    manager->grid_info = (grid_info_t *)malloc(sizeof(grid_info_t) * manager->max_widgets);

    if (!manager->widget_layouts || !manager->grid_info)
    {
        free(manager->widget_layouts);
        free(manager->grid_info);
        free(manager);
        return NULL;
    }

    memset(manager->widget_layouts, 0, sizeof(widget_layout_t) * manager->max_widgets);
    memset(manager->grid_info, 0, sizeof(grid_info_t) * manager->max_widgets);

    return manager;
}

void layout_manager_destroy(layout_manager_t *manager)
{
    if (!manager)
        return;

    free(manager->widget_layouts);
    free(manager->grid_info);
    free(manager);
}

void layout_manager_set_orientation(layout_manager_t *manager, layout_orientation_t orientation)
{
    if (manager)
    {
        manager->orientation = orientation;
    }
}

void layout_manager_set_alignment(layout_manager_t *manager, layout_align_t align,
                                  layout_align_t cross_align)
{
    if (manager)
    {
        manager->align = align;
        manager->cross_align = cross_align;
    }
}

void layout_manager_set_spacing(layout_manager_t *manager, int spacing)
{
    if (manager)
    {
        manager->spacing = spacing > 0 ? spacing : 0;
    }
}

int layout_manager_calculate(layout_manager_t *manager)
{
    if (!manager || !manager->parent)
        return -1;

    // This is a simplified implementation
    // In a real system, this would calculate sizes based on constraints
    return 0;
}

int layout_manager_apply(layout_manager_t *manager)
{
    if (!manager || !manager->parent)
        return -1;

    if (manager->orientation == LAYOUT_ORIENTATION_VERTICAL)
    {
        // Vertical box layout
        int x = manager->parent->base.bounds.x;
        int y = manager->parent->base.bounds.y;
        int total_height = 0;

        // First pass: calculate sizes
        for (int i = 0; i < manager->parent->child_count; i++)
        {
            widget_t *child = manager->parent->children[i];
            if (child)
            {
                widget_data_t *data = (widget_data_t *)child;
                total_height += data->bounds.height + manager->spacing;
            }
        }

        // Second pass: position widgets
        y = manager->parent->base.bounds.y;
        for (int i = 0; i < manager->parent->child_count; i++)
        {
            widget_t *child = manager->parent->children[i];
            if (child)
            {
                widget_set_position(child, x, y);
                widget_data_t *data = (widget_data_t *)child;
                y += data->bounds.height + manager->spacing;
            }
        }
    }
    else
    {
        // Horizontal box layout
        int x = manager->parent->base.bounds.x;
        int y = manager->parent->base.bounds.y;

        for (int i = 0; i < manager->parent->child_count; i++)
        {
            widget_t *child = manager->parent->children[i];
            if (child)
            {
                widget_set_position(child, x, y);
                widget_data_t *data = (widget_data_t *)child;
                x += data->bounds.width + manager->spacing;
            }
        }
    }

    return 0;
}

/* ============================================================================
 * WIDGET CONSTRAINTS
 * =========================================================================== */

void layout_set_width_constraint(widget_t *widget, size_constraint_t constraint)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    // Store constraint somewhere - simplified for now
    (void)constraint;
}

void layout_set_height_constraint(widget_t *widget, size_constraint_t constraint)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    // Store constraint somewhere - simplified for now
    (void)constraint;
}

void layout_set_margins(widget_t *widget, int top, int right, int bottom, int left)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    // Store margins in widget layout info
    (void)data;
}

void layout_set_padding(widget_t *widget, int padding)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    // Store padding in widget layout info
    (void)data;
}

/* ============================================================================
 * BOX LAYOUT
 * =========================================================================== */

layout_manager_t *layout_box_create(widget_container_t *container, layout_orientation_t orientation)
{
    layout_manager_t *manager = layout_manager_create(container);
    if (manager)
    {
        manager->orientation = orientation;
    }
    return manager;
}

/* ============================================================================
 * GRID LAYOUT
 * =========================================================================== */

layout_manager_t *layout_grid_create(widget_container_t *container, int rows, int cols)
{
    layout_manager_t *manager = layout_manager_create(container);
    if (manager)
    {
        manager->rows = rows > 0 ? rows : 1;
        manager->cols = cols > 0 ? cols : 1;
    }
    return manager;
}

void layout_grid_set_position(widget_t *widget, int row, int col, int row_span, int col_span)
{
    if (!widget)
        return;

    // In a full implementation, store grid position in widget's layout data
    (void)widget;
    (void)row;
    (void)col;
    (void)row_span;
    (void)col_span;
}

/* ============================================================================
 * DOCK LAYOUT
 * =========================================================================== */

layout_manager_t *layout_dock_create(widget_container_t *container)
{
    layout_manager_t *manager = layout_manager_create(container);
    if (manager)
    {
        // Dock layout specific initialization
        manager->orientation = LAYOUT_ORIENTATION_VERTICAL;
    }
    return manager;
}

void layout_dock_set_position(widget_t *widget, dock_position_t position)
{
    if (!widget)
        return;

    // Store dock position in widget's layout data
    (void)position;
}
