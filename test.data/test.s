section .text

extern my_printf
extern my_scanf

global _start

_start:
		call main
		mov rax, 0x3C		;exit
		xor rdi, rdi
		syscall

main:
		push rbp
		mov rbp, rsp
		sub rsp, 32			;allocate memory
		mov qword [rbp - 8], 50		;write value [n]
		mov qword [rbp - 16], 1000		;write value [count]
		mov qword [rbp - 24], 0		;write value [cur]
.while_0:
		mov rax, qword [rbp - 16]
		cmp qword [rbp - 24], rax
		jl .begin_while_0
		jmp .end_while_0
.begin_while_0:
		mov rdi, qword [rbp - 8]
		call Fact
		mov qword [rbp - 32], rax		;write value [tmp]
		mov rax, qword [rbp - 24]
		add rax, 1		;add
		mov qword [rbp - 24], rax		;write value [cur]
		jmp .while_0
.end_while_0:
		leave
		ret

Fact:
		push rbp
		mov rbp, rsp
		sub rsp, 16			;allocate memory
;save arguments to memory
		mov qword [rbp - 8], rdi		;write value [n]
		cmp qword [rbp - 8], 1
		jle .if_0
		jmp .end_if_0
.if_0:
		mov rax, 1		;pass the argument
		leave
		ret

.end_if_0:
		mov rax, qword [rbp - 8]
		sub rax, 1		;sub
		mov qword [rbp - 16], rax		;write value [tmp]
		mov rdi, qword [rbp - 16]
		call Fact
		imul rax, qword [rbp - 8]		;mul
		leave
		ret

section .rodata
