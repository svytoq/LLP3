#ifndef GRAPH_QL_PARSER_H
#define GRAPH_QL_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "../model/request.h"
#include "malloc.h"

enum parser_status parse_request(char * req, struct request *request);

enum wrapper_status wrap_to_XML(struct request *request, xmlDocPtr xmlDocPtr);

enum parser_status {
    PARSE_OK = 0,
    PARSE_INCORRECT_REQUEST = 1,
    PARSE_ERROR = 2
};

enum wrapper_status {
    WRAP_OK = 0,
    WRAP_ERROR = 1
};

#endif //GRAPH_QL_PARSER_H
