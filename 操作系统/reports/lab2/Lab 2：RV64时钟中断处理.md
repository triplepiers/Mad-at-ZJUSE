# Lab 2：RV64时钟中断处理

---

​                专业：软件工程   日期：2022年9月21日

---

## 1 实验目的

* 学习 RISC-V 的 trap 处理相关寄存器与指令，完成对 trap 处理的初始化。
* 理解 CPU 上下文切换机制，并正确实现上下文切换功能。
* 编写 trap 处理函数，完成对特定 trap 的处理。
* 调用 OpenSBI 提供的接口，完成对时钟中断事件的设置。

## 2 实验环境

* Ubuntu 22.04.1 LTS Windows Subsystem for Linux 2（同Lab0）

## 3 实验步骤

### 3.1 准备工程

1. 从 `repo` 同步代码：`stddef.h` `printk.h` `printk.c` 并按照指定的目录结构放置，并删除 `print.h` `print.c`，此时的 Lab2 文件夹目录结构如下：

<img src="imgs\tree.jpg" alt="tree" style="zoom: 50%;" />

* 修改 `init/main.c` 的内容：

```c
// init/main.c
#include "printk.h" //修改原本对 print.h 的引用
#include "sbi.h"

extern void test();

int start_kernel() {
    //修改原本对 puti & puts 的使用
    printk("%d", 2022);
    printk("%s", " Hello RISC-V\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
```

* 修改 `init/test.c` 中原本对 `print.h` 的引用为对 `printk.h` 的引用。

* 修改 `lib/Makefile` 下 `SRC` 与 `OBJ` 标签的内容。

2. 随后，根据实验指导修改 `vmlinux.lds` 以及 `head.S`，完成对相应代码片段的替换后完成准备工作。

### 3.2 开启 `trap` 处理

​	在运行 `start_kernel`  前，我们需要对CSR进行初始化。该阶段包括以下步骤：

1. 设置 `stvec`，将 `_traps` 所表示的地址写入 `stvec` 。

   ​        此处采用 `Direct 模式`  , `_traps` 是 `trap` 处理入口函数的及地址。

   ```
   # set stvec = _traps
   la t0, _traps       # 将 _traps 的地址加载到寄存器 t0 中
   csrw stvec, t0      # 将 _traps 的地址写入寄存器 stvec
   ```

2. 开启时钟中断，将 `sie[STIE]` 置 1。

   ​        查询《The RISC-V Instruction Set Manual Volume II: Privileged Architecture (Ver1.10)》，得到 Supervisor interrupt-enable (sie) 寄存器的结构如下：

   <img src="imgs\sie.jpg" alt="tree" style="zoom: 70%;" />

   ​        其中，`SITE` 处于寄存器的第五个 bit 位上。我们需要在保持 `sie` 寄存器其余位置数据不变的基础上将第五位置1，该部分补全后的代码如下：

   ```
   # set sie[STIE] = 1
   csrr t0, sie        # 读取sie寄存器的值
   ori t1, t0, 1<<5    # 将sie寄存器值的第五位置1，结果写入t1
   csrw sie, t1        # 将更改后的值保存回sie寄存器
   ```

3. 参考 `clock_set_next_event()` 中的逻辑设置第一次时钟中断。

   ​        在 `clock_set_next_event()`  函数中，我们首先设置了下一次时钟中断的时间点（`mtime` 寄存器值 + TIMECLOCK），随后调用了 `sbi_ecall` 以完成对下一次时钟中断的设置。

   ​        在相关实现中，我们使用了 `rdtime` 指令来获取 `time` 寄存器（即 `mtime`  寄存器）的值。查阅文档，得到对该指令的描述如下：

   <img src="imgs\rdtime.jpg" alt="tree" style="zoom: 60%;" />

   ​        此外，在 `clock_set_next_event()`  的实现中 TIMECLOCK 的值被设为 10000000 （由于QEMU中的时钟频率为 10MHz，10000000个时钟周期相当于 1s）。

   ​        由此，我们完成了对相应部分的补全：

   ```
   # set first time interrupt
   call set_first_timer      # 在 arch/riscv/kernel/clock.c 中实现
   ```

   ​        因为没办法在汇编中直接调用  `sbi_ecall` ，所以直接用 C + 内联汇编实现了。`set_first_timer` 函数的相关实现如下：

   ```c
   // arch/riscv/kernel/clock.c
   void set_first_timer() {
       // 使用 rdtime 命令，将time寄存器的值读入 t
       unsigned long t;
       __asm__ volatile ("rdtime %0" : "=r"(t));
       // 首次时钟中断时间点 = 当前时间点 + 1s
       t = t + TIMECLOCK;
       // 使用 sbi_ecall 来完成对首次时钟中断的设置
       sbi_ecall(0x0, 0, t, 0, 0, 0, 0, 0);
   }
   ```

   ​         其中 `sbi_ecall()` 的第一个参数 `0x0` 表示 本次调用的功能为 `SBI_SET_TIME` 。参考的 function list 如下图所示：

   <img src="imgs\opensbi_func_list.jpg" alt="tree" style="zoom: 50%;" />

