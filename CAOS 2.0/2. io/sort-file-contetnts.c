#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
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

int close_files(int fd1, int fd2)
{
    int error = close_file(fd1);
    error |= close_file(fd2);
    return error;
}

#define BAD_ERRNO(condition, fd1, fd2)                                         \
    if (condition) {                                                           \
        close_files(fd1, fd2);                                                 \
        return 1;                                                              \
    }                                                                          \
    continue;

int min(int a, int b)
{
    if (a < b) {
        return a;
    }
    return b;
}

static const int BUFF_SIZE = 65536;

int radixSort(char* filename)
{
    // radix bucket length; 8 gives the best performance
    const size_t groupLength = 8;

    // radix bucket size
    const size_t groupSize = 1ull << groupLength;

    // 32 = 4 << 3; length of int in bits
    const size_t size = 32;

    // number of groups in one number 32-bit number
    const size_t groups = size / groupLength;

    // value for extracting i-th group from number
    const size_t rightShift = size - groupLength;

    // 256 is maximum number of elements in one group
    int32_t digits[256];

    // auxiliary file
    int aux, fd;
    while ((aux = open("tmp.txt", O_RDWR | O_CREAT, 00640)) < 0) {
        BAD_ERRNO(errno != EINTR, -1, -1)
    }

    while ((fd = open(filename, O_RDWR)) < 0) {
        BAD_ERRNO(errno != EINTR, aux, -1)
    }

    uint32_t buff[BUFF_SIZE];
    int res;
    int cnt = 0;
    while ((res = pread(fd, buff, BUFF_SIZE * 4, cnt))) {
        if (res < 0) {
            BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
        }
        for (int i = 0; i * 4 < res; ++i) {
            // flip leftmost bit in order to operate with negative numbers as with positive ones
            // at the end of the sort these bits will be flipped again
            buff[i] ^= 1u << 31;
        }
        while ((res = pwrite(fd, buff, res, cnt)) < 0) {
            BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
        }
        cnt += res;
    }
    // numbers in file
    int N = cnt / 4;

    memset(buff, 0, BUFF_SIZE * 4);
    cnt = 0;
    while (cnt < N * 4) {
        while ((res = pwrite(aux, buff, BUFF_SIZE * 4, cnt)) < 0) {
            BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
        }
        cnt += res;
    }

    for (size_t byte = 1; byte <= groups; ++byte) {
        // value for extracting byte-th group from number
        const size_t leftShift = size - groupLength * byte;
        memset(digits, 0, groupSize * 4);

        cnt = 0;
        while ((res = pread(fd, buff, BUFF_SIZE * 4, cnt))) {
            if (res < 0) {
                BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
            }
            for (int i = 0; i * 4 < res; ++i) {
                uint32_t number = buff[i];
                ++digits[(number << leftShift) >> rightShift];
            }
            cnt += res;
        }

        cnt = 0;
        for (size_t i = 0; i < groupSize; ++i) {
            int d = digits[i];
            digits[i] = cnt;
            cnt += d;
        }

        cnt = 0;
        while ((res = pread(fd, buff, BUFF_SIZE * 4, cnt))) {
            if (res < 0) {
                BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
            }
            for (int i = 0; i * 4 < res; ++i) {
                uint32_t number = buff[i];
                int index = digits[(number << leftShift) >> rightShift]++;
                while (pwrite(aux, &number, 4, index * 4) < 0) {
                    BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
                }
            }
            cnt += res;
        }

        cnt = 0;
        while ((res = pread(aux, buff, min(BUFF_SIZE * 4, N * 4 - cnt), cnt))) {
            while (res < 0 || pwrite(fd, buff, res, cnt) < 0) {
                BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
            }
            cnt += res;
        }
    }

    cnt = 0;
    while ((res = pread(fd, buff, BUFF_SIZE, cnt))) {
        if (res < 0) {
            BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
        }
        for (int i = 0; i * 4 < res; ++i) {
            // restore sign bit
            buff[i] ^= 1u << 31;
        }
        while (pwrite(fd, buff, res, cnt) < 0) {
            BAD_ERRNO(errno != EINTR && errno != EAGAIN, aux, fd)
        }
        cnt += res;
    }
    return close_files(fd, aux);
}

int main(int argc, char** argv)
{
    assert(argc == 2);
    char* name = argv[1];
    return radixSort(name);
}