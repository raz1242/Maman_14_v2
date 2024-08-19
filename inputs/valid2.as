.extern functionOne
.entry startLabel

startLabel: mov r1, r2
            prn stringOne
            lea labelOne, r3
            sub r3, dataOne
            jsr functionOne

stringOne:  .string "Sample String"
dataOne:    .data 10, 20, -30, 40

functionTwo:    clr r5
            add r3, r4
            jsr functionThree
            prn r6
            stop

labelOne:   .data 100, 200, 300

functionThree:    inc r7
            add r7, r5
            jmp functionOne
