#!/usr/bin/env python3
"""
SimpleOS Documentation Viewer
Desktop application for Windows and Linux
Provides offline access to SimpleOS documentation with search and offline mode

Author: Mert Doğanay
License: MIT
Version: 1.0.0
"""

import sys
import json
import os
import requests
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

try:
    from PyQt5.QtWidgets import (
        QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
        QLineEdit, QPushButton, QTextEdit, QListWidget, QListWidgetItem,
        QSplitter, QLabel, QComboBox, QProgressBar, QMessageBox,
        QTabWidget, QTableWidget, QTableWidgetItem, QFileDialog
    )
    from PyQt5.QtCore import Qt, QThread, pyqtSignal, QUrl, QSize
    from PyQt5.QtGui import QIcon, QFont, QColor, QTextCursor
    from PyQt5.QtWebEngineWidgets import QWebEngineView
except ImportError:
    print("PyQt5 not installed. Install with: pip install PyQt5 PyQtWebEngine")
    sys.exit(1)


# ============================================================
# Configuration
# ============================================================
CONFIG = {
    'api_url': 'http://localhost:8000/docs/docsservices.php',
    'cache_dir': str(Path.home() / '.simpleos' / 'docs_cache'),
    'offline_mode': False,
    'theme': 'light',
    'font_size': 11,
}


