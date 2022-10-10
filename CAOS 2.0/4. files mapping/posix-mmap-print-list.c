#include <inttypes.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct Item {
    int value;
    uint32_t next_pointer;
} item_t;

int close_file(int fd)
{
    while (close(fd) < 0) {
        if (errno != EINTR) {
            return 1;
        }
    }
    return 0;
}

item_t bit_cast_memcpy(char* buf)
{
    item_t result;
    memcpy(&result, buf, sizeof(result));
    return result;
}

int main(int argc, char* argv[])
{
    assert(argc == 2);
    char* input_file_name = argv[1];
    int fd;
    while ((fd = open(input_file_name, O_RDONLY)) < 0) {
        if (errno != EINTR) {
            return 1;
        }
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
    if (file_stat.st_size != 0) {
        uint32_t position = 0;
        do {
            item_t node = bit_cast_memcpy(map + position);
            printf("%d ", node.value);
            position = node.next_pointer;
        } while (position != 0);
    }

    if (munmap(map, file_stat.st_size) == -1) {
        close_file(fd);
        return 1;
    }
    return close_file(fd);
}