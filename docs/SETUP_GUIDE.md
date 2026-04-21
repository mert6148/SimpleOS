# SimpleOS Documentation Services - Setup & Usage Guide

## Overview

SimpleOS Documentation Services provides:
- **PHP Backend API** for managing and serving documentation
- **Windows Desktop Application** for offline documentation access
- **Google Drive Integration** for cloud storage and sharing
- **Full-text Search** with intelligent ranking
- **Multi-format Export** (JSON, HTML, Markdown)

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│         Web Browser / Desktop Application               │
│                                                          │
└────────────────────┬────────────────────────────────────┘
                     │ HTTP/REST API
┌────────────────────▼────────────────────────────────────┐
│  PHP Backend (docsservices.php)                         │
│  ├─ Documentation API                                   │
│  ├─ Search Engine                                       │
│  ├─ Export Services                                     │
│  └─ Google Drive Sync                                   │
└────────────────────┬────────────────────────────────────┘
                     │
        ┌────────────┼────────────┐
        │            │            │
    ┌───▼──┐   ┌────▼──┐   ┌────▼────┐
    │ JSON │   │Cache  │   │  Logs   │
    │  DB  │   │Files  │   │ Files   │
    └──────┘   └───────┘   └─────────┘
        │
    ┌───▼──────────────────────────┐
    │  Google Drive Cloud Storage  │
    └──────────────────────────────┘
```

## Installation

### Prerequisites

- **PHP 7.4+** with curl extension
- **Python 3.7+** (for Windows app)
- **Node.js** (optional, for build tools)

### PHP Backend Setup

1. **Copy files to web server:**
   ```bash
   cp docsservices.php /var/www/html/docs/
   cp config.json /var/www/html/docs/
   cp google_integration.php /var/www/html/docs/
   ```

2. **Create necessary directories:**
   ```bash
   mkdir -p /var/www/html/docs/cache
   mkdir -p /var/www/html/docs/logs
   mkdir -p /var/www/html/docs/backups
   chmod 755 cache logs backups
   ```

3. **Initialize database:**
   - docs.json will be created automatically on first request
   - Or manually: `touch /var/www/html/docs/docs.json`

4. **Test API health:**
   ```bash
   curl http://localhost/docs/docsservices.php?api/health
   ```

### Windows Desktop Application Setup

1. **Install Python dependencies:**
   ```bash
   pip install PyQt5 PyQtWebEngine requests
   ```

2. **Run the application:**
   ```bash
   python simpleos_docs_viewer.py
   ```

3. **Build as Windows executable (optional):**
   ```bash
   pip install pyinstaller
   pyinstaller --onefile --windowed simpleos_docs_viewer.py
   ```

4. **Create Windows installer (optional):**
   - Use InnoSetup or NSIS to create .exe installer
   - Include PyQt5 dependencies
   - Create start menu shortcuts

### Google Drive Integration Setup

1. **Create Google Cloud Project:**
   - Go to https://console.cloud.google.com/
   - Create new project "SimpleOS Docs"
   - Enable Google Drive API
   - Create OAuth 2.0 credentials (Desktop application)

2. **Set environment variables:**
   ```bash
   export GOOGLE_CLIENT_ID="your-client-id.apps.googleusercontent.com"
   export GOOGLE_CLIENT_SECRET="your-client-secret"
   export GOOGLE_SEARCH_API_KEY="your-search-api-key"
   ```

3. **Test Google integration:**
   ```bash
   curl "http://localhost/docs/google_integration.php?action=listFiles"
   ```

## API Reference

### Base URL
```
http://localhost/docs/docsservices.php
```

### Endpoints

#### Get Documentation Version
```bash
GET /api/versions
Response: { "version": "1.0.0", "timestamp": "2026-04-21T10:00:00Z" }
```

#### Get All Sections
```bash
GET /api/sections
Response: {
  "overview": { "title": "Overview", "lastModified": "2026-04-21T10:00:00Z" },
  "architecture": { ... },
  ...
}
```

#### Get Section Content
```bash
GET /section/{sectionId}
Response: {
  "title": "Overview",
  "content": "...",
  "lastModified": "2026-04-21T10:00:00Z"
}
```

#### Update Section
```bash
PUT /section/{sectionId}
Content-Type: application/json
Body: { "content": "New content here..." }
Response: { "success": true, "message": "Section updated" }
```

#### Search Documentation
```bash
GET /search?q={query}
Response: {
  "query": "memory",
  "count": 5,
  "results": [
    {
      "id": "architecture",
      "title": "Architecture",
      "snippet": "...memory management using buddy allocator...",
      "score": 15
    },
    ...
  ]
}
```

#### Health Check
```bash
GET /api/health
Response: {
  "status": "healthy",
  "version": "1.0.0",
  "database": "ok",
  "cache": "ok",
  "timestamp": "2026-04-21T10:00:00Z"
}
```

#### Export Documentation
```bash
# JSON format
GET /export/json

