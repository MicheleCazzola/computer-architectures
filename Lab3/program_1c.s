# program_1.s

.data

v1:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v2:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0

v3:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v4:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v5:		.space 512
v6:		.space 512
v7:		.space 512	
				
.text
main:

	daddui R1, R0, 0		# i		
	daddui R2, R0, 0		# offset-i
	daddui R3, R0, 64		# limit
	daddui R4, R0, 1		# m
	daddui R5, R0, 1		# j
	daddui R6, R0, 8		# offset-j
	mtc1 R0, F0				# zero-double
	mtc1 R0, F10			# k
	mtc1 R0, F11			# p
	
loop:
	l.d F1, v1(R2)						
even:	
	dsllv R11, R4, R1
	mtc1 R11, F12
	cvt.d.l F12, F12		
	mul.d F11, F1, F12		# p_even
	l.d F24, v4(R6)
	l.d F2, v2(R2)
	cvt.l.d F17, F24
	mfc1 R13, F17
	dsrlv R14, R13, R5
	l.d F3, v3(R2)
	mul.d F5, F11, F2		# v5_even_first
	cvt.l.d F20, F11
	add.d F7, F2, F3		# v2[i] + v3[i]
	l.d F4, v4(R2)
	add.d F6, F10, F1		# v6_even_divisor

v5_op_even:
	
	mfc1 R4, F20			# m
	add.d F5, F5, F3

	
	add.d F5, F5, F4
	
v6_op_even:
	
	div.d F6, F5, F6
	l.d F21, v1(R6)
	s.d F5, v5(R2)
	mtc1 R14, F16
	mtc1 R4, F13
	cvt.d.l F13, F13
	mtc1 R5, F14
	cvt.d.l F14, F14
	mul.d F15, F13, F14
	cvt.d.l	F10, F16		# k
	
	
	
odd:
	div.d F31, F21, F15		# p_odd
	s.d F6, v6(R2)
	l.d F22, v2(R6)
	l.d F23, v3(R6)
	daddui R1, R1, 2		# i++
	
	add.d F25, F22, F23		# v2[j] + v3[j]
	
v5_op_odd:
	mul.d F26, F31, F22
	
	add.d F26, F26, F23
	add.d F26, F26, F24

k:
	add.d F27, F10, F21		# v6_odd_divisor
	s.d F26, v5(R6)
	
v6_op_odd:
	div.d F27, F26, F27
	daddui R5, R5, 2		# j++
	daddui R2, R2, 16		# offset-i ++
	s.d F27, v6(R6)

v7_op_even:
	
	mul.d F7, F6, F7
	s.d F7, v7(R2)
	
v7_op_odd:
	mul.d F25, F27, F25
	s.d F25, v7(R6)

increment:	
	bne R1, R3, loop
	daddui R6, R6, 16		# offset-j ++
	
end_loop:
	halt								