#include <iostream>
#include <kernel/arch/x64/setup.h>
#include <kernel/arch/x64/idt.h>

int main()
{
    std::cout << "Setting up x64 architecture..." << std::endl;
    setup_idt();
    std::cout << "x64 architecture setup complete." << std::endl;
    return 0;
}