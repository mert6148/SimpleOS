/**
 * SimpleOS Kernel Backend Service
 * WebSocket server bridging React frontend and kernel communication
 * 
 * Provides:
 * - Real-time kernel monitoring (processes, memory, scheduler stats)
 * - Command routing from frontend to kernel
 * - Response formatting and broadcasting
 * - Kernel data polling and caching
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <map>
#include <memory>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

#define WEBSOCKET_PORT 8888
#define KERNEL_SERIAL_PORT "/dev/ttyS0"
#define KERNEL_BAUD_RATE 9600
#define POLLING_INTERVAL_MS 100
#define MAX_CONNECTIONS 10
#define MAX_MESSAGE_SIZE 4096
#define KERNEL_TIMEOUT_MS 1000

// ============================================================================
// KERNEL DATA STRUCTURES
// ============================================================================

/**
 * Process information from kernel
 */
struct KernelProcess {
    u_int32_t pid;
    u_int32_t priority;
    const char* name;
    const char* state;
    u_int32_t memory;
    u_int32_t cpu_time;
};

/**
 * Memory information from kernel
 */
struct KernelMemory {
    u_int32_t total;
    u_int32_t used;
    u_int32_t free;
    u_int32_t available;
};

/**
 * Kernel statistics
 */
struct KernelStats {
    u_int32_t uptime_ms;
    u_int32_t total_switches;
    u_int32_t total_syscalls;
    u_int32_t context_switches_per_sec;
};

/**
 * Kernel state snapshot
 */
struct KernelSnapshot {
    u_int32_t timestamp;
    u_int32_t num_processes;
    KernelProcess processes[32];
    KernelMemory memory;
    KernelStats stats;
    char last_message[256];
};

// ============================================================================
// GLOBAL STATE
// ============================================================================

volatile bool g_running = true;
int g_serial_fd = -1;
std::mutex g_data_mutex;
KernelSnapshot g_kernel_data;
std::queue<std::string> g_command_queue;

// ============================================================================
// SERIAL COMMUNICATION
// ============================================================================

/**
 * Initialize serial connection to kernel
 */
int kernel_serial_init(const char* port) {
    g_serial_fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (g_serial_fd < 0) {
        std::cerr << "Failed to open serial port: " << port << std::endl;
        return -1;
    }
    
    // Configure serial port for 9600 baud, 8N1
    // (Simplified - full implementation would use termios)
    std::cout << "Serial port opened: " << port << std::endl;
    return 0;
}

/**
 * Send command to kernel via serial
 */
int kernel_send_command(const char* command) {
    if (g_serial_fd < 0) {
        std::cerr << "Serial port not initialized" << std::endl;
        return -1;
    }
    
    size_t len = strlen(command);
    ssize_t written = write(g_serial_fd, command, len);
    
    if (written < 0) {
        std::cerr << "Failed to send command to kernel" << std::endl;
        return -1;
    }
    
    std::cout << "[KERNEL] Sent: " << command << std::endl;
    return 0;
}

/**
 * Read response from kernel via serial
 */
int kernel_read_response(char* buffer, size_t max_len) {
    if (g_serial_fd < 0) return -1;
    
    ssize_t bytes_read = read(g_serial_fd, buffer, max_len - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        return bytes_read;
    }
    
    return 0;
}

/**
 * Close serial connection
 */
void kernel_serial_close() {
    if (g_serial_fd >= 0) {
        close(g_serial_fd);
        g_serial_fd = -1;
    }
}

// ============================================================================
// KERNEL DATA PARSING
// ============================================================================

/**
 * Poll kernel for current state
 */
void kernel_poll_data() {
    // Request process list
    kernel_send_command("ps\n");
    usleep(50000);  // 50ms wait for response
    
    // Request memory status
    kernel_send_command("meminfo\n");
    usleep(50000);
    
    // Request scheduler stats
    kernel_send_command("sched\n");
    usleep(50000);
    
    // Collect any available data
    char buffer[4096];
    int bytes = kernel_read_response(buffer, sizeof(buffer));
    
    if (bytes > 0) {
        std::lock_guard<std::mutex> lock(g_data_mutex);
        strncpy(g_kernel_data.last_message, buffer, sizeof(g_kernel_data.last_message) - 1);
        g_kernel_data.timestamp = time(nullptr);
    }
}

/**
 * Parse process list response from kernel
 */
void parse_processes(const char* data) {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    
    // Simple parsing: expects format "PID PRIORITY NAME STATE MEMORY CPUTIME"
    // This is a placeholder - actual parsing depends on kernel output format
    
    g_kernel_data.num_processes = 0;
    // Parse logic here...
}

/**
 * Parse memory information from kernel
 */
