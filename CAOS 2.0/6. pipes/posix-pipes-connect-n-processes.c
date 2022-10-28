#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc >= 3);
    int fd[2];
    for (int i = 0; i + 2 < argc; ++i) {
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0) {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            execlp(argv[i + 1], argv[i + 1], NULL);
            return 1;
        }
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
    }
    execlp(argv[argc - 1], argv[argc - 1], NULL);
}