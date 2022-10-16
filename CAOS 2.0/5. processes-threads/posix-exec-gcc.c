#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main()
{
    char buffer[BUFF_SIZE];
    fgets(buffer, BUFF_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    pid_t pid = fork();
    int status;
    if (pid == 0) {
        FILE* file = fopen("aux.c", "w");
        char program[BUFF_SIZE + 25];
        memset(program, '\0', BUFF_SIZE + 25);
        fprintf(file, "int main() {return %s;}", buffer);
        fflush(file);

        char* argv[] = {"gcc", "-o", "aux.o", "aux.c", NULL};
        execvp("gcc", argv);
    } else {
        waitpid(pid, &status, 0);
        pid = fork();
        if (pid == 0) {
            execl("./aux.o", "aux", NULL);
        } else {
            waitpid(pid, &status, 0);
            printf("%d", WEXITSTATUS(status));
        }
    }
}