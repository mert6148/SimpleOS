#include <stdio.h>
#include <string.h>

static void print_help(const char *prog) {
    printf("SimpleOS x86 System Demo\n");
    printf("Usage: %s [--help] [--info-only] [--demo-only]\n", prog);
    printf("  --help       Show this help text.\n");
    printf("  --info-only  Print architecture and build metadata only.\n");
    printf("  --demo-only  Run the x86 demo logic only.\n");
}

int main(const std::vector<std::string> &args) {
    std::cout << "Hello from the x86 system demo!" << std::endl;
    std::cout << "This is a simple C++ program running on the x86 architecture of SimpleOS." << std::endl;
    std::cout << "Feel free to modify and expand it as you like." << std::endl;
    return 0;
}