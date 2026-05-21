#!/usr/bin/env python3
"""
SimpleOS - Interactive Project Navigator
Provides easy access to all kernel components and documentation
"""

import os
import sys
import subprocess
import re
from pathlib import Path
from typing import List, Dict, Optional

class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    GRAY = '\033[90m'

class ProjectNavigator:
    def __init__(self, base_path: str = "."):
        self.base_path = Path(base_path)
        self.favorites = []
        self.history = []
    
    def print_header(self):
        print(f"""
{Colors.BLUE}╔════════════════════════════════════════════════════════════╗
║                                                            ║
║            {Colors.BOLD}SimpleOS Kernel Navigator v2.0{Colors.ENDC}{Colors.BLUE}               ║
║                                                            ║
║        Building the Central Brain from Scratch             ║
║              Complete x86 Operating System                 ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝{Colors.ENDC}
""")
    
    def count_lines(self, file_path: Path) -> int:
        """Count lines in a file (for statistics)."""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                return len(f.readlines())
        except:
            return 0
    
    def search_files(self, query: str) -> List[Path]:
        """Search for files matching the query."""
        results = []
        pattern = re.compile(query, re.IGNORECASE)
        try:
            for root, dirs, files in os.walk(self.base_path):
                # Skip ignored directories
                dirs[:] = [d for d in dirs if d not in ['.git', '__pycache__', 'venv', 'node_modules', '.vscode']]
                for file in files:
                    if pattern.search(file):
                        full_path = Path(root) / file
                        results.append(full_path)
        except Exception as e:
            print(f"{Colors.RED}Error searching: {e}{Colors.ENDC}")
        return results
    
    def open_file(self, file_path: Path) -> bool:
        """Open a file with default application."""
        try:
            if sys.platform == 'win32':
                os.startfile(file_path)
            elif sys.platform == 'darwin':
                subprocess.run(['open', file_path])
            else:
                subprocess.run(['xdg-open', file_path])
            return True
        except Exception as e:
            print(f"{Colors.RED}Could not open file: {e}{Colors.ENDC}")
            return False
    
    def open_directory(self, dir_path: Path) -> bool:
        """Open a directory in file explorer."""
        try:
            if sys.platform == 'win32':
                os.startfile(dir_path)
            elif sys.platform == 'darwin':
                subprocess.run(['open', dir_path])
            else:
                subprocess.run(['nautilus', dir_path])
            return True
        except Exception as e:
            print(f"{Colors.RED}Could not open directory: {e}{Colors.ENDC}")
            return False
    
    def open_terminal(self, dir_path: Path) -> bool:
        """Open terminal in specified directory."""
        try:
            if sys.platform == 'win32':
                os.chdir(dir_path)
                os.system("start cmd.exe")
            elif sys.platform == 'darwin':
                subprocess.run(['open', '-a', 'Terminal', dir_path])
            else:
                subprocess.Popen(['gnome-terminal', f'--working-directory={dir_path}'])
            return True
        except Exception as e:
            print(f"{Colors.RED}Could not open terminal: {e}{Colors.ENDC}")
            return False
    
    def run_command(self, command: str) -> bool:
        """Run a shell command."""
        try:
            print(f"{Colors.CYAN}▶ Running: {command}{Colors.ENDC}")
            result = subprocess.run(command, shell=True, capture_output=True, text=True)
            if result.stdout:
                print(result.stdout)
            if result.stderr:
                print(f"{Colors.YELLOW}{result.stderr}{Colors.ENDC}")
            return result.returncode == 0
        except Exception as e:
            print(f"{Colors.RED}Command failed: {e}{Colors.ENDC}")
            return False
    
    def calculate_statistics(self) -> Dict[str, int]:
        """Calculate project statistics."""
        stats = {
            'total_lines': 0,
            'total_files': 0,
            'c_files': 0,
            'py_files': 0,
            'h_files': 0,
            'md_files': 0,
        }
        
        for root, dirs, files in os.walk(self.base_path):
            dirs[:] = [d for d in dirs if d not in ['.git', '__pycache__', 'venv', 'node_modules']]
            for file in files:
                if file.endswith(('.c', '.py', '.h', '.md', '.s', '.asm')):
                    stats['total_files'] += 1
                    file_path = Path(root) / file
                    lines = self.count_lines(file_path)
                    stats['total_lines'] += lines
                    
                    if file.endswith('.c'):
                        stats['c_files'] += 1
                    elif file.endswith('.py'):
                        stats['py_files'] += 1
                    elif file.endswith('.h'):
                        stats['h_files'] += 1
                    elif file.endswith('.md'):
                        stats['md_files'] += 1
        
        return stats

    def print_section(self, title: str, items: List[str]):
        """Print a formatted section."""
        print(f"\n{Colors.YELLOW}{Colors.BOLD}{title}{Colors.ENDC}")
        print("─" * 60)
        for i, item in enumerate(items, 1):
            print(f"  {Colors.CYAN}{i}{Colors.ENDC}. {item}")
    
    def show_main_menu(self) -> str:
        """Display main menu and get user choice."""
        self.print_header()
        
        menu_items = [
            "📂 Browse Components",
            "🔍 Search Files",
            "📊 Project Statistics",
            "⚡ Quick Commands",
            "📚 Documentation",
            "🎯 Learning Path",
            "❤️  Favorites",
            "❌ Exit",
        ]
        
        self.print_section("Main Menu", menu_items)
        
        choice = input(f"\n{Colors.GREEN}Select option (1-{len(menu_items)}): {Colors.ENDC}").strip()
        return choice
    
    def show_components_menu(self) -> Optional[str]:
        """Show project components browser."""
        components = {
            '1': ('📖 Documentation', self.base_path),
            '2': ('🧠 Kernel Core', self.base_path / 'kernel' / 'core'),
            '3': ('💾 Memory Manager', self.base_path / 'kernel' / 'mm'),
            '4': ('📁 File System', self.base_path / 'kernel' / 'fs'),
            '5': ('🔌 Drivers', self.base_path / 'kernel' / 'drivers'),
            '6': ('⚙️  x86 Architecture', self.base_path / 'kernel' / 'arch' / 'x86'),
            '7': ('📋 Headers', self.base_path / 'kernel' / 'include'),
            '8': ('🔨 Build System', self.base_path),
            '9': ('📚 Project Root', self.base_path),
            '0': ('◀ Back', None),
        }
        
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Project Components{Colors.ENDC}")
        print("─" * 60)
        for key, (label, _) in components.items():
            print(f"  {Colors.CYAN}{key}{Colors.ENDC}. {label}")
        
        choice = input(f"\n{Colors.GREEN}Select component: {Colors.ENDC}").strip()
        
        if choice in components and choice != '0':
            _, path = components[choice]
            if path and path.exists():
                self.open_directory(path)
                self.history.append(str(path))
            return choice
        
        return choice
    
    def show_search_menu(self):
        """Search files in project."""
        print(f"\n{Colors.YELLOW}{Colors.BOLD}File Search{Colors.ENDC}")
        print("─" * 60)
        
        query = input(f"{Colors.GREEN}Search pattern (e.g., 'sched', '*.c'): {Colors.ENDC}").strip()
        
        if not query:
            print(f"{Colors.YELLOW}Search cancelled.{Colors.ENDC}")
            return
        
        results = self.search_files(query)
        
        if not results:
            print(f"{Colors.YELLOW}No files found matching '{query}'.{Colors.ENDC}")
            return
        
        print(f"\n{Colors.GREEN}Found {len(results)} file(s):{Colors.ENDC}")
        for i, file in enumerate(results[:20], 1):
            lines = self.count_lines(file)
            rel_path = file.relative_to(self.base_path) if file.is_relative_to(self.base_path) else file
            print(f"  {Colors.CYAN}{i}{Colors.ENDC}. {rel_path} {Colors.GRAY}({lines} lines){Colors.ENDC}")
        
        if len(results) > 20:
            print(f"  {Colors.YELLOW}... and {len(results) - 20} more{Colors.ENDC}")
        
        try:
            choice = input(f"\n{Colors.GREEN}Open file (number/n to skip): {Colors.ENDC}").strip()
            if choice.isdigit() and 1 <= int(choice) <= len(results[:20]):
                self.open_file(results[int(choice) - 1])
        except (ValueError, IndexError):
            pass
    
    def show_statistics(self):
        """Display project statistics."""
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Project Statistics{Colors.ENDC}")
        print("─" * 60)
        print(f"{Colors.CYAN}Calculating...{Colors.ENDC}")
        
        stats = self.calculate_statistics()
        
        print(f"\n{Colors.BOLD}Code Files:{Colors.ENDC}")
        print(f"  C Files:          {stats['c_files']}")
        print(f"  Python Files:     {stats['py_files']}")
        print(f"  Header Files:     {stats['h_files']}")
        print(f"  Documentation:    {stats['md_files']}")
        print(f"  {Colors.GRAY}{'─' * 50}{Colors.ENDC}")
        print(f"  Total Files:      {stats['total_files']}")
        print(f"  Total Lines:      {Colors.GREEN}{stats['total_lines']:,}{Colors.ENDC}")
    
    def show_commands_menu(self):
        """Show quick commands."""
        commands = {
            '1': ('Build kernel', 'make'),
            '2': ('Run in QEMU', 'make qemu'),
            '3': ('Clean build', 'make clean'),
            '4': ('Full rebuild', 'make all'),
            '5': ('Run tests', 'make test 2>nul || echo "No tests"'),
            '6': ('Show git status', 'git status'),
            '7': ('View recent commits', 'git log --oneline -10'),
            '0': ('◀ Back', None),
        }
        
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Quick Commands{Colors.ENDC}")
        print("─" * 60)
        for key, (label, _) in commands.items():
            print(f"  {Colors.CYAN}{key}{Colors.ENDC}. {label}")
        
        choice = input(f"\n{Colors.GREEN}Select command: {Colors.ENDC}").strip()
        
        if choice in commands and choice != '0':
            _, cmd = commands[choice]
            if cmd:
                self.run_command(cmd)
    
    def show_documentation(self):
        """Show documentation sections."""
        docs = {
            '1': ('README.md', self.base_path / 'README.md'),
            '2': ('ARCHITECTURE.md', self.base_path / 'ARCHITECTURE.md'),
            '3': ('BUILDING.md', self.base_path / 'BUILDING.md'),
            '4': ('SUMMARY.md', self.base_path / 'SUMMARY.md'),
            '0': ('◀ Back', None),
        }
        
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Documentation{Colors.ENDC}")
        print("─" * 60)
        for key, (label, _) in docs.items():
            print(f"  {Colors.CYAN}{key}{Colors.ENDC}. {label}")
        
        choice = input(f"\n{Colors.GREEN}Select document: {Colors.ENDC}").strip()
        
        if choice in docs and choice != '0':
            _, doc_path = docs[choice]
            if doc_path and doc_path.exists():
                self.open_file(doc_path)
    
    def show_learning_path(self):
        """Display recommended learning path."""
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Recommended Learning Path{Colors.ENDC}")
        print("─" * 60)
        
        path_items = [
            "README.md              (10 min)  - Understand what SimpleOS is",
            "ARCHITECTURE.md        (20 min)  - Learn the design & structure",
            "kernel/core/kmain.c    (15 min)  - See how kernel starts",
            "kernel/core/sched.c    (30 min)  - Understand scheduling",
            "kernel/mm/mm.c         (30 min)  - Learn memory management",
            "BUILDING.md            (15 min)  - How to build & debug",
            "Run in QEMU            (5 min)   - See it in action",
            "Debug in GDB           (45 min)  - Step through execution",
        ]
        
        for item in path_items:
            print(f"  ▶ {item}")
        
        print(f"\n  {Colors.GREEN}Total: ~3 hours for solid understanding{Colors.ENDC}")
        
        input(f"\n{Colors.GRAY}Press Enter to continue...{Colors.ENDC}")
    
    def show_favorites(self):
        """Show and manage favorites."""
        if not self.favorites:
            print(f"\n{Colors.YELLOW}No favorites yet. Add directories to your favorites!{Colors.ENDC}")
            return
        
        print(f"\n{Colors.YELLOW}{Colors.BOLD}Favorites{Colors.ENDC}")
        print("─" * 60)
        for i, fav in enumerate(self.favorites, 1):
            print(f"  {Colors.CYAN}{i}{Colors.ENDC}. {fav}")
    
    def run(self):
        """Main interactive loop."""
        while True:
            try:
                choice = self.show_main_menu()
                
                if choice == '1':
                    self.show_components_menu()
                elif choice == '2':
                    self.show_search_menu()
                elif choice == '3':
                    self.show_statistics()
                elif choice == '4':
                    self.show_commands_menu()
                elif choice == '5':
                    self.show_documentation()
                elif choice == '6':
                    self.show_learning_path()
                elif choice == '7':
                    self.show_favorites()
                elif choice == '8':
                    print(f"\n{Colors.GREEN}Goodbye!{Colors.ENDC}\n")
                    break
                else:
                    print(f"{Colors.YELLOW}Invalid option. Try again.{Colors.ENDC}")
                
                input(f"\n{Colors.GRAY}Press Enter to continue...{Colors.ENDC}")
                print("\n" * 2)
                
            except KeyboardInterrupt:
                print(f"\n\n{Colors.GREEN}Goodbye!{Colors.ENDC}\n")
                break
            except Exception as e:
                print(f"{Colors.RED}Error: {e}{Colors.ENDC}")
                input(f"\n{Colors.GRAY}Press Enter to continue...{Colors.ENDC}")



def main():
    """Entry point for the navigator."""
    try:
        navigator = ProjectNavigator()
        navigator.run()
    except Exception as e:
        print(f"{Colors.RED}Fatal error: {e}{Colors.ENDC}")
        sys.exit(1)


if __name__ == "__main__":
    main()
