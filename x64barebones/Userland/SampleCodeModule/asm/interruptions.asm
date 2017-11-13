GLOBAL systemCall
GLOBAL yield

section .text

systemCall:
	push rbp
	mov rbp, rsp

	;recivo en rdi el valor que describe que system call es
	int 80h

	mov rsp, rbp
	pop rbp
	ret

yield:
	push rbp
	mov rbp, rsp

	int 71h

	mov rsp, rbp
	pop rbp
	ret