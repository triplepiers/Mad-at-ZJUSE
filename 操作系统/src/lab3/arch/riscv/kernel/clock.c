// arch/riscv/kernel/clock.c
#include "sbi.h"
#include "printk.h"

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

void set_first_timer() {
    printk("%s", "Set first timer...\n");
    // 使用 rdtime 命令，将time寄存器的值读入 t
    unsigned long t;
    __asm__ volatile ("rdtime %0" : "=r"(t));
    // 首次时钟中断时间点 = 当前时间点 + 1s
    t = t + TIMECLOCK;
    // 使用 sbi_ecall 来完成对首次时钟中断的设置
    sbi_ecall(0x0, 0, t, 0, 0, 0, 0, 0);
}