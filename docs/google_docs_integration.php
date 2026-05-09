<?php
/**
 * Google Docs Integration for SimpleOS Documentation
 * 
 * Google Docs, Sheets, Slides API entegrasyonu
 * Dökümantasyonları Google'dan senkronize etme ve yönetme
 * 
 * @version 1.0.0
 * @license MIT
 */

class GoogleDocsIntegration {
    private $accessToken;
    private $refreshToken;
    private $clientId;
    private $clientSecret;
    private $tokenFile;
    private $cacheDir;
    
    public function __construct($config = []) {
        $this->clientId = $config['client_id'] ?? $_ENV['GOOGLE_CLIENT_ID'] ?? '';
        $this->clientSecret = $config['client_secret'] ?? $_ENV['GOOGLE_CLIENT_SECRET'] ?? '';
        $this->tokenFile = $config['token_file'] ?? sys_get_temp_dir() . '/google_docs_token.json';
        $this->cacheDir = $config['cache_dir'] ?? __DIR__ . '/cache';
        
        @mkdir($this->cacheDir, 0755, true);
        
        if (file_exists($this->tokenFile)) {
            $this->loadTokens();
        }
    }
    
    /**
     * Export Google Docs to HTML/Markdown
     */
    public function exportGoogleDoc($docId, $format = 'html') {
        $mimeTypes = [
            'html' => 'text/html',
            'markdown' => 'text/markdown',
            'pdf' => 'application/pdf',
            'txt' => 'text/plain',
            'json' => 'application/json'
        ];
        
        $mimeType = $mimeTypes[$format] ?? 'text/html';
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://docs.googleapis.com/v1/documents/{$docId}/export?mimeType=" . urlencode($mimeType),
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            return [
                'success' => true,
                'format' => $format,
                'content' => $response,
                'size' => strlen($response)
            ];
        }
        
