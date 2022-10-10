#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int close_file(int fd)
{
    while (close(fd) < 0) {
        if (errno != EINTR) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    char* file_name = argv[1];
    char* pattern = argv[2];
    size_t length = strlen(pattern);

    int fd;
    while ((fd = open(file_name, O_RDONLY)) < 0) {
        if (errno != EINTR) {
            return 1;
        }
    }
    if (fd == -1) {
        return 1;
    }
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        close_file(fd);
        return 1;
    }
    char* map = mmap(0, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close_file(fd);
        return 1;
    }
    for (size_t pos = 0; pos + length < file_stat.st_size; ++pos) {
        if (memcmp(map + pos, pattern, length) == 0) {
            printf("%zu\n", pos);
        }
    }
    if (munmap(map, file_stat.st_size) == -1) {
        close_file(fd);
        return 1;
    }
    return close_file(fd);
}