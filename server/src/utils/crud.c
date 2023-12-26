#include "../../include/utils/crud_interface.h"

uint64_t get_ptr_by_id(FILE *file, uint64_t id) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    if (file_header->node_id == 0) return INVALID_PTR;
    if (file_header->node_id < id) return INVALID_PTR;
    uint64_t node_ptr = file_header->first_node_ptr;
    free(file_header);

    struct node *iter_node;
    while (node_ptr != INVALID_PTR) {
        iter_node = read_file(file, node_ptr, sizeof(struct node));
        if (iter_node->id == id) {
            free(iter_node);
            return node_ptr;
        }
        node_ptr = iter_node->next_ptr;
    }

    return INVALID_PTR;
}

enum crud_status create_node(FILE *file, uint64_t intgr, uint64_t dbl, uint64_t bln, uint64_t str) {

    if (bln != 0 && bln != 1) {
        printf("{\n");
        printf("%s boolean is 0 or 1", TABULATION);
        printf("}\n");
        return CRUD_ERROR;
    }
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    struct node *new_node = (struct node *) malloc(sizeof(struct node));

    file_header->node_id = file_header->node_id + 1;

    new_node->id = file_header->node_id;
    new_node->prev_ptr = INVALID_PTR;
    new_node->next_ptr = INVALID_PTR;
    new_node->nodes = INVALID_PTR;

    new_node->intgr = intgr;
    new_node->dbl = dbl;
    new_node->bln = bln;

    new_node->n_strings = 1;
    struct string_save *strings = (struct string_save *) malloc(new_node->n_strings * sizeof(struct string_save));

    struct string_save tmp;
    tmp.string_line_ptr = (uint64_t) str;
    tmp.size_of_string = strlen((char *) tmp.string_line_ptr);
    strings[0] = tmp;

    struct string_save *current = strings;

    current->string_line_ptr = write_file(file, (char *) (current->string_line_ptr), current->size_of_string);

    new_node->strings_array_ptr = write_file(file, strings, sizeof(struct string_save) * new_node->n_strings);
    free(strings);

    if (file_header->first_node_ptr == INVALID_PTR) {
        file_header->first_node_ptr = write_file(file, new_node, sizeof(struct node));
        file_header->last_node_ptr = file_header->first_node_ptr;
    } else {
        new_node->prev_ptr = file_header->last_node_ptr;
        struct node *last_node = read_file(file, file_header->last_node_ptr, sizeof(struct node));
        last_node->next_ptr = write_file(file, new_node, sizeof(struct node));
        file_header->last_node_ptr = last_node->next_ptr;

        fseek(file, new_node->prev_ptr, SEEK_SET);
        fwrite(last_node, 1, sizeof(struct node), file);
        free(last_node);
    }

    fseek(file, 0, SEEK_SET);
    fwrite(file_header, 1, sizeof(struct header), file);
    free(file_header);
    free(new_node);

    return CRUD_OK;
}

enum crud_status find_node_by_id(FILE *file, uint64_t id, struct node *node) {

    struct header *file_header = read_file(file, 0, sizeof(struct header));
    if (file_header->node_id == 0) return CRUD_ERROR;
    if (file_header->node_id < id) return CRUD_ERROR;
    uint64_t node_ptr = file_header->first_node_ptr;
    free(file_header);

    struct node *iter_node;
    while (node_ptr != INVALID_PTR) {
        iter_node = read_file(file, node_ptr, sizeof(struct node));
        if (iter_node->id == id) {
            node->id = iter_node->id;
            node->d = iter_node->d;
            node->nodes = iter_node->nodes;
            node->prev_ptr = iter_node->prev_ptr;
            node->next_ptr = iter_node->next_ptr;
            node->intgr = iter_node->intgr;
            node->dbl = iter_node->dbl;
            node->bln = iter_node->bln;
            node->n_strings = iter_node->n_strings;
            node->strings_array_ptr = iter_node->strings_array_ptr;
            free(iter_node);
            return CRUD_OK;
        }
        node_ptr = iter_node->next_ptr;
    }
    return CRUD_OK;
}

