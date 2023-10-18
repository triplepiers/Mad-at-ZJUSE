# Lab 4：RV64 虚拟内存管理

---

​                专业：软件工程   日期：2022年9月21日

---

## 1 实验目的

* 学习虚拟内存的相关知识，实现物理地址到虚拟地址的切换。
* 了解 RISC-V 架构中 SV39 分页模式，实现虚拟地址到物理地址的映射，并对不同的段进行相应的权限设置。

## 2 实验环境

* Ubuntu 22.04.1 LTS Windows Subsystem for Linux 2（Environment in previous labs）

## 3 实验步骤

### 3.1 准备工程

1. 在 `defs.h`  中添加如下内容：

   ```c
   #define OPENSBI_SIZE (0x200000)
   
   #define VM_START (0xffffffe000000000)
   #define VM_END   (0xffffffff00000000)
   #define VM_SIZE  (VM_END - VM_START)
   
   #define PA2VA_OFFSET (VM_START - PHY_START)
   ```


2. 从 `repo`  同步代码 `vmlinux.lds.S` & `Makeflie`。

   ​        其中：`vmlinux.lds.S` 模板用于生成 `vmlinux.lds` 文件。链接脚本中的 `ramv` 代表 `VMA(Virtual Memoery Address)` ，即“虚拟地址”； `ram`  代表 `LMA(Load Memory Address)` ，即 OS Image 被 load 的地址，可理解为物理地址。使用 `vmlinux.lds` 进行编译后，得到的 `System.map` & `vmlinux` 都将采用虚拟地址。

   ​        按照指定路径放置后，项目文件结构如下图所示：

<img src="imgs\file.jpg" alt="file" style="zoom:40%;" />

### 3.2 开启虚拟内存映射

​	在 `RISC-V` 中，虚拟地址的开启分为 `setup_vm` 与 `setup_vm_final` 两步，下面将依次进行实现。

#### 3.2.1 `setup_vm` 的实现

* 将 0x80000000 开始的 1GB 区域进行两次映射，其中一次是等值映射 ( PA == VA ) ，另一次是将其映射至高地址 ( PA + PV2VA_OFFSET == VA )。如下图所示：

<img src="imgs\1-1.jpg" alt="file" style="zoom:50%;" />

`RISC-V Virtual Memory System(Sv39)` 中的相关存储结构定义如下：

其中，物理地址共计56位：

* `PRN[2]~PRN[0]` 代表每级页表的**物理页号**
* `page offset(低12位)` 代表页内偏移

```
     55                30 29        21 20        12 11                           0
     -----------------------------------------------------------------------------
    |       PPN[2]       |   PPN[1]   |   PPN[0]   |          page offset         |
     -----------------------------------------------------------------------------
                                Sv39 physical address
```

虚拟地址共计64位，但只有低39位有效：

* 其 63-39 位为'0'时，表示 `user space address`；为'1'时表示 `kernel space address`
  且 63-39 位必须与 38 位相等，否则认为该虚拟地址不合法。
* `VPN[2]~VPN[0]` 代表每级页表的**虚拟页号**
* `page offset(低12位)` 代表页内偏移

```
         38        30 29        21 20        12 11                           0
         ---------------------------------------------------------------------
        |   VPN[2]   |   VPN[1]   |   VPN[0]   |          page offset         |
         ---------------------------------------------------------------------
                                Sv39 virtual address
```

页表项  `Page Table Entry(PTE)` 共计64位：

```
         63      54 53        28 27        19 18        10 9   8 7 6 5 4 3 2 1 0
         -----------------------------------------------------------------------
        | Reserved |   PPN[2]   |   PPN[1]   |   PPN[0]   | RSW |D|A|G|U|X|W|R|V|
         -----------------------------------------------------------------------
```

其中 9-0 位 为`protection bits`

* `V` : 有效位，当 `V = 0`, 访问该 `PTE` 会产生 `Pagefault`。
* `R` : `R = 1` 该页可读。
* `W` : `W = 1` 该页可写。
* `X` : `X = 1` 该页可执行。
* `U` , `G` , `A` , `D` , `RSW` 本次实验中设置为 0 即可。

