.global skokUSekciji, skokVanSekcije
.section prva
ld $0x222, %r5
ld $0x333, %r6
ld $0x1, %r1
bne %r5, %r6, skokUSekciji
halt
skokUSekciji:
ld $0x8888, %r8
ld $0x9999, %r9
ld $0xAAAA, %r10
ld $0xBBBB, %r11
ld $0xCCCC, %r12
bne %r11, %r12, skokVanSekcije
ld $0x0, %r12
halt


.section druga
skokVanSekcije:
ld $0x1, %r1
ld $0x1, %r2
ld $0x1, %r3
ld $0x1, %r4
halt

.section cetvrta
int
ret

.end