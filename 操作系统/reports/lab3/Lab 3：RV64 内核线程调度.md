# Lab 3：RV64 内核线程调度

---

​                姓名：沈韵沨   专业：软件工程   学号：3200104392   日期：2022年9月21日

---

## 1 实验目的

* 了解线程概念, 并学习线程相关结构体, 并实现线程的初始化功能。
* 了解如何使用时钟中断来实现线程的调度。
* 了解线程切换原理, 并实现线程的切换。
* 掌握简单的线程调度算法, 并完成两种简单调度算法的实现。

## 2 实验环境

* Ubuntu 22.04.1 LTS Windows Subsystem for Linux 2（Environment in previous labs）

## 3 实验步骤

### 3.1 准备工程

1. 从 `repo` 同步以下代码: `rand.h/rand.c`, `string.h/string.c`, `mm.h/mm.c`，并按照规定位置放置。

   ​	同步后的文件结构如下图所示，新增文件已经框出。

   <img src="imgs\tree.jpg" alt="tree" style="zoom:50%;" />

2. 由于在本次实验中引入了简单的物理内存管理, 需要在 `_start` 的适当位置调用 `mm_init`, 来初始化内存管理系统, 并且在初始化时需要用一些自定义的宏, 需要修改 `defs.h`, 在 `defs.h` 添加如下内容：

   ```c
   #define PHY_START 0x0000000080000000
   #define PHY_SIZE  128 * 1024 * 1024 // 128MB,  QEMU 默认内存大小
   #define PHY_END   (PHY_START + PHY_SIZE)
   
   #define PGSIZE 0x1000 // 4KB
   #define PGROUNDUP(addr) ((addr + PGSIZE - 1) & (~(PGSIZE - 1)))
   #define PGROUNDDOWN(addr) (addr & (~(PGSIZE - 1)))
   ```

3. 由于 `lib` 目录下新增了 `rand.c` 与 `string.c` 文件，此处调整 `lib/Makefile` 中的部分内容。

   ```makefile
   # lib/Makefile
   SRC = printk.c --> SRC = printk.c rand.c string.c
   OBJ = printk.o --> OBJ = printk.o rand.o string.o
   ```

   修改后执行 `make run` 命令，发现工程可正常运行。

4.  `arch/riscv/include/proc.h` 与 `arch/riscv/kernel/proc.c` 将在后续实现时再进行添加与修改。

### 3.2 `porc.h` 数据结构定义

* 添加 `arch/riscv/include/proc.h` 

  <img src="imgs\add_proc_h.jpg" alt="tree" style="zoom:50%;" />

* 将 `proc.h` 的内容修改如下：

  ```c
  // arch/riscv/include/proc.h
  
  #include "types.h"
  
  #define NR_TASKS  (1 + 31) // 用于控制 最大线程数量 （idle 线程 + 31 内核线程）
  
  #define TASK_RUNNING    0 // 为了简化实验, 所有的线程都只有一种状态
  
  #define PRIORITY_MIN 1
  #define PRIORITY_MAX 10
  
  /* 用于记录 `线程` 的 `内核栈与用户栈指针` */
  /* (lab3中无需考虑, 在这里引入是为了之后实验的使用) */
  struct thread_info {
      uint64 kernel_sp;
      uint64 user_sp;
  };
  
  /* 线程状态段数据结构 */
  struct thread_struct {
      uint64 ra;
      uint64 sp;
      uint64 s[12];
  };
  
  /* 线程数据结构 */
  struct task_struct {
      struct thread_info* thread_info;
      uint64 state;    // 线程状态
      uint64 counter;  // 运行剩余时间
      uint64 priority; // 运行优先级 1最低 10最高
      uint64 pid;      // 线程id
  
      struct thread_struct thread;
  };
  
  /* 线程初始化 创建 NR_TASKS 个线程 */
  void task_init();
  
  /* 在时钟中断处理中被调用 用于判断是否需要进行调度 */
  void do_timer();
  
  /* 调度程序 选择出下一个运行的线程 */
  void schedule();
  
  /* 线程切换入口函数*/
  void switch_to(struct task_struct* next);
  
  /* dummy funciton: 一个循环程序, 循环输出自己的 pid 以及一个自增的局部变量 */
  void dummy();
  ```

