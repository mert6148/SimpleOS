#!/usr/bin/env python3
"""
SimpleOS Architecture Detector
Standalone utility for system architecture detection and information
Works on both Windows and Linux for x86 and x64 systems

Usage:
    python arch_detector.py              # Show system info
    python arch_detector.py --json       # JSON output
    python arch_detector.py --check x64  # Check specific architecture
"""

import sys
import os
import json
import struct
import platform
import argparse
from typing import Dict, Any
from datetime import datetime


class ArchitectureDetector:
    """Main architecture detection and reporting class"""
    
    ARCH_X86 = "x86"
    ARCH_X64 = "x64"
    ARCH_UNKNOWN = "unknown"
    
    @classmethod
    def detect(cls) -> Dict[str, Any]:
        """Detect and return complete system information"""
        bits = struct.calcsize("P") * 8
        machine = platform.machine().lower()
        
        # Determine architecture
        arch = cls.ARCH_UNKNOWN
        if bits == 64:
            arch = cls.ARCH_X64
        elif bits == 32:
            arch = cls.ARCH_X86
        
        info = {
            "timestamp": datetime.now().isoformat(),
            "architecture": arch,
            "bits": bits,
            "is_64bit": bits == 64,
            "is_32bit": bits == 32,
            "machine": machine,
            "processor": platform.processor(),
            "platform": sys.platform,
            "python_version": sys.version.split()[0],
            "python_build": platform.python_build()[0],
            "os_name": os.name,
            "system": platform.system(),
            "release": platform.release(),
            "version": platform.version(),
        }
        
        return info
    
    @classmethod
    def is_x64(cls) -> bool:
        """Check if 64-bit system"""
        return struct.calcsize("P") * 8 == 64
    
    @classmethod
    def is_x86(cls) -> bool:
        """Check if 32-bit system"""
        return struct.calcsize("P") * 8 == 32
    
    @classmethod
    def validate_architecture(cls, expected: str) -> bool:
        """Validate if running on expected architecture"""
        info = cls.detect()
        return info["architecture"].lower() == expected.lower()


def print_banner():
    """Print application banner"""
    print("\n" + "="*60)
    print("SimpleOS Architecture Detector v1.0")
    print("="*60 + "\n")


def print_info(info: Dict[str, Any], verbose: bool = False):
    """Print formatted information"""
    print(f"{'─'*60}")
    print(f"System Architecture Information")
    print(f"{'─'*60}")
    print(f"Architecture:        {info['architecture'].upper()}")
    print(f"Bit Width:           {info['bits']}-bit")
    print(f"Is 64-bit (x64):     {info['is_64bit']}")
    print(f"Is 32-bit (x86):     {info['is_32bit']}")
    print(f"Machine:             {info['machine']}")
    print(f"Processor:           {info['processor']}")
    print(f"Platform:            {info['platform']}")
    print(f"System:              {info['system']}")
    print(f"Release:             {info['release']}")
    
    if verbose:
        print(f"\nDetailed Information:")
        print(f"{'─'*60}")
        print(f"Python Version:      {info['python_version']}")
        print(f"Python Build:        {info['python_build']}")
        print(f"OS Name:             {info['os_name']}")
        print(f"Version:             {info['version']}")
        print(f"Timestamp:           {info['timestamp']}")
    
    print(f"{'─'*60}\n")


def print_json(info: Dict[str, Any], pretty: bool = True):
    """Print information as JSON"""
    if pretty:
        print(json.dumps(info, indent=2))
    else:
        print(json.dumps(info))


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="SimpleOS Architecture Detector - System information utility",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                    Show system information
  %(prog)s --verbose          Show detailed information
  %(prog)s --json             Output as JSON
  %(prog)s --check x64        Check if running on x64
  %(prog)s --check x86        Check if running on x86
        """
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Show detailed system information"
    )
    
    parser.add_argument(
        "--json", "-j",
        action="store_true",
        help="Output information as JSON"
    )
    
    parser.add_argument(
        "--compact",
        action="store_true",
        help="Compact JSON output (no indentation)"
    )
    
    parser.add_argument(
        "--check",
        type=str,
        choices=['x86', 'x64'],
        help="Check if running on specific architecture"
    )
    
    parser.add_argument(
        "--no-banner",
        action="store_true",
        help="Don't print banner"
    )
    
    args = parser.parse_args()
    
    # Print banner unless disabled
    if not args.no_banner:
        print_banner()
    
    # Detect system information
    info = ArchitectureDetector.detect()
    
    # Handle check mode
    if args.check:
        is_valid = ArchitectureDetector.validate_architecture(args.check)
        status = "✓ YES" if is_valid else "✗ NO"
        arch = args.check.upper()
        print(f"Running on {arch}? {status}")
        return 0 if is_valid else 1
    
    # Handle output modes
    if args.json:
        print_json(info, pretty=not args.compact)
    else:
        print_info(info, verbose=args.verbose)
    
    return 0


if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(130)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
