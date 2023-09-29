# Homework 4

> 2023.07.15 沈韵沨 3200104392

![5.2](/Users/shen/Desktop/5.2.png)

- The size of `MDR` should be 64 bit.
  The *addressability* represents the number of bits stored at each memory location and 64 bits addressability means each memory location is 64 bits long.
  As the MDR has the same size as the memory location, it should be 64 bits long.
- The size of `MAR` could not be determined.
  The size of MAR is related with the *address space*(the **total number** of memory locations). We can't determine its length from the given information.

---

![5.4](/Users/shen/Desktop/5.4.png)

a. 
$$
\because log_2(256) = 8 \\
\therefore 8\ bits\ are\ required\ for\ the\ address
$$
b. If we want to allow control transfer between instructions 20 locations away, then a branch instruction needs at least **6 bits** to specify PC-relative offset. The reasons are as follows:

​	(1) PC-offsets shold be represented by 2's complement.

​	(2) Since PC has be incremented in FETCH phase, k bits offsets could transfer between $[-(2^{k-1}-1), + 2^{k-1}]$ (relavated to current instruction).
$$
According\ to\ (2),\ we\ have
\left\{
\begin{align*}
& 2^{k-1} \geq 20 \\
& \left| -(2^{k-1}-1)\right| \geq 20
\end{align*}
\right. \\
\Rightarrow k \geq \lceil log_2(21) \rceil = 6
$$


c. Since PC has be incremented in FETCH phase:
$$
10 = PC + offset = (curLocation +1) + offset \\
\Rightarrow offset = 10 - (curLocation+1) = 10-(3+1) = 6
$$


---

![5.9](/Users/shen/Desktop/5.9.png)

**Instruction (c)** could be used for NOP, the reason are as follows:

- Instruction (a) means `ADD R1, R1, #0`. It doesn't change the value stored in R1, but would affect the condition code.
  => If we do nothing in EXECUTION phase, it might affect the result of some BR instruction after if.
- Instruction (b) means `BRnzp #1`, which means an **unconditionally** branch.
  => If we do nothing in EXECUTION phase, it would certainlly affect the control flow.
- Instruction (c) means `BR #0`.
  => Since every integer must be negative/zero/positive, the branch would **never taken**, thus this instruction could be used for NOP & the program still work correctly.

---

![5.15](/Users/shen/Desktop/5.15.png)

| Address(Hex) |   Instruction    |                          Desc                           |
| :----------: | :--------------: | :-----------------------------------------------------: |
|   `x3100`    |  `LEA R1, #32`   |                R1 = x3101 + x20 = x3121                 |
|   `x3101`    |   `LD R2, #32`   |         R2 = M[x3102 + x20] = M[x3122] = x4566          |
|   `x3102`    |  `LDI R3, #32`   | R3 = M[M[x3103 + x20]] = M[M[x3123]] = M[x4567] = xABCD |
|   `x3103`    | `LDR R4, R2, #1` |           R4 = M[R2 + x1] = M[x4567] = xABCD            |
|   `x3104`    |    `TRAP #37`    |                        **HALT**                         |
|     ...      |                  |                                                         |
|   `x3122`    |      x4566       |                                                         |
|   `x3123`    |      X4567       |                                                         |
|     ...      |                  |                                                         |
|   `x4567`    |      xABCD       |                                                         |
|   `x4568`    |      xFED3       |                                                         |

According to the table above, the contents of each register after the program starting at `x3100` halts are:

| Register | Content |
| :------: | :-----: |
|    R1    | `x3121` |
|    R2    | `x4566` |
|    R3    | `xABCD` |
|    R4    | `xABCD` |

---

![截屏2023-07-15 21.18.09](/Users/shen/Desktop/截屏2023-07-15 21.18.09.png)

a. `PC-Relative` Mode

- PC-Relative mode use 9 bits offset, which cover the range $[-(2^8-1), +2^8]$. Since the destination is less than $ \pm 2^8$ away, this mode can satisfy the request.
- Although `Indirect` mode also works, the `PC-Relative` mode is faster since it only visit memory ONCE (`Indirect` mode need TWICE, 1 for address & 1 for data).

=> Thus, `PC-Relative` Mode is the most sense one under this situation.

b. `Indirect` Mode

|      Mode       |                Range                |
| :-------------: | :---------------------------------: |
| `Base + offset` |      Base + $[-(2^5-1), +2^5]$      |
|  `PC-Relative`  | curInstruction + $[-(2^8-1), +2^8]$ |
|   `Indirect`    |       Anywhere in the Memory        |

=> Since only the `Indirect` mode can satisfy the request, it must be the most sense one.

> In fact,  `Base + offset` mode also allow 

c. `Base + offset` Mode

- If the destination address is close, then  `PC-Relative` mode is faster.
- Buf if the destination address is far away,  `Base + offset` mode works better —— it can just modify offset in the instruction to visit those sequential address.
   `Indirect` mode requires us to modify the content of DR, which needs more instructions.