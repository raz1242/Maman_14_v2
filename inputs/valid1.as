.entry MAIN
.extern XYZ

START:   mov r1, XYZ
         add r2, r3
         prn STR
         sub K, K
         jsr XYZ

MAIN:    lea STR, r4
         inc r4
         mov r4, *r5
         bne END
         jmp START

END:     stop

STR:     .string "Hello, World!"
K:       .data 123, -456
