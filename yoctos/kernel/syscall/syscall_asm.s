%include "const.inc"

section .text                      ; start of the text (code) section
align 4                            ; the code must be 4 byte aligned

extern syscall_handler

global _syscall_handler

_syscall_handler:
    ; Save segments registers as they are modified to point to the kernel's data
    push    ds
    push    es
    push    fs
    push    gs
    
    ; Load kernel data descriptor into all segments
    push    eax
    mov     ax,GDT_KERNEL_DATA_SELECTOR
    mov     ds,ax
    mov     es,ax
    mov     fs,ax
    mov     gs,ax
    pop     eax

    ; Pass the 5 arguments (nb, arg1, ..., arg4) to the syscall_handler
    ; They are in reverse order to match the cdecl IA-32 ABI.
    push    esi
    push    edx
    push    ecx
    push    ebx
    push    eax

    call    syscall_handler

    ; These 5 "pop eax" instructions are only here to balance the pushes
    ; above used to pass the arguments to the syscall_handler function
    pop     ebx
    pop     ebx
    pop     ebx
    pop     ebx
    pop     ebx

    ; Restore segment registers
    pop     gs
    pop     fs
    pop     es
    pop     ds
    iret
