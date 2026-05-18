<?php

/**
 * SimpleOS Kernel API - PHP Backend
 * Provides REST API for kernel management
 */

class KernelAPI {
    private $socket;
    private $host;
    private $port;

    public function __construct($host = 'localhost', $port = 9999) {
        $this->host = $host;
        $this->port = $port;
    }

    /**
     * Connect to kernel server
     */
    public function connect() {
        $this->socket = fsockopen($this->host, $this->port, $errno, $errstr, 5);
        if (!$this->socket) {
            throw new Exception("Could not connect to kernel: $errstr");
        }
        return true;
    }

    /**
     * Send command to kernel and get response
     */
    public function sendCommand($command, $params = []) {
        if (!$this->socket) {
            $this->connect();
        }

        $request = json_encode([
            'cmd' => $command,
            'params' => $params,
            'timestamp' => time()
        ]);

        fwrite($this->socket, $request . "\n");
        $response = fgets($this->socket, 4096);
        
        return json_decode($response, true);
    }

    /**
     * Get list of all processes
     */
    public function getProcesses() {
        return $this->sendCommand('list_processes');
    }

    /**
     * Get system statistics
     */
    public function getSystemStats() {
        return $this->sendCommand('system_stats');
    }

    /**
     * Kill a process
     */
    public function killProcess($pid) {
        return $this->sendCommand('kill_process', ['pid' => $pid]);
    }

    /**
     * Get detailed process information
     */
    public function getProcessInfo($pid) {
        return $this->sendCommand('process_info', ['pid' => $pid]);
    }

    public function __destruct() {
        if ($this->socket) {
            fclose($this->socket);
        }
    }
}

/**
 * REST API Router
 */
class APIRouter {
    private $kernel;

    public function __construct() {
        $this->kernel = new KernelAPI();
        $this->kernel->connect();
    }

    /**
     * Route incoming request
     */
    public function route() {
        header('Content-Type: application/json');

        $method = $_SERVER['REQUEST_METHOD'];
        $path = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
        $path = trim($path, '/');

        try {
            if ($method === 'GET') {
                $this->handleGet($path);
            } elseif ($method === 'POST') {
                $this->handlePost($path);
            } else {
                $this->sendError(405, 'Method not allowed');
            }
        } catch (Exception $e) {
            $this->sendError(500, $e->getMessage());
        }
    }

    /**
     * Handle GET requests
     */
    private function handleGet($path) {
        if ($path === 'api/processes') {
            $processes = $this->kernel->getProcesses();
            $this->sendJSON($processes);
        } elseif ($path === 'api/stats') {
            $stats = $this->kernel->getSystemStats();
            $this->sendJSON($stats);
        } elseif (strpos($path, 'api/process/') === 0) {
            $pid = (int)substr($path, strlen('api/process/'));
            $info = $this->kernel->getProcessInfo($pid);
            $this->sendJSON($info);
        } else {
            $this->sendError(404, 'Not found');
        }
    }

    /**
     * Handle POST requests
     */
    private function handlePost($path) {
        $data = json_decode(file_get_contents('php://input'), true);

        if ($path === 'api/process/kill') {
            if (!isset($data['pid'])) {
                $this->sendError(400, 'Missing pid parameter');
                return;
            }
            $result = $this->kernel->killProcess($data['pid']);
            $this->sendJSON($result);
        } else {
            $this->sendError(404, 'Not found');
        }
    }

    /**
     * Send JSON response
     */
    private function sendJSON($data) {
        echo json_encode([
            'status' => 'success',
            'data' => $data,
            'timestamp' => time()
        ]);
    }

    /**
     * Send error response
     */
    private function sendError($code, $message) {
        http_response_code($code);
        echo json_encode([
            'status' => 'error',
            'code' => $code,
            'message' => $message,
            'timestamp' => time()
        ]);
    }
}

// Initialize and route
$router = new APIRouter();
$router->route();
?>