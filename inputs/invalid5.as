.extern EXT_LABEL

; invalid second operand - no such label (nor should be possible for such label)
invalidOperand: mov r1, r2
                add r3, r4
                prn _string

; illegal character in label name
!labelName:     cmp r1, r2
                bne LOOP

; invalid operand - immediate value with illegal character
add r2, #@5


; missing comma in directive
dataLabel: .data 100 200 -300 400 , 500

; numbers out of bounds of machine's range
.data 99999, -99999

; invalid label name - starts with Integer
1label: prn #7