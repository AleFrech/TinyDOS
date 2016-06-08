;kernel.asm
;Michael Black, 2007
;kernel.asm contains assembly functions that you can use in your kernel

	.global _putInMemory
	.global _interrupt
	.global _makeInterrupt21
        .global _makeTimerInterrupt
        .global _loadProgram
	.global _printChar
	.global _readChar
	.global _readSector
	.global _launchProgram
	.global _moveToSegment
	.global _returnFromTimer
	.global _initializeProgram
	.global _setKernelDataSegment
	.global _restoreDataSegment
	.global _setCursor
	.global _clearScreen
	.global _writeSector
	.extern _handleInterrupt21
        .extern _handleTimerInterrupt
	
    
;void printChar(char chr);

_printChar:
	push bp
	mov bp, sp
	mov al, [bp+4]  
	mov ah, #0x0e			
	int #0x10
	pop bp
	ret
		
_readChar:
	int #0x16
	ret
	
;_readSector(char *buffer, int sector)

_readSector:
	push bp
	mov bp, sp
	sub sp, #6
	mov bx, [bp+4]
	mov ax, [bp+6] ;sector
	mov cl, #36 ;track del sector
	div cl 
	mov ah,#0
	mov [bp-2],ax ;ax = track
	mov ax, [bp+6] ;sector
	mov cl, #18 ;calc head
	div cl	
	and al, #0x1 ; cambiando resultado a residuo
	mov dx,#0
	mov dl, al   ; mov residuo a dl
	mov [bp-4], dx ;head 
	add ah,#1     ; relative sector +1
	mov dx,#0 ;cleaning dx
	mov dl, ah  ;ya contiene el valor (sector mod 18) + 1 ah = +1 al = sector mod 18
	mov [bp-6], dx ;relative sector
	mov ah, #0x2 	;bios read sector call
	mov al, #0x1 ;leer solo un sector
	mov ch, [bp-2] ;track 
	mov cl, [bp-6] ;relative sector
	mov dh, [bp-4] ; head number
	mov dl, #0  ;floppy image use 0
	int #0x13 
	add sp,#6 ; nivelar la pila
	pop bp
	ret
	
;_writeSector(char *buffer, int sector)
_writeSector:
	push bp
	mov bp, sp
	sub sp, #6
	mov bx, [bp+4]
	mov ax, [bp+6]
	mov cl, #36
	div cl
	mov ah,#0
	mov [bp-2], ax
	mov ax, [bp+6]
	mov cl, #18
	div cl
	and al,#0x1
	mov dx,#0
	mov dl, al
	mov [bp-4], dx
	add ah, #1
	mov dx, #0
	mov dl, ah
	mov [bp-6], dx
	mov ah, #0x3 	
	mov al, #0x1
	mov ch, [bp-2]
	mov cl, [bp-6]
	mov dh, [bp-4]
	mov dl, #0 		
	int #0x13
	add sp, #6
	pop bp
	ret
	
;void putInMemory (int segment, int address, char character)

_putInMemory:
	push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret
		
_clearScreen:
	mov ah, #6
	mov al, #0 ;(00 clear entire window)
	mov bh, #7
	mov cx, #0
	mov dh, #24
	mov dl, #79
	int #0x10
	call _setCursor
	ret
	
_setCursor:
	mov ah,#2	
	mov al,#0 
	mov bl,#0	
	mov bh,#0	; page number
	mov dx,#0	; dh for row (00 is top ) dl column(00 is left)	
	int #0x10
	ret
	
_moveToSegment:
	push bp       
	mov bp, sp
        push es
        push si
        push di
	mov es, [bp+4]	;segment
	mov di, [bp+6]	;destiny Address
	mov si, [bp+8]	;source Address
	mov cx, [bp+10]	;size
	rep
	movsb
        pop di
        pop si
        pop es
	pop bp
	ret

;int interrupt (int number, int AX, int BX, int CX, int DX)

_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,#intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:	int #0x00	;call the interrupt (00 will be changed above)
	mov ah,#0	;we only want AL returned
	pop bp
	ret

;void makeInterrupt21()
;this sets up the interrupt 0x21 vector
;when an interrupt 0x21 is called in the future, 
;_interrupt21ServiceRoutine will run

_makeInterrupt21:
	;get the address of the service routine
	mov dx,#_interrupt21ServiceRoutine
	push ds
	mov ax, #0	;interrupts are in lowest memory
	mov ds,ax
	mov si,#0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret
	
;this is called to start a program that is loaded into memory
;void launchProgram(int segment)
_launchProgram:
	mov bp,sp
	mov bx,[bp+2]	;get the segment into bx

	mov ax,cs	;modify the jmp below to jump to our segment
	mov ds,ax	;this is self-modifying code
	mov si,#jump
	mov [si+3],bx	;change the first 0000 to the segment

	mov ds,bx	;set up the segment registers
	mov ss,bx
	mov es,bx

	mov sp,#0xfff0	;set up the stack pointer
	mov bp,#0xfff0

jump:	jmp #0x0000:0x0000	;and start running (the first 0000 is changed above)


