.PHONY:clean build rebuild

build:

bootSrc:=boot.asm
bootBin:=boot.bin
bootTxt:=boot.txt

loadSrc:=load.asm
loadBin:=load.bin
loadTxt:=load.txt

blfuncSrc:=blfunc.asm

commonSrc:=common.asm

dataImg:=data.img

RM:=rm -rf

MOUNT:=mount -o loop
UMOUNT:=umount

MNTPATH:=/mnt/rootb

$(bootTxt):$(bootBin)
	$(RM) $@
	ndisasm -o 0x7c00 $^ >> $@

$(loadTxt):$(loadBin)
	$(RM) $@
	ndisasm -b32 -o 0x9000 $^ >> $@

$(bootBin):$(bootSrc) $(blfuncSrc)
	nasm $< -o $@
	dd if=$@ of=$(dataImg) bs=512 count=1 conv=notrunc

$(loadBin):$(loadSrc) $(blfuncSrc) $(commonSrc)
	nasm $< -o $@
	sudo $(MOUNT) $(dataImg) $(MNTPATH)
	sudo cp $@ $(MNTPATH)/$@
	sudo $(UMOUNT) $(MNTPATH)

$(dataImg):
	bximage $@ -q -fd -size=1.44

clean:
	$(RM) $(dataImg) $(bootBin) $(bootTxt) $(loadBin) $(loadTxt)
	@echo "clean Success!"

build:$(dataImg) $(bootBin) $(bootTxt) $(loadBin) $(loadTxt)
	@echo "build Success!"

rebuild:
	@$(MAKE) clean
	@$(MAKE) build
	@echo "rebuild Success!"