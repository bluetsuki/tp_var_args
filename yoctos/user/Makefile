include ../common.mk

override LD_FLAGS+=-Tapp.ld $(BAREMETAL_FLAGS) -lgcc

USER_C_SRC=$(shell find . -iname "*.c")
USER_ASM_SRC=$(shell find . -iname "*.s")
USER_C_OBJ=$(USER_C_SRC:.c=.o)
USER_C_DEP=$(USER_C_SRC:.c=.d)
USER_ASM_OBJ=$(USER_ASM_SRC:.s=.o)

DEP=$(USER_C_OBJ:%.o=%.d)

APP_DEP=entrypoint_asm.o syscall_asm.o ulibc.o $(COMMON_OBJ)

APPS=hello.exe shell.exe gpf.exe pagefault.exe pix.exe

all: $(APPS)

# Since these obj files are not deps of anything (intermediary files), make deletes them.
# <HACK>Here we force them to be a dep of something</HACK>
# There is probably a better way of doing this...
UNUSED: $(USER_C_OBJ)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEFINES) -I.. $< -o $@

%.o: %.s
	nasm -f elf32 $< -o $@

%.exe: %.o $(APP_DEP)
	$(LD) $^ -o $@ $(LD_FLAGS)

clean:
	$(MAKE) -C $(COMMON_DIR) clean
	/bin/rm -f $(APPS) $(USER_C_OBJ) $(USER_C_DEP) $(USER_ASM_OBJ)

.PHONY: clean

-include $(DEP)
