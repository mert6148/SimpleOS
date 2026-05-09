/**
 * CommandInput Component
 * Command input field for sending commands to kernel
 */

import React, { useState, useRef, useEffect } from 'react';

export function CommandInput({ onCommand, disabled = false, history = [] }) {
    const [input, value] = useState('');
    const [historyIndex, setHistoryIndex] = useState(-1);
    const inputRef = useRef(null);

    const commands = [
        { name: 'ps', description: 'List running processes' },
        { name: 'meminfo', description: 'Show memory information' },
        { name: 'sched', description: 'Show scheduler statistics' },
        { name: 'help', description: 'Show available commands' },
        { name: 'clear', description: 'Clear terminal output' },
        { name: 'reboot', description: 'Restart the system' },
    ];

    const handleKeyDown = (e) => {
        if (e.key === 'Enter') {
            if (input.trim()) {
                onCommand(input.trim());
                value('');
                setHistoryIndex(-1);
            }
            e.preventDefault();
        } else if (e.key === 'ArrowUp') {
            e.preventDefault();
            const newIndex = Math.min(historyIndex + 1, history.length - 1);
            if (newIndex >= 0 && history[newIndex]) {
                value(history[newIndex]);
                setHistoryIndex(newIndex);
            }
        } else if (e.key === 'ArrowDown') {
            e.preventDefault();
            if (historyIndex > 0) {
                const newIndex = historyIndex - 1;
                value(history[newIndex]);
                setHistoryIndex(newIndex);
            } else {
                value('');
                setHistoryIndex(-1);
            }
        } else if (e.key === 'Tab') {
            e.preventDefault();
            // Auto-complete command
            const partial = input.trim();
            const matches = commands.filter(cmd => cmd.name.startsWith(partial));
            if (matches.length === 1) {
                value(matches[0].name + ' ');
            }
        }
    };

    useEffect(() => {
        if (inputRef.current) {
            inputRef.current.focus();
        }
    }, []);

    return (
        <div className="command-input-container">
            <div className="command-input-prompt">
                <div className="prompt-icon">➜</div>
                <input
                    ref={inputRef}
                    type="text"
                    className="command-input"
                    placeholder="Enter kernel command (try 'ps', 'meminfo', 'help')..."
                    value={input}
                    onChange={(e) => {
                        value(e.target.value);
                        setHistoryIndex(-1);
                    }}
                    onKeyDown={handleKeyDown}
                    disabled={disabled}
                    autoFocus
                />
                <button
                    className="send-button"
                    onClick={() => {
                        if (input.trim()) {
                            onCommand(input.trim());
                            value('');
                            setHistoryIndex(-1);
                        }
                    }}
                    disabled={disabled || !input.trim()}
                    title="Send command (Enter)"
                >
                    Send
                </button>
            </div>

            <div className="command-help">
                <h4 style={{ margin: '10px 0 5px 0', fontSize: '13px', color: '#e9ecef' }}>
                    Available Commands:
                </h4>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(200px, 1fr))', gap: '8px' }}>
                    {commands.map((cmd) => (
                        <div
                            key={cmd.name}
                            className="command-suggestion"
                            onClick={() => {
                                value(cmd.name + ' ');
                                inputRef.current?.focus();
                            }}
                            style={{
                                backgroundColor: '#0d0e12',
                                border: '1px solid #373a40',
                                borderRadius: '4px',
                                padding: '8px 12px',
                                cursor: 'pointer',
                                transition: 'all 0.2s ease'
                            }}
                            onMouseEnter={(e) => {
                                e.currentTarget.style.backgroundColor = '#1a1b1e';
                                e.currentTarget.style.borderColor = '#007bff';
                            }}
                            onMouseLeave={(e) => {
                                e.currentTarget.style.backgroundColor = '#0d0e12';
                                e.currentTarget.style.borderColor = '#373a40';
                            }}
                        >
                            <div style={{ color: '#007bff', fontWeight: 'bold', fontSize: '12px' }}>
                                {cmd.name}
                            </div>
                            <div style={{ color: '#a6a7ab', fontSize: '11px' }}>
                                {cmd.description}
                            </div>
                        </div>
                    ))}
                </div>
            </div>

            <div className="command-info" style={{
                marginTop: '10px',
                padding: '10px 15px',
                backgroundColor: 'rgba(0, 123, 255, 0.1)',
                border: '1px solid rgba(0, 123, 255, 0.3)',
                borderRadius: '4px',
                fontSize: '12px',
                color: '#a6a7ab',
                lineHeight: '1.5'
            }}>
                <strong>Tips:</strong>
                <ul style={{ margin: '5px 0 0 20px', paddingLeft: 0 }}>
                    <li>Use <code style={{ color: '#51cf66', backgroundColor: '#0d0e12', padding: '2px 4px', borderRadius: '2px' }}>↑↓</code> arrow keys to navigate command history</li>
                    <li>Press <code style={{ color: '#51cf66', backgroundColor: '#0d0e12', padding: '2px 4px', borderRadius: '2px' }}>Tab</code> to auto-complete commands</li>
                    <li>Type a command name to see what it does</li>
                </ul>
            </div>
        </div>
    );
}

// Inline CSS styles
export const commandInputStyles = `
  .command-input-container {
    background-color: #1a1b1e;
    border: 1px solid #373a40;
    border-radius: 8px;
    padding: 15px;
  }

  .command-input-prompt {
    display: flex;
    gap: 10px;
    align-items: center;
    margin-bottom: 15px;
    background-color: #0d0e12;
    border: 1px solid #373a40;
    border-radius: 6px;
    padding: 10px 15px;
  }

  .prompt-icon {
    color: #007bff;
    font-weight: bold;
    font-size: 14px;
  }

  .command-input {
    flex: 1;
    background-color: transparent;
    border: none;
    color: #e9ecef;
    font-family: 'Courier New', monospace;
    font-size: 14px;
    outline: none;
  }

  .command-input::placeholder {
    color: #a6a7ab;
  }

  .command-input:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }

  .send-button {
    background-color: #007bff;
    color: white;
    border: none;
    padding: 8px 16px;
    border-radius: 4px;
    cursor: pointer;
    font-weight: 500;
    transition: all 0.2s ease;
  }

  .send-button:hover:not(:disabled) {
    background-color: #0056b3;
    box-shadow: 0 0 8px rgba(0, 123, 255, 0.3);
  }

  .send-button:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }

  .command-help {
    margin: 15px 0;
    padding-top: 15px;
    border-top: 1px solid #373a40;
  }

  .command-suggestion {
    cursor: pointer;
    user-select: none;
  }

  .command-suggestion:active {
    transform: scale(0.98);
  }
`;

export default CommandInput;
