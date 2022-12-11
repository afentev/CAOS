#include <sys/syscall.h>

long syscall(long number, ...) asm("syscall");

void _start()
{
    char* msg = "Hello, World!";
    syscall(SYS_write, 1, msg, 13);
    syscall(SYS_exit, 0);
}