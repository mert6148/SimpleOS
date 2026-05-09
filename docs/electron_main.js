/**
 * SimpleOS Documentation Viewer - Electron Main Process
 * Chromium tabanlı masaüstü uygulaması
 * 
 * @version 1.0.0
 * @license MIT
 */

const { app, BrowserWindow, Menu, ipcMain, dialog } = require('electron');
const path = require('path');
const isDev = require('electron-is-dev');
const { spawn } = require('child_process');
const fs = require('fs');

let mainWindow;
let phpServer;
let apiClient;

/**
 * Create Application Window
 */
function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1400,
        height: 900,
        minWidth: 800,
        minHeight: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            nodeIntegration: false,
            contextIsolation: true,
            enableRemoteModule: false
        },
        icon: path.join(__dirname, 'assets/icon.png')
    });

    const startUrl = isDev
        ? 'http://localhost:3000'
        : `file://${path.join(__dirname, '../build/index.html')}`;

    mainWindow.loadURL(startUrl);

    if (isDev) {
        mainWindow.webContents.openDevTools();
    }

    mainWindow.on('closed', () => {
        mainWindow = null;
    });

    createMenu();
}

/**
 * Create Application Menu
 */
function createMenu() {
    const template = [
        {
            label: 'File',
            submenu: [
                {
                    label: 'Import Documentation',
                    accelerator: 'CmdOrCtrl+I',
                    click: () => handleImportDocs()
                },
                {
                    label: 'Export as HTML',
                    accelerator: 'CmdOrCtrl+E',
                    click: () => handleExportDocs('html')
                },
                {
                    label: 'Export as PDF',
                    click: () => handleExportDocs('pdf')
                },
                { type: 'separator' },
                {
                    label: 'Settings',
                    accelerator: 'CmdOrCtrl+,',
                    click: () => openSettings()
                },
                { type: 'separator' },
                {
                    label: 'Exit',
                    accelerator: 'CmdOrCtrl+Q',
                    click: () => {
                        app.quit();
                    }
                }
            ]
        },
        {
            label: 'Edit',
            submenu: [
                { role: 'undo' },
                { role: 'redo' },
                { type: 'separator' },
                { role: 'cut' },
                { role: 'copy' },
                { role: 'paste' }
            ]
        },
        {
            label: 'View',
            submenu: [
                { role: 'reload' },
                { role: 'forceReload' },
                { role: 'toggleDevTools' },
                { type: 'separator' },
                { role: 'resetZoom' },
                { role: 'zoomIn' },
                { role: 'zoomOut' },
                { type: 'separator' },
                { role: 'togglefullscreen' }
            ]
        },
        {
            label: 'Help',
            submenu: [
                {
                    label: 'About',
                    click: () => openAbout()
                },
                {
                    label: 'Documentation',
                    click: () => {
                        require('electron').shell.openExternal('https://simpleos-docs.example.com');
                    }
                }
            ]
        }
    ];

    const menu = Menu.buildFromTemplate(template);
    Menu.setApplicationMenu(menu);
}

/**
 * Start PHP Server
 */
function startPHPServer() {
    const phpPath = process.env.PHP_PATH || 'php';
    const docsPath = path.join(__dirname, '../docs');
    
    console.log('[PHP Server] Starting on port 8000...');
    
    phpServer = spawn(phpPath, ['-S', '127.0.0.1:8000'], {
        cwd: docsPath,
        stdio: ['ignore', 'pipe', 'pipe']
    });

    phpServer.stdout.on('data', (data) => {
        console.log(`[PHP] ${data}`);
    });

    phpServer.stderr.on('data', (data) => {
        console.error(`[PHP Error] ${data}`);
    });

    phpServer.on('close', (code) => {
        console.log(`[PHP Server] Exited with code ${code}`);
    });
}

/**
 * Stop PHP Server
 */
function stopPHPServer() {
    if (phpServer) {
        console.log('[PHP Server] Stopping...');
        phpServer.kill();
    }
}

/**
 * App Initialization
 */