### 3.3 线程调度功能实现

#### 3.3.1 线程初始化

​	在这一环节中，我们需要添加 `arch/riscv/kernel/proc.c` 文件：

<img src="imgs\add_proc_c.jpg" alt="tree" style="zoom:50%;" />

​	参考 `Linux v0.11` 中的实现完成线程初始化，每个线程分配有一个 4kB 的物理页， `task_struct` 存放于该页的低地址部分，线程栈指针 `sp` 指向该页的高地址。具体内存布局如下：

```
                    ┌─────────────┐◄─── High Address
                    │             │
                    │    stack    │
                    │             │
                    │             │
              sp ──►├───────┬──────┤
                    │      │      │
                    │      ▼      │
                    │             │
                    │             │
                    │             │
                    │             │
    4KB Page        │             │
                    │             │
                    │             │
                    │             │
                    ├──────────────┤
                    │             │
                    │             │
                    │ task_struct │
                    │             │
                    │             │
                    └──────────────┘◄─── Low Address
```

* 首先，我们为  `idle` 设置 `task_struct`，并将 `current`, `task[0]` 都指向 `idle`。

  此处用于内存分配的函数 `void *kalloc(void)` 将分配一个大小为 4096Byte（即 4kB）的物理页，并返回一个可供内核使用的指针。

  ```c
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
  ```

* 随后，将 `task[1]` ~ `task[NR_TASKS - 1]` 全部初始化，区别于设置 `idle` ，我们要为这些线程设置 `thread_struct` 中的 `ra` 和 `sp`。该部分通过以下步骤实现：

  1. 参考 `idle` 的设置, 为 `task[1]` ~ `task[NR_TASKS - 1]` 进行初始化。
  2. 每个线程的 `state` 为 `TASK_RUNNING`；
     `counter` 为 `0`；
     `priority` 使用 `rand()` 来设置，且 $$priority \in [1, 10]$$；
     `pid` 为该线程在线程数组中的下标。
  3. 为` task[1]` ~ `task[NR_TASKS - 1]` 设置 `thread_struct` 中 `ra` 为 `__dummy` 的地址,  `sp` 设置为 该线程申请的物理页的高地址。

  ```c
  // 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
      for(int i = 1; i < NR_TASKS; i++) {
          // 调用 kalloc() 为 task[i] 分配一个物理页
          task[i] = (struct task_struct *)kalloc(); 
          // 每个线程的 state 为 TASK_RUNNING
          task[i]->state = TASK_RUNNING;
          // 每个线程的 counter 为 0
          task[i]->counter = 0;
          // PRIORITY = [1, 10] 此处使用 rand() 进行设置
          task[i]->priority = rand()%10 + 1;
          // pid 为该线程在线程数组中的下标
          task[i]->pid = i;                         
  
          // 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`
          // `ra` 设置为 __dummy
          task[i]->thread.ra = (uint64)__dummy;     
          // `sp` 设置为 该线程申请的物理页的高地址 = 低地址 + 4kB
          task[i]->thread.sp = (uint64)task[i] + 4096; 
      }
  ```

* 最后，在 `_start` 适当的位置调用 `task_init()`。

---

​	编写完成后的  `arch/riscv/kernel/proc.c` 内容如下：

```c
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
```

####  3.3.2 `__dummy` 与 `dummy` 介绍

​	首先，我们在 `proc.c` 中添加 `task[1]` ~ `task[NR_TASK-1]`通用的函数 `dummy()` ，内容如下：

```c
// arch/riscv/kernel/proc.c

void dummy() {
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            printk("[PID = %d] is running. auto_inc_local_var = %d\n",c current->pid, auto_inc_local_var);
        }
    }
}
```

​	当线程运行时，触发的时钟中断会将当前进程的上下文环境保存在栈上，并在线程再次被调度时从栈上恢复。但当我们创建一个新的进程时，线程的栈为空，不需要恢复上下文。

​	因此，我们在 `entry.S` 中为 `第一次调度` 的线程提供一个特殊的返回函数 `__dummy` ，用于将 `sepc` 设置为 `dummy()` 的地址，并使用 `sret`  从中断中返回。与 `_traps`  中的 `restore` 部分相比，`__dummy` 只是省略了从栈中恢复上下文的过程，但手动对 `sepc` 进行了设置。该部分补全后的逻辑如下：

```
# arch/riscv/kernel/entry.S
    .global __dummy
