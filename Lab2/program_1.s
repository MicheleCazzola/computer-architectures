# program_1.s

.data

v1:		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		
v2:		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1

v3:		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		
v4:		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		.double	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		
v5:		.space 512
v6:		.space 512
v7:		.space 512	
				
.text
main:

	daddui R1, R0, 0		# index		#1, CPI=1, n = 131
	daddui R2, R0, 0		# offset
	daddui R3, R0, 64		# limit
	
loop:
	beq R1, R3, end_loop				#2, CPI=1, n = 65
	
load_op:
	l.d F1, v1(R2)						#3, CPI=1, n = 256
	l.d F2, v2(R2)
	l.d F3, v3(R2)
	l.d F4, v4(R2)
	
v5_op:
	mul.d F5, F1, F2					#4, CPI=8, n = 128
	add.d F5, F5, F3					#5, CPI=3, n = 256
	add.d F5, F5, F4
	s.d F5, v5(R2)						#6, CPI=1, n = 192

v6_op:
	add.d F6, F4, F1
	div.d F6, F5, F6					#7, CPI=20, n = 64
	s.d F6, v6(R2)

v7_op:
	add.d F7, F2, F3
	mul.d F7, F6, F7
	s.d F7, v7(R2)
	
increment:
	daddui R1, R1, 1
	daddui R2, R2, 8
	
	j loop								#8, CPI=1, n = 64	
	
end_loop:

	nop

	halt								#9, CPI=1, n = 1