;void makeTimerInterrupt()
;sets up the timer's interrupt service routine
_makeTimerInterrupt:
        cli
        mov dx,#timer_ISR ;get address of timerISR in dx

        push ds
        mov ax,#0       ;interrupts are at lowest memory
        mov ds,ax
        mov si,#0x20    ;timer interrupt vector (8 * 4)
        mov ax,cs       ;have interrupt go to the current segment
        mov [si+2],ax
        mov [si],dx     ;address of our vector
        pop ds

        ;start the timer
        mov al,#0x36
        out #0x43,al
        mov ax,#0xFF
        out #0x40,al
        mov ax,#0xFF
        out #0x40,al

	sti
        ret

;this routine runs on timer interrupts
timer_ISR:

        ;disable interrupts
        cli

        ;save all regs for the old process on the old process's stack
        push bx
        push cx
        push dx
        push si
        push di
        push bp
        push ax
        push ds
        push es

        ;reset interrupt controller so it performs more interrupts
        mov al,#0x20
        out #0x20,al

        ;get the segment (ss) and the stack pointer (sp) - we need to keep these
        mov bx,ss
        mov cx,sp

        ;set all segments to the kernel
        mov ax,#0x1000
        mov ds,ax
        mov es,ax
        mov ss,ax
        ;set the kernel's stack
        mov ax,#0xdff0
        mov sp,ax
        mov bp,ax

        ;call handle interrupt with 2 parameters: the segment, the stack pointer.
	mov ax,#0
        push cx
        push bx
        call _handleTimerInterrupt

;void returnFromTimer(int segment, int sp)
;returns from a timer interrupt to a different process
_returnFromTimer:
        ;pop off the local return address - don't need it
        pop ax
        ;get the segment and stack pointer
        pop bx
        pop cx

        ;get rid of the junk from the two calls and no returns
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax

        ;set up the stack
        mov sp,cx
        ;set up the stack segment
        mov ss,bx

        ;now we're back to the program's area
        ;reload the registers (if this is it's first time running, these will be zeros)
        pop es
        pop ds
        pop ax
        pop bp
        pop di
        pop si
        pop dx
        pop cx
        pop bx

        ;enable interrupts and return
        sti
        iret

;void initializeProgram(int segment)
;this initializes a new program but doesn't start it running
;the scheduler will take care of that
;the program will be located at the beginning of the segment at [sp+2]
_initializeProgram:
        ;bx=new segment
	push	bp
        mov     bp,sp
        mov     bx,[bp+4]

;make a stack image so that the timer interrupt can start this program

        ;save the caller's stack pointer and segment
        mov     cx,sp
        mov     dx,ss
        mov     ax,#0xff18      ;this allows an initial sp of 0xff00
        mov     sp,ax
        mov     ss,bx

        mov     ax,#0   ;IP
        push    ax
        mov     ax,bx   ;CS
        push    ax
        mov     ax,#0x0         ;a normal flag setting
        push    ax
        mov     ax,#0           ;set all the general registers to 0
        push    ax      ;bx
        push    ax      ;cx
        push    ax      ;dx
        push    ax      ;si
        push    ax      ;di
        push    ax      ;bp
        push    ax      ;ax
        mov     ax,bx
        push    ax      ;ds
        push    ax      ;es

        ;restore the stack to the caller
        mov     sp,cx
        mov     ss,dx
	pop	bp
        ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx
	iret

;void setKernelDataSegment()
;sets the data segment to the kernel, saving the current ds on the stack
_setKernelDataSegment:
        pop bx
        push ds
        push bx
        mov ax,#0x1000
        mov ds,ax
        ret

;void restoreDataSegment()
;restores the data segment
_restoreDataSegment:
        pop bx
        pop ds
        push bx
        ret

;printhex is used for debugging only
;it prints out the contents of ax in hexadecimal
_printhex:
        push bx
        push ax
        push ax
        push ax
        push ax
        mov al,ah
        mov ah,#0xe
        mov bx,#7
        shr al,#4
        and al,#0xf
        cmp al,#0xa
        jb ph1
        add al,#0x7
ph1:    add al,#0x30
        int 0x10

        pop ax
        mov al,ah
        mov ah,#0xe
        and al,#0xf
        cmp al,#0xa
        jb ph2
        add al,#0x7
ph2:    add al,#0x30
        int 0x10

        pop ax
        mov ah,#0xe
        shr al,#4
        and al,#0xf
        cmp al,#0xa
        jb ph3
        add al,#0x7
ph3:    add al,#0x30
        int 0x10

        pop ax
        mov ah,#0xe
        and al,#0xf
        cmp al,#0xa
        jb ph4
        add al,#0x7
ph4:    add al,#0x30
        int 0x10

        pop ax
        pop bx
        ret


	
; Load a program from sector 11 into segment 0x20000
_loadProgram:
    mov ax, #0x2000
    mov ds, ax
    mov ss, ax
    mov es, ax ;let's have the stack start at 0x2000:fff0
    mov ax, #0xfff0
    mov sp, ax
    mov bp, ax ; Read the program from the floppy
    mov cl, #12 ;cl holds sector number
    mov dh, #0 ;dh holds head number - 0
    mov ch, #0 ;ch holds track number - 0
    mov ah, #2 ;absolute disk read
    mov al, #1 ;read 1 sector
    mov dl, #0  ;read from floppy disk A
    mov bx, #0   ;read into offset 0 (in the segment)
    int #0x13  ;call BIOS disk read function
    jmp #0x2000:#0  ; Switch to program

