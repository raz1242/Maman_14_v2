; invalid label name - macro name
macr myMacro
cmp r4, #-9
endmacr

myMacro: add r1, r2

; too many arguments
add r3, r4, r5

; missing comma
add r3 r4

; too little arguments
sub
