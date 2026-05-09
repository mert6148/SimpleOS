/**
 * SimpleOS Dashboard Components Index
 * Central export point for all dashboard components
 */

export { ProcessList } from './ProcessList.js';
export { MemoryMonitor } from './MemoryMonitor.js';
export { FileSystemBrowser } from './FileSystemBrowser.js';
export { CommandInput } from './CommandInput.js';

// Component descriptions
export const COMPONENTS = {
    ProcessList: {
        name: 'ProcessList',
        description: 'Displays running processes with detailed information',
        props: ['processes'],
        features: [
            'Real-time process list',
            'Sortable columns (PID, Name, Priority, State, Memory)',
            'Process state visualization (running, sleeping, etc.)',
            'Terminate button for each process',
            'Responsive table layout'
        ]
    },
    MemoryMonitor: {
        name: 'MemoryMonitor',
        description: 'Shows memory usage statistics with visualization',
        props: ['memory'],
        features: [
            'Total, used, free, and available memory displays',
            'SVG pie chart visualization',
            'Memory usage percentage',
            'Color-coded status (green, yellow, red)',
            'Progress bars for quick assessment'
        ]
    },
    FileSystemBrowser: {
        name: 'FileSystemBrowser',
        description: 'Navigates and displays file system structure',
        props: ['files'],
        features: [
            'Expandable/collapsible directory tree',
            'File and directory icons',
            'Detailed file information panel',
            'Inode and permission display',
            'File system statistics'
        ]
    },
    CommandInput: {
        name: 'CommandInput',
        description: 'Sends commands to kernel and manages command history',
        props: ['onCommand', 'disabled', 'history'],
        features: [
            'Interactive command input with prompt',
            'Command history navigation (Arrow up/down)',
            'Tab auto-completion',
            'Available commands reference',
            'Real-time validation and tips'
        ]
    }
};

export default {
    ProcessList,
    MemoryMonitor,
    FileSystemBrowser,
    CommandInput,
    COMPONENTS
};