__dummy:
    # YOUR CODE HERE
    
    la t0, dummy       # 将 dummy() 的地址加载到寄存器 t0 中
    csrw sepc, t0      # 将 dummy() 的地址写入寄存器 sepc
    sret               # 使用 sret 从中断中返回
```

#### 3.3.3 实现线程切换

1. 在 `switch_to()` 函数中，我们首先需要判断下一个执行的线程 `next`  与当前的线程 `current` 是否为同一个线程 —— 若为同一线程，则无需做任何处理；反之，则调用 `__switch` 进行线程切换。补全后的 `switch_to()` 函数如下：

   ```c
   void switch_to(struct task_struct* next) {
       /* YOUR CODE HERE */
       if(current != next) {// current 与 next 不属于同一进程，
           // 调用 __switch_to 进行线程切换
           struct task_struct* prev = current;
           current = next;
           __switch_to(prev, next);
       }
       
       return;
   }
   ```

2. 在 `entry.S` 中实现上下文切换 `__switch_to` ，该函数：

   * 接受两个 `task_struct` 指针作为参数
   * 保存当前线程的 `ra`, `sp`, `s0~s11` 到当前线程的 `thread_struct` 中
   * 将下一个线程的 `thread_struct`  中的相关数据载入到 `ra`, `sp`, `s0~s11` 中

   该部分补全后的逻辑如下：

   ```
   # arch/riscv/kernel/entry.S
       .globl __switch_to
   __switch_to:
       # save state to prev process
       # YOUR CODE HERE
   
       # 保存当前线程的 `ra`, `sp`, `s0~s11` 到当前线程的 `thread_struct` 中
       # 因为 task_struct = thread_info -> state -> counter -> priority -> pid -> (thread_struct)thread
       # 所以 thread 的起始地址 = prev + 5*8 = prev + 40
       
       sd ra, 40(a0) # 保存当前线程的 ra
       sd sp, 48(a0) # 首先保存当前栈顶指针 sp
   
       # 依次保存 s[0] ~ s[11]
       sd s0, 56(a0)
       sd s1, 64(a0)
       sd s2, 72(a0)
       sd s3, 80(a0)
       sd s4, 88(a0)
       sd s5, 96(a0)
       sd s6, 104(a0)
       sd s7, 112(a0)
       sd s8, 120(a0)
       sd s9, 128(a0)
       sd s10, 136(a0)
       sd s11, 144(a0)
   
       # restore state from next process
       # YOUR CODE HERE
       # 按照与保存时相同的顺序，将下一个线程的 `thread_struct`  中的相关数据载入到 `ra`, `sp`, `s0~s11` 中
       ld ra, 40(a1) # 载入 ra
       ld sp, 48(a1) # 换栈
       # 依次载入 s[0] ~ s[11]
       ld s0, 56(a1)
       ld s1, 64(a1)
       ld s2, 72(a1)
       ld s3, 80(a1)
       ld s4, 88(a1)
       ld s5, 96(a1)
       ld s6, 104(a1)
       ld s7, 112(a1)
       ld s8, 120(a1)
       ld s9, 128(a1)
       ld s10, 136(a1)
       ld s11, 144(a1)
   
       ret
   ```

#### 3.3.4 实现调度入口函数

​	在本环节中，我们需要实现 `do_timer()` ，并在 `时钟中断处理函数` 中调用。在该函数中，我们应判断当前线程是否为 `dile`：

* 若为“是”，则直接调度。
* 若为“否”，则对当前线程的运行剩余时间 -1。
  若剩余时间仍 > 0， 则直接返回，否则进行调度。

​	该函数补全后的逻辑如下：

```c
// arch/riscv/kernel/proc.c

