<?php
/**
 * Google Drive Integration for SimpleOS Documentation
 * 
 * Handles:
 * - Upload documentation to Google Drive
 * - Sync with Google Drive
 * - Share documents
 * - Version control
 * 
 * @version 1.0.0
 * @license MIT
 */

class GoogleDriveIntegration {
    private $clientId;
    private $clientSecret;
    private $accessToken;
    private $refreshToken;
    private $tokenFile;
    
    public function __construct($config = []) {
        $this->clientId = $config['client_id'] ?? $_ENV['GOOGLE_CLIENT_ID'] ?? '';
        $this->clientSecret = $config['client_secret'] ?? $_ENV['GOOGLE_CLIENT_SECRET'] ?? '';
        $this->tokenFile = $config['token_file'] ?? sys_get_temp_dir() . '/google_token.json';
        
        if (file_exists($this->tokenFile)) {
            $this->loadTokens();
        }
    }
    
    /**
     * Get authorization URL
     */
    public function getAuthUrl() {
        $scopes = [
            'https://www.googleapis.com/auth/drive.file',
            'https://www.googleapis.com/auth/drive'
        ];
        
        $params = [
            'client_id' => $this->clientId,
            'redirect_uri' => 'http://localhost:8000/google_callback.php',
            'response_type' => 'code',
            'scope' => implode(' ', $scopes),
            'access_type' => 'offline',
            'prompt' => 'consent'
        ];
        
        return 'https://accounts.google.com/o/oauth2/auth?' . http_build_query($params);
    }
    
    /**
     * Exchange authorization code for access token
     */
    public function handleCallback($code) {
        $params = [
            'client_id' => $this->clientId,
            'client_secret' => $this->clientSecret,
            'code' => $code,
            'grant_type' => 'authorization_code',
            'redirect_uri' => 'http://localhost:8000/google_callback.php'
        ];
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => 'https://oauth2.googleapis.com/token',
            CURLOPT_POST => true,
            CURLOPT_POSTFIELDS => http_build_query($params),
            CURLOPT_RETURNTRANSFER => true
        ]);
        
        $response = curl_exec($ch);
        curl_close($ch);
        
        $data = json_decode($response, true);
        
        if (isset($data['access_token'])) {
            $this->accessToken = $data['access_token'];
            $this->refreshToken = $data['refresh_token'] ?? '';
            $this->saveTokens();
            return true;
        }
        
        return false;
    }
    
    /**
     * Upload documentation file to Google Drive
     */
    public function uploadFile($filePath, $fileName, $mimeType = 'application/json') {
        if (!file_exists($filePath)) {
            throw new Exception("File not found: $filePath");
        }
        
        $fileContent = file_get_contents($filePath);
        
        // Create file metadata
        $fileMetadata = json_encode([
            'name' => $fileName,
            'mimeType' => $mimeType,
            'properties' => [
                'source' => 'simpleos-docs'
            ]
        ]);
        
        // Prepare multipart upload
        $boundary = uniqid();
        $body = "--$boundary\r\n";
        $body .= "Content-Type: application/json; charset=UTF-8\r\n\r\n";
        $body .= $fileMetadata . "\r\n";
        $body .= "--$boundary\r\n";
        $body .= "Content-Type: $mimeType\r\n\r\n";
        $body .= $fileContent . "\r\n";
        $body .= "--$boundary--";
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => 'https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart',
            CURLOPT_POST => true,
            CURLOPT_POSTFIELDS => $body,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken,
                'Content-Type: multipart/related; boundary=' . $boundary
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            return json_decode($response, true);
        }
        
        throw new Exception("Upload failed: $response");
    }
    
    /**
     * List files in Google Drive
     */
    public function listFiles($pageSize = 10) {
        $query = "trashed=false and properties has { key='source' and value='simpleos-docs' }";
        
        $params = [
            'q' => $query,
            'pageSize' => $pageSize,
            'fields' => 'files(id,name,mimeType,createdTime,modifiedTime,size)'
        ];
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => 'https://www.googleapis.com/drive/v3/files?' . http_build_query($params),
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        curl_close($ch);
        
        return json_decode($response, true);
    }
    
    /**
     * Download file from Google Drive
     */
    public function downloadFile($fileId, $destinationPath) {
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://www.googleapis.com/drive/v3/files/$fileId?alt=media",
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $content = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($httpCode === 200) {
            file_put_contents($destinationPath, $content);
            return true;
        }
        
        return false;
    }
    
    /**
     * Share file with specific email
     */
    public function shareFile($fileId, $email, $role = 'reader') {
        $permission = json_encode([
            'type' => 'user',
            'role' => $role,
            'emailAddress' => $email
        ]);
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://www.googleapis.com/drive/v3/files/$fileId/permissions",
            CURLOPT_POST => true,
            CURLOPT_POSTFIELDS => $permission,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken,
                'Content-Type: application/json'
            ]
        ]);
        
        $response = curl_exec($ch);
        $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        return $httpCode === 200;
    }
    
    /**
     * Get file metadata
     */
    public function getFileMetadata($fileId) {
        $fields = 'id,name,mimeType,size,createdTime,modifiedTime,owners,lastModifyingUser';
        
        $ch = curl_init();
        curl_setopt_array($ch, [
            CURLOPT_URL => "https://www.googleapis.com/drive/v3/files/$fileId?fields=$fields",
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_HTTPHEADER => [
                'Authorization: Bearer ' . $this->accessToken
            ]
        ]);
        
        $response = curl_exec($ch);
        curl_close($ch);
        
        return json_decode($response, true);
    }
    
    /**
     * Sync documentation to Google Drive
     */
    public function syncDocs($sourceDir) {
        $results = [
            'uploaded' => [],
            'errors' => [],
            'total' => 0
        ];
        
        if (!is_dir($sourceDir)) {
            $results['errors'][] = "Source directory not found: $sourceDir";
            return $results;
        }
        
        $files = glob($sourceDir . '/*.{json,html,md}', GLOB_BRACE);
        
        foreach ($files as $file) {
            $fileName = basename($file);
            
            try {
                $result = $this->uploadFile($file, $fileName);
                $results['uploaded'][] = [
                    'name' => $fileName,
                    'driveId' => $result['id'] ?? '',
                    'size' => filesize($file)
                ];
                $results['total']++;
            } catch (Exception $e) {
                $results['errors'][] = "$fileName: " . $e->getMessage();
            }
        }
        
        return $results;
    }
    
    /**
     * Publish documentation to Google Sites (if integration enabled)
     */
    public function publishToSites($fileId, $siteId) {
        // This would require additional setup with Google Sites API
        // Placeholder for future implementation
        throw new Exception("Google Sites integration not yet implemented");
    }
    
    /**
     * Save tokens to file
     */
    private function saveTokens() {
        $data = [
            'access_token' => $this->accessToken,
            'refresh_token' => $this->refreshToken,
            'timestamp' => time()
        ];
        
        file_put_contents($this->tokenFile, json_encode($data));
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
}

