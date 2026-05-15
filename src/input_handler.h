/**
 * SimpleOS Input Handler Header
 * Keyboard and mouse input processing system
 */

#ifndef SIMPLEOS_INPUT_HANDLER_H
#define SIMPLEOS_INPUT_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * INPUT TYPES
     * =========================================================================== */

    /**
     * Input handler handle
     */
    typedef struct input_handler input_handler_t;

    /**
     * Key codes (subset)
     */
    typedef enum
    {
        KEY_NONE = 0,
        KEY_ESCAPE = 27,
        KEY_RETURN = 13,
        KEY_TAB = 9,
        KEY_BACKSPACE = 8,
        KEY_DELETE = 127,
        KEY_UP = 256,
        KEY_DOWN = 257,
        KEY_LEFT = 258,
        KEY_RIGHT = 259,
        KEY_HOME = 260,
        KEY_END = 261,
        KEY_PAGE_UP = 262,
        KEY_PAGE_DOWN = 263,
        KEY_F1 = 280,
        KEY_F2 = 281,
        KEY_F3 = 282,
        KEY_F4 = 283,
        KEY_F5 = 284,
        KEY_F10 = 289,
        KEY_CTRL_C = 3,
        KEY_CTRL_D = 4,
        KEY_CTRL_Z = 26
    } key_code_t;

    /**
     * Mouse buttons
     */
    typedef enum
    {
        MOUSE_BUTTON_NONE = 0,
        MOUSE_BUTTON_LEFT = 1,
        MOUSE_BUTTON_RIGHT = 2,
        MOUSE_BUTTON_MIDDLE = 4
    } mouse_button_t;

    /**
     * Mouse event types
     */
    typedef enum
    {
        MOUSE_EVENT_NONE = 0,
        MOUSE_EVENT_MOVE = 1,
        MOUSE_EVENT_CLICK = 2,
        MOUSE_EVENT_DOUBLE_CLICK = 3,
        MOUSE_EVENT_DRAG = 4,
        MOUSE_EVENT_WHEEL_UP = 5,
        MOUSE_EVENT_WHEEL_DOWN = 6
    } mouse_event_type_t;

    /**
     * Keyboard event
     */
    typedef struct
    {
        key_code_t key;
        char ascii;
        bool shift;
        bool ctrl;
        bool alt;
        bool repeat;
    } keyboard_event_t;

    /**
     * Mouse event
     */
    typedef struct
    {
        mouse_event_type_t type;
        int x;
        int y;
        mouse_button_t button;
        int click_count;
        bool shift;
        bool ctrl;
    } mouse_event_t;

    /**
     * Input event
     */
    typedef enum
    {
        INPUT_EVENT_NONE = 0,
        INPUT_EVENT_KEYBOARD = 1,
        INPUT_EVENT_MOUSE = 2
    } input_event_type_t;

    typedef struct
    {
        input_event_type_t type;
        union
        {
            keyboard_event_t keyboard;
            mouse_event_t mouse;
        } data;
    } input_event_t;

    /**
     * Input callback function
     */
    typedef void (*input_callback_t)(const input_event_t *event);

    /* ============================================================================
     * INPUT HANDLER FUNCTIONS
     * =========================================================================== */

    /**
     * Create input handler
     * @return Input handler handle or NULL on error
     */
    input_handler_t *input_handler_create(void);

    /**
     * Destroy input handler
     * @param handler Input handler handle
     */
    void input_handler_destroy(input_handler_t *handler);

    /**
     * Initialize input handler
     * @param handler Input handler handle
     * @return 0 on success, -1 on error
     */
    int input_handler_init(input_handler_t *handler);

    /**
     * Shutdown input handler
     * @param handler Input handler handle
     */
    void input_handler_shutdown(input_handler_t *handler);

    /**
     * Poll for input events
     * @param handler Input handler handle
     * @param event Pointer to receive event
     * @return 1 if event available, 0 if no event, -1 on error
     */
    int input_handler_poll(input_handler_t *handler, input_event_t *event);

    /**
     * Wait for input event (blocking)
     * @param handler Input handler handle
     * @param timeout_ms Timeout in milliseconds (0 = infinite)
     * @param event Pointer to receive event
     * @return 1 if event received, 0 if timeout, -1 on error
     */
    int input_handler_wait(input_handler_t *handler, int timeout_ms, input_event_t *event);

    /**
     * Set input callback
     * @param handler Input handler handle
     * @param callback Callback function
     */
    void input_handler_set_callback(input_handler_t *handler, input_callback_t callback);

    /**
     * Enable input capture
     * @param handler Input handler handle
     */
    void input_handler_enable(input_handler_t *handler);

    /**
     * Disable input capture
     * @param handler Input handler handle
     */
    void input_handler_disable(input_handler_t *handler);

    /**
     * Check if input is enabled
     * @param handler Input handler handle
     * @return true if enabled, false otherwise
     */
    bool input_handler_is_enabled(input_handler_t *handler);

    /* ============================================================================
     * KEYBOARD INPUT
     * =========================================================================== */

    /**
     * Get keyboard input
     * @param handler Input handler handle
     * @param event Keyboard event to fill
     * @return 1 if key pressed, 0 if no input, -1 on error
     */
    int input_handler_get_key(input_handler_t *handler, keyboard_event_t *event);

    /**
     * Check if key is pressed
     * @param handler Input handler handle
     * @param key Key code
     * @return true if pressed, false otherwise
     */
    bool input_handler_is_key_pressed(input_handler_t *handler, key_code_t key);

    /* ============================================================================
     * MOUSE INPUT
     * =========================================================================== */

    /**
     * Get mouse input
     * @param handler Input handler handle
     * @param event Mouse event to fill
     * @return 1 if mouse event, 0 if no input, -1 on error
     */
    int input_handler_get_mouse(input_handler_t *handler, mouse_event_t *event);

    /**
     * Get mouse position
     * @param handler Input handler handle
     * @param x Pointer to receive X coordinate
     * @param y Pointer to receive Y coordinate
     */
    void input_handler_get_mouse_position(input_handler_t *handler, int *x, int *y);

    /**
     * Check if mouse button is pressed
     * @param handler Input handler handle
     * @param button Mouse button
     * @return true if pressed, false otherwise
     */
    bool input_handler_is_mouse_button_pressed(input_handler_t *handler, mouse_button_t button);

    /* ============================================================================
     * MODIFIER KEYS
     * =========================================================================== */

    /**
     * Check if shift is pressed
     * @param handler Input handler handle
     * @return true if pressed, false otherwise
     */
    bool input_handler_is_shift_pressed(input_handler_t *handler);

    /**
     * Check if control is pressed
     * @param handler Input handler handle
     * @return true if pressed, false otherwise
     */
    bool input_handler_is_ctrl_pressed(input_handler_t *handler);

    /**
     * Check if alt is pressed
     * @param handler Input handler handle
     * @return true if pressed, false otherwise
     */
    bool input_handler_is_alt_pressed(input_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_INPUT_HANDLER_H */
