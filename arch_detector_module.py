"""
SimpleOS Architecture Detector Module
Can be imported directly as: from arch_detector_module import ...
"""

import os
import sys
import struct
import platform
from typing import Dict, Tuple, Optional
from dataclasses import dataclass


__version__ = "1.0.0"
__all__ = [
    "SystemArchitecture",
    "get_system_info",
    "is_x64",
    "is_x86",
    "get_architecture",
    "ArchitectureInfo",
]


@dataclass
class ArchitectureInfo:
    """Architecture information container"""
    arch: str
    bits: int
    is_64bit: bool
    is_32bit: bool
    machine: str
    processor: str
    python_bits: int


class SystemArchitecture:
    """
    Detects and manages system architecture information
    Supports both x86 (32-bit) and x64 (64-bit) systems
    """
    
    ARCH_X86 = "x86"
    ARCH_X64 = "x64"
    ARCH_UNKNOWN = "unknown"
    
    def __init__(self):
        """Initialize architecture detector"""
        self._arch_cache = None
        self._bits_cache = None
    
    @staticmethod
    def get_architecture() -> str:
        """
        Detect system architecture
        Returns: 'x86' (32-bit) or 'x64' (64-bit)
        """
        machine = platform.machine().lower()
        
        # Windows
        if sys.platform == 'win32':
            if 'AMD64' in platform.processor() or machine == 'amd64':
                return SystemArchitecture.ARCH_X64
            elif 'x86' in platform.processor() or machine == 'x86':
                return SystemArchitecture.ARCH_X86
        
        # Linux/Unix
        if machine in ('x86_64', 'amd64'):
            return SystemArchitecture.ARCH_X64
        elif machine in ('i386', 'i686', 'x86'):
            return SystemArchitecture.ARCH_X86
        
        # Fallback to struct
        if struct.calcsize("P") == 8:
            return SystemArchitecture.ARCH_X64
        elif struct.calcsize("P") == 4:
            return SystemArchitecture.ARCH_X86
        
        return SystemArchitecture.ARCH_UNKNOWN
    
    @staticmethod
    def get_bits() -> int:
        """Get system bit width (32 or 64)"""
        return struct.calcsize("P") * 8
    
    @staticmethod
    def is_x64() -> bool:
        """Check if system is 64-bit"""
        return SystemArchitecture.get_bits() == 64
    
    @staticmethod
    def is_x86() -> bool:
        """Check if system is 32-bit"""
        return SystemArchitecture.get_bits() == 32
    
    @staticmethod
    def get_full_info() -> ArchitectureInfo:
        """Get complete architecture information"""
        arch = SystemArchitecture.get_architecture()
        bits = SystemArchitecture.get_bits()
        
        return ArchitectureInfo(
            arch=arch,
            bits=bits,
            is_64bit=bits == 64,
            is_32bit=bits == 32,
            machine=platform.machine(),
            processor=platform.processor(),
            python_bits=struct.calcsize("P") * 8,
        )


# Global instance
_arch_detector = SystemArchitecture()


def get_architecture() -> str:
    """Get current system architecture ('x86' or 'x64')"""
    return _arch_detector.get_architecture()


def is_x64() -> bool:
    """Check if running on 64-bit system"""
    return _arch_detector.is_x64()


def is_x86() -> bool:
    """Check if running on 32-bit system"""
    return _arch_detector.is_x86()


def get_system_info() -> ArchitectureInfo:
    """Get detailed system architecture information"""
    return _arch_detector.get_full_info()


def print_system_info():
    """Print formatted system information"""
    info = get_system_info()
    print(f"\n{'='*50}")
    print(f"System Architecture Information")
    print(f"{'='*50}")
    print(f"Architecture:     {info.arch.upper()}")
    print(f"Bit Width:        {info.bits}-bit")
    print(f"Is 64-bit:        {info.is_64bit}")
    print(f"Is 32-bit:        {info.is_32bit}")
    print(f"Machine:          {info.machine}")
    print(f"Processor:        {info.processor}")
    print(f"Python Bits:      {info.python_bits}")
    print(f"Platform:         {sys.platform}")
    print(f"Python Version:   {sys.version.split()[0]}")
    print(f"{'='*50}\n")


if __name__ == "__main__":
    print_system_info()
