#include <assert.h>
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

    FILE* file = fopen(argv[2], "r");
    int chr;
    while ((chr = getc(file)) != EOF) {
        write(main_write2child[1], &chr, 1);
    }
    close(main_write2child[1]);
    fclose(file);

    waitpid(pid, NULL, 0);
    size_t count = 0;
    while (read(child_write2main[0], &chr, sizeof(char)) > 0) {
        ++count;
    }
    close(child_write2main[0]);
    printf("%zu", count);
    return 0;
}