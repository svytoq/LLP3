#include "../include/parser/parser.h"
#include "../include/parser/view.h"
#include "../include/net/client.h"

int main(int argc, char **argv) {

    int size_tree;
    xmlDocPtr request_tree;

    ////////////////////////////////////////////////////////////////

    FILE *file;
    file = fopen("request.txt", "r");
    char *req = calloc(MAX_REQUEST_SIZE, sizeof(char));
    fread(req, sizeof(char), MAX_REQUEST_SIZE, file);

    printf("Your request:\n%s\n\n", req);

    struct request *request = malloc(sizeof(struct request));
    enum parser_status status = parse_request(req, request);

    request_tree = xmlNewDoc(BAD_CAST "1.0");
    wrap_to_XML(request, request_tree);

    xmlChar *str_tree = (xmlChar *) malloc(sizeof(xmlChar) * MAX_REQUEST_SIZE);
    xmlDocDumpMemory(request_tree, &str_tree, &size_tree);

    printf("%s\n", (char *) str_tree);

    if (status == PARSE_OK) sendRequest(atoi(argv[1]), size_tree, (char *) str_tree);

    //////////////////////////////////////////////////////////////

    fclose(file);
    free(req);
    free(request);

    return 0;
}
