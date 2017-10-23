GLOBAL processSwitch

EXTERN switchUserToKernel

EXTERN switchKernelToUser

section .text

; Ref https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/CPU/irqHandlers.asm

processSwitch:
	mov rdi, rsp ; salvo puntero a stack

	call switchUserToKernel ; se pasa a modo kernel. Ver scheduler.c

	mov rsp, rax ; retorno de switchUserToKernel es el puntero al stack del kernel

	; En la realidad tendria que pushear y luego popear el contexto del stack pero no lo hago porque no hago nada con el kernel en el tp durante el process switch.

	mov rdi, rsp ; paso como param stack del kernel.

	call switchKernelToUser ; se pasa a modo user. Ver scheduler.c

	mov rsp, rax ; retorno de switchKernelToUser es el puntero al stack del siguiente proceso a ejecutar

	ret