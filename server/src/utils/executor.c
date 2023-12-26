#include "../../include/utils/executor.h"

void get_text_node(FILE *file, struct node *node, char *response) {

    char *curr = calloc(sizeof(char), 256);

    uint64_t *nodes = read_file(file, node->nodes, sizeof(uint64_t) * node->d);

    char *neighbours = calloc(sizeof(char), 32);
    for (uint64_t i = 0; i < node->d; i++) {
        struct node *tmp_node = read_file(file, nodes[i], sizeof(struct node));
        char *n_id = calloc(sizeof(char), 4);
        snprintf(n_id, 16, "%lu ", tmp_node->id);
        strcat(neighbours, n_id);
        free(tmp_node);
    }

    double dbl;
    memcpy(&dbl, &node->dbl, sizeof(node->dbl));

    struct string_save *strings = read_file(file, node->strings_array_ptr,
                                            sizeof(struct string_save) * node->n_strings);

    char *str = (char *) read_file(file, strings[0].string_line_ptr, strings[0].size_of_string);

    snprintf(curr, MAX_RESPONSE_SIZE,
             "{\n\tid : %lu\n\tneighbour's id's : %s\n\tinteger : %lu\n\tdouble : %.2f\n\tboolean : %lu\n\tstring : %s\n}\n",
             node->id,
             neighbours,
             node->intgr,
             dbl,
             node->bln,
             str
    );

    strcat(response, curr);

    free(strings);
}

void get_text_node_list(FILE *file, struct node_list *list, char *response) {
    if (list != NULL) {
        struct node_list *next;
        while (list != NULL) {
            next = list->prev;
            get_text_node(file, list->value, response);
            list = next;
        }
    }
}

void execute_request(char xml_request[], FILE *file, int fd) {

    xmlDocPtr request_tree = xmlReadMemory(xml_request, MAX_REQUEST_SIZE, 0, NULL, XML_PARSE_RECOVER);
    xmlNodePtr lastNode = request_tree->last;

    while (lastNode->last != NULL) {
        lastNode = lastNode->last;
    }

    char *response = calloc(MAX_RESPONSE_SIZE, sizeof(char));

    if (xmlStrEqual(request_tree->last->name, xmlCharStrdup("create"))) {

        uint64_t intgr = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("intgr")));
        uint64_t dbl;
        double d = atof((char *) xmlGetProp(lastNode, xmlCharStrdup("dbl")));
        memcpy(&dbl, &d, sizeof(d));
        uint64_t bln = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("bln")));
        uint64_t str = (uint64_t)((char *) xmlGetProp(lastNode, xmlCharStrdup("str")));

        enum crud_status status = create_node(file, intgr, dbl, bln, str);

        if (status == CRUD_OK) response = "Node created";
        else response = "Not successful";


    } else if (xmlStrEqual(request_tree->last->name, xmlCharStrdup("delete"))) {

        if (xmlStrEqual(xmlGetProp(lastNode, xmlCharStrdup("id")), xmlCharStrdup("*"))) {

            enum crud_status status = delete_all(file);

            if (status == CRUD_OK) response = "Nodes deleted";
            else response = "Not successful";
        } else {

            uint64_t id = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("id")));

            enum crud_status status = delete_node_by_id(file, id);

            if (status == CRUD_OK) response = "Node deleted";
            else response = "Not successful";

        }
    } else if (xmlStrEqual(request_tree->last->name, xmlCharStrdup("update"))) {

        uint64_t id = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("id")));
        uint64_t new_value = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("intgr")));

        enum crud_status status = update_field_value_by_id(file, id, (uint64_t) "integer", new_value);

        if (status == CRUD_OK) response = "Node updated";
        else response = "Not successful";

    } else if (xmlStrEqual(request_tree->last->name, xmlCharStrdup("connect"))) {

        uint64_t id_1 = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("id_1")));
        uint64_t id_2 = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("id_2")));

        enum crud_status status = connect_nodes(file, id_1, id_2);

        if (status == CRUD_OK) response = "Node connected";
        else response = "Not successful";

    } else if (xmlStrEqual(request_tree->last->name, xmlCharStrdup("find"))) {

        if (xmlStrEqual(xmlGetProp(lastNode, xmlCharStrdup("intgr")), xmlCharStrdup("10"))) {

            uint64_t intgr = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("intgr")));

            struct node_list *node_list = NULL;
            enum crud_status status = find_node_by_filed_value(file, (uint64_t) "integer", intgr, &node_list);

            if (status == CRUD_OK) get_text_node_list(file, node_list, response);
            else response = "Not successful";

        } else if (xmlStrEqual(xmlGetProp(lastNode, xmlCharStrdup("id")), xmlCharStrdup("*"))) {

            struct node_list *node_list = NULL;
            enum crud_status status = find_all(file, &node_list);

            if (status == CRUD_OK) get_text_node_list(file, node_list, response);
            else response = "Not successful";

        } else {

            uint64_t id = (uint64_t) atoi((char *) xmlGetProp(lastNode, xmlCharStrdup("id")));

            struct node *node = malloc(sizeof(struct node));;
            enum crud_status status = find_node_by_id(file, id, node);

            if (status == CRUD_OK) get_text_node(file, node, response);
            else response = "Not successful";
        }
    }

    send_response(response, fd);
}