enum crud_status delete_node_by_id(FILE *file, uint64_t id) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));

    uint64_t address = get_ptr_by_id(file, id);
    struct node *delete_node = read_file(file, address, sizeof(struct node));

    if (address == INVALID_PTR) {
        free(file_header);
        return CRUD_ERROR;
    }

    // case 1: this is root node:
    if (file_header->first_node_ptr == address) {
        file_header->first_node_ptr = delete_node->next_ptr;
    }

    // case 2: this is the last element of linked list:
    if (file_header->last_node_ptr == address) {
        file_header->last_node_ptr = delete_node->prev_ptr;
    }

    // other:
    if (delete_node->prev_ptr != INVALID_PTR) {
        struct node *prev_node = read_file(file, delete_node->prev_ptr, sizeof(struct node));
        prev_node->next_ptr = delete_node->next_ptr;
        delete_from_file(file, delete_node->next_ptr, sizeof(struct node));

        fseek(file, delete_node->prev_ptr, SEEK_SET);
        fwrite(prev_node, 1, sizeof(struct node), file);

        free(prev_node);
    }

    if (delete_node->next_ptr != INVALID_PTR) {
        struct node *next_node = read_file(file, delete_node->next_ptr, sizeof(struct node));
        next_node->prev_ptr = delete_node->prev_ptr;
        delete_from_file(file, delete_node->next_ptr, sizeof(struct node));

        fseek(file, delete_node->next_ptr, SEEK_SET);
        fwrite(next_node, 1, sizeof(struct node), file);

        free(next_node);
    }

    uint64_t *nodes = read_file(file, delete_node->nodes, delete_node->d * sizeof(uint64_t));
    for (uint64_t i = 0; i < delete_node->d; i++) {
        struct node *tmp_node = read_file(file, nodes[i], sizeof(struct node));
        uint64_t *nodes_array = read_file(file, tmp_node->nodes, sizeof(uint64_t) * (tmp_node->d));

        tmp_node->d--;

        //check on INVALID ptrs
        if (!(tmp_node->d)) {
            tmp_node->nodes = INVALID_PTR;
            delete_from_file(file, tmp_node->nodes, sizeof(uint64_t) * (tmp_node->d + 1));
        } else {
            uint64_t *tmp_buf = (uint64_t *) malloc(sizeof(uint64_t) * tmp_node->d);
            uint64_t correct_pointer = 0;
            for (uint64_t j = 0; j < (tmp_node->d) + 1; j++) {
                if (j == correct_pointer) {
                    continue;
                } else {
                    tmp_buf[correct_pointer] = nodes_array[j];
                    correct_pointer++;
                }
            }

            delete_from_file(file, tmp_node->nodes, sizeof(uint64_t) * (tmp_node->d + 1));
            tmp_node->nodes = write_file(file, tmp_buf, (tmp_node->d) * sizeof(uint64_t));
            free(tmp_buf);
        }

        fseek(file, nodes[i], SEEK_SET);
        fwrite(tmp_node, 1, sizeof(struct node), file);

        free(tmp_node);
        free(nodes_array);
    }

    fseek(file, 0, SEEK_SET);
    fwrite(file_header, 1, sizeof(struct header), file);

    free(file_header);
    free(delete_node);

    return CRUD_OK;
}

enum crud_status delete_all(FILE *file) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    struct node *iter_node = read_file(file, file_header->last_node_ptr, sizeof(struct node));

    uint64_t idx = iter_node->id;

    for (uint64_t i = 1; i < idx + 1; i++) {
        delete_node_by_id(file, i);
    }

    file_header->node_id = 0;
    file_header->first_node_ptr = INVALID_PTR;
    file_header->last_node_ptr = INVALID_PTR;
    file_header->first_hole_ptr = INVALID_PTR;

    ftrunc(fileno(file), sizeof(struct header));
    fseek(file, 0, SEEK_SET);
    fwrite(file_header, 1, sizeof(struct header), file);

    free(file_header);
    free(iter_node);

    return CRUD_OK;
}

void append_to_node_list(struct node **node_to_add, uint64_t id, struct node_list **result) {

    if ((*result) == NULL) {
        (*result) = malloc(sizeof(struct node_list));
        (*result)->prev = NULL;

    } else {
        struct node_list *new_node_list = malloc(sizeof(struct node_list));
        new_node_list->prev = *result;
        *result = new_node_list;
    }

    (*result)->value = *node_to_add;
    (*result)->id = id;
    (*node_to_add) = malloc(sizeof(struct node));
}

enum crud_status find_all(FILE *file, struct node_list **node_list) {

    struct header *file_header = read_file(file, 0, sizeof(struct header));
    uint64_t node_ptr = file_header->last_node_ptr;
    free(file_header);

    struct node *iter_node;
    while (node_ptr != INVALID_PTR) {

        iter_node = read_file(file, node_ptr, sizeof(struct node));

        node_ptr = iter_node->prev_ptr;

        append_to_node_list(&iter_node, iter_node->id, node_list);

        free(iter_node);
    }

    return CRUD_OK;
}

enum crud_status connect_nodes(FILE *file, uint64_t id1, uint64_t id2) {

    uint64_t p1 = get_ptr_by_id(file, id1);
    uint64_t p2 = get_ptr_by_id(file, id2);

    if (p1 == INVALID_PTR || p2 == INVALID_PTR) {
        printf("{\n");
        printf("%sInvalid pointers\n", TABULATION);
        printf("}\n");
        return CRUD_ERROR;
    }

    struct node *node1 = read_file(file, p1, sizeof(struct node));
    struct node *node2 = read_file(file, p2, sizeof(struct node));

    uint64_t *nodes1 = read_file(file, node1->nodes, sizeof(uint64_t) * node1->d);
    uint64_t *nodes2 = read_file(file, node2->nodes, sizeof(uint64_t) * node2->d);

