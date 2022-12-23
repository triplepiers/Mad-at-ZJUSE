// arch/rsicv/kernel/trap.c
#include "printk.h" // 调用 printk()
#include "clock.h"  // 调用 clock_set_next_event()
#include "proc.h"   // 调用 do_timer()
#include "syscall.h" // 调用 sys_call()

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
