    lw 0 1 neg1
    lw 0 2 ten
    lw 0 3 one
loop  add 2 1 2
    beq 2 0 done
    beq 0 0 loop
done  halt
neg1  .fill -1
ten   .fill 10
one   .fill 1
