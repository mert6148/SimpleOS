# SimpleOS Kernel & System Protocol Manager

Comprehensive system management toolkit for SimpleOS featuring kernel operations, system protocols, and network utilities.

## Features

### Kernel Management
- **Kernel Initialization**: Initialize SimpleOS kernel systems with modules and drivers
- **Kernel Information**: Display detailed kernel configuration and system information
- **Kernel Status**: Monitor kernel runtime status, modules, drivers, and memory usage
- **System Info**: OS name, version, architecture, hostname, boot time tracking

### System Protocols
- **Protocol Discovery**: TCP, UDP, ICMP, and IPv4 protocol support
- **Network Interfaces**: Discover and manage system network interfaces
- **IP Validation**: Validate IPv4 addresses with CIDR range support
- **IP Classification**: Classify IPs as public, private, loopback, multicast, or broadcast

### Network Management
- **Endpoint Validation**: Validate IP and port combinations
- **Port Classification**: Identify reserved, registered, and dynamic ports
- **Well-known Ports**: Built-in mapping of standard service ports (SSH, HTTP, HTTPS, etc.)
- **Network Status**: Monitor validated IPs, cached endpoints, and active interfaces

### Package Management
- **Package Listing**: View all available system packages
- **Package Installation**: Install packages with dependency resolution
- **Package Information**: Display detailed package metadata

## Installation

### Requirements
- PHP 7.4 or higher
- SimpleOS kernel environment

### Setup

```bash
cd script/package
php package.php list  # Verify installation
```

## Usage

### Kernel Commands

Display comprehensive kernel information:
```bash
php package.php kernel
```

Initialize kernel systems:
```bash
php package.php kernel-init
```

Check kernel runtime status:
```bash
php package.php kernel-status
```

### Protocol Commands

Show active system protocols:
```bash
php package.php protocol
```

Display network interfaces and status:
```bash
php package.php network
```

### Network Validation

Validate IPv4 address:
```bash
php package.php validate-ip 192.168.1.1
php package.php validate-ip 10.0.0.1
php package.php validate-ip 127.0.0.1
```

Check endpoint (IP + port):
```bash
php package.php endpoint 192.168.1.1 8080
php package.php endpoint 127.0.0.1 3306
php package.php endpoint 0.0.0.0 443
```

### Package Commands

List all packages:
```bash
php package.php list
```

Install a package:
```bash
php package.php install kernel-headers
php package.php install shell-tools
```

Get package information:
```bash
php package.php info kernel-headers
php package.php info core-utils
```

### Application Package Commands

List available application packages:
```bash
php package.php app list
```

Install an application package:
```bash
php package.php app install text-editor
```

Show application package details:
```bash
php package.php app info file-manager
```

Run an installed application package:
```bash
php package.php app run text-editor -- notes.txt
```

Remove an installed application package:
```bash
php package.php app remove process-viewer
```

## Architecture

### Directory Structure

```
script/package/
├── src/
│   ├── App/
│   │   ├── Kernel/
│   │   │   └── Kernel.php              # Kernel system manager (singleton)
│   │   └── Script/
│   │       ├── Package/                # Package management
│   │       │   ├── PackageApplication.php
│   │       │   ├── PackageManager.php
│   │       │   ├── PackageRepository.php
│   │       │   ├── PackageInstaller.php
│   │       │   ├── Package.php
│   │       │   └── PackageInterface.php
│   │       └── Sys Protocol/           # System protocols & networking
│   │           ├── sysprotocol.php     # Protocol handler
│   │           └── NetworkHelper.php   # Network utilities
│   └── bootstrap.php                   # PSR-4 autoloader
├── package.php                         # CLI entry point
├── composer.json                       # Project metadata
└── README.md                           # This file
```

### Core Classes

#### Kernel (Singleton)
Manages kernel initialization, configuration, modules, and drivers.

**Key Methods:**
- `getInstance()`: Get singleton kernel instance
- `initialize()`: Initialize kernel systems
- `getConfig($key)`: Get configuration values
- `getStatus()`: Get kernel runtime status
- `getKernelInfo()`: Get formatted kernel information

#### SysProtocol
Manages system protocols, network interfaces, and IP validation.

**Key Methods:**
- `validateIPv4($ip)`: Validate IPv4 address
- `isPrivateIP($ip)`: Check if IP is private
- `validateEndpoint($ip, $port)`: Validate endpoint
- `getNetworkInterfaces()`: Get active interfaces
- `getStatus()`: Get protocol status

