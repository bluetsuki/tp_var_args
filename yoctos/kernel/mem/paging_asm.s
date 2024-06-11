%include "const.inc"

global paging_enable
global paging_load_pagedir
global paging_get_current_pagedir

section .text:           ; start of the text (code) section
align 4                  ; the code must be 4 byte aligned

; Enable paging.
; Paging is enabled by setting bit 31 (PG) of the CR0 register (CR0 = CR0 | 0x80000000)
; void paging_enable(void)
paging_enable:
	mov    eax,cr0
	or     eax,0x80000000
	mov    cr0,eax
	ret

; load the CR3 register with the specified page directory
; void paging_load_pagedir(uint32_t dir_entry_addr)
paging_load_pagedir:
	mov    eax,[esp+4]
	mov    cr3,eax
	ret

; pagetable_t *paging_get_current_pagedir();
paging_get_current_pagedir:
	mov		eax,cr3
	ret