        return [
            'success' => false,
            'error' => "Export failed with HTTP {$httpCode}"
        ];
    }
    
    /**
     * Get Google Doc metadata
     */
    public function getDocMetadata($docId) {
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://docs.googleapis.com/v1/documents/{$docId}",
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            return [
                'success' => true,
                'title' => $data['title'] ?? 'Untitled',
                'doc_id' => $docId,
                'body' => isset($data['body']['content']) ? count($data['body']['content']) : 0,
                'created_time' => $data['createdTime'] ?? '',
                'updated_time' => $data['modifiedTime'] ?? ''
            ];
        }
        
        return ['success' => false, 'error' => "Metadata request failed"];
    }
    
    /**
     * List all Google Docs in a folder
     */
    public function listDocsInFolder($folderId) {
        $query = "'{$folderId}' in parents and trashed=false and mimeType='application/vnd.google-apps.document'";
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => 'https://www.googleapis.com/drive/v3/files?' . http_build_query([
                'q' => $query,
                'fields' => 'files(id,name,createdTime,modifiedTime,mimeType)'
            ]),
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            return [
                'success' => true,
                'count' => count($data['files'] ?? []),
                'docs' => $data['files'] ?? []
            ];
        }
        
        return ['success' => false, 'error' => 'Failed to list documents'];
    }
    
    /**
     * Batch export Google Docs from folder
     */
    public function batchExportDocs($folderId, $format = 'html') {
        $docs = $this->listDocsInFolder($folderId);
        
        if (!$docs['success']) {
            return $docs;
        }
        
        $results = [
            'total' => count($docs['docs']),
            'exported' => 0,
            'failed' => 0,
            'files' => []
        ];
        
        foreach ($docs['docs'] as $doc) {
            $export = $this->exportGoogleDoc($doc['id'], $format);
            
            if ($export['success']) {
                $filename = $this->sanitizeFilename($doc['name']) . '.' . $format;
                $filepath = $this->cacheDir . '/' . $filename;
                
                file_put_contents($filepath, $export['content']);
                
                $results['exported']++;
                $results['files'][] = [
                    'name' => $doc['name'],
                    'file' => $filename,
                    'size' => $export['size']
                ];
            } else {
                $results['failed']++;
            }
        }
        
        return $results;
    }
    
    /**
     * Sync Google Sheet data to JSON database
     */
    public function syncGoogleSheet($sheetId, $sheetName = 'Sheet1', $outputFile = null) {
        $range = urlencode($sheetName . '!A1:Z1000');
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://sheets.googleapis.com/v4/spreadsheets/{$sheetId}/values/{$range}",
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            $values = $data['values'] ?? [];
            
            // Convert to associative array if headers exist
            $result = [];
            if (count($values) > 0) {
                $headers = $values[0];
                for ($i = 1; $i < count($values); $i++) {
                    $row = [];
                    foreach ($headers as $j => $header) {
                        $row[$header] = $values[$i][$j] ?? '';
                    }
                    $result[] = $row;
                }
            }
            
            // Save to cache if output file specified
            if ($outputFile) {
                file_put_contents($outputFile, json_encode($result, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE));
            }
            
            return [
                'success' => true,
                'rows' => count($result),
                'data' => $result,
                'file' => $outputFile
            ];
        }
        
        return ['success' => false, 'error' => 'Failed to sync sheet'];
    }
    
    /**
     * Get Google Slides info
     */
    public function getSlidesInfo($presentationId) {
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://slides.googleapis.com/v1/presentations/{$presentationId}",
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            return [
                'success' => true,
                'title' => $data['name'] ?? 'Untitled',
                'slides' => count($data['slides'] ?? []),
                'locale' => $data['locale'] ?? 'en_US',
                'revision_id' => $data['revisionId'] ?? ''
            ];
        }
        
        return ['success' => false, 'error' => 'Failed to get slides info'];
    }
    
    /**
     * Export Slides to images
     */
    public function exportSlidesToImages($presentationId, $imageFormat = 'png') {
        $formats = ['png' => 'PNG', 'jpeg' => 'JPG', 'svg' => 'SVG'];
        $mimeType = $formats[$imageFormat] ?? 'PNG';
        
        $slides = $this->getSlidesInfo($presentationId);
        
        if (!$slides['success']) {
            return $slides;
        }
        
        $results = [
            'presentation' => $slides['title'],
            'total_slides' => $slides['slides'],
            'exported' => 0,
            'images' => []
        ];
        
        for ($i = 0; $i < $slides['slides']; $i++) {
            $pageId = $i + 1;
            
            $ch = curl_init();
            curl_setopt_array($ch, [
                CURLOPT_URL => "https://slides.googleapis.com/v1/presentations/{$presentationId}/pages/{$pageId}/thumbnail",
                CURLOPT_RETURNTRANSFER => true,
                CURLOPT_HTTPHEADER => [
                    'Authorization: Bearer ' . $this->accessToken
                ]
            ]);
            
            $response = curl_exec($ch);
            $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
            curl_close($ch);
            
            if ($httpCode === 200) {
                $data = json_decode($response, true);
                if (isset($data['contentUrl'])) {
                    $results['images'][] = [
                        'slide' => $pageId,
                        'url' => $data['contentUrl']
                    ];
                    $results['exported']++;
                }
            }
        }
        
        return $results;
    }
    
    /**
     * Create real-time listener for document changes (Webhooks)
     */
    public function setupDocumentWebhook($docId, $webhookUrl) {
        // Google Docs doesn't have direct webhooks like Drive
        // This is a helper to set up polling-based updates
        
        $cacheKey = "doc_version_{$docId}";
        $cacheFile = $this->cacheDir . '/.versions';
        
        $versions = [];
        if (file_exists($cacheFile)) {
            $versions = json_decode(file_get_contents($cacheFile), true) ?? [];
        }
        
        $metadata = $this->getDocMetadata($docId);
        
        if ($metadata['success']) {
            $versions[$docId] = [
                'updated_time' => $metadata['updated_time'],
                'webhook_url' => $webhookUrl,
                'last_checked' => date('c')
            ];
            
            file_put_contents($cacheFile, json_encode($versions, JSON_PRETTY_PRINT));
            
            return [
                'success' => true,
                'message' => 'Webhook setup completed',
                'doc_id' => $docId,
                'webhook_url' => $webhookUrl
            ];
        }
        
        return ['success' => false, 'error' => 'Failed to setup webhook'];
    }
    
    /**
     * Check for document updates (polling)
     */
    public function checkDocumentUpdates($docId) {
        $metadata = $this->getDocMetadata($docId);
        
        if (!$metadata['success']) {
            return $metadata;
        }
        
        $cacheFile = $this->cacheDir . "/.doc_last_update_{$docId}";
        $lastUpdate = file_exists($cacheFile) ? file_get_contents($cacheFile) : null;
        
        $hasUpdate = $lastUpdate !== $metadata['updated_time'];
        
        if ($hasUpdate) {
            file_put_contents($cacheFile, $metadata['updated_time']);
        }
        
        return [
            'success' => true,
            'doc_id' => $docId,
            'has_update' => $hasUpdate,
            'last_update' => $metadata['updated_time'],
            'previous_update' => $lastUpdate
        ];
    }
    
    /**
     * Get document revision history
     */
    public function getRevisionHistory($docId, $maxResults = 10) {
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://www.googleapis.com/drive/v3/files/{$docId}/revisions?" . http_build_query([
                'pageSize' => $maxResults,
                'fields' => 'revisions(id,modifiedTime,lastModifyingUser,size)'
            ]),
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            return [
                'success' => true,
                'doc_id' => $docId,
                'revisions' => $data['revisions'] ?? [],
                'count' => count($data['revisions'] ?? [])
            ];
        }
        
        return ['success' => false, 'error' => 'Failed to get revision history'];
    }
    
    /**
     * Search in Google Docs content
     */
    public function searchInDocs($query, $folderId = null) {
        $queryStr = "fullText contains '{$query}' and mimeType='application/vnd.google-apps.document'";
        
        if ($folderId) {
            $queryStr .= " and '{$folderId}' in parents";
        }
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => 'https://www.googleapis.com/drive/v3/files?' . http_build_query([
                'q' => $queryStr,
                'fields' => 'files(id,name,modifiedTime)',
                'pageSize' => 20
            ]),
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            $data = json_decode($response, true);
            return [
                'success' => true,
                'query' => $query,
                'results' => $data['files'] ?? [],
                'count' => count($data['files'] ?? [])
            ];
        }
        
        return ['success' => false, 'error' => 'Search failed'];
    }
    
    /**
     * Load tokens from file
     */
    private function loadTokens() {
        if (file_exists($this->tokenFile)) {
            $data = json_decode(file_get_contents($this->tokenFile), true);
            $this->accessToken = $data['access_token'] ?? '';
            $this->refreshToken = $data['refresh_token'] ?? '';
        }
    }
    
    /**
     * Sanitize filename
     */
    private function sanitizeFilename($filename) {
        return preg_replace('/[^a-zA-Z0-9_-]/', '_', $filename);
    }
}

