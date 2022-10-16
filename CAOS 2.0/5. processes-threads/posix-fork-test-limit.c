#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    size_t pid_num = 1;
    pid_t pid;
    while ((pid = fork()) == 0) {
        ++pid_num;
    }
    if (pid < 0) {
        printf("%zu", pid_num);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 0;
}