/**
 * SimpleOS Event Dispatcher Implementation
 * Centralized event handling and dispatching
 */

#include "event_dispatcher.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define get_time_ms() GetTickCount()
#else
#include <unistd.h>
#include <sys/time.h>
static uint64_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

/* ============================================================================
 * EVENT QUEUE SIZE
 * =========================================================================== */

#define DEFAULT_QUEUE_SIZE 1024

/* ============================================================================
 * EVENT LISTENER STRUCTURE
 * =========================================================================== */

struct event_listener
{
    uint32_t event_type;
    event_callback_t callback;
    void *context;
    event_priority_t priority;
    bool enabled;
    struct event_listener *next;
};

/* ============================================================================
 * EVENT DISPATCHER STRUCTURE
 * =========================================================================== */

struct event_dispatcher
{
    event_queue_entry_t *queue;
    int queue_size;
    int queue_head;
    int queue_tail;
    int queue_count;

    event_listener_t *listeners;
    int listener_count;

    window_t *focused_window;
    uint64_t total_events_processed;
    bool running;
};

/* ============================================================================
 * EVENT DISPATCHER LIFECYCLE
 * =========================================================================== */

event_dispatcher_t *event_dispatcher_create(void)
{
    event_dispatcher_t *dispatcher = (event_dispatcher_t *)malloc(sizeof(event_dispatcher_t));
    if (!dispatcher)
        return NULL;

    memset(dispatcher, 0, sizeof(event_dispatcher_t));
    dispatcher->queue_size = DEFAULT_QUEUE_SIZE;
    dispatcher->queue_head = 0;
    dispatcher->queue_tail = 0;
    dispatcher->queue_count = 0;
    dispatcher->listeners = NULL;
    dispatcher->listener_count = 0;
    dispatcher->focused_window = NULL;
    dispatcher->total_events_processed = 0;
    dispatcher->running = false;

    dispatcher->queue = (event_queue_entry_t *)malloc(sizeof(event_queue_entry_t) * dispatcher->queue_size);
    if (!dispatcher->queue)
    {
        free(dispatcher);
        return NULL;
    }

    memset(dispatcher->queue, 0, sizeof(event_queue_entry_t) * dispatcher->queue_size);

    return dispatcher;
}

void event_dispatcher_destroy(event_dispatcher_t *dispatcher)
{
    if (!dispatcher)
        return;

    // Destroy queue
    free(dispatcher->queue);

    // Destroy listeners
    event_listener_t *listener = dispatcher->listeners;
    while (listener)
    {
        event_listener_t *next = listener->next;
        free(listener);
        listener = next;
    }

    free(dispatcher);
}

int event_dispatcher_init(event_dispatcher_t *dispatcher, int queue_size)
{
    if (!dispatcher)
        return -1;

    if (queue_size > 0 && queue_size != dispatcher->queue_size)
    {
        event_queue_entry_t *new_queue = (event_queue_entry_t *)malloc(sizeof(event_queue_entry_t) * queue_size);
        if (!new_queue)
            return -1;

        free(dispatcher->queue);
        dispatcher->queue = new_queue;
        dispatcher->queue_size = queue_size;
        dispatcher->queue_count = 0;
        dispatcher->queue_head = 0;
        dispatcher->queue_tail = 0;
    }

    dispatcher->running = true;
    return 0;
}

void event_dispatcher_shutdown(event_dispatcher_t *dispatcher)
{
    if (dispatcher)
    {
        dispatcher->running = false;
    }
}

int event_dispatcher_process(event_dispatcher_t *dispatcher)
{
    if (!dispatcher || dispatcher->queue_count == 0)
        return 0;

    int processed = 0;

    while (dispatcher->queue_count > 0)
    {
        // Get next event from queue
        event_queue_entry_t *event = &dispatcher->queue[dispatcher->queue_head];
        dispatcher->queue_head = (dispatcher->queue_head + 1) % dispatcher->queue_size;
        dispatcher->queue_count--;

        // Dispatch to listeners
        event_listener_t *listener = dispatcher->listeners;
        while (listener)
        {
            if (listener->enabled && (listener->event_type == 0 || listener->event_type == event->type))
            {
                listener->callback(event->type, event->data, listener->context);
            }
            listener = listener->next;
        }

        // Clean up event data
        if (event->data && event->data_destructor)
        {
            event->data_destructor(event->data);
        }

        processed++;
        dispatcher->total_events_processed++;
    }

    return processed;
}

/* ============================================================================
 * EVENT POSTING
 * =========================================================================== */

int event_dispatcher_post(event_dispatcher_t *dispatcher, uint32_t type,
                          event_priority_t priority, void *data, int data_size)
{
    if (!dispatcher || dispatcher->queue_count >= dispatcher->queue_size)
        return -1;

    // Find insertion point based on priority
    int insert_pos = dispatcher->queue_tail;

    if (priority > EVENT_PRIORITY_NORMAL)
    {
        // High priority events go to front of queue
        insert_pos = dispatcher->queue_head;
    }

    event_queue_entry_t *event = &dispatcher->queue[insert_pos];
    event->type = type;
    event->priority = priority;
    event->timestamp = get_time_ms();

    if (data && data_size > 0)
    {
        event->data = malloc(data_size);
        if (!event->data)
            return -1;
        memcpy(event->data, data, data_size);
    }
    else
    {
        event->data = NULL;
    }

    event->data_destructor = free;
    dispatcher->queue_tail = (dispatcher->queue_tail + 1) % dispatcher->queue_size;
    dispatcher->queue_count++;

    return 0;
}

