# Homework 1

> 2023.07.12 沈韵沨 3200104392

## Chap 1

![1.2](/Users/shen/Desktop/1.2.png)

No. 

Since both higher-level programming language & lower-level programming language would be ultimately translated into the machine language (the lowest), they have the same ability to instruct a computer to computer.

---



![1.4](/Users/shen/Desktop/1.4.png)

The *ambiguity*.

According to the textbook, the sentence "Time flies like an arrow" has at least 3 possible interpretations.

=> Those ambiguity-caused multiple interpretations would cause the computer to not know which interpretation to follow, preventing natural languages from being used as programming languages.

---

![1.10](/Users/shen/Desktop/1.10.png)

1. `Detiniteness`：Each step should be precisely stated.
2. `Effective Computability`： Each step can be carried out by a computer。
3. `Finiteness`：The procedure could ultimately terminate.

---

![1.16](/Users/shen/Desktop/1.16.png)

1. The interface between the computer program directing the computer hardware and the hardware carrying out those directions.
2. The set of instructions that the computer can carry out.
3.  `data type`：The acceptable representations for operands.
4. `addressing mode`：The mechanisms that the computer can use to figure out where the operands are located.
5. The number of unique locations that comprise the computer’s memory
6. The number of individual 0s and 1s that are contained in each location.

---

![1.18](/Users/shen/Desktop/1.18.png)

- A single microarchitecture could normally implemente **ONLY ONE** ISA.
- A single ISA could be implemented by **MANY** microarchitectures (which means INFINITE).

## Chap 2

![2.4](/Users/shen/Desktop/2.4.png)

- `n bits` can represent 2^n^ unsigned integers.
- The range of these integers is  $[0\ ,\ 2^n-1]$

---

![2.8](/Users/shen/Desktop/2.8.png)

a. The largest positive number can be represented by 8-bit 2's complement code is:
$$
2^{8-1} - 1 = (127)_{Decimal} \ \& \ (0111 \ 1111)_{Binary}
$$
b. The greatest magnitude negative number can be represented by 8-bit 2's complement code is:
$$
- 2^{8-1} = (-128)_{Decimal} \ \& \ (1000 \ 0000)_{Bianry}
$$
c. The largest positive number can be represented in n-bit 2's complement code is:
$$
(2^{n-1}-1)_{Decimal}
$$
d. The greatest magnitude negative number can be represented in n-bit 2's complement code is:
$$
(-2^{n-1})_{Decimal}
$$

---

![2.17](/Users/shen/Desktop/2.17.png)

a. 01 + 1011 =
$$
\begin{align*}
&= 0001 + 1011 \\
&= (1100)_{Binary} = (-4)_{Decimal}
\end{align*}
$$
b. 11 + 01010101 = 
$$
\begin{align*}
&= 1111 \ 1111 + 0101 \ 0101 \\
&= (0101 \ 0100)_{Binary} = (84)_{Decimal}
\end{align*}
$$
c. 0101 + 110 = 
$$
\begin{align*}
&= 0101 + 1110 \\
&= (0011)_{Binary} = (3)_{Decimal}
\end{align*}
$$
d. 01 + 10 =
$$
= (11)_{Binary} = (-1)_{Decimal}
$$

---

![2.20](/Users/shen/Desktop/2.20.png)

| index | overflow | operands | result |
| :---: | :------: | :------: | :----: |
|   a   |    No    | (-4) + 3 |   -1   |
|   b   |    No    | (-4) + 4 |   0    |
|   c   |   Yes    |  7 + 1   |   -8   |
|   d   |   Yes    | (-8) - 1 |   7    |
|   e   |    No    | 7 + (-7) |   0    |

---

![2.34](/Users/shen/Desktop/2.34.png)

a. NOT(1011) OR NOT(1100)
$$
\begin{align*}
&= 0100 \ or \ 0011 \\
&= 0111
\end{align*}
$$
b. NOT(1000 AND (1100 OR 0101))
$$
\begin{align*}
&= not(1000 \ and \ 1101) \\ 
&= not(1000) \\ 
&= 0111
\end{align*}
$$
c. NOT(NOT(1101))
$$
\begin{align*}
&= not(0010) \\
&= 1101
\end{align*}
$$
d. (0110 OR 0000) AND 1111
$$
\begin{align*}
&= 0110 \ and \ 1111 \\
&= 0110
\end{align*}
$$