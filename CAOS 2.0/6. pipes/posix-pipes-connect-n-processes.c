#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc >= 3);
    int fd[2][2];
    pipe(fd[0]);
    pipe(fd[1]);
    int cur = 0;
    for (int i = 0; i + 2 < argc; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            close(fd[cur][0]);
            dup2(fd[cur][1], STDOUT_FILENO);
            close(fd[cur][1]);
            execlp(argv[i + 1], argv[i + 1], NULL);
            return 1;
        }
        close(fd[cur][1]);
        dup2(fd[cur][0], STDIN_FILENO);
        close(fd[cur][0]);

        pipe(fd[cur]);
        cur = (cur + 1) % 2;
    }
    execlp(argv[argc - 1], argv[argc - 1], NULL);
}