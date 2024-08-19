.extern someExternal


; invalid label name - contains a special character
start-label: mov r1, r2
             sub r3, r4

; invalid command - too many operands
add r1, r2, r3, r4

; consecutive commas
lea r3, ,, r4

; illegal comma usage
add , r3, r4

; line too long
LONG_LABEL_NAME: .string "123456789123456789123456789123465789123456789123456789"