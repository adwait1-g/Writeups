;SHELL=/bin/bash instead of /bin/sh

section .text
	global _start

_start:
	xor eax, eax
	push eax
	push 0x68736162
	push 0x2f2f6e69
	sub esp, 0x04
	mov dword [esp], 0x622f0000

	mov eax, 0x01
	xor ebx, ebx
	int 0x80
