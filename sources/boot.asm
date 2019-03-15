[org 0x8000]
some:

lap:
		jmp short bsp
		xor ax,ax
		mov ss,ax
		mov sp, 0x7c00
		xor ax,ax
		mov ds,ax
        ; Mark CPU as active
        lock
		inc byte [ds:0x2555]
		jmp zop
	bsp:
	xor ax,ax
	mov ds,ax
	mov dword[ds:0x2550],0
	mov dword[ds:0x2555],0
	mov word [ds:0x8000], 0x9090
	cli
    mov ax, 1D0h
    mov ss, ax
    mov sp, 0200h
	zop:
    in al, 0x92;включаем A20
	or al, 2
	out 0x92, al
	
    sti

lMoveGDT:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov si, GDT
    mov di, [GDTbase]
    mov cx, [GDTsize]
    cld
    rep movsb

lEnterPMode:
    cli
    mov eax, cr0
    or al, 1
    mov cr0, eax

    lgdt[GDTR]

    jmp CODESELECTOR:FlushPipeline  

[BITS 32]
FlushPipeline:	
    mov eax, DATASELECTOR
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
	mov eax,dword[ds:0x2555]
	cmp eax,0
	jnz bsp_ok
    mov esp, 0ffffh
	mov ecx,0x80000
	mov edi,0x100000
	mov esi,kernel
	rep movsd
	
	mov eax,cr4
	or eax,0x200
	mov cr4,eax
	mov edi,0xb8000
	movaps 	[edi],xmm0
	jmp 0x100000
bsp_ok:
	mov esp, 0x10000
	lock
	xadd [0x9925], esp
	add esp,0x10000
	mov eax,cr4
	or eax,0x200
	mov cr4,eax
	mov edi,0xb8000
	movaps 	[edi],xmm0

	mov eax,esp
	sub eax,dword[0x9929]
	shr eax,16
	push eax
	push eax
	mov edi,dword[0x9933]

	jmp edi
;times 1024-($-$$) db 0

GDT:
NULLSELECTOR    EQU             $ - GDT
                DD 0x0
                DD 0x0

CODESELECTOR    EQU             $ - GDT          ; 4GB Flat Code at 0x0 with max 0xFFFFF limit
                DW              0FFFFh           ; Limit(2):0xFFFF
                DW              0h               ; Base(3)
                DB              0h               ; Base(2)
                DB              09Ah             ; Type: present,ring0,code,exec/read/accessed (10011000)
                DB              0CFh             ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
                DB              0h               ; Base(1)

DATASELECTOR    EQU             $ - GDT          ; 4GB Flat Data at 0x0 with max 0xFFFFF limit
                DW              0FFFFh           ; Limit(2):0xFFFF
                DW              0h               ; Base(3)
                DB              0h               ; Base(2)
                DB              092h             ; Type: present,ring0,data/stack,read/write (10010010)
                DB              0CFh             ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
                DB              0h               ; Base(1)
				
DATASELECTORR    EQU             $ - GDT          ; 4GB Flat Data at 0x0 with max 0xFFFFF limit
                DW              0FFFFh           ; Limit(2):0xFFFF
                DW              0h               ; Base(3)
                DB              0h               ; Base(2)
                DB              0b10011000            ; Type: present,ring0,data/stack,read/write (10010010)
                DB              0b10001111             ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
                DB              0h               ; Base(1)
DATASELECTOR2    EQU             $ - GDT          ; 4GB Flat Data at 0x0 with max 0xFFFFF limit
                DW              0FFFFh           ; Limit(2):0xFFFF
                DW              0h               ; Base(3)
                DB              0h               ; Base(2)
                DB              0b10010010            ; Type: present,ring0,data/stack,read/write (10010010)
                DB              0b10001111             ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
                DB              0h               ; Base(1)
GDTEND:

GDTR:
GDTsize DW GDTEND - GDT - 1
GDTbase DD GDT
times 1024-($-$$) db 0
[BITS 32]
	cli
	;EBX - MODE
	;EAX - RETURN PTR
	cli
	mov [mode], bx
	mov [return], eax
	mov [stack], esp
	mov edx, real
	jmp DATASELECTORR:WF
[BITS 16]
WF:
	mov ax, DATASELECTOR2        ;0x20 is 16-bit protected mode selector.
    mov ss, ax  
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov sp, 0x7c00+0x200    ;Stack hase base at 0x7c00+0x200    


    mov eax, cr0
    and eax, 0xfffffffe ;Clear protected enable bit in cr0

    mov cr0, eax    

    jmp 0x00:realMode   ;Load CS and IP


realMode:
;Load segment registers with 16-bit Values.
    mov ax, 0x00
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov ax, 0
    mov ss, ax
    mov ax, 0
    mov es, ax
    mov sp, 0x7c00+0x200    

    cli
    lidt[.idtR]     ;Load real mode interrupt vector table
    sti

    push word 0x00       ;New CS
    push dx         ;New IP (saved in edx)
    retf            ;Load CS, IP and Start real mode
;Real mode interrupt vector table
.idtR:  dw 0xffff       ;Limit
    dd 0            ;Base
real:
	mov ax,0x4f02
	mov dx, [mode]
	int 10h
	mov [0x8c00],ax
	cli
	
	mov eax, cr0
	or al,1
	mov cr0, eax
	jmp 0x8:return_kernel
[BITS 32]
return_kernel:
	 mov eax, DATASELECTOR
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
	mov esp, [stack]
	mov eax, [return]
	mov ebx,0x09800
	;lidt [ebx]
	;sti
	;hlt
	jmp eax
	jmp $;
mode dw 0
return dd 0
stack dd 0
jmp $
times 2048-($-$$) db 0
kernel incbin 'binaries\kernel.bin'