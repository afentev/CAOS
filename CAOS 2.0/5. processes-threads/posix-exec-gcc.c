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
        char program[BUFF_SIZE + 50];
        memset(program, '\0', BUFF_SIZE + 50);
        fprintf(
            file,
            "#include<stdio.h>\n"
            "int main() {"
            "    printf(\"%%d\", (%s));"
            "}",
            buffer);
        fflush(file);
        execlp("gcc", "gcc", "-w", "-o", "aux", "aux.c", NULL);
        return 1;
    } else {
        waitpid(pid, &status, 0);
        pid = fork();
        if (pid == 0) {
            execl("aux", "aux", NULL);
            return 1;
        }
        waitpid(pid, &status, 0);
        pid = fork();
        if (pid == 0) {
            execlp("rm", "rm", "aux.c", "aux", NULL);
            return 1;
        }
        waitpid(pid, &status, 0);
    }
    return 0;
}