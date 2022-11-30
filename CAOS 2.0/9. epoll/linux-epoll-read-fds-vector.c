#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_SIZE 4096

typedef struct {
    int fd;
    size_t count;
    bool done;
} data_t;

typedef struct epoll_event event_t;

void handle_event(event_t* event)
{
    data_t* data = event->data.ptr;

    ssize_t scanned;
    char buffer[BUFF_SIZE];
    while ((scanned = read(data->fd, buffer, BUFF_SIZE)) > 0) {
        data->count += scanned;
    }
    if (scanned == 0) {
        data->done = true;
        close(data->fd);
    }
}

extern size_t read_data_and_count(size_t N, int in[N])
{
    int epoll_fd = epoll_create1(0);
    data_t** events = malloc(N * sizeof(data_t*));

    for (size_t i = 0; i < N; ++i) {
        int fd = in[i];
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

        data_t* data = calloc(1, sizeof(data_t));
        data->fd = fd;

        event_t event_ready_read;
        event_ready_read.events = EPOLLIN;
        event_ready_read.data.ptr = data;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_read);

        events[i] = data;
    }

    ssize_t opened = N;
    event_t* ready = malloc(N * sizeof(event_t));
    while (opened > 0) {
        int updated = epoll_wait(epoll_fd, ready, N, -1);
        for (int i = 0; i < updated; ++i) {
            handle_event(&ready[i]);

            data_t* data = ready[i].data.ptr;
            if (data->done) {
                --opened;
            }
        }
    }

    size_t result = 0;
    for (int i = 0; i < N; ++i) {
        result += events[i]->count;
        free(events[i]);
    }
    free(events);
    free(ready);

    return result;
}
