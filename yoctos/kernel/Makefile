include ../common.mk

override LD_FLAGS+=-Tkernel.ld $(BAREMETAL_FLAGS) -lgcc

KERNEL_C_SRC=$(shell find . -iname "*.c")
KERNEL_ASM_SRC=$(shell find . -iname "*.s")
KERNEL_C_OBJ=$(KERNEL_C_SRC:.c=.o)
KERNEL_C_DEP=$(KERNEL_C_SRC:.c=.d)
KERNEL_ASM_OBJ=$(KERNEL_ASM_SRC:.s=.o)

KERNEL_PRECOMPILED_OBJ=$(shell find . -iname "*.o")

KERNEL_OUT=kernel.elf

DEP=$(KERNEL_C_OBJ:%.o=%.d)

all: $(KERNEL_OUT)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEFINES) -I. -I.. $< -o $@

%.o: %.s
	nasm -f elf32 $< -o $@

$(KERNEL_OUT): $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) $(COMMON_OBJ) $(KERNEL_PRECOMPILED_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

clean:
	$(MAKE) -C $(COMMON_DIR) clean
	/bin/rm -f $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) $(KERNEL_C_DEP)
	/bin/rm -f $(KERNEL_OUT)

.PHONY: clean

-include $(DEP)
