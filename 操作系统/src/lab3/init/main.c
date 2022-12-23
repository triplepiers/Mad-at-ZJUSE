// init/main.c
#include "printk.h" //修改原本对 print.h 的引用
#include "sbi.h"

extern void test();

int start_kernel() {
    //修改原本对 puti & puts 的使用
    printk("%d", 2022);
    printk("%s", " Hello RISC-V\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