4. 开启 S 态下的中断响应，将 `sstatus[SIE]`  置 1。

   ​        查询《The RISC-V Instruction Set Manual Volume II: Privileged Architecture (Ver1.10)》，得到 RV64 下 Supervisor-mode status  (sstatus) 寄存器的结构如下：

   <img src="imgs\sstatus.jpg" alt="tree" style="zoom: 70%;" />

   ​        其中，`SIE` 处于寄存器的第一个 bit 位上。与（2）中类似的，我们需要在保持 `sstatus` 寄存器其余位置数据不变的基础上将第一位置1，该部分补全后的代码如下：

   ```
   # set sstatus[SIE] = 1
   csrr t0, sstatus        # 读取sie寄存器的值
   ori t1, t0, 1<<1    # 将sie寄存器值的第五位置1，结果写入t1
   csrw sstatus, t1        # 将更改后的值保存回sie寄存器
   ```

---

补全后的 `_start` 内容如下：

```
# arch/riscv/kernel/head.S
.extern start_kernel

    .section .text.init   # 将 _start 放入.text.init section
    .globl _start
    .equ stack_size, 4096 # 定义栈大小为4kB
_start:

    la sp, boot_stack_top # 使栈指针指向栈顶

    # ------------------

        # set stvec = _traps
        la t0, _traps       # 将 _traps 的地址加载到寄存器 t0 中
        csrw stvec, t0      # 将 _traps 的地址写入寄存器 stvec
        
    # ------------------

        # set sie[STIE] = 1
        csrr t0, sie        # 读取sie寄存器的值
        ori t1, t0, 1<<5    # 将sie寄存器值的第五位置1，结果写入t1
        csrw sie, t1        # 将更改后的值保存回sie寄存器

    # ------------------

        # set first time interrupt
        # rdtime t2               # 将time寄存器的值读入t2
        # addi t2, t2, 10000000   # 下一次时钟中断时间点 = 当前时间点 + 1s
        # 写到这里发现不能直接调用 sbi_ecall，干脆全都扔到 c 的函数里去了
        call set_first_timer      # 在 arch/riscv/kernel/clock.c 中实现

    # ------------------

        # set sstatus[SIE] = 1
        csrr t0, sstatus        # 读取sstatus寄存器的值
        ori t1, t0, 1<<1        # 将sstatus寄存器值的第一位置1，结果写入t1
        csrw sstatus, t1        # 将更改后的值保存回ssstatus寄存器

    # ------------------

        # - my lab1 code -
        jal start_kernel  # 跳转至main.c中的start_kernel函数
        
    # ------------------

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space stack_size # 将栈大小设置为4kB
    .globl boot_stack_top
boot_stack_top:
```

### 3.3 实现上下文切换

​	使用汇编实现上下文切换包含以下几个步骤：

1. 在 `arch/riscv/kernel/` 目录下添加 `entry.S` 文件。

   <img src="imgs\add_entry.jpg" alt="tree" style="zoom: 40%;" />

