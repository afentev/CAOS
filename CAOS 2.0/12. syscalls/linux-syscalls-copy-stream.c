#include <sys/syscall.h>

#define BUFF_SIZE 1024

long syscall(long number, ...) asm("syscall");

void _start()
{
    char buff[BUFF_SIZE];
    long result;
    while ((result = syscall(SYS_read, 0, buff, BUFF_SIZE))) {
        syscall(SYS_write, 1, buff, result);
    }
    syscall(SYS_exit, 0);
}
