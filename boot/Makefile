bootx64.efi: bootx64.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 bootx64.so bootx64.efi
bootx64.so: boot.o
	ld boot.o /usr/lib/crt0-efi-x86_64.o -nostdlib -znocombreloc -T/usr/lib/elf_x86_64_efi.lds \
    	-shared -Bsymbolic -L/usr/lib -l:libgnuefi.a -l:libefi.a -o bootx64.so
boot.o: boot.c
	gcc boot.c -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I/usr/include/efi \
    	-I/usr/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER -o boot.o
clean:
	rm -rf boot.o bootx64.so bootx64.efi