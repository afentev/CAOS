#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    size_t n = atoi(argv[1]);

    size_t pid_num = 1;
    pid_t pid;
    while ((pid = fork()) == 0 && pid_num != n) {
        printf("%zu ", pid_num);
        fflush(stdout);
        ++pid_num;
    }
    if (pid_num == n) {
        printf("%zu", pid_num);
        fflush(stdout);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 0;
}