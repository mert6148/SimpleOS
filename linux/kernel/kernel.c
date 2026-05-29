#include "kernel.h"
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>

/* ============================================================================
 * GLOBAL STATE
 * ========================================================================== */

static kernel_server_t *g_kernel = NULL;

/* ============================================================================
 * PROCESS MANAGEMENT FUNCTIONS
 * ========================================================================== */

KERNEL_STATUS kernel_list_processes(process_info_t **processes, int *count)
{
	if (!processes || !count)
	{
		return KERNEL_EINVAL;
	}

	DIR *dir = opendir("/proc");
	if (!dir)
	{
		return KERNEL_ERROR;
	}

	// Count processes first
	int process_count = 0;
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (atoi(entry->d_name) > 0)
		{
			process_count++;
		}
	}

	// Allocate memory for processes
	*processes = (process_info_t *)malloc(process_count * sizeof(process_info_t));
	if (!*processes)
	{
		closedir(dir);
		return KERNEL_ENOMEM;
	}

	rewinddir(dir);
	int index = 0;

	// Read process information
	while ((entry = readdir(dir)) != NULL)
	{
		pid_t pid = atoi(entry->d_name);
		if (pid > 0)
		{
			(*processes)[index].pid = pid;

			// Read process name from /proc/[pid]/comm
			char comm_path[256];
			snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm", pid);
			FILE *comm_file = fopen(comm_path, "r");
			if (comm_file)
			{
				fscanf(comm_file, "%255s", (*processes)[index].name);
				fclose(comm_file);
			}
			else
			{
				strcpy((*processes)[index].name, "unknown");
			}

			(*processes)[index].priority = 0;
			(*processes)[index].state = 0;
			(*processes)[index].memory_usage = 0;

			// Read memory usage from /proc/[pid]/status
			char status_path[256];
			snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
			FILE *status_file = fopen(status_path, "r");
			if (status_file)
			{
				char line[256];
				while (fgets(line, sizeof(line), status_file))
				{
					if (strncmp(line, "VmRSS:", 6) == 0)
					{
						sscanf(line, "VmRSS: %ld", &(*processes)[index].memory_usage);
						break;
					}
				}
				fclose(status_file);
			}

			index++;
		}
	}

	closedir(dir);
	*count = process_count;
	return KERNEL_SUCCESS;
}

KERNEL_STATUS kernel_get_process_info(pid_t pid, process_info_t *info)
{
	if (!info)
	{
		return KERNEL_EINVAL;
	}

	char path[256];
	snprintf(path, sizeof(path), "/proc/%d/comm", pid);

	FILE *file = fopen(path, "r");
	if (!file)
	{
		return KERNEL_ERROR;
	}

	fscanf(file, "%255s", info->name);
	fclose(file);

	info->pid = pid;
	info->priority = 0;
	info->state = 0;
	info->memory_usage = 0;

	return KERNEL_SUCCESS;
}

KERNEL_STATUS kernel_kill_process(pid_t pid)
{
	if (pid <= 0)
	{
		return KERNEL_EINVAL;
	}

	if (kill(pid, SIGTERM) != 0)
	{
		return KERNEL_ERROR;
	}

	return KERNEL_SUCCESS;
}

/* ============================================================================
 * SYSTEM INFORMATION FUNCTIONS
 * ========================================================================== */

KERNEL_STATUS kernel_get_system_stats(system_stats_t *stats)
{
	if (!stats)
	{
		return KERNEL_EINVAL;
	}

	FILE *meminfo = fopen("/proc/meminfo", "r");
	if (!meminfo)
	{
		return KERNEL_ERROR;
	}

	char line[256];
	while (fgets(line, sizeof(line), meminfo))
	{
		if (strncmp(line, "MemTotal:", 9) == 0)
		{
			sscanf(line, "MemTotal: %d", &stats->total_memory);
		}
		else if (strncmp(line, "MemFree:", 8) == 0)
		{
			sscanf(line, "MemFree: %d", &stats->free_memory);
		}
	}

	fclose(meminfo);
	stats->used_memory = stats->total_memory - stats->free_memory;

	// Get load average
	double load[3];
	if (getloadavg(load, 3) != -1)
	{
		stats->load_average = (float)load[0];
	}

	return KERNEL_SUCCESS;
}

KERNEL_STATUS kernel_get_uptime(long *uptime)
{
	if (!uptime)
	{
		return KERNEL_EINVAL;
	}

	FILE *uptime_file = fopen("/proc/uptime", "r");
	if (!uptime_file)
	{
		return KERNEL_ERROR;
	}

	double up;
	fscanf(uptime_file, "%lf", &up);
	fclose(uptime_file);

	*uptime = (long)up;
	return KERNEL_SUCCESS;
}

/* ============================================================================
 * SERVER FUNCTIONS
 * ========================================================================== */

static void *kernel_server_thread(void *arg)
{
	kernel_server_t *kernel = (kernel_server_t *)arg;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(kernel->port);

	if (bind(kernel->server_socket, (struct sockaddr *)&server_addr,
			 sizeof(server_addr)) < 0)
	{
		perror("bind");
		return NULL;
	}

	listen(kernel->server_socket, 5);
	printf("[Kernel] Server listening on port %d\n", kernel->port);

	while (kernel->running)
	{
		int client_socket = accept(kernel->server_socket,
								   (struct sockaddr *)&client_addr,
								   &addr_len);
		if (client_socket < 0)
		{
			continue;
		}

		// Handle client request
		char buffer[1024] = {0};
		recv(client_socket, buffer, sizeof(buffer), 0);

		printf("[Kernel] Received command: %s\n", buffer);

		// Send response
		system_stats_t stats;
		if (kernel_get_system_stats(&stats) == KERNEL_SUCCESS)
		{
			char response[512];
			snprintf(response, sizeof(response),
					 "OK|Memory: %d/%d MB|Load: %.2f\n",
					 stats.used_memory / 1024,
					 stats.total_memory / 1024,
					 stats.load_average);
			send(client_socket, response, strlen(response), 0);
		}

		close(client_socket);
	}

	return NULL;
}

KERNEL_STATUS kernel_init(kernel_server_t *kernel, int port)
{
	if (!kernel || port <= 0)
	{
		return KERNEL_EINVAL;
	}

	kernel->port = port;
	kernel->server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (kernel->server_socket < 0)
	{
		return KERNEL_ERROR;
	}

	int opt = 1;
	setsockopt(kernel->server_socket, SOL_SOCKET, SO_REUSEADDR,
			   &opt, sizeof(opt));

	g_kernel = kernel;
	return KERNEL_SUCCESS;
}

KERNEL_STATUS kernel_start(kernel_server_t *kernel)
{
	if (!kernel)
	{
		return KERNEL_EINVAL;
	}

	kernel->running = 1;
	if (pthread_create(&kernel->listener_thread, NULL,
					   kernel_server_thread, kernel) != 0)
	{
		return KERNEL_ERROR;
	}

	return KERNEL_SUCCESS;
}

KERNEL_STATUS kernel_stop(kernel_server_t *kernel)
{
	if (!kernel)
	{
		return KERNEL_EINVAL;
	}

	kernel->running = 0;
	close(kernel->server_socket);
	pthread_join(kernel->listener_thread, NULL);

	return KERNEL_SUCCESS;
}