#### NetworkHelper
Provides network utilities and endpoint management.

**Key Methods:**
- `validateIP($ip)`: Validate IP address
- `classifyIP($ip)`: Classify IP type (public, private, etc.)
- `getEndpointInfo($ip, $port)`: Get endpoint details
- `formatEndpoint($ip, $port)`: Format endpoint for display
- `getNetworkStatus()`: Get network statistics

#### PackageApplication
CLI application controller orchestrating all operations.

**Key Methods:**
- `run($argv)`: Main entry point
- `showKernelInfo()`: Display kernel information
- `initializeKernel()`: Initialize kernel
- `showProtocolInfo()`: Display protocols
- `validateIP($ip)`: Validate IP address

## IP Classification

The system classifies IPv4 addresses into the following categories:

| Type | Range(s) | Usage |
|------|----------|-------|
| **Loopback** | 127.0.0.0/8 | Local machine testing |
| **Private** | 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16 | Local networks |
| **Multicast** | 224.0.0.0/4 | Group communication |
| **Broadcast** | 255.255.255.255 | Network broadcast |
| **Public** | All others | Internet routing |

## Port Classification

| Type | Range | Usage |
|------|-------|-------|
| **Reserved** | 0-1023 | Well-known services, requires privileges |
| **Registered** | 1024-49151 | User applications |
| **Dynamic** | 49152-65535 | Ephemeral/temporary connections |

## Well-known Ports

Built-in support for 9 standard service ports:
- SSH (22), SMTP (25), DNS (53)
- HTTP (80), HTTPS (443)
- MySQL (3306), PostgreSQL (5432), Redis (6379)
- HTTP Proxy (8080)

## Kernel Configuration

Default kernel configuration:
- **Version**: 1.0.0
- **Architecture**: x86_64
- **Memory Model**: Buddy allocator with 4KB pages
- **Scheduler**: Priority-based round-robin (6 levels)
- **Max Memory**: 2GB
- **Tick Rate**: 100Hz
- **Max Tasks**: 256

## System Modules

The kernel loads the following core modules:
1. Memory Manager - Physical/virtual memory management
2. Task Scheduler - Process scheduling and context switching
3. Virtual File System (VFS) - Inode-based file abstraction
4. Interrupt Handler - Exception and interrupt processing
5. Device Manager - Hardware device management

## Initialized Drivers

Three drivers are initialized on kernel startup:
1. **PIC** (Programmable Interrupt Controller) - 16 IRQ lines
2. **PIT** (Programmable Interval Timer) - 100Hz tick rate
3. **Serial** - Serial port I/O (port 0x3F8)

## Examples

### Complete System Check

```bash
# Initialize kernel
php package.php kernel-init

# Check kernel status
php package.php kernel-status

# Display protocols
php package.php protocol

# Show network status
php package.php network

# List available packages
php package.php list
```

### Network Validation Workflow

```bash
# Validate local IP
php package.php validate-ip 192.168.1.100
# Output: Valid, Classification: private

# Check endpoint
php package.php endpoint 192.168.1.100 3306
# Output: Endpoint details with MySQL service identification

# Validate public IP
php package.php validate-ip 8.8.8.8
# Output: Valid, Classification: public

# Validate broadcast
php package.php validate-ip 255.255.255.255
# Output: Valid, Classification: broadcast
```

### Package Management Workflow

```bash
# List packages
php package.php list

# Get kernel-headers info
php package.php info kernel-headers

# Install package with dependencies
php package.php install system-packages
```

## Error Handling

All commands include comprehensive error handling:
- Invalid IP addresses are rejected
- Out-of-range ports are detected
- Missing packages are reported
- Invalid endpoints are identified

## Performance Considerations

- **IP Caching**: Validated IPs are cached to improve performance
- **Endpoint Caching**: Endpoint information is cached for repeated queries
- **Singleton Pattern**: Kernel instance is cached as singleton
- **Protocol Discovery**: Network interfaces discovered once on initialization

## Future Enhancements

- IPv6 support
- Dynamic network interface monitoring
- Extended protocol support (DNS, HTTP, FTP, etc.)
- Network performance metrics
- Kernel module hot-loading
- Advanced packet filtering

## License

Part of SimpleOS educational kernel project.

## Support

For issues or questions, please refer to the SimpleOS documentation or AGENTS.md for detailed kernel specifications.
