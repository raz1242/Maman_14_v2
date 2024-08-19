.extern EXTERNALLABEL
.entry MAIN

MAIN: mov r1, r2
      add r3, r4
      prn K
      jsr EXTERNALLABEL

LOOP: cmp r3, #-6
      bne END
      inc r1
      jmp LOOP

END:  stop

K:    .data 1, 2, 3, 4
STR:  .string "Some String"
