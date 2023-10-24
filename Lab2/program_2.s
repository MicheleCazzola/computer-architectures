# program_2.s

.data
K:		.word 	30
i:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
w:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
b:		.double 171.0	# 0xab
y:		.space 8

.text
main:
	
	daddui R1, R0, 0	#index
	daddui R2, R0, 0	#offset
	ld R3, K(R0)		#limit
	mtc1 R0, F4			#result
	
loop:
	beq R1, R3, end_loop
	
	l.d F1, i(R2)
	l.d F2, w(R2)
	
	mul.d F3, F1, F2			# input*weight
	add.d F4, F4, F3			# sum weighted inputs
	
increment:
	daddui R1, R1, 1
	daddui R2, R2, 8

	j loop
	
end_loop:

	l.d F10, b(R0)
	add.d F4, F4, F10			# add offset input "b"
	
function:						# implement f(x)

	mfc1 R3, F4
	dsll R3, R3, 1
	dsrl R3, R3, 31
	dsrl R3, R3, 22
	daddui R4, R0, 0x7ff
	
	mtc1 R0, F1
	beq R3, R4, output			# check instruction
	add.d F1, F4, F1
output:
	s.d F1, y(R0)				# store final result
	
	halt