2. 保存 CPU 的寄存器到内存中。

   ​        此处我们需要对32个通用寄存器和1个与中断相关的CSR（`sepc`）进行保存。

   ​        我们首先让栈顶指针向低地址空间延伸33个寄存器的空间，随后使用 `sd` 命令对32个通用寄存器进行保存。手册中关于 `sd` 命令的说明如下：

   <img src="imgs\sd.jpg" alt="tree" style="zoom: 70%;" />

   ​        其中，原本的栈顶指针需要特殊处理。我们先将其保存至 `sscratch` 寄存器中，最后再将其保存至栈中。

   ​        由于 RISCV 不能直接将 CSR 写到内存，我们使用 `csrr`  命令将 CSR 读取到通用寄存器，再将其保存到内存中。

   ​        补全后的相关逻辑如下：

   ```
   # 1. save 32 registers and sepc to stack
   
   csrw sscratch, sp # 将原先的栈顶指针保存到sscratch
   
   addi sp, sp -33*8 # 让栈顶指针向低地址空间延伸33个寄存器的空间
   
   # 依次保存 31 个通用寄存器（除 x2 寄存器外）
   sd x0, 0*8(sp)
   sd x1, 1*8(sp)
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
   
   # 保存 x2 寄存器（原本的栈顶指针）
   csrr t0, sscratch
   sd t0, 2*8(sp)
   ```

3. 将 `scause` 和 `sepc` 中的值传入 `trap` 处理函数 `trap_handler`。

   ​        文件 `trap.c` 中处理函数 `trap_handler` 的接口定义如下：

   ```c
   void trap_handler(unsigned long scause, unsigned long sepc);
   ```

   ​        相应的，我们应将 `scause`  的值保存至 `a0` ，`sepc` 的值保存至 `a1` 以作为 `trap_handler` 的参数。

   ​        补全后的相关逻辑如下：

   ```
   # 2. call trap_handler
           
   csrr a0, scause   # scause -> a0
   csrr a1, sepc     # sepc -> a1
   call trap_handler # 在 arch/riscv/kernel/trap.c 中实现
   ```

4. 在完成对 `trap` 的处理之后， 我们从内存中恢复 CPU 的寄存器。

   ​        此处我们采用与（1）中相反的顺序对寄存器进行恢复，即：先恢复 `sepc` 寄存器，再恢复除 `x2(sp)` 外的31个通用寄存器，最后恢复 `x2(sp)` 寄存器（因为此时还需要根据 sp 确定其他寄存器保存的位置）。

   ​        与 `sd` 相对的，我们在此处采用 `ld` 命令完成对寄存器的恢复，在手册中该命令的说明如下：

   <img src="imgs\ld.jpg" alt="tree" style="zoom: 70%;" />

   ​        最后，我们释放在（1）中申请的栈空间。

   ​        补全后的相关逻辑如下：

   ```
   # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
   
   # 首先，恢复 sepc 寄存器
   ld t0, 32*8(sp)
   csrw sepc, t0
   
   # 随后，恢复除 x2 外的 31 个通用寄存器
   ld x0, 0*8(sp)
   ld ra, 1*8(sp)
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
   
   # 释放申请的栈空间
   addi sp, sp, 33*8
   ```

5. 从 `trap` 中返回。

   ​        此处使用指令 `sret` 实现，在手册中的相关定义如下：

   <img src="imgs\sret.jpg" alt="tree" style="zoom: 70%;" />

   ​        该部分实现如下：

   ```
   # 4. return from trap
   
   sret
   ```

---

​	补全后的 `_traps` 内容如下：

```
# arch/riscv/kernel/entry.S
    .section .text.entry
    .align 2
    .globl _traps 
_traps:
    # -----------

        # 1. save 32 registers and sepc to stack

        csrw sscratch, sp # 将原先的栈顶指针保存到sscratch

        addi sp, sp, -33*8 # 让栈顶指针向低地址空间延伸33个寄存器的空间
        
        # 依次保存 31 个通用寄存器（除 x2 寄存器外）
        sd x0, 0*8(sp)
        sd ra, 1*8(sp) # 因为会报 unknown CSR `x1' 的错，所以改成 ra 了
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

        # 保存 x2 寄存器（原本的栈顶指针）
        csrrw t0, sscratch, x0 # 此时处于内核态（S态），将 sscratch 置 0
        sd t0, 2*8(sp)

    # -----------

        # 2. call trap_handler
        
        csrr a0, scause   # scause -> a0
        csrr a1, sepc     # sepc -> a1
        call trap_handler # 在 arch/riscv/kernel/trap.c 中实现

    # -----------

        # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack

        # 首先，恢复 sepc 寄存器
        ld t0, 32*8(sp)
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

        # 释放申请的栈空间
        addi sp, sp, 33*8

    # -----------

        # 4. return from trap

        sret

    # -----------
