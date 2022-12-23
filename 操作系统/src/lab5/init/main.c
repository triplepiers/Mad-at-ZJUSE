// init/main.c
#include "printk.h" //修改原本对 print.h 的引用
#include "sbi.h"
#include "proc.h"  // schedule

extern void test();

int start_kernel() {
    //修改原本对 puti & puts 的使用
    printk("[S-Mode] Hello RISC-V\n");

    schedule();

    test(); // DO NOT DELETE !!!

	return 0;
}