void do_timer(void) {
    // 1. 如果当前线程是 idle 线程 直接进行调度
    // 2. 如果当前线程不是 idle 对当前线程的运行剩余时间减1 若剩余时间仍然大于0 则直接返回 否则进行调度

    /* YOUR CODE HERE */
    if(current == idle) {
        schedule();            // 当前线程为 idle 线程，直接调用 schedule() 进行调度
    }
    else {
        current->counter -= 1; // 对当前线程运行时间 -1
        if(current->counter > 0) {
            return;            // 剩余时间大于零，直接返回
        }
        else {
            schedule();        // 剩余时间小于等于零，调用 schedule() 进行调度
        }
    }

    return;
}
```

#### 3.3.5 实现线程调度

##### 3.3.5.1 短作业优先调度算法

​	本算法按照以下规则进行调度：

* 遍历线程指针数组 `task` （不包括 `idle` ，即 `task[0]` ），在所有运行状态 （`TASK_RUNNING`）下的线程运行剩余时间 `最少` 的线程作为下一个执行的线程。
* 如果 `所有`  运行状态下的线程运行剩余时间都为 0 ，则对 `task[1]` ~ `task[NR_TASK-1]` 的运行剩余时间重新赋值（使用 `rand()` ）。之后再重新进行调度。

​	最短优先度调度算法实现如下：

```c
// arch/riscv/kernel/proc.c

void schedule(void) {
    /* YOUR CODE HERE */
    struct task_struct *next = current; // 符合条件的线程

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
            for(int i = 1; i < NR_TASKS; i++) { 
                task[i]->counter = rand()%(10) + 1; // counter 属于 [1, 10]
                printk("SET [PID = %d COUNTER = %d]\n", task[i]->pid, task[i]->counter);
            }
        }
        else {
            break;
        }
    }

    return;
}
```

##### 3.3.5.2 优先级调度算法

​	Linux v0.11 中的相关调度算法实现如下：

```c
void schedule(void)
{
	int i,next,c;
	struct task_struct ** p;

/* ... */

/* this is the scheduler proper: */

	while (1) {
		c = -1;
		next = 0;
		i = NR_TASKS;
		p = &task[NR_TASKS];
		while (--i) {
			if (!*--p)
				continue;
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
				c = (*p)->counter, next = i;
		}
		if (c) break;
		for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
			if (*p)
				(*p)->counter = ((*p)->counter >> 1) +
						(*p)->priority;
	}
	switch_to(next);
}
```

​	在该调度算法中 `idle` （即 `task[0]` ）将在没有其他任务可以运行时被调用。但它不能被 `kill` 或 `sleep` ，且 `task[0]` 中的 `state` 信息从未被使用。

​	优先度调度算法实现如下：

```c
// arch/riscv/kernel/proc.c

void schedule(void) {
    /* YOUR CODE HERE */
    struct task_struct *next = current; // 符合条件的线程

    while(1) {
        uint64 maxx = -1; // 最高优先级
        uint64 c = -1;    // TASK_RUNNING 状态下的 最大 count
        for(int i = 1; i < NR_TASKS; i++) {
            if(task[i]->state == TASK_RUNNING && task[i]->counter > c) {
                c= task[i]->counter;
                if(task[i]->piority > p) {
                    p = task[i]->piority;
                    next = task[i];
                }
            }
        }
        if(c) break; // 说明存在 counter>0 的运行态线程
        // 对 counter 进行重新赋值
        for(int i = 1; i < NR_TASKS; i++) { 
            task[i]->counter = rand()%(10) + 1; // counter 属于 [1, 10]
        }
    }

    if(next != current) {
        switch_to(next);
    }

    return;

}
```

### 3.4 编译及测试

​	在本次实验中完成了两个调度算法，在编译时，我们可以`gcc -D DCJF/DPRIORITY` 选项进行相应的宏定义，使得相应的代码被成功执行：

* 在 `proc.c` 中，我们使用 `#ifdef DCJF/DPRIORITY` + `#endif` 来控制代码。
* 在顶层Makefile 文件中，我们将相应选项修改为`CFLAG = ${CF} ${INCLUDE} -DSJF/DPRIORITY`
* 此外，我们根据输出示例中打印的信息为相应函数添加了 `printk()` 语句。

​	采用两种不同调度方式的代码运行结果如下（为便于展示，此处的 `NR_TASK` 设为 4）：

#### 短作业优先调度 DSJF 输出

<img src="D:\OSReports\lab3_report\imgs\SJF.jpg" alt="SJF" style="zoom:33%;"/>

