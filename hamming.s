@ Task: compute the Hamming distance on 2 words
@       Given:  2 integer arrays xs and ys, where len(xs)==len(ys)
@       Return: the number of positions where the two arrays differ
@               i.e. | { i | i <- 0..len(xs), xs[i]!=ys[i] } |
	
@ Follows ARM subroutine calling conventions
	
	@ Entry point (Callable from C): 
	.global hamming
	
hamming:		  @ Input: 2 ptrs to int arrays in R0 and R1, length in R2
			  @ don't forget to push relevant registers here
	@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@ ... COMPLETE THIS CODE ...
	@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			  @ don't forget to pop relevant registers here
	BX   LR

	LDR r0, =array1      
    LDR r1, =array2      
    .ltorg @was getting OFFSET_IMM errors, after some research,
		   @this seemed to fix the issue   
		   
	MOV r2, #0 @hamming distance
	MOV r3, #0 @for loop counter
	
loop:
	ADD r3, #1 @increment counter
	LDR r4, [r0] @load array1 value into r4
	LDR r5, [r1] @load array2 value into r5
	
	ADD r0, #4 @move to next array variable for next loop iteration
	ADD r1, #4 @move to next array variable for next loop iteration
	
	CMP r4, r5
	BNE add @if not equal, branch to the add branch
	BEQ check @if equal, check if we need to loop again
	
add:
	ADD r2, #1
	B check
	
check:
	CMP r3, #3
	BLT loop

@ Test data	
.data
.equ VAL1, 1
.equ VAL2, 2	


@ Indicate to the linker that the code in this file does not need the stack
@ to be executable. (Recent versions of GNU ld warn if this is not present.)
.section .note.GNU-stack,"",%progbits
	
