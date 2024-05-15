section .text

global _start

_start:
		call main
		mov rax, 0x3C		;exit
		xor rdi, rdi
		syscall

main:
		push rbp
		mov rbp, rsp
		mov qword [rbp - 8], 0		;write value [a]
		mov qword [rbp - 16], 10		;write value [b]
		pop rbp
		ret

difference:
		push rbp
		mov rbp, rsp
;save arguments to memory
		mov qword [rbp - 8], rdi		;write value [a]
		mov qword [rbp - 16], rsi		;write value [b]
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		push 12
		add
		push 3
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		mul
		sub
		push 4
		add
		push 5
		add
		pop rax
		pop rbp
		ret

