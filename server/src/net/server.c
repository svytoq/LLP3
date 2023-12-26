#include "../../include/net/server.h"

int start_server(int port) {
    struct sockaddr_in adr = {0};
    int server = Socket(AF_INET, SOCK_STREAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    Bind(server, (struct sockaddr *) &adr, sizeof adr);
    return server;
}

int handler_request(int server, char buf[]) {
    struct sockaddr_in adr = {0};
    Listen(server, 5);
    socklen_t adr_len = sizeof adr;
    int fd = Accept(server, (struct sockaddr *) &adr, &adr_len);
    ssize_t n_read;
    n_read = read(fd, buf, MAX_RESPONSE_SIZE);
    if (n_read == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    if (n_read == 0) {
        printf("nothing question mb error\n");
    }
    return fd;
}

void send_response(char *msg, int fd) {
    write(fd, msg, MAX_RESPONSE_SIZE);
}

void finish_server(int server) {
    close(server);
}
