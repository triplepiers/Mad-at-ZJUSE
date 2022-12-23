// arch/riscv/include/syscall.h
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "proc.h" // current

extern struct task_struct *current;

struct pt_regs {
    uint64 x[32];
    uint64 sepc;
    uint64 sstatus;
};

void sys_call(struct pt_regs *regs);

#endif