/**
 * SimpleOS Widget System Implementation
 * Interactive UI components
 */

#include "widget.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * WIDGET STRUCTURE
 * =========================================================================== */

typedef struct widget_data
{
    widget_type_t type;
    widget_state_t state;
    widget_container_t *parent;
    rect_t bounds;
    bool visible;
    bool enabled;
    widget_callback_t callback;
    void *data; // Widget-specific data
} widget_data_t;

typedef struct widget_container
{
    widget_data_t base;
    widget_t **children;
    int child_count;
    int max_children;
} widget_container_t;

/* ============================================================================
 * BUTTON WIDGET DATA
 * =========================================================================== */

typedef struct
{
    char label[256];
    bool is_pressed;
} button_data_t;

/* ============================================================================
 * LABEL WIDGET DATA
 * =========================================================================== */

typedef struct
{
    char text[512];
} label_data_t;

/* ============================================================================
 * TEXT INPUT WIDGET DATA
 * =========================================================================== */

typedef struct
{
    char text[1024];
    int cursor_pos;
    int max_length;
} text_input_data_t;

/* ============================================================================
 * CHECKBOX WIDGET DATA
 * =========================================================================== */

typedef struct
{
    char label[256];
    bool checked;
} checkbox_data_t;

/* ============================================================================
 * SLIDER WIDGET DATA
 * =========================================================================== */

typedef struct
{
    int min_value;
    int max_value;
    int current_value;
} slider_data_t;

/* ============================================================================
 * PROGRESS BAR WIDGET DATA
 * =========================================================================== */

typedef struct
{
    int min_value;
    int max_value;
    int current_value;
} progress_bar_data_t;