---

​    由于在 `setup_vm` 中，我们只需实现 1GB 的映射（需要 30 bit），因此我们只需使用 `PRN[2]` 作为索引（单级页表），并使用物理地址的后 30位 作为 `page offset` 。

​	基于上述存储结构，我们通过以下方式实现线性映射：

1. 从虚拟地址 `va` 中获取虚拟页号 `vpn = (va >> 30) & 0x1ff;`
2. 从物理地址 `pa`  中获取物理页号 `ppn = (pa >> 30) & 0x3ffffff;`

3. 为页表项赋值 `early_pgtbl[vpn] = (ppn<<28) | 15;`
   其中 `| 15` 表示将页表项 低4位（V | R | W | X 位）置 '1'

补全后的 `setup_vm()` 函数逻辑如下：

```c
// arch/riscv/kernel/vm.c

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
```

---

* 通过 `relocate` 函数，完成对 `satp` 的设置，以及跳转到对应的虚拟地址。

在 Sv39 中，`satp` 寄存器（Supervisor Address Translation and Protection Register）的结构如下：

```
		 63      60 59                  44 43                                0
		 ---------------------------------------------------------------------
		|   MODE   |         ASID         |                PPN                |
 		 ---------------------------------------------------------------------
```

* `ASID` 字段 ( Address Space Identifier ) ： 此次实验中直接置 0 即可。

* `PPN` 字段 ( Physical Page Number ) ：顶级页表的物理页号。

  由于我们的物理页的大小为 4KB，因此：`PA >> 12 == PPN`。

* 在 RV64 架构下，`Mode` 字段取值含义如下：

  由于我们在本次实验中采用 Sv39，故 `Mode` 应置为 8。

  ```
               ----------------------------------------------------------
              |  Value  |  Name  |             Description               |
              |----------------------------------------------------------|
              |    0    | Bare   | No translation or protection          |
              |  1 - 7  | ---    | Reserved for standard use             |
              |    8    | Sv39   | Page-based 39 bit virtual addressing  |
              |    9    | Sv48   | Page-based 48 bit virtual addressing  |
              |    10   | Sv57   | Page-based 57 bit virtual addressing  |
              |    11   | Sv64   | Page-based 64 bit virtual addressing  |
              | 12 - 13 | ---    | Reserved for standard use             |
              | 14 - 15 | ---    | Reserved for standard use             |
               ----------------------------------------------------------
  ```

补全后的 `relocate` 函数逻辑如下：

```
# arch/riscv/kernel/head.S

relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)

    # ------------------

        li t0, PA2VA_OFFSET
        add ra, ra, t0 # set ra = ra + PA2VA_OFFSET
        add sp, sp, t0 # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
    
    # ------------------

    # set satp with early_pgtbl

    # ------------------

        # Mode = 8
        addi t0, x0, 8     # t0 = 8
        slli t0, t0, 60    # 令 63~60 bit 上的 Mode = 8
        # 因为物理页大小为4KB, 故 PPN = PA >> 12
        la t1, early_pgtbl # 加载顶级页表地址
        srli t1, t1, 12    # t1 = PPN
        or t0, t0, t1      # 拼合 Mode & PPN
        csrw satp, t0      # 设置 satp

    # ------------------

    # flush tlb
    sfence.vma zero, zero

    # flush icache
    fence.i

    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    ...
```

**至此我们已经完成了虚拟地址的开启，之后我们运行的代码也都将在虚拟地址上运行。**

#### 3.2.2 `setup_vm_final` 的实现

​	由于申请页面接口（`set_up_final`）需要在内存管理初始化后进行，我们对 `mm.c` 进行修改，使得初始化函数接受的起始结束地址调整为虚拟地址。

​	修改后的 `mm_init()` 函数逻辑如下：

```c
// arch/riscv/kernel/mm.c

void mm_init(void) {
    // 初始化函数接收的起始结束地址更改为虚拟地址 PHY_END -> PHY_END+PA2VA_OFFSET
    kfreerange(_ekernel, (char *)(PHY_END + PA2VA_OFFSET)); 
    printk("...mm_init done!\n");
}
```

