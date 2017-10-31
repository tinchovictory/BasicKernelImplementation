GLOBAL processSwitch
GLOBAL endLoadingKernel

EXTERN switchUserToKernel
EXTERN runScheduler
EXTERN switchKernelToUser
EXTERN currentProcessEntryPoint

section .text

; Ref https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/CPU/irqHandlers.asm

%macro pushaq 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push fs
	push gs
%endmacro

%macro popaq 0
	pop gs
	pop fs
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

;tickHandler
processSwitch:

	pushaq

	mov rdi, rsp ; salvo puntero a stack

	call switchUserToKernel ; se pasa a modo kernel. Ver scheduler.c

	mov rsp, rax ; retorno de switchUserToKernel es el puntero al stack del kernel

	; En la realidad tendria que pushear y luego popear el contexto del stack pero no lo hago porque no hago nada con el kernel en el tp durante el process switch.

	call runScheduler

	call switchKernelToUser ; se pasa a modo user. Ver scheduler.c

	mov rsp, rax ; retorno de switchKernelToUser es el puntero al stack del siguiente proceso a ejecutar

	popaq

	mov al, 20h ; EOI
	out 20h, al

	iretq


endLoadingKernel:
	call switchKernelToUser

	mov rsp, rax

	call currentProcessEntryPoint

	sti
	jmp rax

	ret