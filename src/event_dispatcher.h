/**
 * SimpleOS Event Dispatcher Header
 * Centralized event handling and dispatching
 */

#ifndef SIMPLEOS_EVENT_DISPATCHER_H
#define SIMPLEOS_EVENT_DISPATCHER_H

#include <stdint.h>
#include <stdbool.h>
#include "window_manager.h"
#include "input_handler.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * EVENT DISPATCHER TYPES
     * =========================================================================== */

    /**
     * Event dispatcher handle
     */
    typedef struct event_dispatcher event_dispatcher_t;

    /**
     * Event priority levels
     */
    typedef enum
    {
        EVENT_PRIORITY_LOW = 0,
        EVENT_PRIORITY_NORMAL = 1,
        EVENT_PRIORITY_HIGH = 2,
        EVENT_PRIORITY_CRITICAL = 3
    } event_priority_t;

    /**
     * Event listener handle
     */
    typedef struct event_listener event_listener_t;

    /**
     * Generic event callback
     * @param event_type Type of event
     * @param data Event data
     * @param context Listener context
     */
    typedef void (*event_callback_t)(uint32_t event_type, void *data, void *context);

    /**
     * Event queue entry
     */
    typedef struct
    {
        uint32_t type;
        event_priority_t priority;
        void *data;
        void (*data_destructor)(void *);
        uint64_t timestamp;
    } event_queue_entry_t;

    /* ============================================================================
     * EVENT DISPATCHER LIFECYCLE
     * =========================================================================== */

    /**
     * Create event dispatcher
     * @return Event dispatcher handle or NULL on error
     */
    event_dispatcher_t *event_dispatcher_create(void);

    /**
     * Destroy event dispatcher
     * @param dispatcher Event dispatcher handle
     */
    void event_dispatcher_destroy(event_dispatcher_t *dispatcher);

    /**
     * Initialize event dispatcher
     * @param dispatcher Event dispatcher handle
     * @param queue_size Event queue size
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_init(event_dispatcher_t *dispatcher, int queue_size);

    /**
     * Shutdown event dispatcher
     * @param dispatcher Event dispatcher handle
     */
    void event_dispatcher_shutdown(event_dispatcher_t *dispatcher);

    /**
     * Process all pending events
     * @param dispatcher Event dispatcher handle
     * @return Number of events processed
     */
    int event_dispatcher_process(event_dispatcher_t *dispatcher);

    /* ============================================================================
     * EVENT POSTING
     * =========================================================================== */

    /**
     * Post event to queue
     * @param dispatcher Event dispatcher handle
     * @param type Event type
     * @param priority Event priority
     * @param data Event data (copied)
     * @param data_size Event data size
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_post(event_dispatcher_t *dispatcher, uint32_t type,
                              event_priority_t priority, void *data, int data_size);

    /**
     * Post event immediately (bypass queue)
     * @param dispatcher Event dispatcher handle
     * @param type Event type
     * @param data Event data
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_post_immediate(event_dispatcher_t *dispatcher, uint32_t type, void *data);

    /**
     * Post window event
     * @param dispatcher Event dispatcher handle
     * @param window Window handle
     * @param event Window event
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_post_window(event_dispatcher_t *dispatcher, window_t *window,
                                     const window_event_t *event);

    /**
     * Post input event
     * @param dispatcher Event dispatcher handle
     * @param input Input event
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_post_input(event_dispatcher_t *dispatcher, const input_event_t *input);

    /* ============================================================================
     * EVENT LISTENERS
     * =========================================================================== */

    /**
     * Register event listener
     * @param dispatcher Event dispatcher handle
     * @param type Event type (0 for all types)
     * @param callback Callback function
     * @param context User context
     * @return Event listener handle or NULL on error
     */
    event_listener_t *event_dispatcher_listen(event_dispatcher_t *dispatcher, uint32_t type,
                                              event_callback_t callback, void *context);

    /**
     * Unregister event listener
     * @param dispatcher Event dispatcher handle
     * @param listener Event listener handle
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_unlisten(event_dispatcher_t *dispatcher, event_listener_t *listener);

    /**
     * Enable event listener
     * @param listener Event listener handle
     */
    void event_listener_enable(event_listener_t *listener);

    /**
     * Disable event listener
     * @param listener Event listener handle
     */
    void event_listener_disable(event_listener_t *listener);

    /**
     * Check if event listener is enabled
     * @param listener Event listener handle
     * @return true if enabled, false otherwise
     */
    bool event_listener_is_enabled(event_listener_t *listener);

    /**
     * Set event listener priority
     * @param listener Event listener handle
     * @param priority Listener priority
     */
    void event_listener_set_priority(event_listener_t *listener, event_priority_t priority);

    /* ============================================================================
     * EVENT FILTERING & ROUTING
     * =========================================================================== */

    /**
     * Set event filter for window
     * @param dispatcher Event dispatcher handle
     * @param window Window handle
     * @param type Event type to filter (0 for all)
     * @return 0 on success, -1 on error
     */
    int event_dispatcher_set_window_filter(event_dispatcher_t *dispatcher, window_t *window, uint32_t type);

    /**
     * Route event to window
     * @param dispatcher Event dispatcher handle
     * @param window Window handle
     * @param event Event
     * @return true if event was handled
     */
    bool event_dispatcher_route_to_window(event_dispatcher_t *dispatcher, window_t *window,
                                          const window_event_t *event);

    /**
     * Get focused window
     * @param dispatcher Event dispatcher handle
     * @return Window handle or NULL
     */
    window_t *event_dispatcher_get_focused_window(event_dispatcher_t *dispatcher);

    /* ============================================================================
     * EVENT STATISTICS
     * =========================================================================== */

    /**
     * Get event queue depth
     * @param dispatcher Event dispatcher handle
     * @return Queue depth
     */
    int event_dispatcher_get_queue_depth(event_dispatcher_t *dispatcher);

    /**
     * Get total events processed
     * @param dispatcher Event dispatcher handle
     * @return Total events
     */
    uint64_t event_dispatcher_get_total_events(event_dispatcher_t *dispatcher);

    /**
     * Clear event queue
     * @param dispatcher Event dispatcher handle
     */
    void event_dispatcher_clear_queue(event_dispatcher_t *dispatcher);

    /* ============================================================================
     * EVENT LOOP
     * =========================================================================== */

    /**
     * Run event loop
     * @param dispatcher Event dispatcher handle
     * @param window_manager Window manager handle
     * @param input_handler Input handler handle
     * @param timeout_ms Timeout between iterations (0 for non-blocking)
     * @return true to continue, false to exit
     */
    typedef bool (*event_loop_predicate_t)(event_dispatcher_t *dispatcher);

    /**
     * Start event loop
     * @param dispatcher Event dispatcher handle
     * @param predicate Loop continuation predicate
     * @return Exit code
     */
    int event_dispatcher_run_loop(event_dispatcher_t *dispatcher, event_loop_predicate_t predicate);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_EVENT_DISPATCHER_H */
