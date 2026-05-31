#include "system.h"
#include <iostream>

system_info get_system_info() {
    return {
        "x86",
        "32-bit",
        "A 32-bit SimpleOS simulation with legacy x86 register semantics and 4GB address space emulation."
    };
}

void run_system_demo() {
    std::cout << "[x86] Running system prototype demo...\n";
    std::cout << "[x86] Register set: EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP\n";
    std::cout << "[x86] Execution model: Protected mode with segmented memory and compatibility support.\n";
    std::cout << "[x86] System tasks: low-level I/O, BIOS-style interrupt vectors, and small-kernel memory layout.\n";
}
