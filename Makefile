.PHONY:clean build rebuild

build:

bootSrc := boot.asm
bootBin := boot.bin

loadSrc := load.asm
loadBin := load.bin

kmainSrc  := kmain.c
kmainObj  := kmain.o

kentrySrc := kentry.asm
kentryObj := kentry.obj

kernelBin := kernel.bin
kernelElf := kernel.elf

kernelLds := kernel.lds

blfuncSrc := blfunc.asm
commonSrc := common.asm

dataImg := data.img

RM := rm -rf

MOUNT  := mount -o loop
UMOUNT := umount

MNTPATH := /mnt/rootb

CFLAGS := -m32 -fno-builtin -fno-stack-protector

rmFiles := $(dataImg) $(bootBin) $(loadBin) $(kernelBin) $(kernelElf) $(kentryObj) $(kmainObj)

$(bootBin):$(bootSrc) $(blfuncSrc)
	nasm $< -o $@
	dd if=$@ of=$(dataImg) bs=512 count=1 conv=notrunc

$(loadBin):$(loadSrc) $(blfuncSrc) $(commonSrc)
	nasm $< -o $@
	sudo $(MOUNT) $(dataImg) $(MNTPATH)
	sudo cp $@ $(MNTPATH)/$@
	sudo $(UMOUNT) $(MNTPATH)

$(kentryObj):$(kentrySrc)
	nasm -f elf $^ -o $@

$(kmainObj):$(kmainSrc)
	gcc $(CFLAGS) -c $^ -o $@

$(kernelElf):$(kentryObj) $(kmainObj)
	ld -m elf_i386 -s -T$(kernelLds) $^ -o $@	

$(kernelBin):$(kernelElf)
	objcopy -O binary -R .note -R .comment -S $< $@	
	sudo $(MOUNT) $(dataImg) $(MNTPATH)
	sudo cp $@ $(MNTPATH)/$@
	sudo $(UMOUNT) $(MNTPATH)

$(dataImg):
	bximage $@ -q -fd -size=1.44

clean:
	$(RM) $(rmFiles)
	@echo "clean Success!"

build:$(dataImg) $(bootBin) $(loadBin) $(kernelBin)
	@echo "build Success!"

rebuild:
	@$(MAKE) clean
	@$(MAKE) build
	@echo "rebuild Success!"