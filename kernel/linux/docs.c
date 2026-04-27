#include <include/linux/module.h>
#include <include/linux/kernel.h>
#include <include/linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux kernel module to demonstrate documentation.");
MODULE_VERSION("1.0");
static int __init docs_init(void) {
    printk(KERN_INFO "Docs module loaded: Hello, Kernel!\n");
    return 0; // Return 0 for successful initialization
}

static void __exit docs_exit(void) {
    printk(KERN_INFO "Docs module exiting: Goodbye, Kernel!\n");
    void; // No return value for exit function
}