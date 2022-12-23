# arch/riscv/kernel/head.S
.extern start_kernel

    .section .text.entry
    .globl _start
    .equ stack_size, 4096 # 定义栈大小为4kB
_start:
    # ------------------
    # my code
    la sp, boot_stack_top # 使栈指针指向栈顶
    jal start_kernel  # 跳转至main.c中的start_kernel函数
    # ------------------

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space stack_size # 将栈大小设置为4kB
    .globl boot_stack_top
boot_stack_top: