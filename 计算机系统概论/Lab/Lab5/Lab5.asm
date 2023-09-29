.ORIG x3000
        ; int main(), return: R2(max length)
        AND R2, R2, #0; return 0
        ; i = n-1
        LD  R0, POSN
        LDR R0, R0, #0
        ST  R0, N
        ADD R0, R0, #-1
        ; j = m
        LD  R1, POSM
        LDR R1, R1, #0
        ST  R1, M
MLOOP   ADD R1, R1, #-1; j--
        BRzp CALC
        ; j==-1
        ADD R0, R0, #-1; i--
        BRn FIN
        ; j = m-1
        LD  R1, M
        ADD R1, R1, #-1;
CALC    JSR MAXL;         R5 = findMAX(i,j) = findMAX(R0, R1)
        JSR MAX;          R2 = Max(R2, R5)
CONT    BR  MLOOP
FIN     HALT

POSN    .FILL x4000
POSM    .FILL x4001
N       .BLKW #1
M       .BLKW #1

; int max(R5, R2), return R2 = MAX(R5, R2)
; tmp: R6
MAX     ST  R6, Save1
        NOT R6, R2
        ADD R6, R6, #1
        ADD R6, R6, R5; R6 = R5-R2
        BRnz RESTX
        ADD R2, R5, #0
RESTX   LD  R6, Save1
        RET
        
Save1   .BLKW #1

; MulM(i:=R0), return R5 = i*m
; tmp: R1, R6
MULM    ST  R1, Save2
        ST  R6, Save3
        AND R5, R5, #0
        ADD R6, R0, #0;  R6 = i
        LD  R1, M;       R1 = m
MULLP   ADD R6, R6, #-1
        BRn RESTM
        ADD R5, R5, R1;  res += m
        BR  MULLP
RESTM   LD  R1, Save2
        LD  R6, Save3
        RET
        
Save2   .BLKW #1
Save3   .BLKW #1

; int less(i:=R0, j:=R1, cur:=R4), return R5 = 0 - Illegal/GET(bad),
;                                              1 - LT(okay)
; tmp: R2
LESS    ST  R2, Save4
        ADD R6, R6, #-1
        STR R7, R6, #0
        ; is legal?
        ; 1. i == -1
        ADD R2, R0, #1
        BRz RESTL
        ; 2. i == n?
        LD  R2, N
        NOT R2, R2
        ADD R2, R2, #1
        ADD R2, R2, R0; R2 = i - n
        BRz RET0
        ; 3. j == -1
        ADD R2, R1, #1
        BRz RET0
        ; 4. j == m
        LD  R2, M
        NOT R2, R2
        ADD R2, R2, #1
        ADD R2, R2, R1; R2 = j - m
        BRz RET0
        ; get a[i][j]
        JSR MULM; R5 = i*m
        ADD R5, R5, R1; off = i*m+j
        LD  R2, BASE
        ADD R2, R2, R5
        LDR R2, R2, #0; R2 = *(base+offset) = a[i][j]
        ; if a[i][j] < cur:=R4 ?
        NOT R2, R2
        ADD R2, R2, #1
        ADD R2, R2, R4; cur - a[i][j]
        BRnz RET0
        ; return 1 cur-a[i][j]>0
        AND R5, R5, #0
        ADD R5, R5, #1
        
RESTL   LD  R2, Save4
        LDR R7, R6, #0
        ADD R6, R6, #1
        RET
        
RET0    AND R5, R5, #0
        BR RESTL

BASE    .FILL x4002
Save4   .BLKW #1

; finaMax(i:=R0, j:=R1), return: R5(maxLength @ arr[i][j])
; tmp: R2, R4
MAXL    ADD R6, R6, #-1;
        STR R0, R6, #0;
        ADD R6, R6, #-1;
        STR R1, R6, #0;
        ADD R6, R6, #-1;
        STR R2, R6, #0;
        ADD R6, R6, #-1
        STR R4, R6, #0;
        ADD R6, R6, #-1;
        STR R7, R6, #0
        ;
        AND R2, R2, #0; res=0
        JSR MULM;       R5 = i*m
        LD  R4, BASE
        ADD R4, R4, R5; R4 = BASE + i*m
        ADD R4, R4, R1; R4 = BASE + i*m + j = offset
        LDR R4, R4, #0; R4 = arr[i][j]
        ; if less(i-1, j, cur)
        ADD R0, R0, #-1; R0 = i-1
        JSR LESS;        R5 = less(i-1, j, cur)
        ADD R5, R5, #0;  R5 == 0, next
        BRz CASE2
        JSR MAXL;        R5 = findmax(i-1, j)
        JSR MAX;
        ; if less(i+1, j)
CASE2   ADD R0, R0, #2;  R0 = i+1
        JSR LESS
        ADD R5, R5, #0;  R6 == 0, next
        BRz CASE3
        JSR MAXL;        R6 = findmax(i-1, j)
        JSR MAX;
        ; if less(i, j-1)
CASE3   ADD R0, R0, #-1; R0 = i
        ADD R1, R1, #-1; R1 = j-1
        JSR LESS
        ADD R5, R5, #0;  R6 == 0, next
        BRz CASE4
        JSR MAXL;        R6 = findmax(i-1, j)
        JSR MAX;
        ; if less(i, j+1)
CASE4   ADD R1, R1, #2; R1 = j+1
        JSR LESS
        ADD R5, R5, #0;  R6 == 0, next
        BRz RESTML
        JSR MAXL;        R6 = findmax(i-1, j)
        JSR MAX;
        
RESTML  ADD R5, R2, #1;  return res+1
        ; restore
        LDR R7, R6, #0
        ADD R6, R6, #1
        LDR R4, R6, #0
        ADD R6, R6, #1
        LDR R2, R6, #0
        ADD R6, R6, #1
        LDR R1, R6, #0
        ADD R6, R6, #1
        LDR R0, R6, #0
        ADD R6, R6, #1
        RET
        
.END