/**
 * Google Search Integration
 */
class GoogleSearchIntegration {
    private $siteUrl;
    private $searchConsoleApiKey;
    
    public function __construct($siteUrl, $apiKey = '') {
        $this->siteUrl = $siteUrl;
        $this->searchConsoleApiKey = $apiKey ?? $_ENV['GOOGLE_SEARCH_API_KEY'] ?? '';
    }
    
    /**
     * Submit URL to Google for indexing
     */
    public function submitUrl($url) {
        // This requires Google Search Console setup
        // Use Indexing API to submit URLs
        throw new Exception("Google Search Console integration not yet configured");
    }
    
    /**
     * Get search analytics
     */
    public function getAnalytics($days = 30) {
        // Requires Search Console credentials
        throw new Exception("Google Search Console integration not yet configured");
    }
    
    /**
     * Generate sitemap
     */
    public function generateSitemap($baseUrl, $pages = []) {
        $xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        $xml .= "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">\n";
        
        foreach ($pages as $page) {
            $xml .= "  <url>\n";
            $xml .= "    <loc>" . htmlspecialchars($baseUrl . $page) . "</loc>\n";
            $xml .= "    <lastmod>" . date('Y-m-d') . "</lastmod>\n";
            $xml .= "    <priority>0.8</priority>\n";
            $xml .= "  </url>\n";
        }
        
        $xml .= "</urlset>";
        
        return $xml;
    }
}


// ============================================================
// Callback Handler
// ============================================================
if (basename($_SERVER['PHP_SELF']) === 'google_callback.php' && isset($_GET['code'])) {
    $config = [
        'client_id' => $_ENV['GOOGLE_CLIENT_ID'] ?? '',
        'client_secret' => $_ENV['GOOGLE_CLIENT_SECRET'] ?? ''
    ];
    
    $integration = new GoogleDriveIntegration($config);
    
    if ($integration->handleCallback($_GET['code'])) {
        echo "Authorization successful! You can close this window.";
    } else {
        echo "Authorization failed. Please try again.";
    }
}