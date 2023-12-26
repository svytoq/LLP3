#ifndef THIRD_CLIENT_H
#define THIRD_CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include "net.h"
#include "../conf/conf.h"

void sendRequest(int port, int str_len, char request[]);

#endif //THIRD_CLIENT_H
