// arch/riscv/kernel/sbi.c
#include "types.h"
#include "sbi.h"


struct sbiret sbi_ecall(int ext, int fid, uint64 arg0,
			            uint64 arg1, uint64 arg2,
			            uint64 arg3, uint64 arg4,
			            uint64 arg5) 
{
    struct sbiret res; // 记录OpenSBI的返回值
	
	// 对各寄存器进行赋值
	register uint64 a0 asm ("a0") = arg0;
	register uint64 a1 asm ("a1") = arg1;
	register uint64 a2 asm ("a2") = arg2;
	register uint64 a3 asm ("a3") = arg3;
	register uint64 a4 asm ("a4") = arg4;
	register uint64 a5 asm ("a5") = arg5;

	uint64 _fid = (uint64)(fid);
	uint64 _ext = (uint64)(ext);
	register uint64 a6 asm ("a6") = _fid;
	register uint64 a7 asm ("a7") = _ext;

	// 内联汇编部分
	__asm__ volatile (
		"ecall\n" //使系统进入M模式
		:"=r" (a0), "=r" (a1)::
    );

	// 将OpenSBI的返回值存入res
	res.error = a0;
	res.value = a1;

	return res;

}
