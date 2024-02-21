.global funkcija, skokB
.section prva
call funkcija
ld $0x1, %r6
not %r7
funkcija:
ld $0x1113, %r8
ld $0x2223, %r9
ld $0x3334, %r10
ld $0x4443, %r11
call skokB
halt


.section druga
skokB:
ld $0x1, %r1
ld $0x2, %r2
ld $0x3, %r3
ld $0x4, %r4
halt

.section cetvrta
int
ret

.end