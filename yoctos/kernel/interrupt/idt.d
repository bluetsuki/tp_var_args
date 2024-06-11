interrupt/idt.o: interrupt/idt.c ../common/types.h ../common/mem.h \
 ../common/types.h drivers/keyboard.h drivers/timer.h drivers/pic.h \
 drivers/term.h ../common/colors.h mem/gdt.h task/tss.h task/task.h \
 task/tss.h mem/paging.h descriptors.h interrupt/idt.h interrupt/irq.h \
 x86.h
