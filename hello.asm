; Hello World C compiler
; Task 6, by Begichev and Shishkin

; assemble:              nasm -f elf -l hello.lst hello.asm
; assemble with debug:   nasm -g -f elf -l hello.lst hello.asm
; link:                  gcc -m32 hello.o -o hello
; run:                   hello

[section .text]
	global main

main:
	; CONST_953 != CONST_51 -> TEMP_0
	; Comparation work only whith floats!
	fld qword [CONST_953]
	fld qword [CONST_51]
	fcomip
jne lbl_0_jne_true
	fldz
	jmp lbl_0_jne_exit
lbl_0_jne_true:
	fld1
lbl_0_jne_exit:
	fistp dword [TEMP_0]

	; a = TEMP_0 -> a
	fild dword [TEMP_0]
	fstp qword [TEMP_1]
	fld qword [TEMP_1]
	fstp qword [a]

	; Exit program
	mov eax, 1
	int 80h


[section .data]
	a dq 0.0
	TEMP_1 dq 0.0
	TEMP_0 dd 0
	CONST_51 dq 2.000000
	CONST_953 dq 1.000000
