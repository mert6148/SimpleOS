/**
 * Çekirdek içi görevler: zamanlayıcı ile paylaşılan adres uzayında çalışan
 * örnek C "programları". Bu dosya, scheduler, VFS ve hafıza tahsisini
 * gösteren küçük demo kodu içerir.
 */

#include "kernel_types.h"
#include "sched.h"
#include "mm.h"
#include "fs.h"
#include <string.h>

extern void printk(const char *fmt, ...);

static void demo_vfs_from_task(void)
{
    const char msg[] = "SimpleOS demo file contents";
    s32 fd = vfs_open("/hello_demo.txt",
                      VFS_O_READ | VFS_O_WRITE | VFS_O_CREAT,
                      0644);
    if (fd < 0) {
        printk("[INIT] VFS open failed (fd=%d)\n", fd);
        return;
    }

    s32 written = vfs_write(fd, msg, sizeof(msg) - 1);
    if (written < 0) {
        printk("[INIT] VFS write failed (%d)\n", written);
        vfs_close(fd);
        return;
    }

    inode_t st;
    if (vfs_fstat(fd, &st) == KERNEL_SUCCESS) {
        printk("[INIT] VFS demo: wrote %d bytes, inode size=%u\n",
               written, st.size);
    }

    vfs_close(fd);
}

static void demo_memory_allocation(void)
{
    printk("[INIT] Memory demo: allocating small buffers...\n");

    void *block1 = kmalloc(64);
    if (block1) {
        memset(block1, 0x41, 64);
        printk("[INIT] Allocated 64 bytes at %p\n", block1);
        kfree(block1);
        printk("[INIT] Freed 64-byte block\n");
    } else {
        printk("[INIT] Memory demo failed: kmalloc returned NULL\n");
    }
}

void init_task(void)
{
    printk("\n[INIT] Init task started (PID %d)\n",
           sched_get_current()->pid);

    demo_vfs_from_task();
    demo_memory_allocation();

    printk("[INIT] Creating worker tasks...\n");
    task_create(&worker_task_1, SCHED_PRIORITY_NORMAL);
    task_create(&worker_task_2, SCHED_PRIORITY_HIGH);
    task_create(&monitor_task, SCHED_PRIORITY_LOW);

    printk("[INIT] Child tasks created, yielding CPU to worker tasks...\n");
    for (int i = 0; i < 5; i++) {
        sched_yield();
    }

    printk("[INIT] Init task exiting\n");
    task_exit(0);
}

void worker_task_1(void)
{
    task_t *current = sched_get_current();
    printk("[WORKER-1] Started (PID %d, priority %d)\n",
           current->pid, current->priority);

    for (int i = 0; i < 4; i++) {
        printk("[WORKER-1] Doing work cycle %d\n", i);
        sched_yield();
    }

    printk("[WORKER-1] Exiting\n");
    task_exit(0);
}

void worker_task_2(void)
{
    task_t *current = sched_get_current();
    printk("[WORKER-2] Started (PID %d, priority %d)\n",
           current->pid, current->priority);

    for (int i = 0; i < 6; i++) {
        printk("[WORKER-2] Doing work cycle %d\n", i);
        sched_yield();
    }

    printk("[WORKER-2] Exiting\n");
    task_exit(0);
}

void monitor_task(void)
{
    task_t *current = sched_get_current();
    printk("[MONITOR] Started (PID %d, priority %d)\n",
           current->pid, current->priority);

    for (int i = 0; i < 8; i++) {
        printk("[MONITOR] Checking system state, cycle %d\n", i);
        sched_yield();
    }

    printk("[MONITOR] Exiting\n");
    task_exit(0);
}
