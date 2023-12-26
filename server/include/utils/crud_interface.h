#ifndef GRAPH_ORIENTED_CRUD_INTERFACE_H
#define GRAPH_ORIENTED_CRUD_INTERFACE_H

#include "../model/node.h"

enum crud_status create_node(FILE *file, uint64_t intgr, uint64_t dbl, uint64_t bln, uint64_t str);

enum crud_status find_node_by_id(FILE *file, uint64_t id, struct node *node);

enum crud_status delete_node_by_id(FILE *file, uint64_t id);

enum crud_status delete_all(FILE *file);

enum crud_status find_all(FILE *file, struct node_list **node_list);

enum crud_status connect_nodes(FILE *file, uint64_t id1, uint64_t id2);

enum crud_status update_field_value_by_id(FILE *file, uint64_t id, uint64_t filed_name, uint64_t new_value);

enum crud_status find_node_by_filed_value(FILE *file, uint64_t filed_name, uint64_t search_value, struct node_list **node_list);

enum crud_status {
    CRUD_OK = 0,
    CRUD_ERROR = 1
};

#endif //GRAPH_ORIENTED_CRUD_INTERFACE_H
