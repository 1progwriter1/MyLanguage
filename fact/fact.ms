		call main
		push 10
		outc
		hlt
:main
		push 0
		pop [0]		;fill variable [n]
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
		pop [0]		;fill variable [n]
		call n
		call Fact
		pop rax
		pop [1]		;fill variable [ans]
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
		push [1]		;get value [ans]
		out
		ret
:Fact
		pop [2]		;[n] save to ram
		push 1
		push [2]		;get value [n]
		jbe if_0
		jmp end_if_0
:if_0
		push 1
		push rax
		ret
:end_if_0
		push [2]		;get value [n]
		push 1
		sub
		pop [3]		;fill variable [tmp]
		call tmp
		call Fact
		pop rax
		push [2]		;get value [n]
		mul
		push rax
		ret
