# Lab 5：RV64用户态程序

---

​                姓名：沈韵沨   专业：软件工程   学号：3200104392   日期：2022年11月30日

---

## 1 实验目的

* 建用户态进程，并设置 `sstatus` 来完成内核态转换至用户态。
* 正确设置用户进程的**用户态栈**和**内核态栈**， 并在异常处理时正确切换。
* 补充异常处理逻辑，完成指定的系统调用（ SYS_WRITE, SYS_GETPID ）功能。

## 2 实验环境

* Ubuntu 22.04.1 LTS Windows Subsystem for Linux 2（Environment in previous labs）

## 3 实验步骤

### 3.1 准备工程

1. 为了将用户态程序 `uapp` 加载至 `.data` 段，我们对 `vmlinux.lds.S` 做如下修改：

   ```
   // arch/riscv/kernel/vmlinux.lds.S
   ...
   
   .data : ALIGN(0x1000){
           _sdata = .;
   
           *(.sdata .sdata*)
           *(.data .data.*)
   
           _edata = .;
           
           . = ALIGN(0x1000);
           uapp_start = .;
           *(.uapp .uapp*)
           uapp_end = .;
           . = ALIGN(0x1000);
   
       } >ramv AT>ram
   
   ...
   ```

2. 此外，在 `defs.h` 中添加如下内容：

   ```c
   // arch/riscv/include/defs.h
   
   #define USER_START (0x0000000000000000) // user space start virtual address
   #define USER_END   (0x0000004000000000) // user space end virtual address
   ```

3. 从仓库中同步文件 `user` 与  `Makefile`，并按照指定路径放置。完成后的文件结构如下图所示：

   <img src=".\imgs\tree.jpg" alt="tree" style="zoom:33%;" />

4. 对根目录下的 Makefile 进行如下修改，以便将 `user` 纳入工程管理：

   ```makefile
   # Makefile
   ...
   all:
   	...
   	${MAKE} -C user all
   ...
   clean:
   	...
   	${MAKE} -C user clean 
   ...
   ```

### 3.2 创建用户态进程

1. 修改 `proc.h` 

   * 将 `NR_TASKS`  修改为 `5` （本次实验只需要创建 4 个用户态进程）

   * 由于创建用户态进程要对 `sepc` `sstatus` `sscratch` 做设置，我们将其加入 `thread_struct` 中；又因为多个用户态进程需要保证相对隔离，因此不可以共用页表。我们为每个用户态进程都创建一个页表。修改 `task_struct` 如下：

   ```c
   // arch/riscv/include/proc.h
   typedef unsigned long* pagetable_t;
   
   struct thread_struct {
       uint64 ra;
       uint64 sp;                     
       uint64 s[12];
   
       uint64 sepc, sstatus, sscratch; 
   };
   
   struct task_struct {
       struct thread_info* thread_info;
       uint64 state;
       uint64 counter;
       uint64 priority;
       uint64 pid;
   
       struct thread_struct thread;
   
       pagetable_t pgd;
   };
   ```

