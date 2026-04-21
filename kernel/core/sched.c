/**
 * SimpleOS Kernel - Process Scheduler Implementation
 * Round-robin multitasking with priority-based scheduling
 */

#include "sched.h"
#include "mm.h"
#include <string.h>

static scheduler_t kernel_scheduler;
static pid_t next_pid = 1;

/* ============================================================================
 * SCHEDULER INITIALIZATION
 * =========================================================================== */

void sched_init(void) {
    kernel_scheduler.current_task = NULL;
    kernel_scheduler.total_tasks = 0;
    kernel_scheduler.context_switches = 0;
    kernel_scheduler.total_cpu_time = 0;
    
    /* Initialize priority queues */
    for (int i = 0; i < 5; i++) {
        list_init(&kernel_scheduler.run_queue[i]);
    }
}

/* ============================================================================
 * TASK CREATION
 * =========================================================================== */

pid_t task_create(void (*entry_point)(void), sched_priority_t priority) {
    if (priority >= 5) {
        return KERNEL_EINVAL;
    }
    
    /* Allocate task control block */
    task_t *task = (task_t *)kmalloc(sizeof(task_t));
    if (!task) {
        return KERNEL_ENOMEM;
    }
    
    /* Initialize TCB */
    memset(task, 0, sizeof(task_t));
    task->pid = next_pid++;
    task->priority = priority;
    task->state = TASK_STATE_CREATED;
    task->time_slice = 20;      /* 20ms time quantum */
    task->cpu_ticks = 0;
    task->status = 0;
    task->flags = 0;
    
    /* Setup CPU context */
    memset(&task->context, 0, sizeof(cpu_context_t));
    task->context.eip = (u32)entry_point;
    task->context.esp = 0x1000;         /* Stack pointer */
    task->context.ebp = 0x1000;
    task->context.eflags = 0x0200;      /* IE flag set */
    task->context.cs = 0x08;
    task->context.ds = task->context.es = task->context.fs = 
    task->context.gs = task->context.ss = 0x10;
    
    /* Initialize lists */
    list_init(&task->children);
    
    /* Add to appropriate run queue */
    list_add(&kernel_scheduler.run_queue[priority], &task->sched_node);
    task->state = TASK_STATE_READY;
    kernel_scheduler.total_tasks++;
    
    return task->pid;
}

/* ============================================================================
 * TASK STATE MANAGEMENT
 * =========================================================================== */

void task_set_state(task_t *task, task_state_t state) {
    if (!task) return;
    
    task->state = state;
    
    switch (state) {
        case TASK_STATE_READY:
            /* Add back to run queue */
            if (task->sched_node.next == NULL) {
                list_add(&kernel_scheduler.run_queue[task->priority], 
                        &task->sched_node);
            }
            break;
            
        case TASK_STATE_BLOCKED:
        case TASK_STATE_SUSPENDED:
            /* Remove from run queue */
            list_remove(&task->sched_node);
            break;
            
        case TASK_STATE_RUNNING:
            /* Already in run queue, just mark as running */
            break;
            
        case TASK_STATE_ZOMBIE:
            /* Will be cleaned up by parent */
            list_remove(&task->sched_node);
            break;
            
        default:
            break;
    }
}

/* ============================================================================
 * SCHEDULER TICK (Called by timer interrupt)
 * =========================================================================== */

void sched_tick(void) {
    task_t *current = kernel_scheduler.current_task;
    
    if (current) {
        current->cpu_ticks++;
        kernel_scheduler.total_cpu_time++;
        
        /* Check if time slice expired */
        if (current->cpu_ticks >= current->time_slice) {
            current->cpu_ticks = 0;
            
            /* Move to back of its priority queue for round-robin */
            if (current->state == TASK_STATE_RUNNING) {
                task_set_state(current, TASK_STATE_READY);
                task_set_state(current, TASK_STATE_READY);
            }
        }
    }
    
    sched_yield();
}

/* ============================================================================
 * SCHEDULER - NEXT TASK SELECTION
 * =========================================================================== */

