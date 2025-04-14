;tester to show errors in the first pass
.entry START
MAIN: add r3, r4
MAIN: sub r1, r2
INVALID-LABEL: mov r1, r2
.extern r3
.extern add
jmp #2,START
bne &.string 
AS: mov r6,r5,
add .data
bne #-5
.data 3,,5
.data
.data 3, hello
.string "abc
.string abc"
.entry r1
.entry add
mov #5, #6
LABEL: .extern L3
VALID: stop #8, A
cmp: add r1,r5
string "abcd"
.string 3749
.string
