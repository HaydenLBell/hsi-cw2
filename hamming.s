@ Task: compute the Hamming distance on 2 words
@       Given:  2 integer arrays xs and ys, where len(xs)==len(ys)
@       Return: the number of positions where the two arrays differ
@               i.e. | { i | i <- 0..len(xs), xs[i]!=ys[i] } |
	
@ Follows ARM subroutine calling conventions
	
	@ Entry point (Callable from C): 
	.global hamming
	
hamming:	  @ Input: 2 ptrs to int arrays in R0 and R1, length in R2
			  @ don't forget to push relevant registers here
			  @ don't forget to pop relevant registers here
			  @ r0, r1, r2, r3, r4, r5, r6, r7, r8 
		   
	MOV r3, #0 @hamming distance
	MOV r4, #0 @for loop counter
	
loop:
	ADD r4, #1 @increment counter
	LDR r5, [r0] @load array1 value into r4
	LDR r6, [r1] @load array2 value into r5
	
	ADD r0, #4 @move to next array variable for next loop iteration
	ADD r1, #4 @move to next array variable for next loop iteration
	
	CMP r5, r6
	BNE add @if not equal, branch to the add branch
	BEQ check @if equal, check if we need to loop again
	
add:
	ADD r3, #1
	B check
	
check:
	CMP r4, r2
	BLT loop
	BEQ return

return: 
	move r0, r3
	BX LR

@ Test data	
.data
.equ VAL1, 1
.equ VAL2, 2	

@ Indicate to the linker that the code in this file does not need the stack
@ to be executable. (Recent versions of GNU ld warn if this is not present.)
.section .note.GNU-stack,"",%progbits