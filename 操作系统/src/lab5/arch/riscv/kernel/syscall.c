// arch\riscv\kernel\syscall.c
#include "syscall.h"

void sys_call(struct pt_regs *regs) {
    uint64 func = regs->x[17]; // 获取系统调用编号
    
    if (func == 172) {
        // sys_getpid()
        regs->x[10] = current->pid;
    } else if (func == 64){
        // sys_write()
        if (regs->x[10] == 1) { // fd(a0 = 1)
            ((char*)(regs->x[11]))[regs->x[12]] = '\0';  // 使 char数组 a1 的结尾 a1[length](a1[a2]) = '\0'
            printk((char*)(regs->x[11]));
            regs->x[10] = regs->x[12]; // a0 = 实际输出字符串长度
        }
    }
    return;
}