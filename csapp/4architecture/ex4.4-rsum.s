	.file	"ex4.4-rsum.c"
	.text
	.globl	rsum
	.type	rsum, @function
rsum:
.LFB0:
	.cfi_startproc
	endbr64
	testq	%rsi, %rsi	;判断count
	jle	.L3
	pushq	%rbx			;被调用者寄存器，“保护现场”
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	(%rdi), %rbx	;*start
	subq	$1, %rsi		;count-1
	addq	$8, %rdi		;start++
	call	rsum			;调用rsum(start+1, count-1)
	addq	%rbx, %rax		;%rax保存rsum的返回值，这一行计算*start + rsum返回值
	popq	%rbx			;恢复现场
	.cfi_def_cfa_offset 8
	ret
.L3:
	.cfi_restore 3
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	rsum, .-rsum
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
