%include "const.inc"

section .text                      ; start of the text (code) section
align 4                            ; the code must be 4 byte aligned

global idt_load

; Load the IDT
idt_load:
    mov     eax,[esp+4]  ; Get the pointer to the IDT, passed as a parameter. 
    lidt    [eax]        ; Load the IDT pointer.
    ret

%macro exception_with_code 1
global _exception%1
_exception%1:
    cli
    push    %1
    jmp     exception_wrapper
%endmacro

%macro exception_wout_code 1
global _exception%1
_exception%1:
    cli
    push    0  ; dummy error code
    push    %1
    jmp     exception_wrapper
%endmacro

%macro irq 1
global _irq%1
_irq%1:
    cli
    push    0  ; dummy error code
    push    %1
    jmp     irq_wrapper
%endmacro

%macro isr_wrapper 2
extern %2
%1:
    ; Save all registers
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp
    push    ds
    push    es
    push    fs
    push    gs

    ; Load kernel data descriptor into all segments
    mov     ax,GDT_KERNEL_DATA_SELECTOR
    mov     ds,ax
    mov     es,ax
    mov     fs,ax
    mov     gs,ax
    
    ; Pass the stack pointer (which gives the CPU context) to the C function
    mov     eax,esp
    push    eax    
    call    %2
    pop     eax  ; only here to balance the "push eax" done before the call

    ; Restore all registers
    pop     gs
    pop     fs
    pop     es
    pop     ds
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    
	; Fix the stack pointer due to the 2 push done before the call
	; to exception_wrapper/irq_wrapper : error code and exception/irq number
    add     esp,8
    iret
%endmacro

; CPU exception
exception_wout_code   0
exception_wout_code   1
exception_wout_code   2
exception_wout_code   3
exception_wout_code   4
exception_wout_code   5
exception_wout_code   6
exception_wout_code   7
exception_with_code   8
exception_wout_code   9
exception_with_code   10
exception_with_code   11
exception_with_code   12
exception_with_code   13
exception_with_code   14
exception_wout_code   15
exception_wout_code   16
exception_with_code   17
exception_wout_code   18
exception_wout_code   19
exception_wout_code   20

; The 16 hardware interrupts defined at once using a macro repetition
; irq     0
; ...
; irq     15
%assign i 0 
%rep 16
    irq i
%assign i i+1 
%endrep

; exception_handler and irq_handler are implemented in idt.c
isr_wrapper exception_wrapper,exception_handler
isr_wrapper irq_wrapper,irq_handler
