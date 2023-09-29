# Homework 3

> 2023.07.14 沈韵沨 3200104392

## Chap 3

<img src="/Users/shen/Desktop/3.35.1.png" alt="3.35.1" style="zoom:40%;" />

<img src="/Users/shen/Desktop/3.35.2.png" alt="3.35.2" style="zoom:40%;" />

|      | cycle 0 | cycle 1 | cycle 2 | cycle 3 | cycle 4 | cycle 5 | cycle 6 | cycle 7 |
| :--: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | ------- |
|  D2  |    0    |    1    |    1    |    1    |    1    |    0    |    0    | 0       |
|  D0  |    0    |    1    |    1    |    0    |    0    |    1    |    1    | 0       |
|  D1  |    0    |    1    |    0    |    1    |    0    |    1    |    0    | 1       |

The circuit works as a `3-bit down counter`.

---

<div style="page-break-after:always"></div>

<img src="/Users/shen/Desktop/3.61.1.png" alt="3.61.1" style="zoom:55s%;" />

<img src="/Users/shen/Desktop/3.61.2.png" alt="3.61.2" style="zoom:55%;" />

<div style="page-break-after:always"></div>

a. Truth table for combinationtional logic:

|  S1  |  S0  |  X   |  Z   | S1'  | S0'  |
| :--: | :--: | :--: | :--: | :--: | :--: |
|  0   |  0   |  0   |  1   |  0   |  0   |
|  0   |  0   |  1   |  1   |  0   |  1   |
|  0   |  1   |  0   |  0   |  1   |  0   |
|  0   |  1   |  1   |  0   |  0   |  0   |
|  1   |  0   |  0   |  0   |  0   |  1   |
|  1   |  0   |  1   |  0   |  1   |  0   |
|  1   |  1   |  0   |  0   |  0   |  0   |
|  1   |  1   |  1   |  0   |  0   |  0   |

b. The state machine:

<img src="/Users/shen/Desktop/IMG_B159AC92A2E4-1.jpeg" alt="IMG_B159AC92A2E4-1" style="zoom:30%;" />

<div style="page-break-after:always"></div>

## Chap 4

![4.1](/Users/shen/Desktop/4.1.png)

The five components of the von Neumann model are:

1. `Memory`: Store instructions & data.
2. `Processing Unit`: Execute instructions & process data.
3. `Input`: Load instructions & data.
4. `Output`: Store instructions & data, display result.
5. `Control Unit`:  Keep track of both where we are within the process of executing the program and where we are in the process of executing each instruction, in order to coordination of the work of all components.

---

![4.3](/Users/shen/Desktop/4.3.png)

- The misleading is the register called `program counter` is not a *counter*, but store the **next instruction’s address** in fact.
- The name `instruction pointer` (by Intel) might be more insightful.

---

![4.8](/Users/shen/Desktop/4.8.png)

a. 
$$
\because 
\left\{
\begin{align*}
&n_{OPCODE}=255 \\
&2^7 = 128 < 255 < 256 = 2^8 
\end{align*}
\right. \\
\therefore The \ minimum\ number\ of\ bits\ required\ to\ represent\ the\ OPCODE=8
$$
b.
$$
\because 
\left\{
\begin{align*}
&n_{register}=120 \\
&2^6 = 64 < 255 < 128 = 2^7 
\end{align*}
\right. \\
\therefore The \ minimum\ number\ of\ bits\ required\ to\ represent\ the\ DR=7
$$
c.
$$
According\ to\ a. \&\ b.
\left\{
\begin{align*}
&min(OPCODE) = 8\ bit\\
&min(DR) = 7\ bit \\
&min(SR1) = min(SR2) = min(DR) = 7\ bit
\end{align*}
\right. \\

max(UNUSED) = total - (min(OPCODE) +min(DR) + min(SR1) + min(SR2)) \\
=32 - (8+7+7+7) = 3\ bit \\
\therefore The \ maximum\ number\ of\ UNUSED\ bits\ in\ the\ instruction\ coding\ = 3
$$

---

![4.10.1](/Users/shen/Desktop/4.10.1.png)

![4.10.2](/Users/shen/Desktop/4.10.2.png)

|      | Fetch Instruction      | Decode | Evaluate Address | Fetch Data | Execute | Store Result |
| ---- | ---------------------- | ------ | ---------------- | ---------- | ------- | ------------ |
| PC   | `0001`, `0110`, `1100` |        |                  |            | `1100`  |              |
| IR   | `0001`, `0110`, `1100` |        |                  |            |         |              |
| MAR  | `0001`, `0110`, `1100` |        |                  | `0110`     |         |              |
| MDR  | `0001`, `0110`, `1100` |        |                  | `0110`     |         |              |

