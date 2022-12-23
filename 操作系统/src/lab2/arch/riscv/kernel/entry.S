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

        sret # S态trap返回，通过sepc设置pc值，返回原程序运行

    # -----------
