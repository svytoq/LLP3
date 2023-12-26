#ifndef GRAPH_QL_REQUEST_H
#define GRAPH_QL_REQUEST_H

#include "../conf/conf.h"

struct attribute {
    char *left;
    char *right;
    char *condition;
    struct attribute *next_attribute;
    char *combined_condition;
};

struct request {
    char *operation;
    struct attribute *attributes;
};

#endif //GRAPH_QL_REQUEST_H
