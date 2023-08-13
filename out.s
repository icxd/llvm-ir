	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"out.ll"
	.def	 main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %entry
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	leaq	str_0(%rip), %rcx
	callq	printf
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
str_0:                                  # @str_0
	.asciz	"Hello world!"

