// arch/riscv/kernel/vm.c
#include "string.h"  // memset()
#include "defs.h"    // PGSIZE
#include "mm.h"      // kalloc()
#include "printk.h"

/* early_pgtbl: 用于 setup_vm 进行 1GB 的 映射。 */
unsigned long  early_pgtbl[512] __attribute__((__aligned__(0x1000)));

void setup_vm(void) {
    /* 
    1. 由于是进行 1GB 的映射 这里不需要使用多级页表 
    2. 将 va 的 64bit 作为如下划分： | high bit | 9 bit | 30 bit |
        high bit 可以忽略
        中间9 bit 作为 early_pgtbl 的 index
        低 30 bit 作为 页内偏移 这里注意到 30 = 9 + 9 + 12， 即我们只使用根页表， 根页表的每个 entry 都对应 1GB 的区域。 
    3. Page Table Entry 的权限 V | R | W | X 位设置为 1
    */

    memset(early_pgtbl, 0x0, PGSIZE); // 初始化大小为4KB的顶级页表
   
    unsigned long pa, va;
    // 将 0x80000000 开始的 1GB 区域进行一次等值映射 ( PA == VA )
    pa = PHY_START;
    va = PHY_START;
    int vpn = (va >> 30) & 0x1ff;      // 取虚拟页号：va的38-30 (9 bit)
    int ppn = (pa >> 30) & 0x3ffffff;  // 取物理页号，pa的55-30 (26 bit)
    early_pgtbl[vpn] = (ppn<<28) | 15; // 拼合 PPN & 权限位 - 15 == 4~0位为'1'，即 V | R | W | X 位为 1

    // 将 0x80000000 开始的 1GB 区域映射至高地址 ( PA + PV2VA_OFFSET == VA )
    va = VM_START;
    vpn = (va >> 30) & 0x1ff;          // 取新的虚拟页号
    // 因为 pa 没有变，ppn和上面一样，就不重复取了
    early_pgtbl[vpn] = (ppn<<28) | 15; // 二次映射
}

/* swapper_pg_dir: kernel pagetable 根目录， 在 setup_vm_final 进行映射。 */
unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

extern char _srodata[];
extern char _stext[];
extern char _sdata[];

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);

void setup_vm_final(void) {

    memset(swapper_pg_dir, 0x0, PGSIZE);

    unsigned long pa, va, size;
    // No OpenSBI mapping required

    // mapping kernel text X|-|R|V
    // X|-|R|V -> 0b1011 -> perm = 11
    pa = PHY_START + OPENSBI_SIZE;
    va = VM_START  + OPENSBI_SIZE;
    size = (unsigned long)_srodata-(unsigned long)_stext;
    create_mapping(swapper_pg_dir, va, pa, size, 11);

    // mapping kernel rodata -|-|R|V
    // -|-|R|V ->0b0011 -> perm = 3
    pa += size;
    va += size;
    size = (unsigned long)_sdata-(unsigned long)_srodata;
    create_mapping(swapper_pg_dir, va, pa, size, 3);

    // mapping other memory -|W|R|V
    // -|W|R|V -> 0b0111 -> perm = 7
    pa += size;
    va += size;
    size = PHY_SIZE - ((unsigned long)_sdata-(unsigned long)_stext);
    create_mapping(swapper_pg_dir, va, pa, size, 7);

    // set satp with swapper_pg_dir

    // YOUR CODE HERE
    asm volatile (
        "addi t0, x0, 8\n"
        "slli t0, t0, 60\n"
        "mv t1, %[addr]\n"
        "srli t1, t1, 12\n"
        "or t0, t0, t1\n"
        "csrw satp, t0"
        : 
        :[addr] "r" ((unsigned long)swapper_pg_dir - PA2VA_OFFSET)
        :"memory"
    );

    // flush TLB
    asm volatile("sfence.vma zero, zero");

    // flush icache
    asm volatile("fence.i");

    return;
}

/* 创建多级页表映射关系 */
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm) {
    /*
    pgtbl 为根页表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的大小
    perm 为映射的读写权限

    创建多级页表的时候可以使用 kalloc() 来获取一页作为页表目录
    可以使用 V bit 来判断页表项是否存在
    */

    unsigned long *tbl[3];
    unsigned int VPN[3];
    unsigned long PTE[3];
    unsigned long offset = 0;
    unsigned long *pg = NULL;

    tbl[2] = pgtbl; // 顶级页表

    while(offset < sz) {
        // 处理三级页表
        VPN[2] = ((va + offset) >> 30) & 0x1ff;
        PTE[2] = tbl[2][VPN[2]];
        // invalid entry -> allocate new page
        if((PTE[2] & 1) == 0) { // 检查页表项是否有效 ? V == 1
            pg = (unsigned long*)kalloc(); // 地址在虚拟空间上
            // 物理地址 >> 12 得到 PPN && V = 1
            PTE[2] = ((((unsigned long)pg - PA2VA_OFFSET) >> 12) << 10) | 1;
            tbl[2][VPN[2]] = PTE[2];
        }

        // 处理二级页表
        tbl[1] = (unsigned long*)((PTE[2] >> 10) << 12);
        VPN[1] = ((va + offset) >> 21) & 0x1ff;
        PTE[1] = tbl[1][VPN[1]];
        if((PTE[1] & 1) == 0) {
            pg = (unsigned long*)kalloc();
            PTE[1] = ((((unsigned long)pg - PA2VA_OFFSET) >> 12) << 10) | 1;
            tbl[1][VPN[1]] = PTE[1];
        }

        // 处理一级页表
        tbl[0] = (unsigned long*)((PTE[1] >> 10) << 12);
        VPN[0] = ((va + offset) >> 12) & 0x1ff;
        // 一级页表项中存储 pa 本身的 PPN && 根据传入的 perm 改写 flags
        tbl[0][VPN[0]] = (((pa + offset) >> 12) << 10) | (perm & 0b1111);

        offset += PGSIZE;
    }

    return;
}