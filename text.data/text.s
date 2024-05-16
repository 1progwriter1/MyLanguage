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
		mov qword [rbp - 16], 10		;write value [b]
		call my_scanf		;read number to rax
		mov qword [rbp - 8], rax		;write value [a]
		call my_scanf		;read number to rax
		mov qword [rbp - 16], rax		;write value [b]
		mov rdi, STR0		;print Here is the answer: 
		call my_printf
		mov rdi, qword [rbp - 8]
		mov rsi, qword [rbp - 16]
		call test
		mov rdi, STR1
		mov rsi, rax
		call my_printf		;print number
		mov rdi, STR2		;print \n
		call my_printf
		leave
		ret

