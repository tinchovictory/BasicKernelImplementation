GLOBAL sti
GLOBAL cli
GLOBAL setPicMaster
GLOBAL setPicSlave
GLOBAL irq0Handler
GLOBAL irq1Handler
GLOBAL irq11Handler
GLOBAL sysCallHandler
GLOBAL yieldHandler

EXTERN irqDispatcher
EXTERN systemCall
EXTERN yieldSwitch
EXTERN switchUserToKernel
EXTERN switchKernelToUser

%include "./asm/macros.m"

section .text

;irq0Handler:
;	irqHandler 0
irq1Handler:
	irqHandler 1
irq11Handler:
	irqHandlerSlave 11
	
yieldHandler:
	pushaq

	mov rdi, rsp 
	call switchUserToKernel

	; fn returns kernel stack. Start using kernel stack.
	mov rsp, rax

	; Now i'm in kernel space
	; Run scheculer for process switch
	call yieldSwitch

	; Change to user space. Kernel stack will remain as it was since no register was pushed.
	call switchKernelToUser

	; fn returns user stack. Start using user stack
	mov rsp, rax 

	popaq

	iretq


sysCallHandler:
	pushaq

	;recivo parametros en un orden y es el mismo orden de salida
	call systemCall
	
	mov [return], rax ; guardo el valor de retorno

	popaq

	mov rax, [return]

	iretq


sti:
	sti
	ret

cli:
	cli
	ret

	
setPicMaster:
	push rbp
	mov rbp, rsp
	
	mov rax, rdi
	out 21h, al
	
	mov rsp, rbp
	pop rbp
	ret


setPicSlave:
	push rbp
	mov rbp, rsp
	
	mov rax, rdi
	out 0A1h, al
	
	mov rsp, rbp
	pop rbp
	ret


section .bss
	return resq 1