#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

const int BUFF_SIZE = 4096;
const static char* OK = "HTTP/1.1 200 OK\n";
const static char* NOT_FOUND = "HTTP/1.1 404 Not Found\n";
const static char* PERMISSION_DENIED = "HTTP/1.1 403 Forbidden\n";

volatile sig_atomic_t in_work = 1;
volatile sig_atomic_t is_blocked = 0;
volatile sig_atomic_t child = 0;

void server_response(int client_fd, char* file_path)
{
    struct stat file_stat;
    int res = stat(file_path, &file_stat);
    if (res < 0) {
        write(client_fd, NOT_FOUND, strlen(NOT_FOUND));
    } else if (!(file_stat.st_mode & S_IRUSR)) {
        write(client_fd, PERMISSION_DENIED, strlen(PERMISSION_DENIED));
    } else if (!(file_stat.st_mode & S_IXUSR)) {
        write(client_fd, OK, strlen(OK));

        FILE* file = fopen(file_path, "r");
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);

        char content_length[40];
        sprintf(content_length, "Content-Length: %ld\n\n", size);
        write(client_fd, content_length, strlen(content_length));

        rewind(file);
        char buffer[BUFF_SIZE];
        size_t scanned;
        while ((scanned = fread(buffer, 1, BUFF_SIZE, file))) {
            write(client_fd, buffer, scanned);
        }
        fclose(file);
    } else {
        write(client_fd, OK, strlen(OK));

        pid_t pid = fork();
        if (pid == 0) {
            dup2(client_fd, STDOUT_FILENO);
            execlp(file_path, file_path, NULL);
            exit(1);
        }
        child = pid;
        is_blocked = 1;
        waitpid(pid, NULL, 0);
        is_blocked = 0;
    }
}

void handle_client(int client_fd, char* path)
{
    FILE* client = fdopen(client_fd, "r");
    char filename[BUFF_SIZE];
    fscanf(client, "GET %s HTTP/1.1", filename);

    char file_path[BUFF_SIZE];
    memset(file_path, '\0', BUFF_SIZE);
    strcpy(file_path, path);
    strcat(file_path, filename);

    server_response(client_fd, file_path);
    shutdown(client_fd, SHUT_RDWR);
    fclose(client);
    close(client_fd);
}

int server_socket_init(uint16_t port)
{
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct in_addr input_address;
    input_address.s_addr = inet_addr("127.0.0.1");
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr = input_address;
    server_address.sin_port = htons(port);

    int connection_status = bind(
        server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (connection_status) {
        perror("Bind failed: ");
        exit(1);
    }

    int listening_status = listen(server_fd, SOMAXCONN);
    if (listening_status) {
        perror("Listen failed: ");
        exit(1);
    }
    return server_fd;
}

void signal_handler(int32_t sig_num)
{
    if (is_blocked) {
        if (child != 0) {
            kill(child, SIGKILL);
        }
        exit(0);
    } else {
        in_work = 0;
    }
}

int main(int argc, char* argv[])
{
    assert(argc == 3);
    uint16_t port = atoi(argv[1]);
    char* path = argv[2];

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    int server_fd = server_socket_init(port);
    while (in_work) {
        is_blocked = 1;
        int client_fd = accept(server_fd, NULL, NULL);
        is_blocked = 0;

        handle_client(client_fd, path);
    }
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);
    return 0;
}