app.on('ready', () => {
    startPHPServer();
    setTimeout(() => {
        createWindow();
    }, 1000);
});

app.on('window-all-closed', () => {
    stopPHPServer();
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

app.on('activate', () => {
    if (mainWindow === null) {
        createWindow();
    }
});

/**
 * IPC Handlers
 */

/**
 * Import documentation from Google Docs
 */
ipcMain.handle('import-google-docs', async (event, { docId, format }) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/export?doc_id=${docId}&format=${format}`);
        const data = await response.json();
        
        if (data.success) {
            return {
                success: true,
                content: data.content,
                title: data.title || 'Imported Document'
            };
        }
        
        return { success: false, error: data.error };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Sync Google Drive folder
 */
ipcMain.handle('sync-google-drive', async (event, { folderId, format }) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/batch-export`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ folder_id: folderId, format: format || 'html' })
        });
        
        const data = await response.json();
        return data;
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Sync Google Sheet
 */
ipcMain.handle('sync-google-sheet', async (event, { sheetId, sheetName }) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/sheet-sync`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                sheet_id: sheetId,
                sheet_name: sheetName || 'Sheet1'
            })
        });
        
        const data = await response.json();
        return data;
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Search in docs
 */
ipcMain.handle('search-docs', async (event, { query, folderId }) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/search?query=${encodeURIComponent(query)}&folder_id=${folderId || ''}`);
        const data = await response.json();
        return data;
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Export documentation
 */
ipcMain.handle('export-docs', async (event, { content, title, format }) => {
    try {
        const defaultPath = path.join(app.getPath('documents'), `${title}.${format}`);
        
        const { filePath, canceled } = await dialog.showSaveDialog(mainWindow, {
            defaultPath: defaultPath,
            filters: [
                { name: format.toUpperCase() + ' Files', extensions: [format] },
                { name: 'All Files', extensions: ['*'] }
            ]
        });

        if (!canceled && filePath) {
            fs.writeFileSync(filePath, content);
            return { success: true, path: filePath };
        }

        return { success: false, message: 'Export canceled' };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Get app configuration
 */
ipcMain.handle('get-config', async () => {
    try {
        const configPath = path.join(__dirname, '../electron_config.json');
        if (fs.existsSync(configPath)) {
            const config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
            return { success: true, config };
        }
        return { success: false, error: 'Config not found' };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Update app configuration
 */
ipcMain.handle('update-config', async (event, newConfig) => {
    try {
        const configPath = path.join(__dirname, '../electron_config.json');
        fs.writeFileSync(configPath, JSON.stringify(newConfig, null, 2));
        return { success: true, message: 'Configuration updated' };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Check document updates
 */
ipcMain.handle('check-doc-update', async (event, docId) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/check-update?doc_id=${docId}`);
        const data = await response.json();
        return data;
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Get revision history
 */
ipcMain.handle('get-revision-history', async (event, { docId, maxResults }) => {
    try {
        const response = await fetch(`http://127.0.0.1:8000/api/google-docs/history?doc_id=${docId}&max_results=${maxResults || 10}`);
        const data = await response.json();
        return data;
    } catch (error) {
        return { success: false, error: error.message };
    }
});

/**
 * Helper Functions
 */

function handleImportDocs() {
    if (mainWindow) {
        mainWindow.webContents.send('open-import-dialog');
    }
}

function handleExportDocs(format) {
    if (mainWindow) {
        mainWindow.webContents.send('export-docs', { format });
    }
}

function openSettings() {
    if (mainWindow) {
        mainWindow.webContents.send('open-settings');
    }
}

function openAbout() {
    dialog.showMessageBox(mainWindow, {
        type: 'info',
        title: 'About SimpleOS Documentation Viewer',
        message: 'SimpleOS Documentation Viewer',
        detail: 'Version 1.0.0\n\nA modern documentation viewer with Google Docs integration\n\nBuilt with Electron and Chromium'
    });
}

module.exports = {
    mainWindow,
    createWindow,
    stopPHPServer
};
