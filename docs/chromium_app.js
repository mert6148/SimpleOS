/**
 * SimpleOS Chromium App - JavaScript Frontend
 * Google Docs integration ve döküman yönetimi
 * 
 * @version 1.0.0
 * @license MIT
 */

// ============================================================
// Global State
// ============================================================

let appState = {
    currentDoc: null,
    cacheEnabled: true,
    darkMode: localStorage.getItem('darkMode') === 'true',
    apiUrl: 'http://127.0.0.1:8000/api',
    googleConfig: {},
    syncStatus: 'idle',
    recentDocs: JSON.parse(localStorage.getItem('recentDocs') || '[]')
};

// ============================================================
// API Client
// ============================================================

class ChromiumAPIClient {
    constructor(baseUrl) {
        this.baseUrl = baseUrl;
        this.cache = new Map();
        this.cacheExpiry = 3600000; // 1 hour
    }

    async request(endpoint, options = {}) {
        const cacheKey = `${endpoint}_${JSON.stringify(options)}`;
        
        if (this.cache.has(cacheKey)) {
            const { data, timestamp } = this.cache.get(cacheKey);
            if (Date.now() - timestamp < this.cacheExpiry) {
                return data;
            }
        }

        try {
            const response = await fetch(`${this.baseUrl}${endpoint}`, {
                headers: {
                    'Content-Type': 'application/json',
                    ...options.headers
                },
                ...options
            });

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }

            const data = await response.json();
            
            // Cache successful responses
            if (appState.cacheEnabled) {
                this.cache.set(cacheKey, {
                    data,
                    timestamp: Date.now()
                });
            }

            return data;
        } catch (error) {
            console.error(`API Request Error: ${endpoint}`, error);
            throw error;
        }
    }

    async exportGoogleDoc(docId, format = 'html') {
        return this.request(`/google-docs/export?doc_id=${docId}&format=${format}`);
    }

    async listGoogleDocs(folderId) {
        return this.request(`/google-docs/list?folder_id=${folderId}`);
    }

    async batchExportDocs(folderId, format = 'html') {
        return this.request('/google-docs/batch-export', {
            method: 'POST',
            body: JSON.stringify({ folder_id: folderId, format })
        });
    }

    async syncGoogleSheet(sheetId, sheetName = 'Sheet1') {
        return this.request('/google-docs/sheet-sync', {
            method: 'POST',
            body: JSON.stringify({ sheet_id: sheetId, sheet_name: sheetName })
        });
    }

    async searchDocs(query, folderId = null) {
        let url = `/google-docs/search?query=${encodeURIComponent(query)}`;
        if (folderId) {
            url += `&folder_id=${folderId}`;
        }
        return this.request(url);
    }

    async checkDocUpdate(docId) {
        return this.request(`/google-docs/check-update?doc_id=${docId}`);
    }

    async getRevisionHistory(docId, maxResults = 10) {
        return this.request(`/google-docs/history?doc_id=${docId}&max_results=${maxResults}`);
    }

    clearCache() {
        this.cache.clear();
    }
}

const apiClient = new ChromiumAPIClient(appState.apiUrl);

// ============================================================
// UI Components
// ============================================================

class DocumentViewer {
    constructor() {
        this.container = document.getElementById('doc-viewer');
        this.currentContent = null;
    }

    async loadDocument(docId, format = 'html') {
        showLoadingSpinner(true);
        
        try {
            const result = await apiClient.exportGoogleDoc(docId, format);
            
            if (result.success) {
                this.currentContent = result.content;
                this.render(result.content, format);
                
                // Add to recent docs
                addToRecentDocs({ id: docId, title: result.title || 'Untitled' });
                
                showNotification(`Document loaded: ${result.title || 'Untitled'}`, 'success');
            } else {
                showNotification(`Error: ${result.error}`, 'error');
            }
        } catch (error) {
            showNotification(`Failed to load document: ${error.message}`, 'error');
        } finally {
            showLoadingSpinner(false);
        }
    }

    render(content, format = 'html') {
        if (format === 'html') {
            this.container.innerHTML = DOMPurify.sanitize(content);
        } else if (format === 'markdown') {
            const html = marked.parse(content);
            this.container.innerHTML = DOMPurify.sanitize(html);
        } else {
            this.container.textContent = content;
        }

        // Syntax highlighting if code blocks exist
        document.querySelectorAll('pre code').forEach(block => {
            if (hljs) {
                hljs.highlightElement(block);
            }
        });
    }

