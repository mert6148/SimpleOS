/**
 * FileSystemBrowser Component
 * Displays file system structure with inode information
 */

import React, { useState, useMemo } from 'react';

export function FileSystemBrowser({ files = [] }) {
    const [expandedDirs, setExpandedDirs] = useState(new Set(['/', '/sys', '/proc']));
    const [selectedFile, setSelectedFile] = useState(null);

    const defaultFiles = [
        { path: '/', name: 'root', type: 'directory', size: 0, inode: 2, permissions: '0755' },
        { path: '/bin', name: 'bin', type: 'directory', size: 0, inode: 256, permissions: '0755' },
        { path: '/bin/sh', name: 'sh', type: 'file', size: 4096, inode: 257, permissions: '0755' },
        { path: '/bin/ls', name: 'ls', type: 'file', size: 8192, inode: 258, permissions: '0755' },
        { path: '/sys', name: 'sys', type: 'directory', size: 0, inode: 512, permissions: '0755' },
        { path: '/sys/kernel', name: 'kernel', type: 'directory', size: 0, inode: 513, permissions: '0555' },
        { path: '/sys/kernel/version', name: 'version', type: 'file', size: 128, inode: 514, permissions: '0444' },
        { path: '/proc', name: 'proc', type: 'directory', size: 0, inode: 1024, permissions: '0555' },
        { path: '/proc/cpuinfo', name: 'cpuinfo', type: 'file', size: 256, inode: 1025, permissions: '0444' },
        { path: '/proc/meminfo', name: 'meminfo', type: 'file', size: 512, inode: 1026, permissions: '0444' },
    ];

    const fileList = files.length > 0 ? files : defaultFiles;

    const toggleDir = (path) => {
        const newExpanded = new Set(expandedDirs);
        if (newExpanded.has(path)) {
            newExpanded.delete(path);
        } else {
            newExpanded.add(path);
        }
        setExpandedDirs(newExpanded);
    };

    const getDepth = (path) => {
        return path.split('/').length - 2;
    };

    const getParentPath = (path) => {
        return path.substring(0, path.lastIndexOf('/')) || '/';
    };

    const getChildFiles = (parentPath) => {
        return fileList.filter(file => {
            const parent = getParentPath(file.path);
            return parent === parentPath && file.path !== parentPath;
        });
    };

    const sortedFiles = useMemo(() => {
        return [...fileList].sort((a, b) => {
            if (a.type !== b.type) {
                return a.type === 'directory' ? -1 : 1;
            }
            return a.name.localeCompare(b.name);
        });
    }, [fileList]);

    const renderFileTree = (parentPath = '/', depth = 0) => {
        const children = getChildFiles(parentPath);

        return children.map((file) => {
            const isDir = file.type === 'directory';
            const isExpanded = expandedDirs.has(file.path);
            const hasChildren = isDir && getChildFiles(file.path).length > 0;

            return (
                <div key={file.path}>
                    <div
                        className="file-item"
                        style={{ paddingLeft: depth * 20 + 'px' }}
                        onClick={() => setSelectedFile(file)}
                    >
                        {isDir && (
                            <button
                                className="expand-btn"
                                onClick={(e) => {
                                    e.stopPropagation();
                                    toggleDir(file.path);
                                }}
                            >
                                {isExpanded ? '▼' : '▶'}
                            </button>
                        )}
                        {!isDir && <span style={{ marginRight: '8px' }}>-</span>}

                        <span className={`file-icon ${file.type}`}>
                            {isDir ? '📁' : '📄'}
                        </span>

                        <span
                            className={`file-name ${selectedFile?.path === file.path ? 'selected' : ''}`}
                            style={{
                                color: selectedFile?.path === file.path ? '#007bff' : '#c1c2c5',
                                fontWeight: selectedFile?.path === file.path ? 'bold' : 'normal'
                            }}
                        >
                            {file.name}
                        </span>
                    </div>

                    {isDir && isExpanded && hasChildren && (
                        renderFileTree(file.path, depth + 1)
                    )}
                </div>
            );
        });
    };

    return (
        <div className="filesystem-browser">
            <h3>File System</h3>

            <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '20px', marginTop: '20px' }}>
                {/* File tree */}
                <div className="file-tree-container">
                    <h4 style={{ marginTop: 0, marginBottom: '10px', color: '#e9ecef' }}>
                        Directory Structure
                    </h4>
                    <div className="file-tree" style={{
                        backgroundColor: '#0d0e12',
                        border: '1px solid #373a40',
                        borderRadius: '8px',
                        padding: '15px',
                        maxHeight: '400px',
                        overflowY: 'auto',
                        fontFamily: 'monospace',
                        fontSize: '13px'
                    }}>
                        {renderFileTree()}
                    </div>
                </div>

                {/* File details */}
                <div className="file-details-container">
                    <h4 style={{ marginTop: 0, marginBottom: '10px', color: '#e9ecef' }}>
                        File Information
                    </h4>
                    {selectedFile ? (
                        <div style={{
                            backgroundColor: '#0d0e12',
                            border: '1px solid #373a40',
                            borderRadius: '8px',
                            padding: '15px',
                            fontFamily: 'monospace',
                            fontSize: '13px'
                        }}>
                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Path:</span>
                                <br />
                                <code style={{ color: '#51cf66' }}>{selectedFile.path}</code>
                            </div>

                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Name:</span>
                                <br />
                                <code>{selectedFile.name}</code>
                            </div>

                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Type:</span>
                                <br />
                                <code style={{ color: '#ffd43b' }}>
                                    {selectedFile.type}
                                </code>
                            </div>

                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Size:</span>
                                <br />
                                <code>
                                    {selectedFile.size === 0
                                        ? 'N/A (directory)'
                                        : selectedFile.size + ' bytes'
                                    }
                                </code>
                            </div>

                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Inode:</span>
                                <br />
                                <code>{selectedFile.inode}</code>
                            </div>

                            <div style={{ marginBottom: '10px' }}>
                                <span style={{ color: '#a6a7ab' }}>Permissions:</span>
                                <br />
                                <code style={{ color: '#ff6b6b' }}>
                                    {selectedFile.permissions}
                                </code>
                            </div>
                        </div>
                    ) : (
                        <div style={{
                            backgroundColor: '#0d0e12',
                            border: '1px solid #373a40',
                            borderRadius: '8px',
                            padding: '15px',
                            color: '#a6a7ab',
                            textAlign: 'center'
                        }}>
                            Select a file to view details
                        </div>
                    )}
                </div>
            </div>

            {/* File statistics */}
            <div className="file-stats" style={{ marginTop: '20px' }}>
                <h4 style={{ marginTop: 0, color: '#e9ecef' }}>Statistics</h4>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: '10px' }}>
                    <div style={{
                        backgroundColor: '#1a1b1e',
                        border: '1px solid #373a40',
                        borderRadius: '8px',
                        padding: '15px',
                        textAlign: 'center'
                    }}>
                        <p style={{ margin: '0 0 10px 0', color: '#a6a7ab', fontSize: '12px' }}>
                            Total Files
                        </p>
                        <p style={{ margin: 0, fontSize: '24px', fontWeight: 'bold', color: '#51cf66' }}>
                            {fileList.filter(f => f.type === 'file').length}
                        </p>
                    </div>

                    <div style={{
                        backgroundColor: '#1a1b1e',
                        border: '1px solid #373a40',
                        borderRadius: '8px',
                        padding: '15px',
                        textAlign: 'center'
                    }}>
                        <p style={{ margin: '0 0 10px 0', color: '#a6a7ab', fontSize: '12px' }}>
                            Total Directories
                        </p>
                        <p style={{ margin: 0, fontSize: '24px', fontWeight: 'bold', color: '#007bff' }}>
                            {fileList.filter(f => f.type === 'directory').length}
                        </p>
                    </div>

                    <div style={{
                        backgroundColor: '#1a1b1e',
                        border: '1px solid #373a40',
                        borderRadius: '8px',
                        padding: '15px',
                        textAlign: 'center'
                    }}>
                        <p style={{ margin: '0 0 10px 0', color: '#a6a7ab', fontSize: '12px' }}>
                            Total Size
                        </p>
                        <p style={{ margin: 0, fontSize: '24px', fontWeight: 'bold', color: '#ffd43b' }}>
                            {(fileList.reduce((sum, f) => sum + f.size, 0) / 1024).toFixed(1)} KB
                        </p>
                    </div>

                    <div style={{
                        backgroundColor: '#1a1b1e',
                        border: '1px solid #373a40',
                        borderRadius: '8px',
                        padding: '15px',
                        textAlign: 'center'
                    }}>
                        <p style={{ margin: '0 0 10px 0', color: '#a6a7ab', fontSize: '12px' }}>
                            Max Inode
                        </p>
                        <p style={{ margin: 0, fontSize: '24px', fontWeight: 'bold', color: '#ff6b6b' }}>
                            {Math.max(...fileList.map(f => f.inode))}
                        </p>
                    </div>
                </div>
            </div>
        </div>
    );
}

// Inline styles for file tree items
export const fileTreeStyles = `
  .file-item {
    display: flex;
    align-items: center;
    padding: 5px 0;
    cursor: pointer;
    user-select: none;
    color: #c1c2c5;
    transition: color 0.2s ease;
  }

  .file-item:hover {
    color: #e9ecef;
  }

  .expand-btn {
    background: none;
    border: none;
    color: #a6a7ab;
    padding: 0 5px;
    cursor: pointer;
    font-size: 12px;
    transition: color 0.2s ease;
    width: 20px;
    text-align: center;
  }

  .expand-btn:hover {
    color: #007bff;
  }

  .file-icon {
    margin-right: 8px;
    font-size: 14px;
  }

  .file-name {
    transition: color 0.2s ease;
  }
`;

export default FileSystemBrowser;
