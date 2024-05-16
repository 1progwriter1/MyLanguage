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
		mov qword [rbp - 8], 0		;write value [a]
		mov qword [rbp - 16], 0		;write value [b]
		mov qword [rbp - 24], 0		;write value [c]
		mov rdi, STR0		;print Enter a: 
		call my_printf
		call my_scanf		;read number to rax
		mov qword [rbp - 8], rax		;write value [a]
		mov rdi, STR1		;print Enter b: 
		call my_printf
		call my_scanf		;read number to rax
		mov qword [rbp - 16], rax		;write value [b]
		mov rdi, STR2		;print Enter c: 
		call my_printf
		call my_scanf		;read number to rax
		mov qword [rbp - 24], rax		;write value [c]
		cmp qword [rbp - 8], 0
		je .if_0
		jmp .end_if_0
.if_0:
		mov rdi, qword [rbp - 16]
		mov rsi, qword [rbp - 24]
		call Linear
		leave
		ret

.end_if_0:
		mov rax, qword [rbp - 16]
		imul rax, qword [rbp - 16]		;mul
		mov rbx, qword [rbp - 8]
		imul rbx, qword [rbp - 24]		;mul
		mov rcx, 4
		imul rcx, rbx		;mul
		sub rax, rcx		;sub
		mov qword [rbp - 32], rax		;write value [d]
		cmp qword [rbp - 32], 0
		jge .if_1
		mov rdi, STR3		;print no roots
		call my_printf
		jmp .end_if_1
.if_1:
		mov rdi, qword [rbp - 8]
		mov rsi, qword [rbp - 16]
		mov rdx, qword [rbp - 24]
		mov rcx, qword [rbp - 32]
		call Square
.end_if_1:
		leave
		ret

Linear:
		push rbp
		mov rbp, rsp
		sub rsp, 24			;allocate memory
;save arguments to memory
		mov qword [rbp - 8], rdi		;write value [b]
		mov qword [rbp - 16], rsi		;write value [c]
		cmp qword [rbp - 8], 0
		je .if_2
		jmp .end_if_2
.if_2:
		cmp qword [rbp - 16], 0
		je .if_3
		mov rdi, STR4		;print no roots
		call my_printf
		jmp .end_if_3
.if_3:
		mov rdi, STR5		;print inf roots
		call my_printf
.end_if_3:
		leave
		ret

.end_if_2:
		mov rax, -1
		imul rax, qword [rbp - 16]		;mul
		mov rax, rax
		cqo
		idiv qword qword [rbp - 8]		;div
		mov qword [rbp - 24], rax		;write value [x]
		mov rdi, STR6		;print x = 
		call my_printf
		mov rdi, STR7
		mov rsi, qword [rbp - 24]
		call my_printf		;print number
		leave
		ret

Square:
		push rbp
		mov rbp, rsp
		sub rsp, 64			;allocate memory
;save arguments to memory
		mov qword [rbp - 8], rdi		;write value [a]
		mov qword [rbp - 16], rsi		;write value [b]
		mov qword [rbp - 24], rdx		;write value [c]
		mov qword [rbp - 32], rcx		;write value [d]
		cmp qword [rbp - 32], 0
		je .if_4
		jmp .end_if_4
.if_4:
		mov rax, -1
		imul rax, qword [rbp - 16]		;mul
		mov rbx, 2
		imul rbx, qword [rbp - 8]		;mul
		mov rax, rax
		cqo
		idiv qword rbx		;div
		mov qword [rbp - 40], rax		;write value [x]
		mov rdi, STR8		;print x = 
		call my_printf
		mov rdi, STR9
		mov rsi, qword [rbp - 40]
		call my_printf		;print number
		leave
		ret

.end_if_4:
		fild qword [rbp - 32]
		fsqrt		;sqrt
		fisttp qword [rbp - 32]
		mov rax, qword [rbp - 32]
		mov qword [rbp - 48], rax		;write value [tmp]
		mov rax, -1
		imul rax, qword [rbp - 16]		;mul
		add rax, qword [rbp - 48]		;add
		mov rbx, 2
		imul rbx, qword [rbp - 8]		;mul
		mov rax, rax
		cqo
		idiv qword rbx		;div
		mov qword [rbp - 56], rax		;write value [x1]
		mov rax, -1
		imul rax, qword [rbp - 16]		;mul
		sub rax, qword [rbp - 48]		;sub
		mov rbx, 2
		imul rbx, qword [rbp - 8]		;mul
		mov rax, rax
		cqo
		idiv qword rbx		;div
		mov qword [rbp - 64], rax		;write value [x2]
		mov rdi, STR10		;print x1 = 
		call my_printf
		mov rdi, STR11
		mov rsi, qword [rbp - 56]
		call my_printf		;print number
		mov rdi, STR12		;print \nx2 = 
		call my_printf
		mov rdi, STR13
		mov rsi, qword [rbp - 64]
		call my_printf		;print number
		leave
		ret

section .rodata
STR0:
		db "Enter a: ", 0x0
STR1:
		db "Enter b: ", 0x0
STR2:
		db "Enter c: ", 0x0
STR3:
		db "no roots", 0x0
STR4:
		db "no roots", 0x0
STR5:
		db "inf roots", 0x0
STR6:
		db "x = ", 0x0
STR7:
		db "%d", 0x0
STR8:
		db "x = ", 0x0
STR9:
		db "%d", 0x0
STR10:
		db "x1 = ", 0x0
STR11:
		db "%d", 0x0
STR12:
		db "", 0xA, "x2 = ", 0x0
STR13:
		db "%d", 0x0