2. 修改 `task_init()` 

   * 每个用户态进程拥有两个 stack： `U-Mode Stack` 以及 `S-Mode Stack`， 其中 `S-Mode Stack` 已经在 `Lab3` 中设置完毕；在本次实验中，我们通过 `kalloc` 接口申请一个空的页面来作为 `U-Mode Stack`：

     ```c
     //arch/riscv/kernel/proc.c
     
     task[i]->thread_info->user_sp = kalloc();
     ```
   
   * 为每个用户态进程创建自己的页表，并将 `uapp` 所在页面以及 `U-Mode Stack` 做相应的映射；
     同时，为避免 `U-Mode` 和 `S-Mode` 切换的时候切换页表，我们也将内核页表 （ `swapper_pg_dir` ） 复制到每个进程的页表中。
     注意：程序运行过程中，有部分数据不在栈上，而在初始化的过程中就已经被分配了空间（比如我们的 `uapp` 中的 `counter` 变量），所以二进制文件需要先被 **拷贝** 到一块某个进程专用的内存之后再进行映射，防止所有的进程共享数据，造成期望外的进程间相互影响。
     
     ```c
     //arch/riscv/kernel/proc.c
     
     // 为用户态进程创建自己的页表，并将 uapp 所在界面 以及 U-Mode Stack 做相应映射
     pagetable_t tbl = (pagetable_t)kalloc();
     // 1. 为避免 U/S-Mode 切换时切换页表，将内核页表 swapper_pg_dir 复制到进程页表中
     char* dst = tbl;
     char* src = swapper_pg_dir;
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
     ```
     
   * 对于每个用户态进程，我们需要：
   
     * 将 `sepc` 修改为 `USER_START` 
   
       ```c
       //arch/riscv/kernel/proc.c
       
       task[i]->thread.sepc = USER_START;
       ```
   
     * 设置 `sstatus` 中的 `SPP`  ，使得 `sret` 返回至 `U-Mode` ；`SPIE` ，使得  `sret` 后开启中断；`SUM`  ，使得 `S-Mode` 可以访问 User 页面
   
       ​	查询手册，得到 64 位环境中 `sstatus`  寄存器的存储结构如下：
       ![sstatus](.\imgs\sstatus.jpg)
       ​	其中 `SPP` 处于 8 号位，`SPIE`  处于 5 号位，`SUM` 处于 18 号位。
   
       ​	当 `SPP = 0` 时，privilege level 被 设置给 U-Mode，使得 `sret` 可以返回至 `U-Mode` ；
       ![SPP](.\imgs\SPP.jpg)
       ​	当 `SPIE = 1` 时，`sret` 后将开启中断；
       ![SPIE](.\imgs\SPIE.jpg)
       ​	当 `SUM = 1` 时， `S-Mode` 将被允许访问 `U-Mode`  页面。
       ![SUM](.\imgs\SUM.jpg)
       ​	因此，我们需要将 `sstatus` 寄存器的 8 号位置 0；5 & 18 号位置 1：
   
       ```c
       //arch/riscv/kernel/proc.c
       
       // 设置 sstatus 寄存器
       uint64 sstatus = csr_read(sstatus);
       // SPP = 0
       sstatus = sstatus & 0xfffffffffffffeff;
       // SPIE = 1
       sstatus = sstatus | (1<<5);                         
       // SUM = 1
       sstatus = sstatus | (1<<18);
       
       task[i]->thread.sstatus = sstatus;
       ```
   
     * 使 `sscratch` 等于 `U-Mode` 的 `sp` （其值为 `USER_END` ），即 `U-Mode Stack` 被放置在  `user space`  的最后一个页面
   
       ```c
       //arch/riscv/kernel/proc.c
       
       task[i]->thread.sscratch = USER_END;
       ```
   
   
   * 修改 `__switch_to` ，加入 保存/恢复 `sepc` `sstatus` `sscratch` 以及 切换页表的逻辑：
   
     ```
     # arch/riscv/kernel/entry.S
     
     __switch_to:
         # save state to prev process
         ...
         
         # 保存 sepc -> 在 thread_struct 中，sepc 的存储位置紧随 s[11]
         csrr t1, sepc
         sd t1, 152(a0)
         # 保存 sstatus
         csrr t1, sstatus
         sd t1, 160(a0)
         # 保存 sscratch
         csrr t1, sscratch
         sd t1, 168(a0)
     
         # 切换页表（1）- 保存 satp
         csrr t1, satp
         sd t1, 176(a0) # 即存储在 task_struct.pgd 的位置上
     
         # restore state from next process
         ...
         
         # 读取 sepc，sstatus，sscratch
         ld t1, 152(a1)
         csrw sepc, t1
         ld t1, 160(a1)
         csrw sstatus, t1
         ld t1, 168(a1)
         csrw sscratch, t1
     
         # 切换页表（2）- 恢复 satp
         ld t1, 176(a1)
         csrw satp, t1
         # 刷新页表
         sfence.vma zero, zero
         
         ...
     ```

### 3.3 修改中断入口/返回逻辑 `_traps` 以及中断处理函数 `trap_handler` 

​	由于在 RISC-V 架构中只存在一个栈指针寄存器 `sp` ，因此我们需要手动完成对用户栈和内核栈的切换。具体表现为：触发异常时，由 `U-Mode Stack` → `S-Mode Stack`；完成异常处理后，由 `S-Mode Stack` → `U-Mode Stack`。

#### 3.3.1 修改 `__dummy` 

​	我们在 3.2 中进行用户线程初始化时使得 `thread_struct.sp = S-Mode sp`，而 `thread_struct.sscratch = U-Mode sp`。 因此在 `__dummy` 中，我们只需要将两者的值进行交换即可。

​	更改后的 `__dummy` 逻辑如下：

