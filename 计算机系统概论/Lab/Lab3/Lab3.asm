      .ORIG x3000
      ; initialization
      LEA  R2, QUE2;  R2 = begin
      LEA  R3, QUE2;  R3 = end
      LEA  R4, RES;   R4 = &res[0]
      ;
LOOP  GETC; R0 = input
      OUT; echo
      ; if input == '\n' break
      LD   R1, ASC_LF
      ADD  R1, R1, R0
      BRz  END
      ; if input == '-'
      LD   R1, ASC_MINUS
      ADD  R1, R1, R0
      BRnp RPOP
      ; is empty?
      NOT  R5, R3;
      ADD  R5, R5, #1;
      ADD  R5, R5, R2;
      BRnp POPL;
      ; empty -> res[idx] = '_', idx++
      LD   R5, ASC_ULINE
      STR  R5, R4, #0;
      ADD  R4, R4, #1; idx++
      BR   LOOP;
      ; not empty POP(left)
POPL  LDR  R5, R2, #0;
      STR  R5, R4, #0; res[idx] = *begin
      ADD  R2, R2, #1; begin++
      ADD  R4, R4, #1; idx++
      BRnzp LOOP;
      ; if input == ']'
RPOP  LD   R1, ASC_RBR
      ADD  R1, R1, R0
      BRnp LPUSH
      ; is empty?
      NOT  R5, R3;
      ADD  R5, R5, #1;
      ADD  R5, R5, R2;
      BRnp POPR;
      ; empty -> res[idx] = '_', idx++
      LD   R5, ASC_ULINE
      STR  R5, R4, #0;
      ADD  R4, R4, #1; idx++
      BRnzp LOOP;
      ; not empty POP(left)
POPR  ADD  R3, R3, #-1; end--
      LDR  R5, R3, #0;
      STR  R5, R4, #0; res[idx] = *end
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
      STR  R0, R2, #0;  push
      BRnzp LOOP
      ; Push(right)
RPUSH GETC
      OUT
      ; Push(right)
      STR  R0, R3, #0;  push
      ADD  R3, R3, #1;  end++
      BRnzp LOOP
END   ; print(Res)
      AND R5, R5, #0;
      STR R5, R4, #0;
      LEA R0, RES;
      PUTS;
      HALT;

; ASCII
ASC_LF    .FILL #-10
ASC_PLUS  .FILL #-43
ASC_MINUS .FILL #-45
ASC_RBR   .FILL #-93
ASC_ULINE .FILL #95

; deque
QUE1      .BLKW #60
QUE2      .BLKW #60
; result
RES       .BLKW #101
          .END