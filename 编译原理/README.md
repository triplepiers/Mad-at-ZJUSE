# README

- 本次实验完成了一个类C语言编译器，支持将其编译至 LLVM IR，随后通过 LLVM 编译至 x86_64 汇编语言，并使用 GCC 生成最终的可执行文件。

- 本项目通过 CMake 搭建工程，同时提供一系列 make 命令用于快速生成目标语言文件与可执行文件。

- 本项目使用 Echarts 提供的树状图实现抽象语法树的可视化。

## 1 环境与依赖

- 运行环境

  - Ubuntu 20.04（架构为 x86_64）
  - GCC 9.0.4
  - Bison 3.5.1
  - Flex 2.6.4

- 依赖
  - Cmake 3.16.3
  - LLVM 10.0.0
  - VSCode 中的 Live Server 插件

## 2 项目运行

本项目使用 Cmake 构建，可以使用以下任意一种方式编译运行：

1. 自行逐步编译运行

	```bash
	cd src
	make all    # 使用 flex & bison 处理对应文件

	cd ../
	mkdir build # 在根路径下创建目录
	
	cd build
	cmake ..
	make # 本路径下将产生用于将 .c 文件翻译为中间代码的可执行文件 Compiler

	# 输入指定的类C语言代码，输出 LLVM IR 中间代码文件
	cat path/to/XXX.c | ./Compiler > path/to/XXX.ll

	# 生成 LLVM 字节码文件 (.bc)
	llvm-as path/to/XXX.ll

	# 生成指定架构的目标代码 (.s)
	llc --march=riscv64 path/to/XXX.bc

	# 使用 GCC 进行汇编
	gcc -c path/to/XXX.s
	# 使用 GCC 链接形成可执行文件
	gcc test.o -o path/to/XXX.out --static
	# 运行可执行文件，验证结果
	path/to/XXX.out
	```

2. 使用提供的 `make` 命令

	```bash
	# 在根路径下执行
	make all  # 生成可执行文件 Compiler

	# 由 /test/test.c 生成中间代码、目标代码，并最终形成可执行文件
	# 可在 /test/test.s 中查看生成的 x86_64 目标代码（及其他中间产物）
	# 可运行 /visual/index.html 以查看 AST 可视化结果
	make test 

	# 其他 make 命令如下
	make build     # 仅生成至中间代码(.ll)

	make clean     # 清空所有构建产物
	make clean_tes # 仅清空对 test.c 的处理结果
	```