[section .text]
	global _start

_start:
	; CONST_675 + CONST_953 -> TEMP_0
	fild qword [CONST_675]
	fadd qword [CONST_953]
	fstp qword [TEMP_0]

	; a = TEMP_0 -> a
	fld qword [TEMP_0]
	fstp qword [a]

	; Finish is here
	mov eax, 1
	int 0x80


[section .data]
	a dq 0.0
	TEMP_0 dq 0.0
	CONST_953 dq 1.000000
	CONST_675 dd 1
