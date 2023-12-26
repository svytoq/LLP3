#include "../../include/parser/view.h"

void print_tab(int *count) {
    for (int i = 0; i < *count; i++) {
        printf("%s", TAB);
    }
    (*count)++;
}

void print_attribute(struct attribute *attribute, int *count_tab) {

    print_tab(count_tab);
    printf("Left operand: %s\n", attribute->left);
    print_tab(count_tab);
    printf("Condition: %s\n", attribute->condition);
    print_tab(count_tab);
    printf("Right operand: %s\n", attribute->right);

    if (attribute->next_attribute) {
        if (attribute->next_attribute->combined_condition) {
            print_tab(count_tab);
            printf("Combined condition: %s\n", attribute->next_attribute->combined_condition);
        }
        print_attribute(attribute->next_attribute, count_tab);
    }
}

void get_request_view(struct request *request) {
    int count_tab = 1;
    struct attribute *attribute = request->attributes;

    printf("Operation: %s\n", request->operation);

    print_attribute(attribute, &count_tab);
}



