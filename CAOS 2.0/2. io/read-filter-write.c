#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int close_file(int fd)
{
    if (fd != -1) {
        while (close(fd) < 0) {
            if (errno != EINTR) {
                return 1;
            }
        }
    }
    return 0;
}

int close_files(int fd1, int fd2, int fd3)
{
    int error = close_file(fd1);
    error |= close_file(fd2);
    error |= close_file(fd3);
    return error;
}

#define BAD_ERRNO(condition, fd1, fd2, fd3)                                    \
    if (condition) {                                                           \
        close_files(fd1, fd2, fd3);                                            \
        return 1;                                                              \
    }

#define BUFF_SIZE 1024

int main(int argc, char** argv)
{
    assert(argc == 4);
    int input_d, output_dig_d, output_nondig_d;

    while ((input_d = open(argv[1], O_RDONLY)) < 0) {
        BAD_ERRNO(errno != EINTR, -1, -1, -1);
    }
    while ((output_dig_d = open(argv[2], O_WRONLY | O_CREAT, 0640)) < 0) {
        BAD_ERRNO(errno != EINTR, input_d, -1, -1);
    }
    while ((output_nondig_d = open(argv[3], O_WRONLY | O_CREAT, 0640)) < 0) {
        BAD_ERRNO(errno != EINTR, input_d, output_dig_d, -1);
    }

    char buff[BUFF_SIZE];
    char digits_buff[BUFF_SIZE];
    char non_digits_buff[BUFF_SIZE];
    int read_chr;
    while ((read_chr = read(input_d, buff, BUFF_SIZE))) {
        if (read_chr == -1) {
            BAD_ERRNO(
                errno != EAGAIN && errno != EINTR,
                input_d,
                output_dig_d,
                output_nondig_d);
            continue;
        }
        int digits_cnt = 0;
        int non_digits_cnt = 0;
        for (int byte = 0; byte < read_chr; ++byte) {
            if ('0' <= buff[byte] && buff[byte] <= '9') {
                digits_buff[digits_cnt++] = buff[byte];
            } else {
                non_digits_buff[non_digits_cnt++] = buff[byte];
            }
        }
        while (write(output_dig_d, digits_buff, digits_cnt) < 0) {
            BAD_ERRNO(
                errno != EAGAIN && errno != EINTR,
                input_d,
                output_dig_d,
                output_nondig_d);
        }
        while (write(output_nondig_d, non_digits_buff, non_digits_cnt) < 0) {
            BAD_ERRNO(
                errno != EAGAIN && errno != EINTR,
                input_d,
                output_dig_d,
                output_nondig_d);
        }
    }
    return close_files(input_d, output_dig_d, output_nondig_d);
}
