ENTRY(kernel_main)
SECTIONS
{
    . = 0x10000;
    _kernel_start = .;
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
    }
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
    .dynsym BLOCK(4K) : ALIGN(4K)
    {
        *(.dynsym)
    }
    .dynamic BLOCK(4K) : ALIGN(4K)
    {
        *(.dynamic)
    }
    .rel BLOCK(4K) : ALIGN(4K)
    {
        *(.rel)
    }
    .rela BLOCK(4K) : ALIGN(4K)
    {
        *(.rela)
    }
    .reloc BLOCK(4K) : ALIGN(4K)
    {
        *(.reloc)
    }
    .sdata BLOCK(4K) : ALIGN(4K)
    {
        *(.sdata)
    }
    _kernel_end = .;
}