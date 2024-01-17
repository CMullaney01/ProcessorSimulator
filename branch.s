.LC0:
    .word   1           # array length

    .text
    .align  2
    .globl  main
    .type   main, @function
main:
.LFB0:
# Branch benchmark
# a0 = counter
# a1 = end counter
# a3 = comparison value
# zero = comparison

# Load in comparison value and counters
    addi    a3,zero,%lo(.LC0)
    lw      a3,0(a3)
    addi    a0,zero,0
    addi    a1,zero,10

# Loop
L1:
    addi a3,a3,-2
    addi a0,a0,1
    j Loop
L2:
    addi a3,a3,1
    addi a0,a0,1
    j Loop
Loop:
    bge    a0,a1,end
    bgt     a3,zero,L1
    ble a3 ,zero, L2
end:
  li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
  addi	sp,sp,32
.LFE0: