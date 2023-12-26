#ifndef GRAPH_ORIENTED_FILE_H
#define GRAPH_ORIENTED_FILE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "platform.h"

#define INVALID_PTR 0
#define TABULATION "    "

// file's header
struct header {
    uint32_t signature;
    uint64_t first_hole_ptr; // link on the first hole in file
    uint64_t first_node_ptr; // link on the first node in file
    uint64_t last_node_ptr; // link on the last node in file
    uint64_t node_id; // current node id
} __attribute__((packed));

// file's hole
struct hole {
    uint64_t hole_ptr; // link on hole
    uint64_t size_of_hole; // hole size
    uint64_t prev_ptr;  // link on previous hole
    uint64_t next_ptr; // link on next hole
} __attribute__((packed));


FILE *open_file(char *name);

void *read_file(FILE *file, uint64_t offset, uint64_t length);

uint64_t write_file(FILE *file, void *data_ptr, uint64_t size_of_data);

void delete_from_file(FILE *file, uint64_t offset, uint64_t length);

#endif //GRAPH_ORIENTED_FILE_H





