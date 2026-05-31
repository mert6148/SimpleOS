#include "system.h"
#include <cstring>
#include <iostream>

static void print_usage(const char *program_name) {
    std::cout << "Usage: " << program_name << " [--info-only] [--demo-only] [--help]" << std::endl;
    std::cout << "  --info-only   Show system description without running the demo." << std::endl;
    std::cout << "  --demo-only   Run the architecture demo with minimal info." << std::endl;
    std::cout << "  --help        Show this help message." << std::endl;
}

int main(int argc, char **argv) {
    system_info info = get_system_info();
    bool show_info = true;
    bool show_demo = true;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (std::strcmp(argv[i], "--info-only") == 0) {
            show_demo = false;
            continue;
        }
        if (std::strcmp(argv[i], "--demo-only") == 0) {
            show_info = false;
            continue;
        }
    }

    if (show_info) {
        std::cout << "SimpleOS System Prototype\n";
        std::cout << "Target architecture: " << info.architecture << "\n";
        std::cout << "Pointer width: " << info.pointer_width << "\n";
        std::cout << info.description << "\n\n";
    }

    if (show_demo) {
        run_system_demo();
    }

    return 0;
}
