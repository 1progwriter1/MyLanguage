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

		push 0
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

		push 69
		outc
		push 110
		outc
		push 116
		outc
		push 101
		outc
		push 114
		outc
		push 32
		outc
		push 110
		outc
		push 58
		outc
		push 32
		outc
		in
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		pop [rbx]		;write value [n]
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
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [ans]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 65
		outc
		push 110
		outc
		push 115
		outc
		push 119
		outc
		push 101
		outc
		push 114
		outc
		push 58
		outc
		push 32
		outc
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [ans]
		out
		push 10
		outc
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
		add
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		pop [rbx]		;write value [n]
		push rcx
		push rdx
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [n]
		push 2
		sub
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
		push 1
		sub
		mul
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [tmp_fact]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [tmp_fact]
		pop rax
		ret
