# Lab 0： GDB & QEMU 调试64位 RISC-V LINUX

---

​                专业：软件工程   日期：2022年9月21日

---

## 实验目的

* 使用交叉编译工具，完成Linux内核代码编译
* 使用QEMU运行内核
* 熟悉GDB和QEMU联合调试

## 实验环境

* Ubuntu 22.04.1 LTS Windows Subsystem for Linux 2

## 实验步骤

### 搭建实验环境

#### 安装编译内核所需要的交叉编译工具链和用于构建程序的软件包

![4-1-1](D:\操作系统\lab0_report\imgs\4-1-1.jpg)

<center>如图，我们成功安装了 gcc-riscv64-linux-gnu 编译器</center>

![4-1-22](D:\操作系统\lab0_report\imgs\4-1-22.jpg)

<center>如图，我们成功安装了编译riscv交叉编译工具链所必需的库</center>

#### 安装用于启动 riscv64 平台上的内核模拟器 `	QEMU`	

![4-1-31](D:\操作系统\lab0_report\imgs\4-1-31.jpg)

<center>如图，我们成功安装了QEMU</center>

#### 安装 `GDB` 

![4-1-4](D:\操作系统\lab0_report\imgs\4-1-4.jpg)

<center>如图，我们成功安装了gdb-multiarch </center>

### 获取linux源码和已经编译好的文件系统

#### 下载最新的Linux源码

![download](D:\操作系统\lab0_report\imgs\download.jpg)

<center>首先，从https://www.kernel.org下载最新的Linux源码（6.0-rc5版本）</center>

![unload](D:\操作系统\lab0_report\imgs\unload.jpg)

<center>通过tar -zxvf命令，将tar.gz文件解压到当前文件夹下</center>

#### 使用 git 工具 clone 仓库

![4-2-1](D:\操作系统\lab0_report\imgs\4-2-1.jpg)

<center>使用 git 工具将仓库 clone 到当前目录下</center>

![4-2-2](D:\操作系统\lab0_report\imgs\4-2-2.jpg)

<center>进入子目录 lab0，通过 ls 命令确定该目录下存在 rootfs.img 文件</center>

### 编译linux内核

![path](D:\操作系统\lab0_report\imgs\path.jpg)

<center>通过 cd 命令，将当前路径切换为 linux 源码所在目录</center>

![4-3-1](D:\操作系统\lab0_report\imgs\4-3-1.jpg)

<center>使用riscv平台的默认配置</center>

![4-3-21](D:\操作系统\lab0_report\imgs\4-3-21.jpg)

![4-3-22](D:\操作系统\lab0_report\imgs\4-3-22.jpg)

<center>使用全部CPU内核对riscv平台内核进行编译</center>

### 使用QEMU运行内核

![4-4-1](D:\操作系统\lab0_report\imgs\4-4-1.jpg)

<center>使用QEMU启动Linux内核，并加载根文件系统镜像</center>
<center>(使用命令行；使用OpenSBI firmware作为bootloader；指定要模拟的机器、内核、文件系统)</center>

![4-4-2](D:\操作系统\lab0_report\imgs\4-4-2.jpg)

<center>Ctrl + A + X，QEMU 显示 Terminated，退出成功</center>

### 使用GDB对内核进行调试

![4-5-2](D:\操作系统\lab0_report\imgs\4-5-2.jpg)

右侧终端使用 `QEMU` 启动 Linux，左侧终端使用 `GDB` 与 `QEMU` 进行远程通信

1. 使用 `target remote :1234` 指定远程调试的目标主机端口为1234（`QEMU` 开放的远程调试端口）
   `GDB` 显示“Remote debugging using :1234”，连接成功
   <img src="D:\操作系统\lab0_report\imgs\4-5_!.jpg" alt="4-5_!" style="zoom:33%;" />

2. 使用 `b start_kernel` 在 `start_kernel` 函数上设置断点
   `GDB` 显示“Breakpoint 1 at 0xffffffff808006b8”，断点设置成功
   <img src="D:\操作系统\lab0_report\imgs\4-5_2.jpg" alt="4-5_2" style="zoom:33%;" />

3. 使用 `continue` 继续运行

   `GDB` 显示“Breakpoint 1，0xffffffff808006b8 in start_kernel ()”，成功在2中设置的断点停止运行
   <img src="D:\操作系统\lab0_report\imgs\4-5_3.jpg" alt="4-5_3" style="zoom:33%;" />

4. 使用 `quit` 退出 `GDB` 

   `GDB` 显示“[Inferior 1 (process 1) detached]”，成功退出
   <img src="D:\操作系统\lab0_report\imgs\4-5_4.jpg" alt="4-5_4" style="zoom:33%;" />

调试期间，`QEMU` 虚拟机显示的部分信息如下：

![4-5-3](D:\操作系统\lab0_report\imgs\4-5-3.jpg)

## 思考题

### 使用 `riscv64-linux-gnu-gcc` 编译单个 `.c` 文件