```

### 3.4 实现 `trap` 处理函数

1. 在 `arch/riscv/kernel/` 目录下添加 `trap.c` 文件。

   <img src="imgs\add_trap.jpg" alt="tree" style="zoom: 40%;" />

2. 在 `trap.c` 中实现 `trap` 处理函数 `trap_handler()`, 其接收的两个参数分别是 `scause` 和 `sepc` 两个寄存器中的值。

   ​        在本函数中需要通过 `scause` 寄存器判断 `trap` 类型，查询文档可知 `scause` 寄存器的存储结构及不同数值所代表的含义（如下图所示）。

   <img src="imgs\scause.jpg" alt="tree" style="zoom: 70%;" />

   ​        如图，当 `scause` 的 63 号位为 `1` 时为 `interrupt` ，且当 `Exception Code `  为 `5`时可进一步确定为 `timer interrupt` 。

   ​        当 `scause` 符合  `timer interrupt`  条件时，我们调用 `printk()` 函数输出提示，并调用 `clock_set_next_event()` 函数设置下一次时钟中断。

   ​        编写完成的 `lab2/arch/riscv/kernel/trap.c ` 内容如下：

   ```c
   // arch/rsicv/kernel/trap.c
   #include "printk.h" // 调用 printk()
   #include "clock.h"  // 调用 clock_set_next_event()
   
   void trap_handler(unsigned long scause, unsigned long sepc) {
       // 通过 `scause` 判断trap类型
       // 如果是interrupt 判断是否是timer interrupt
       // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
       // `clock_set_next_event()` 见 4.5 节
       // 其他interrupt / exception 可以直接忽略
   
       // YOUR CODE HERE
       if ((scause >> 63) == 1) {           // 通过63号位判断是否为 trap
           if(((scause << 1) >> 1 ) == 5) { // 左移去掉63号位，右移恢复，得到 Exception Code
               // Exception Code == 5，判断为 timer interrupt
               printk("%s", "[Trap Handler] S-Mode Timer Interrupt!");
               clock_set_next_event();
           }
       }
   
       return;
       // 忽略其他 interrupt / exception
   }
   ```

### 3.5 实现时钟中断相关函数

1. 在 `arch/riscv/kernel/` 目录下添加 `clock.c` 文件。

   <img src="imgs\add_clock.jpg" alt="tree" style="zoom: 40%;" />

2. 在 `clock.c` 中实现 `get_cycles()` : 使用 `rdtime` 汇编指令获得当前 `time` 寄存器中的值。

   ​        在 3.2 中，我们已经查询了 `rdtime` 命令的相关使用规则，在此不做赘述，实现如下：

   ```c
   // arch/riscv/kernel/clock.c
   unsigned long get_cycles() {
       // 编写内联汇编，使用 rdtime 获取 time 寄存器中 (也就是mtime 寄存器 )的值并返回
       // YOUR CODE HERE
       unsigned long t;
       // 使用 rdtime 命令，将 time 寄存器的值读取到变量 t 中
       __asm__ volatile ("rdtime %0" : "=r"(t));  
       
       return t;
   }
   ```

3. 在 `clock.c` 中实现 `clock_set_next_event()` : 调用 `sbi_ecall`，设置下一个时钟中断事件。

   ​        在 3.2 中，我们已经了解到使用 `sbi_ecall()` 设置时钟中断的相关规则，不做赘述，实现如下：

   ```c
   // arch/riscv/kernel/clock.c
   void clock_set_next_event() {
       // 下一次 时钟中断 的时间点
       unsigned long next = get_cycles() + TIMECLOCK;
   
       // 使用 sbi_ecall 来完成对下一次时钟中断的设置
       // YOUR CODE HERE
       sbi_ecall(0x0, 0, next, 0, 0, 0, 0, 0); // 0x0 表示 SBI_SET_TIMER
   } 
   ```

---

​        编写完成的 `arch/riscv/kernel/clock.c` 内容如下：

```c
// arch/riscv/kernel/clock.c
#include "sbi.h"

// QEMU中时钟的频率是10MHz, 也就是1秒钟相当于10000000个时钟周期。
unsigned long TIMECLOCK = 10000000;