void parse_memory(const char* data) {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    
    // Example parsing
    sscanf(data, "Total: %u, Used: %u, Free: %u",
           &g_kernel_data.memory.total,
           &g_kernel_data.memory.used,
           &g_kernel_data.memory.free);
    
    g_kernel_data.memory.available = 
        g_kernel_data.memory.total - g_kernel_data.memory.used;
}

/**
 * Update kernel snapshot with default values
 */
void kernel_update_snapshot() {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    
    g_kernel_data.timestamp = time(nullptr);
    
    // Default sample data (replace with real kernel polling)
    g_kernel_data.memory.total = 256 * 1024 * 1024;  // 256MB
    g_kernel_data.memory.used = 128 * 1024 * 1024;   // 128MB
    g_kernel_data.memory.free = 128 * 1024 * 1024;   // 128MB
    g_kernel_data.memory.available = 128 * 1024 * 1024;
    
    g_kernel_data.stats.uptime_ms += POLLING_INTERVAL_MS;
    g_kernel_data.stats.total_switches += 10;
    g_kernel_data.stats.context_switches_per_sec = 1000;
    
    g_kernel_data.num_processes = 3;
    
    // Sample processes
    strcpy((char*)g_kernel_data.processes[0].name, "init");
    g_kernel_data.processes[0].pid = 1;
    g_kernel_data.processes[0].priority = 0;
    strcpy((char*)g_kernel_data.processes[0].state, "running");
    
    strcpy((char*)g_kernel_data.processes[1].name, "shell");
    g_kernel_data.processes[1].pid = 2;
    g_kernel_data.processes[1].priority = 2;
    strcpy((char*)g_kernel_data.processes[1].state, "sleeping");
    
    strcpy((char*)g_kernel_data.processes[2].name, "idle");
    g_kernel_data.processes[2].pid = 0;
    g_kernel_data.processes[2].priority = -1;
    strcpy((char*)g_kernel_data.processes[2].state, "running");
}

// ============================================================================
// JSON FORMATTING
// ============================================================================

/**
 * Convert kernel snapshot to JSON
 */
std::string kernel_snapshot_to_json() {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    
    std::string json = "{";
    
    // Timestamp
    char timestamp[32];
    sprintf(timestamp, "%u", g_kernel_data.timestamp);
    json += "\"timestamp\":" + std::string(timestamp) + ",";
    
    // Memory information
    json += "\"memory\":{";
    char mem_total[32], mem_used[32], mem_free[32];
    sprintf(mem_total, "%u", g_kernel_data.memory.total);
    sprintf(mem_used, "%u", g_kernel_data.memory.used);
    sprintf(mem_free, "%u", g_kernel_data.memory.free);
    json += "\"total\":" + std::string(mem_total) + ",";
    json += "\"used\":" + std::string(mem_used) + ",";
    json += "\"free\":" + std::string(mem_free) + "},";
    
    // Kernel statistics
    json += "\"stats\":{";
    char uptime[32], switches[32], syscalls[32];
    sprintf(uptime, "%u", g_kernel_data.stats.uptime_ms);
    sprintf(switches, "%u", g_kernel_data.stats.total_switches);
    sprintf(syscalls, "%u", g_kernel_data.stats.total_syscalls);
    json += "\"uptime_ms\":" + std::string(uptime) + ",";
    json += "\"switches\":" + std::string(switches) + ",";
    json += "\"syscalls\":" + std::string(syscalls) + "},";
    
    // Processes array
    json += "\"processes\":[";
    for (u_int32_t i = 0; i < g_kernel_data.num_processes; i++) {
        if (i > 0) json += ",";
        
        char pid_str[32], priority_str[32], memory_str[32];
        sprintf(pid_str, "%u", g_kernel_data.processes[i].pid);
        sprintf(priority_str, "%u", g_kernel_data.processes[i].priority);
        sprintf(memory_str, "%u", g_kernel_data.processes[i].memory);
        
        json += "{";
        json += "\"pid\":" + std::string(pid_str) + ",";
        json += "\"name\":\"" + std::string(g_kernel_data.processes[i].name) + "\",";
        json += "\"priority\":" + std::string(priority_str) + ",";
        json += "\"state\":\"" + std::string(g_kernel_data.processes[i].state) + "\",";
        json += "\"memory\":" + std::string(memory_str);
        json += "}";
    }
    json += "],";
    
    // Status message
    json += "\"message\":\"" + std::string(g_kernel_data.last_message) + "\"";
    json += "}";
    
    return json;
}

// ============================================================================
// COMMAND PROCESSING
// ============================================================================

/**
 * Process command from frontend
 */
