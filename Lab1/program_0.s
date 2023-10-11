# program_0.s
.data
N:		.byte		15
v1:  	.byte    	2, 6, -3, 11, 9, 11, -3, 6, -3, 11, 9, 11, -3, 6, 2 
v2:  	.byte    	4, 7, -10,3, 11, 9, 7, 6, 4, 7, 0, 0, 0, 0, 0, 0
v3:  	.byte    	1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1
flag1: 	.space 		1
flag2: 	.space 		1
flag3: 	.space 		1
v4:  	.space 		15


.code
main:

init1:
	daddi R1, R0, 0			# index
	daddi R2, R0, 0 		# offset left
	lb R3, N(R0)
	daddi R6, R3, -1			# offset right
	dsrl R3, R3, 1				# limit
	daddi R5, R0, 1				# flag
	sb R5, flag1(R0)

palindrome1:
	slt R8, R3, R1
	bnez R8, done_palindrome1
	lb R4, v1(R2)
	lb R7, v1(R6)
	
	bne R4, R7, not_palindrome1
	j increments1
	
not_palindrome1:
	sb R0, flag1(R0)
	j done_palindrome1
	
increments1:
	daddi R1, R1, 1
	daddi R2, R2, 1
	daddi R6, R6, -1
	j palindrome1
	
done_palindrome1:
	nop
	
	
init2:
	daddi R1, R0, 0			# index
	daddi R2, R0, 0 		# offset left
	lb R3, N(R0)
	daddi R6, R3, -1			# offset right
	dsrl R3, R3, 1				# limit
	daddi R5, R0, 1				# flag
	sb R5, flag2(R0)

palindrome2:
	slt R8, R3, R1
	bnez R8, done_palindrome2
	lb R4, v2(R2)
	lb R7, v2(R6)
	
	bne R4, R7, not_palindrome2
	j increments2
	
not_palindrome2:
	sb R0, flag2(R0)
	j done_palindrome2
	
increments2:
	daddi R1, R1, 1
	daddi R2, R2, 1
	daddi R6, R6, -1
	j palindrome2
	
done_palindrome2:
	nop
	

init3:
	daddi R1, R0, 0			# index
	daddi R2, R0, 0 		# offset left
	lb R3, N(R0)
	daddi R6, R3, -1			# offset right
	dsrl R3, R3, 1				# limit
	daddi R5, R0, 1				# flag
	sb R5, flag3(R0)

palindrome3:
	slt R8, R3, R1
	bnez R8, done_palindrome3
	lb R4, v3(R2)
	lb R7, v3(R6)
	
	bne R4, R7, not_palindrome3
	j increments3
	
not_palindrome3:
	sb R0, flag3(R0)
	j done_palindrome3
	
increments3:
	daddi R1, R1, 1
	daddi R2, R2, 1
	daddi R6, R6, -1
	j palindrome3
	
done_palindrome3:
	nop
	
parallel_sum:
	daddi R1, R0, 0		# offset
	daddi R2, R0, 0		# index
	lb R3, N(R0)		# limit
	lb R4, flag1(R0)
	lb R5, flag2(R0)
	lb R6, flag3(R0)
	
loop:
	beq R2, R3, end_loop
	
	daddi R8, R0, 0			#sum
	lb R9, v1(R1)
	lb R10, v2(R1)
	lb R11, v3(R1)

first:
	beqz R4, second
	dadd R8, R8, R9
second:
	beqz R5, third
	dadd R8, R8, R10
third:
	beqz R6, store
	dadd R8, R8, R11
store:
	sb R8, v4(R1)

increments:
	daddi R1, R1, 1
	daddi R2, R2, 1
	j loop

end_loop:
	halt