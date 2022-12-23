# arch/riscv/kernel/head.S

# for PA2VA_OFFSET
#include "defs.h" 

.extern start_kernel

    .section .text.init   # 将 _start 放入.text.init section
    .globl _start
    .equ stack_size, 4096 # 定义栈大小为4kB
_start:

    la sp, boot_stack_top # 使栈指针指向栈顶

    # ------------------

        # code of lab4
        call setup_vm
        call relocate

    # ------------------

        # code of lab3
        call mm_init          # 初始化内存管理系统

        call setup_vm_final   # code of lab4

        call task_init        # 线程初始化

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
        csrr t0, sstatus        # 读取sie寄存器的值
        ori t1, t0, 1<<1    # 将sie寄存器值的第五位置1，结果写入t1
        csrw sstatus, t1        # 将更改后的值保存回sie寄存器

    # ------------------

        # - my lab1 code -
        j start_kernel  # 跳转至main.c中的start_kernel函数
        
    # ------------------
relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)

    # ------------------

        li t0, PA2VA_OFFSET
        add ra, ra, t0 # set ra = ra + PA2VA_OFFSET
        add sp, sp, t0 # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
    
    # ------------------

    # set satp with early_pgtbl

    # ------------------

        # Mode = 8
        addi t0, x0, 8     # t0 = 8
        slli t0, t0, 60    # 令 63~60 bit 上的 Mode = 8
        # 因为物理页大小为4KB, 故 PPN = PA >> 12
        la t1, early_pgtbl # 加载顶级页表地址
        srli t1, t1, 12    # t1 = PPN
        or t0, t0, t1      # 拼合 Mode & PPN
        csrw satp, t0      # 设置 satp

    # ------------------

    # flush tlb
    sfence.vma zero, zero

    # flush icache
    fence.i

    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space stack_size # 将栈大小设置为4kB
    .globl boot_stack_top
boot_stack_top: