SYSTEM?=UEFI
PLATFORM?=QEMU

QEMU=qemu-system-i386 -enable-kvm -m 512 -monitor stdio -vga virtio

ISO_NAME=yoctos.iso

GRUB_CONF=grub/grub.cfg

SYSTEM_OK=0
ifeq ($(SYSTEM),BIOS)
GRUB_MKRESCUE_ARGS=
SYSTEM_OK=1
endif
ifeq ($(SYSTEM),UEFI)
GRUB_MKRESCUE_ARGS=/usr/lib/grub/i386-pc
SYSTEM_OK=1
endif
ifneq ($(SYSTEM_OK),1)
$(error invalid SYSTEM)
endif

PLATFORM_OK=0
ifeq ($(PLATFORM),QEMU)
CC_DEFINES=-DQEMU
PLATFORM_OK=1
endif
ifeq ($(PLATFORM),PC)
CC_DEFINES=-DEMULATE_DISK_IN_RAM
PLATFORM_OK=1
endif
ifneq ($(PLATFORM_OK),1)
$(error invalid PLATFORM)
endif

ifeq ($(DEBUG),0)
CC_FLAGS=-O3
LD_FLAGS=
else
CC_FLAGS=-g -O0
LD_FLAGS=-g
endif

help:
	@echo "Available targets:"
	@echo "run      build the OS ISO image (+ filsystem) and run it in QEMU"
	@echo "iso      build the OS ISO image (+ filesystem)"
	@echo "common   build the common object files only"
	@echo "kernel   build the kernel only"
	@echo "user     build the user space executables only"
	@echo "debug    build the OS ISO image (+ filsystem) and run it in QEMU for debugging"
	@echo "deploy   build the OS ISO image (+ filsystem) and deploy it to the specified device"
	@echo "         Requires DEV to be defined (eg. DEV=/dev/sdb)"
	@echo "clean    clean up everything"
	@echo ""
	@echo "Available variables:"
	@echo "SYSTEM   target system type, either UEFI or BIOS (default: UEFI)"
	@echo "PLATFORM target platform type, either QEMU or PC (default: QEMU)"
	@echo "DEBUG    whether to generate debug code, either on or off (default: on)"
	@echo "DEV      device to deploy the ISO image onto (only used by the \"deploy\" target)"
	@echo ""
	@echo "Usage examples:"
	@echo "make run"
	@echo "make run SYSTEM=BIOS PLATFORM=QEMU"
	@echo "make PLATFORM=PC DEBUG=0 DEV=/dev/sdb deploy"

iso: $(ISO_NAME)

msg:
	@echo "================================================="
	@echo "Build targeting a $(PLATFORM)/$(SYSTEM) platform" 
	@echo "================================================="

run: $(ISO_NAME)
	$(QEMU) -cdrom $<

# The "accel=tcg" option is necessary to be able to debug an ELF within QEMU
debug: $(ISO_NAME)
	$(QEMU) -s -S -cdrom $< -machine accel=tcg

# ISO image creation taken from http://wiki.osdev.org/Bare_Bones#Booting_the_Kernel
# Requires grub-mkrescue and xorriso
# NOTE: on hosts that boot via UEFI, the path /usr/lib/grub/i386-pc is required
# by grub-mkrescue otherwise the ISO won't be bootable by qemu.
$(ISO_NAME): msg $(GRUB_CONF) kernel user
	mkdir -p build/boot/grub build/bin
	cp $(GRUB_CONF) build/boot/grub/
	cp kernel/kernel.elf build/boot/
	cp user/*.exe build/bin/
	for f in user/*.exe; do \
		name=`basename $$f`; \
		echo "\tmodule /bin/$$name $$name" >> build/boot/grub/grub.cfg; \
	done
	echo "}" >> build/boot/grub/grub.cfg
	grub-mkrescue $(GRUB_MKRESCUE_ARGS) -o $@ build
	@echo "Built the $@ image for a $(SYSTEM) system."

common:
	$(MAKE) -C $@ CC_DEFINES=$(CC_DEFINES) CC_FLAGS="$(CC_FLAGS)" LD_FLAGS="$(LD_FLAGS)"

kernel:
	$(MAKE) -C $@ CC_DEFINES=$(CC_DEFINES) CC_FLAGS="$(CC_FLAGS)" LD_FLAGS="$(LD_FLAGS)"

user:
	$(MAKE) -C $@ CC_DEFINES=$(CC_DEFINES) CC_FLAGS="$(CC_FLAGS)" LD_FLAGS="$(LD_FLAGS)"

deploy: $(ISO_NAME)
	 sudo dd if=/dev/urandom of=$(DEV) bs=1M count=10
	 sudo dd if=$< of=$(DEV)
	 sudo sync

clean:
	/bin/rm -rf build $(ISO_NAME)
	$(MAKE) -C common clean
	$(MAKE) -C kernel clean
	$(MAKE) -C user clean

.PHONY: clean common kernel user
