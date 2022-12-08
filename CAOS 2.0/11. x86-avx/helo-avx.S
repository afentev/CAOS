.global very_important_function
.intel_syntax noprefix
.text

// (N=rdi, *A=rsi, *B=rdx, *R=rcx)
very_important_function:
    vxorps ymm4, ymm4, ymm4  // clean
    mov r8, rdi

.Loop:
    sub rdi, 8

    vmovaps ymm0, [rsi + rdi * 4]
    vmovaps ymm1, [rdx + rdi * 4]
    vaddps ymm0, ymm0, ymm1
    vmovaps [rcx + rdi * 4], ymm0

    vxorps  ymm3, ymm3, ymm3 // clean using XOR method
    vperm2f128 ymm3, ymm0, ymm0, 0b10000001 // ymm3 = [0,0,0,0,x4,x7,x6,x5]
    vhaddps ymm3, ymm3, ymm0
    vhaddps ymm3, ymm3, ymm3
    vhaddps ymm3, ymm3, ymm3

    vaddps ymm4, ymm4, ymm3

    cmp rdi, 0
    jg .Loop

    cvtss2sd xmm4, xmm4
    cvtsi2sd xmm5, r8

    divsd xmm4, xmm5
    movsd xmm0, xmm4

    ret
