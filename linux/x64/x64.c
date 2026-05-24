#include <iostream>
#include <string>
#include <vector>

static void print_help(const char *prog) {
    std::cout << "SimpleOS x64 System Demo\n";
    std::cout << "Usage: " << prog << " [--help] [--info-only] [--demo-only]" << std::endl;
}

static double x64_compute(double a, double b) {
    return a * b + (a - b) / 2.0;
}

int main(int argc, char **argv) {
    bool show_info = true;
    bool show_demo = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            print_help(argv[0]);
            return 0;
        }
        if (arg == "--info-only") {
            show_demo = false;
        }
        if (arg == "--demo-only") {
            show_info = false;
        }
    }

    if (show_info) {
        std::cout << "SimpleOS Linux x64 System" << std::endl;
        std::cout << "Architecture: x64" << std::endl;
        std::cout << "Pointer width: 64-bit" << std::endl;
        std::cout << "Build type: linux/x64 system demo" << std::endl;
        std::cout << std::endl;
    }

    if (show_demo) {
        std::cout << "[x64] Running simple kernel-style demo..." << std::endl;
        std::cout << "[x64] Register set: RAX RBX RCX RDX R8 R9 R10 R11 R12 R13 R14 R15 RIP RSP" << std::endl;
        std::cout << "[x64] Emulated memory model: 64-bit flat address space with modern paging semantics." << std::endl;
        std::cout << "[x64] Example compute result: " << x64_compute(4.5, 2.0) << std::endl;
    }

    return 0;
}