# ============================================================
# Utility Classes
# ============================================================
class ApiClient:
    """Handle API requests to PHP backend"""
    
    def __init__(self, base_url: str):
        self.base_url = base_url.rstrip('/')
        self.timeout = 10
    
    def get_sections(self) -> Optional[Dict]:
        """Get all documentation sections"""
        try:
            response = requests.get(
                f"{self.base_url}/api/sections",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"Error fetching sections: {e}")
            return None
    
    def get_section(self, section_id: str) -> Optional[Dict]:
        """Get specific section content"""
        try:
            response = requests.get(
                f"{self.base_url}/section/{section_id}",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"Error fetching section: {e}")
            return None
    
    def search(self, query: str) -> Optional[Dict]:
        """Search documentation"""
        try:
            response = requests.get(
                f"{self.base_url}/search",
                params={'q': query},
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"Error searching: {e}")
            return None
    
    def get_health(self) -> Optional[Dict]:
        """Check API health"""
        try:
            response = requests.get(
                f"{self.base_url}/api/health",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException:
            return None


class CacheManager:
    """Manage offline caching"""
    
    def __init__(self, cache_dir: str):
        self.cache_dir = Path(cache_dir)
        self.cache_dir.mkdir(parents=True, exist_ok=True)
        self.sections_file = self.cache_dir / 'sections.json'
    
    def save_sections(self, sections: Dict) -> bool:
        """Save sections to cache"""
        try:
            with open(self.sections_file, 'w', encoding='utf-8') as f:
                json.dump({
                    'timestamp': datetime.now().isoformat(),
                    'data': sections
                }, f, indent=2, ensure_ascii=False)
            return True
        except Exception as e:
            print(f"Error saving cache: {e}")
            return False
    
    def load_sections(self) -> Optional[Dict]:
        """Load sections from cache"""
        try:
            if self.sections_file.exists():
                with open(self.sections_file, 'r', encoding='utf-8') as f:
                    return json.load(f).get('data')
        except Exception as e:
            print(f"Error loading cache: {e}")
        return None
    
    def get_cache_age(self) -> Optional[str]:
        """Get cache age"""
        if self.sections_file.exists():
            stat = self.sections_file.stat()
            age = datetime.now() - datetime.fromtimestamp(stat.st_mtime)
            return f"{int(age.total_seconds() / 3600)}h ago"
        return None


# ============================================================
# Worker Thread
# ============================================================
class ApiWorker(QThread):
    """Background thread for API calls"""
    
    result_ready = pyqtSignal(dict)
    error_occurred = pyqtSignal(str)
    
    def __init__(self, api_client: ApiClient, action: str, **kwargs):
        super().__init__()
        self.api_client = api_client
        self.action = action
        self.kwargs = kwargs
    
    def run(self):
        """Execute API call"""
        try:
            if self.action == 'get_sections':
                result = self.api_client.get_sections()
            elif self.action == 'get_section':
                result = self.api_client.get_section(self.kwargs.get('section_id'))
            elif self.action == 'search':
                result = self.api_client.search(self.kwargs.get('query'))
            elif self.action == 'health':
                result = self.api_client.get_health()
            else:
                result = None
            
            if result:
                self.result_ready.emit(result)
            else:
                self.error_occurred.emit('API call failed')
        except Exception as e:
            self.error_occurred.emit(str(e))


# ============================================================
# Main Application
# ============================================================
class SimpleOSDocsViewer(QMainWindow):
    """Main application window"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle('SimpleOS Documentation Viewer')
        self.setGeometry(100, 100, 1200, 800)
        self.setStyleSheet(self.get_stylesheet())
        
        # Initialize components
        self.api_client = ApiClient(CONFIG['api_url'])
        self.cache_manager = CacheManager(CONFIG['cache_dir'])
        self.current_section = None
        self.search_results = []
        
        # Create UI
        self.create_ui()
        self.load_sections()
    
    def create_ui(self):
        """Create user interface"""
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Top toolbar
        toolbar = QHBoxLayout()
        
        # Search box
        self.search_input = QLineEdit()
        self.search_input.setPlaceholderText('Search documentation...')
        self.search_input.returnPressed.connect(self.perform_search)
        toolbar.addWidget(self.search_input)
        
        # Search button
        search_btn = QPushButton('🔍 Search')
        search_btn.clicked.connect(self.perform_search)
        toolbar.addWidget(search_btn)
        
        # Theme toggle
        theme_btn = QPushButton('🌙 Theme')
        theme_btn.clicked.connect(self.toggle_theme)
        toolbar.addWidget(theme_btn)
        
        # Offline mode indicator
        self.offline_indicator = QLabel('🌐 Online')
        toolbar.addWidget(self.offline_indicator)
        
        main_layout.addLayout(toolbar)
        
        # Content area with splitter
        splitter = QSplitter(Qt.Horizontal)
        
        # Left panel: sections list
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        left_layout.addWidget(QLabel('📚 Sections'))
        
        self.sections_list = QListWidget()
        self.sections_list.itemClicked.connect(self.on_section_selected)
        left_layout.addWidget(self.sections_list)
        
        # Right panel: content area with tabs
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        
        self.tabs = QTabWidget()
        
        # Content tab
        self.content_display = QTextEdit()
        self.content_display.setReadOnly(True)
        self.tabs.addTab(self.content_display, '📄 Content')
        
        # Search results tab
        self.search_results_widget = QTableWidget()
        self.search_results_widget.setColumnCount(3)
        self.search_results_widget.setHorizontalHeaderLabels(['Title', 'Snippet', 'Score'])
        self.search_results_widget.itemClicked.connect(self.on_result_selected)
        self.tabs.addTab(self.search_results_widget, '🔎 Search Results')
        
        # Info tab
        self.info_display = QTextEdit()
        self.info_display.setReadOnly(True)
        self.tabs.addTab(self.info_display, 'ℹ️ Info')
        
        right_layout.addWidget(self.tabs)
        
        splitter.addWidget(left_widget)
        splitter.addWidget(right_widget)
        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 2)
        
        main_layout.addWidget(splitter)
        
        # Status bar
        self.status_label = QLabel('Ready')
        self.statusBar().addWidget(self.status_label)
    
    def load_sections(self):
        """Load sections from API or cache"""
        self.status_label.setText('Loading sections...')
        
        # Try API first
        sections = self.api_client.get_sections()
        
        if sections:
            CONFIG['offline_mode'] = False
            self.offline_indicator.setText('🌐 Online')
            self.cache_manager.save_sections(sections)
        else:
            # Fall back to cache
            sections = self.cache_manager.load_sections()
            if sections:
                CONFIG['offline_mode'] = True
                self.offline_indicator.setText('📴 Offline (Cached)')
            else:
                self.status_label.setText('Error: No sections available')
                return
        
        # Populate sections list
        self.sections_list.clear()
        for section_id, section_data in sections.items():
            item = QListWidgetItem(section_data.get('title', section_id))
            item.setData(Qt.UserRole, section_id)
            self.sections_list.addItem(item)
        
        self.status_label.setText(f'Loaded {len(sections)} sections')
    
    def on_section_selected(self, item: QListWidgetItem):
        """Handle section selection"""
        section_id = item.data(Qt.UserRole)
        self.current_section = section_id
        
        self.status_label.setText(f'Loading {item.text()}...')
        
        # Fetch section content
        section = self.api_client.get_section(section_id)
        
        if section:
            self.content_display.setText(section.get('content', 'No content'))
            
            # Display info
            info_text = f"""
Section: {section.get('title', 'Unknown')}
Last Modified: {section.get('lastModified', 'Unknown')}
"""
            self.info_display.setText(info_text)
            self.tabs.setCurrentIndex(0)
            self.status_label.setText('Section loaded')
        else:
            self.content_display.setText('Error loading section')
            self.status_label.setText('Error loading section')
    
    def perform_search(self):
        """Perform search"""
        query = self.search_input.text().strip()
        
        if len(query) < 2:
            QMessageBox.warning(self, 'Search', 'Query must be at least 2 characters')
            return
        
        self.status_label.setText(f'Searching for "{query}"...')
        
        results = self.api_client.search(query)
        
        if results and 'results' in results:
            self.display_search_results(results['results'])
            self.tabs.setCurrentIndex(1)
        else:
            QMessageBox.information(self, 'Search', 'No results found')
    
    def display_search_results(self, results: List[Dict]):
        """Display search results"""
        self.search_results_widget.setRowCount(0)
        
        for i, result in enumerate(results):
            self.search_results_widget.insertRow(i)
            self.search_results_widget.setItem(
                i, 0, QTableWidgetItem(result['title'])
            )
            self.search_results_widget.setItem(
                i, 1, QTableWidgetItem(result.get('snippet', '')[:100])
            )
            self.search_results_widget.setItem(
                i, 2, QTableWidgetItem(str(result['score']))
            )
            # Store section ID
            self.search_results_widget.item(i, 0).setData(Qt.UserRole, result['id'])
        
        self.status_label.setText(f'Found {len(results)} results')
    
    def on_result_selected(self, item: QTableWidgetItem):
        """Handle result selection"""
        section_id = item.data(Qt.UserRole)
        
        # Find and select in sections list
        for i in range(self.sections_list.count()):
            list_item = self.sections_list.item(i)
            if list_item.data(Qt.UserRole) == section_id:
                self.sections_list.setCurrentItem(list_item)
                self.on_section_selected(list_item)
                break
    
    def toggle_theme(self):
        """Toggle dark/light theme"""
        if CONFIG['theme'] == 'light':
            CONFIG['theme'] = 'dark'
        else:
            CONFIG['theme'] = 'light'
        
        self.setStylesheet(self.get_stylesheet())
    
    def get_stylesheet(self) -> str:
        """Get theme stylesheet"""
        if CONFIG['theme'] == 'dark':
            return """
            QMainWindow, QWidget {
                background-color: #1e1e1e;
                color: #e0e0e0;
            }
            QLineEdit, QTextEdit, QListWidget {
                background-color: #2a2a2a;
                color: #e0e0e0;
                border: 1px solid #3a3a3a;
                border-radius: 4px;
            }
            QPushButton {
                background-color: #0d47a1;
                color: white;
                border: none;
                padding: 6px 12px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #1565c0;
            }
            """
        else:
            return """
            QMainWindow, QWidget {
                background-color: #ffffff;
                color: #333333;
            }
            QLineEdit, QTextEdit, QListWidget {
                background-color: #f5f5f5;
                color: #333333;
                border: 1px solid #ddd;
                border-radius: 4px;
            }
            QPushButton {
                background-color: #667eea;
                color: white;
                border: none;
                padding: 6px 12px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #764ba2;
            }
            """


# ============================================================
# Main Execution
# ============================================================
if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SimpleOSDocsViewer()
    window.show()
    sys.exit(app.exec_())
