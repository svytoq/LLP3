#include "../../include/parser/parser.h"

void print_error() {
    printf("The request is incorrectly written\n");
    exit(EXIT_FAILURE);
}

void check_path(char *req, int *path_length) {
    int bracket = 0;

    for (int i = 0; i < *path_length; i++) {
        if (req[i] == '{') bracket++;
        if (req[i] == '}') bracket--;
    }

    if (bracket != 0) {
        print_error();
    }
}

void remove_char(int *size, char **request_path) {
    (*size)--;
    (*request_path)++;
}

void new_line(char **req, int *path_length) {
    int i;
    for (i = 0; i < *path_length; i++) {
        if ((*req)[i] == '\n') break;
    }
    i++;
    for (; i < *path_length; i++) {
        if ((*req)[i] != ' ') break;
    }

    for (int j = 0; j < i; j++) {
        remove_char(path_length, req);
    }
}

char *read_word(char **req, int *path_length) {

    char *word = calloc(MAX_STRING_SIZE, sizeof(char));

    int i;
    for (i = 0; i < *path_length; i++) {
        if ((*req)[i] == ' ' || (*req)[i] == '\n' || (*req)[i] == ',' || (*req)[i] == '\r') {
            if ((*req)[i] == ',') i--;
            break;
        }
        word[i] = (*req)[i];
    }
    i++;

    for (int j = 0; j < i; j++) {
        remove_char(path_length, req);
    }
    return word;
}

void read_attributes(char **req, int *path_length, struct attribute *attribute) {

    if ((*req)[0] != '|' && (*req)[0] != '&') {
        char *left = read_word(req, path_length);
        char *condition = read_word(req, path_length);
        char *right = read_word(req, path_length);

        attribute->left = left;
        attribute->condition = condition;
        attribute->right = right;
    }

    if ((*req)[0] == ',') {

        new_line(req, path_length);
        struct attribute *new_attribute = malloc(sizeof(struct attribute));
        read_attributes(req, path_length, new_attribute);
        attribute->next_attribute = new_attribute;
    } else if ((*req)[0] == '|' || (*req)[0] == '&') {

        char *combined_condition = read_word(req, path_length);
        attribute->combined_condition = combined_condition;
        new_line(req, path_length);
        read_attributes(req, path_length, attribute);
    }

    remove_char(path_length, req);
}

enum parser_status parse_request(char *req, struct request *request) {

    int path_length = strlen(req);

    check_path(req, &path_length);

    char *query = read_word(&req, &path_length);
    if (strcmp("query", query) != 0) print_error();

    new_line(&req, &path_length);

    char *operation = read_word(&req, &path_length);
    if (!(
            strcmp("create", operation) == 0 ||
            strcmp("find", operation) == 0 ||
            strcmp("delete", operation) == 0 ||
            strcmp("update", operation) == 0 ||
            strcmp("connect", operation) == 0
    ))
        print_error();

    request->operation = operation;

    new_line(&req, &path_length);

    struct attribute *attribute = malloc(sizeof(struct attribute));
    request->attributes = attribute;
    read_attributes(&req, &path_length, attribute);

    new_line(&req, &path_length);
    new_line(&req, &path_length);

    return PARSE_OK;
}



void pack_attribute(struct request *request, xmlDocPtr request_tree) {

    struct attribute *attribute = request->attributes;

    xmlNodePtr node = xmlNewChild(request_tree->last, NULL, BAD_CAST "node", NULL);

    int i = 1, j = 1;
    while (attribute) {

        if (attribute->combined_condition) {
            char *comb_cond = calloc(sizeof(char), 16);
            snprintf(comb_cond, 16, "comb_cond%d", j++);
            xmlNewProp(node, BAD_CAST comb_cond, BAD_CAST attribute->combined_condition);
        }

        xmlNewProp(node, BAD_CAST attribute->left, BAD_CAST attribute->right);

        char *operand = calloc(sizeof(char), 16);
        snprintf(operand, 16, "operand_%d", i++);
        xmlNewProp(node, BAD_CAST operand, BAD_CAST attribute->condition);

        attribute = attribute->next_attribute;
    }
}

enum wrapper_status wrap_to_XML(struct request *request, xmlDocPtr request_tree) {

    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST request->operation);
    xmlDocSetRootElement(request_tree, root);

    pack_attribute(request, request_tree);

    return WRAP_OK;
}