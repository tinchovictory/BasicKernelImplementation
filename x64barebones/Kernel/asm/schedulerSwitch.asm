GLOBAL processSwitch
GLOBAL endLoadingKernel
GLOBAL threadSwitch

EXTERN switchUserToKernel
EXTERN runScheduler
EXTERN switchKernelToUser
EXTERN currentProcessEntryPoint
EXTERN quantumCheck
EXTERN nextThread

EXTERN acaEstoy

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

	push rbp
	mov rbp, rsp

	call quantumCheck

	cmp eax, 0
	je END

	cmp eax, 2
	je THREAD

	mov rsp, rbp
	pop rbp

	; Save current context.
	pushaq

	; Call switchUserToKernel fn in C with rsp as parameter.
	mov rdi, rsp 
	call switchUserToKernel

	; fn returns kernel stack. Start using kernel stack.
	mov rsp, rax

	; Now i'm in kernel space
	; Run scheculer for process switch
	call runScheduler

	; Change to user space. Kernel stack will remain as it was since no register was pushed.
	call switchKernelToUser

	; fn returns user stack. Start using user stack
	mov rsp, rax ; retorno de switchKernelToUser es el puntero al stack del siguiente proceso a ejecutar

	; Restore context
	popaq

	; End interruption
	mov al, 20h ; EOI
	out 20h, al

	iretq

END:
	mov rsp, rbp
	pop rbp

	; End interruption
	mov al, 20h ; EOI
	out 20h, al

	iretq

THREAD:
	mov rsp, rbp
	pop rbp

	; Save current context.
	pushaq

	; Call nextThread fn in C with rsp as parameter.
	mov rdi, rsp 
	call nextThread

	; fn returns next thread stack.
	mov rsp, rax

	; Restore context
	popaq

	; End interruption
	mov al, 20h ; EOI
	out 20h, al

	iretq



; Load first process
endLoadingKernel:

	; Load first process user stack.
	call switchKernelToUser
	mov rsp, rax

	; Get first process entry point.
	call currentProcessEntryPoint

	; Allow interruptions.
	sti

	; Start running first process entry point.
	jmp rax

	ret

; 
threadSwitch:
	