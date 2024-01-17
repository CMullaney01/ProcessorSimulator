.LC0:
    .word   5           # array length
    .word   4           # array[0]
    .word   3           # array[1]
    .word   1           # array[2]
    .word   2           # array[3]

    .text
    .align  2
    .globl  main
    .type   main, @function
main:
.LFB0:
# Bubblesort benchmark
# t0 = pointer to array start
# t1 = (bool) swapped
# t2 = current pointer
# t3 = current element val


    addi t0, zero, %lo(.LC0)
    addi s3 , zero, 4

bubblesort:
    xor     t1, t1, t1
    addi    t2, t0, 0

loop:
    add     a1, t2, t0
    addi    a3, s3, 12
    bge     a1, a3, end
    
    lw      t3, 0(t2)  
    lw      t4, 4(t2)
    blt     t3, t4, loop_end
    sw      t4, 0(t2)
    sw      t3, 4(t2)
    ori     t1, t1, 1

loop_end:
    addi    t2, t2, 4
    beq     zero, zero, loop


end:
    bne     t1, zero, bubblesort 
    li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
    addi	sp,sp,32
.LFE0: