include ../common.mk
 
SRC=$(shell find . -iname "*.c")
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: $(OBJ)

%.o: %.c
	$(CC) $(CC_FLAGS) $< -o $@

%.o: %.s
	nasm -f elf32 $< -o $@

clean:
	/bin/rm -f $(OBJ) $(DEP)

.PHONY: clean

-include $(DEP)
