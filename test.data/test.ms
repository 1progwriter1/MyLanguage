		call main
		push 10
		outc
		hlt
:main
;zero registers
		push 0
		push 0
		pop rcx
		pop rdx

		push 50
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [n]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 1000
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [count]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 0
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [cur]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

:while_0
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [count]
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [cur]
		jb begin_while_0
		jmp end_while_0
:begin_while_0
		push rcx
		push rdx
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [n]
		call Fact
		pop rdx
		pop rcx
		push rax
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [tmp]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [cur]
		push 1
		add
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		pop [rbx]		;write value [cur]
		jmp while_0
:end_while_0
		ret
:Fact
;set segment
		push rcx
		push rdx
		add
		pop rcx
		push 0
		pop rdx

;save arguments to memory
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [n]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 1
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [n]
		jbe if_0
		jmp end_if_0
:if_0
		push 1
		pop rax
		ret
:end_if_0
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [n]
		push 1
		sub
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [tmp]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push rcx
		push rdx
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [tmp]
		call Fact
		pop rdx
		pop rcx
		push rax
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [n]
		mul
		pop rax
		ret
