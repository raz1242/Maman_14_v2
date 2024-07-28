#include "table_utils.h"

/**
 * Allocates memory for a code image and initializes its elements.
 *
 * @return A pointer to the allocated code image.
 *         Exits the program if memory allocation fails.
 */
code_image *codeImageAllocator() {
    code_image *new_code_image = (code_image*)malloc(sizeof(code_image));
    if (new_code_image == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_code_image->first = NULL;
    new_code_image->last = NULL;

    return new_code_image;
}

/**
 * Creates a new code node with the given line, length, and binary word.
 *
 * @param line The original line of code.
 * @param L The length of the binary word.
 * @param word_in_binary The binary representation of the word.
 * @return A pointer to the newly created code node.
 *         Exits the program if memory allocation fails.
 */
code_node *newCodeNode(const char *line, const int L, const char* word_in_binary) {
    code_node *new_node = (code_node*) malloc(sizeof(code_node));
    if (new_node == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_node->word = (char*)malloc(L * 16);
    if (new_node->word == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node);
        exit(1);
    }
    strncpy(new_node->word, word_in_binary,16);
    new_node->word[16] = '\0';
    new_node->original_line = (char *) malloc(strlen(line) + 1);
    if (new_node->original_line == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node->word);
        free(new_node);
        return NULL;
    }
    strcpy(new_node->original_line, line);

    new_node->length = L;
    new_node->next_node = NULL;
    return new_node;
}

/**
 * Adds a new code node to the code image.
 *
 * @param code_image A pointer to the code image where the node will be added.
 * @param new_node A pointer to the new code node to be added.
 */
void codeNodeAdd(code_image* code_image, code_node *new_node) {
    if (code_image->first == NULL) {
        code_image->first = new_node;
        code_image->last = new_node;
    } else {
        code_image->last->next_node = new_node;
        code_image->last = new_node;
    }
}

/**
 * Allocates memory for a data image and initializes its elements.
 *
 * @return A pointer to the allocated data image.
 *         Exits the program if memory allocation fails.
 */
data_image *dataImageAllocator() {
    data_image *new_data_image = malloc(sizeof(data_image));
    if (new_data_image == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_data_image->first = NULL;
    new_data_image->last = NULL;

    return new_data_image;
}

/**
 * Creates a new data node with the given line, command type, data array, and array size.
 *
 * @param line The original line of code.
 * @param data The array of data.
 * @param array_size The size of the data array.
 * @return A pointer to the newly created data node.
 *         Exits the program if memory allocation fails.
 */
data_node *newDataNode(const char *line, const int* data, const int array_size) {
    int i;
    data_node *new_node = (data_node *) malloc(sizeof(data_node));
    if (new_node == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_node->word = (int *) malloc(array_size * sizeof(int));
    if (new_node->word == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    for (i = 0; i < array_size; i++)
        new_node->word[i] = data[i];

    new_node->original_line = (char *) malloc(strlen(line) + 1); // mainly for testing purposes
    if (new_node->original_line == NULL) { // mainly for testing purposes
        printf("Failed to allocate memory\n");
        free(new_node->word);  // mainly for testing purposes
        free(new_node); // mainly for testing purposes
        return NULL; // mainly for testing purposes
    } // mainly for testing purposes
    strcpy(new_node->original_line, line); // mainly for testing purposes

    new_node->length = array_size;
    new_node->next_node = NULL;
    return new_node;
}
/**
 *
 * Adds a new data node to the data image.
 *
 * @param data_image A pointer to the data image where the node will be added.
 * @param new_node A pointer to the new data node to be added.
 */
void dataNodeAdd(data_image *data_image, data_node *new_node) {
    if (data_image->first == NULL) {
        data_image->first = new_node;
        data_image->last = new_node;
    } else {
        data_image->last->next_node = new_node;
        data_image->last = new_node;
    }
}
