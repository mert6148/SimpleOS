/**
 * SimpleOS Kernel Client Header
 * Client library for communicating with SimpleOS kernel
 */

#ifndef SIMPLEOS_KERNEL_CLIENT_H
#define SIMPLEOS_KERNEL_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * KERNEL CLIENT TYPES
     * =========================================================================== */

    /**
     * Kernel client handle
     */
    typedef struct kernel_client kernel_client_t;

    /**
     * Process information
     */
    typedef struct
    {
        uint32_t pid;
        uint32_t priority;
        char name[64];
        char state[32];
        uint32_t memory;
        uint32_t cpu_time;
    } kernel_process_t;

    /**
     * Memory information
     */
    typedef struct
    {
        uint32_t total;
        uint32_t used;
        uint32_t free;
        uint32_t available;
    } kernel_memory_t;

    /**
     * Kernel statistics
     */
    typedef struct
    {
        uint32_t uptime_ms;
        uint32_t total_switches;
        uint32_t total_syscalls;
        uint32_t context_switches_per_sec;
    } kernel_stats_t;

    /**
     * Kernel state snapshot
     */
    typedef struct
    {
        uint32_t timestamp;
        uint32_t num_processes;
        kernel_process_t processes[32];
        kernel_memory_t memory;
        kernel_stats_t stats;
        char message[256];
    } kernel_snapshot_t;

    /**
     * Kernel request types
     */
    typedef enum
    {
        KERNEL_REQ_GET_STATUS = 1,
        KERNEL_REQ_GET_PROCESSES = 2,
        KERNEL_REQ_GET_MEMORY = 3,
        KERNEL_REQ_GET_STATS = 4,
        KERNEL_REQ_EXECUTE_COMMAND = 5,
        KERNEL_REQ_KILL_PROCESS = 6,
        KERNEL_REQ_GET_FILES = 7,
        KERNEL_REQ_READ_FILE = 8,
        KERNEL_REQ_WRITE_FILE = 9,
        KERNEL_REQ_LIST_DEVICES = 10
    } kernel_request_type_t;

    /**
     * Kernel response status
     */
    typedef enum
    {
        KERNEL_RESP_OK = 0,
        KERNEL_RESP_ERROR = -1,
        KERNEL_RESP_NOT_FOUND = -2,
        KERNEL_RESP_PERMISSION_DENIED = -3,
        KERNEL_RESP_TIMEOUT = -4
    } kernel_response_status_t;

    /**
     * Kernel response
     */
    typedef struct
    {
        kernel_response_status_t status;
        union
        {
            kernel_snapshot_t snapshot;
            kernel_process_t process;
            kernel_memory_t memory;
            kernel_stats_t stats;
            char data[4096];
        } data;
    } kernel_response_t;

    /* ============================================================================
     * CONNECTION MANAGEMENT
     * =========================================================================== */

    /**
     * Create kernel client
     * @param host Host address (IP or "localhost")
     * @param port Port number (default 8888)
     * @return Client handle or NULL on error
     */
    kernel_client_t *kernel_client_create(const char *host, int port);

    /**
     * Destroy kernel client
     * @param client Client handle
     */
    void kernel_client_destroy(kernel_client_t *client);

    /**
     * Connect to kernel service
     * @param client Client handle
     * @return 0 on success, -1 on error
     */
    int kernel_client_connect(kernel_client_t *client);

    /**
     * Disconnect from kernel service
     * @param client Client handle
     */
    void kernel_client_disconnect(kernel_client_t *client);

    /**
     * Check if connected
     * @param client Client handle
     * @return true if connected, false otherwise
     */
    bool kernel_client_is_connected(kernel_client_t *client);

    /**
     * Set connection timeout
     * @param client Client handle
     * @param timeout_ms Timeout in milliseconds
     */
    void kernel_client_set_timeout(kernel_client_t *client, int timeout_ms);

    /* ============================================================================
     * KERNEL QUERIES
     * =========================================================================== */

    /**
     * Get kernel status
     * @param client Client handle
     * @param response Response buffer
     * @return 0 on success, -1 on error
     */
    int kernel_client_get_status(kernel_client_t *client, kernel_response_t *response);

    /**
     * Get process list
     * @param client Client handle
     * @param processes Process buffer array
     * @param max_count Maximum number of processes to retrieve
     * @param count Pointer to receive actual count
     * @return 0 on success, -1 on error
     */
    int kernel_client_get_processes(kernel_client_t *client, kernel_process_t *processes,
                                    size_t max_count, size_t *count);

    /**
     * Get memory information
     * @param client Client handle
     * @param memory Memory info buffer
     * @return 0 on success, -1 on error
     */
    int kernel_client_get_memory(kernel_client_t *client, kernel_memory_t *memory);

    /**
     * Get kernel statistics
     * @param client Client handle
     * @param stats Stats buffer
     * @return 0 on success, -1 on error
     */
    int kernel_client_get_stats(kernel_client_t *client, kernel_stats_t *stats);

    /**
     * Get complete kernel snapshot
     * @param client Client handle
     * @param snapshot Snapshot buffer
     * @return 0 on success, -1 on error
     */
    int kernel_client_get_snapshot(kernel_client_t *client, kernel_snapshot_t *snapshot);

    /* ============================================================================
     * KERNEL COMMANDS
     * =========================================================================== */

    /**
     * Execute command in kernel
     * @param client Client handle
     * @param command Command string
     * @param response Response buffer
     * @return 0 on success, -1 on error
     */
    int kernel_client_execute_command(kernel_client_t *client, const char *command,
                                      kernel_response_t *response);

    /**
     * Terminate process
     * @param client Client handle
     * @param pid Process ID
     * @return 0 on success, -1 on error
     */
    int kernel_client_kill_process(kernel_client_t *client, uint32_t pid);

    /**
     * Send signal to process
     * @param client Client handle
     * @param pid Process ID
     * @param signal Signal number
     * @return 0 on success, -1 on error
     */
    int kernel_client_signal_process(kernel_client_t *client, uint32_t pid, int signal);

    /* ============================================================================
     * FILE SYSTEM OPERATIONS
     * =========================================================================== */

    /**
     * List directory contents
     * @param client Client handle
     * @param path Directory path
     * @param entries Entry buffer
     * @param max_entries Maximum entries
     * @param count Pointer to receive actual count
     * @return 0 on success, -1 on error
     */
    int kernel_client_list_files(kernel_client_t *client, const char *path,
                                 char **entries, size_t max_entries, size_t *count);

    /**
     * Read file contents
     * @param client Client handle
     * @param path File path
     * @param buffer Data buffer
     * @param size Buffer size
     * @param bytes_read Pointer to receive bytes read
     * @return 0 on success, -1 on error
     */
    int kernel_client_read_file(kernel_client_t *client, const char *path,
                                void *buffer, size_t size, size_t *bytes_read);

    /**
     * Write file contents
     * @param client Client handle
     * @param path File path
     * @param buffer Data buffer
     * @param size Bytes to write
     * @param bytes_written Pointer to receive bytes written
     * @return 0 on success, -1 on error
     */
    int kernel_client_write_file(kernel_client_t *client, const char *path,
                                 const void *buffer, size_t size, size_t *bytes_written);

    /* ============================================================================
     * ERROR HANDLING
     * =========================================================================== */

    /**
     * Get last error message
     * @param client Client handle
     * @return Error message string
     */
    const char *kernel_client_get_error(kernel_client_t *client);

    /**
     * Clear error state
     * @param client Client handle
     */
    void kernel_client_clear_error(kernel_client_t *client);

    /**
     * Convert response status to string
     * @param status Response status
     * @return Status string
     */
    const char *kernel_response_status_str(kernel_response_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEOS_KERNEL_CLIENT_H */
