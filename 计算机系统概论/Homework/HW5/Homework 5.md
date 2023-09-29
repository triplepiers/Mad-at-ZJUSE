

# Homework 5

> 2023.07.17 沈韵沨 3200104392

## Chap 5

![5.37](/Users/shen/Desktop/5.37.png)

Elements implement the STI instruction are:

- IR, RegFile, SEXT(with IR[8:0])
- PC, ADDR2MUX, ADDR1MUX(set to PC), Adder(connected with the former 2 components)
- MARMUX, GateMARMAX, MAR
- Memory, MDR

---

![5.39](/Users/shen/Desktop/5.39.png)

Elements implement the LEA instruction are:

- IR, Reg File, SEXT(with IR[8:0])
- PC, ADDR2MUX, ADDR1MUX(set to PC), Adder(connected with the former 2 components)
- MARMUX, GateMARMAX, MAR

<div style="page-break-after: always;"></div>

## Chap 6

![6.24.1](/Users/shen/Desktop/6.24.1.png)

![a.6.24](/Users/shen/Desktop/a.6.24.jpeg)

<div style="page-break-after: always;"></div>

## Chap 7

<img src="/Users/shen/Desktop/7.32.1.png" alt="7.32.1" style="zoom:50%;" />

![7.32.2](/Users/shen/Desktop/7.32.2.png)

<div style="page-break-after: always;"></div>

1. Construct the symbol table:

| Symbol | Address |
| :----: | :-----: |
|  SKIP  |  x8009  |
|   A    |  x800A  |
|   B    |  x800B  |
| BANNER |  x800C  |
|   C    |  x800D  |

2. Show the result of assembly of line 5 through 7 above:

| Address |        Binary        |   Description   |
| :-----: | :------------------: | :-------------: |
|  x8006  | `0010 001 000000011` |    LD R1, #3    |
|  x8007  | `0000 010 000000001` |     BRz #1      |
|  x8008  | `0011 000 000000010` |    ST R0, #2    |
|  x8009  | `1111 0000 00100101` | [SKIP] Trap x25 |

3. Explain the difference between line 7 causing the value 5 to be location B and line 10 causing the value 5 to be in location B:

   - Line 7: *Fetch* data from register R0 first(which is value 5), then *store* the value to memory location B.

   - Line 10: Pseudo-Op `.FILL` *initialized* location B to the value #5.

---

<img src="/Users/shen/Desktop/7.34.png" alt="7.34" style="zoom:50%;" />

<img src="/Users/shen/Desktop/a.7.34.jpeg" alt="a.7.34" style="zoom: 33%;" />