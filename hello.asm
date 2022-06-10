; Hello World C compiler
; Task 6, by Begichev and Shishkin

; assemble:              nasm -f elf -l hello.lst hello.asm
; assemble with debug:   nasm -g -f elf -l hello.lst hello.asm
; link:                  gcc -m32 hello.o -o hello
; run:                   hello

[section .text]
	global main

main:
	; CONST_675 + CONST_675 -> TEMP_0
	fild dword [CONST_675]
	fiadd dword [CONST_675]
	fistp dword [TEMP_0]

	; CONST_953 + TEMP_0 -> TEMP_1
	fld qword [CONST_953]
	fiadd dword [TEMP_0]
	fstp qword [TEMP_1]

	; a = TEMP_1 -> a
	fld qword [TEMP_1]
	fstp qword [TEMP_1]
	fld qword [TEMP_1]
	fstp qword [a]

	; Exit program
	mov eax, 1
	int 0x80


[section .data]
	a dq 0.0
	TEMP_1 dq 0.0
	TEMP_0 dd 0
	CONST_675 dd 1
	CONST_953 dq 1.000000
