	.file	"bsort.cpp"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.section	.rodata
	.align	2
.LC0:
	.word	5
	.word	4
	.word	3
	.word	1
	.word	2
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	addi	sp,sp,-64
	.cfi_def_cfa_offset 64
	sw	s0,60(sp)
	.cfi_offset 8, -4
	addi	s0,sp,64
	.cfi_def_cfa 8, 0
	lui	a5,%hi(.LC0)
	addi	a5,a5,%lo(.LC0)
	lw	a1,0(a5)
	lw	a2,4(a5)
	lw	a3,8(a5)
	lw	a4,12(a5)
	lw	a5,16(a5)
	sw	a1,-52(s0)
	sw	a2,-48(s0)
	sw	a3,-44(s0)
	sw	a4,-40(s0)
	sw	a5,-36(s0)
	li	a5,5
	sw	a5,-28(s0)
	sw	zero,-20(s0)
	j	.L2
.L6:
	sw	zero,-24(s0)
	j	.L3
.L5:
	lw	a5,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-36(a5)
	lw	a5,-24(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-36(a5)
	ble	a4,a5,.L4
	lw	a5,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-36(a5)
	sw	a5,-32(s0)
	lw	a5,-24(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-36(a5)
	lw	a5,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	a4,-36(a5)
	lw	a5,-24(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-32(s0)
	sw	a4,-36(a5)
.L4:
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L3:
	lw	a4,-28(s0)
	lw	a5,-20(s0)
	sub	a5,a4,a5
	addi	a5,a5,-1
	lw	a4,-24(s0)
	blt	a4,a5,.L5
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lw	a5,-28(s0)
	addi	a5,a5,-1
	lw	a4,-20(s0)
	blt	a4,a5,.L6
	li	a5,0
	mv	a0,a5
	lw	s0,60(sp)
	.cfi_restore 8
	.cfi_def_cfa 2, 64
	addi	sp,sp,64
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (g2ee5e430018) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
