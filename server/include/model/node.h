#ifndef GRAPH_ORIENTED_NODE_H
#define GRAPH_ORIENTED_NODE_H

#include "string.h"
#include "../utils/file.h"

struct node_list {
    struct node_list *prev;
    struct node *value;
    uint64_t id;
};

// struct for string save in file
struct string_save {
    uint64_t size_of_string;
    uint64_t string_line_ptr;
} __attribute__((packed));

// main struct
struct node {
    uint64_t id; // node id
    uint64_t d; // vertex
    uint64_t nodes; // link on connected nodes
    uint64_t prev_ptr; // link on the previous node
    uint64_t next_ptr; // link on the next node
    uint64_t intgr; // integer
    uint64_t dbl; // double
    uint64_t bln; // boolean
    uint64_t n_strings;
    uint64_t strings_array_ptr; // link on struct "string_save" which have our string
} __attribute__((packed));

#endif //GRAPH_ORIENTED_NODE_H