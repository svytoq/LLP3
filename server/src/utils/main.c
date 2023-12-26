#include "../../include/test/test.h"
#include "../../include/net/server.h"
#include "../../include/utils/executor.h"

void print_node(FILE *file, struct node *node) {

    printf("{\n");
    printf("%sid : %lu\n", TABULATION, node->id);
    printf("%svertex : %lu\n", TABULATION, node->d);

    uint64_t *nodes = read_file(file, node->nodes, sizeof(uint64_t) * node->d);

    for (uint64_t i = 0; i < node->d; i++) {
        struct node *tmp_node = read_file(file, nodes[i], sizeof(struct node));
        printf("%sneighbour %lu, id : %lu\n", TABULATION, i + 1, tmp_node->id);
        free(tmp_node);
    }
    //free(nodes);

    printf("%sinteger : %lu\n", TABULATION, node->intgr);
    double dbl;
    memcpy(&dbl, &node->dbl, sizeof(node->dbl));
    printf("%sdouble : %.3f\n", TABULATION, dbl);
    printf("%sboolean : %lu\n", TABULATION, node->bln);

    struct string_save *strings = read_file(file, node->strings_array_ptr,
                                            sizeof(struct string_save) * node->n_strings);

    printf("%sstring : %s\n", TABULATION,
           (char *) read_file(file, strings[0].string_line_ptr, strings[0].size_of_string));

    printf("}\n");

    free(strings);
}

void print_node_list(FILE *file, struct node_list *node_list) {
    if (node_list != NULL) {
        struct node_list *next;
        while (node_list != NULL) {
            next = node_list->prev;
            print_node(file, node_list->value);
            node_list = next;
        }
    }
}

void print_all_nodes(FILE *file) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    uint64_t node_ptr = file_header->first_node_ptr;
    free(file_header);

    struct node *iter_node;
    while (node_ptr != INVALID_PTR) {
        printf("{\n");
        iter_node = read_file(file, node_ptr, sizeof(struct node));
        printf("%sid : %lu\n", TABULATION, iter_node->id);
        printf("%svertex : %lu\n", TABULATION, iter_node->d);
        printf("%sinteger : %lu\n", TABULATION, iter_node->intgr);
        double dbl;
        memcpy(&dbl, &iter_node->dbl, sizeof(iter_node->dbl));
        printf("%sdouble : %.3f\n", TABULATION, dbl);
        printf("%sboolean : %lu\n", TABULATION, iter_node->bln);

        struct string_save *strings = read_file(file, iter_node->strings_array_ptr,
                                                sizeof(struct string_save) * iter_node->n_strings);

        printf("%sstring : %s\n", TABULATION,
               (char *) read_file(file, strings[0].string_line_ptr, strings[0].size_of_string));

        free(strings);

        printf("}\n");
        node_ptr = iter_node->next_ptr;
        free(iter_node);
    }
}

void free_node_list(struct node_list *node_list) {
    if (node_list != NULL) {
        struct node_list *next;
        while (node_list != NULL) {
            next = node_list->prev;
            free(node_list->value);
            free(node_list);
            node_list = next;
        }
    }
}


int main(int argc, char **argv) {
    FILE *file = open_file("data.txt");

    int server = start_server(atoi(argv[1]));

    printf("Server start\n");

    while (1) {

        char request_xml[MAX_REQUEST_SIZE];

        int fd = handler_request(server, request_xml);

        execute_request(request_xml, file, fd);

        print_all_nodes(file);
    }

    fclose(file);
    return 0;

}