int event_dispatcher_post_immediate(event_dispatcher_t *dispatcher, uint32_t type, void *data)
{
    if (!dispatcher)
        return -1;

    // Dispatch immediately to all listeners
    event_listener_t *listener = dispatcher->listeners;
    while (listener)
    {
        if (listener->enabled && (listener->event_type == 0 || listener->event_type == type))
        {
            listener->callback(type, data, listener->context);
        }
        listener = listener->next;
    }

    dispatcher->total_events_processed++;
    return 0;
}

int event_dispatcher_post_window(event_dispatcher_t *dispatcher, window_t *window,
                                 const window_event_t *event)
{
    if (!dispatcher || !event)
        return -1;

    return event_dispatcher_post(dispatcher, 0x10000 + event->type, EVENT_PRIORITY_NORMAL,
                                 (void *)event, sizeof(window_event_t));
}

int event_dispatcher_post_input(event_dispatcher_t *dispatcher, const input_event_t *input)
{
    if (!dispatcher || !input)
        return -1;

    return event_dispatcher_post(dispatcher, 0x20000 + input->type, EVENT_PRIORITY_HIGH,
                                 (void *)input, sizeof(input_event_t));
}

/* ============================================================================
 * EVENT LISTENERS
 * =========================================================================== */

event_listener_t *event_dispatcher_listen(event_dispatcher_t *dispatcher, uint32_t type,
                                          event_callback_t callback, void *context)
{
    if (!dispatcher || !callback)
        return NULL;

    event_listener_t *listener = (event_listener_t *)malloc(sizeof(event_listener_t));
    if (!listener)
        return NULL;

    listener->event_type = type;
    listener->callback = callback;
    listener->context = context;
    listener->priority = EVENT_PRIORITY_NORMAL;
    listener->enabled = true;
    listener->next = dispatcher->listeners;

    dispatcher->listeners = listener;
    dispatcher->listener_count++;

    return listener;
}

int event_dispatcher_unlisten(event_dispatcher_t *dispatcher, event_listener_t *listener)
{
    if (!dispatcher || !listener)
        return -1;

    event_listener_t *current = dispatcher->listeners;
    event_listener_t *prev = NULL;

    while (current)
    {
        if (current == listener)
        {
            if (prev)
                prev->next = current->next;
            else
                dispatcher->listeners = current->next;

            free(current);
            dispatcher->listener_count--;
            return 0;
        }
        prev = current;
        current = current->next;
    }

    return -1;
}

void event_listener_enable(event_listener_t *listener)
{
    if (listener)
        listener->enabled = true;
}

void event_listener_disable(event_listener_t *listener)
{
    if (listener)
        listener->enabled = false;
}

bool event_listener_is_enabled(event_listener_t *listener)
{
    if (!listener)
        return false;
    return listener->enabled;
}

void event_listener_set_priority(event_listener_t *listener, event_priority_t priority)
{
    if (listener)
        listener->priority = priority;
}

/* ============================================================================
 * EVENT FILTERING & ROUTING
 * =========================================================================== */

int event_dispatcher_set_window_filter(event_dispatcher_t *dispatcher, window_t *window, uint32_t type)
{
    if (!dispatcher || !window)
        return -1;

    // In a full implementation, maintain per-window filters
    (void)type;
    return 0;
}

bool event_dispatcher_route_to_window(event_dispatcher_t *dispatcher, window_t *window,
                                      const window_event_t *event)
{
    if (!dispatcher || !window || !event)
        return false;

    // Route event to window's callback
    window_event_t *win_event = (window_event_t *)malloc(sizeof(window_event_t));
    if (!win_event)
        return false;

    memcpy(win_event, event, sizeof(window_event_t));

    return event_dispatcher_post_immediate(dispatcher, 0x10000 + event->type, win_event) == 0;
}

window_t *event_dispatcher_get_focused_window(event_dispatcher_t *dispatcher)
{
    if (!dispatcher)
        return NULL;

    return dispatcher->focused_window;
}

/* ============================================================================
 * EVENT STATISTICS
 * =========================================================================== */

int event_dispatcher_get_queue_depth(event_dispatcher_t *dispatcher)
{
    if (!dispatcher)
        return 0;

    return dispatcher->queue_count;
}

uint64_t event_dispatcher_get_total_events(event_dispatcher_t *dispatcher)
{
    if (!dispatcher)
        return 0;

    return dispatcher->total_events_processed;
}

void event_dispatcher_clear_queue(event_dispatcher_t *dispatcher)
{
    if (!dispatcher)
        return;

    for (int i = 0; i < dispatcher->queue_count; i++)
    {
        int index = (dispatcher->queue_head + i) % dispatcher->queue_size;
        event_queue_entry_t *event = &dispatcher->queue[index];
        if (event->data && event->data_destructor)
        {
            event->data_destructor(event->data);
        }
    }

    dispatcher->queue_count = 0;
    dispatcher->queue_head = 0;
    dispatcher->queue_tail = 0;
}

/* ============================================================================
 * EVENT LOOP
 * =========================================================================== */

int event_dispatcher_run_loop(event_dispatcher_t *dispatcher, event_loop_predicate_t predicate)
{
    if (!dispatcher || !predicate)
        return -1;

    dispatcher->running = true;

    while (dispatcher->running && predicate(dispatcher))
    {
        event_dispatcher_process(dispatcher);

#ifdef _WIN32
        Sleep(10); // Sleep 10ms to avoid busy-waiting
#else
        usleep(10000); // Sleep 10ms
#endif
    }

    dispatcher->running = false;
    return 0;
}
