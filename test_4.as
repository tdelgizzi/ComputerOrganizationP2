alab    add 0   1   2
Alab    add 0   1   2
    add 0   1   2
nlab    nor 0   1   2
NLAB    nor 0   1   2
    nor 0   1   2
llab    lw  0   1   9
Llab    lw  0   1   9
    lw  0   1   ltest
    lw  0   1   Ltest
    lw  0   1   9
slab    sw  0   1   9
Slab    sw  0   1   9
    sw  0   1   stest
    sw  0   1   Stest
    sw  0   1   9
blab    beq 0   1   -9
Blab    beq 0   1   -9
    beq 0   1   beqt
    beq 0   1   Beqt
    beq 0   1   -9
nolab   noop
Nolab   noop
    noop
    halt
ltest   .fill 1
Ltest   .fill 2
stest   .fill 1
Stest   .fill 2
beqt    .fill 1
Beqt    .fill 2
