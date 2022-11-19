.PHONY:clean build rebuild

build:

NASM    := nasm
LD      := ld
CC      := gcc
OBJCOPY := objcopy

RM := rm
DD := dd
MKDIR := mkdir
CORPY := cp

SUDO := sudo
ECHO := echo

BXIMAGE := bximage

MOUNT  := mount 
UMOUNT := umount

MNTPATH := /mnt

V := @

CFLAGS  := -I. -m32 -fno-builtin -fno-stack-protector -nostdlib
EFLAGS  := -E -MM
LDFLAGS := -m elf_i386 -s
DDFLAGS := bs=512 count=1 conv=notrunc
RMFLAGS := -rf

MOUNTFLAGS := -o loop

OBJCOPYFLAGS := -O binary -R .note -R .comment -S
BXIMAGEFLAGS := -q -fd -size=1.44

DIR_DEPS := deps
DIR_EXES := exes
DIR_OBJS := objs

DIRS := $(DIR_DEPS) $(DIR_EXES) $(DIR_OBJS)	

RootDIR := $(realpath ./)

kernelSrc := kmain.c \
			  task.c \
			screen.c \
			kernel.c \
			string.c \
			pic.c  \
			interrupt.c \
			ihandler.c \
			list.c \
			queue.c \
			stack.c \
			btree.c \
			assert.c \
			syscall.c \
			app.c \
			memory.c \
			vMemory.c \
			pMemory.c \
			bitmap.c \
			clock.c

kernelAsm := kentry.asm \
			   task.asm \
			 screen.asm \
			 ihandler.asm \
			 syscall.asm \
			 io.asm			

bootSrc := boot.asm
bootBin := boot.bin
bootBin := $(addprefix $(DIR_OBJS)/, $(bootBin))

loadSrc := load.asm
loadBin := load.bin
loadBin := $(addprefix $(DIR_OBJS)/, $(loadBin))

kernelBin := kernel.bin
kernelBin := $(addprefix $(DIR_OBJS)/, $(kernelBin))

kernelElf := kernel.elf
kernelElf := $(addprefix $(DIR_OBJS)/, $(kernelElf))

kernelLds := kernel.lds

blfuncSrc := blfunc.asm
commonSrc := common.asm

dataImg := data.img

ASM_OBJS := $(patsubst %.asm, %.obj, $(kernelAsm))
ASM_OBJS := $(addprefix $(DIR_OBJS)/, $(ASM_OBJS))

SRCS := $(kernelSrc)

OBJS := $(patsubst %.c, %.o, $(SRCS))
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))

DEPS := $(patsubst %.c, %.dep, $(SRCS))
DEPS := $(addprefix $(DIR_DEPS)/, $(DEPS))

rmFiles := $(dataImg) $(DIRS)

ifeq ("$(MAKECMDGOALS)", "rebuild")
-include $(DEPS)	
endif

ifeq ("$(MAKECMDGOALS)", "build")
-include $(DEPS)	
endif

ifeq ("$(MAKECMDGOALS)", "")
-include $(DEPS)	
endif

$(bootBin) : $(bootSrc) $(blfuncSrc) $(commonSrc)
	$(NASM) $< -o $@
	$(DD) if=$@ of=$(dataImg) $(DDFLAGS)

$(loadBin) : $(loadSrc) $(blfuncSrc) $(commonSrc)
	$(NASM) $< -o $@
	$(SUDO) $(MOUNT) $(MOUNTFLAGS) $(dataImg) $(MNTPATH)
	$(SUDO) $(CORPY) $(RootDIR)/$@ $(MNTPATH)/$(notdir $@)
	$(SUDO) $(UMOUNT) $(MNTPATH)

$(DIR_OBJS)/%.o : %.c
	$(CC) $(CFLAGS) -c $(filter %.c, $^) -o $@

$(DIR_OBJS)/%.obj : %.asm $(commonSrc)	
	$(NASM) -f elf $< -o $@

$(kernelElf) : $(ASM_OBJS) $(OBJS)
	$(LD) $(LDFLAGS) -T$(kernelLds) $^ -o $@	

$(kernelBin) : $(kernelElf)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@	
	$(SUDO) $(MOUNT) $(MOUNTFLAGS) $(dataImg) $(MNTPATH)
	$(SUDO) $(CORPY) $(RootDIR)/$@ $(MNTPATH)/$(notdir $@)
	$(SUDO) $(UMOUNT) $(MNTPATH)

$(dataImg):
	$(BXIMAGE) $@ $(BXIMAGEFLAGS)

$(DIRS):
	$(MKDIR) $@

ifeq ("$(wildcard $(DIR_DEPS))", "")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c	
endif
	$(V)$(ECHO) "Creating $@ ..."
	$(V)set -e; \
	$(CC) $(EFLAGS) $(filter %.c, $^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g' > $@

clean:
	$(RM) $(RMFLAGS) $(rmFiles)
	$(V)$(ECHO) "clean Success!"

build:$(DIR_OBJS) $(dataImg) $(bootBin) $(loadBin) $(kernelBin)
	$(V)$(ECHO) "build Success!"

rebuild:
	$(V)$(MAKE) clean
	$(V)$(MAKE) build
	$(V)$(ECHO) "rebuild Success!"