​	本环节采用三级页表映射，对所有物理内存（128M）进行操作并设置正确权限。不需要进行等值映射，不需要将 OpenSBI 映射至高地址（**OpenSBI 运行在 M态，使用物理地址**）。

<img src="imgs\1-2.jpg" alt="file" style="zoom:50%;" />

​	我们首先实现函数 `create_mapping()`  以创建三级页表映射关系，该函数共有五个参数：

* `uint64 *pgtbl`：根页表的基地址
* `uint64 va`：需要映射的虚拟地址
* `uint64 pa`：需要映射的物理地址
* `uint64 sz`：映射的大小
*  `int perm`：映射的读写权限

<img src="imgs\1-3.jpg" alt="file" style="zoom:40%;" />

​	该函数执行的大致流程如下：

1. 处理三级页表
   * 通过 `va` 获取三级虚拟页号 `VPN[2]`
   * 从三级页表中找出对应的页表项 `PTE[2]`
   * 检查页表项 `PTE[2]` 是否有效（`V == 1`）
     * 若有效，进入下一步
     * 若无效，则申请一个新的页面，将其 **物理地址** +  V = 1 作为新的 `PTE[2]`
2. 处理二级页表
   * 通过 `PTE[2]` 获取二级页表的物理地址 
   * 通过 `va` 获取二级虚拟页号 `VPN[1]`
   * 从二级页表中找出对应的页表项 `PTE[1] = tbl[1][VPN[1]]`
   * 检查页表项 `PTE[1]` 是否有效（`V == 1`）
     * 若有效，进入下一步
     * 若无效，则申请一个新的页面，将其 **物理地址** +  V = 1 作为新的 `PTE[1]`
3. 处理一级页表
   * 通过 `PTE[1]` 获取一级页表的物理地址 
   * 通过 `va` 获取一级虚拟页号 `VPN[0]`
   * 根据物理地址 `pa` 与传入的权限 `perm` 改写一级列表项，使得
     `tbl[0][VPN[0]] = (((pa + offset) >> 12) << 10) | (perm & 0b1111);`

​	补全后的 `create_mapping()` 函数逻辑如下：

```c
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
```

---

​	在 `setup_vm()` 中，我们构造了一个简单的映射，使得内核能够运行在虚拟空间上。但不同程序段之间的访问权限是不同的，将所有页表项均标记为 `W = 1` 将产生极大隐患，因此在 `setup_vm_final()` 中，我们对下列段分别使用 `create_mapping()` 进行重映射（虽然每个段还是以同样的偏移量映射到相同的地方，但访问权限都得到正确的设置）：

```
        0x80000000 ┐         ┌------ Kernel -----┐
                    --------------------------------------------
                    | OpenSBI | .text | .rodata | other memory |
                    -------------------------------------------- 
                    └---┬---┴---┬--┴---┬----┴------┬------┘
                    OPENSBI_SIZE  │(_sdata - _srodata) │           
                         (_srodata - _stext)   (PHY_SIZE - size(.text + .rodata))
                         
                              其中 other memmory 的结构如下:
                              
                ┌ _sdata                                         PHY_END ┐
                -----------------------------------------------------------
                | kernel.data | kernal.bss | rest of 128M phisical memory |
                -----------------------------------------------------------
```

1. `.text` 段：

   用于存放代码，`X|-|R|V`

2. `.rodata` 段：

   用于存放**只读**数据，`-|-|R|V`

3. `.data`  & `.bss`  段 - 此处均归为 `other memory` ：

   `.data` 用于存放经过初始化的数据，`.bss`  用于存放经过零初始化的数据，`-|W|R|V`

---

​	由于我们使用了新的页表，下面需要对 `satp` 寄存器进行更新，同时刷新 TLB 与 icache。此处采用内联汇编的形式实现与 `relocate()` 函数类似的功能。值得注意的是，`swapper_pg_dir` 为虚拟地址，`swapper_pg_dir - PA2VA_OFFSET ` 方为其物理地址，该部分实现如下：