unsigned long get_cycles() {
    // 编写内联汇编，使用 rdtime 获取 time 寄存器中 (也就是mtime 寄存器 )的值并返回
    // YOUR CODE HERE
    unsigned long t;
    __asm__ volatile ("rdtime %0" : "=r"(t));  // 使用 rdtime 命令，将 time 寄存器的值读取到变量 t 中

    return t;
}

void clock_set_next_event() {
    // 下一次 时钟中断 的时间点
    unsigned long next = get_cycles() + TIMECLOCK;

    // 使用 sbi_ecall 来完成对下一次时钟中断的设置
    // YOUR CODE HERE
    sbi_ecall(0x0, 0, next, 0, 0, 0, 0, 0); // 0x0 表示 SBI_SET_TIMER
} 
```

### 3.6 编译及测试

​        在根目录下执行 `make run` 命令进行编译运行，输出以下结果：

<img src="imgs\test.jpg" alt="tree" style="zoom: 40%;" />

​        其中：

*  `Set first timer` 是成功设置第一次时钟中断的提示输出
* `2022 Hello RISC-V` 是执行 `main()` 函数的提示输出
* `[Trap Handler] S-Mode Timer Intterrupt` 是 `trap_handler()` 函数每隔 1s 处理时钟中断的提示输出。

## 4 思考题

​	**在我们使用 `make run` 时，OpenSBI会产生如下输出：**

```
OpenSBI v0.9
____                    _____ ____ _____
/ __ \                  / ____|  _ \_   _|
| |  | |_ __   ___ _ __ | (___ | |_) || |
| |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
| |__| | |_) |  __/ | | |____) | |_) || |_
\____/| .__/ \___|_| |_|_____/|____/_____|
        | |
        |_|

......

Boot HART MIDELEG         : 0x0000000000000222
Boot HART MEDELEG         : 0x000000000000b109

......
```

​	**通过查看 `RISC-V Privileged Spec` 中的 `medeleg` 和 `mideleg` 解释上面 `MIDELEG` 值的含义：**

> 参考资料： Volume 2, Privileged Spec v. 20211203

​        文档中对 `medeleg` 和 `mideleg` 的相关描述如下图所示：

<img src="imgs\medeleg.jpg" alt="tree" style="zoom: 70%;" />

​         此处 `mideleg`  的值为 `0x0x0000000000000222` 等价于 `1000100010(B)` ，即第 2，6，10 位置 1。查询文档得到以下内容：

> **8.4.2 Machine Interrupt Delegation Register (mideleg)**
>
> When the hypervisor extension is implemented, bits 10, 6, and 2 of mideleg (corresponding to the standard VS-level interrupts) are each read-only one. Furthermore, if any guest external interrupts are implemented (GEILEN is nonzero), bit 12 of mideleg (corresponding to supervisor-level guest external interrupts) is also read-only one. VS-level interrupts and guest external interrupts are always delegated past M-mode to HS-mode.
>
> —— Volume II: RISC-V Privileged Architectures V20211203，p121

​        由上文中的描述可知：若 `mideleg`  的值为 `0x0x0000000000000222` ，则表明 `hypervisor extension` 被执行。

## 5 心得体会

​        在实现第一次时钟中断时，本来打算在 `head.S` 中全部使用汇编解决，结果发现不能直接在汇编中调用 `sbi_ecall()` 函数，就全都丢到 `.c` 文件的函数中用内联汇编实现了。后面在实现 `trap_handler()` 时发现可以直接把数据传到 `a0 / a1` 寄存器中作为函数参数，感觉开辟了新世界。 

​        在查询相关资料时发现，南开大学的对应指导文档时发现他们是使用汇编宏实现上下文的保存和恢复的，感觉到了一丝优雅。此外，由于  `x0`  寄存器恒为 0，在保存和恢复上下文时可以不用对其进行处理。

​        此外，在判断 `scause`  的 `Exception Code `  是否为 5 时，本来打算用左右移判断第 0，2 位均等于 1（被前面判断第 63 位为 1 的思路限制住了），之后突然想起来可以丢掉最高位然后判断十进制是否为 5，事情突然变得简单了起来。

​        通过本次实验，我对于 CPU 上下文切换机制有了更为直观深入的理解，对 RISCV 中与异常处理相关的寄存器进行了学习，掌握了时钟中断的设置，实现了对课堂知识的消化，收获颇丰。
