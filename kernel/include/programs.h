/**
 * SimpleOS — çekirdek içi örnek görevler (C ile "program" giriş noktaları)
 */

#ifndef KERNEL_PROGRAMS_H
#define KERNEL_PROGRAMS_H

void init_task(void);
void worker_task_1(void);
void worker_task_2(void);
void monitor_task(void);

#endif /* KERNEL_PROGRAMS_H */
