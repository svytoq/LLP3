#include "../../include/net/net.h"

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    if (res == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Bind(int sock_fd, const struct sockaddr *addr, socklen_t addr_len) {
    int res = bind(sock_fd, addr, addr_len);
    if (res == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sock_fd, int back_log) {
    int res = listen(sock_fd, back_log);
    if (res == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sock_fd, struct sockaddr *addr, socklen_t *addr_len) {
    int res = accept(sock_fd, addr, addr_len);
    if (res == -1) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Connect(int sock_fd, const struct sockaddr *addr, socklen_t addr_len) {
    int res = connect(sock_fd, addr, addr_len);
    if (res == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
}

void Inet_pton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == 0) {
        printf("inet failed\n");
        exit(EXIT_FAILURE);
    }
    if (res == -1) {
        perror("inet failed");
        exit(EXIT_FAILURE);
    }
}
