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
    assert(argc == 2);
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct in_addr input_address;
    input_address.s_addr = inet_addr("127.0.0.1");
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr = input_address;
    server_address.sin_port = htons(atoi(argv[1]));

    connect(
        fd, (const struct sockaddr*)&server_address, sizeof(server_address));

    int32_t number;
    while (scanf("%d", &number) > 0) {
        write(fd, &number, 4);
        read(fd, &number, 4);
        printf("%d\n", number);
    }

    close(fd);
    return 0;
}