```
# arch/riscv/kernel/entry.S

__dummy:
    # YOUR CODE HERE

    # 交换 U-Mode Stack 与 S-Mode Stack
    csrr t1, sscratch
    csrw sscratch, sp
    add sp, t1, x0

    sret               # 使用 sret 从中断中返回
```

#### 3.3.2 修改 `_traps`

​	在异常处理的开始和结束阶段，我们同样需要对用户线程的 `sscratch`  与 `sp` 指针进行交换。

​	值得注意的是，内核线程没有 `U-Mode Stack` ，其 `sp` 指针永远指向 `S-Mode Stack`，而 `sscratch = 0`。在其触发异常时不需要进行切换。

​	因此，我们可以通过 `sscratch` 的值是否为 `0` 来区分用户线程与内核线程，并进行相应操作。

​	修改后的 `__trap`  逻辑如下：

​	其中 `__u_thread_trap`  用于处理用户线程触发的异常，内容等同于在 `_traps` 的 `1` 之前 与 `3`  之后添加了和 `__dummy` 中相同的代码，以交换 `U-Mode Stack`  与 `S-Mode Stack`。

```
_traps:
    # -----------

        # 判断为 内核线程 / 用户线程
        csrr t1, sscratch
        bne t1, x0, __u_thread_trap # 用户线程转到 __u_thread_trap

    # -----------

        # 1. save 32 registers and sepc to stack
        addi sp, sp, -33*8 # 让栈顶指针向低地址空间延伸33个寄存器的空间

        # 依次保存 32 个通用寄存器
        sd x1, 1*8(sp)

        addi x1, sp, 33*8
        sd x1, 2*8(sp)

        sd x3, 3*8(sp)
        sd x4, 4*8(sp)
        sd x5, 5*8(sp)
        sd x6, 6*8(sp)
        sd x7, 7*8(sp)
        sd x8, 8*8(sp)
        sd x9, 9*8(sp)
        sd x10, 10*8(sp)
        sd x11, 11*8(sp)
        sd x12, 12*8(sp)
        sd x13, 13*8(sp)
        sd x14, 14*8(sp)
        sd x15, 15*8(sp)
        sd x16, 16*8(sp)
        sd x17, 17*8(sp)
        sd x18, 18*8(sp)
        sd x19, 19*8(sp)
        sd x20, 20*8(sp)
        sd x21, 21*8(sp)
        sd x22, 22*8(sp)
        sd x23, 23*8(sp)
        sd x24, 24*8(sp)
        sd x25, 25*8(sp)
        sd x26, 26*8(sp)
        sd x27, 27*8(sp)
        sd x28, 28*8(sp)
        sd x29, 29*8(sp)
        sd x30, 30*8(sp)
        sd x31, 31*8(sp)


        # 保存 spec
        csrr t0, sepc    # 将 sepc 读取到通用寄存器
        sd t0, 32*8(sp)  # 将 sepc 的值保存到栈中

    # -----------

        # 2. call trap_handler
        
        csrr a0, scause   # scause -> a0
        # csrr a1, sepc     # sepc -> a1
        add a1, t0, x0
        add a2, sp, x0
        call trap_handler # 在 arch/riscv/kernel/trap.c 中实现


    # -----------

        # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack

        # 首先，恢复 sepc 寄存器
        ld t0, 32*8(sp)
        addi t0, t0, 4 # 不知道为啥 +4
        csrw sepc, t0

        # 随后，恢复除 x2 外的 31 个通用寄存器
        ld x0, 0*8(sp)
        ld ra, 1*8(sp) # 因为会报 unknown CSR `x1' 的错，所以改成 ra 了
        ld x3, 3*8(sp)
        ld x4, 4*8(sp)
        ld x5, 5*8(sp)
        ld x6, 6*8(sp)
        ld x7, 7*8(sp)
        ld x8, 8*8(sp)
        ld x9, 9*8(sp)
        ld x10, 10*8(sp)
        ld x11, 11*8(sp)
        ld x12, 12*8(sp)
        ld x13, 13*8(sp)
        ld x14, 14*8(sp)
        ld x15, 15*8(sp)
        ld x16, 16*8(sp)
        ld x17, 17*8(sp)
        ld x18, 18*8(sp)
        ld x19, 19*8(sp)
        ld x20, 20*8(sp)
        ld x21, 21*8(sp)
        ld x22, 22*8(sp)
        ld x23, 23*8(sp)
        ld x24, 24*8(sp)
        ld x25, 25*8(sp)
        ld x26, 26*8(sp)
        ld x27, 27*8(sp)
        ld x28, 28*8(sp)
        ld x29, 29*8(sp)
        ld x30, 30*8(sp)
        ld x31, 31*8(sp)

        # 恢复 x2 寄存器
        ld x2, 2*8(sp)

    # -----------

        # 4. return from trap

        sret

    # -----------
