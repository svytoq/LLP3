#ifndef THIRD_NET_H
#define THIRD_NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#define MAX_RESPONSE_SIZE 8192
#define MAX_REQUEST_SIZE 1024

int Socket(int domain, int type, int protocol);

void Bind(int sock_fd, const struct sockaddr* addr, socklen_t addr_len);

void Listen(int sock_fd, int backlog);

int Accept(int sock_fd, struct sockaddr* addr, socklen_t* addr_len);

void Connect(int sock_fd, const struct sockaddr* addr, socklen_t addr_len);

void Inet_pton(int af, const char* src, void* dst);

#endif //THIRD_NET_H
