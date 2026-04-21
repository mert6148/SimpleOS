<?php
/**
 * SimpleOS Documentation Services
 * RESTful API for managing and serving documentation
 * 
 * @version 1.0.0
 * @author Mert Doğanay
 * @license MIT
 */

header('Content-Type: application/json; charset=utf-8');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type, Authorization');

// ============================================================
// Configuration
// ============================================================
define('DOCS_ROOT', __DIR__);
define('DOCS_VERSION', '1.0.0');
define('DOCS_DB_FILE', DOCS_ROOT . '/docs.json');
define('DOCS_CACHE_DIR', DOCS_ROOT . '/cache');
define('DOCS_LOG_FILE', DOCS_ROOT . '/logs/docs.log');

// Create necessary directories
@mkdir(DOCS_CACHE_DIR, 0755, true);
@mkdir(dirname(DOCS_LOG_FILE), 0755, true);

// ============================================================
// Classes
// ============================================================

class DocsDatabase {
    private $filePath;
    
    public function __construct($filePath) {
        $this->filePath = $filePath;
        $this->ensureFile();
    }
    
    private function ensureFile() {
        if (!file_exists($this->filePath)) {
            $defaultData = [
                'version' => DOCS_VERSION,
                'lastUpdated' => date('c'),
                'sections' => [
                    'overview' => ['title' => 'Overview', 'content' => '', 'lastModified' => date('c')],
                    'architecture' => ['title' => 'Architecture', 'content' => '', 'lastModified' => date('c')],
                    'api' => ['title' => 'API Reference', 'content' => '', 'lastModified' => date('c')],
                    'examples' => ['title' => 'Code Examples', 'content' => '', 'lastModified' => date('c')],
                    'guides' => ['title' => 'Developer Guides', 'content' => '', 'lastModified' => date('c')],
                ],
                'searchIndex' => []
            ];
            file_put_contents($this->filePath, json_encode($defaultData, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES));
        }
    }
    
    public function read() {
        $content = file_get_contents($this->filePath);
        return json_decode($content, true);
    }
    
    public function write($data) {
        $data['lastUpdated'] = date('c');
        $result = file_put_contents($this->filePath, json_encode($data, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES));
        return $result !== false;
    }
    
    public function getSection($sectionId) {
        $data = $this->read();
        return $data['sections'][$sectionId] ?? null;
    }
    
    public function updateSection($sectionId, $content) {
        $data = $this->read();
        if (isset($data['sections'][$sectionId])) {
            $data['sections'][$sectionId]['content'] = $content;
            $data['sections'][$sectionId]['lastModified'] = date('c');
            return $this->write($data);
        }
        return false;
    }
}

class DocsSearchEngine {
    private $database;
    
    public function __construct($database) {
        $this->database = $database;
    }
    
    public function search($query) {
        $data = $this->database->read();
        $results = [];
        $queryLower = strtolower($query);
        
        foreach ($data['sections'] as $id => $section) {
            $titleMatch = stripos($section['title'], $query) !== false;
            $contentMatch = stripos($section['content'], $query) !== false;
            
            if ($titleMatch || $contentMatch) {
                $score = 0;
                if ($titleMatch) $score += 10;
                if ($contentMatch) $score += 5;
                
                // Extract snippet
                $snippet = '';
                if ($contentMatch) {
                    $pos = stripos($section['content'], $query);
                    $start = max(0, $pos - 50);
                    $snippet = substr($section['content'], $start, 150);
                }
                
                $results[] = [
                    'id' => $id,
                    'title' => $section['title'],
                    'snippet' => $snippet ? '...' . $snippet . '...' : '',
                    'score' => $score
                ];
            }
        }
        
        // Sort by score
        usort($results, function($a, $b) {
            return $b['score'] - $a['score'];
        });
        
        return $results;
    }
}

class DocsAPI {
    private $database;
    private $searchEngine;
    private $method;
    private $path;
    
    public function __construct() {
        $this->database = new DocsDatabase(DOCS_DB_FILE);
        $this->searchEngine = new DocsSearchEngine($this->database);
        $this->parseRequest();
    }
    
    private function parseRequest() {
        $this->method = $_SERVER['REQUEST_METHOD'];
        $this->path = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
        $this->path = str_replace('/docs/', '', $this->path);
        $this->path = trim($this->path, '/');
    }
    
