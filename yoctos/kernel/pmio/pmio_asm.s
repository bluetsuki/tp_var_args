global outb
global inb
global outw
global inw

section .text    ; start of the text (code) section
align 4          ; the code must be 4 byte aligned

; void outb(uint16 port, uint8 data)
outb:
    mov     dx,word [esp+4]    ; port (2 bytes)
    mov     al,byte [esp+8]    ; data (1 byte)
    out     dx,al
    ret

; uint8 inb(uint16 port)
inb:
    mov     dx,word [esp+4]    ; port (2 bytes)
    in      al,dx
    ret

; void outw(uint16 port, uint16 data)
outw:
    mov     dx,word [esp+4]    ; port (2 bytes)
    mov     ax,word [esp+8]    ; data (2 bytes)
    out     dx,ax
    ret

; uint16 inw(uint16 port)
inw:
    mov     dx,word [esp+4]    ; port (2 bytes)
    in      ax,dx
    ret
