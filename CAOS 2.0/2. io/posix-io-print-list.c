#include <inttypes.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
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

#define CHECK_ERRNO                                                            \
    if (errno != EINTR && errno != EAGAIN) {                                   \
        close_file(fd);                                                        \
        return 1;                                                              \
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
    uint32_t position = 0;
    char buff[sizeof(item_t)];
    do {
        int cnt;
        while ((cnt = pread(fd, buff, sizeof(item_t), position)) < 0) {
            CHECK_ERRNO
        }
        if (cnt == 0) {
            break;
        }
        item_t node = bit_cast_memcpy(buff);
        char strbuf[20];
        int n = sprintf(strbuf, "%d ", node.value);
        if (n < 0) {
            close_file(fd);
            return 1;
        }
        while (write(STDOUT_FILENO, strbuf, n) < 0) {
            CHECK_ERRNO
        }
        position = node.next_pointer;
    } while (position != 0);

    return close_file(fd);
}