void process_command(const std::string& command) {
    std::cout << "[COMMAND] Processing: " << command << std::endl;
    
    if (command == "ps") {
        // Send process list command to kernel
        kernel_send_command("ps\n");
    } else if (command == "meminfo") {
        // Send memory info command to kernel
        kernel_send_command("meminfo\n");
    } else if (command == "sched") {
        // Send scheduler stats command to kernel
        kernel_send_command("sched\n");
    } else if (command.find("kill") == 0) {
        // Kill process command
        kernel_send_command((command + "\n").c_str());
    } else {
        // Forward to kernel
        kernel_send_command((command + "\n").c_str());
    }
}

// ============================================================================
// KERNEL POLLING THREAD
// ============================================================================

/**
 * Thread function: continuously poll kernel for data
 */
void kernel_polling_thread() {
    std::cout << "[POLLER] Kernel polling thread started" << std::endl;
    
    while (g_running) {
        // Update snapshot with fresh data
        kernel_update_snapshot();
        
        // Process any pending commands
        while (!g_command_queue.empty()) {
            std::string cmd = g_command_queue.front();
            g_command_queue.pop();
            process_command(cmd);
        }
        
        // Sleep for polling interval
        usleep(POLLING_INTERVAL_MS * 1000);
    }
    
    std::cout << "[POLLER] Kernel polling thread stopped" << std::endl;
}

// ============================================================================
// WEBSOCKET SERVER
// ============================================================================

/**
 * Simple WebSocket frame encoding (text frame)
 */
std::string websocket_frame_encode(const std::string& payload) {
    std::string frame;
    
    // FIN + opcode (0x81 = text frame)
    frame += (char)0x81;
    
    // Payload length
    size_t len = payload.length();
    if (len < 126) {
        frame += (char)len;
    } else if (len < 65536) {
        frame += (char)126;
        frame += (char)((len >> 8) & 0xFF);
        frame += (char)(len & 0xFF);
    }
    
    // Payload (no mask for server→client)
    frame += payload;
    
    return frame;
}

/**
 * Handle WebSocket connection
 */
void handle_websocket_client(int client_fd) {
    char buffer[MAX_MESSAGE_SIZE];
    
    // Simple WebSocket handshake (minimal implementation)
    std::cout << "[WS] Client connected, FD=" << client_fd << std::endl;
    
    // Send welcome message
    std::string welcome = kernel_snapshot_to_json();
    std::string frame = websocket_frame_encode(welcome);
    send(client_fd, frame.c_str(), frame.length(), 0);
    
    // Main message loop
    while (g_running) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            break;  // Client disconnected
        }
        
        // Extract command from WebSocket frame (simplified)
        std::string msg(buffer);
        if (msg.find("command") != std::string::npos) {
            // Parse command and queue it
            size_t start = msg.find("\"command\":\"");
            if (start != std::string::npos) {
                start += 11;
                size_t end = msg.find("\"", start);
                std::string cmd = msg.substr(start, end - start);
                g_command_queue.push(cmd);
            }
        }
        
        // Send current snapshot
        std::string response = kernel_snapshot_to_json();
        std::string frame_out = websocket_frame_encode(response);
        send(client_fd, frame_out.c_str(), frame_out.length(), 0);
    }
    
    close(client_fd);
    std::cout << "[WS] Client disconnected, FD=" << client_fd << std::endl;
}

/**
 * WebSocket server main loop
 */
void websocket_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[WS] Failed to create socket" << std::endl;
        return;
    }
    
    // Enable socket reuse
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // Bind to port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(WEBSOCKET_PORT);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[WS] Failed to bind to port " << WEBSOCKET_PORT << std::endl;
        close(server_fd);
        return;
    }
    
    // Listen for connections
    listen(server_fd, MAX_CONNECTIONS);
    std::cout << "[WS] Server listening on localhost:" << WEBSOCKET_PORT << std::endl;
    
    while (g_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd >= 0) {
            // Create thread to handle client
            std::thread client_thread(handle_websocket_client, client_fd);
            client_thread.detach();
        }
    }
    
    close(server_fd);
    std::cout << "[WS] Server stopped" << std::endl;
}

// ============================================================================
// SIGNAL HANDLING
// ============================================================================

void signal_handler(int sig) {
    std::cout << "\n[MAIN] Shutdown signal received" << std::endl;
    g_running = false;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "SimpleOS Kernel Backend Service" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize kernel serial connection
    if (kernel_serial_init(KERNEL_SERIAL_PORT) < 0) {
        std::cerr << "Warning: Could not connect to kernel serial port" << std::endl;
        std::cout << "Running in simulation mode" << std::endl;
    }
    
    // Start kernel polling thread
    std::thread poller(kernel_polling_thread);
    
    // Start WebSocket server (blocks until shutdown)
    websocket_server();
    
    // Wait for poller thread
    poller.join();
    
    // Cleanup
    kernel_serial_close();
    
    std::cout << "[MAIN] Shutdown complete" << std::endl;
    return 0;
}
