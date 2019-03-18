	.file	"ehci.cpp"
	.text
	.globl	__Z4Waiti
	.def	__Z4Waiti;	.scl	2;	.type	32;	.endef
__Z4Waiti:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$16, %esp
	movl	_time, %eax
	movl	_time+4, %edx
	movl	%eax, -16(%ebp)
	movl	%edx, -12(%ebp)
	cmpl	$49, 8(%ebp)
	jg	L2
	movl	$50, 8(%ebp)
L2:
	movl	_time, %eax
	movl	_time+4, %edx
	subl	-16(%ebp), %eax
	sbbl	-12(%ebp), %edx
	movl	%eax, %ecx
	movl	%edx, %ebx
	movl	8(%ebp), %esi
	movl	$1374389535, %edx
	movl	%esi, %eax
	imull	%edx
	sarl	$4, %edx
	movl	%esi, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	cmpl	%edx, %ebx
	jg	L3
	cmpl	%edx, %ebx
	jl	L2
	cmpl	%eax, %ecx
	jnb	L3
	jmp	L2
L3:
	nop
	addl	$16, %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.globl	__Z4MRIRjj
	.def	__Z4MRIRjj;	.scl	2;	.type	32;	.endef
__Z4MRIRjj:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	popl	%ebp
	ret
	.globl	__Z4MWIRjjj
	.def	__Z4MWIRjjj;	.scl	2;	.type	32;	.endef
__Z4MWIRjjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	movl	16(%ebp), %edx
	movl	%edx, (%eax)
	nop
	leave
	ret
	.globl	__Z3WORP14EhciControllerjj
	.def	__Z3WORP14EhciControllerjj;	.scl	2;	.type	32;	.endef
