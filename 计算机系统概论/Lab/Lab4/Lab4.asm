; rewrite booting code
          .ORIG x0200
          LD  R6, OS_SP

          LD  R0, USER_PSR
          ADD R6, R6, #-1
          STR R0, R6, #0;  Push(USP)

          LD  R0, USER_PC
          ADD R6, R6, #-1
          STR R0, R6, #0;  Push(UPC)

          ; enable keyboard interrupt - KBSR[14] = 1
          LD  R0, KBSR_VAL
          STI R0, KBSR
          
          ; add start_address @x80
          LD  R0, ADDR_KB
          STI R0, INTV_KB
          
          RTI

OS_SP    .FILL x3000
USER_PSR .FILL x8002
USER_PC  .FILL x3000; affect the return addr
KBSR     .FILL xFE00
KBSR_VAL .FILL x4000
ADDR_KB  .FILL x0450
INTV_KB  .FILL x0180
         .END


          ; user program
          .ORIG x3000
          ; initialization
          LD   R1, ASC_A;     init_char = 'a'
          AND  R2, R2, #0;    init_pos = 5
          ADD  R2, R2, #6
          ; pos = max(pos-1, 0)
MLOOP     ADD  R2, R2, #-1;
          BRzp PRE;
          AND  R2, R2, #0;
          ; pre ...
PRE       ADD  R3, R2, #0;    n_preDots = pos
          JSR DOTS;
          ; printBird()
          ADD  R0, R1, #0;
          PUTC
          PUTC
          PUTC
          ; post ...
          LD  R0, EIGHTT
          NOT R3, R2
          ADD R3, R3, R0; R3 = 17 - R2 = 18 + not(R2)
          JSR DOTS
          ; new line
          LD  R0, ASC_LF
          PUTC
          ; delay
          JSR DELAY
          BR  MLOOP

; setTimeout(), delay: Timer
DELAY     ST   R4, Saver1
          LD   R4, Timer
DLOOP     ADD  R4, R4, #-1
          BRnp DLOOP
          LD   R4, Saver1
          RET
      
; printDots(), input: R3 = n_dots
DOTS      ST   R0, SaveR0
          LD   R0, ASC_DOT
PLOOP     ADD  R3, R3, #-1
          BRn  FIN
          PUTC
          BR   PLOOP
FIN       LD   R0, SaveR0
          RET
          
SaveR0    .BLKW #1  
; for delay
Saver1    .BLKW #1
Timer     .FILL x8000
; for output
ASC_A     .FILL #97
ASC_DOT   .FILL #46
ASC_LF    .FILL #10
EIGHTT    .FILL #18
          .END

          ; trap for keyboard
          .ORIG x0450
          ; save R6, R0, R3
INTR_KB   ST  R0, INTR_S1
          ST  R3, INTR_S2
          ST  R6, INTR_S3
          
          LDI R0, KBDR;    R0 = asc(input)
          LD  R3, NEG_A
          ADD R3, R3, R0
          BRn FLY
          ; input = a~z, modify R1
          ADD R1, R0, #0
          BR  RESTORE
          ; input = 1~9, modify R2
FLY       LD  R3, NEG_ZERO
          ADD R3, R0, R3;  R3 = offset
          ADD R2, R2, R3;  pos += offset
          ; if pos > 17
          LD  R3, NEG_SEVT
          ADD R3, R3, R2; R3 = R2 - 17
          BRnz RESTORE
          LD  R2, SEVT;   if pos > 17 then pos = 17
          ; restore R6, R0, R1
RESTORE   LD  R0, INTR_S1
          LD  R3, INTR_S2
          LD  R6, INTR_S3
          RTI
; callee-save
INTR_S1   .BLKW #1
INTR_S2   .BLKW #2
INTR_S3   .BLKW #3
; device registers
KBDR      .FILL xFE02
; judge
NEG_A     .FILL #-97
NEG_ZERO  .FILL #-47
NEG_SEVT  .FILL #-18; n_pre = pos--
SEVT      .FILL #18
          .END