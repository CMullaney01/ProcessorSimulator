.LFB0:
addi s0, zero, 10
addi t0, zero, 1
addi t1, zero, 1

fib_loop:
    add t2, t0, t1
    add t0, t1, zero
    add t1, t2, zero
    addi s0, s0, -1

    # check loop condition
    bne	s0,zero,fib_loop
    li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
    addi	sp,sp,32
.LFE0: