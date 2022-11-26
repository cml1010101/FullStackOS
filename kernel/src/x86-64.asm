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