#include "table_utils.h"

code_image *codeImageAllocator() {
    code_image *new_code_image = (code_image*)malloc(sizeof(code_image));
    if (new_code_image == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_code_image->first = NULL;
    new_code_image->last = NULL;

    return new_code_image;
}

code_node *newCodeNode(const char *line, const int L, const char* word_in_binary) {
    code_node *new_node = (code_node*) malloc(sizeof(code_node));
    if (new_node == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_node->word = (char*)malloc(L * 16);
    if (new_node->word == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node);
        exit(1);
    }
    strncpy(new_node->word, word_in_binary,16);
    new_node->word[16] = '\0';
    new_node->original_line = (char *) malloc(strlen(line) + 1);
    if (new_node->original_line == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word);
        free(new_node);
        return NULL;
    }
    strcpy(new_node->original_line, line);

    new_node->length = L;
    new_node->next_node = NULL;
    return new_node;
}

void codeNodeAdd(code_image* code_image, code_node *new_node) {
    if (code_image->first == NULL) {
        code_image->first = new_node;
        code_image->last = new_node;
    } else {
        code_image->last->next_node = new_node;
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

data_node *newDataNode(const char *line, const int command_type, const int** data, const int array_size) {
    int i;
    data_node *new_node = (data_node *) malloc(sizeof(data_node));
    if (new_node == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node);
        exit(1);
    }
    new_node->word = (int *) malloc(array_size * sizeof(int));
    if (new_node->word == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word);
        exit(1);
    }
    for (i = 0; i < array_size; i++)
        new_node->word[i] = (*data)[i];

    new_node->original_line = (char *) malloc(strlen(line) + 1);
    if (new_node->original_line == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word);
        free(new_node);
        return NULL;
    }
    strcpy(new_node->original_line, line);

    new_node->length = array_size;
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
