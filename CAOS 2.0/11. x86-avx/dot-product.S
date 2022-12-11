.global dot_product
.intel_syntax noprefix
.text

// (N=rdi, *A=rsi, *B=rdx)
dot_product:
    vxorps ymm4, ymm4, ymm4  // clean
    mov r8, rdi
    mov r9, r8
    shr r9, 3
    lea r9, [r9 * 8]
    sub r8, r9
    mov r10, r9

.Loop:
    sub r9, 8

    vmovups ymm0, [rsi + r9 * 4]
    vmovups ymm1, [rdx + r9 * 4]
    vmulps ymm0, ymm0, ymm1

    vxorps  ymm3, ymm3, ymm3 // clean using XOR method
    vperm2f128 ymm3, ymm0, ymm0, 0b10000001 // ymm3 = [0,0,0,0,x4,x7,x6,x5]
    vhaddps ymm3, ymm3, ymm0
    vhaddps ymm3, ymm3, ymm3
    vhaddps ymm3, ymm3, ymm3

    vaddps ymm4, ymm4, ymm3

    cmp r9, 8
    jge .Loop

    movss xmm10, xmm4

    cmp r8, 0
    je .end
    dec r8
    add r10, r8
.rest:
    vmovss xmm0, [rsi + r10 * 4]
    vmovss xmm1, [rdx + r10 * 4]
    vmulss xmm0, xmm0, xmm1
    vaddss xmm10, xmm10, xmm0

    dec r10
    dec r8
    cmp r8, 0
    jge .rest

.end:
    movss xmm0, xmm10
    ret