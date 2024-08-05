;line 1:excceded max size 80
onaoifnoawijdioawjdiojawiodjawijdioawjdiojawoidjioawjdiojawidjioawjdiojawidjawiojdioawjdijawidjiawojdioawjdijawijdioawjdoijawidjawiojdioawjdiojawd

;undefined line structure
dpawjoidjawiojdiawjdijiadio

;good lines
.entry L1
.entry K
L1: sub r1,r1

;line 6:undefined line structure
move r1,r2

;line 7:invalid source operand
mov R1,R2

;line 8:invalid traget operand
mov r1,r78

;line 9: invalid addressing type for source operand
lea #1, r1

;line 10:missing 2 operands
add

;line 11:missing 1 operand
cmp r1,

;line 12:missing 1 operand
jmp

;line 13:only one operand required
not r1,r2

;line 14:invalid addressing type for target operand
dec #-5

;line 15:invalid source operand
mov K[],K[2]

;good line
K: .data 1,2,3,4,5,6,7

;line 17:command has extorendos text
hlt diawdioawjid

;line 18:Undefined line structure
.Define k=3

;line 19:data can store integers only
.data odija,2,3,4

;line 20:define Expected label
.define =6

;line 21:define Usage: .define NAME = NUMBER
.define f=

;line 22:string has no ending comma
.string "ddd 

;line 23:string has no starting comma
.string ddd

;line 24:extern label needs to start in an alphabetic letter
.extern 4j

;line 25:label can only start with an alphabetic letter
4j:mov r1,r2

;line 26:Illigal space between the label name and : sign
lp :mov r1,r2

;line 27:entry declaring label that dont exist in file
.entry HOME

;line 28:define number must be in range [-2048, 2047]
.define kl = 565665

;line 29:data number must be in range [-8192, 8191]
.data 2,3,45434,4,5

;line 30:label excceded max size of 32 letters
ThisLabelIsTooLongForTheFileAndWillGetAnErrorForThat: hlt

