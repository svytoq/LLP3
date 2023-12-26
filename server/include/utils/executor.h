#ifndef THIRD_EXECUTOR_H
#define THIRD_EXECUTOR_H

#include <libxml/tree.h>
#include <libxml/parser.h>
#include "crud_interface.h"
#include "../net/server.h"

void execute_request(char xml_request[], FILE* file, int fd);

#endif //THIRD_EXECUTOR_H
