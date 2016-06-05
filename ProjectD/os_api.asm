.global _syscall_printString
.global _syscall_readString
.global _syscall_readSector
.global _syscall_writeSector
.global _syscall_readFile
.global _syscall_writeFile
.global _syscall_executeProgram
.global _syscall_terminate
.global _syscall_deleteFile
.global _syscall_clearScreen
.global _syscall_listFiles
.global _syscall_copyFile

_syscall_readString:
	push bp
	mov bp,sp
	mov ax, #1
	mov bx, [bp+4]
	int #0x21
	pop bp
	ret
	
_syscall_terminate:
	push bp
	mov bp,sp
	mov ax, #5
	int #0x21
	pop bp
	ret
	
_syscall_executeProgram:
	push bp
	mov bp,sp
	mov ax, #4
	mov bx, [bp+4]
	mov cx, [bp+6]
	int #0x21
	pop bp
	ret	
	
_syscall_copyFile:
	push bp
	mov bp,sp
	mov ax, #9
	mov bx, [bp+4]
	mov cx, [bp+6]
	int #0x21
	pop bp
	ret	
	
_syscall_writeFile:
	push bp
	mov bp,sp
	mov ax, #8
	mov bx, [bp+4]
	mov cx, [bp+6]
	mov dx, [bp+8]
	int #0x21
	pop bp
	ret
	

_syscall_listFiles:
	push bp
	mov bp,sp
	mov ax,#11
	int #0x21
	pop bp
	ret

_syscall_readFile:
	push bp
	mov bp,sp
	mov ax, #3
	mov bx, [bp+4]
	mov cx, [bp+6]
	int #0x21
	pop bp
	ret

_syscall_clearScreen:
	push bp
	mov bp,sp
	mov ax, #0xa
	int #0x21
	pop bp

_syscall_readSector:
	push bp
	mov bp,sp
	mov ax, #2
	mov bx, [bp+4]
	mov cx, [bp+6]
	int #0x21
	pop bp
	ret
	
_syscall_writeSector:
	push bp
	mov bp,sp
	mov ax, #6
	mov bx, [bp+4]
	mov cx, [bp+6]
	int #0x21
	pop bp
	ret

_syscall_deleteFile:
	push bp
	mov bp,sp
	mov ax, #7
	mov bx, [bp+4]
	int #0x21
	pop bp
	ret

_syscall_printString:
	push bp
	mov bp,sp
	mov ax, #0
	mov bx, [bp+4]
	int #0x21
	pop bp
	ret

