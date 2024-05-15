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

		pop [rbx]		;fill variable [a]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 10
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [b]
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

		push [rbx]		;get value [b]
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		call difference
		pop rdx
		pop rcx
		push rax
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [c]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		ret
:difference
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

		pop [rbx]		;fill variable [a]
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

		pop [rbx]		;fill variable [b]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		sub
		pop rax
		ret
