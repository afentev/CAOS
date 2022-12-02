.intel_syntax noprefix

.text
    .global mergesort

mergesort:
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi  // save "from" to r12
    mov r13, rsi  // save "to" to r13
    mov r14, rdx  // save "in" to r14
    mov r15, rcx  // save "out" to r15
    // after this line we can currupt callee-saved registers

    // because our sort will be inplace, we need to copy "in" into "out"
    xor rax, rax  // index
copy_loop:
    mov r8d, [r14 + rax * 4]  // save in[rax] into r8d
    mov [r15 + rax * 4], r8d  // save r8d into out[rax]
    inc rax  // increment index
    cmp rax, r13  // compare index against "to"
    jne copy_loop  // index != "to", so we can do one more iteration

    // copying is done, now we may start a loop through step size
    mov rax, 1  // rax is a step-size
    xor r8, r8  // r8 is a left pointer

main_loop:
    lea r9, [rax + r8]  // r9 is a mid pointer

    mov r10, r13  // r10 is an right pointer = min(end, left + 2 * step)
    mov rdi, rax  // compute left + 2 * step, now rdi = step
    shl rdi, 1  // rdi *= 2, now rdi = step * 2
    add rdi, r8  // rdi += r8, now rdi = step * 2 + left
    cmp rdi, r10  // check minima
    jg loop_body  // left + 2 * step > end, r10 already correct
    mov r10, rdi  // right = left + 2 * step

loop_body:
    mov rdi, r9  // rdi is lhs
    dec rdi
    mov rsi, r10  // rsi is rhs
    dec rsi
    xor rcx, rcx  // rcx is total

paired_loop:
    cmp rdi, r8  // check lhs ? mid
    jl right_half_completion_loop  // lhs >= mid, complete the right half

    cmp rsi, r9  // check rhs ? right
    jl left_half_completion_loop  // rhs >= right, complete the left half

    mov rdx, [r15 + rdi * 4]  // out[lhs]
    mov r11, [r15 + rsi * 4]  // out[rhs]
    cmp edx, r11d
    jl push_rhs  // out[lhs] < out[rhs]

push_lhs:
    push rdx
    dec rdi
    jmp end_paired_loop

push_rhs:
    push r11
    dec rsi

end_paired_loop:
    inc rcx
    jmp paired_loop

left_half_completion_loop:
    push [r15 + rdi * 4]  // push out[lhs]
    dec rdi  // --lhs
    inc rcx  // ++total
    cmp rdi, r8  // check lhs ? mid
    jl main_loop_end // lhs >= mid
    jmp left_half_completion_loop  // lhs < mid, do one more iteration

right_half_completion_loop:
    push [r15 + rsi * 4]  // push out[rhs]
    dec rsi  // --rhs
    inc rcx  // ++total
    cmp rsi, r9  // check rhs ? right
    jnl right_half_completion_loop  // rhs < right, do one more iteration

main_loop_end:
    xor r10, r10  // r10 is counter = 0..<total a.k.a i

    // copy from stack into "out"
copying_loop:
    pop r11  // r11 is stack top
    lea rsi, [r8 + r10]  // rsi = left + i
    mov [r15 + rsi * 4], r11d  // out[left + i] = tmp[i]
    inc r10  // ++i
    cmp r10, rcx  // check i ? total
    jl copying_loop  // if i < total, do one more iteration

    mov r12, r13  // end - i
    sub r12, rax  // r12 = end - i

    add r8, rax
    add r8, rax

    cmp r8, r12  // j ? end - i
    jl main_loop

step_size_increment:
    shl rax, 1  // i *= 2
    xor r8, r8  // r8 = 0

    cmp rax, r13
    jl main_loop


return:
    pop r15
    pop r14
    pop r13
    pop r12
    ret