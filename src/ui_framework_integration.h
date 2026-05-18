/**
 * SimpleOS UI Framework - Integration Guide
 * How all UI components work together
 */

#ifndef SIMPLEOS_UI_FRAMEWORK_INTEGRATION_H
#define SIMPLEOS_UI_FRAMEWORK_INTEGRATION_H

/*
 * ==============================================================================
 * SIMPLEOS UI FRAMEWORK ARCHITECTURE
 * ==============================================================================
 *
 * The SimpleOS UI framework provides a complete, integrated system for building
 * terminal-based graphical user interfaces. All components work together to provide
 * window management, event handling, widgets, layout, input processing, and rendering.
 *
 * ==============================================================================
 * COMPONENT OVERVIEW
 * ==============================================================================
 *
 * 1. WINDOW MANAGER (window_manager.h/c)
 *    - Core multi-window management system
 *    - Creates, destroys, positions windows
 *    - Manages window state (visible, hidden, focused, minimized)
 *    - Maintains z-order for window stacking
 *    - Provides window event callback system
 *
 * 2. INPUT HANDLER (input_handler.h/c)
 *    - Cross-platform keyboard and mouse input
 *    - Polls or waits for input events
 *    - Tracks key and mouse button states
 *    - Supports keyboard modifiers (Shift, Ctrl, Alt)
 *    - Terminal mode management (canonical/non-canonical)
 *
 * 3. RENDERER (renderer.h/c)
 *    - ANSI-based text rendering engine
 *    - Draws primitives (pixels, lines, rectangles, boxes)
 *    - Text rendering with colors and attributes (bold, underline)
 *    - Double-buffering for flicker-free updates
 *    - Buffer save/load for screen capture
 *
 * 4. WIDGET SYSTEM (widget.h/c)
 *    - Interactive UI components
 *    - Built-in widgets: Button, Label, TextInput, Checkbox, Slider, ProgressBar
 *    - Widget containers for hierarchical layouts
 *    - State management (normal, focused, disabled, pressed, hovered)
 *    - Widget-specific callbacks for events
 *
 * 5. LAYOUT MANAGER (layout.h/c)
 *    - Automatic widget positioning and sizing
 *    - Three layout types: Box (vertical/horizontal), Grid, Dock
 *    - Size constraints (fixed, fill, preferred, wrap)
 *    - Margin and padding support
 *    - Responsive layout recalculation
 *
 * 6. EVENT DISPATCHER (event_dispatcher.h/c)
 *    - Centralized event handling system
 *    - Event queue with priority levels
 *    - Event listener registration and callback dispatch
 *    - Event filtering and routing
 *    - Statistics and queue management
 *
 * ==============================================================================
 * INTEGRATION PATTERN
 * ==============================================================================
 *
 * Typical application flow:
 *
 * 1. Initialize Components:
 *    - Create event dispatcher
 *    - Create window manager
 *    - Create input handler
 *    - Create renderer
 *    - Initialize all components
 *
 * 2. Create UI:
 *    - Create main window
 *    - Create widget container (panel)
 *    - Create widgets (buttons, labels, etc.)
 *    - Set up layout manager
 *    - Register widget callbacks
 *
 * 3. Setup Event Handlers:
 *    - Register listeners for input events
 *    - Register listeners for window events
 *    - Register listeners for widget events
 *    - Set up input callback
 *
 * 4. Event Loop:
 *    - Poll/wait for input events
 *    - Post events to dispatcher
 *    - Process pending events
 *    - Update window/widget state
 *    - Render to screen
 *    - Repeat
 *
 * ==============================================================================
 * COMPONENT INTERACTION
 * ==============================================================================
 *
 * INPUT -> EVENT DISPATCHER -> LISTENERS (WINDOW/WIDGET)
 *
 * 1. Input Handler gets keyboard/mouse input
 * 2. Input posted to Event Dispatcher
 * 3. Dispatcher routes to focused window
 * 4. Window forwards to widgets
 * 5. Widgets update state and post change events
 *
 * RENDERING PIPELINE
 *
 * 1. Renderer creates back buffer
 * 2. Widgets draw themselves to renderer
 * 3. Windows render widget children
 * 4. Renderer outputs to screen (ANSI codes)
 *
 * LAYOUT MANAGEMENT
 *
 * 1. Widgets added to container
 * 2. Layout manager calculates sizes/positions
 * 3. Layout manager applies to widgets
 * 4. Widgets reposition and redraw
 *
 * ==============================================================================
 * EXAMPLE: SIMPLE DIALOG
 * ==============================================================================
 *
 * // Create components
 * event_dispatcher_t *dispatcher = event_dispatcher_create();
 * window_manager_t *wm = window_manager_create(80, 24);
 * input_handler_t *input = input_handler_create();
 * renderer_t *renderer = renderer_create(80, 24);
 *
 * // Initialize
 * event_dispatcher_init(dispatcher, 256);
 * window_manager_init(wm);
 * input_handler_init(input);
 * renderer_init(renderer);
 *
 * // Create window
 * window_t *dialog = window_create(wm, "Settings", 10, 5, 60, 14, WINDOW_FLAGS_NONE);
 *
 * // Create widgets
 * widget_container_t *panel = widget_container_create(NULL, 11, 6, 58, 12);
 * widget_t *label = widget_label_create(panel, 2, 1, "Name:");
 * widget_t *input_field = widget_text_input_create(panel, 10, 1, 40, 1);
 * widget_t *ok_btn = widget_button_create(panel, 20, 10, 12, 1, "OK");
 * widget_t *cancel_btn = widget_button_create(panel, 35, 10, 12, 1, "Cancel");
 *
 * // Setup layout
 * layout_manager_t *layout = layout_box_create(panel, LAYOUT_ORIENTATION_VERTICAL);
 * layout_manager_set_spacing(layout, 1);
 * layout_manager_apply(layout);
 *
 * // Register callbacks
 * void on_ok_click(const widget_event_t *evt) {
 *     printf("OK clicked!\n");
 * }
 * widget_set_callback(ok_btn, on_ok_click);
 *
 * // Event loop
 * bool running = true;
 * while (running) {
 *     input_event_t event;
 *     if (input_handler_poll(input, &event) > 0) {
 *         event_dispatcher_post_input(dispatcher, &event);
 *     }
 *
 *     event_dispatcher_process(dispatcher);
 *
 *     renderer_clear(renderer, {0, 0, 0});
 *     window_manager_render(wm);
 *     widget_container_draw(panel, renderer);
 *     renderer_present(renderer);
 *
 *     if (event.type == INPUT_EVENT_KEYBOARD &&
 *         event.data.keyboard.key == KEY_ESCAPE) {
 *         running = false;
 *     }
 * }
 *
 * // Cleanup
 * event_dispatcher_destroy(dispatcher);
 * window_manager_destroy(wm);
 * input_handler_destroy(input);
 * renderer_destroy(renderer);
 *
 * ==============================================================================
 * TYPICAL WIDGET CALLBACKS
 * ==============================================================================
 *
 * void on_button_click(const widget_event_t *event) {
 *     if (event->type == WIDGET_EVENT_CLICK) {
 *         const char *label = widget_button_get_label(event->widget);
 *         printf("Button '%s' clicked\n", label);
 *     }
 * }
 *
 * void on_text_changed(const widget_event_t *event) {
 *     if (event->type == WIDGET_EVENT_CHANGE) {
 *         const char *text = widget_text_input_get_text(event->widget);
 *         printf("Text changed to: %s\n", text);
 *     }
 * }
 *
 * void on_checkbox_toggle(const widget_event_t *event) {
 *     if (event->type == WIDGET_EVENT_CHANGE) {
 *         bool checked = widget_checkbox_is_checked(event->widget);
 *         printf("Checkbox is now %s\n", checked ? "checked" : "unchecked");
 *     }
 * }
 *
 * ==============================================================================
 * ERROR HANDLING PATTERNS
 * ==============================================================================
 *
 * // Check return codes
 * if (event_dispatcher_post(dispatcher, EVENT_TYPE, priority, data, size) != 0) {
 *     fprintf(stderr, "Failed to post event\n");
 * }
 *
 * // Handle allocation failures
 * widget_t *btn = widget_button_create(container, x, y, w, h, "Click");
 * if (!btn) {
 *     fprintf(stderr, "Failed to create button widget\n");
 *     return -1;
 * }
 *
 * // Verify window state
 * window_state_t state = window_get_state(window);
 * if (state == WINDOW_STATE_HIDDEN) {
 *     window_show(window);
 * }
 *
 * ==============================================================================
 * PLATFORM CONSIDERATIONS
 * ==============================================================================
 *
 * WINDOWS:
 * - Uses Windows Console API for terminal control
 * - Input via _getch() and _kbhit()
 * - ANSI escape codes may require compatibility mode
 *
 * UNIX/LINUX/macOS:
 * - Uses termios for terminal configuration
 * - Input via select() and read()
 * - Native ANSI escape code support
 *
 * ==============================================================================
 * PERFORMANCE TIPS
 * ==============================================================================
 *
 * 1. Use renderer double-buffering for flicker-free updates
 * 2. Only redraw dirty regions (call widget_invalidate())
 * 3. Batch event processing before rendering
 * 4. Use high-priority events for critical input
 * 5. Disable listeners that aren't needed
 * 6. Pre-allocate event queue large enough
 * 7. Use layout manager to avoid manual positioning
 * 8. Cache widget lookups instead of searching repeatedly
 *
 * ==============================================================================
 * HEADER FILES TO INCLUDE
 * ==============================================================================
 *
 * For a complete UI application, include:
 * #include "window_manager.h"
 * #include "input_handler.h"
 * #include "renderer.h"
 * #include "widget.h"
 * #include "layout.h"
 * #include "event_dispatcher.h"
 *
 * ==============================================================================
 */

#endif /* SIMPLEOS_UI_FRAMEWORK_INTEGRATION_H */
