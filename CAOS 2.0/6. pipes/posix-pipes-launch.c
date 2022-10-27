#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc == 3);
    int main_write2child[2];
    int child_write2main[2];
    pipe(main_write2child);
    pipe(child_write2main);

    pid_t pid = fork();
    if (pid == 0) {
        dup2(main_write2child[0], STDIN_FILENO);
        dup2(child_write2main[1], STDOUT_FILENO);

        close(main_write2child[0]);
        close(main_write2child[1]);
        close(child_write2main[0]);
        close(child_write2main[1]);

        execlp(argv[1], argv[1], NULL);
        return 1;
    }
    close(main_write2child[0]);
    close(child_write2main[1]);

    int fd = open(argv[2], O_RDONLY);
    char buff[1024];
    ssize_t got;
    while ((got = read(fd, buff, 1024)) > 0) {
        write(main_write2child[1], buff, got);
    }
    close(main_write2child[1]);
    close(fd);

    waitpid(pid, NULL, 0);
    size_t count = 0;
    while ((got = read(child_write2main[0], buff, 1024)) > 0) {
        count += got;
    }
    close(child_write2main[0]);
    printf("%zu", count);
    return 0;
}