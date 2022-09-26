#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BAD_ERRNO(condition)                                                   \
    if (condition) {                                                           \
        return 1;                                                              \
    }

int main(int argc, char** argv)
{
    assert(argc == 4);
    int input_d, output_dig_d, output_nondig_d;

    while ((input_d = open(argv[1], O_RDONLY)) < 0) {
        BAD_ERRNO(errno != EINTR);
    }
    while ((output_dig_d = open(argv[2], O_WRONLY | O_CREAT, 0640)) < 0) {
        BAD_ERRNO(errno != EINTR);
    }
    while ((output_nondig_d = open(argv[3], O_WRONLY | O_CREAT, 0640)) < 0) {
        BAD_ERRNO(errno != EINTR);
    }

    char buff;
    int read_chr;
    while ((read_chr = read(input_d, &buff, 1))) {
        if (read_chr == -1) {
            BAD_ERRNO(errno != EAGAIN && errno != EINTR);
        }
        int output_d = output_nondig_d;
        if ('0' <= buff && buff <= '9') {
            output_d = output_dig_d;
        }
        int write_chr = write(output_d, &buff, 1);
        while (write_chr < 0) {
            BAD_ERRNO(errno != EAGAIN && errno != EINTR);
        }
    }

    while (close(input_d) < 0) {
        BAD_ERRNO(errno != EINTR);
    }
    while (close(output_dig_d) < 0) {
        BAD_ERRNO(errno != EINTR);
    }
    while (close(output_nondig_d) < 0) {
        BAD_ERRNO(errno != EINTR);
    }
    return 0;
}
