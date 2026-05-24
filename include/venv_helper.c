#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const char *venv = getenv("VIRTUAL_ENV");
    printf("VIRTUAL_ENV=%s\n", venv ? venv : "(not set)");

    const char *path = getenv("PATH");
    printf("PATH=%s\n", path ? path : "(not set)");

    printf("\nPython availability check:\n");
    int status = system("python --version");
    if (status == -1)
    {
        perror("system");
        return 1;
    }

    printf("\nIf you want to create a venv from C, run:\n");
    printf("    python -m venv venv\n");
    return 0;
}
