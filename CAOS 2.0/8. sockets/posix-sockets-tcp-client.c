#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc == 3);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct in_addr input_address;
    input_address.s_addr = inet_addr(argv[1]);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr = input_address;
    server_address.sin_port = htons(atoi(argv[2]));

    int connection_status = connect(
        fd, (const struct sockaddr*)&server_address, sizeof(server_address));
    if (connection_status) {
        return 1;
    }

    int32_t number;
    while (scanf("%d", &number) > 0) {
        int result = write(fd, &number, 4);
        if (result <= 0) {
            break;
        }
        result = read(fd, &number, 4);
        if (result <= 0) {
            break;
        }
        printf("%d ", number);
    }
    shutdown(fd, SHUT_RDWR);
    close(fd);
    return 0;
}