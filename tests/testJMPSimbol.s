.global skokUSekciji, skokVanSekcije
.section prva
ld $0x555, %r5
ld $0x111, %r6
ld $0x1, %r1
jmp skokUSekciji
halt
skokUSekciji:
ld $0x8888, %r8
jmp skokVanSekcije
ld $0xAAAA, %r12
halt


.section druga
skokVanSekcije:
ld $0x1, %r3
ld $0x1, %r4
halt

.section cetvrta
int
ret

.end