#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_SIZE 4096
#define CONNECTIONS_LIMIT 65535

volatile sig_atomic_t killed = 0;

typedef struct {
    int fd;
    bool closed;
} data_t;

typedef struct epoll_event event_t;

void handler(int signum)
{
    if (signum == SIGTERM) {
        killed = 1;
    }
}

void close_connection(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

data_t* create_event(int epoll_fd, int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    data_t* data = calloc(1, sizeof(data_t));
    data->fd = fd;

    event_t event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.ptr = data;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    return data;
}

void handle_event(
    event_t* event,
    int* open_connections,
    int epoll_fd,
    int server_fd)
{
    data_t* data = event->data.ptr;
    if (data->fd == server_fd) {
        int client_fd = accept(server_fd, NULL, NULL);
        create_event(epoll_fd, client_fd);
    } else if (event->events & EPOLLIN) {
        char buff[BUFF_SIZE];
        ssize_t scanned;
        while ((scanned = read(data->fd, buff, BUFF_SIZE)) > 0) {
            for (int i = 0; i < scanned; ++i) {
                buff[i] = toupper(buff[i]);
            }
            write(data->fd, buff, scanned);
        }
        if (scanned == 0) {
            close_connection(data->fd);
            event_t restore;
            memset(&restore, 0, sizeof(restore));
            restore.events = EPOLLIN;
            restore.data.fd = data->fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, data->fd, &restore);
            close(data->fd);
        }
    }
}

int main(int argc, char* argv[])
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    sigaction(SIGTERM, &action, NULL);

    assert(argc == 2);
    uint16_t port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_sock_addr;
    serv_sock_addr.sin_family = AF_INET;
    serv_sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_sock_addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&serv_sock_addr, sizeof(serv_sock_addr));
    listen(server_fd, SOMAXCONN);

    int epoll_fd = epoll_create1(0);
    data_t* listen_event = create_event(epoll_fd, server_fd);

    int open_connections = 0;
    event_t ready[CONNECTIONS_LIMIT];

    while (!killed) {
        int updated = epoll_wait(epoll_fd, ready, CONNECTIONS_LIMIT, -1);
        for (int i = 0; i < updated; ++i) {
            handle_event(&ready[i], &open_connections, epoll_fd, server_fd);
        }
    }

    close(epoll_fd);
    close(server_fd);
}