    //check for duplicates:
    for (uint64_t i = 0; i < node1->d; i++) {
        if (nodes1[i] == p2) {
            printf("{\n");
            printf("%sNodes are already connected\n", TABULATION);
            printf("}\n");
            return CRUD_ERROR;
        }
    }

    for (uint64_t i = 0; i < node2->d; i++) {
        if (nodes2[i] == p1) {
            printf("{\n");
            printf("%sNodes are already connected\n", TABULATION);
            printf("}\n");
            return CRUD_ERROR;
        }
    }
    //continue

    if (node1->d) {
        delete_from_file(file, node1->nodes, sizeof(uint64_t) * (node1->d));
    }
    if (node2->d) {
        delete_from_file(file, node2->nodes, sizeof(uint64_t) * (node2->d));
    }

    nodes1 = realloc(nodes1, sizeof(uint64_t) * (node1->d + 1));
    nodes2 = realloc(nodes2, sizeof(uint64_t) * (node2->d + 1));

    nodes2[node2->d] = p1;
    nodes1[node1->d] = p2;
    node1->d++;
    node2->d++;

    node1->nodes = write_file(file, nodes1, node1->d * sizeof(uint64_t));
    node2->nodes = write_file(file, nodes2, node2->d * sizeof(uint64_t));

    fseek(file, p1, SEEK_SET);
    fwrite(node1, 1, sizeof(struct node), file);

    fseek(file, p2, SEEK_SET);
    fwrite(node2, 1, sizeof(struct node), file);

    free(node1);
    free(node2);

    return CRUD_OK;
}


enum crud_status update_field_value_by_id(FILE *file, uint64_t id, uint64_t filed_name, uint64_t new_value) {

    uint64_t node_ptr = get_ptr_by_id(file, id);

    if (node_ptr == INVALID_PTR) {
        printf("{\n");
        printf("%sERROR, Node does not exist\n", TABULATION);
        printf("}\n");
        return CRUD_ERROR;
    }

    struct node *iter_node = read_file(file, node_ptr, sizeof(struct node));

    if (strcmp((char *) filed_name, "integer") == 0) {
        iter_node->intgr = new_value;
    } else if (strcmp((char *) filed_name, "double") == 0) {
        iter_node->dbl = new_value;
    } else if (strcmp((char *) filed_name, "boolean") == 0) {
        if (new_value == 1 || new_value == 0) {
            iter_node->bln = new_value;
        } else {
            return CRUD_ERROR;
        }
    } else if (strcmp((char *) filed_name, "string") == 0) {
        iter_node->strings_array_ptr = new_value;

        struct string_save *strings = (struct string_save *) malloc(iter_node->n_strings * sizeof(struct string_save));
        struct string_save tmp;
        tmp.string_line_ptr = (uint64_t) new_value;
        tmp.size_of_string = strlen((char *) tmp.string_line_ptr);
        strings[0] = tmp;

        struct string_save *current = strings;

        current->string_line_ptr = write_file(file, (char *) (current->string_line_ptr), current->size_of_string);

        iter_node->strings_array_ptr = write_file(file, strings, sizeof(struct string_save) * iter_node->n_strings);
        free(strings);
    } else {
        return CRUD_ERROR;
    }

    fseek(file, node_ptr, SEEK_SET);
    fwrite(iter_node, 1, sizeof(struct node), file);
    free(iter_node);
    return CRUD_OK;
}

enum crud_status
find_node_by_filed_value(FILE *file, uint64_t filed_name, uint64_t search_value, struct node_list **node_list) {

    struct header *file_header = read_file(file, 0, sizeof(struct header));
    uint64_t node_ptr = file_header->last_node_ptr;
    free(file_header);

    struct node *iter_node;
    while (node_ptr != INVALID_PTR) {

        iter_node = read_file(file, node_ptr, sizeof(struct node));

        node_ptr = iter_node->prev_ptr;

        if (strcmp((char *) filed_name, "integer") == 0) {
            if (search_value == iter_node->intgr) {
                append_to_node_list(&iter_node, iter_node->id, node_list);
            }
        } else if (strcmp((char *) filed_name, "double") == 0) {
            if (search_value == iter_node->dbl) {
                append_to_node_list(&iter_node, iter_node->id, node_list);
            }
        } else if (strcmp((char *) filed_name, "boolean") == 0) {
            if (search_value == iter_node->bln) {
                append_to_node_list(&iter_node, iter_node->id, node_list);
            }
        } else if (strcmp((char *) filed_name, "string") == 0) {
            struct string_save *strings = read_file(file, iter_node->strings_array_ptr,
                                                    sizeof(struct string_save) * iter_node->n_strings);
            if (strcmp(
                    (char *) search_value,
                    (char *) read_file(file, strings[0].string_line_ptr, strings[0].size_of_string)
            )) {
                free(strings);
                append_to_node_list(&iter_node, iter_node->id, node_list);
            }
        }
        free(iter_node);
    }

    return CRUD_OK;
}