/* ============================================================================
 * BUTTON WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_button_create(widget_container_t *parent, int x, int y,
                               int width, int height, const char *label)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_BUTTON;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = width;
    data->bounds.height = height;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    button_data_t *button_data = (button_data_t *)malloc(sizeof(button_data_t));
    if (!button_data)
    {
        free(widget);
        return NULL;
    }

    if (label)
        strncpy(button_data->label, label, sizeof(button_data->label) - 1);
    else
        button_data->label[0] = '\0';
    button_data->is_pressed = false;

    data->data = button_data;

    return widget;
}

void widget_button_set_label(widget_t *widget, const char *label)
{
    if (!widget || !label)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_BUTTON)
        return;

    button_data_t *button_data = (button_data_t *)data->data;
    strncpy(button_data->label, label, sizeof(button_data->label) - 1);
}

const char *widget_button_get_label(widget_t *widget)
{
    if (!widget)
        return "";

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_BUTTON)
        return "";

    button_data_t *button_data = (button_data_t *)data->data;
    return button_data->label;
}

/* ============================================================================
 * LABEL WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_label_create(widget_container_t *parent, int x, int y, const char *text)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_LABEL;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = text ? strlen(text) : 0;
    data->bounds.height = 1;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    label_data_t *label_data = (label_data_t *)malloc(sizeof(label_data_t));
    if (!label_data)
    {
        free(widget);
        return NULL;
    }

    if (text)
        strncpy(label_data->text, text, sizeof(label_data->text) - 1);
    else
        label_data->text[0] = '\0';

    data->data = label_data;

    return widget;
}

void widget_label_set_text(widget_t *widget, const char *text)
{
    if (!widget || !text)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_LABEL)
        return;

    label_data_t *label_data = (label_data_t *)data->data;
    strncpy(label_data->text, text, sizeof(label_data->text) - 1);
    data->bounds.width = strlen(text);
}

const char *widget_label_get_text(widget_t *widget)
{
    if (!widget)
        return "";

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_LABEL)
        return "";

    label_data_t *label_data = (label_data_t *)data->data;
    return label_data->text;
}

/* ============================================================================
 * TEXT INPUT WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_text_input_create(widget_container_t *parent, int x, int y,
                                   int width, int height)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_TEXT_INPUT;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = width;
    data->bounds.height = height > 0 ? height : 1;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    text_input_data_t *input_data = (text_input_data_t *)malloc(sizeof(text_input_data_t));
    if (!input_data)
    {
        free(widget);
        return NULL;
    }

    input_data->text[0] = '\0';
    input_data->cursor_pos = 0;
    input_data->max_length = sizeof(input_data->text) - 1;

    data->data = input_data;

    return widget;
}

void widget_text_input_set_text(widget_t *widget, const char *text)
{
    if (!widget || !text)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_TEXT_INPUT)
        return;

    text_input_data_t *input_data = (text_input_data_t *)data->data;
    strncpy(input_data->text, text, input_data->max_length);
    input_data->cursor_pos = strlen(input_data->text);
}

const char *widget_text_input_get_text(widget_t *widget)
{
    if (!widget)
        return "";

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_TEXT_INPUT)
        return "";

    text_input_data_t *input_data = (text_input_data_t *)data->data;
    return input_data->text;
}

void widget_text_input_insert_char(widget_t *widget, char c)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_TEXT_INPUT)
        return;

    text_input_data_t *input_data = (text_input_data_t *)data->data;

    if (input_data->cursor_pos >= input_data->max_length)
        return;

    // Insert character at cursor position
    int len = strlen(input_data->text);
    for (int i = len; i > input_data->cursor_pos; i--)
    {
        input_data->text[i] = input_data->text[i - 1];
    }
    input_data->text[input_data->cursor_pos] = c;
    input_data->text[len + 1] = '\0';
    input_data->cursor_pos++;
}

void widget_text_input_delete_char(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_TEXT_INPUT)
        return;

    text_input_data_t *input_data = (text_input_data_t *)data->data;

    if (input_data->cursor_pos <= 0)
        return;

    int len = strlen(input_data->text);
    for (int i = input_data->cursor_pos - 1; i < len; i++)
    {
        input_data->text[i] = input_data->text[i + 1];
    }
    input_data->cursor_pos--;
}

void widget_text_input_set_cursor(widget_t *widget, int pos)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_TEXT_INPUT)
        return;

    text_input_data_t *input_data = (text_input_data_t *)data->data;
    int len = strlen(input_data->text);
    input_data->cursor_pos = pos < 0 ? 0 : (pos > len ? len : pos);
}

/* ============================================================================
 * CHECKBOX WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_checkbox_create(widget_container_t *parent, int x, int y, const char *label)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_CHECKBOX;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = label ? strlen(label) + 3 : 3;
    data->bounds.height = 1;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    checkbox_data_t *checkbox_data = (checkbox_data_t *)malloc(sizeof(checkbox_data_t));
    if (!checkbox_data)
    {
        free(widget);
        return NULL;
    }

    if (label)
        strncpy(checkbox_data->label, label, sizeof(checkbox_data->label) - 1);
    else
        checkbox_data->label[0] = '\0';
    checkbox_data->checked = false;

    data->data = checkbox_data;

    return widget;
}

void widget_checkbox_set_checked(widget_t *widget, bool checked)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_CHECKBOX)
        return;

    checkbox_data_t *checkbox_data = (checkbox_data_t *)data->data;
    checkbox_data->checked = checked;
}

bool widget_checkbox_is_checked(widget_t *widget)
{
    if (!widget)
        return false;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_CHECKBOX)
        return false;

    checkbox_data_t *checkbox_data = (checkbox_data_t *)data->data;
    return checkbox_data->checked;
}

/* ============================================================================
 * SLIDER WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_slider_create(widget_container_t *parent, int x, int y, int width,
                               int min_value, int max_value)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_SLIDER;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = width;
    data->bounds.height = 1;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    slider_data_t *slider_data = (slider_data_t *)malloc(sizeof(slider_data_t));
    if (!slider_data)
    {
        free(widget);
        return NULL;
    }

    slider_data->min_value = min_value;
    slider_data->max_value = max_value;
    slider_data->current_value = min_value;

    data->data = slider_data;

    return widget;
}

void widget_slider_set_value(widget_t *widget, int value)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_SLIDER)
        return;

    slider_data_t *slider_data = (slider_data_t *)data->data;
    if (value < slider_data->min_value)
        value = slider_data->min_value;
    if (value > slider_data->max_value)
        value = slider_data->max_value;
    slider_data->current_value = value;
}

int widget_slider_get_value(widget_t *widget)
{
    if (!widget)
        return 0;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_SLIDER)
        return 0;

    slider_data_t *slider_data = (slider_data_t *)data->data;
    return slider_data->current_value;
}

/* ============================================================================
 * PROGRESS BAR WIDGET IMPLEMENTATION
 * =========================================================================== */

