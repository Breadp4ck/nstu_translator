#!/bin/bash

nasm -g -f elf -l hello.lst hello.asm
gcc -m32 -o hello hello.o

rm hello.o hello.lst
gdb hello