#### 优先级调度 DPRIORITY 输出

<img src="D:\OSReports\lab3_report\imgs\PRIORITY.jpg" alt="SJF" style="zoom:33%;"/>

## 4 思考题

1. **在 RV64 中一共用 32 个通用寄存器, 为什么 `context_switch` 中只保存了14个 ？**

   ​	因为在本次实验中，线程切换是通过调用函数实现的，`Caller Saved Register`  将被保存到当前的栈上。

   ​	而在 `context_switch` 时，需要将就进程的相关信息入栈，且需要保存返回地址，因此在相关函数中不仅需要保存 `Callee Saved Register` ，还需要保存 `ra` 寄存器（返回地址）与 `sp`  寄存器（栈指针），共计保存 14 个寄存器。

2. **当线程第一次调用时, 其 `ra` 所代表的返回点是 `__dummy`。 那么在之后的线程调用中 `context_switch` 中, `ra` 保存/恢复的函数返回点是什么呢 ？ 请同学用 gdb 尝试追踪一次完整的线程切换流程, 并关注每一次 `ra` 的变换 (需要截图)。**

   ​	为便于观察线程第一次调用与后续调用时 `ra` 寄存器中返回点的区别，此处暂时将 `NR_TASK`  的值更改为 3，即：除 `idle` 线程外，只有两个 `task` 。

   1. 输入 `make debug` 命令，使用 `QEMU` + `gdb` 对程序进行调试。
   2. 使用 `disassemble __switch_to` 命令查看函数 `__switch_to` 的汇编代码，结果如下：
      可以发现 `ra` 的保存与恢复分别在地址 `0x80200160` 与 ` 0x80200198` 处。

   <img src="imgs\dis_switch.jpg" alt="SJF" style="zoom:33%;"/>

   3. 分别在上述两处地址打上断点，此时的断点信息如下：

<img src="imgs\i_b.jpg" alt="SJF" style="zoom:50%;"/>

   				4. 首次调度时，`ra` 中保存和恢复的函数返回点为：`switch_to+132` - `__dummy`

<img src="imgs\ra_init.jpg" alt="SJF" style="zoom:50%;"/>

​			后续调度中，`ra` 中保存和恢复的函数返回点为：`switch_to+132`（调用 `__switch_to` 的下一行）

<img src="imgs\ra_then.jpg" alt="SJF" style="zoom:50%;"/>

​			`switch_to+132`为调用 `__switch_to` 的下一行，具体内容如下：

<img src="imgs\+132.jpg" alt="SJF" style="zoom:50%;"/>

## 5 心得体会

​	在编写优先级调度算法时，由于需要选出 `priority` 最大的 `task` ，在初始化 `maxx` 变量时将其赋为了 -1，然后发现无论如何都不能进入 IF 语句。定睛一看 `maxx` 的变量类型是 `uint64` （为了便于与 `task_struct` 的分量进行比较），在比较时这个 -1 变成了比 `PRIORITY_MAX` 还要"大"很多的值。最后将 `maxx` 的初始值改成了 0（虽然这给后续的讨论带来了一丝丝的不便）。

​	然后是在 `gdb` 调试阶段的一堆坑：

* 首先尝试了直接用 `gdb` 跑（显然不可能跑通），报了很诡异的错误 `internal-error: i387_supply_fxsave: Assertion 'tdep->st0_regnum >= I386_ST0_REGNUM' failed.`，弃之。

* 转战了 `qemu-user`，然而直接跪在了安装上（怒而换清华源） ，然后被 `Segmentation fault` 迎头痛击。

* 最后翻了翻根目录下的 Makefile，找到了`make debug` 。大喜，跑之。又被 `No symbol table loaded.  Use the "file" command.` 暴捶（孩子打不了断点啦）。回去重新进修了一遍 Makefile 语法和 gcc 编译选项，然后发现 `CF` 变量后已经存在 `-g` 选项了（怎会如此）。最后十分迷惑的在用 `gdb-multiarch` 进行远程调试时，使用 `file vmlinux` 重新加载文件后解决了。

​	无论如何，本次实验使我对于线程间的切换过程有了更为深刻的理解（感谢开头的字符艺术创作），同时对简单的进程调度算法的实现有了明确的认知，收获良多。