# HTML format
GET /export/html

# Markdown format
GET /export/markdown
```

## Usage Examples

### PHP Backend Usage

```php
<?php
// Include services
require 'docsservices.php';

// Create API instance
$api = new DocsAPI();
$api->handleRequest();
```

### Windows Desktop App Usage

1. **Start the application:**
   ```bash
   python simpleos_docs_viewer.py
   ```

2. **Features:**
   - Browse sections in left sidebar
   - Search using top search bar
   - Toggle dark/light theme
   - View offline status
   - Export documentation

3. **Keyboard shortcuts:**
   - `Ctrl+T`: Toggle theme
   - `Enter` in search: Perform search

### Google Drive Integration Usage

```php
<?php
require 'google_integration.php';

// Initialize Google Drive integration
$google = new GoogleDriveIntegration([
    'client_id' => 'your-client-id',
    'client_secret' => 'your-client-secret'
]);

// Upload file
$result = $google->uploadFile('/path/to/docs.json', 'docs.json');
echo "Uploaded with ID: " . $result['id'];

// List files
$files = $google->listFiles();
foreach ($files['files'] as $file) {
    echo $file['name'] . " - " . $file['size'] . " bytes\n";
}

// Sync all documentation
$syncResult = $google->syncDocs('./docs_folder');
echo "Uploaded " . $syncResult['total'] . " files\n";
```

## Configuration

Edit `config.json` to customize:

```json
{
  "api": {
    "base_url": "http://localhost:8000/docs/docsservices.php",
    "timeout": 10
  },
  "google_integration": {
    "enabled": true,
    "client_id": "your-client-id",
    "client_secret": "your-client-secret"
  },
  "windows_app": {
    "theme": "light",
    "font_size": 11,
    "offline_sync": true
  }
}
```

## Development

### Adding New Sections

1. Edit `docs.json` or use API:
   ```bash
   curl -X PUT http://localhost/docs/docsservices.php/section/newsection \
     -H "Content-Type: application/json" \
     -d '{"content": "Section content..."}'
   ```

2. Sections auto-appear in Windows app

### Extending Search

Modify `DocsSearchEngine` class in `docsservices.php`:
- Add custom ranking algorithms
- Support filters and facets
- Implement synonym detection

### Custom Export Formats

Add method to `DocsAPI::handleExport()`:
```php
private function exportPDF($data) {
    // Implement PDF export
}
```

## Troubleshooting

### Windows App Won't Connect

1. Check PHP API is running:
   ```bash
   curl http://localhost/docs/docsservices.php/api/health
   ```

2. Update API URL in app if running on different machine:
   - Edit CONFIG in `simpleos_docs_viewer.py`
   - Rebuild with `pyinstaller`

### Google Integration Not Working

1. Check credentials in environment variables
2. Verify OAuth callback URL matches in Google Console
3. Check API quota limits in Google Cloud Console

### Search Not Finding Results

1. Ensure content is properly formatted in JSON
2. Check minimum query length (default: 2 characters)
3. Rebuild search index if needed

## Performance Optimization

1. **Enable caching:**
   ```json
   "cache": { "enabled": true, "ttl": 3600 }
   ```

2. **Use CDN for static files:**
   - Serve docs.html through CDN
   - Cache JSON responses

3. **Database optimization:**
   - Regularly backup docs.json
   - Consider SQLite for large installations

## Security Considerations

1. **API Authentication:** Set in config.json
   ```json
   "security": {
     "require_auth": true,
     "api_key": "your-secret-key"
   }
   ```

2. **HTTPS:** Always use HTTPS in production

3. **CORS:** Configure allowed origins in config.json

4. **Rate Limiting:** Implement in API endpoints for production

## Support & Resources

- **Documentation:** See ../README.md and ../ARCHITECTURE.md
- **Issues:** Check logs in docs/logs/ directory
- **Contact:** mertdogan00ay437@gmail.com

## License

MIT License - See LICENSE file

## Version History

- **v1.0.0** (2026-04-21): Initial release
  - PHP backend API
  - Windows desktop app
  - Google Drive integration
  - Search functionality
  - Multi-format export
