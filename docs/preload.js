/**
 * Electron Preload Script
 * IPC (Inter-Process Communication) için güvenli köprü
 * 
 * @version 1.0.0
 * @license MIT
 */

const { contextBridge, ipcRenderer } = require('electron');

/**
 * API exposed to renderer process
 * Güvenli bir şekilde ana işlemle iletişim kurmak için
 */
contextBridge.exposeInMainWorld('electron', {
    ipcRenderer: {
        /**
         * Import Google Docs
         */
        importGoogleDocs: (docId, format) =>
            ipcRenderer.invoke('import-google-docs', { docId, format }),

        /**
         * Sync Google Drive folder
         */
        syncGoogleDrive: (folderId, format) =>
            ipcRenderer.invoke('sync-google-drive', { folderId, format }),

        /**
         * Sync Google Sheet
         */
        syncGoogleSheet: (sheetId, sheetName) =>
            ipcRenderer.invoke('sync-google-sheet', { sheetId, sheetName }),

        /**
         * Search documentation
         */
        searchDocs: (query, folderId) =>
            ipcRenderer.invoke('search-docs', { query, folderId }),

        /**
         * Export documentation
         */
        exportDocs: (content, title, format) =>
            ipcRenderer.invoke('export-docs', { content, title, format }),

        /**
         * Get application configuration
         */
        getConfig: () =>
            ipcRenderer.invoke('get-config'),

        /**
         * Update application configuration
         */
        updateConfig: (config) =>
            ipcRenderer.invoke('update-config', config),

        /**
         * Check for document updates
         */
        checkDocUpdate: (docId) =>
            ipcRenderer.invoke('check-doc-update', docId),

        /**
         * Get document revision history
         */
        getRevisionHistory: (docId, maxResults) =>
            ipcRenderer.invoke('get-revision-history', { docId, maxResults }),

        /**
         * Listen to main process events
         */
        on: (channel, func) => {
            const validChannels = [
                'open-import-dialog',
                'export-docs',
                'open-settings'
            ];

            if (validChannels.includes(channel)) {
                ipcRenderer.on(channel, (event, ...args) => func(...args));
            }
        },

        /**
         * Remove event listener
         */
        off: (channel, func) => {
            ipcRenderer.removeListener(channel, func);
        },

        /**
         * Send message to main process
         */
        send: (channel, ...args) => {
            const validChannels = ['app-ready', 'window-ready'];

            if (validChannels.includes(channel)) {
                ipcRenderer.send(channel, ...args);
            }
        }
    }
});

/**
 * Application metadata exposed to renderer
 */
contextBridge.exposeInMainWorld('app', {
    name: 'SimpleOS Documentation Viewer',
    version: '1.0.0',
    isElectron: true,
    isDev: process.env.NODE_ENV === 'development',
    platform: process.platform
});

/**
 * Utility functions exposed to renderer
 */
contextBridge.exposeInMainWorld('electronUtils', {
    /**
     * Minimize window
     */
    minimize: () => ipcRenderer.send('minimize'),

    /**
     * Maximize window
     */
    maximize: () => ipcRenderer.send('maximize'),

    /**
     * Close window
     */
    close: () => ipcRenderer.send('close'),

    /**
     * Open external URL in default browser
     */
    openExternal: (url) => ipcRenderer.send('open-external', url),

    /**
     * Show file picker dialog
     */
    showFilePicker: (options) =>
        ipcRenderer.invoke('show-file-picker', options),

    /**
     * Show folder picker dialog
     */
    showFolderPicker: (options) =>
        ipcRenderer.invoke('show-folder-picker', options),

    /**
     * Show save file dialog
     */
    showSaveDialog: (options) =>
        ipcRenderer.invoke('show-save-dialog', options),

    /**
     * Get system info
     */
    getSystemInfo: () => ipcRenderer.invoke('get-system-info'),

    /**
     * Get app paths
     */
    getAppPaths: () => ({
        home: ipcRenderer.sendSync('get-app-path', 'home'),
        documents: ipcRenderer.sendSync('get-app-path', 'documents'),
        appData: ipcRenderer.sendSync('get-app-path', 'appData'),
        temp: ipcRenderer.sendSync('get-app-path', 'temp')
    }),

    /**
     * Log message to console
     */
    log: (message, level = 'info') => {
        ipcRenderer.send('log', { message, level });
    }
});