    export(format = 'html') {
        if (!this.currentContent) {
            showNotification('No document loaded', 'warning');
            return;
        }

        if (window.electron) {
            window.electron.ipcRenderer.invoke('export-docs', {
                content: this.currentContent,
                title: appState.currentDoc?.title || 'document',
                format: format
            }).then(result => {
                if (result.success) {
                    showNotification(`Document exported to ${result.path}`, 'success');
                }
            });
        }
    }

    clear() {
        this.container.innerHTML = '';
        this.currentContent = null;
    }
}

// ============================================================
// Google Integration Panel
// ============================================================

class GoogleIntegrationPanel {
    constructor() {
        this.panel = document.getElementById('google-panel');
        this.setupEventListeners();
    }

    setupEventListeners() {
        document.getElementById('import-btn').addEventListener('click', () => this.openImportDialog());
        document.getElementById('sync-btn').addEventListener('click', () => this.openSyncDialog());
        document.getElementById('search-btn').addEventListener('click', () => this.openSearchDialog());
    }

    openImportDialog() {
        const dialog = createModal('Import from Google Docs', `
            <form id="import-form">
                <div class="form-group">
                    <label>Google Doc ID:</label>
                    <input type="text" id="import-doc-id" placeholder="Paste document ID" required>
                </div>
                <div class="form-group">
                    <label>Format:</label>
                    <select id="import-format">
                        <option value="html">HTML</option>
                        <option value="markdown">Markdown</option>
                        <option value="pdf">PDF</option>
                        <option value="txt">Plain Text</option>
                    </select>
                </div>
                <div class="form-actions">
                    <button type="submit" class="btn btn-primary">Import</button>
                    <button type="button" class="btn btn-secondary" onclick="closeModal()">Cancel</button>
                </div>
            </form>
        `);

        document.getElementById('import-form').addEventListener('submit', (e) => {
            e.preventDefault();
            const docId = document.getElementById('import-doc-id').value;
            const format = document.getElementById('import-format').value;
            
            documentViewer.loadDocument(docId, format);
            closeModal();
        });
    }

    openSyncDialog() {
        const dialog = createModal('Sync from Google Drive', `
            <form id="sync-form">
                <div class="form-group">
                    <label>Folder ID:</label>
                    <input type="text" id="sync-folder-id" placeholder="Paste folder ID" required>
                </div>
                <div class="form-group">
                    <label>Format:</label>
                    <select id="sync-format">
                        <option value="html">HTML</option>
                        <option value="markdown">Markdown</option>
                        <option value="pdf">PDF</option>
                    </select>
                </div>
                <div class="form-actions">
                    <button type="submit" class="btn btn-primary">Sync</button>
                    <button type="button" class="btn btn-secondary" onclick="closeModal()">Cancel</button>
                </div>
            </form>
        `);

        document.getElementById('sync-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            await this.syncFolder(
                document.getElementById('sync-folder-id').value,
                document.getElementById('sync-format').value
            );
            closeModal();
        });
    }

    openSearchDialog() {
        const dialog = createModal('Search Documentation', `
            <form id="search-form">
                <div class="form-group">
                    <label>Search Query:</label>
                    <input type="text" id="search-query" placeholder="Enter search terms" required>
                </div>
                <div class="form-group">
                    <label>Folder ID (optional):</label>
                    <input type="text" id="search-folder-id" placeholder="Leave empty to search all">
                </div>
                <div class="form-actions">
                    <button type="submit" class="btn btn-primary">Search</button>
                    <button type="button" class="btn btn-secondary" onclick="closeModal()">Cancel</button>
                </div>
            </form>
            <div id="search-results"></div>
        `);

        document.getElementById('search-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            await this.performSearch(
                document.getElementById('search-query').value,
                document.getElementById('search-folder-id').value
            );
        });
    }

    async syncFolder(folderId, format) {
        showLoadingSpinner(true);
        appState.syncStatus = 'syncing';
        
        try {
            const result = await apiClient.batchExportDocs(folderId, format);
            
            if (result.success || result.exported) {
                showNotification(
                    `Synced ${result.exported} documents (${result.failed || 0} failed)`,
                    'success'
                );
                
                this.displaySyncResults(result);
            } else {
                showNotification(`Sync failed: ${result.error}`, 'error');
            }
        } catch (error) {
            showNotification(`Sync error: ${error.message}`, 'error');
        } finally {
            showLoadingSpinner(false);
            appState.syncStatus = 'idle';
        }
    }

    async performSearch(query, folderId) {
        showLoadingSpinner(true);
        
        try {
            const result = await apiClient.searchDocs(query, folderId || null);
            
            if (result.success) {
                this.displaySearchResults(result.results || result);
            } else {
                showNotification(`Search failed: ${result.error}`, 'error');
            }
        } catch (error) {
            showNotification(`Search error: ${error.message}`, 'error');
        } finally {
            showLoadingSpinner(false);
        }
    }

    displaySyncResults(results) {
        const resultsHtml = `
            <div class="sync-results">
                <h3>Sync Results</h3>
                <p>Total: ${results.total} | Exported: ${results.exported} | Failed: ${results.failed || 0}</p>
                <ul>
                    ${results.files.map(file => `
                        <li>
                            <strong>${file.name}</strong>
                            <span class="file-size">${formatFileSize(file.size)}</span>
                        </li>
                    `).join('')}
                </ul>
            </div>
        `;
        
        document.getElementById('sync-status').innerHTML = resultsHtml;
    }

    displaySearchResults(results) {
        const resultsHtml = `
            <div class="search-results">
                <h4>Search Results (${results.length})</h4>
                <ul>
                    ${results.map(doc => `
                        <li class="search-result">
                            <h5>${doc.name || doc.title}</h5>
                            <p>${doc.snippet || ''}</p>
                            <button class="btn btn-sm btn-primary" onclick="documentViewer.loadDocument('${doc.id}', 'html')">
                                Load Document
                            </button>
                        </li>
                    `).join('')}
                </ul>
            </div>
        `;
        
        const resultsContainer = document.getElementById('search-results');
        if (resultsContainer) {
            resultsContainer.innerHTML = resultsHtml;
        }
    }
}

