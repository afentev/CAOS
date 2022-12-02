.intel_syntax noprefix

.data
format_input:
    .string "%d"

format_output:
    .string "%d\n"
number:
    .long 0

.text
    .extern A
    .extern scanf
    .extern printf
    .global very_important_function

very_important_function:
    push rbx  // align the stack and save rbx
    xor r12, r12  // r12 -- loop iteration counter
    mov rbx, rdi  // save N which is in rdi into rbx
    mov r13, A[rip]  // save A into r13 register

loop:
    lea rdi, format_input[rip]  // Address of format is the first argument
    lea rsi, number[rip]  // Address of the int is second argument
    call scanf  // Calls scanf with our arguments

    mov rsi, number[rip]  // save read number into rsi, the second argument
    mov rcx, [r13 + r12 * 4]  // save A[r12] into rcx
    imul rsi, rcx  // multiply rax by A[r12], the second argument

    lea rdi, format_output[rip]  // put format string into rdi, the first argument
    call printf

    inc r12  // increment counter
    cmp r12, rbx  // check counter against N
    jne loop  // if counter < N

    pop rbx  // align the stack and restore rbx
    ret
