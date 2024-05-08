		call main
		push 10
		outc
		hlt
:main
		push 0
		pop [0]		;fill variable [a]
		push 0
		pop [1]		;fill variable [b]
		push 0
		pop [2]		;fill variable [c]
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
		pop [0]		;fill variable [a]
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
		pop [1]		;fill variable [b]
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
		pop [2]		;fill variable [c]
		push 0
		push [0]		;get value [a]
		je if_0
		jmp end_if_0
:if_0
		push [1]		;get value [b]
		push [2]		;get value [c]
		call Linear
		ret
:end_if_0
		push [1]		;get value [b]
		push [1]		;get value [b]
		mul
		push 4
		push [0]		;get value [a]
		push [2]		;get value [c]
		mul
		mul
		sub
		pop [3]		;fill variable [d]
		push 0
		push [3]		;get value [d]
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
		push [0]		;get value [a]
		push [1]		;get value [b]
		push [2]		;get value [c]
		push [3]		;get value [d]
		call Square
:end_if_1
		ret
:Linear
		pop [4]		;[b] save to ram
		pop [5]		;[c] save to ram
		push 0
		push [4]		;get value [b]
		je if_2
		jmp end_if_2
:if_2
		push 0
		push [5]		;get value [c]
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
		push [5]		;get value [c]
		mul
		push [4]		;get value [b]
		div
		pop [6]		;fill variable [x]
		push 120
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
		push [6]		;get value [x]
		out
		ret
:Square
		pop [7]		;[a] save to ram
		pop [8]		;[b] save to ram
		pop [9]		;[c] save to ram
		pop [10]		;[d] save to ram
		push 0
		push [10]		;get value [d]
		je if_4
		jmp end_if_4
:if_4
		push -1
		push [8]		;get value [b]
		mul
		push 2
		push [7]		;get value [a]
		mul
		div
		pop [11]		;fill variable [x]
		push 120
		outc
		push 32
		outc
		push 61
		outc
		push 32
		outc
		push [11]		;get value [x]
		out
		ret
:end_if_4
		push -1
		push [8]		;get value [b]
		mul
		push [10]		;get value [d]
		sqrt
		add
		push 2
		push [7]		;get value [a]
		mul
		div
		pop [12]		;fill variable [x1]
		push -1
		push [8]		;get value [b]
		mul
		push [10]		;get value [d]
		sqrt
		sub
		push 2
		push [7]		;get value [a]
		mul
		div
		pop [13]		;fill variable [x2]
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
		push [12]		;get value [x1]
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
		push [13]		;get value [x2]
		out
		ret
