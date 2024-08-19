.extern extLabel1, extLabel2

; illegal use of a command as a label name
prn:    mov r1, r2
        inc r7

; invalid instruction with too many arguments
mov r1, r2, r3

; missing operand
clr

; duplicate label and invalid operand
LableDUP: jmp extLabel1
LabelDUP: prn r4


; invalid label name - starts with a digit
2ndLabel: add r1, r2