```

#### 3.3.2 修改 `trap_handler` 

​	将 `trap_handler`  的定义修改为 `void trap_handler(uint64 scause, uint64 sepc, struct pt_regs *regs)` ，同时在 `_traps` 中将 `sp` 写入 `a2` 寄存器，作为 `trap_handler` 的第三个参数：该参数 `regs` 指向的 `struct pt_regs`  结构体，用于从 `S-Mode Stack` 中读取相应寄存器的值。

​	由下图可知，`struct pt_regs` 中的数据的存储顺序为：32个寄存器 → `sepc` 寄存器 → `sstatus` 寄存器：

```
                         High Addr ───►  ┌────────┐
                                            │   sstatus   │
                                            │             │
                                            │     sepc    │
                                            │             │
                                            │     x31     │
                                            │             │
                                            │      .      │
                                            │      .      │
                                            │      .      │
                                            │             │
                                            │     x1      │
                                            │             │
                                            │     x0      │
                        sp (pt_regs)  ──► ├────────┤
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                                            │             │
                        Low  Addr ───►   └────────┘

```

​	因此，我们可以补充 `struct pt_regs` 的定义如下：

```c
// arch/riscv/include/syscall.h

struct pt_regs {
    uint64 x[32];
    uint64 sepc;
    uint64 sstatus;
};
```

​	由于 `uapp` 在使用 `ecall` 时会产生 `ECALL_FROM_U_MODE exception`，我们需要在 `trap_handler()` 中进行捕获和处理。

​	查询的到 `scause` 寄存器的存储结构如下：

<img src=".\imgs\SCAUSE.jpg" alt="SCAUSE" style="zoom:67%;" />

​	在产生中断后，`scause` 寄存器的值与意义的对应关系如下：

![scause_after](.\imgs\scause_after.jpg)

​	由上表可知：当 `Interrupt = 1 && Exception Code = 8` 时为 `ECALL_FROM_U_MODE` 异常。我们需要在 `trap_handler()` 中添加对该情况的处理逻辑：

```c
// arch/rsicv/kernel/trap.c

void trap_handler(uint64 scause, uint64 sepc, struct pt_regs *regs) {
    // 通过 `scause` 判断trap类型
    // 如果是interrupt 判断是否是timer interrupt
    // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
    // `clock_set_next_event()` 见 4.5 节
    // 其他interrupt / exception 可以直接忽略

    // YOUR CODE HERE
    if ((scause >> 63) == 1) {           // 通过63号位判断是否为 trap
        if(((scause << 1) >> 1 ) == 5) { // 左移去掉63号位，右移恢复，得到 Exception Code
            // Exception Code == 5，判断为 timer interrupt
            // printk("\n%s", "[Trap Handler] S-Mode Timer Interrupt!\n");
            // do_timer();
            clock_set_next_event();
            do_timer();
        }
    } else {
        // interrupt == 0 && exception code == 8
        if(scause == 8) { // 此时 Intterupt == 1, scause 本身即为 Exception Code 的值
            sys_call(regs);
        }
    }

    return;
    // 忽略其他 interrupt / exception
}
```

### 3.4 添加系统调用

​	增加 `syscall.h` 与 `syscall.c` ，用于实现系统调用函数 `void sys_call(struct pt_regs *regs)`，用于实现 `getpid` 与 `write` 逻辑，返回值位于 `regs` 指向的 `a0` 寄存器。

​	172 号系统调用 `sys_getpid()` 用于从 `current ` 中获取 `pid` ，并于寄存器 `a0` 中返回。

​	64 号系统调用 `sys_write(unsigned int fd, const char* buf, size_t count)` 用于将用户传递的字符串打印到屏幕上，其中 `fd=1` 为标准输出， `buf` 为用户需要打印的起始地址，`count` 为字符串长度。最终于 `a0` 中返回打印的字符数。

​	从函数原型中可以看出：系统调用首先通过 `li a7, %1` 指令从寄存器 `a7`（`x[17]`）中获取系统调用编号，并最终通过 `mv %0, a0` 将返回值赋给寄存器 `a0` 。

```c
// arch\riscv\kernel\syscall.c

