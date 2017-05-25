bits 32
	section .text
	bits 32
	align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)

	global start
	extern kmain

start:
	cli
	call kmain
	hlt

	section .bss
	resb 8192		;8KB for stack

stack_space: