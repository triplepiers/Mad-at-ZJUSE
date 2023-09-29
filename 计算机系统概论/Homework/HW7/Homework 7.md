# Homework 7

> 2023.07.20 沈韵沨 3200104392

![9.2](/Users/shen/Desktop/9.2.png)

​	When using *synchronous I/O*, the processor will know exactly when the data will arrive or be taken away, since it will do input & output *at regular intervals*. And it is guaranteed that input data will be taken by those consumers right during the intervals.

​	Thus, a ready bit is not needed in *synchronous I/O*.

---

![9.6](/Users/shen/Desktop/9.6.png)

​	If the program doesn't check the ready bit of KBSR, it would *read the same input character more than once* if `KBSR[15] == 0`.

---

![9.10](/Users/shen/Desktop/9.10.png)

​	If the program doesn't check the ready bit of DSR, *the former output character would never been displayed* if `DSR[15] == 0`.

---

![9.14](/Users/shen/Desktop/9.14.png)

​	Since address `xFE00~xFFFF` in LC-3 are reserved for input/output registers, the LC-3 Load instruction with address `xFE02` would load from **KBDR**, also it seems like a normal memory address to user.

​	During the EXECUTE phase, the *addr control logic* would select the corresponding device register to provide input according to the memory-mapped address.

---

![9.26](/Users/shen/Desktop/9.26.png)

- No.

- `R7` is overwritten by **B**, **A** can't return properly.