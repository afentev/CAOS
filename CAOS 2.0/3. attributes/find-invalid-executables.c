#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_PATH 4099
#define MAX_FILENAME 1025

int main()
{
    char buff[MAX_FILENAME];

    while (fgets(buff, MAX_FILENAME, stdin)) {
        buff[strcspn(buff, "\n")] = 0;
        FILE* file = fopen(buff, "r");
        if (file == NULL) {
            continue;
        }
        struct stat file_stat;
        int res = stat(buff, &file_stat);
        if (res < 0 || !(file_stat.st_mode & S_IXUSR)) {
            continue;
        }
        char line[MAX_PATH];
        char* ret = fgets(line, MAX_PATH, file);
        if (ret == NULL) {
            printf("%s\n", buff);
            continue;
        }
        line[strcspn(line, "\n")] = 0;
        if (line[0] == 0x7f && line[1] == 'E' && line[2] == 'L' &&
            line[3] == 'F') {
            continue;
        }
        if (line[0] == '#' && line[1] == '!') {
            res = stat(line + 2, &file_stat);
            if (res >= 0 && file_stat.st_mode & S_IXUSR) {
                continue;
            }
        }
        printf("%s\n", buff);
    }
    if (ferror(stdin)) {
        return 1;
    }
}