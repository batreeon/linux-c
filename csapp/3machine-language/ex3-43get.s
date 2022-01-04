	.file	"ex3-43get.c"
	.text
	.globl	get
	.type	get, @function
get:
.LFB0:
	.cfi_startproc
	endbr64
	addq	$10, %rdi
	movq	%rdi, (%rsi)
	ret
	.cfi_endproc
.LFE0:
	.size	get, .-get
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