task_t* sched_pick_next(void) {
    /* Find highest priority task in READY state */
    for (int priority = 4; priority >= 0; priority--) {
        list_node_t *node;
        list_for_each(node, &kernel_scheduler.run_queue[priority]) {
            task_t *task = list_entry(node, task_t, sched_node);
            if (task->state == TASK_STATE_READY) {
                return task;
            }
        }
    }
    
    /* Fallback to idle */
    return NULL;
}

void sched_yield(void) {
    task_t *prev = kernel_scheduler.current_task;
    task_t *next = sched_pick_next();
    
    if (!next) {
        /* No ready tasks - loop in idle */
        return;
    }
    
    if (prev == next) {
        return;
    }
    
    /* Perform context switch */
    if (prev && prev->state == TASK_STATE_RUNNING) {
        task_set_state(prev, TASK_STATE_READY);
    }
    
    next->state = TASK_STATE_RUNNING;
    kernel_scheduler.current_task = next;
    kernel_scheduler.context_switches++;
    
    /* TODO: Actual context switch assembly code here */
    /* This would:
     * 1. Save prev task's context (registers, etc)
     * 2. Restore next task's context
     * 3. Switch page directory if different address space
     */
}

/* ============================================================================
 * TASK MANAGEMENT
 * =========================================================================== */

task_t* sched_get_current(void) {
    return kernel_scheduler.current_task;
}

void task_exit(s32 exit_code) {
    task_t *current = kernel_scheduler.current_task;
    if (!current) {
        while (1) asm("hlt");  /* Halt */
    }
    
    current->status = exit_code;
    current->state = TASK_STATE_ZOMBIE;
    
    /* TODO: Clean up resources */
    
    sched_yield();
    
    /* Should not reach here */
    while (1) asm("hlt");
}

void task_sleep(u32 ms) {
    task_t *current = kernel_scheduler.current_task;
    if (current) {
        current->state = TASK_STATE_BLOCKED;
        /* TODO: Set timer for wake-up */
        sched_yield();
    }
}

void task_wake(task_t *task) {
    if (task && task->state == TASK_STATE_BLOCKED) {
        task_set_state(task, TASK_STATE_READY);
    }
}

task_t* task_get_by_pid(pid_t pid) {
    /* Search all priority queues */
    for (int priority = 0; priority < 5; priority++) {
        list_node_t *node;
        list_for_each(node, &kernel_scheduler.run_queue[priority]) {
            task_t *task = list_entry(node, task_t, sched_node);
            if (task->pid == pid) {
                return task;
            }
        }
    }
    return NULL;
}

pid_t task_waitpid(pid_t pid, s32 *status) {
    task_t *child = task_get_by_pid(pid);
    
    if (!child) {
        return KERNEL_ENOENT;
    }
    
    /* Wait for child to become zombie */
    while (child->state != TASK_STATE_ZOMBIE) {
        task_sleep(100);
    }
    
    if (status) {
        *status = child->status;
    }
    
    /* Clean up zombie task */
    kfree(child);
    return pid;
}

/* ============================================================================
 * DIAGNOSTICS
 * =========================================================================== */

void sched_dump_state(void) {
    printk("=== SCHEDULER STATE ===\n");
    printk("Total tasks: %d\n", kernel_scheduler.total_tasks);
    printk("Context switches: %d\n", kernel_scheduler.context_switches);
    printk("Total CPU time: %lld ticks\n", kernel_scheduler.total_cpu_time);
    
    if (kernel_scheduler.current_task) {
        printk("Current task: PID %d (priority %d)\n",
               kernel_scheduler.current_task->pid,
               kernel_scheduler.current_task->priority);
    }
    
    printk("\nTasks by priority:\n");
    for (int p = 4; p >= 0; p--) {
        list_node_t *node;
        list_for_each(node, &kernel_scheduler.run_queue[p]) {
            task_t *task = list_entry(node, task_t, sched_node);
            printk("  PID %d: priority=%d state=%d cpu_time=%d\n",
                   task->pid, task->priority, task->state, task->cpu_ticks);
        }
    }
}
