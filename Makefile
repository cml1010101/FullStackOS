all: boot/bootx64.efi kernel/kernel.elf disk.img res/font.psf res/cursor.tga
	mcopy -i disk.img boot/bootx64.efi ::/EFI/BOOT/ -Do
	mcopy -i disk.img kernel/kernel.elf :: -Do
	mcopy -i disk.img res/font.psf ::/RES/ -Do
	mcopy -i disk.img res/cursor.tga ::/RES/ -Do
	sudo qemu-system-x86_64 -s -S -serial stdio -d cpu_reset -cpu qemu64 -drive \
		if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd -hda disk.img
	cd boot && make clean
	cd kernel && make clean
boot/bootx64.efi:
	cd boot && make bootx64.efi
kernel/kernel.elf:
	cd kernel && make kernel.elf
disk.img:
	dd if=/dev/zero of=disk.img bs=512 count=91669
	mformat -i disk.img -h 32 -t 32 -n 64 -c 1
	mmd -i disk.img ::/EFI
	mmd -i disk.img ::/EFI/BOOT
	mmd -i disk.img ::/RES
clean:
	cd boot && make clean
	cd kernel && make clean