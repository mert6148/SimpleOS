/**
 * ProcessList Component
 * Displays running processes with state, priority, and memory
 */

import React, { useState, useEffect } from 'react';

export function ProcessList({ processes = [] }) {
    const [sortBy, setSortBy] = useState('pid');
    const [sortOrder, setSortOrder] = useState('asc');

    const sortedProcesses = [...processes].sort((a, b) => {
        let aVal = a[sortBy];
        let bVal = b[sortBy];

        if (typeof aVal === 'string') {
            return sortOrder === 'asc'
                ? aVal.localeCompare(bVal)
                : bVal.localeCompare(aVal);
        }

        return sortOrder === 'asc' ? aVal - bVal : bVal - aVal;
    });

    const handleSort = (field) => {
        if (sortBy === field) {
            setSortOrder(sortOrder === 'asc' ? 'desc' : 'asc');
        } else {
            setSortBy(field);
            setSortOrder('asc');
        }
    };

    return (
        <div className="process-section">
            <h3>Running Processes ({processes.length})</h3>

            <div className="table-wrapper">
                <table className="process-table">
                    <thead>
                        <tr>
                            <th onClick={() => handleSort('pid')} style={{ cursor: 'pointer' }}>
                                PID {sortBy === 'pid' && (sortOrder === 'asc' ? '↑' : '↓')}
                            </th>
                            <th onClick={() => handleSort('name')} style={{ cursor: 'pointer' }}>
                                Name {sortBy === 'name' && (sortOrder === 'asc' ? '↑' : '↓')}
                            </th>
                            <th onClick={() => handleSort('priority')} style={{ cursor: 'pointer' }}>
                                Priority {sortBy === 'priority' && (sortOrder === 'asc' ? '↑' : '↓')}
                            </th>
                            <th onClick={() => handleSort('state')} style={{ cursor: 'pointer' }}>
                                State {sortBy === 'state' && (sortOrder === 'asc' ? '↑' : '↓')}
                            </th>
                            <th onClick={() => handleSort('memory')} style={{ cursor: 'pointer' }}>
                                Memory {sortBy === 'memory' && (sortOrder === 'asc' ? '↑' : '↓')}
                            </th>
                            <th>Actions</th>
                        </tr>
                    </thead>
                    <tbody>
                        {sortedProcesses.length === 0 ? (
                            <tr>
                                <td colSpan="6" style={{ textAlign: 'center', padding: '20px' }}>
                                    No processes running
                                </td>
                            </tr>
                        ) : (
                            sortedProcesses.map((proc) => (
                                <tr key={proc.pid}>
                                    <td>{proc.pid}</td>
                                    <td><code>{proc.name}</code></td>
                                    <td>{proc.priority}</td>
                                    <td>
                                        <span className={`state ${proc.state.toLowerCase()}`}>
                                            {proc.state}
                                        </span>
                                    </td>
                                    <td>{(proc.memory / 1024).toFixed(2)} KB</td>
                                    <td>
                                        {proc.pid > 0 && (
                                            <button
                                                className="action-btn kill"
                                                onClick={() => console.log(`Kill ${proc.pid}`)}
                                                title="Terminate process"
                                            >
                                                ✕
                                            </button>
                                        )}
                                    </td>
                                </tr>
                            ))
                        )}
                    </tbody>
                </table>
            </div>
        </div>
    );
}

export default ProcessList;
