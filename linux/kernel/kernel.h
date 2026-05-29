#ifndef LINUX_KERNEL_H
#define LINUX_KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* ============================================================================
 * KERNEL TYPES AND DEFINITIONS
 * ========================================================================== */

typedef int KERNEL_STATUS;
#define KERNEL_SUCCESS 0
#define KERNEL_ERROR -1
#define KERNEL_ENOMEM -2
#define KERNEL_EINVAL -3

typedef struct
{
    pid_t pid;
    char name[256];
    int priority;
    int state;
    long memory_usage;
} process_info_t;

typedef struct
{
    int total_memory;
    int used_memory;
    int free_memory;
    float load_average;
} system_stats_t;

typedef struct
{
    int server_socket;
    int port;
    pthread_t listener_thread;
    int running;
} kernel_server_t;

/* ============================================================================
 * KERNEL API FUNCTIONS
 * ========================================================================== */

// Initialization
KERNEL_STATUS kernel_init(kernel_server_t *kernel, int port);
KERNEL_STATUS kernel_start(kernel_server_t *kernel);
KERNEL_STATUS kernel_stop(kernel_server_t *kernel);

// Process Management
KERNEL_STATUS kernel_list_processes(process_info_t **processes, int *count);
KERNEL_STATUS kernel_get_process_info(pid_t pid, process_info_t *info);
KERNEL_STATUS kernel_kill_process(pid_t pid);

// System Information
KERNEL_STATUS kernel_get_system_stats(system_stats_t *stats);
KERNEL_STATUS kernel_get_uptime(long *uptime);

// IPC and Communication
KERNEL_STATUS kernel_create_message_queue(const char *name);
KERNEL_STATUS kernel_send_message(const char *queue, const char *message);
KERNEL_STATUS kernel_receive_message(const char *queue, char *buffer, int size);

#endif // LINUX_KERNEL_H