/**
 * Google Docs API Handler (REST endpoint)
 */
class GoogleDocsAPIHandler {
    private $integration;
    
    public function __construct($config = []) {
        $this->integration = new GoogleDocsIntegration($config);
    }
    
    public function handleRequest($action, $params = []) {
        $method = $_SERVER['REQUEST_METHOD'];
        
        switch ($action) {
            case 'export':
                return $this->integration->exportGoogleDoc(
                    $params['doc_id'],
                    $params['format'] ?? 'html'
                );
                
            case 'metadata':
                return $this->integration->getDocMetadata($params['doc_id']);
                
            case 'list':
                return $this->integration->listDocsInFolder($params['folder_id']);
                
            case 'batch-export':
                return $this->integration->batchExportDocs(
                    $params['folder_id'],
                    $params['format'] ?? 'html'
                );
                
            case 'sheet-sync':
                return $this->integration->syncGoogleSheet(
                    $params['sheet_id'],
                    $params['sheet_name'] ?? 'Sheet1',
                    $params['output_file'] ?? null
                );
                
            case 'slides-info':
                return $this->integration->getSlidesInfo($params['presentation_id']);
                
            case 'slides-export':
                return $this->integration->exportSlidesToImages(
                    $params['presentation_id'],
                    $params['format'] ?? 'png'
                );
                
            case 'check-update':
                return $this->integration->checkDocumentUpdates($params['doc_id']);
                
            case 'history':
                return $this->integration->getRevisionHistory(
                    $params['doc_id'],
                    $params['max_results'] ?? 10
                );
                
            case 'search':
                return $this->integration->searchInDocs(
                    $params['query'],
                    $params['folder_id'] ?? null
                );
                
            default:
                return ['error' => 'Unknown action'];
        }
    }
}
