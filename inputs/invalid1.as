macr myMacro
cmp r3, #-6
endmacr

; invalid macro name (reserved word)
macr mov
mov r1, r2
endmacr

; invalid macro name (contains special character)
macr macro@Name
cmp r3, #-6
bne LABEL1
endmacr

; invalid macro ending
macr someMacro
mov r1, r2
endmacr hello:)

; program shouldn't reach here

; invalid string
STR: .string "invalid" extra
