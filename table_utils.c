#include "table_utils.h"

code_image *codeImageAllocator() {
    code_image *new_code_image = malloc(sizeof(code_image));
    if (new_code_image == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_code_image);
        exit(1);
    }
    new_code_image->first = NULL;
    new_code_image->last = NULL;

    return new_code_image;
}

void newCodeNode(code_image *code_image, code_node *new_node) {
    if (code_image->first == NULL) {
        code_image->first = new_node;
        code_image->last = new_node;
    } else {
        code_image->last->next = new_node;
        code_image->last = new_node;
    }
}

data_image *dataImageAllocator() {
    data_image *new_data_image = malloc(sizeof(data_image));
    if (new_data_image == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_data_image);
        exit(1);
    }
    new_data_image->first = NULL;
    new_data_image->last = NULL;

    return new_data_image;
}

data_node *newDataNode(const char *line, const int L, const int command_type, const int* data/*const int *string_data /*maybe should replace with char instead, const int *machine_code_data*/) {
    int i;
    data_node *new_node = (data_node *) malloc(sizeof(data_node));
    if (new_node == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node);
        exit(1);
    }
    new_node->word = (int *) malloc(L * sizeof(int));
    if (new_node->word == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word);
        exit(1);
    }
    if (command_type == 2 /*STRING*/)
        for (i = 0; i < L; i++)
            new_node->word[i] = data[i];

    new_node->original_line = (char *) malloc(strlen(line) + 1);
    if (new_node->original_line == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word);
        free(new_node);
        return NULL;
    }
    strcpy(new_node->original_line, line);

    new_node->L = L;
    new_node->next_node = NULL;
    return new_node;
}

void dataNodeAdd(data_image *data_image, data_node *new_node) {
    if (data_image->first == NULL) {
        data_image->first = new_node;
        data_image->last = new_node;
    } else {
        data_image->last->next_node = new_node;
        data_image->last = new_node;
    }
}

symbol_node* newSymboleNode(const char *symbol_name, const int local_address, const line_type line_type) {
    symbol_node *new_symbol = (symbol_node *) malloc(sizeof(symbol_node));
    if (new_symbol == NULL) {
         //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_symbol);
        exit(1);
    }

    /* Allocate memory for the symbol_name and copy the string */
    new_symbol->symbol_name = (char *) malloc(strlen(symbol_name) + 1);
    if (new_symbol->symbol_name == NULL) {
         //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_symbol);
        exit(1);
    }
    strcpy(new_symbol->symbol_name, symbol_name);

    new_symbol->symbol_address = local_address;
    new_symbol->line_type = line_type;
    new_symbol->next_symbol = NULL;
    return new_symbol;
}

symbol_table *symbolTableAllocator() {
    symbol_table *new_symbol_table = malloc(sizeof(symbol_table));
    if (new_symbol_table == NULL) {
         //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_symbol_table);
        exit(1);
    }
    new_symbol_table->first = NULL;
    new_symbol_table->last = NULL;

    return new_symbol_table;
}

int symbolAdd(symbol_table *table, const char *symbol_name, const int *address, const line_type line_type) {
    int i;
    symbol_node* current, *new_symbol;

    current = table->first;
    for(i = 0; current != NULL; current = current->next_symbol){
        if (strcmp(current->symbol_name, symbol_name) == 0) {
            return 0;
        }
    }
    if (line_type == EXTERN)
        new_symbol = newSymboleNode(symbol_name, EXTERN_ADDRESS, line_type);
    else
        new_symbol = newSymboleNode(symbol_name, *address, line_type);

    if (table->first == NULL) {
        table->first = new_symbol;
        table->last = new_symbol;
    } else {
        table->last->next_symbol = new_symbol;
        table->last = new_symbol;
    }
    return 1;
}