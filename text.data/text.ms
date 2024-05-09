		call main
		push 10
		outc
		hlt
:main
		push 0
		pop [0]		;fill variable [a]
		push 10
		pop [1]		;fill variable [b]
		push [0]		;get value [a]
		push [1]		;get value [b]
		call difference
		pop rax
		out
		ret
:difference
		pop [2]		;[a] save to ram
		pop [3]		;[b] save to ram
		push [2]		;get value [a]
		push [3]		;get value [b]
		sub
		push rax
		ret
