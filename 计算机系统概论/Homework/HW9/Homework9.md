# Homework 9

> 2023.07.24 沈韵沨 3200104392

## Q1

​	**Using the format of Figure 10.6, please draw the stack usage procedure during the computation of `(51-49)*(172+205)-(17*2)`. The stack pointer is x4000 initially. Hint: you may refer the whole process sequence in Page 396 of textbook.**

![截屏2023-07-24 18.42.47](/Users/shen/Desktop/截屏2023-07-24 18.42.47.png)

## Q2

​	**There is a 4-dimensional array $A[M,N,P,Q]$, in which $M=3,N=5,P=7,Q=9$. Each element is a 16-bit integer and stored sequentially in LC-3's memory. The first element.$A[0,0,0,0]$ is stored at address x4000. The access way of this 4D array can be describe in C like:**

```c
int i, j, k, l;
for (i=0 ; i<M ; i++) {
  for (j=0 ; j<N ; j++) {
    for (k=0 ; k<P ; k++) {
      for (l=0 ; l<Q ; l++) {
        // access A[i, j, k, l]
      }
    }
  }
}
```

​	**What's the address of $A[2,4,3,5]$? Also show the calculation procedure of your answer.**

| index |   element    |
| :---: | :----------: |
|   1   | `A[0,0,0,0]` |
|   2   | `A[0,0,0,1]` |
|  ...  |     ...      |
|   9   | `A[0,0,0,8]` |
|  10   | `A[0,0,1,0]` |
|  11   | `A[0,0,1,1]` |
|  ...  |     ...      |
|  64   | `A[0,1,0,0]` |
|  ...  |     ...      |

It's easy to find that $A[i,j,k,l]$ is the $(i*(5*7*9) + j*(7*9)+k*9+l+1)^{th}$ element in the array, which means $A[2,4,3,5]$ is the 915^th^ element.

Since the 1^st^ element is stored at `x4000` && each 16-bit integer would take one memory location, the 915^th^ element would locate at the memory address:
$$
0x4000 + (915-1) = 17298 = 0x4392
$$
