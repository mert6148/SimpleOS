/**
 * MemoryMonitor Component
 * Displays memory usage with pie chart visualization and statistics
 */

import React, { useMemo } from 'react';

export function MemoryMonitor({ memory = {} }) {
    const {
        total = 268435456,  // 256MB default
        used = 134217728,   // 128MB default
        free = 134217728,
        available = 134217728
    } = memory;

    // Calculate percentages
    const usedPercent = ((used / total) * 100).toFixed(1);
    const freePercent = ((free / total) * 100).toFixed(1);
    const availablePercent = ((available / total) * 100).toFixed(1);

    // Format bytes to human readable
    const formatBytes = (bytes) => {
        if (bytes === 0) return '0 B';
        const k = 1024;
        const sizes = ['B', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return (bytes / Math.pow(k, i)).toFixed(2) + ' ' + sizes[i];
    };

    // Calculate color based on usage
    const getMemoryColor = () => {
        const usage = (used / total) * 100;
        if (usage < 50) return '#51cf66';  // Green - good
        if (usage < 75) return '#ffd43b';  // Yellow - warning
        return '#ff6b6b';                  // Red - critical
    };

    // SVG Pie chart
    const pieData = useMemo(() => {
        const usedAngle = (used / total) * 360;
        const freeAngle = (free / total) * 360;
        return { usedAngle, freeAngle };
    }, [total, used, free]);

    return (
        <div className="memory-monitor">
            <h3>Memory Statistics</h3>

            <div className="memory-stats">
                <div className="memory-card">
                    <h4>Total Memory</h4>
                    <div className="large-value">{formatBytes(total)}</div>
                    <p style={{ fontSize: '12px', color: '#a6a7ab' }}>
                        {(total / (1024 * 1024 * 1024)).toFixed(2)} GB
                    </p>
                </div>

                <div className="memory-card">
                    <h4>Used Memory</h4>
                    <div className="large-value" style={{ color: getMemoryColor() }}>
                        {formatBytes(used)}
                    </div>
                    <div className="stat-bar">
                        <div
                            className="stat-fill memory"
                            style={{ width: usedPercent + '%' }}
                        ></div>
                    </div>
                    <p style={{ fontSize: '12px', color: '#a6a7ab' }}>
                        {usedPercent}% of total
                    </p>
                </div>

                <div className="memory-card">
                    <h4>Free Memory</h4>
                    <div className="large-value" style={{ color: '#51cf66' }}>
                        {formatBytes(free)}
                    </div>
                    <div className="stat-bar">
                        <div
                            className="stat-fill cpu"
                            style={{ width: freePercent + '%' }}
                        ></div>
                    </div>
                    <p style={{ fontSize: '12px', color: '#a6a7ab' }}>
                        {freePercent}% of total
                    </p>
                </div>

                <div className="memory-card">
                    <h4>Available Memory</h4>
                    <div className="large-value" style={{ color: '#51cf66' }}>
                        {formatBytes(available)}
                    </div>
                    <p style={{ fontSize: '12px', color: '#a6a7ab' }}>
                        {availablePercent}% available for allocation
                    </p>
                </div>
            </div>

            <div className="memory-pie">
                <svg width="200" height="200" style={{ marginBottom: '20px' }}>
                    {/* Used memory segment */}
                    <circle
                        cx="100"
                        cy="100"
                        r="80"
                        fill="none"
                        stroke={getMemoryColor()}
                        strokeWidth="30"
                        strokeDasharray={`${(pieData.usedAngle / 360) * 502.4} 502.4`}
                        strokeDashoffset="0"
                        transform="rotate(-90 100 100)"
                    />
                    {/* Free memory segment */}
                    <circle
                        cx="100"
                        cy="100"
                        r="80"
                        fill="none"
                        stroke="#373a40"
                        strokeWidth="30"
                        strokeDasharray={`${(pieData.freeAngle / 360) * 502.4} 502.4`}
                        strokeDashoffset={-(pieData.usedAngle / 360) * 502.4}
                        transform="rotate(-90 100 100)"
                    />
                    {/* Center text */}
                    <text
                        x="100"
                        y="95"
                        textAnchor="middle"
                        fontSize="24"
                        fontWeight="bold"
                        fill="#c1c2c5"
                    >
                        {usedPercent}%
                    </text>
                    <text
                        x="100"
                        y="115"
                        textAnchor="middle"
                        fontSize="12"
                        fill="#a6a7ab"
                    >
                        Used
                    </text>
                </svg>

                <div style={{ textAlign: 'center' }}>
                    <p style={{ margin: '0 0 10px 0', color: '#e9ecef', fontWeight: '500' }}>
                        Memory Distribution
                    </p>
                    <div style={{ display: 'flex', justifyContent: 'center', gap: '20px', fontSize: '12px' }}>
                        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                            <div
                                style={{
                                    width: '12px',
                                    height: '12px',
                                    borderRadius: '2px',
                                    backgroundColor: getMemoryColor()
                                }}
                            ></div>
                            <span>Used: {usedPercent}%</span>
                        </div>
                        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                            <div
                                style={{
                                    width: '12px',
                                    height: '12px',
                                    borderRadius: '2px',
                                    backgroundColor: '#51cf66'
                                }}
                            ></div>
                            <span>Free: {freePercent}%</span>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
}

export default MemoryMonitor;
