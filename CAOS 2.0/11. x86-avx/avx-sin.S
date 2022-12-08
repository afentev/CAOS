    .intel_syntax noprefix
    .text
    .global avx_sin

avx_sin:
    movsd xmm1, xmm0 // preserve x
    vxorpd xmm0, xmm0, xmm0 // clean result
    mov rax, 1
    cvtsi2sd xmm2, rax // x^n
    cvtsi2sd xmm5, rax // n!

    mov rax, 0 // i
    mov rdi, 50 // iteration_count
    mov r8, 1
.Loop:
    vmulsd xmm2, xmm2, xmm1 // x^(n-1)*x
    add rax, 1 // i++
    cvtsi2sd xmm6, rax
    vmulsd xmm5, xmm5, xmm6 // (n-1)! * n

    vmovsd xmm4, xmm4, xmm2 // x^n
    vdivsd xmm4, xmm4, xmm5 // x^n/n!

    cmp r8, 1
    jne .Neg
    vaddsd xmm0, xmm0, xmm4 // res += x^n/n!
    mov r8, 0
    jmp .Body

.Neg:
    vsubsd xmm0, xmm0, xmm4 // res -= x^n/n!
    mov r8, 1

.Body:
    vmulsd xmm2, xmm2, xmm1 // x^(n)*x
    add rax, 1 // i++
    cvtsi2sd xmm6, rax
    vmulsd xmm5, xmm5, xmm6 // (n)! * (n+1)

    cmp rax, rdi
    jl .Loop
.Finalize:
    ret