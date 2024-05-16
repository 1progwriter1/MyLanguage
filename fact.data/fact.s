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
		sub rsp, 16			;allocate memory
		mov qword [rbp - 8], 0		;write value [n]
		mov rdi, STR0		;print Enter n: 
		call my_printf
		call my_scanf		;read number to rax
		mov qword [rbp - 8], rax		;write value [n]
		mov rdi, qword [rbp - 8]
		call Fact
		mov qword [rbp - 16], rax		;write value [ans]
		mov rdi, STR1		;print Answer: 
		call my_printf
		mov rdi, STR2
		mov rsi, qword [rbp - 16]
		call my_printf		;print number
		mov rdi, STR3		;print \n
		call my_printf
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
		add rax, 1		;add
		mov qword [rbp - 8], rax		;write value [n]
		mov rax, qword [rbp - 8]
		sub rax, 2		;sub
		mov rdi, rax
		call Fact
		mov rbx, qword [rbp - 8]
		sub rbx, 1		;sub
		imul rax, rbx		;mul
		mov qword [rbp - 16], rax		;write value [tmp_fact]
		mov rax, [rbp - 16]		;pass the argument
		leave
		ret

section .rodata
STR0:
		db "Enter n: ", 0x0
STR1:
		db "Answer: ", 0x0
STR2:
		db "%d", 0x0
STR3:
		db "", 0xA, "", 0x0
