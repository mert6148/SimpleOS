/**
 * SimpleOS Kernel - Process Management (Task Scheduler)
 * Implements multitasking, process scheduling, and context switching
 */

#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include "kernel_types.h"

/* ============================================================================
 * PROCESS/TASK STATES
 * =========================================================================== */

typedef enum {
    TASK_STATE_CREATED = 0,     /* Task just created */
    TASK_STATE_READY = 1,       /* Ready to run */
    TASK_STATE_RUNNING = 2,     /* Currently executing */
    TASK_STATE_BLOCKED = 3,     /* Waiting for I/O or event */
    TASK_STATE_ZOMBIE = 4,      /* Terminated but not cleaned up */
    TASK_STATE_SUSPENDED = 5    /* Paused by parent */
} task_state_t;

typedef enum {
    SCHED_PRIORITY_IDLE = 0,
    SCHED_PRIORITY_LOW = 1,
    SCHED_PRIORITY_NORMAL = 2,
    SCHED_PRIORITY_HIGH = 3,
    SCHED_PRIORITY_REALTIME = 4
} sched_priority_t;

/* ============================================================================
 * CPU CONTEXT (Register State)
 * =========================================================================== */

typedef struct {
    /* General purpose registers (x86) */
    u32 eax, ebx, ecx, edx;
    u32 esi, edi, ebp, esp;
    
    /* Segment registers */
    u16 cs, ds, es, fs, gs, ss;
    
    /* Special registers */
    u32 eip;        /* Instruction pointer */
    u32 eflags;     /* Flags register */
    
    /* FPU/extensions (simplified) */
    u64 fpu_state[16];
} cpu_context_t;

/* ============================================================================
 * TASK CONTROL BLOCK (TCB)
 * =========================================================================== */

typedef struct task {
    pid_t pid;                          /* Process ID */
    s32 status;                         /* Return status */
    u32 flags;                          /* Task flags */
    
    /* Scheduling */
    task_state_t state;                 /* Current state */
    sched_priority_t priority;          /* Scheduling priority */
    u32 time_slice;                     /* Time quantum for round-robin */
    u32 cpu_ticks;                      /* CPU time used */
    
    /* Memory */
    addr_t page_directory;              /* Page directory address */
    u32 memory_used;                    /* Memory usage in KB */
    
    /* Context */
    cpu_context_t context;              /* CPU registers state */
    
    /* Linked list for scheduler */
    list_node_t sched_node;
    
    /* Parent/Child relationships */
    pid_t parent_pid;
    list_t children;
} task_t;

/* ============================================================================
 * SCHEDULER STATE
 * =========================================================================== */

typedef struct {
    task_t *current_task;               /* Currently running task */
    list_t run_queue[5];                /* Run queues by priority */
    u32 total_tasks;
    u32 context_switches;
    u64 total_cpu_time;
} scheduler_t;

/* ============================================================================
 * SCHEDULER INTERFACE
 * =========================================================================== */

/**
 * Initialize the scheduler
 */
void sched_init(void);

/**
 * Create a new task/process
 * @param entry_point Function to execute
 * @param priority Task priority level
 * @return Task PID or error code
 */
pid_t task_create(void (*entry_point)(void), sched_priority_t priority);

/**
 * Terminate current task
 * @param exit_code Exit status
 */
void task_exit(s32 exit_code) NORETURN;

/**
 * Get currently running task
 * @return Pointer to current task TCB
 */
task_t* sched_get_current(void);

/**
 * Perform context switch to next task
 * Called by timer interrupt
 */
void sched_tick(void);

/**
 * Change task state
 */
void task_set_state(task_t *task, task_state_t state);

/**
 * Yield CPU to next ready task
 */
void sched_yield(void);

/**
 * Put task to sleep (block until condition)
 */
void task_sleep(u32 ms);

/**
 * Wake up a sleeping task
 */
void task_wake(task_t *task);

/**
 * Wait for child task to complete
 */
pid_t task_waitpid(pid_t pid, s32 *status);

/**
 * Get task by PID
 */
task_t* task_get_by_pid(pid_t pid);

/**
 * Dump scheduler state (debugging)
 */
void sched_dump_state(void);

#endif /* KERNEL_SCHED_H */
