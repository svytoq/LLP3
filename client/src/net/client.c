#include "../../include/net/client.h"
#include "../../include/conf/conf.h"

void sendRequest(int port, int str_len, char request[]) {
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
    Connect(fd, (struct sockaddr *) &adr, sizeof adr);
    write(fd, request, str_len - 1);


    char buf[MAX_RESPONSE_SIZE];
    ssize_t n_read;

    n_read = read(fd, buf, MAX_RESPONSE_SIZE);

    if (n_read == -1) {
        perror("read failed\n");
        exit(EXIT_FAILURE);
    }

    if (n_read == 0) {
        printf("nothing answer mb error?\n");
    }

    printf("%s\n", buf);

    close(fd);
}