void sys_call(struct pt_regs *regs) {
    uint64 func = regs->x[17]; // 获取系统调用编号
    
    if (func == 172) {
        // sys_getpid()
        regs->x[10] = current->pid;
    } else if (func == 64){
        // sys_write()
        if (regs->x[10] == 1) { // fd(a0 = 1)
            ((char*)(regs->x[11]))[regs->x[12]] = '\0';  
            // 使 char数组 a1 的结尾 a1[length](a1[a2]) = '\0'
            regs->x[10] = printk((char *)(regs->x[11])); 
            // a0 = 实际输出字符串长度
        }
    }
    return;
}
```

---

​	由于 `sepc` 中记录的是触发异常的指令地址，但在处理系统调用异常后，我们应该继续执行其后置指令。所以此处我们手动将 `sepc + 4` :

```
# arch/riscv/kernel/entry.S

_traps:
	...
	call trap_handler
	2
    ld t0, 32*8(sp)
    addi t0, t0, 4 # 手动修改 spec 的地址，使得 sret 之后 程序继续执行
    csrw sepc, t0
    
    ...
```

### 3.5 修改 `head.S` 以及 `start_kernel` 

​	在之前的实验中，操作系统在完成 boot 后需要等待一个时间片才开始调度。我们此时将其修改为 boot 完成后立刻调度 `uapp` 运行。

#### 3.5.1 修改 `head.S`

​	注释 enable interrupt 的逻辑（ 令 `sstatus.SIE = 1`），确保调度过程不受中断影响。

#### 3.5.2 修改 `start_kernel` 

​	在 `test()` 前调用 `schedule()`：

```c
// init/main.c

void sys_call(struct pt_regs *regs) {
    uint64 func = regs->x[17]; // 获取系统调用编号
    
    if (func == 172) {
        // sys_getpid()
        regs->x[10] = current->pid;
    } else if (func == 64){
        // sys_write()
        if (regs->x[10] == 1) { // fd(a0 = 1)
            ((char*)(regs->x[11]))[regs->x[12]] = '\0';  
            // 使 char数组 a1 的结尾 a1[length](a1[a2]) = '\0'
            printk((char*)(regs->x[11]));
            regs->x[10] = regs->x[12]; // a0 = 实际输出字符串长度
        }
    }
    return;
}
```

### 3.6 编译及测试

​	按照输出示例的结果对 `print()` 语句进行相应调整后的测试输出结果如下：

<img src=".\imgs\output.jpg" alt="output" style="zoom:67%;" />

## 4 思考题

1. **我们在实验中使用的用户态线程和内核态线程的对应关系是怎样的？（一对一，一对多，多对一还是多对多）**

   一对一

2. **为什么 Phdr 中，`p_filesz` 和 `p_memsz` 是不一样大的？**

   （使用了去年的框架，然后没有涉及这个部分orz）

3. **为什么多个进程的栈虚拟地址可以是相同的？用户有没有常规的方法知道自己栈所在的物理地址？**

   ​	因为每个进程都有自己的页表。在不同的页表中，相同的虚拟地址被映射到非重合的物理地址上，这使得多个进程可以拥有相同的栈虚拟地址。

   ​	善用搜索引擎后得到的答案似乎是没有。（不知道分配空间的时候直接让 print 出来算不算）。

## 5 心得体会

​	在完成本次实验的过程中首先发觉自己亟需提升汇编编程能力 —— 指在 `_traps` 里尝试用 `bne` 写分支，但是在尝试各种跳转 bug 频出后无奈选择了 CV 编程大法 —— 把原本的 保存/恢复 函数整个复制一遍，然后在头尾塞上两段栈切换的代码（也是 CV 的）。这一举动显著增加了 `entry.S`  的代码量，同时也增加了 debug 的复杂度 —— 在 CV 段上做更改经常会忘记把所有 copy 的段落都修正过来。这段惨痛经历再一次证明了：善用函数，善用宏，减少代码重复量是十分重要的。

​	部分实现的实验指导还是有点模糊（比如“添加系统调用”那一块），一开始我以为是要 remake 两个系统调用函数，后来和队友交流一通才发现：调用就完事了。尽管如此，如何从 `regs` 中获取输入，返回输出值还是让我对着屏幕发了好久呆，希望之后能够更加完善一些。
