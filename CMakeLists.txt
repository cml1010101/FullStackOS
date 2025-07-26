cmake_minimum_required(VERSION 3.10)
project(FullStackOS VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(IS_DEBUG "Build in debug mode" ON)
if(IS_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    # Define the DEBUG macro for conditional compilation
    add_definitions(-D__DEBUG__)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()
add_subdirectory(kernel)
add_subdirectory(boot)
add_custom_command(
    OUTPUT disk.img
    COMMAND qemu-img create -f raw disk.img 5G
    COMMAND parted disk.img -s -a minimal mklabel gpt
	COMMAND parted disk.img -s -a minimal mkpart EFI FAT16 2048s 93716s
	COMMAND parted disk.img -s -a minimal toggle 1 boot
    COMMENT "Creating 5GB disk image"
)
add_custom_target(create_disk_image ALL DEPENDS disk.img)
add_custom_target(
    mount
    COMMAND dd if=disk.img of=/tmp/part.img bs=512 count=91669 skip=2048 conv=notrunc
    COMMAND mformat -i /tmp/part.img -h 32 -t 32 -n 64 -c 1
    COMMAND mmd -i /tmp/part.img ::/EFI
    COMMAND mmd -i /tmp/part.img ::/EFI/BOOT
    COMMAND mcopy -i /tmp/part.img ${CMAKE_BINARY_DIR}/boot/boot.efi ::/EFI/BOOT/BOOTX64.efi
    COMMAND mcopy -i /tmp/part.img ${CMAKE_BINARY_DIR}/kernel/kernel.elf ::KERNEL.ELF
    COMMAND dd if=/tmp/part.img of=disk.img bs=512 count=91669 seek=2048 conv=notrunc
    DEPENDS boot kernel.elf
)
add_custom_target(
    run
    COMMAND  qemu-system-x86_64 -s -serial stdio -d cpu_reset -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/ovmf/x64/OVMF_CODE.4m.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/ovmf/x64/OVMF_VARS.4m.fd -hda disk.img -netdev user,id=net0,hostfwd=tcp:127.0.0.1:8080-:8080 -device rtl8139,netdev=net0 -object filter-dump,id=fd0,file=traffic.log,netdev=net0
    DEPENDS mount
)
add_custom_target(
    debug
    COMMAND qemu-system-x86_64 -s -S -serial stdio -d cpu_reset -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/ovmf/x64/OVMF_CODE.4m.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/ovmf/x64/OVMF_VARS.4m.fd -hda disk.img -netdev user,id=net0,hostfwd=tcp:127.0.0.1:8080-:8080 -device rtl8139,netdev=net0 -object filter-dump,id=fd0,file=traffic.log,netdev=net0
    DEPENDS mount
)