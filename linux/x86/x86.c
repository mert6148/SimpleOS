#include <stdio.h>
#include <string.h>

static void print_help(const char *prog) {
    printf("SimpleOS x86 System Demo\n");
    printf("Usage: %s [--help] [--info-only] [--demo-only]\n", prog);
    printf("  --help       Show this help text.\n");
    printf("  --info-only  Print architecture and build metadata only.\n");
    printf("  --demo-only  Run the x86 demo logic only.\n");
}

int main(int argc, char **argv) {
    int show_info = 1;
    int show_demo = 1;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "--info-only") == 0) {
            show_demo = 0;
        }
        if (strcmp(argv[i], "--demo-only") == 0) {
            show_info = 0;
        }
    }

    if (show_info) {
        printf("SimpleOS Linux x86 System\n");
        printf("Architecture: x86\n");
        printf("Pointer width: 32-bit\n");
        printf("Build type: linux/x86 system demo\n\n");
    }

    if (show_demo) {
        printf("[x86] Running simple kernel-style demo...\n");
        printf("[x86] Simulated registers: EAX EBX ECX EDX ESI EDI EBP ESP\n");
        printf("[x86] Emulated memory model: 32-bit flat address space with legacy compatibility.\n");
    }

    return 0;
}
