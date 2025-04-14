;tester to show errors in the second pass
.entry st
.extern st
bne &E
add #1, J
.extern S
jmp &S
A: .string "abc"
bne &r