widget_t *widget_progress_bar_create(widget_container_t *parent, int x, int y,
                                     int width, int height)
{
    widget_t *widget = (widget_t *)malloc(sizeof(widget_data_t));
    if (!widget)
        return NULL;

    widget_data_t *data = (widget_data_t *)widget;
    data->type = WIDGET_TYPE_PROGRESS_BAR;
    data->state = WIDGET_STATE_NORMAL;
    data->parent = parent;
    data->bounds.x = x;
    data->bounds.y = y;
    data->bounds.width = width;
    data->bounds.height = height > 0 ? height : 1;
    data->visible = true;
    data->enabled = true;
    data->callback = NULL;

    progress_bar_data_t *progress_data = (progress_bar_data_t *)malloc(sizeof(progress_bar_data_t));
    if (!progress_data)
    {
        free(widget);
        return NULL;
    }

    progress_data->min_value = 0;
    progress_data->max_value = 100;
    progress_data->current_value = 0;

    data->data = progress_data;

    return widget;
}

void widget_progress_bar_set_value(widget_t *widget, int value)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_PROGRESS_BAR)
        return;

    progress_bar_data_t *progress_data = (progress_bar_data_t *)data->data;
    if (value < progress_data->min_value)
        value = progress_data->min_value;
    if (value > progress_data->max_value)
        value = progress_data->max_value;
    progress_data->current_value = value;
}

int widget_progress_bar_get_value(widget_t *widget)
{
    if (!widget)
        return 0;

    widget_data_t *data = (widget_data_t *)widget;
    if (data->type != WIDGET_TYPE_PROGRESS_BAR)
        return 0;

    progress_bar_data_t *progress_data = (progress_bar_data_t *)data->data;
    return progress_data->current_value;
}

/* ============================================================================
 * CONTAINER WIDGET IMPLEMENTATION
 * =========================================================================== */

#define MAX_CHILDREN 256

widget_container_t *widget_container_create(widget_container_t *parent, int x, int y,
                                            int width, int height)
{
    widget_container_t *container = (widget_container_t *)malloc(sizeof(widget_container_t));
    if (!container)
        return NULL;

    container->base.type = WIDGET_TYPE_CONTAINER;
    container->base.state = WIDGET_STATE_NORMAL;
    container->base.parent = parent;
    container->base.bounds.x = x;
    container->base.bounds.y = y;
    container->base.bounds.width = width;
    container->base.bounds.height = height;
    container->base.visible = true;
    container->base.enabled = true;
    container->base.callback = NULL;
    container->base.data = NULL;

    container->children = (widget_t **)malloc(sizeof(widget_t *) * MAX_CHILDREN);
    if (!container->children)
    {
        free(container);
        return NULL;
    }

    container->child_count = 0;
    container->max_children = MAX_CHILDREN;

    return container;
}

void widget_container_destroy(widget_container_t *container)
{
    if (!container)
        return;

    for (int i = 0; i < container->child_count; i++)
    {
        if (container->children[i])
        {
            widget_data_t *data = (widget_data_t *)container->children[i];
            if (data->type == WIDGET_TYPE_CONTAINER)
            {
                widget_container_destroy((widget_container_t *)container->children[i]);
            }
            else
            {
                if (data->data)
                    free(data->data);
                free(container->children[i]);
            }
        }
    }

    free(container->children);
    free(container);
}

void widget_container_draw(widget_container_t *container, renderer_t *renderer)
{
    if (!container || !renderer)
        return;

    for (int i = 0; i < container->child_count; i++)
    {
        widget_t *child = container->children[i];
        if (child)
        {
            widget_draw(child, renderer);
        }
    }
}

widget_t *widget_container_get_child(widget_container_t *container, int index)
{
    if (!container || index < 0 || index >= container->child_count)
        return NULL;

    return container->children[index];
}

/* ============================================================================
 * GENERIC WIDGET FUNCTIONS
 * =========================================================================== */

void widget_destroy(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;

    if (data->type == WIDGET_TYPE_CONTAINER)
    {
        widget_container_destroy((widget_container_t *)widget);
        return;
    }

    if (data->data)
        free(data->data);

    free(widget);
}

void widget_set_position(widget_t *widget, int x, int y)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->bounds.x = x;
    data->bounds.y = y;
}

void widget_set_size(widget_t *widget, int width, int height)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->bounds.width = width;
    data->bounds.height = height;
}

void widget_set_state(widget_t *widget, widget_state_t state)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->state = state;
}

