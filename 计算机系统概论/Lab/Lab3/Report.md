# Report for LAB-3

## 1 Algorithm (flowchart)

<img src="/Users/shen/Desktop/data.jpeg" alt="data" style="zoom: 25%;" />

<center>Figure 1: Data Structure(?)</center>

<img src="/Users/shen/Desktop/flowchart.png" alt="flowchart" style="zoom:50%;" />

<center>Figure 2: Flowchart of the algorithm</center>

## 2 Code (essential parts with comments)

```assembly
LOOP  GETC
      OUT
      ; if input == '\n' break
      LD   R1, ASC_LF
      ADD  R1, R1, R0
      BRz  END
      ; if input == '-'
      LD   R1, ASC_MINUS
      ADD  R1, R1, R0
      BRnp RPOP
      ; is dequeue empty?
      NOT  R5, R3;
      ADD  R5, R5, #1; R5 = -end
      ADD  R5, R5, R2; R5 = begin - end
      BRnp POPL;
      ; empty -> res[idx++] = '_'
      LD   R5, ASC_ULINE
      STR  R5, R4, #0;
      ADD  R4, R4, #1; idx++
      BR   LOOP;
      ; POP(left)
POPL  LDR  R5, R2, #0; R5 = Mem[begin]
      STR  R5, R4, #0; res[idx] = Mem[begin]
      ADD  R2, R2, #1; begin++(Pop)
      ADD  R4, R4, #1; idx++
      BRnzp LOOP;

		 ; if input == '+'
LPUSH LD   R1, ASC_PLUS
      ADD  R1, R1, R0
      BRnp RPUSH
      GETC
      OUT
      ; Push(left)
      ADD  R2, R2, #-1; begin--
      STR  R0, R2, #0;  Mem[begin] = input(Push)
      BRnzp LOOP
```



```c
// initialization
char deque[100];
char* begin, end;
begin = end = deque;

char res[100];
int idx = 0;

char input;
scanf("%c", &input);

while(true) {      // not the end
    printf("%c", input);    // echo
    if(input == '\n') break;
    if (input == '-') {     // Pop(left)
        if (begin == end) { // empty
            res[idx++] = '_';
            continue;
        }
        res[idx++] = *begin++;
        continue;
    }
    if (input == ']') {    // Pop(right)
        if (begin == end) { // empty
            res[idx++] = '_';
            continue;
        }
        res[idx++] = *--end;
        continue;
    }
    if (input == '+') {   // Push(left)
        scanf("%c", &input);
        printf("%c", input);
        deque[--begin] = input;
    }
    if (input == '[') {
        scanf("%c", &input);
        printf("%c", input);
        deque[end] = input;
        end++;
    }
    scanf("%c", &input);
}

// res
res[++idx] = "\0";
printf("%s", res);
```

## 3 Q & A