    public function handleRequest() {
        // OPTIONS request (CORS preflight)
        if ($this->method === 'OPTIONS') {
            http_response_code(200);
            return;
        }
        
        // Route requests
        $parts = explode('/', $this->path);
        $action = $parts[0] ?? '';
        $param = $parts[1] ?? '';
        
        switch ($action) {
            case 'api':
                $this->handleAPI($param);
                break;
            case 'search':
                $this->handleSearch();
                break;
            case 'section':
                $this->handleSection($param);
                break;
            case 'health':
                $this->handleHealth();
                break;
            case 'export':
                $this->handleExport($param);
                break;
            default:
                $this->error('Unknown action', 404);
        }
    }
    
    private function handleAPI($action) {
        switch ($action) {
            case 'versions':
                $this->response(['version' => DOCS_VERSION, 'timestamp' => date('c')]);
                break;
            case 'sections':
                $data = $this->database->read();
                $sections = [];
                foreach ($data['sections'] as $id => $section) {
                    $sections[$id] = [
                        'title' => $section['title'],
                        'lastModified' => $section['lastModified']
                    ];
                }
                $this->response($sections);
                break;
            default:
                $this->error('Unknown API action', 404);
        }
    }
    
    private function handleSearch() {
        $query = $_GET['q'] ?? '';
        
        if (strlen($query) < 2) {
            $this->error('Query must be at least 2 characters', 400);
            return;
        }
        
        $results = $this->searchEngine->search($query);
        $this->response([
            'query' => $query,
            'count' => count($results),
            'results' => $results
        ]);
    }
    
    private function handleSection($sectionId) {
        if ($this->method === 'GET') {
            $section = $this->database->getSection($sectionId);
            if (!$section) {
                $this->error('Section not found', 404);
                return;
            }
            $this->response($section);
        } elseif ($this->method === 'PUT') {
            $input = json_decode(file_get_contents('php://input'), true);
            if (!isset($input['content'])) {
                $this->error('Content field required', 400);
                return;
            }
            
            if ($this->database->updateSection($sectionId, $input['content'])) {
                $this->response(['success' => true, 'message' => 'Section updated']);
            } else {
                $this->error('Failed to update section', 500);
            }
        }
    }
    
    private function handleHealth() {
        $dbExists = file_exists(DOCS_DB_FILE);
        $cacheDir = is_dir(DOCS_CACHE_DIR);
        
        $this->response([
            'status' => 'healthy',
            'version' => DOCS_VERSION,
            'database' => $dbExists ? 'ok' : 'missing',
            'cache' => $cacheDir ? 'ok' : 'missing',
            'timestamp' => date('c')
        ]);
    }
    
    private function handleExport($format = 'json') {
        $data = $this->database->read();
        
        if ($format === 'html') {
            header('Content-Type: text/html; charset=utf-8');
            $this->exportHTML($data);
        } elseif ($format === 'markdown') {
            header('Content-Type: text/markdown; charset=utf-8');
            $this->exportMarkdown($data);
        } else {
            header('Content-Type: application/json; charset=utf-8');
            $this->response($data);
        }
    }
    
    private function exportHTML($data) {
        echo '<!DOCTYPE html><html><head><meta charset="utf-8"><title>SimpleOS Docs</title></head><body>';
        foreach ($data['sections'] as $id => $section) {
            echo '<h2>' . htmlspecialchars($section['title']) . '</h2>';
            echo '<p>' . nl2br(htmlspecialchars($section['content'])) . '</p>';
        }
        echo '</body></html>';
    }
    
    private function exportMarkdown($data) {
        foreach ($data['sections'] as $id => $section) {
            echo '# ' . $section['title'] . "\n\n";
            echo $section['content'] . "\n\n";
        }
    }
    
    private function response($data) {
        echo json_encode($data, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    }
    
    private function error($message, $code = 400) {
        http_response_code($code);
        echo json_encode(['error' => $message, 'code' => $code]);
    }
}

// ============================================================
// Logger
// ============================================================
class Logger {
    public static function log($message, $level = 'INFO') {
        $timestamp = date('Y-m-d H:i:s');
        $logMessage = "[$timestamp] [$level] $message\n";
        @file_put_contents(DOCS_LOG_FILE, $logMessage, FILE_APPEND);
    }
}

// ============================================================
// Main Execution
// ============================================================
try {
    $api = new DocsAPI();
    $api->handleRequest();
} catch (Exception $e) {
    http_response_code(500);
    Logger::log('Error: ' . $e->getMessage(), 'ERROR');
    echo json_encode(['error' => 'Internal server error']);
}