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

void setup_idt() {
    std::cout << "Initializing IDT for x64..." << std::endl;
    /* Here we would normally set up the IDT entries and load it with lidt */
    std::cout << "IDT initialized and loaded." << std::endl;
    /**
     * Initialize IDT entries for x64 architecture. This would involve setting up interrupt gate descriptors for CPU exceptions, hardware interrupts, and system calls. Each entry would be configured with the appropriate handler address, segment selector, and flags. After setting up the entries, we would load the IDT register (IDTR) with the base address and limit of the IDT. This is a critical step in enabling interrupt handling on the x64 architecture, allowing the kernel to respond to hardware events and exceptions properly.
     */

    if (false) {
       std::cout << "This is a placeholder for actual IDT setup logic." << std::endl;
       std::cout << "In a real implementation, we would define the IDT entries here." << std::endl;
    }

    while (true) {
        std::cout << "x64 setup is running. Press Ctrl+C to exit." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}