```c
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
    :[addr] "r" ((unsigned long)swapper_pg_dir-PA2VA_OFFSET)
    :"memory"
);

// flush TLB
asm volatile("sfence.vma zero, zero");

// flush icache
asm volatile("fence.i");
```

​	补全后的 `setup_vm_final()`  函数逻辑如下：

```c
// arch/riscv/kernel/vm.c 

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
```

​	最后，我们在 `head.S` 中的适当位置调用 `setup_vm_final` 

### 3.3 编译及测试

​	为使得测试输出于输出示例一致，在此对 `proc.c` 中 `dummy()` 函数的输出提示做如下更改：

```c
printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
                                            ↓
                                            ↓
                                            ↓
printk("[PID = %d] is running. thread space begin at 0x%lx\n", current->pid, (unsigned long)current);
```

​	测试输出如下图所示：

<img src="imgs\1-4.jpg" alt="file" style="zoom:40%;" />

## 4 思考题

1. **验证 `.text`, `.rodata` 段的属性是否成功设置，给出截图。**

   `.text` 段具有：可执行，可读权限；`.rodata` 段具有：可读权限。

   * 由于编译后的程序能够正常执行，显然 `.text` 段**具有可执行权限**。

   * 下面验证对两个数据段的**读权限**。在使用 `setup_vm_final()` 重新对各段的权限进行设置后，使用以下代码进行检验：

     ```c
     printk("[checking permission]...\n");
     printk("For [.text]\n");
     printk("_stext = %ld\n", *_stext);
     printk("For [.rodata]\n");
     printk("_srodata = %ld\n", *_srodata);
     ```

     执行结果如下图所示，显然我们**具备**对这两个数据段的**读权限**

     <img src="imgs\2-1.jpg" alt="file" style="zoom:67%;" />

   * 下面验证对两个数据段的**写权限**。在使用 `setup_vm_final()` 重新对各段的权限进行设置后，分别使用以下代码进行检验：

     ```c
     printk("[checking permission]...\n");
     printk("For [.text]\n");
     printk("[berfore W] _stext = %ld\n", *_stext);c
     *_stext = 11;
     printk("[after W]   _stext = %ld\n", *_stext);
     ```

     <img src="imgs\2-2.jpg" alt="file" style="zoom:67%;" />

     ```c
     printk("[checking permission]...\n");
     printk("For [.rodata]\n");
     printk("[berfore W] _srodata = %ld\n", *_srodata);
     *_srodata = 11;
     printk("[afterW]    _srodata = %ld\n", *_srodata);c
     ```
     
     <img src="imgs\2-3.jpg" alt="file" style="zoom:67%;" />
     
     两段程序均未能成功执行写操作，显然我们**不具备**对 `.text` 和  `.rodata` 段的**写权限**。

   ---

   综上所述，我们成功设置了 `.text`, `.rodata` 段的属性。

2. **为什么我们在 `setup_vm` 中需要做等值映射?**

   ​	首先，CPU发出的所有内存访问指令都是虚拟地址，都需要通过 `satp` 中存储的顶级页表信息转换成对应的物理地址。

   ​	因为在使用 `create_mapping()` 函数创建三级页表时，我们需要读取页表项 `PTE` 中存储的物理页号 `PPN` 并将其转换为物理地址从而对二级页表与一级页表进行访问，下面是一个例子。

   * 当我们进行两次映射 `PA == VA ` + `PA + PV2VA_OFFSET == VA` 时，在 `early_pgtbl` 存在：

     `(va)ffffffe007fff000 == (va)0000000087fff000 == (pa)0000000087fff000`

     在使用 `pgtbl[1] = (unsigned long*)((pte[2] >> 10) << 12)` 得到二级页表 `pgtbl[1]` 的虚拟地址为 `0000000087fff000` 时，可以顺利的访问对应的物理地址。

   * 但当我们仅作 `PA + PV2VA_OFFSET == VA` 映射时，在 `early_pgtbl` 仅存在：

     `(va)ffffffe007fff000 == (pa)0000000087fff000`

     此时 `early_pgtbl` 并不知道 `(va)0000000087fff000` 所对应的物理地址，这将导致内存访问错误。

   > 新的根页表 `swapper_pg_dir` 通过分配得到的是一个高位的虚拟地址（`ffffffe00?`），即便只进行一次映射也可以正常访问。

