//arch/riscv/kernel/proc.c
#include "proc.h"

extern void __dummy();

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组, 所有的线程都保存在此

void task_init() {
    // printk("\n\ntask_init() begin\n");
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
        // printk("%s%d\n","begin ini tsak_",i);
        task[i] = (struct task_struct *)kalloc(); // 调用 kalloc() 为 task[i] 分配一个物理页
        task[i]->state = TASK_RUNNING;            // 每个线程的 state 为 TASK_RUNNING
        task[i]->counter = 0;                     // 每个线程的 counter 为 0
        task[i]->priority = rand()%10 + 1;        // PRIORITY = [1, 10] 此处使用 rand() 进行设置
        task[i]->pid = i;                         // pid 为该线程在线程数组中的下标

        // 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`
        task[i]->thread.ra = (uint64)__dummy;     // `ra` 设置为 __dummy
        task[i]->thread.sp = (uint64)task[i] + 4096; // `sp` 设置为 该线程申请的物理页的高地址 = 低地址 + 4kB

        // 通过 kalloc 接口申请一个空的页表作为 U-Mode Stack
        task[i]->thread_info = (struct thread_info*)kalloc(); // 给 thread_info 分配空间
        task[i]->thread_info->kernel_sp = (uint64)(task[i]) + 4096;
        task[i]->thread_info->user_sp = kalloc();

        // 为用户态进程创建自己的页表，并将 uapp 所在界面 以及 U-Mode Stack 做相应映射
        pagetable_t tbl = (pagetable_t)kalloc();

        // 1. 为避免 U/S-Mode 切换时切换页表，将内核页表 swapper_pg_dir 复制到进程页表中

        char* dst = (char*)tbl;
        char* src = (char*)swapper_pg_dir;
        for(int i = 0 ; i < 4096; i++) {
            dst[i] = src[i];
        }

        // 2. 创建 uapp 的映射，权限为 U|X|W|R|V = 11111(b) = 31
        uint64 pa = (uint64)uapp_start - PA2VA_OFFSET;
        uint64 va = USER_START;
        create_mapping(tbl, va, pa, (uint64)uapp_end - (uint64)uapp_start, 31);

        // 3. 将 U-Mode Stack 映射到用户空间的最后一个页面，权限为 U|-|W|R|V = 10111(b) = 23
        pa = task[i]->thread_info->user_sp - PA2VA_OFFSET; // U-Mode Stack 的起始位置
        va = USER_END - 4096;                              // 用户空间最后一页的起始位置
        create_mapping(tbl, va, pa, 4096, 23);

        // 4. 将映射完成的页表存入 tsak[i]->pgt
        uint64 satp = csr_read(satp);
        // 保留 MODE 和 ASID 取值（63~44)，顶级页表物理页号由 (虚拟地址-偏移量)>>12 计算得出（PGSIZE = 2^12）
        satp = ((satp >> 44) << 44) | (((uint64)tbl - PA2VA_OFFSET) >> 12);
        task[i]->pgd = satp;

        // 将 sepc 寄存器修改为 USER_START
        task[i]->thread.sepc = USER_START;

        // 设置 sstatus 寄存器
        uint64 sstatus = csr_read(sstatus);
        // SPP = 0
        sstatus = sstatus & 0xfffffffffffffeff;
        // SPIE = 1
        sstatus = sstatus | (1<<5);
        // SUM = 1
        sstatus = sstatus | (1<<18);

        task[i]->thread.sstatus = sstatus;
        
        // 设置 sscratch 为 U-Mode 的 sp（USER_END)
        task[i]->thread.sscratch = USER_END;
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
        // printk("\nswitch to [PID = %d", next->pid);
        #ifdef PRIORITY
            printk(" PRIORITY = %d", next->priority);
        #endif
        // printk(" COUNTER = %d]\n", next->counter);
        printk("\n");
        
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