// ============================================================
// Document Management
// ============================================================

class DocumentManager {
    constructor() {
        this.documents = [];
        this.setupEventListeners();
    }

    setupEventListeners() {
        document.getElementById('refresh-btn')?.addEventListener('click', () => this.refreshDocuments());
        document.getElementById('clear-cache-btn')?.addEventListener('click', () => this.clearCache());
    }

    async addDocument(docId, title) {
        const doc = {
            id: docId,
            title: title || 'Untitled',
            timestamp: Date.now(),
            lastModified: Date.now()
        };

        this.documents.push(doc);
        this.saveToLocalStorage();
        this.render();

        return doc;
    }

    removeDocument(docId) {
        this.documents = this.documents.filter(d => d.id !== docId);
        this.saveToLocalStorage();
        this.render();
    }

    async refreshDocuments() {
        showLoadingSpinner(true);
        
        try {
            // Check for updates on all stored documents
            const updatePromises = this.documents.map(doc =>
                apiClient.checkDocUpdate(doc.id)
                    .then(result => ({
                        ...doc,
                        hasUpdate: result.has_update,
                        lastUpdate: result.last_update
                    }))
                    .catch(() => doc)
            );

            const updated = await Promise.all(updatePromises);
            this.documents = updated;
            this.saveToLocalStorage();
            this.render();
            
            showNotification('Documents refreshed', 'success');
        } catch (error) {
            showNotification(`Refresh error: ${error.message}`, 'error');
        } finally {
            showLoadingSpinner(false);
        }
    }

    clearCache() {
        apiClient.clearCache();
        localStorage.removeItem('documentCache');
        showNotification('Cache cleared', 'success');
    }

    render() {
        const container = document.getElementById('documents-list');
        if (!container) return;

        const html = `
            <div class="documents-grid">
                ${this.documents.map(doc => `
                    <div class="doc-card" data-doc-id="${doc.id}">
                        <div class="doc-card-header">
                            <h4>${doc.title}</h4>
                            ${doc.hasUpdate ? '<span class="badge badge-info">Updated</span>' : ''}
                        </div>
                        <div class="doc-card-body">
                            <p class="doc-id">${doc.id}</p>
                            <small>Modified: ${new Date(doc.lastModified).toLocaleDateString()}</small>
                        </div>
                        <div class="doc-card-footer">
                            <button class="btn btn-sm btn-primary" onclick="documentViewer.loadDocument('${doc.id}', 'html')">
                                Open
                            </button>
                            <button class="btn btn-sm btn-danger" onclick="documentManager.removeDocument('${doc.id}')">
                                Remove
                            </button>
                        </div>
                    </div>
                `).join('')}
            </div>
        `;

        container.innerHTML = html;
    }

    saveToLocalStorage() {
        localStorage.setItem('documents', JSON.stringify(this.documents));
    }

    loadFromLocalStorage() {
        const stored = localStorage.getItem('documents');
        if (stored) {
            this.documents = JSON.parse(stored);
        }
    }
}

