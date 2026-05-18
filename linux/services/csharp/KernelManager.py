#!/usr/bin/env python3
"""
SimpleOS Kernel Manager - Python Service
Provides system monitoring and kernel management
"""

import os
import sys
import json
import socket
import threading
import subprocess
from datetime import datetime
from typing import Dict, List, Optional
import psutil
from flask import Flask, jsonify, request
from flask_cors import CORS

class KernelManager:
    """Manages kernel operations and system monitoring"""

    def __init__(self, kernel_host='localhost', kernel_port=9999):
        self.kernel_host = kernel_host
        self.kernel_port = kernel_port
        self.socket = None
        self.connected = False

    def connect(self) -> bool:
        """Connect to kernel server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.kernel_host, self.kernel_port))
            self.connected = True
            return True
        except Exception as e:
            print(f"Failed to connect to kernel: {e}")
            return False

    def send_command(self, command: str, params: Dict = None) -> Dict:
        """Send command to kernel and receive response"""
        if not self.connected:
            self.connect()

        try:
            request_data = {
                'cmd': command,
                'params': params or {},
                'timestamp': datetime.now().isoformat()
            }

            self.socket.sendall(json.dumps(request_data).encode() + b'\n')
            response = self.socket.recv(4096).decode()
            return json.loads(response)
        except Exception as e:
            print(f"Error sending command: {e}")
            return {'status': 'error', 'message': str(e)}

    def get_processes(self) -> List[Dict]:
        """Get list of all running processes"""
        processes = []
        for proc in psutil.process_iter(['pid', 'name', 'memory_info', 'status']):
            try:
                pinfo = proc.as_dict(attrs=['pid', 'name', 'memory_info', 'status'])
                processes.append({
                    'pid': pinfo['pid'],
                    'name': pinfo['name'],
                    'memory': pinfo['memory_info'].rss / 1024,  # KB
                    'status': pinfo['status']
                })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                pass

        return sorted(processes, key=lambda x: x['memory'], reverse=True)

    def get_system_stats(self) -> Dict:
        """Get system statistics"""
        vm = psutil.virtual_memory()
        cpu_percent = psutil.cpu_percent(interval=0.1)

        return {
            'total_memory': vm.total / (1024 * 1024),  # MB
            'used_memory': vm.used / (1024 * 1024),
            'free_memory': vm.available / (1024 * 1024),
            'memory_percent': vm.percent,
            'cpu_percent': cpu_percent,
            'cpu_count': psutil.cpu_count(),
            'load_average': os.getloadavg() if hasattr(os, 'getloadavg') else [0, 0, 0]
        }

    def get_process_info(self, pid: int) -> Optional[Dict]:
        """Get detailed information about a specific process"""
        try:
            proc = psutil.Process(pid)
            return {
                'pid': proc.pid,
                'name': proc.name(),
                'status': proc.status(),
                'memory': proc.memory_info().rss / 1024,  # KB
                'cpu_percent': proc.cpu_percent(interval=0.1),
                'create_time': datetime.fromtimestamp(proc.create_time()).isoformat(),
                'num_threads': proc.num_threads(),
                'nice': proc.nice()
            }
        except psutil.NoSuchProcess:
            return None

    def kill_process(self, pid: int) -> bool:
        """Kill a process"""
        try:
            os.kill(pid, 15)  # SIGTERM
            return True
        except Exception as e:
            print(f"Failed to kill process: {e}")
            return False

    def get_disk_stats(self) -> Dict:
        """Get disk statistics"""
        partitions = {}
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                partitions[partition.mountpoint] = {
                    'total': usage.total / (1024 * 1024 * 1024),  # GB
                    'used': usage.used / (1024 * 1024 * 1024),
                    'free': usage.free / (1024 * 1024 * 1024),
                    'percent': usage.percent
                }
            except PermissionError:
                pass

        return partitions

    def get_network_stats(self) -> Dict:
        """Get network statistics"""
        stats = psutil.net_if_stats()
        io_counters = psutil.net_io_counters()

        return {
            'interfaces': {name: {
                'isup': stat.isup,
                'speed': stat.speed,
                'mtu': stat.mtu
            } for name, stat in stats.items()},
            'total': {
                'bytes_sent': io_counters.bytes_sent,
                'bytes_recv': io_counters.bytes_recv,
                'packets_sent': io_counters.packets_sent,
                'packets_recv': io_counters.packets_recv
            }
        }


# Create Flask application
app = Flask(__name__)
CORS(app)

# Initialize kernel manager
kernel_manager = KernelManager()

# ============================================================================
# API ROUTES
# ============================================================================

@app.route('/api/processes', methods=['GET'])
def get_processes():
    """Get list of all running processes"""
    try:
        processes = kernel_manager.get_processes()
        return jsonify({
            'status': 'success',
            'data': processes,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/stats', methods=['GET'])
def get_system_stats():
    """Get system statistics"""
    try:
        stats = kernel_manager.get_system_stats()
        return jsonify({
            'status': 'success',
            'data': stats,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/process/<int:pid>', methods=['GET'])
def get_process_info(pid):
    """Get detailed process information"""
    try:
        process_info = kernel_manager.get_process_info(pid)
        if process_info is None:
            return jsonify({
                'status': 'error',
                'message': f'Process {pid} not found',
                'timestamp': datetime.now().isoformat()
            }), 404

        return jsonify({
            'status': 'success',
            'data': process_info,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/process/<int:pid>/kill', methods=['POST'])
def kill_process(pid):
    """Kill a process"""
    try:
        result = kernel_manager.kill_process(pid)
        return jsonify({
            'status': 'success' if result else 'error',
            'data': result,
            'message': 'Process killed successfully' if result else 'Failed to kill process',
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/disk', methods=['GET'])
def get_disk_stats():
    """Get disk statistics"""
    try:
        stats = kernel_manager.get_disk_stats()
        return jsonify({
            'status': 'success',
            'data': stats,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/network', methods=['GET'])
def get_network_stats():
    """Get network statistics"""
    try:
        stats = kernel_manager.get_network_stats()
        return jsonify({
            'status': 'success',
            'data': stats,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e),
            'timestamp': datetime.now().isoformat()
        }), 500


@app.route('/api/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        'status': 'success',
        'service': 'SimpleOS Kernel Manager',
        'version': '1.0.0',
        'timestamp': datetime.now().isoformat()
    })


if __name__ == '__main__':
    print("[SimpleOS] Starting Kernel Manager Service...")
    print("[SimpleOS] Listening on http://0.0.0.0:5000")
    app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)