.section prva
ld $0x1, %r1
bgt %r1, %r0, 0x300F
ld $0x75, %r3
ld $0x30, %r4
ld $0x40, %r12
ld $0x120, %r6
ld $0x20307, %r10
ld %r6, %r7

add %r3, %r4
sub %r4, %r6
mul %r4, %r3
div %r6, %r4
not %r6
not %r6
and %r6, %r7
or %r6, %r3
xor %r7, %r3
ld $0x6, %r4
shl %r4, %r6
shr %r4, %r6
xchg %r6, %r3
csrwr %r4, %handler
csrwr %r12, %status
csrwr %r7, %cause
csrrd %status, %r8
halt
ret
ret
ret

.section druga
ld $0x111, %r8
ld $0x222, %r9
ld $0x333, %r10
ld $0x444, %r11
halt
int

.section treca
int

.section cetvrta
int


.end