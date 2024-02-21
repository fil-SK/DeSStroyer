# test za ld $simbol, gpr
.global simVanSek
.section prva
ld $0x100, %r1
not %r1
not %r1
ld simbUSek, %r4
ld $0x606, %r6
ld $0x707, %r7
ld $0x808, %r8
simbUSek:
not %r6
not %r6
ld simVanSek, %r10
not %r6
not %r6
halt
# adresa 0x4000 0034 -- tu je smestena vrednost simbola simVanSek

.section druga
# simVanSek nalazi se na offset-u 24 od pocetka sekcije
not %r6
not %r6
not %r6
not %r6
not %r6
not %r6
simVanSek:
ld $0x1, %r0
not %r6
not %r6
not %r6
halt

halt

.section cetvrta
int
ret

.end