/**
 * Çekirdek içi görevler: zamanlayıcı ile paylaşılan adres uzayında çalışan
 * örnek C "programları". Gerçek kullanıcı modu yürütmesi için ayrı ELF yükleme gerekir.
 */

#include "kernel_types.h"
#include "sched.h"
#include "fs.h"

extern void printk(const char *fmt, ...);

static void demo_vfs_from_task(void)
{
    const char msg[] = "SimpleOS";
    s32 fd = vfs_open("/hello_demo.txt",
                      VFS_O_READ | VFS_O_WRITE,
                      0644);
    if (fd < 0) {
        printk("[INIT] VFS open failed (fd=%d)\n", fd);
        return;
    }

    s32 w = vfs_write(fd, msg, sizeof(msg) - 1);
    if (w < 0) {
        printk("[INIT] VFS write failed (%d)\n", w);
        vfs_close(fd);
        return;
    }

    inode_t st;
    if (vfs_fstat(fd, &st) == KERNEL_SUCCESS) {
        printk("[INIT] VFS demo: wrote %d bytes, inode size=%u\n", w, st.size);
    }

    vfs_close(fd);
}

void init_task(void)
{
    printk("\n[INIT] Init task started (PID %d)\n",
           sched_get_current()->pid);

    demo_vfs_from_task();

    printk("[INIT] Creating child tasks...\n");

    task_create(&worker_task_1, SCHED_PRIORITY_NORMAL);
    task_create(&worker_task_2, SCHED_PRIORITY_HIGH);

    printk("[INIT] Child tasks created, yielding CPU...\n");

    for (u32 i = 0; i < 1000000000U; i++) {
        asm volatile("nop");
    }

    printk("[INIT] Init task exiting\n");
    task_exit(0);
}

void worker_task_1(void)
{
    task_t *current = sched_get_current();
    printk("[WORKER-1] Task started (PID %d, priority %d)\n",
           current->pid, current->priority);

    for (int i = 0; i < 5; i++) {
        printk("[WORKER-1] Iteration %d\n", i);
        sched_yield();
    }

    printk("[WORKER-1] Task exiting\n");
    task_exit(0);
}

void worker_task_2(void)
{
    task_t *current = sched_get_current();
    printk("[WORKER-2] Task started (PID %d, priority %d)\n",
           current->pid, current->priority);

    for (int i = 0; i < 5; i++) {
        printk("[WORKER-2] Iteration %d\n", i);
        sched_yield();
    }

    printk("[WORKER-2] Task exiting\n");
    task_exit(0);
}
