#include "system.h"
#include <iostream>

system_info get_system_info() {
    return {
        "x64",
        "64-bit",
        "A 64-bit SimpleOS simulation with long mode support, extended registers, and modern kernel abstractions."
    };
}

void run_system_demo() {
    std::cout << "[x64] Running system prototype demo...\n";
    std::cout << "[x64] Register set: RAX, RBX, RCX, RDX, R8-R15, RIP, RSP\n";
    std::cout << "[x64] Execution model: Flat 64-bit address space with paging and higher-half kernel compatibility.\n";
    std::cout << "[x64] System tasks: large memory support, extended syscall interface, and modern runtime services.\n";
}
