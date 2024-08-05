;file kelet2.as
;macro spread

macr EXP1
cmp K, #2
sub r1, r4
jmp LOOP
endmacr
.extern LOOP
.entry DO
EXP1
DO:mov STR, r1
STR:.string "abcdef"
K:.data 22,1,-8,-5,-7
