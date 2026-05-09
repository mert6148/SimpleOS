/**
 * SimpleOS Dashboard Component
 * Main application for kernel monitoring and system interaction
 */

import React, { useState, useEffect } from 'react';
import './App.css';

// Import components (to be created)
// import KernelStatus from './components/KernelStatus';
// import TerminalEmulator from './components/TerminalEmulator';
// import MemoryMonitor from './components/MemoryMonitor';
// import ProcessList from './components/ProcessList';
// import SystemMetrics from './components/SystemMetrics';

function App() {
  const [activeTab, setActiveTab] = useState('overview');
  const [kernelData, setKernelData] = useState(null);
  const [connected, setConnected] = useState(false);

  // Connect to kernel/system on mount
  useEffect(() => {
    // Simulate kernel connection
    const mockKernelData = {
      status: 'running',
      uptime: 3600,
      version: '1.0.0',
      memory: {
        total: 128,
        used: 45,
        free: 83
      },
      cpu: {
        utilization: 34,
        cores: 1,
        frequency: 2400
      },
      processes: [
        { pid: 1, name: 'init', priority: 'normal', state: 'running' },
        { pid: 2, name: 'idle', priority: 'low', state: 'sleeping' }
      ]
    };

    setKernelData(mockKernelData);
    setConnected(true);
  }, []);

  const handleTabChange = (tab) => {
    setActiveTab(tab);
  };

  return (
    <div className="App">
      {/* Header */}
      <header className="app-header">
        <div className="header-content">
          <h1>🖥️ SimpleOS Kernel Dashboard</h1>
          <div className="header-status">
            <span className={`status-indicator ${connected ? 'connected' : 'disconnected'}`}></span>
            <span className="status-text">
              {connected ? '✓ Connected' : '✗ Disconnected'}
            </span>
          </div>
        </div>
      </header>

      {/* Navigation */}
      <nav className="app-nav">
        <button
          className={`nav-button ${activeTab === 'overview' ? 'active' : ''}`}
          onClick={() => handleTabChange('overview')}
        >
          📊 Overview
        </button>
        <button
          className={`nav-button ${activeTab === 'terminal' ? 'active' : ''}`}
          onClick={() => handleTabChange('terminal')}
        >
          💻 Terminal
        </button>
        <button
          className={`nav-button ${activeTab === 'processes' ? 'active' : ''}`}
          onClick={() => handleTabChange('processes')}
        >
          ⚙️ Processes
        </button>
        <button
          className={`nav-button ${activeTab === 'memory' ? 'active' : ''}`}
          onClick={() => handleTabChange('memory')}
        >
          💾 Memory
        </button>
        <button
          className={`nav-button ${activeTab === 'metrics' ? 'active' : ''}`}
          onClick={() => handleTabChange('metrics')}
        >
          📈 Metrics
        </button>
      </nav>

      {/* Main Content */}
      <main className="app-main">
        {/* Overview Tab */}
        {activeTab === 'overview' && kernelData && (
          <div className="tab-content">
            <h2>System Overview</h2>
            <div className="overview-grid">
              {/* Kernel Status Card */}
              <div className="kernel-status-card">
                <h3>Kernel Status</h3>
                <div className="status-info">
                  <p><strong>Version:</strong> SimpleOS v{kernelData.version}</p>
                  <p><strong>Status:</strong> <span className="status-running">Running</span></p>
                  <p><strong>Uptime:</strong> {Math.floor(kernelData.uptime / 3600)}h {Math.floor((kernelData.uptime % 3600) / 60)}m</p>
                </div>
              </div>

              {/* Quick Stats */}
              <div className="quick-stats">
                <div className="stat-card">
                  <h4>Memory</h4>
                  <div className="stat-value">
                    {kernelData.memory.used}/{kernelData.memory.total} MB
                  </div>
                  <div className="stat-bar">
                    <div
                      className="stat-fill memory"
                      style={{
                        width: `${(kernelData.memory.used / kernelData.memory.total) * 100}%`
                      }}
                    ></div>
                  </div>
                  <small>{((kernelData.memory.used / kernelData.memory.total) * 100).toFixed(1)}%</small>
                </div>

                <div className="stat-card">
                  <h4>CPU</h4>
                  <div className="stat-value">
                    {kernelData.cpu.utilization}%
                  </div>
                  <div className="stat-bar">
                    <div
                      className="stat-fill cpu"
                      style={{ width: `${kernelData.cpu.utilization}%` }}
                    ></div>
                  </div>
                  <small>{kernelData.cpu.cores} core @ {kernelData.cpu.frequency} MHz</small>
                </div>

                <div className="stat-card">
                  <h4>Processes</h4>
                  <div className="stat-value">
                    {kernelData.processes.length}
                  </div>
                  <small>Active processes</small>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* Terminal Tab */}
        {activeTab === 'terminal' && (
          <div className="tab-content">
            <h2>Terminal Emulator</h2>
            <div className="terminal-container">
              <div className="terminal">
                <div className="terminal-output">
                  <p>SimpleOS Terminal v1.0.0</p>
                  <p>Type 'help' for available commands</p>
                  <p>> </p>
                </div>
                <input
                  type="text"
                  className="terminal-input"
                  placeholder="Enter command..."
                />
              </div>
            </div>
          </div>
        )}

        {/* Processes Tab */}
        {activeTab === 'processes' && kernelData && (
          <div className="tab-content">
            <h2>Process List</h2>
            <table className="process-table">
              <thead>
                <tr>
                  <th>PID</th>
                  <th>Name</th>
                  <th>Priority</th>
                  <th>State</th>
                </tr>
              </thead>
              <tbody>
                {kernelData.processes.map((proc) => (
                  <tr key={proc.pid}>
                    <td>{proc.pid}</td>
                    <td>{proc.name}</td>
                    <td>{proc.priority}</td>
                    <td><span className={`state ${proc.state}`}>{proc.state}</span></td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        )}

        {/* Memory Tab */}
        {activeTab === 'memory' && kernelData && (
          <div className="tab-content">
            <h2>Memory Monitor</h2>
            <div className="memory-stats">
              <div className="memory-card">
                <h4>Total Memory</h4>
                <p className="large-value">{kernelData.memory.total} MB</p>
              </div>
              <div className="memory-card">
                <h4>Used</h4>
                <p className="large-value" style={{ color: '#ff6b6b' }}>{kernelData.memory.used} MB</p>
              </div>
              <div className="memory-card">
                <h4>Free</h4>
                <p className="large-value" style={{ color: '#51cf66' }}>{kernelData.memory.free} MB</p>
              </div>
              <div className="memory-pie">
                <div style={{
                  width: '200px',
                  height: '200px',
                  borderRadius: '50%',
                  background: `conic-gradient(#ff6b6b ${(kernelData.memory.used / kernelData.memory.total) * 360}deg, #51cf66 0deg)`,
                  margin: '20px auto'
                }}></div>
                <p>Memory Distribution</p>
              </div>
            </div>
          </div>
        )}

        {/* Metrics Tab */}
        {activeTab === 'metrics' && kernelData && (
          <div className="tab-content">
            <h2>System Metrics</h2>
            <div className="metrics-grid">
              <div className="metric-card">
                <h4>CPU Frequency</h4>
                <p className="metric-value">{kernelData.cpu.frequency} MHz</p>
              </div>
              <div className="metric-card">
                <h4>CPU Cores</h4>
                <p className="metric-value">{kernelData.cpu.cores}</p>
              </div>
              <div className="metric-card">
                <h4>System Version</h4>
                <p className="metric-value">v{kernelData.version}</p>
              </div>
              <div className="metric-card">
                <h4>Connection Status</h4>
                <p className="metric-value" style={{ color: connected ? '#51cf66' : '#ff6b6b' }}>
                  {connected ? 'Connected' : 'Disconnected'}
                </p>
              </div>
            </div>
          </div>
        )}
      </main>

      {/* Footer */}
      <footer className="app-footer">
        <p>SimpleOS v{kernelData?.version || '1.0.0'} | Educational OS Dashboard</p>
        <p>© 2026 SimpleOS Project</p>
      </footer>
    </div>
  );
}

export default App;
