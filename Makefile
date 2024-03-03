
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

GFILES := GPATH GRTAGS GTAGS

V := @

CFLAGS  := -m32 # 32 位的程序
CFLAGS  += -fno-builtin # 不需要 gcc 内置函数
CFLAGS  += -nostdinc    # 不需要标准头文件
CFLAGS  += -nostdlib    # 不需要标准库
CFLAGS  += -fno-pic     # 不需要位置无关的代码 position independent code
CFLAGS  += -fno-pie     # 不需要位置无关的可执行程序 position independent executable
CFLAGS  += -fno-stack-protector # 不需要栈保护

CFLAGS  := $(strip $(CFLAGS))

EFLAGS  := -E -MM
NMLAGS  := -f elf32
LDFLAGS := -m elf_i386 -static
DDFLAGS := bs=512 count=1 conv=notrunc
RMFLAGS := -rf

DEBUG := -g

MOUNTFLAGS := -o loop

OBJCOPYFLAGS := -O binary -R .note -R .comment -S
BXIMAGEFLAGS := -q -func=create -fd=1.44M

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
			clock.c \
			mutex.c \
			keyboard.c \
			event.c

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

rmFiles := $(dataImg) $(GFILES) $(DIRS)

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
	$(CC) $(CFLAGS) $(DEBUG) -c $(filter %.c, $^) -o $@

$(DIR_OBJS)/%.obj : %.asm $(commonSrc)	
	$(NASM) $(NMLAGS) $(DEBUG) $< -o $@

$(kernelElf) : $(ASM_OBJS) $(OBJS) $(kernelLds)
	$(LD) $(LDFLAGS) -T$(kernelLds) $(filter %.o %.obj, $^) -o $@	

$(kernelBin) : $(kernelElf)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@	
	$(SUDO) $(MOUNT) $(MOUNTFLAGS) $(dataImg) $(MNTPATH)
	$(SUDO) $(CORPY) $(RootDIR)/$@ $(MNTPATH)/$(notdir $@)
	$(SUDO) $(UMOUNT) $(MNTPATH)

$(dataImg):
	$(BXIMAGE) $@ $(BXIMAGEFLAGS)

$(DIRS):
	$(MKDIR) $@

$(GFILES) : $(kernelSrc)
	gtags	
	global -u

ifeq ("$(wildcard $(DIR_DEPS))", "")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c	
endif
	$(V)$(ECHO) "Creating $@ ..."
	$(V)set -e; \
	$(CC) $(EFLAGS) $(filter %.c, $^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g' > $@

.PHONY: build
build:$(DIR_OBJS) $(dataImg) $(bootBin) $(loadBin) $(kernelBin) $(GFILES)
	$(V)$(ECHO) "build Success!"

.PHONY: clean
clean:
	$(RM) $(RMFLAGS) $(rmFiles)
	$(V)$(ECHO) "clean Success!"

.PHONY: rebuild
rebuild:
	$(V)$(MAKE) clean
	$(V)$(MAKE) build
	$(V)$(ECHO) "rebuild Success!"

.PHONY: bochs-gui
bochs-gui:
	bochs -q -f ${RootDIR}/bochs/bochsrc.gui

.PHONY: bochs-gdb
bochs-gdb:
	bochs-gdb -q -f ${RootDIR}/bochs/bochsrc.gdb		
