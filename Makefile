.PHONY:clean build rebuild

build:

DIR_DEPS := deps
DIR_EXES := exes
DIR_OBJS := objs

DIRS := $(DIR_DEPS) $(DIR_EXES) $(DIR_OBJS)	

kernelSrc := kmain.c \
			screen.c

bootSrc := boot.asm
bootBin := boot.bin
bootBin := $(addprefix $(DIR_OBJS)/, $(bootBin))

loadSrc := load.asm
loadBin := load.bin
loadBin := $(addprefix $(DIR_OBJS)/, $(loadBin))

kentrySrc := kentry.asm
kentryObj := kentry.obj
kentryObj := $(addprefix $(DIR_OBJS)/, $(kentryObj))

kernelBin := kernel.bin
kernelBin := $(addprefix $(DIR_OBJS)/, $(kernelBin))

kernelElf := kernel.elf
kernelElf := $(addprefix $(DIR_OBJS)/, $(kernelElf))

kernelLds := kernel.lds

blfuncSrc := blfunc.asm
commonSrc := common.asm

dataImg := data.img

RM := rm -rf

MOUNT  := mount -o loop
UMOUNT := umount

MNTPATH := /mnt/rootb

CFLAGS := -m32 -fno-builtin -fno-stack-protector

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
	nasm $< -o $@
	dd if=$@ of=$(dataImg) bs=512 count=1 conv=notrunc

$(loadBin) : $(loadSrc) $(blfuncSrc) $(commonSrc)
	nasm $< -o $@
	sudo $(MOUNT) $(dataImg) $(MNTPATH)
	sudo cp $@ $(MNTPATH)/$(notdir $@)
	sudo $(UMOUNT) $(MNTPATH)

$(kentryObj) : $(kentrySrc)
	nasm -f elf $< -o $@

$(DIR_OBJS)/%.o : %.c
	gcc $(CFLAGS) -c $(filter %.c, $^) -o $@

$(kernelElf) : $(kentryObj) $(OBJS)
	ld -m elf_i386 -s -T$(kernelLds) $^ -o $@	

$(kernelBin) : $(kernelElf)
	objcopy -O binary -R .note -R .comment -S $< $@	
	sudo $(MOUNT) $(dataImg) $(MNTPATH)
	sudo cp $@ $(MNTPATH)/$(notdir $@)
	sudo $(UMOUNT) $(MNTPATH)

$(dataImg):
	bximage $@ -q -fd -size=1.44

$(DIRS):
	mkdir $@

ifeq ("$(wildcard $(DIR_DEPS))", "")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c	
endif
	@echo "Creating $@ ..."
	@set -e; \
	gcc -MM -E $(filter %.c, $^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g' > $@

clean:
	$(RM) $(rmFiles)
	@echo "clean Success!"

build:$(DIR_OBJS) $(dataImg) $(bootBin) $(loadBin) $(kernelBin)
	@echo "build Success!"

rebuild:
	@$(MAKE) clean
	@$(MAKE) build
	@echo "rebuild Success!"