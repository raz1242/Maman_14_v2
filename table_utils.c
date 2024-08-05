#include "table_utils.h"

/**
 * Allocates memory for a code image and initializes its elements.
 *
 * @return A pointer to the allocated code image.
 *         Exits the program if memory allocation fails.
 */
code_image *code_image_allocator() {
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
code_node *new_code_node(const char *line, const int L, const char* word_in_binary) {
    code_node *new_node = (code_node*) malloc(sizeof(code_node));
    if (new_node == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_node->word_command_in_binary = (char*)malloc(16); // change to #define from a constant
    if (new_node->word_command_in_binary == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node);
        exit(1);
    }
    new_node->first_operand.word_in_binary = (char*)malloc(16);// change to #define from a constant
    if(new_node->first_operand.word_in_binary == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node->word_command_in_binary);
        free(new_node);
        exit(1);
    }
    new_node->second_operand.word_in_binary = (char*)malloc(16);// change to #define from a constant
    if(new_node->second_operand.word_in_binary == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node->word_command_in_binary);
        free(new_node->first_operand.word_in_binary);
        free(new_node);
        exit(1);
    }
    strncpy(new_node->word_command_in_binary, word_in_binary,16);// change to #define from a constant
    new_node->word_command_in_binary[15] = '\0';
    new_node->original_line = (char *) malloc(strlen(line) + 1); // for testing
    if (new_node->original_line == NULL) {
        printf("Failed to allocate memory\n");
        free(new_node->word_command_in_binary);
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
void code_node_add(code_image* code_image, code_node *new_node) {
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
data_image *data_image_allocator() {
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
 * @param array_size The size of the data array.
 * @param data The array of data.
 * @return A pointer to the newly created data node.
 *         Exits the program if memory allocation fails.
 */
data_node *new_data_node(const char *line, const int array_size, const int* data) {
    int i;
    data_node *new_node = (data_node *) malloc(sizeof(data_node));
    if (new_node == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    new_node->char_in_ASCII = (int *) malloc(array_size * sizeof(int));
    if (new_node->char_in_ASCII == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    for (i = 0; i < array_size; i++)
        new_node->char_in_ASCII[i] = data[i];

    new_node->original_line = (char *) malloc(strlen(line) + 1); // mainly for testing purposes
    if (new_node->original_line == NULL) { // mainly for testing purposes
        printf("Failed to allocate memory\n");
        free(new_node->char_in_ASCII);  // mainly for testing purposes
        free(new_node); // mainly for testing purposes
        return NULL; // mainly for testing purposes
    } // mainly for testing purposes
    strcpy(new_node->original_line, line); // mainly for testing purposes

    new_node->length = array_size;
    new_node->next_node = NULL;
    return new_node;
}

/**
 * Adds a new data node to the data image.
 *
 * @param data_image A pointer to the data image where the node will be added.
 * @param new_node A pointer to the new data node to be added.
 */
void data_node_add(data_image *data_image, data_node *new_node) {
    if (data_image->first == NULL) {
        data_image->first = new_node;
        data_image->last = new_node;
    } else {
        data_image->last->next_node = new_node;
        data_image->last = new_node;
    }
}

/**
 * Converts ASCII values in a data node to their binary representation.
 *
 * @param data_node A pointer to the data node containing ASCII values to be converted.
 */
void convert_ascii_to_binary(data_node *data_node) {
    int i;
    char str[16]; // Buffer to hold the 15-bit binary string + null-terminator
    data_node->word_in_binary = malloc(data_node->length * sizeof(char *));
    if (data_node->word_in_binary == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }
    for (i = 0; i < data_node->length; i++) {
        memset(str, 0, 16);
        if(data_node->char_in_ASCII[i] >= 0)
            strcpy(str, "000");
        else
            strcpy(str, "111");
        strcat(str, decimal_to_binary(data_node->char_in_ASCII[i])); // retruns 12 bit binary number
        data_node->word_in_binary[i] = (char *)malloc(16);
        strcpy(data_node->word_in_binary[i], str);
    }
}

/**
 * Writes the object file with the code and data images in octal format.
 *
 * @param file_name The name of the file to write the object code to.
 * @param data_image A pointer to the data image containing the data nodes.
 * @param code_image A pointer to the code image containing the code nodes.
 * @param IC The instruction counter.
 * @param DC The data counter.
 */
void ob_file_usher(const char* file_name, const data_image *data_image, const code_image *code_image, const int IC, const int DC) {
    int i, j = STARTING_POINT_OF_MEMORY;
    const code_node *code_node = code_image->first;
    const data_node *data_node = data_image->first;
    const char *file_OB = file_name_extender(file_name, ".ob");
    FILE *ob_extension  = fopen(file_OB, "w");
    file_inspector(ob_extension, file_OB);
    fprintf(ob_extension, "%d %d\n", IC, DC);

    while(code_node != NULL) { // for testing - maybe think on a smarter way instead of while(1)
        fprintf(ob_extension, "%04d %s\n", j++, binary_to_octal(code_node->word_command_in_binary));
        if (code_node->length >= 2)
            fprintf(ob_extension, "%04d %s\n", j++, binary_to_octal(code_node->first_operand.word_in_binary));
        if (code_node->length == 3)
            fprintf(ob_extension, "%04d %s\n", j++, binary_to_octal(code_node->second_operand.word_in_binary));
        code_node = code_node->next_node;
    }
    while (data_node != NULL) {// for testing - maybe think on a smarter way instead of while(1)
        for (i = 0; i < data_node->length; i++) {
            fprintf(ob_extension, "%04d %s\n", j++, binary_to_octal(data_node->word_in_binary[i]));
        }
        data_node = data_node->next_node;
    }
    fclose(ob_extension);
}

/**
 * Writes the external labels to a file.
 *
 * @param file_name The name of the file to write the external labels to.
 * @param code_image A pointer to the code image containing the code and its characteristics.
 */
void  ext_file_usher(const char *file_name, const code_image *code_image) {
    char *file_EXT = file_name_extender(file_name, ".ext");
    FILE *ext_extension = fopen(file_EXT, "w");
    file_inspector(ext_extension, file_EXT);
    const code_node *code_node = code_image->first;
    while(code_node != NULL) {
        if(code_node->first_operand.word_in_binary) {
            if(strcmp(code_node->first_operand.word_in_binary, "000000000000001") == 0) {
                fprintf(ext_extension, "%s %04d\n", code_node->first_operand.name, code_node->decimal_address_in_machine + 1);
            }
        }
        if(code_node->second_operand.word_in_binary) {
            if(strcmp(code_node->second_operand.word_in_binary, "000000000000001") == 0) {
                fprintf(ext_extension, "%s %04d\n", code_node->second_operand.name, code_node->decimal_address_in_machine + 2);
            }
        }
        code_node = code_node->next_node;
    }
    fclose(ext_extension);
    free(file_EXT);
}

/**
 * Writes the entry labels to a file.
 *
 * @param file_name The name of the file to write the entry labels to.
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 */
void ent_file_usher(const char *file_name, const label_array *label_table) {
    int entry_index = 0;
    char *file_ENT = file_name_extender(file_name, ".ent");
    FILE *ent_extension = fopen(file_ENT, "w");
    if (ent_extension == NULL) {
        printf("Error opening file\n");
        free(file_ENT);
        return;
    }
    while (entry_index < label_table->rep) {
        if (label_table->label_element[entry_index].characteristic == ENTRY)
            fprintf(ent_extension, "%s %d\n", label_table->label_element[entry_index].name, label_table->label_element[entry_index].address);
        entry_index++;
    }
    fclose(ent_extension);
    free(file_ENT);
}


void free_data_node(data_node *node) {
    int i;
    if (node != NULL) {
        if (node->original_line != NULL) {
            free(node->original_line);
        }
        if (node->char_in_ASCII != NULL) {
            free(node->char_in_ASCII);
        }
        if (node->word_in_binary != NULL) {
            for (i = 0; i < node->length; i++) {
                if (node->word_in_binary[i] != NULL) {
                    free(node->word_in_binary[i]);
                }
            }
            free(node->word_in_binary);
        }
        free(node);
    }
}

void free_data_image(data_image *image) {
    data_node *current = image->first;
    data_node *next;
    while (current != NULL) {
        next = current->next_node;
        free_data_node(current);
        current = next;
    }
    free(image);
}

void free_operand(const operand *operand) {
    if (operand != NULL) {
        if (operand->name != NULL) {
            free(operand->name);
        }
        if (operand->word_in_binary != NULL) {
            free(operand->word_in_binary);
        }
        //free(operand);
    }
}

void free_code_node(code_node *node) {
    if (node != NULL) {
        if (node->original_line != NULL) {
            free(node->original_line);
        }
        if (node->word_command_in_binary != NULL) {
            free(node->word_command_in_binary);
        }
        free_operand(&node->first_operand);
        free_operand(&node->second_operand);
        free(node);
    }
}

void free_code_image(code_image *code_image) {
    code_node *current_node = code_image->first;
    code_node *next;
    while (current_node != NULL) {
        next = current_node->next_node;
        free_code_node(current_node);
        current_node = next;
    }
    free(code_image);
}