![think-1-1](D:\操作系统\lab0_report\imgs\think-1-1.jpg)

<center>在lab0目录下新建如图的helloworld.c文件</center>

![think-1-2](D:\操作系统\lab0_report\imgs\think-1-2.jpg)

<center>使用riscv64-linux-gnu-gcc将上述文件编译为helloworld</center>

### 使用 `riscv64-linux-gnu-gcc-objdump` 反汇编1中得到的编译产物

![think-2-1](D:\操作系统\lab0_report\imgs\think-2-1.jpg)

<center>使用objdump -d命令对helloworld的代码段进行反汇编，部分结果如下：</center>

![think-2-2](D:\操作系统\lab0_report\imgs\think-2-2.jpg)

### 调试 Linux

#### 在 `GDB` 中查看汇编代码

在使用 `GDB` 远程调试 Linux 时，使用 `x/i $pc` 查看从当前程序运行位置开始的 1 /10 条汇编指令

<img src="D:\操作系统\lab0_report\imgs\think-3-1.jpg" alt="think-3-1" style="zoom:50%;" />

#### 在 0x80000000 处下断点

使用 `break *addr` 命令在指定地址处下断点，运行结果如下：

<img src="D:\操作系统\lab0_report\imgs\think-3-2.jpg" alt="think-3-2" style="zoom:60%;" />

#### 查看所有已下的断点

使用 `info checkpoints` 命令查看所有已下的断点，运行结果如下：

<img src="D:\操作系统\lab0_report\imgs\think-3-3.jpg" alt="think-3-3" style="zoom:33%;" />

从运行结果不难看出，上一步成功在 0x80000000 处设置了断点

#### 在 0x80200000 处下断点

再次使用 `break *addr` 命令在 0x80200000 处下断点，运行结果如下：

<img src="D:\操作系统\lab0_report\imgs\think-3-4.jpg" alt="think-3-4" style="zoom:60%;" />

使用 `info checkpoints` 命令可验证该断点设置成功：

<img src="D:\操作系统\lab0_report\imgs\think-3-5.jpg" alt="think-3-5" style="zoom:33%;" />

#### 清除 0x80000000 处的断点

使用 `delete [breakpoints] [num]` 命令删除编号为 1（0x80000000 处）的断点，并进行验证：

<img src="D:\操作系统\lab0_report\imgs\think-3-6.jpg" alt="think-3-6" style="zoom:33%;" />

显然，0x80000000 处的断点已被成功清除

#### 继续运行直到触发 0x80200000 处的断点

使用 `continue` 命令继续运行程序：

<img src="D:\操作系统\lab0_report\imgs\think-3-7.jpg" alt="think-3-7" style="zoom:36%;" />

由上图可见，程序停在了断点 0x80200000 处

#### 单步调试一次

[未解决] 使用 `next` `step` `until` 命令进行单步调试均未成功（显示“Cannot find current fuctnion”）

<img src="D:\操作系统\lab0_report\imgs\question.jpg" alt="question" style="zoom:33%;" />

#### 退出 `QEMU` 

使用组合键 `Ctrl + A + X` 退出 `QEMU` 

<img src="D:\操作系统\lab0_report\imgs\think-3-8.jpg" alt="think-3-8" style="zoom:33%;" />

### 使用 `make` 工具清除 Linux 的构建产物

此处使用了 `make mrproper` 对构建产物进行清除，运行结果如下：
![clean](D:\操作系统\lab0_report\imgs\clean.jpg)

执行期间曾遇到以下错误，通过手动删除进行了解决：

![err](D:\操作系统\lab0_report\imgs\err.jpg)

### `vmlinux` 和 `Image` 的关系和区别

查阅相关资料后，整合得到以下答案：

`vmlinux` 是由 Linux 内核编译产生的原始内核文件（包含符号信息0，为 elf 格式，未进行压缩。该格式的内核映像可用于定位内核文体，但不能直接引导 Linux 系统启动。

`Image` 则是 Linux 内核在进行编译，使用 objcopy 处理 `vmlinux` 后所产生的二进制映像（去除符号和重定位信息等），同样未经过压缩。该格式的内核映像可直接引导 Linux 内核系统启动。

## 心得体会

​	上学期搭建 minisql 开发环境时图方便直接把 Ubuntu 装在了C盘，导致存储空间暴毙。本学期励精图治，通过查阅资料成功将其导出至D盘，完成对C盘存储空间的抢救。

​	随后因为被不知道 path/to/linux 和 path/to/rootfs.img 的含义卡了一整天，把clone下来的仓库目录和linux源码解压得到的目录翻了一整天都没找到叫 path 的目录，在向旁边同学请教后茅塞顿开，并将相应文件目录存储在便签中备用。

​	在完成思考题时查阅了许多相关资料，如如何对编译产物进行反汇编、如何在GDB中查看汇编代码、如何使用make工具清除构建产物......本次实验的操作过程加强了我对于命令行工具使用的熟练度，同时使得我对于gdb、make等工具的使用有了更加深入的理解，令我受益匪浅。