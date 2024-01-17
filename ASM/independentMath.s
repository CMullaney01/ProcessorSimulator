.LFB0:
addi a0, zero, 1
addi a1, zero, 2
addi a2, zero, 3
addi a3, zero, 4
addi a4, zero, 0
addi a5, zero, 10
fib_loop:
  add t0, a0, a1
  add t1, a2, a3
  add t2, t0, t1
  addi a0, a0, 1
  addi a1, a1, 1
  addi a2, a2, 1
  addi a3, a3, 1
  add a4, a4, t2      
  blt a0, a5, fib_loop
  li	a5,0
	mv	a0,a5
.LFE0: