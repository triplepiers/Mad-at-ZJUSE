        .ORIG x3000
        ; Load Data
        LD  R4, x00ff;   R4 = input
        ; Init
        AND R1, R1, #0; 
        ADD R1, R1, #15; count = 15
        AND R2, R2, #0;  res = 0
        AND R3, R3, #0;
        ADD R3, R3, #7;  mask = x0007
Loop    ADD R1, R1, #-1;
        BRz Fin;
        AND R5, R4, R3;
        NOT R6, R3;
        ADD R6, R6, #1;
        ADD R5, R5, R6;
        BRz Success;
        ADD R3, R3, R3;
        BRnzp Loop;
Success ADD R2, R2, #1;
Fin     HALT;
        .END