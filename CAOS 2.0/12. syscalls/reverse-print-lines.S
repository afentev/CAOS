.intel_syntax noprefix
.global _start
.data
newline:
    .string "\n"

.text
    _start:
        push r12
        push r13  // initial brk pointer
        push r14  // total read
        push r15  // remain in the current buff (<= 4096)

        xor r14, r14

        mov rax, 12  // brk syscall
        mov rdi, 0  // brk(NULL) return current heap pointer
        syscall
        mov r13, rax

    .allocate:
        mov rax, 12  // brk syscall
        mov rdi, 0  // brk(NULL) return current heap pointer
        syscall

        lea rdi, [rax + 4096]  // increase heap pointer
        mov rax, 12
        syscall

        mov r15, 4096

    .read:
        mov rax, 0  // read syscall
        mov rdi, 0  // 0 means stdin
        lea rsi, [r13 + r14]
        mov rdx, r15
        syscall

        xor r11, r11
        cmp rax, 0
        jle .printer

        sub r15, rax
        add r14, rax

        cmp r15, 0
        je .allocate

        jmp .read

    .printer:
        cmp r14, 0
        je .end
        
        dec r14
        mov rdx, [r13 + r14]
        inc r11
        cmp r14, 0
        je .end

        cmp dl, '\n'
        jne .printer

    .skip:
        inc r14
        dec r11

        mov rax, 1
        mov rdi, 1
        lea rsi, [r13 + r14]
        mov rdx, r11
        syscall

        mov rax, 1
        mov rdi, 1
        lea rsi, [rip + newline]
        mov rdx, 1
        syscall

        xor r11, r11
        dec r14
        cmp r14, 0
        jg .printer

    .end:
        mov rax, 1
        mov rdi, 1
        mov rsi, r13
        mov rdx, r11
        syscall

        pop r15
        pop r14
        pop r13
        pop r12

        mov rax, 60
        mov rdi, 0
        syscall
