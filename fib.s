	.file	"fib.cpp"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	addi	sp,sp,-32
	.cfi_def_cfa_offset 32
	sw	s0,28(sp)
	.cfi_offset 8, -4
	addi	s0,sp,32
	.cfi_def_cfa 8, 0
	li	a5,10
	sw	a5,-20(s0)
	li	a5,1
	sw	a5,-24(s0)
	li	a5,1
	sw	a5,-28(s0)
	j	.L2
.L3:
	lw	a4,-24(s0)
	lw	a5,-28(s0)
	add	a5,a4,a5
	sw	a5,-32(s0)
	lw	a5,-28(s0)
	sw	a5,-24(s0)
	lw	a5,-32(s0)
	sw	a5,-28(s0)
	lw	a5,-20(s0)
	addi	a5,a5,-1
	sw	a5,-20(s0)
.L2:
	lw	a5,-20(s0)
	bne	a5,zero,.L3
	li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
	.cfi_restore 8
	.cfi_def_cfa 2, 32
	addi	sp,sp,32
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (g2ee5e430018) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
