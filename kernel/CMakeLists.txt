enable_language(ASM)
find_path(GNUEFI_INCLUDE_DIR NAMES "efi.h" "efilib.h" PATH_SUFFIXES "efi" REQUIRED)
add_executable(kernel.elf
    "src/ARP.cpp"
    "src/DHCP.cpp"
    "src/E1000.cpp"
    "src/Ethernet.cpp"
    "src/FAT16.cpp"
    "src/GDT.cpp"
    "src/Graphics.cpp"
    "src/Heap.cpp"
    "src/HTTP.cpp"
    "src/IDE.cpp"
    "src/IDT.cpp"
    "src/IP.cpp"
    "src/IRQ.cpp"
    "src/JSON.cpp"
    "src/kernel.cpp"
    "src/Keyboard.cpp"
    "src/MMU.cpp"
    "src/Module.cpp"
    "src/Partitions.cpp"
    "src/PCI.cpp"
    "src/PIT.cpp"
    "src/RTL8139.cpp"
    "src/Scheduler.cpp"
    "src/Shell.cpp"
    "src/SmartOS.cpp"
    "src/TCP.cpp"
    "src/Terminal.cpp"
    "src/UDP.cpp"
    "src/x86-64.S"
)
target_compile_options(kernel.elf PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-pic> $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-mno-red-zone> $<$<COMPILE_LANGUAGE:CXX>:-mno-mmx> $<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector> $<$<COMPILE_LANGUAGE:CXX>:-fshort-wchar> $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall> $<$<COMPILE_LANGUAGE:CXX>:-Wextra> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> $<$<COMPILE_LANGUAGE:CXX>:-Werror>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-pie> $<$<COMPILE_LANGUAGE:CXX>:-no-pie> $<$<COMPILE_LANGUAGE:ASM_NASM>:-felf64>)
if(IS_QEMU_BUILD)
    target_compile_definitions(kernel.elf PRIVATE __DEBUG__)
endif()
target_link_options(kernel.elf PRIVATE -T${CMAKE_SOURCE_DIR}/kernel/linker.ld -nostdlib -ffreestanding -fno-pie -no-pie)
target_include_directories(kernel.elf PUBLIC include ${GNUEFI_INCLUDE_DIR})