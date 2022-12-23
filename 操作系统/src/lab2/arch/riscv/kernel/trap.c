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
            printk("%s", "[Trap Handler] S-Mode Timer Interrupt!\n");
            clock_set_next_event();
        }
    }

    return;
    // 忽略其他 interrupt / exception
}
