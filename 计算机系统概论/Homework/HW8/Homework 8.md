# Homework 8

> 2023.07.21 沈韵沨 3200104392

![9.16](/Users/shen/Desktop/9.16.png)

a. **256**, for the trap vector in LC-3 is 8-bit, can distinguish at most $ 2^8= 256 $ service routines.

b. Because `RTI` instruction would save PC and PSR before execute TRAP routine, and restore  when finished, while`BR` instruction won't do that.

- Without saving PC, we can't find the return address after finishing TRAP routine.
- Without saving & modifying PSR, the TRAP routine might unable to finish its work due to the lack of supervisor privilege.

c. Only **ONCE**, when visit the Trap Vector Table to load the starting address of corresponding service routine.

---

![9.17](/Users/shen/Desktop/9.17.png)

a. Some **external** mechanism. (The HALT service could **never return** after bit[15] of MCR is cleared, for the clock has stopped, so does the instruction processing)

b. `STI R0, MCR`, which makes `MCR[15] = 0`.

c. `LD R1, SaveR1`

d. The program that executes the HALT instruction (the instruction that is right after HALT).