#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc == 3);
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execlp(argv[1], argv[1], NULL);
        return 1;
    }
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    execlp(argv[2], argv[2], NULL);
}