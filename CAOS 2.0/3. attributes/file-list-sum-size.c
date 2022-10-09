#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// C's standard FILENAME_MAX constant is not safe to use
#define MAX_FILENAME 1025

int main()
{
    struct stat file_stat;
    char buff[MAX_FILENAME];
    unsigned long long total_size = 0;
    while (fgets(buff, MAX_FILENAME, stdin)) {
        buff[strcspn(buff, "\n")] = 0;
        int res = lstat(buff, &file_stat);
        if (res < 0) {
            return 1;
        }
        if (!S_ISREG(file_stat.st_mode)) {
            continue;
        }
        total_size += file_stat.st_size;
    }
    if (ferror(stdin)) {
        return 1;
    }
    printf("%llu", total_size);
}
