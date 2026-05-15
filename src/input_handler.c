/**
 * SimpleOS Input Handler Implementation
 * Keyboard and mouse input processing
 */

#include "input_handler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif

/* ============================================================================
 * INPUT HANDLER STRUCTURE
 * =========================================================================== */

struct input_handler
{
    bool enabled;
    bool initialized;
    input_callback_t callback;
    int last_mouse_x;
    int last_mouse_y;
    uint32_t key_state[256]; // Track key states
    uint32_t mouse_button_state;

#ifndef _WIN32
    struct termios original_termios;
#endif
};

/* ============================================================================
 * INPUT HANDLER LIFECYCLE
 * =========================================================================== */

input_handler_t *input_handler_create(void)
{
    input_handler_t *handler = (input_handler_t *)malloc(sizeof(input_handler_t));
    if (!handler)
        return NULL;

    memset(handler, 0, sizeof(input_handler_t));
    handler->enabled = false;
    handler->initialized = false;
    handler->callback = NULL;
    handler->last_mouse_x = 0;
    handler->last_mouse_y = 0;
    handler->mouse_button_state = 0;

    return handler;
}

void input_handler_destroy(input_handler_t *handler)
{
    if (!handler)
        return;

    if (handler->initialized)
    {
        input_handler_shutdown(handler);
    }

    free(handler);
}

int input_handler_init(input_handler_t *handler)
{
    if (!handler)
        return -1;

#ifndef _WIN32
    struct termios new_termios;

    // Get current terminal settings
    if (tcgetattr(STDIN_FILENO, &handler->original_termios) < 0)
    {
        return -1;
    }

    // Save for restoration
    new_termios = handler->original_termios;

    // Disable canonical mode and echo
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0; // Non-blocking read
    new_termios.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) < 0)
    {
        return -1;
    }
#endif

    handler->initialized = true;
    handler->enabled = true;

    return 0;
}

void input_handler_shutdown(input_handler_t *handler)
{
    if (!handler || !handler->initialized)
        return;

#ifndef _WIN32
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &handler->original_termios);
#endif

    handler->initialized = false;
    handler->enabled = false;
}

int input_handler_poll(input_handler_t *handler, input_event_t *event)
{
    if (!handler || !handler->enabled || !event)
        return -1;

    memset(event, 0, sizeof(input_event_t));

#ifdef _WIN32
    if (!_kbhit())
        return 0;

    int c = _getch();
    if (c == 0 || c == 224)
    {
        // Extended key code
        int c2 = _getch();
        event->type = INPUT_EVENT_KEYBOARD;
        event->data.keyboard.key = (key_code_t)(c2 + 256);
        event->data.keyboard.ascii = 0;
    }
    else
    {
        event->type = INPUT_EVENT_KEYBOARD;
        event->data.keyboard.key = (key_code_t)c;
        event->data.keyboard.ascii = (char)c;
    }
#else
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) <= 0)
    {
        return 0; // No input available
    }

    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) <= 0)
    {
        return 0;
    }

    event->type = INPUT_EVENT_KEYBOARD;

    // Handle escape sequences
    if (c == '\x1b')
    {
        unsigned char seq[3] = {0};
        if (read(STDIN_FILENO, &seq[0], 1) > 0)
        {
            if (seq[0] == '[')
            {
                if (read(STDIN_FILENO, &seq[1], 1) > 0)
                {
                    switch (seq[1])
                    {
                    case 'A':
                        event->data.keyboard.key = KEY_UP;
                        break;
                    case 'B':
                        event->data.keyboard.key = KEY_DOWN;
                        break;
                    case 'C':
                        event->data.keyboard.key = KEY_RIGHT;
                        break;
                    case 'D':
                        event->data.keyboard.key = KEY_LEFT;
                        break;
                    case 'H':
                        event->data.keyboard.key = KEY_HOME;
                        break;
                    case 'F':
                        event->data.keyboard.key = KEY_END;
                        break;
                    default:
                        event->data.keyboard.key = KEY_NONE;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        event->data.keyboard.key = (key_code_t)c;
        event->data.keyboard.ascii = (char)c;
    }
#endif

    // Update key state
    handler->key_state[event->data.keyboard.key] = 1;

    // Call callback if registered
    if (handler->callback)
    {
        handler->callback(event);
    }

    return 1;
}

int input_handler_wait(input_handler_t *handler, int timeout_ms, input_event_t *event)
{
    if (!handler || !handler->enabled || !event)
        return -1;

    // Simple implementation: poll multiple times
    for (int i = 0; i < (timeout_ms > 0 ? timeout_ms / 10 : 1000); i++)
    {
        int result = input_handler_poll(handler, event);
        if (result > 0)
            return 1;

#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }

    return 0; // Timeout
}

void input_handler_set_callback(input_handler_t *handler, input_callback_t callback)
{
    if (handler)
    {
        handler->callback = callback;
    }
}

void input_handler_enable(input_handler_t *handler)
{
    if (handler)
    {
        handler->enabled = true;
    }
}

void input_handler_disable(input_handler_t *handler)
{
    if (handler)
    {
        handler->enabled = false;
    }
}

bool input_handler_is_enabled(input_handler_t *handler)
{
    if (!handler)
        return false;

    return handler->enabled;
}

/* ============================================================================
 * KEYBOARD INPUT
 * =========================================================================== */

int input_handler_get_key(input_handler_t *handler, keyboard_event_t *event)
{
    if (!handler || !event)
        return -1;

    input_event_t input_event;
    int result = input_handler_poll(handler, &input_event);

    if (result > 0 && input_event.type == INPUT_EVENT_KEYBOARD)
    {
        *event = input_event.data.keyboard;
        return 1;
    }

    return result;
}

bool input_handler_is_key_pressed(input_handler_t *handler, key_code_t key)
{
    if (!handler)
        return false;

    return handler->key_state[key] != 0;
}

/* ============================================================================
 * MOUSE INPUT
 * =========================================================================== */

int input_handler_get_mouse(input_handler_t *handler, mouse_event_t *event)
{
    if (!handler || !event)
        return -1;

    // Simplified mouse support
    memset(event, 0, sizeof(mouse_event_t));
    return 0; // No mouse input available
}

void input_handler_get_mouse_position(input_handler_t *handler, int *x, int *y)
{
    if (handler)
    {
        if (x)
            *x = handler->last_mouse_x;
        if (y)
            *y = handler->last_mouse_y;
    }
}

bool input_handler_is_mouse_button_pressed(input_handler_t *handler, mouse_button_t button)
{
    if (!handler)
        return false;

    return (handler->mouse_button_state & button) != 0;
}

/* ============================================================================
 * MODIFIER KEYS
 * =========================================================================== */

bool input_handler_is_shift_pressed(input_handler_t *handler)
{
    if (!handler)
        return false;

    // On Unix-like systems, we'd check shift state
    // For now, simplified implementation
    return false;
}

bool input_handler_is_ctrl_pressed(input_handler_t *handler)
{
    if (!handler)
        return false;

    // Check for Ctrl key state
    return handler->key_state[KEY_CTRL_C] || handler->key_state[KEY_CTRL_D];
}

bool input_handler_is_alt_pressed(input_handler_t *handler)
{
    if (!handler)
        return false;

    // Alt key tracking would go here
    return false;
}
