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
		mov qword [rbp - 8], 0		;write value [a]
		mov qword [rbp - 16], 0		;write value [n_steps]
.while_0:
		mov rax, qword [rbp - 8]
		cmp qword [rbp - 8], rax
		je .begin_while_0
		jmp .end_while_0
.begin_while_0:
		cmp qword [rbp - 16], 2000000000
		je .if_0
		jmp .end_if_0
.if_0:
		mov rdi, 0
		mov rsi, qword [rbp - 8]
		call plant
		mov qword [rbp - 16], 0		;write value [n_steps]
		mov rax, qword [rbp - 8]
		add rax, 1		;add
		mov qword [rbp - 8], rax		;write value [a]
.end_if_0:
		mov rax, qword [rbp - 16]
		add rax, 1		;add
		mov qword [rbp - 16], rax		;write value [n_steps]
		jmp .while_0
.end_while_0:
		leave
		ret

plant:
		push rbp
		mov rbp, rsp
		sub rsp, 16			;allocate memory
;save arguments to memory
		mov qword [rbp - 8], rdi		;write value [a]
		mov qword [rbp - 16], rsi		;write value [b]
		mov rdi, STR0
		mov rsi, 0
		call my_printf		;print number
		mov rdi, STR1		;print  
		call my_printf
		mov rdi, STR2
		mov rsi, 0
		call my_printf		;print number
		mov rdi, STR3		;print  
		call my_printf
		mov rdi, STR4
		mov rsi, qword [rbp - 8]
		call my_printf		;print number
		mov rdi, STR5		;print  
		call my_printf
		mov rdi, STR6
		mov rsi, qword [rbp - 16]
		call my_printf		;print number
		mov rdi, STR7		;print  
		call my_printf
		mov rdi, STR8
		mov rsi, -1
		call my_printf		;print number
		mov rdi, STR9		;print  
		call my_printf
		leave
		ret

section .rodata
STR0:
		db "%d", 0x0
STR1:
		db " ", 0x0
STR2:
		db "%d", 0x0
STR3:
		db " ", 0x0
STR4:
		db "%d", 0x0
STR5:
		db " ", 0x0
STR6:
		db "%d", 0x0
STR7:
		db " ", 0x0
STR8:
		db "%d", 0x0
STR9:
		db " ", 0x0
