.intel_syntax noprefix

.text
    .global sum

sum:
    xor rax, rax  // rax -- loop iteration counter

loop:
    mov r8d, [rsi + rax * 4]  // compute A[rax] into r8d
    add r8d, [rdx + rax * 4]  // r8d += B[rax]
    mov [rcx + rax * 4], r8d  // store r8d into C[rax]

    inc rax  // increment counter
    cmp rax, rdi  // check counter against N
    jne loop  // if counter < N

    ret
