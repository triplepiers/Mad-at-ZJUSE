# Homework 6

> 2023.07.19 沈韵沨 3200104392

## Chap 7

![7.30.1](/Users/shen/Desktop/7.30.1.png)

![7.30.2](/Users/shen/Desktop/7.30.2.png)

![a.7.30](/Users/shen/Desktop/a.7.30.jpeg)

## Chap 8

![8.2](/Users/shen/Desktop/8.2.png)

- On each Push / Pop action, **every value already in the stack MOVES** in the model of Figure 8.8, while they don't in the model of Figure 8.9.

- Since load & save data from memory takes a lot of time, it's wiser to move stack pointer(in Figure 8.9) than move all the other values already in the stack(in Figure 8.8).

---

![8.8](/Users/shen/Desktop/8.8.png)

a. The stack contains [F, A] after `PUSH F`.

b. The stack contains the most elements after perform `PUSH J / PUSH K`:

- After `PUSH J`: [J, I, H, G, F A]
- After `PUSH K`: [K, I, H, G, F, A]

c. Now it contains [M, F, A] (from the top to the bottom).

---

<img src="/Users/shen/Desktop/8.10.1.png" alt="8.10.1" style="zoom: 33%;" />

<img src="/Users/shen/Desktop/8.10.2.png" alt="8.10.2" style="zoom:33%;" />

<img src="/Users/shen/Desktop/8.10.3.png" alt="8.10.3" style="zoom:33%;" />

![a.8.10](/Users/shen/Desktop/a.8.10.jpeg)