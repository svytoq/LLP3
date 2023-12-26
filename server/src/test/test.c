#include "../../include/test/test.h"

void create_node_test(FILE *file, uint64_t intgr, uint64_t dbl, uint64_t bln, uint64_t str) {
    clock_t cl = clock();
    create_node(file, intgr, dbl, bln, str);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void find_node_by_id_test(FILE *file, uint64_t id, struct node *node) {
    clock_t cl = clock();
    find_node_by_id(file, id, node);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void delete_node_by_id_test(FILE *file, uint64_t id) {
    clock_t cl = clock();
    delete_node_by_id(file, id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void update_field_value_by_id_test(FILE *file, uint64_t id, uint64_t filed_name, uint64_t new_value) {
    clock_t cl = clock();
    update_field_value_by_id(file, id, filed_name, new_value);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void find_node_by_filed_value_test(FILE *file, uint64_t filed_name, uint64_t search_value, struct node_list **node_list) {
    clock_t cl = clock();
    find_node_by_filed_value(file, filed_name, search_value, node_list);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}