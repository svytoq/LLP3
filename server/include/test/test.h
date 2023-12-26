#ifndef GRAPH_ORIENTED_TEST_H
#define GRAPH_ORIENTED_TEST_H

#include "time.h"
#include <stdio.h>
#include <inttypes.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "../utils/crud_interface.h"

void create_node_test(FILE *file, uint64_t intgr, uint64_t dbl, uint64_t bln, uint64_t str);

void find_node_by_id_test(FILE *file, uint64_t id, struct node *node);

void delete_node_by_id_test(FILE *file, uint64_t id);

void update_field_value_by_id_test(FILE *file, uint64_t id, uint64_t filed_name, uint64_t new_value);

void find_node_by_filed_value_test(FILE *file, uint64_t filed_name, uint64_t search_value, struct node_list **node_list);

#endif //GRAPH_ORIENTED_TEST_H
