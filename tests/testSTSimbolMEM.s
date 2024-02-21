# test za ld $simbol, gpr
.global simVanSek
.section prva
ld $0x1C2C3D4E, %r1
not %r1
not %r1
# otkomentarisi ako ti treba da testiras ovaj store ispod
# problem je sto onda na toj adresi on upise ovo, a 1C ispadne INT instrukcija
# i onda program stane, ali bitno je da se upis radi
# st %r1, simbUSek
ld $0x606, %r6
ld $0x707, %r7
ld $0x234288FF, %r8
simbUSek:
not %r6
not %r6
st %r8, simVanSek
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
not %r6
not %r6
simVanSek:
not %r6
not %r6
not %r6
not %r6
halt

halt

.section cetvrta
int
ret

.end