/**
 * Clipboard operations
 */
contextBridge.exposeInMainWorld('clipboard', {
    /**
     * Read text from clipboard
     */
    readText: () => ipcRenderer.invoke('clipboard-read'),

    /**
     * Write text to clipboard
     */
    writeText: (text) => ipcRenderer.invoke('clipboard-write', text),

    /**
     * Read image from clipboard
     */
    readImage: () => ipcRenderer.invoke('clipboard-read-image'),

    /**
     * Write image to clipboard
     */
    writeImage: (image) => ipcRenderer.invoke('clipboard-write-image', image),

    /**
     * Clear clipboard
     */
    clear: () => ipcRenderer.invoke('clipboard-clear')
});

/**
 * Storage API (secure local storage)
 */
contextBridge.exposeInMainWorld('storage', {
    /**
     * Get value from storage
     */
    getItem: (key) => ipcRenderer.sendSync('storage-get', key),

    /**
     * Set value in storage
     */
    setItem: (key, value) => ipcRenderer.send('storage-set', key, value),

    /**
     * Remove value from storage
     */
    removeItem: (key) => ipcRenderer.send('storage-remove', key),

    /**
     * Clear all storage
     */
    clear: () => ipcRenderer.send('storage-clear'),

    /**
     * Get all keys in storage
     */
    keys: () => ipcRenderer.sendSync('storage-keys')
});

/**
 * Network operations
 */
contextBridge.exposeInMainWorld('network', {
    /**
     * Check if online
     */
    isOnline: () => ipcRenderer.sendSync('network-is-online'),

    /**
     * Get network info
     */
    getNetworkInfo: () => ipcRenderer.invoke('network-info')
});

/**
 * File operations
 */
contextBridge.exposeInMainWorld('files', {
    /**
     * Open file with default application
     */
    open: (filePath) => ipcRenderer.send('file-open', filePath),

    /**
     * Reveal file in explorer/finder
     */
    reveal: (filePath) => ipcRenderer.send('file-reveal', filePath),

    /**
     * Create file
     */
    create: (filePath, content) => ipcRenderer.invoke('file-create', filePath, content),

    /**
     * Read file
     */
    read: (filePath) => ipcRenderer.invoke('file-read', filePath),

    /**
     * Write file
     */
    write: (filePath, content) => ipcRenderer.invoke('file-write', filePath, content),

    /**
     * Delete file
     */
    delete: (filePath) => ipcRenderer.invoke('file-delete', filePath),

    /**
     * Check if file exists
     */
    exists: (filePath) => ipcRenderer.sendSync('file-exists', filePath)
});

/**
 * Notification API
 */
contextBridge.exposeInMainWorld('notify', {
    /**
     * Show notification
     */
    show: (title, options) => ipcRenderer.send('notify-show', title, options),

    /**
     * Show error notification
     */
    error: (title, message) => ipcRenderer.send('notify-error', title, message),

    /**
     * Show success notification
     */
    success: (title, message) => ipcRenderer.send('notify-success', title, message),

    /**
     * Show warning notification
     */
    warning: (title, message) => ipcRenderer.send('notify-warning', title, message),

    /**
     * Show info notification
     */
    info: (title, message) => ipcRenderer.send('notify-info', title, message)
});

/**
 * Window controls
 */
contextBridge.exposeInMainWorld('window', {
    /**
     * Set window always on top
     */
    setAlwaysOnTop: (flag) => ipcRenderer.send('window-always-on-top', flag),

    /**
     * Fullscreen toggle
     */
    toggleFullscreen: () => ipcRenderer.send('window-fullscreen'),

    /**
     * Get window bounds
     */
    getBounds: () => ipcRenderer.sendSync('window-get-bounds'),

    /**
     * Set window size
     */
    setSize: (width, height) => ipcRenderer.send('window-set-size', width, height),

    /**
     * Center window on screen
     */
    center: () => ipcRenderer.send('window-center'),

    /**
     * Focus window
     */
    focus: () => ipcRenderer.send('window-focus'),

    /**
     * Blur window
     */
    blur: () => ipcRenderer.send('window-blur')
});

console.log('✓ Preload script loaded successfully');
console.log('Available APIs: electron, app, electronUtils, clipboard, storage, network, files, notify, window');