widget_state_t widget_get_state(widget_t *widget)
{
    if (!widget)
        return WIDGET_STATE_NORMAL;

    widget_data_t *data = (widget_data_t *)widget;
    return data->state;
}

void widget_enable(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->enabled = true;
}

void widget_disable(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->enabled = false;
}

void widget_show(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->visible = true;
}

void widget_hide(widget_t *widget)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->visible = false;
}

void widget_set_callback(widget_t *widget, widget_callback_t callback)
{
    if (!widget)
        return;

    widget_data_t *data = (widget_data_t *)widget;
    data->callback = callback;
}

widget_type_t widget_get_type(widget_t *widget)
{
    if (!widget)
        return WIDGET_TYPE_CONTAINER;

    widget_data_t *data = (widget_data_t *)widget;
    return data->type;
}

void widget_draw(widget_t *widget, renderer_t *renderer)
{
    if (!widget || !renderer)
        return;

    widget_data_t *data = (widget_data_t *)widget;

    if (!data->visible)
        return;

    switch (data->type)
    {
    case WIDGET_TYPE_BUTTON:
    {
        button_data_t *button = (button_data_t *)data->data;
        renderer_set_color(renderer, COLOR_BLACK, COLOR_WHITE);
        renderer_draw_rect_filled(renderer, data->bounds.x, data->bounds.y,
                                  data->bounds.width, data->bounds.height, ' ');
        renderer_draw_text(renderer, data->bounds.x + 1, data->bounds.y, button->label);
        break;
    }

    case WIDGET_TYPE_LABEL:
    {
        label_data_t *label = (label_data_t *)data->data;
        renderer_set_color(renderer, COLOR_WHITE, COLOR_BLACK);
        renderer_draw_text(renderer, data->bounds.x, data->bounds.y, label->text);
        break;
    }

    case WIDGET_TYPE_TEXT_INPUT:
    {
        text_input_data_t *input = (text_input_data_t *)data->data;
        renderer_set_color(renderer, COLOR_BLACK, COLOR_WHITE);
        renderer_draw_rect_filled(renderer, data->bounds.x, data->bounds.y,
                                  data->bounds.width, data->bounds.height, ' ');
        renderer_draw_text(renderer, data->bounds.x, data->bounds.y, input->text);
        break;
    }

    case WIDGET_TYPE_CHECKBOX:
    {
        checkbox_data_t *checkbox = (checkbox_data_t *)data->data;
        renderer_set_color(renderer, COLOR_WHITE, COLOR_BLACK);
        renderer_draw_text(renderer, data->bounds.x, data->bounds.y,
                           checkbox->checked ? "[X]" : "[ ]");
        renderer_draw_text(renderer, data->bounds.x + 4, data->bounds.y, checkbox->label);
        break;
    }

    case WIDGET_TYPE_SLIDER:
    {
        slider_data_t *slider = (slider_data_t *)data->data;
        int range = slider->max_value - slider->min_value;
        int pos = (slider->current_value - slider->min_value) * data->bounds.width / range;
        renderer_set_color(renderer, COLOR_WHITE, COLOR_BLACK);
        renderer_draw_rect_outline(renderer, data->bounds.x, data->bounds.y,
                                   data->bounds.width, 1);
        renderer_draw_pixel(renderer, data->bounds.x + pos, data->bounds.y, '●');
        break;
    }

    case WIDGET_TYPE_PROGRESS_BAR:
    {
        progress_bar_data_t *progress = (progress_bar_data_t *)data->data;
        int filled = (progress->current_value - progress->min_value) *
                     (data->bounds.width - 2) /
                     (progress->max_value - progress->min_value);
        renderer_set_color(renderer, COLOR_WHITE, COLOR_BLACK);
        renderer_draw_rect_outline(renderer, data->bounds.x, data->bounds.y,
                                   data->bounds.width, data->bounds.height);
        if (filled > 0)
        {
            renderer_set_color(renderer, COLOR_GREEN, COLOR_GREEN);
            renderer_draw_rect_filled(renderer, data->bounds.x + 1, data->bounds.y + 1,
                                      filled, data->bounds.height - 2, '█');
        }
        break;
    }

    case WIDGET_TYPE_CONTAINER:
    {
        widget_container_draw((widget_container_t *)widget, renderer);
        break;
    }

    default:
        break;
    }
}
