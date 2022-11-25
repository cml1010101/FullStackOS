[bits 64]
extern isr_handler
extern irq_handler
global check_cpuid
check_cpuid:
    pushfq
    pushfq
    xor qword [rsp], 0x200000
    popfq
    pushfq
    pop rax
    xor rax, [rsp]
    popfq
    and rax, 0x200000
    ret
global load_gdt
load_gdt:
    mov rax, [rsp + 8]
    push rbp
    mov rbx, rdi
    lgdt [rbx]
    mov rbx, rsp
    push rax
    push rbx
    pushfq
    push rsi
    push qword .l
    iretq
.l:
    mov rbp, rsp
    mov gs, r9w
    mov es, r8w
    mov fs, cx
    mov ds, dx
    mov rsp, rbp
    pop rbp
    ret
%macro ISR 1
global isr%1
isr%1:
    cli
    push qword 0
    push qword %1
    jmp isr_common
%endmacro
%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    push qword %1
    jmp isr_common
%endmacro
%macro IRQ 1
global irq%1
irq%1:
    cli
    push qword 0
    push qword %1
    jmp irq_common
%endmacro
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_ERR 8
ISR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
isr_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    xor rax, rax
    mov ax, ds
    push rax
    mov ax, es
    push rax
    mov ax, fs
    push rax
    mov ax, gs
    push rax
    mov rdi, rsp
    call isr_handler
    pop rax
    mov gs, ax
    pop rax
    mov fs, ax
    pop rax
    mov es, ax
    pop rax
    mov ds, ax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 16
    iretq
irq_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    xor rax, rax
    mov ax, ds
    push rax
    mov ax, es
    push rax
    mov ax, fs
    push rax
    mov ax, gs
    push rax
    mov rdi, rsp
    call irq_handler
    pop rax
    mov gs, ax
    pop rax
    mov fs, ax
    pop rax
    mov es, ax
    pop rax
    mov ds, ax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 16
    iretq
global load_idt
load_idt:
    mov rbx, rdi
    lidt [rbx]
    ret