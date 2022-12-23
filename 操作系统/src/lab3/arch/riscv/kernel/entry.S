# arch/riscv/kernel/entry.S

    .section .text.entry
    .align 2
    .globl _traps 
    .globl __dummy
    .globl __switch_to
    
_traps:
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

    # -----------

        # 4. return from trap

        sret

    # -----------

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

__dummy:
    # YOUR CODE HERE

    la t0, dummy       # 将 dummy() 的地址加载到寄存器 t0 中
    csrw sepc, t0      # 将 dummy() 的地址写入寄存器 sepc

    sret               # 使用 sret 从中断中返回