__Z3WORP14EhciControllerjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	movl	8(%ebp), %eax
	movl	28(%eax), %eax
	movl	16(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	12(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z4MWIRjjj
	nop
	leave
	ret
	.globl	__Z3RORP14EhciControllerj
	.def	__Z3RORP14EhciControllerj;	.scl	2;	.type	32;	.endef
__Z3RORP14EhciControllerj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	28(%eax), %eax
	movl	12(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z4MRIRjj
	leave
	ret
	.globl	__Z3WCRP14EhciControllerjj
	.def	__Z3WCRP14EhciControllerjj;	.scl	2;	.type	32;	.endef
__Z3WCRP14EhciControllerjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	movl	8(%ebp), %eax
	movl	32(%eax), %eax
	movl	16(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	12(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z4MWIRjjj
	nop
	leave
	ret
	.globl	__Z3RCRP14EhciControllerj
	.def	__Z3RCRP14EhciControllerj;	.scl	2;	.type	32;	.endef
__Z3RCRP14EhciControllerj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	32(%eax), %eax
	movl	12(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z4MRIRjj
	leave
	ret
	.section .rdata,"dr"
LC0:
	.ascii "EhciAllocTD failed\12\0"
	.text
	.def	__ZL11EhciAllocTDP14EhciController;	.scl	3;	.type	32;	.endef
__ZL11EhciAllocTDP14EhciController:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	16(%eax), %eax
	addl	$4096, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
L19:
	movl	-12(%ebp), %eax
	cmpl	-16(%ebp), %eax
	je	L16
	movl	-12(%ebp), %eax
	movl	56(%eax), %eax
	testl	%eax, %eax
	jne	L17
	movl	-12(%ebp), %eax
	movl	$1, 56(%eax)
	movl	-12(%ebp), %eax
	jmp	L18
L17:
	subl	$-128, -12(%ebp)
	jmp	L19
L16:
	movl	$LC0, (%esp)
	call	__Z6printfPKcz
	movl	$0, %eax
L18:
	leave
	ret
	.section .rdata,"dr"
LC1:
	.ascii "EhciAllocQH failed\12\0"
	.text
	.def	__ZL11EhciAllocQHP14EhciController;	.scl	3;	.type	32;	.endef
__ZL11EhciAllocQHP14EhciController:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	12(%eax), %eax
	addl	$1024, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	12(%eax), %eax
	movl	%eax, -12(%ebp)
L24:
	movl	-12(%ebp), %eax
	cmpl	-16(%ebp), %eax
	je	L21
	movl	-12(%ebp), %eax
	movl	84(%eax), %eax
	testl	%eax, %eax
	jne	L22
	movl	-12(%ebp), %eax
	movl	$1, 84(%eax)
	movl	-12(%ebp), %eax
	jmp	L23
L22:
	subl	$-128, -12(%ebp)
	jmp	L24
L21:
	movl	$LC1, (%esp)
	call	__Z6printfPKcz
	movl	$0, %eax
L23:
	leave
	ret
	.def	__ZL10EhciFreeTDP6EhciTD;	.scl	3;	.type	32;	.endef
__ZL10EhciFreeTDP6EhciTD:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$0, 56(%eax)
	nop
	popl	%ebp
	ret
	.def	__ZL10EhciFreeQHP6EhciQH;	.scl	3;	.type	32;	.endef
__ZL10EhciFreeQHP6EhciQH:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$0, 84(%eax)
	nop
	popl	%ebp
	ret
	.def	_EhciInsertAsyncQH;	.scl	3;	.type	32;	.endef
_EhciInsertAsyncQH:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	72(%eax), %eax
	subl	$72, %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	orl	$2, %eax
	movl	%eax, %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	movl	12(%ebp), %eax
	orl	$2, %eax
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	12(%ebp), %eax
	leal	72(%eax), %edx
	movl	8(%ebp), %eax
	addl	$72, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z10LinkBeforeP4LinkS0_
	nop
	leave
	ret
	.def	_EhciInsertPeriodicQH;	.scl	3;	.type	32;	.endef
_EhciInsertPeriodicQH:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	72(%eax), %eax
	subl	$72, %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	movl	$1, (%eax)
	movl	12(%ebp), %eax
	orl	$2, %eax
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	12(%ebp), %eax
	leal	72(%eax), %edx
	movl	8(%ebp), %eax
	addl	$72, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z10LinkBeforeP4LinkS0_
	nop
	leave
	ret
	.def	_EhciRemoveQH;	.scl	3;	.type	32;	.endef
_EhciRemoveQH:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	72(%eax), %eax
	subl	$72, %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	8(%ebp), %eax
	addl	$72, %eax
	movl	%eax, (%esp)
	call	__Z10LinkRemoveP4Link
	nop
	leave
	ret
	.def	__ZL11EhciPortSetPVjj;	.scl	3;	.type	32;	.endef
__ZL11EhciPortSetPVjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	orl	%eax, -4(%ebp)
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, (%eax)
	nop
	leave
	ret
	.def	__ZL11EhciPortClrPVjj;	.scl	3;	.type	32;	.endef
__ZL11EhciPortClrPVjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	notl	%eax
	andl	%eax, -4(%ebp)
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, (%eax)
	nop
	leave
	ret
	.section .rdata,"dr"
LC2:
	.ascii "~\0"
LC3:
	.ascii "#%dStatus%08x;\0"
	.text
	.def	__ZL13EhciResetPortP14EhciControllerj;	.scl	3;	.type	32;	.endef
__ZL13EhciResetPortP14EhciControllerj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	12(%ebp), %edx
	addl	$16, %edx
	sall	$2, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, -24(%ebp)
	movl	$LC2, (%esp)
	call	__Z6printfPKcz
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	orb	$16, %ah
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, (%eax)
	movl	$1000, (%esp)
	call	__Z4Waiti
	movl	$256, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciPortSetPVjj
	movl	$1000, (%esp)
	call	__Z4Waiti
	movl	$256, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciPortClrPVjj
	movl	$0, -12(%ebp)
	movl	$0, -16(%ebp)
	movl	$0, -20(%ebp)
L38:
	cmpl	$4, -20(%ebp)
	ja	L33
	movl	$300, (%esp)
	call	__Z4Waiti
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	andl	$1024, %eax
	testl	%eax, %eax
	je	L34
	cmpl	$0, -16(%ebp)
	jne	L34
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	orb	$32, %ah
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, (%eax)
	movl	$1, -16(%ebp)
	movl	$0, -20(%ebp)
L34:
	movl	-12(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	L40
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC3, (%esp)
	call	__Z6printfPKcz
	movl	-12(%ebp), %eax
	andl	$10, %eax
	testl	%eax, %eax
	je	L36
	movl	$10, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciPortClrPVjj
	jmp	L37
L36:
	movl	-12(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	jne	L41
L37:
	addl	$1, -20(%ebp)
	jmp	L38
L40:
	nop
	jmp	L33
L41:
	nop
L33:
	movl	-12(%ebp), %eax
	leave
	ret
	.def	__ZL10EhciInitTDP6EhciTDS0_jjjPKv;	.scl	3;	.type	32;	.endef
__ZL10EhciInitTDP6EhciTDS0_jjjPKv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	cmpl	$0, 12(%ebp)
	je	L43
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, 52(%eax)
L43:
	movl	8(%ebp), %eax
	movl	$1, (%eax)
	movl	8(%ebp), %eax
	movl	$1, 4(%eax)
	movl	8(%ebp), %eax
	movl	$0, 52(%eax)
	movl	16(%ebp), %eax
	sall	$31, %eax
	movl	%eax, %edx
	movl	24(%ebp), %eax
	sall	$16, %eax
	orl	%eax, %edx
	movl	20(%ebp), %eax
	sall	$8, %eax
	orl	%edx, %eax
	orl	$3200, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%edx, 8(%eax)
	movl	28(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, 12(%eax)
	movl	8(%ebp), %eax
	movl	$0, 32(%eax)
	andl	$-4096, -4(%ebp)
	movl	$1, -8(%ebp)
L45:
	cmpl	$3, -8(%ebp)
	ja	L46
	addl	$4096, -4(%ebp)
	movl	8(%ebp), %eax
	movl	-8(%ebp), %edx
	movl	-4(%ebp), %ecx
	movl	%ecx, 12(%eax,%edx,4)
	movl	8(%ebp), %eax
	movl	-8(%ebp), %edx
	addl	$8, %edx
	movl	$0, (%eax,%edx,4)
	addl	$1, -8(%ebp)
	jmp	L45
L46:
	nop
	leave
	ret
	.def	__ZL10EhciInitQHP6EhciQHP11UsbTransferP6EhciTDP9UsbDevicebjjjj;	.scl	3;	.type	32;	.endef
__ZL10EhciInitQHP6EhciQHP11UsbTransferP6EhciTDP9UsbDevicebjjjj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	24(%ebp), %eax
	movb	%al, -20(%ebp)
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movl	%edx, 68(%eax)
	movl	40(%ebp), %eax
	sall	$16, %eax
	movl	%eax, %edx
	movl	28(%ebp), %eax
	sall	$12, %eax
	orl	%eax, %edx
	movl	36(%ebp), %eax
	sall	$8, %eax
	orl	%edx, %eax
	orl	32(%ebp), %eax
	orb	$64, %ah
	movl	%eax, -4(%ebp)
	movl	$1073741824, -8(%ebp)
	movzbl	-20(%ebp), %eax
	xorl	$1, %eax
	testb	%al, %al
	je	L48
	orl	$1342177280, -4(%ebp)
L48:
	cmpl	$2, 28(%ebp)
	je	L49
	cmpl	$0, 20(%ebp)
	je	L49
	cmpb	$0, -20(%ebp)
	je	L50
	orl	$7168, -8(%ebp)
	jmp	L51
L50:
	orl	$134217728, -4(%ebp)
L51:
	movl	20(%ebp), %eax
	movl	16(%eax), %eax
	sall	$23, %eax
	movl	%eax, %edx
	movl	20(%ebp), %eax
	movl	24(%eax), %eax
	sall	$16, %eax
	orl	%edx, %eax
	orl	%eax, -8(%ebp)
L49:
	cmpb	$0, -20(%ebp)
	je	L52
	orl	$1, -8(%ebp)
L52:
	movl	8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, 4(%eax)
	movl	8(%ebp), %eax
	movl	-8(%ebp), %edx
	movl	%edx, 8(%eax)
	movl	8(%ebp), %eax
	movl	$0, 88(%eax)
	movl	16(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, 80(%eax)
	movl	16(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, 16(%eax)
	movl	8(%ebp), %eax
	movl	$0, 24(%eax)
	nop
	leave
	ret
	.globl	__Z4somev
	.def	__Z4somev;	.scl	2;	.type	32;	.endef
__Z4somev:
	pushl	%ebp
	movl	%esp, %ebp
	nop
	popl	%ebp
	ret
	.section .rdata,"dr"
LC4:
	.ascii " Data Buffer Error\12\0"
LC5:
	.ascii " Babble Detected\12\0"
LC6:
	.ascii " Transaction Error\12\0"
LC7:
	.ascii " Missed Micro-Frame\12\0"
	.text
	.globl	__Z13EhciProcessQHP14EhciControllerP6EhciQH
	.def	__Z13EhciProcessQHP14EhciControllerP6EhciQH;	.scl	2;	.type	32;	.endef
__Z13EhciProcessQHP14EhciControllerP6EhciQH:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	12(%ebp), %eax
	movl	68(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$64, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L55
	movl	-16(%ebp), %eax
	movb	$0, 17(%eax)
	movl	-16(%ebp), %eax
	movb	$1, 16(%eax)
	jmp	L56
L55:
	movl	12(%ebp), %eax
	movl	16(%eax), %eax
	andl	$1, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L56
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$128, %eax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L56
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$32, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L57
	movl	$LC4, (%esp)
	call	__Z6printfPKcz
L57:
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$16, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L58
	movl	$LC5, (%esp)
	call	__Z6printfPKcz
L58:
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$8, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L59
	movl	$LC6, (%esp)
	call	__Z6printfPKcz
L59:
	movl	12(%ebp), %eax
	movl	24(%eax), %eax
	andl	$4, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L60
	movl	$LC7, (%esp)
	call	__Z6printfPKcz
L60:
	movl	-16(%ebp), %eax
	movb	$1, 17(%eax)
	movl	-16(%ebp), %eax
	movb	$1, 16(%eax)
L56:
	movl	-16(%ebp), %eax
	movzbl	16(%eax), %eax
	testb	%al, %al
	je	L65
	movl	12(%ebp), %eax
	movl	$0, 68(%eax)
	movl	-16(%ebp), %eax
	movzbl	17(%eax), %eax
	testb	%al, %al
	je	L62
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	je	L62
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	-16(%ebp), %edx
	movl	(%edx), %edx
	movl	4(%edx), %edx
	xorl	$1, %edx
	movl	%edx, 4(%eax)
L62:
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_EhciRemoveQH
	movl	12(%ebp), %eax
	movl	80(%eax), %eax
	movl	%eax, -12(%ebp)
L64:
	cmpl	$0, -12(%ebp)
	je	L63
	movl	-12(%ebp), %eax
	movl	52(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciFreeTDP6EhciTD
	movl	-20(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	L64
L63:
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciFreeQHP6EhciQH
L65:
	nop
	leave
	ret
	.def	__ZL13EhciWaitForQHP14EhciControllerP6EhciQH;	.scl	3;	.type	32;	.endef
__ZL13EhciWaitForQHP14EhciControllerP6EhciQH:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	12(%ebp), %eax
	movl	68(%eax), %eax
	movl	%eax, -12(%ebp)
L68:
	movl	-12(%ebp), %eax
	movzbl	16(%eax), %eax
	testb	%al, %al
	jne	L69
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13EhciProcessQHP14EhciControllerP6EhciQH
	jmp	L68
L69:
	nop
	leave
	ret
	.def	__ZL14EhciDevControlP9UsbDeviceP11UsbTransfer;	.scl	3;	.type	32;	.endef
__ZL14EhciDevControlP9UsbDeviceP11UsbTransfer:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$120, %esp
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -28(%ebp)
	movl	12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -32(%ebp)
	movl	8(%ebp), %eax
	movl	20(%eax), %eax
	movl	%eax, -36(%ebp)
	movl	8(%ebp), %eax
	movl	24(%eax), %eax
	movl	%eax, -40(%ebp)
	movl	8(%ebp), %eax
	movl	28(%eax), %eax
	movl	%eax, -44(%ebp)
	movl	-32(%ebp), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %eax
	movl	%eax, -48(%ebp)
	movl	-32(%ebp), %eax
	movzwl	6(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, -52(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocTDP14EhciController
	movl	%eax, -56(%ebp)
	cmpl	$0, -56(%ebp)
	je	L80
	movl	-56(%ebp), %eax
	movl	%eax, -60(%ebp)
	movl	$0, -12(%ebp)
	movl	$0, -16(%ebp)
	movl	$2, -64(%ebp)
	movl	$8, -20(%ebp)
	movl	-32(%ebp), %eax
	movl	%eax, 20(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	-64(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitTDP6EhciTDS0_jjjPKv
	movl	-56(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-48(%ebp), %eax
	andl	$128, %eax
	testl	%eax, %eax
	je	L73
	movl	$1, %eax
	jmp	L74
L73:
	movl	$0, %eax
L74:
	movl	%eax, -64(%ebp)
	movl	12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %edx
	movl	-52(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -68(%ebp)
L78:
	movl	-24(%ebp), %eax
	cmpl	-68(%ebp), %eax
	jnb	L75
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocTDP14EhciController
	movl	%eax, -56(%ebp)
	cmpl	$0, -56(%ebp)
	je	L81
	xorl	$1, -16(%ebp)
	movl	-68(%ebp), %edx
	movl	-24(%ebp), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	cmpl	-44(%ebp), %eax
	jbe	L77
	movl	-44(%ebp), %eax
	movl	%eax, -20(%ebp)
L77:
	movl	-24(%ebp), %eax
	movl	%eax, 20(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	-64(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitTDP6EhciTDS0_jjjPKv
	movl	-20(%ebp), %eax
	addl	%eax, -24(%ebp)
	movl	-56(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	L78
L75:
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocTDP14EhciController
	movl	%eax, -56(%ebp)
	cmpl	$0, -56(%ebp)
	je	L82
	movl	$1, -16(%ebp)
	movl	-48(%ebp), %eax
	andl	$128, %eax
	testl	%eax, %eax
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -64(%ebp)
	movl	$0, 20(%esp)
	movl	$0, 16(%esp)
	movl	-64(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitTDP6EhciTDS0_jjjPKv
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocQHP14EhciController
	movl	%eax, -72(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	-44(%ebp), %edx
	movl	%edx, 32(%esp)
	movl	$0, 28(%esp)
	movl	-40(%ebp), %edx
	movl	%edx, 24(%esp)
	movl	-36(%ebp), %edx
	movl	%edx, 20(%esp)
	movl	$0, 16(%esp)
	movl	%eax, 12(%esp)
	movl	-60(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-72(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitQHP6EhciQHP11UsbTransferP6EhciTDP9UsbDevicebjjjj
	movl	-28(%ebp), %eax
	movl	20(%eax), %eax
	movl	-72(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_EhciInsertAsyncQH
	movl	-72(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL13EhciWaitForQHP14EhciControllerP6EhciQH
	jmp	L70
L80:
	nop
	jmp	L70
L81:
	nop
	jmp	L70
L82:
	nop
L70:
	leave
	ret
	.globl	__Z12ehciIntervalP9UsbDevicej
	.def	__Z12ehciIntervalP9UsbDevicej;	.scl	2;	.type	32;	.endef
__Z12ehciIntervalP9UsbDevicej:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	20(%eax), %eax
	testl	%eax, %eax
	jne	L84
	movl	$0, -4(%ebp)
L86:
	cmpl	$0, 12(%ebp)
	je	L85
	addl	$1, -4(%ebp)
	movl	12(%ebp), %eax
	shrl	%eax
	movl	%eax, 12(%ebp)
	jmp	L86
L85:
	movl	-4(%ebp), %eax
	addl	$3, %eax
	jmp	L87
L84:
	movl	12(%ebp), %eax
	addl	$1, %eax
L87:
	leave
	ret
	.def	__ZL11EhciDevIntrP9UsbDeviceP11UsbTransfer;	.scl	3;	.type	32;	.endef
__ZL11EhciDevIntrP9UsbDeviceP11UsbTransfer:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$104, %esp
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	20(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %eax
	movl	24(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movzwl	4(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, -28(%ebp)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movzbl	2(%eax), %eax
	movzbl	%al, %eax
	andl	$15, %eax
	movl	%eax, -32(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocTDP14EhciController
	movl	%eax, -36(%ebp)
	cmpl	$0, -36(%ebp)
	jne	L89
	movl	12(%ebp), %eax
	movb	$0, 17(%eax)
	movl	12(%ebp), %eax
	movb	$1, 16(%eax)
	jmp	L88
L89:
	movl	-36(%ebp), %eax
	movl	%eax, -40(%ebp)
	movl	$0, -44(%ebp)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	4(%eax), %eax
	movl	%eax, -48(%ebp)
	movl	$1, -12(%ebp)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movzbl	2(%eax), %eax
	testb	%al, %al
	jns	L91
	movl	$1, -12(%ebp)
	jmp	L92
L91:
	movl	$0, -12(%ebp)
L92:
	movl	12(%ebp), %eax
	movl	12(%eax), %eax
	movl	%eax, -52(%ebp)
	movl	12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, 20(%esp)
	movl	-52(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-48(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-36(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitTDP6EhciTDS0_jjjPKv
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocQHP14EhciController
	movl	%eax, -56(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	-28(%ebp), %edx
	movl	%edx, 32(%esp)
	movl	-32(%ebp), %edx
	movl	%edx, 28(%esp)
	movl	-24(%ebp), %edx
	movl	%edx, 24(%esp)
	movl	-20(%ebp), %edx
	movl	%edx, 20(%esp)
	movl	$1, 16(%esp)
	movl	%eax, 12(%esp)
	movl	-40(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL10EhciInitQHP6EhciQHP11UsbTransferP6EhciTDP9UsbDevicebjjjj
	movl	12(%ebp), %eax
	movzbl	18(%eax), %eax
	testb	%al, %al
	je	L93
	movl	-16(%ebp), %eax
	movl	20(%eax), %eax
	movl	-56(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_EhciInsertAsyncQH
	jmp	L94
L93:
	movl	-16(%ebp), %eax
	movl	24(%eax), %eax
	movl	-56(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_EhciInsertPeriodicQH
L94:
	movl	12(%ebp), %eax
	movzbl	18(%eax), %eax
	testb	%al, %al
	je	L88
	movl	-56(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL13EhciWaitForQHP14EhciControllerP6EhciQH
L88:
	leave
	ret
	.section .rdata,"dr"
LC8:
	.ascii "Detected device on port #%x\12\0"
LC9:
	.ascii "Some FUKIN BAD!\0"
	.text
	.globl	__Z13probeEhciPortP14EhciControllerj
	.def	__Z13probeEhciPortP14EhciControllerj;	.scl	2;	.type	32;	.endef
__Z13probeEhciPortP14EhciControllerj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL13EhciResetPortP14EhciControllerj
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	L98
	movl	$2, -16(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC8, (%esp)
	call	__Z6printfPKcz
	movl	$1000, (%esp)
	call	__Z4Waiti
	call	__Z12UsbDevCreatev
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	je	L98
	movl	-20(%ebp), %eax
	movl	$0, (%eax)
	movl	-20(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%edx, 8(%eax)
	movl	-20(%ebp), %eax
	movl	12(%ebp), %edx
	movl	%edx, 16(%eax)
	movl	-20(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 20(%eax)
	movl	-20(%ebp), %eax
	movl	$8, 28(%eax)
	movl	-20(%ebp), %eax
	movl	$__ZL14EhciDevControlP9UsbDeviceP11UsbTransfer, 36(%eax)
	movl	-20(%ebp), %eax
	movl	$__ZL11EhciDevIntrP9UsbDeviceP11UsbTransfer, 40(%eax)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10UsbDevInitP9UsbDevice
	xorl	$1, %eax
	testb	%al, %al
	je	L98
	movl	$LC9, (%esp)
	call	__Z6printfPKcz
L98:
	nop
	leave
	ret
	.def	__ZL9EhciProbeP14EhciController;	.scl	3;	.type	32;	.endef
__ZL9EhciProbeP14EhciController:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$4, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RCRP14EhciControllerj
	andl	$15, %eax
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
L101:
	movl	-12(%ebp), %eax
	cmpl	-16(%ebp), %eax
	jnb	L102
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13probeEhciPortP14EhciControllerj
	addl	$1, -12(%ebp)
	jmp	L101
L102:
	nop
	leave
	ret
	.section .rdata,"dr"
LC10:
	.ascii "Addr=%x, QueueHead info:\12\0"
	.align 4
LC11:
	.ascii " QHLP = 0x%x, EndpCh = 0x%x, EndpCap = 0x%x\12 NextLink = 0x%x, AltLink = 0x%x, Token = 0x%x\12\0"
LC12:
	.ascii " Buffers:\12\0"
LC13:
	.ascii "  [%x,%x]\0"
LC14:
	.ascii "\12\0"
	.text
	.globl	__Z7printQhP6EhciQH
	.def	__Z7printQhP6EhciQH;	.scl	2;	.type	32;	.endef
__Z7printQhP6EhciQH:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$60, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC10, (%esp)
	call	__Z6printfPKcz
	movl	8(%ebp), %eax
	movl	24(%eax), %edi
	movl	8(%ebp), %eax
	movl	20(%eax), %esi
	movl	8(%ebp), %eax
	movl	16(%eax), %ebx
	movl	8(%ebp), %eax
	movl	8(%eax), %ecx
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%edi, 24(%esp)
	movl	%esi, 20(%esp)
	movl	%ebx, 16(%esp)
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC11, (%esp)
	call	__Z6printfPKcz
	movl	$LC12, (%esp)
	call	__Z6printfPKcz
	movl	$0, -28(%ebp)
L105:
	cmpl	$4, -28(%ebp)
	jg	L104
	movl	8(%ebp), %eax
	movl	-28(%ebp), %edx
	addl	$12, %edx
	movl	(%eax,%edx,4), %edx
	movl	8(%ebp), %eax
	movl	-28(%ebp), %ecx
	addl	$4, %ecx
	movl	12(%eax,%ecx,4), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC13, (%esp)
	call	__Z6printfPKcz
	addl	$1, -28(%ebp)
	jmp	L105
L104:
	movl	$LC14, (%esp)
	call	__Z6printfPKcz
	nop
	addl	$60, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.def	_EhciControllerPollList;	.scl	3;	.type	32;	.endef
_EhciControllerPollList:
	pushl	%ebp
	movl	%esp, %ebp
	nop
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC15:
	.ascii "USB device has disconnected from EHCI!\12\0"
	.align 4
LC16:
	.ascii "USB device connected to EHCI!\12\0"
	.text
	.def	__ZL18EhciControllerPollP13UsbController;	.scl	3;	.type	32;	.endef
__ZL18EhciControllerPollP13UsbController:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	20(%eax), %eax
	addl	$72, %eax
	movl	%eax, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	_EhciControllerPollList
	movl	-24(%ebp), %eax
	movl	24(%eax), %eax
	addl	$72, %eax
	movl	%eax, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	_EhciControllerPollList
	movl	$4, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RCRP14EhciControllerj
	andl	$15, %eax
	movl	%eax, -28(%ebp)
	movl	$0, -12(%ebp)
L116:
	movl	-12(%ebp), %eax
	cmpl	-28(%ebp), %eax
	jnb	L117
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	-12(%ebp), %edx
	addl	$16, %edx
	sall	$2, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -36(%ebp)
	movl	-36(%ebp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	L109
	movl	-36(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	jne	L109
	movl	$LC15, (%esp)
	call	__Z6printfPKcz
	movl	_g_usbDeviceList, %eax
	movl	%eax, -16(%ebp)
	movl	$0, -20(%ebp)
	movl	$2, 4(%esp)
	movl	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciPortClrPVjj
L114:
	cmpl	$0, -16(%ebp)
	je	L118
	movl	-16(%ebp), %eax
	movl	16(%eax), %eax
	cmpl	-12(%ebp), %eax
	jne	L111
	movl	-16(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	cmpl	%eax, %edx
	jne	L111
	movl	-16(%ebp), %eax
	movl	48(%eax), %eax
	testl	%eax, %eax
	je	L112
	movl	-16(%ebp), %eax
	movl	48(%eax), %eax
	movl	-16(%ebp), %edx
	movl	%edx, (%esp)
	call	*%eax
L112:
	cmpl	$0, -20(%ebp)
	je	L113
	movl	-16(%ebp), %eax
	movl	4(%eax), %edx
	movl	-20(%ebp), %eax
	movl	%edx, 4(%eax)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z4freePv
	jmp	L110
L113:
	movl	-16(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, _g_usbDeviceList
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z4freePv
	jmp	L110
L111:
	movl	-16(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	-16(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -16(%ebp)
	jmp	L114
L110:
	jmp	L118
L109:
	movl	-36(%ebp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	L115
	movl	-36(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	L115
	movl	$LC16, (%esp)
	call	__Z6printfPKcz
	movl	$2, 4(%esp)
	movl	-32(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciPortClrPVjj
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13probeEhciPortP14EhciControllerj
	jmp	L115
L118:
	nop
L115:
	addl	$1, -12(%ebp)
	jmp	L116
L117:
	nop
	leave
	ret
	.section .rdata,"dr"
LC17:
	.ascii "Initializing EHCI\12\0"
LC18:
	.ascii "!!%x!\0"
	.align 4
LC19:
	.ascii "FrameList filled. Turning off Legacy BIOS support...\0"
LC20:
	.ascii ".\0"
LC21:
	.ascii "Done\12\0"
	.align 4
LC22:
	.ascii "Device configured. Probing ports...\12\0"
	.text
	.globl	__Z10_ehci_initjP13PciDeviceInfo
	.def	__Z10_ehci_initjP13PciDeviceInfo;	.scl	2;	.type	32;	.endef
__Z10_ehci_initjP13PciDeviceInfo:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$72, %esp
	movl	12(%ebp), %eax
	movzbl	4(%eax), %eax
	movzbl	%al, %eax
	sall	$8, %eax
	movl	%eax, %edx
	movl	12(%ebp), %eax
	movzbl	5(%eax), %eax
	movzbl	%al, %eax
	orl	%edx, %eax
	cmpl	$3075, %eax
	jne	L142
	movl	12(%ebp), %eax
	movzbl	6(%eax), %eax
	cmpb	$32, %al
	jne	L142
	movl	$LC17, (%esp)
	call	__Z6printfPKcz
	movl	$0, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9PciGetBarP6PciBarjj
	movl	-48(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	jne	L143
	movl	$36, (%esp)
	call	__Z6mallocj
	movl	%eax, -24(%ebp)
	movl	-56(%ebp), %edx
	movl	-24(%ebp), %eax
	movl	%edx, (%eax)
	movl	-56(%ebp), %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 32(%eax)
	movl	-56(%ebp), %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 4(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 28(%eax)
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	4(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC18, (%esp)
	call	__Z6printfPKcz
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	andl	$-51, %eax
	orl	$2, %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3WORP14EhciControllerjj
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
L125:
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
	andl	$2, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L124
	jmp	L125
L124:
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
L127:
	movl	$4, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
	andl	$4096, %eax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L126
	jmp	L127
L126:
	movl	$36, %eax
	sall	$10, %eax
	movl	%eax, (%esp)
	call	__Z6mallocj
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 8(%eax)
	movl	-24(%ebp), %eax
	movl	8(%eax), %eax
	shrl	$14, %eax
	addl	$1, %eax
	sall	$14, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 8(%eax)
	movl	$4224, %eax
	sall	$3, %eax
	movl	%eax, (%esp)
	call	__Z6mallocj
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 12(%eax)
	movl	$1152, %eax
	sall	$5, %eax
	movl	%eax, (%esp)
	call	__Z6mallocj
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 16(%eax)
	movl	-24(%ebp), %eax
	movl	12(%eax), %eax
	shrl	$14, %eax
	addl	$1, %eax
	sall	$14, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 12(%eax)
	movl	-24(%ebp), %eax
	movl	16(%eax), %eax
	shrl	$14, %eax
	addl	$1, %eax
	sall	$14, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%edx, 16(%eax)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocQHP14EhciController
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	orl	$2, %eax
	movl	%eax, %edx
	movl	-32(%ebp), %eax
	movl	%edx, (%eax)
	movl	-32(%ebp), %eax
	movl	$32768, 4(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 8(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 12(%eax)
	movl	-32(%ebp), %eax
	movl	$1, 16(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 20(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 24(%eax)
	movl	$0, -12(%ebp)
L129:
	cmpl	$4, -12(%ebp)
	ja	L128
	movl	-32(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$4, %edx
	movl	$0, 12(%eax,%edx,4)
	movl	-32(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$12, %edx
	movl	$0, (%eax,%edx,4)
	addl	$1, -12(%ebp)
	jmp	L129
L128:
	movl	-32(%ebp), %eax
	movl	$0, 68(%eax)
	movl	-32(%ebp), %eax
	leal	72(%eax), %edx
	movl	-32(%ebp), %eax
	movl	%edx, 72(%eax)
	movl	-32(%ebp), %eax
	leal	72(%eax), %edx
	movl	-32(%ebp), %eax
	movl	%edx, 76(%eax)
	movl	-24(%ebp), %eax
	movl	-32(%ebp), %edx
	movl	%edx, 20(%eax)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL11EhciAllocQHP14EhciController
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	movl	$1, (%eax)
	movl	-32(%ebp), %eax
	movl	$0, 4(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 8(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 12(%eax)
	movl	-32(%ebp), %eax
	movl	$1, 16(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 20(%eax)
	movl	-32(%ebp), %eax
	movl	$0, 24(%eax)
	movl	$0, -16(%ebp)
L131:
	cmpl	$4, -16(%ebp)
	ja	L130
	movl	-32(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$4, %edx
	movl	$0, 12(%eax,%edx,4)
	movl	-32(%ebp), %eax
	movl	-16(%ebp), %edx
	addl	$12, %edx
	movl	$0, (%eax,%edx,4)
	addl	$1, -16(%ebp)
	jmp	L131
L130:
	movl	-32(%ebp), %eax
	movl	$0, 68(%eax)
	movl	-32(%ebp), %eax
	leal	72(%eax), %edx
	movl	-32(%ebp), %eax
	movl	%edx, 72(%eax)
	movl	-32(%ebp), %eax
	leal	72(%eax), %edx
	movl	-32(%ebp), %eax
	movl	%edx, 76(%eax)
	movl	-24(%ebp), %eax
	movl	-32(%ebp), %edx
	movl	%edx, 24(%eax)
	movl	$0, -20(%ebp)
L133:
	cmpl	$1023, -20(%ebp)
	ja	L132
	movl	-24(%ebp), %eax
	movl	8(%eax), %eax
	movl	-20(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	-32(%ebp), %edx
	orl	$2, %edx
	movl	%edx, (%eax)
	addl	$1, -20(%ebp)
	jmp	L133
L132:
	movl	$LC19, (%esp)
	call	__Z6printfPKcz
	movl	$8, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RCRP14EhciControllerj
	andl	$65280, %eax
	shrl	$8, %eax
	movl	%eax, -36(%ebp)
	cmpl	$63, -36(%ebp)
	jbe	L134
	movl	-36(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9PciRead32jj
	movl	%eax, -40(%ebp)
	movl	$LC20, (%esp)
	call	__Z6printfPKcz
	movl	-40(%ebp), %eax
	andl	$65536, %eax
	testl	%eax, %eax
	je	L134
	movl	-40(%ebp), %eax
	orl	$16777216, %eax
	movl	%eax, 8(%esp)
	movl	-36(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10PciWrite32jjj
	movl	$LC20, (%esp)
	call	__Z6printfPKcz
L138:
	movl	-36(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9PciRead32jj
	movl	%eax, -40(%ebp)
	movl	$LC20, (%esp)
	call	__Z6printfPKcz
	movl	-40(%ebp), %eax
	andl	$65536, %eax
	testl	%eax, %eax
	jne	L138
	movl	-40(%ebp), %eax
	andl	$16777216, %eax
	testl	%eax, %eax
	jne	L144
	jmp	L138
L144:
	nop
L134:
	movl	$LC21, (%esp)
	call	__Z6printfPKcz
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$0, 8(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$0, 12(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	-24(%ebp), %edx
	movl	8(%edx), %edx
	movl	%edx, 20(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	-24(%ebp), %edx
	movl	20(%edx), %edx
	movl	%edx, 24(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$0, 16(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$63, 4(%eax)
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$524337, (%eax)
L140:
	movl	$4, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z3RORP14EhciControllerj
	andl	$4096, %eax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L139
	jmp	L140
L139:
	movl	-24(%ebp), %eax
	movl	4(%eax), %eax
	movl	$1, 64(%eax)
	movl	$LC22, (%esp)
	call	__Z6printfPKcz
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__ZL9EhciProbeP14EhciController
	movl	$12, (%esp)
	call	__Z6mallocj
	movl	%eax, -44(%ebp)
	movl	_g_usbControllerList, %edx
	movl	-44(%ebp), %eax
	movl	%edx, (%eax)
	movl	-44(%ebp), %eax
	movl	-24(%ebp), %edx
	movl	%edx, 4(%eax)
	movl	-44(%ebp), %eax
	movl	$__ZL18EhciControllerPollP13UsbController, 8(%eax)
	movl	-44(%ebp), %eax
	movl	%eax, _g_usbControllerList
	jmp	L119
L142:
	nop
	jmp	L119
L143:
	nop
L119:
	leave
	ret
	.ident	"GCC: (tdm-1) 5.1.0"
	.def	__Z6printfPKcz;	.scl	2;	.type	32;	.endef
	.def	__Z10LinkBeforeP4LinkS0_;	.scl	2;	.type	32;	.endef
	.def	__Z10LinkRemoveP4Link;	.scl	2;	.type	32;	.endef
	.def	__Z12UsbDevCreatev;	.scl	2;	.type	32;	.endef
	.def	__Z10UsbDevInitP9UsbDevice;	.scl	2;	.type	32;	.endef
	.def	__Z4freePv;	.scl	2;	.type	32;	.endef
	.def	__Z9PciGetBarP6PciBarjj;	.scl	2;	.type	32;	.endef
	.def	__Z6mallocj;	.scl	2;	.type	32;	.endef
	.def	__Z9PciRead32jj;	.scl	2;	.type	32;	.endef
	.def	__Z10PciWrite32jjj;	.scl	2;	.type	32;	.endef
