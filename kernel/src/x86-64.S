.intel_syntax noprefix

.extern isr_handler
.extern irq_handler

.global check_cpuid
check_cpuid:
    pushfq
    pushfq
    xor QWORD PTR [rsp], 0x200000
    popfq
    pushfq
    pop rax
    xor rax, QWORD PTR [rsp]
    popfq
    and rax, 0x200000
    ret

.global load_gdt
load_gdt:
    mov rax, QWORD PTR [rsp + 8]
    push rbp
    mov rbx, rdi
    lgdt [rbx]
    mov rbx, rsp
    push rax
    push rbx
    pushfq
    push rsi
    push QWORD PTR .L_load_gdt_ret
    iretq
.L_load_gdt_ret:
    mov rbp, rsp
    mov gs, r9w
    mov es, r8w
    mov fs, cx
    mov ds, dx
    mov rsp, rbp
    pop rbp
    ret

.global read_gdt
read_gdt:
    sgdt [rdi]
    ret

.global read_idt
read_idt:
    sidt [rdi]
    ret

/* Macros are typically replaced by direct code generation in GAS */

.global isr0
isr0:
    cli
    push QWORD PTR 0
    push QWORD PTR 0
    jmp isr_common
.global isr1
isr1:
    cli
    push QWORD PTR 0
    push QWORD PTR 1
    jmp isr_common
.global isr2
isr2:
    cli
    push QWORD PTR 0
    push QWORD PTR 2
    jmp isr_common
.global isr3
isr3:
    cli
    push QWORD PTR 0
    push QWORD PTR 3
    jmp isr_common
.global isr4
isr4:
    cli
    push QWORD PTR 0
    push QWORD PTR 4
    jmp isr_common
.global isr5
isr5:
    cli
    push QWORD PTR 0
    push QWORD PTR 5
    jmp isr_common
.global isr6
isr6:
    cli
    push QWORD PTR 0
    push QWORD PTR 6
    jmp isr_common
.global isr7
isr7:
    cli
    push QWORD PTR 0
    push QWORD PTR 7
    jmp isr_common
.global isr8
isr8:
    cli
    push QWORD PTR 8
    jmp isr_common
.global isr9
isr9:
    cli
    push QWORD PTR 0
    push QWORD PTR 9
    jmp isr_common
.global isr10
isr10:
    cli
    push QWORD PTR 10
    jmp isr_common
.global isr11
isr11:
    cli
    push QWORD PTR 11
    jmp isr_common
.global isr12
isr12:
    cli
    push QWORD PTR 12
    jmp isr_common
.global isr13
isr13:
    cli
    push QWORD PTR 13
    jmp isr_common
.global isr14
isr14:
    cli
    push QWORD PTR 14
    jmp isr_common
.global isr15
isr15:
    cli
    push QWORD PTR 0
    push QWORD PTR 15
    jmp isr_common
.global isr16
isr16:
    cli
    push QWORD PTR 0
    push QWORD PTR 16
    jmp isr_common
.global isr17
isr17:
    cli
    push QWORD PTR 0
    push QWORD PTR 17
    jmp isr_common
.global isr18
isr18:
    cli
    push QWORD PTR 0
    push QWORD PTR 18
    jmp isr_common
.global isr19
isr19:
    cli
    push QWORD PTR 0
    push QWORD PTR 19
    jmp isr_common
.global isr20
isr20:
    cli
    push QWORD PTR 0
    push QWORD PTR 20
    jmp isr_common
.global isr21
isr21:
    cli
    push QWORD PTR 0
    push QWORD PTR 21
    jmp isr_common
.global isr22
isr22:
    cli
    push QWORD PTR 0
    push QWORD PTR 22
    jmp isr_common
.global isr23
isr23:
    cli
    push QWORD PTR 0
    push QWORD PTR 23
    jmp isr_common
.global isr24
isr24:
    cli
    push QWORD PTR 0
    push QWORD PTR 24
    jmp isr_common
.global isr25
isr25:
    cli
    push QWORD PTR 0
    push QWORD PTR 25
    jmp isr_common
.global isr26
isr26:
    cli
    push QWORD PTR 0
    push QWORD PTR 26
    jmp isr_common
.global isr27
isr27:
    cli
    push QWORD PTR 0
    push QWORD PTR 27
    jmp isr_common
.global isr28
isr28:
    cli
    push QWORD PTR 0
    push QWORD PTR 28
    jmp isr_common
.global isr29
isr29:
    cli
    push QWORD PTR 0
    push QWORD PTR 29
    jmp isr_common
.global isr30
isr30:
    cli
    push QWORD PTR 0
    push QWORD PTR 30
    jmp isr_common
.global isr31
isr31:
    cli
    push QWORD PTR 0
    push QWORD PTR 31
    jmp isr_common

.global irq0
irq0:
    cli
    push QWORD PTR 0
    push QWORD PTR 0
    jmp irq_common
.global irq1
irq1:
    cli
    push QWORD PTR 0
    push QWORD PTR 1
    jmp irq_common
.global irq2
irq2:
    cli
    push QWORD PTR 0
    push QWORD PTR 2
    jmp irq_common
.global irq3
irq3:
    cli
    push QWORD PTR 0
    push QWORD PTR 3
    jmp irq_common
.global irq4
irq4:
    cli
    push QWORD PTR 0
    push QWORD PTR 4
    jmp irq_common
.global irq5
irq5:
    cli
    push QWORD PTR 0
    push QWORD PTR 5
    jmp irq_common
.global irq6
irq6:
    cli
    push QWORD PTR 0
    push QWORD PTR 6
    jmp irq_common
.global irq7
irq7:
    cli
    push QWORD PTR 0
    push QWORD PTR 7
    jmp irq_common
.global irq8
irq8:
    cli
    push QWORD PTR 0
    push QWORD PTR 8
    jmp irq_common
.global irq9
irq9:
    cli
    push QWORD PTR 0
    push QWORD PTR 9
    jmp irq_common
.global irq10
irq10:
    cli
    push QWORD PTR 0
    push QWORD PTR 10
    jmp irq_common
.global irq11
irq11:
    cli
    push QWORD PTR 0
    push QWORD PTR 11
    jmp irq_common
.global irq12
irq12:
    cli
    push QWORD PTR 0
    push QWORD PTR 12
    jmp irq_common
.global irq13
irq13:
    cli
    push QWORD PTR 0
    push QWORD PTR 13
    jmp irq_common
.global irq14
irq14:
    cli
    push QWORD PTR 0
    push QWORD PTR 14
    jmp irq_common
.global irq15
irq15:
    cli
    push QWORD PTR 0
    push QWORD PTR 15
    jmp irq_common

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
    mov rax, ds
    push rax
    mov rax, es
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax
    mov rdi, rsp
    call isr_handler
    pop rax
    mov gs, rax
    pop rax
    mov fs, rax
    pop rax
    mov es, rax
    pop rax
    mov ds, rax
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
    mov rax, ds
    push rax
    mov rax, es
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax
    mov rdi, rsp
    call irq_handler
    pop rax
    mov gs, rax
    pop rax
    mov fs, rax
    pop rax
    mov es, rax
    pop rax
    mov ds, rax
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

.global load_idt
load_idt:
    mov rbx, rdi
    lidt [rbx]
    ret