3. **在 Linux 中，是不需要做等值映射的。请探索一下不在 `setup_vm` 中做等值映射的方法。**

   ​	从 2) 中可以看出，取消 `setup_vm()` 中等值映射后可能出现的原因在于：

   * 首次设置 `satp` 寄存器后，`PC` 指向一个低地址，此时的 `CPU` 将认为这是一个“虚拟地址”，但无法找到其对应的有效物理地址，导致程序寄掉。
     这里我们需要将 `PC` 寄存器指向的地址后移 `PA2VA_OFFSET`  的长度，以使其能够正常运行。
   
   * 通过 `PTE`  获取的 `PPN` 得到的地址无法访问到正确的物理地址（在缺少等值映射的情况下，我们并不知道等值的 `va` 与 `pa` 之间的对应关系）。
   
   ​	因此，我们需要将通过 `PTE`  获取的 `PPN` 得到的地址转换为“高位”虚拟地址，使其能够被正常映射。即：
   
   ```
   已知 PA + PV2VA_OFFSET <---> VA in [VM_START, VM_END]
   由 (PTE >> 10) << 12 可以得到 VA_1 in [PHY_START - PHY_END]
   -> 我们需要将 VA_1 转化到区间 [VM_START, VM_END] 上
      即，使 VA_2 = VA_1 + PV2VA_OFFSET
      通过 VA_2 与 PA 间的映射关系即可访问正确的物理地址
   ```
   
   ​	下面是实现：
   
   * 对于 `setup_vm()` 我们去除等值映射部分：
   
     ```c
     void setup_vm(void) {
     
         memset(early_pgtbl, 0x0, PGSIZE); // 初始化大小为4KB的顶级页表
        
         unsigned long pa, va;
         // 仅作一次 PA + PV2VA_OFFSET == VA 的映射
         pa = PHY_START;
         va = VM_START;
         int vpn = (va >> 30) & 0x1ff;      // 取虚拟页号：va的38-30 (9 bit)
         int ppn = (pa >> 30) & 0x3ffffff;  // 取物理页号，pa的55-30 (26 bit)
         early_pgtbl[vpn] = (ppn<<28) | 15;
     }
     ```
   
   * 对于 `create_mapping()` 仅有访问二级&一级页表将会受到影响，我们仅作以下更改：
   
     ```c
     void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm) {
     		...
             tbl[1] = (unsigned long*)(((PTE[2] >> 10) << 12) + PA2VA_OFFSET);
     		...
             tbl[0] = (unsigned long*)(((PTE[1] >> 10) << 12) + PA2VA_OFFSET);
     		...
     }
     ```
   
     通过编译测试可以发现，本方法实验效果与进行两次映射得到的结果相同。

## 5 心得体会

​	本次实验加深了我对于 Sv39 分配方案下物理地址、虚拟地址及页表项结构的熟悉程度，同时对于多级页表的实现方式、不同程序段之间的权限划分方法有了更加直观的体验。

​	同时，实验本体和思考题也引导我进行了更为深入的思考：为什么 `setup_vm()` 之后还需要 `setup_vm_final()` ？为什么需要进行等值映射，去掉等值映射后又该如何实现相同的效果？这些问题推动着我不断查阅相关资料并进行思考，随后茅塞顿开：

*  `setup_vm()` 构造的粗糙映射虽然使得内核能够运行在虚拟空间上，但过于粗糙的权限管理将会造成极大隐患（所有的程序段都是可读、可写并可执行的），因此我们需要 `setup_vm_final()` 来实现一个偏移量相同，但更加精细的重映射。
* 等值映射使得直接计算页表项得到的地位虚拟地址与等值的物理地址相互联系，而去除之后，我们得想办法把这个“低位”的虚拟地址变换为存在映射关系的“高位”虚拟地址，从而正确的访问物理地址。

​	另外深受实验指导影响，开始在代码块里用字符码一些说明图，感到十分快乐！

​	总之收获颇丰。