// ============================================================
// Utility Functions
// ============================================================

function showLoadingSpinner(show) {
    const spinner = document.getElementById('loading-spinner');
    if (spinner) {
        spinner.style.display = show ? 'flex' : 'none';
    }
}

function showNotification(message, type = 'info') {
    const container = document.getElementById('notifications');
    if (!container) return;

    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.textContent = message;

    container.appendChild(notification);

    setTimeout(() => {
        notification.classList.add('fade-out');
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

function createModal(title, content) {
    const modal = document.createElement('div');
    modal.className = 'modal active';
    modal.innerHTML = `
        <div class="modal-overlay" onclick="closeModal()"></div>
        <div class="modal-dialog">
            <div class="modal-header">
                <h3>${title}</h3>
                <button class="modal-close" onclick="closeModal()">×</button>
            </div>
            <div class="modal-body">
                ${content}
            </div>
        </div>
    `;

    document.body.appendChild(modal);
    return modal;
}

function closeModal() {
    const modal = document.querySelector('.modal.active');
    if (modal) {
        modal.classList.remove('active');
        setTimeout(() => modal.remove(), 300);
    }
}

function addToRecentDocs(doc) {
    appState.recentDocs = appState.recentDocs.filter(d => d.id !== doc.id);
    appState.recentDocs.unshift(doc);
    appState.recentDocs = appState.recentDocs.slice(0, 10);
    localStorage.setItem('recentDocs', JSON.stringify(appState.recentDocs));
}

function formatFileSize(bytes) {
    const units = ['B', 'KB', 'MB', 'GB'];
    let size = bytes;
    let unitIndex = 0;

    while (size >= 1024 && unitIndex < units.length - 1) {
        size /= 1024;
        unitIndex++;
    }

    return `${size.toFixed(2)} ${units[unitIndex]}`;
}

function toggleDarkMode() {
    appState.darkMode = !appState.darkMode;
    document.documentElement.setAttribute('data-theme', appState.darkMode ? 'dark' : 'light');
    localStorage.setItem('darkMode', appState.darkMode);
}

// ============================================================
// Initialization
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
    // Initialize components
    window.documentViewer = new DocumentViewer();
    window.googlePanel = new GoogleIntegrationPanel();
    window.documentManager = new DocumentManager();

    // Load persisted data
    documentManager.loadFromLocalStorage();
    documentManager.render();

    // Apply theme
    document.documentElement.setAttribute('data-theme', appState.darkMode ? 'dark' : 'light');

    // Setup theme toggle
    document.getElementById('theme-toggle')?.addEventListener('click', toggleDarkMode);

    // Setup export buttons
    document.getElementById('export-html-btn')?.addEventListener('click', () => documentViewer.export('html'));
    document.getElementById('export-markdown-btn')?.addEventListener('click', () => documentViewer.export('markdown'));
    document.getElementById('export-pdf-btn')?.addEventListener('click', () => documentViewer.export('pdf'));
});

// ============================================================
// Electron IPC Integration (if running in Electron)
// ============================================================

if (window.electron) {
    window.electron.ipcRenderer.on('open-import-dialog', () => {
        googlePanel.openImportDialog();
    });

    window.electron.ipcRenderer.on('open-settings', () => {
        // Open settings modal
        createModal('Settings', `
            <form id="settings-form">
                <div class="form-group">
                    <label>API URL:</label>
                    <input type="text" id="api-url" value="${appState.apiUrl}">
                </div>
                <div class="form-group">
                    <label>
                        <input type="checkbox" id="cache-enabled" ${appState.cacheEnabled ? 'checked' : ''}>
                        Enable Caching
                    </label>
                </div>
                <div class="form-actions">
                    <button type="submit" class="btn btn-primary">Save</button>
                    <button type="button" class="btn btn-secondary" onclick="closeModal()">Cancel</button>
                </div>
            </form>
        `);

        document.getElementById('settings-form').addEventListener('submit', (e) => {
            e.preventDefault();
            appState.apiUrl = document.getElementById('api-url').value;
            appState.cacheEnabled = document.getElementById('cache-enabled').checked;
            
            window.electron.ipcRenderer.invoke('update-config', {
                apiUrl: appState.apiUrl,
                cacheEnabled: appState.cacheEnabled
            });

            closeModal();
            showNotification('Settings saved', 'success');
        });
    });

    window.electron.ipcRenderer.on('export-docs', ({ format }) => {
        documentViewer.export(format);
    });
}
