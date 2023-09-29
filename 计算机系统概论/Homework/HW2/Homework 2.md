# Homework 2

> 2023.07.13 沈韵沨 3200104392

## Chap 2

![2.40](/Users/shen/Desktop/2.40.png)

a.
$$
\begin{align*}
Sign &= 0 \\
Exp &= (1000\ 0000)_B - 127_D = 128-127= 1 \\
Frac &= 1.0 \\
& \therefore N_a = (-1)^0*1.0*2^1 = 2.0
\end{align*}
$$
b.
$$
\begin{align*}
Sign &= 1 \\
Exp &= (1000 \ 0011)_B - 127_D = 4 \\
Frac &= 1.0001_B = 1\frac{1}{16} = 1.0625 \\
& \therefore N_b = (-1)^1*1.0625*2^4 = -17
\end{align*}
$$
c.
$$
\because 
\left\{
\begin{align*}
& Sign = 0 \\
& Exp\ containing \ all \ 1's \\
& Frac\ containing \ all \ 0's \\
\end{align*}
\right. \\
\therefore N_c = Positive \ Infinity
$$
d.
$$
\begin{align*}
Sign &= 1 \\
Exp &= (1000 \ 0000)_B - 127_D = 1 \\
Frac &= 1.1001_D = 1\frac{25}{16} = 1.5625 \\
& \therefore N_d = (-1)^1*1.5625*2^1 = -3.125
\end{align*}
$$

---

![2.48](/Users/shen/Desktop/2.48.png)

a.
$$
\begin{align*}
256 &= 0000 \ 0001 \ 0000 \ 0000(2's\ complement) \\
&= 0100(hexadecimal)
\end{align*}
$$


b.
$$
\begin{align*}
111 &= 0110\ 1111(2's\ complement) \\
&= 6F(hexadecimal)
\end{align*}
$$
c.
$$
\begin{align*}
123456789 &= 0000 \ 01111 \ 0101 \ 1100 \ 1101 \ 0001 \ 0101(2's\ complement) \\
&= 075B \ CD15(hexadecimal)
\end{align*}
$$
d.
$$
\left\{
\begin{align*}
&44 = 0010 \ 1100 \\
&\overline{44} = 1101 \ 0011 \\
&\overline{44}+1 = 1101 \ 0100 \\
\end{align*}
\right. \\
\therefore -44's \ 2's \ complement = 1101 \ 0100 \\
it's\ hexadecimal =D4
$$

## Chap 3

![3.6](/Users/shen/Desktop/3.6.png)

- The truth table:

|  A   |  B   |  C   |  D   |  Z   |
| :--: | :--: | :--: | :--: | :--: |
|  0   |  0   |  1   |  1   |  0   |
|  0   |  1   |  1   |  0   |  0   |
|  1   |  0   |  0   |  1   |  0   |
|  1   |  1   |  0   |  0   |  1   |

$$
Z = \overline{C} \cdot \overline{D} = \overline{\overline{A}} \cdot \overline{\overline{B}} = A \cdot B(A \ and\ B)
$$

---

![3.20](/Users/shen/Desktop/3.20.png)

Since the multiplexer has 16 input & $ log_2(16) = 4$, it sould have:

- ONLY ONE output line.

- 4 select lines.

---

![3.30](/Users/shen/Desktop/3.30.png)

a. 
$$
\left\{
\begin{align*}
&X = 0 \Rightarrow output = A+B \\ 
&X = 1 \Rightarrow output = A+C
\end{align*}
\right.
$$
b.

![a3.30](/Users/shen/Desktop/a3.30.jpeg)
$$
\left\{
\begin{align*}
&X = 0 \Rightarrow output = A+B \\ 
&X = 1 \Rightarrow output = A+(\overline{B}+1) = A +(-B) =A-B
\end{align*}
\right.
$$

---

![3.36](/Users/shen/Desktop/3.36.png)

![3.361](/Users/shen/Desktop/3.361.png)

a. The truth table:

|  A   |  B   |  G   |  E   |  L   |
| :--: | :--: | :--: | :--: | :--: |
|  0   |  0   |  0   |  1   |  0   |
|  0   |  1   |  0   |  0   |  1   |
|  1   |  0   |  1   |  0   |  0   |
|  1   |  1   |  0   |  1   |  1   |

b.

![a339b](/Users/shen/Desktop/a339b.jpeg)

c.

![s336c](/Users/shen/Desktop/s336c.jpeg)

---

![3.40.1](/Users/shen/Desktop/3.40.1.png)

![3.40.2](/Users/shen/Desktop/3.40.2.png)

a. According to Figure 3.45 the memory uses 2-bit (A[1] & A[0]) address, thus it's $address\_space = 2^2 = 4 $

b. Since each memory location stores 4-bits(D[3:0]), it's $addressability = 4$

c. Since $ address(2) = 10_B $, we could find that $data\ at\ address(2) = 0001$

---

![3.50](/Users/shen/Desktop/3.50.png)
$$
\because
\left\{
\begin{align*}
NOT &= \overline{X} = \overline{X} + \overline{X} = \overline{X \cdot X} \tag{1} \\
OR &= X+Y = \overline{\overline{X} \cdot \overline{Y}} = \overline{\overline{X \cdot X} \cdot \overline{Y \cdot Y}} \tag{2} \\
AND &= X \cdot Y= \overline{\overline{A\cdot B}} = \overline{\overline{A\cdot B} \cdot \overline{A\cdot B}} \tag{3} \\
\end{align*}
\right.
\\
\\
and \ Set\{AND,OR,NOT\} \ is \ logically\ complete \tag{4} \\
\therefore NAND \ is\ also\ logically\ complete
$$
The logical circuits are as follows:

![a350](/Users/shen/Desktop/a350.jpeg)