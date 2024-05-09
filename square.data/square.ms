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

		push 0
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

		push 0
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
		push 97
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

		pop [rbx]		;write value [a]
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
		push 98
		outc
		push 58
		outc
		push 32
		outc
		in
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		pop [rbx]		;write value [b]
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
		push 99
		outc
		push 58
		outc
		push 32
		outc
		in
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		pop [rbx]		;write value [c]
		push 0
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		je if_0
		jmp end_if_0
:if_0
		push rcx
		push rdx
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [c]
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		call Linear
		pop rdx
		pop rcx
		ret
:end_if_0
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		mul
		push 4
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [c]
		mul
		mul
		sub
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [d]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 0
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		push [rbx]		;get value [d]
		jae if_1
		push 110
		outc
		push 111
		outc
		push 32
		outc
		push 114
		outc
		push 111
		outc
		push 111
		outc
		push 116
		outc
		push 115
		outc
		jmp end_if_1
:if_1
		push rcx
		push rdx
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		push [rbx]		;get value [d]
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [c]
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
		call Square
		pop rdx
		pop rcx
:end_if_1
		ret
:Linear
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

		pop [rbx]		;fill variable [b]
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

		pop [rbx]		;fill variable [c]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 0
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		je if_2
		jmp end_if_2
:if_2
		push 0
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [c]
		je if_3
		push 110
		outc
		push 111
		outc
		push 32
		outc
		push 114
		outc
		push 111
		outc
		push 111
		outc
		push 116
		outc
		push 115
		outc
		jmp end_if_3
:if_3
		push 105
		outc
		push 110
		outc
		push 102
		outc
		push 32
		outc
		push 114
		outc
		push 111
		outc
		push 111
		outc
		push 116
		outc
		push 115
		outc
:end_if_3
		ret
:end_if_2
		push -1
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [c]
		mul
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		div
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [x]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 120
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
;set variable index [2]
		push 2
		push rcx
		add
		pop rbx

		push [rbx]		;get value [x]
		out
		ret
:Square
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

;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [d]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 0
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		push [rbx]		;get value [d]
		je if_4
		jmp end_if_4
:if_4
		push -1
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		mul
		push 2
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		mul
		div
;set variable index [4]
		push 4
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [x]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 120
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
;set variable index [4]
		push 4
		push rcx
		add
		pop rbx

		push [rbx]		;get value [x]
		out
		ret
:end_if_4
		push -1
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		mul
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		push [rbx]		;get value [d]
		sqrt
		add
		push 2
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		mul
		div
;set variable index [5]
		push 5
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [x1]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push -1
;set variable index [1]
		push 1
		push rcx
		add
		pop rbx

		push [rbx]		;get value [b]
		mul
;set variable index [3]
		push 3
		push rcx
		add
		pop rbx

		push [rbx]		;get value [d]
		sqrt
		sub
		push 2
;set variable index [0]
		push 0
		push rcx
		add
		pop rbx

		push [rbx]		;get value [a]
		mul
		div
;set variable index [6]
		push 6
		push rcx
		add
		pop rbx

		pop [rbx]		;fill variable [x2]
;inc rdx
		push 1
		push rdx
		add
		pop rdx

		push 120
		outc
		push 49
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
;set variable index [5]
		push 5
		push rcx
		add
		pop rbx

		push [rbx]		;get value [x1]
		out
		push 10
		outc
		push 120
		outc
		push 50
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
;set variable index [6]
		push 6
		push rcx
		add
		pop rbx

		push [rbx]		;get value [x2]
		out
		ret
