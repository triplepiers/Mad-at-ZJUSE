//arch/riscv/kernel/proc.c
#include "proc.h"

extern void __dummy();

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组, 所有的线程都保存在此

void task_init() {
    // 1. 调用 kalloc() 为 idle 分配一个物理页
    // 2. 设置 state 为 TASK_RUNNING;
    // 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    // 4. 设置 idle 的 pid 为 0
    // 5. 将 current 和 task[0] 指向 idle

    /* YOUR CODE HERE */

    /* 使用 kalloc() 为 idle 分配物理页，关于 kalloc 函数的描述如下：
     * void *kalloc(void)
     * Allocate one 4096-byte page of physical memory.
     * Returns a pointer that the kernel can use.
     * Returns 0 if the memory cannot be allocated. */
    idle = (struct task_struct *)kalloc(); 
    
    // 将 idle 线程的状态设置为 TASK_RUNNING
    idle->state = TASK_RUNNING; 

    // 将 idle 线程的 counter & priority 置 0
    idle->counter = 0;
    idle->priority = 0;

    // 将 idle 线程的 pid 设为 0
    idle->pid = 0;

    // 将 current 和 task[0] 指向 idle
    current = idle;
    task[0] = idle;

    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
    // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址,  `sp` 设置为 该线程申请的物理页的高地址

    /* YOUR CODE HERE */

    // 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    for(int i = 1; i < NR_TASKS; i++) {
        task[i] = (struct task_struct *)kalloc(); // 调用 kalloc() 为 task[i] 分配一个物理页
        task[i]->state = TASK_RUNNING;            // 每个线程的 state 为 TASK_RUNNING
        task[i]->counter = 0;                     // 每个线程的 counter 为 0
        task[i]->priority = rand()%10 + 1;        // PRIORITY = [1, 10] 此处使用 rand() 进行设置
        task[i]->pid = i;                         // pid 为该线程在线程数组中的下标

        // 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`
        task[i]->thread.ra = (uint64)__dummy;     // `ra` 设置为 __dummy
        task[i]->thread.sp = (uint64)task[i] + 4096; // `sp` 设置为 该线程申请的物理页的高地址 = 低地址 + 4kB
    }

    printk("...proc_init done!\n");
}

void dummy() {
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            // output of Lab3
            // printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
            // output of lab4
            printk("[PID = %d] is running. thread space begin at 0x%lx\n", current->pid, (unsigned long)current);
        }
    }    
}


extern void __switch_to(struct task_struct* prev, struct task_struct* next);

void switch_to(struct task_struct* next) {
    /* YOUR CODE HERE */
    if(current != next) {// current 与 next 不属于同一进程，
        // 提示输出
        printk("\nswitch to [PID = %d", next->pid);
        #ifdef PRIORITY
            printk(" PRIORITY = %d", next->priority);
        #endif
        printk(" COUNTER = %d]\n", next->counter);
        
        // 由于 __switch_to 中仅实现了上下文切换，因此在此处对 current 指针进行处理
        struct task_struct* prev = current;
        current = next; 
        // 调用 __switch_to 进行线程切换
        __switch_to(prev, next);
    }
}

void do_timer(void) {   
   /* 如果当前线程是 idle 线程 直接进行调度 */
    if (current == idle) {
        printk("idle process is running!\n");
        schedule();
    }
    /* 如果当前线程不是 idle 对当前线程的运行剩余时间减 1 
       若剩余时间仍然大于0 则直接返回 否则进行调度 */
    else {
        current->counter--;
        if (current->counter > 0) {
            return;
        }
        else {
            schedule();
        }
    }
}

// 假定利用 rand() 重新分配后的 counter 属于区间 [1, 10]

void schedule(void) {
    /* YOUR CODE HERE */

    struct task_struct *next = current; // 符合条件的线程

    #ifdef SJF
        // 遍历指针数组 task[1] - task[NR_TASK-1]，选取所有 TASK_RUNNNING 状态下运行剩余时间最少的线程
        uint64 minn = 11; // 假定 max(counter) = 10
        while(1) {
            for(int i = 1; i < NR_TASKS; i++) {
                if(task[i]->state == TASK_RUNNING && task[i]->counter > 0 && task[i]->counter < minn) {
                    minn = task[i]->counter;
                    next = task[i];
                }
            }
            // 所有处于 TASK_RUNNING 状态下的线程 counter 均为零
            if(minn == 11) { 
                // 对 counter 进行重新赋值
                // printk("\n");
                for(int i = 1; i < NR_TASKS; i++) { 
                    task[i]->counter = rand()%(10) + 1; // counter 属于 [1, 10]
                    // printk("SET [PID = %d COUNTER = %d]\n", task[i]->pid, task[i]->counter);
                }
                // printk("\n");
            }
            else {
                break;
            }
        }

    #endif

    #ifdef PRIORITY
    uint64 maxx = 0; // 最高优先级
        while(1) {
            for(int i = 1; i < NR_TASKS; i++) {
                if(task[i]->state == TASK_RUNNING && task[i]->counter> 0 && task[i]->priority > maxx) {
                    maxx = task[i]->priority;
                    next = task[i];
                }
            }
            // 所有处于 TASK_RUNNING 状态下的线程 counter 均为零
            if(maxx == 0) {
                // 对 counter 进行重新赋值
                // printk("\n");
                for(int i = 1; i < NR_TASKS; i++) { 
                    task[i]->counter = rand()%10 + 1; // counter 属于 [1, 10]
                    // printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n", task[i]->pid, task[i]->priority, task[i]->counter);
                }
                // printk("\n");
            }
            else {
                break;
            }
        }

    #endif

    if(next != current) {
        switch_to(next);
    }

    return;
}