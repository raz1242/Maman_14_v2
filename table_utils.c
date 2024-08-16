#include "table_utils.h"

/**
 * Allocates memory for a instruction image and initializes its elements.
 *
 * @return A pointer to the allocated instruction image.
 *         Exits the program if memory allocation fails.
 */
instruction_image *instruction_image_allocator() {
    instruction_image *new_instruction_image = malloc(sizeof(instruction_image));
    if (new_instruction_image == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_instruction_image->first = NULL;
    new_instruction_image->last = NULL;

    return new_instruction_image;
}

/**
 * Creates a new instruction node with the given line, length, and binary word.
 *
 * @param line The original line of instruction.
 * @param L The length of the binary word.
 * @param word_in_binary The binary representation of the word.
 * @return A pointer to the newly created instruction node.
 *         Exits the program if memory allocation fails.
 */
instruction_node *new_instruction_node(const char *line, const int L, const char* word_in_binary) {
    instruction_node *new_node = malloc(sizeof(instruction_node));
    if (new_node == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_node->word_command_in_binary = (char*)malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if (new_node->word_command_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node);
        exit(1);
    }
    new_node->first_operand.word_in_binary = (char*)malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(new_node->first_operand.word_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word_command_in_binary);
        free(new_node);
        exit(1);
    }
    memset(new_node->first_operand.word_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    new_node->second_operand.word_in_binary = (char*)malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(new_node->second_operand.word_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word_command_in_binary);
        free(new_node->first_operand.word_in_binary);
        free(new_node);
        exit(1);
    }
    memset(new_node->second_operand.word_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    strncpy(new_node->word_command_in_binary, word_in_binary,SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    new_node->word_command_in_binary[SIZE_OF_REGISTER_IN_BITS] = '\0';
    new_node->original_line = (char *) malloc(strlen(line) + LENGTH_OF_NULL_TERMINATOR); // for testing
    if (new_node->original_line == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->word_command_in_binary);
        free(new_node->first_operand.word_in_binary);
        free(new_node->second_operand.word_in_binary);
        free(new_node);
        exit(1);
    }
    strcpy(new_node->original_line, line);
    new_node->length = L;

    new_node->next_node = NULL;
    return new_node;
}

/**
 * Adds a new instruction node to the instruction image.
 *
 * @param instruction_image A pointer to the instruction image where the node will be added.
 * @param new_node A pointer to the new instruction node to be added.
 */
void instruction_node_add(instruction_image* instruction_image, instruction_node *new_node) {
    if (instruction_image->first == NULL) {
        instruction_image->first = new_node;
        instruction_image->last = new_node;
    } else {
        instruction_image->last->next_node = new_node;
        instruction_image->last = new_node;
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
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_data_image->first = NULL;
    new_data_image->last = NULL;

    return new_data_image;
}

/**
 * Creates a new data node with the given line, command type, data array, and array size.
 *
 * @param line The original line of instruction.
 * @param array_size The size of the data array.
 * @param data The array of data.
 * @return A pointer to the newly created data node.
 *         Exits the program if memory allocation fails.
 */
data_node *new_data_node(const char *line, const int array_size, const int* data) {
    int i;
    data_node *new_node = malloc(sizeof(data_node));
    if (new_node == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_node->char_in_ASCII = (int *) malloc(array_size * sizeof(int));
    if (new_node->char_in_ASCII == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    for (i = 0; i < array_size; i++)
        new_node->char_in_ASCII[i] = data[i];

    new_node->original_line = (char *) malloc(strlen(line) + LENGTH_OF_NULL_TERMINATOR); // for testing
    if (new_node->original_line == NULL) { // for testing
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node->char_in_ASCII);  // for testing
        free(new_node); // for testing purposes
        exit(1); // for testing
    } // for testing
    strcpy(new_node->original_line, line); // for testing

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
 * Allocates memory for a macro name image and initializes its elements.
 *
 * @return A pointer to the allocated macro name image.
 *         Exits the program if memory allocation fails.
 */
macro_name_image *macro_name_image_allocator() {
    macro_name_image *new_macro_name_image = malloc(sizeof(macro_name_image));
    if (new_macro_name_image == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_macro_name_image->first = NULL;
    new_macro_name_image->last = NULL;

    return new_macro_name_image;
}

/**
 * Creates a new macro name node with the given name.
 *
 * @param name The name of the macro.
 * @return A pointer to the newly created macro name node.
 *         Exits the program if memory allocation fails.
 */
macro_name *new_macro_name(const char* name) {
    macro_name *new_node = malloc(sizeof(macro_name));
    if (new_node == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    new_node->name = (char *) malloc(strlen(name) + LENGTH_OF_NULL_TERMINATOR);
    if (new_node->name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(new_node);
        exit(1);
    }
    strcpy(new_node->name, name);

    new_node->next = NULL;
    return new_node;
}

/**
 * Adds a new macro name node to the macro name image.
 *
 * @param macro_name_image A pointer to the macro name image where the node will be added.
 * @param new_node A pointer to the new macro name node to be added.
 */
void macro_name_add(macro_name_image *macro_name_image, macro_name *new_node) {
    if (macro_name_image->first == NULL) {
        macro_name_image->first = new_node;
        macro_name_image->last = new_node;
    } else {
        macro_name_image->last->next = new_node;
        macro_name_image->last = new_node;
    }
}

/**
 * Converts ASCII values in a data node to their binary representation.
 *
 * @param data_node A pointer to the data node containing ASCII values to be converted.
 */
void convert_ascii_to_binary(data_node *data_node) {
    int i;
    char str[SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR];
    char* ascii_in_12bit_binary;
    data_node->word_in_binary = malloc(data_node->length * sizeof(char *));
    if (data_node->word_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    for (i = 0; i < data_node->length; i++) {
        memset(str, 0, SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
        if(data_node->char_in_ASCII[i] >= 0)
            strcpy(str, "000");
        else
            strcpy(str, "111"); /* negative number */
        ascii_in_12bit_binary = decimal_to_binary(data_node->char_in_ASCII[i]);
        strcat(str, ascii_in_12bit_binary);
        free(ascii_in_12bit_binary);
        data_node->word_in_binary[i] = (char *)malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
        if(data_node->word_in_binary[i] == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            exit(1);
        }
        strcpy(data_node->word_in_binary[i], str);
    }
}

/**
 * Writes the object file with the instruction and data images in octal format.
 *
 * @param file_name The name of the file to write the object instruction to.
 * @param data_image A pointer to the data image containing the data nodes.
 * @param instruction_image A pointer to the instruction image containing the instruction nodes.
 * @param IC The instruction counter.
 * @param DC The data counter.
 */
void ob_file_usher(const char* file_name, const data_image *data_image, const instruction_image *instruction_image, const int IC, const int DC) {
    int i, j = STARTING_POINT_OF_MEMORY;
    char *command_in_octal, *first_operand_in_octal, *second_operand_in_octal, *data_in_octal;
    const instruction_node *instruction_node = instruction_image->first;
    const data_node *data_node = data_image->first;
    char *file_OB = file_name_extender(file_name, ".ob");
    FILE *ob_extension_file  = fopen(file_OB, "w");
    file_inspector(ob_extension_file, file_OB);
    free(file_OB);
    fprintf(ob_extension_file, "%4d %d\n", IC, DC);

    while(instruction_node != NULL) {
        command_in_octal = binary_to_octal(instruction_node->word_command_in_binary);
        fprintf(ob_extension_file, "%04d %s\n", j++, command_in_octal);
        free(command_in_octal);
        if (instruction_node->length >= 2) {
            first_operand_in_octal = binary_to_octal(instruction_node->first_operand.word_in_binary);
            fprintf(ob_extension_file, "%04d %s\n", j++, first_operand_in_octal);
            free(first_operand_in_octal);
        }
        if (instruction_node->length == 3) {
            second_operand_in_octal = binary_to_octal(instruction_node->second_operand.word_in_binary);
            fprintf(ob_extension_file, "%04d %s\n", j++, second_operand_in_octal);
            free(second_operand_in_octal);
        }
        instruction_node = instruction_node->next_node;
    }
    while (data_node != NULL) {
        for (i = 0; i < data_node->length; i++) {
            data_in_octal = binary_to_octal(data_node->word_in_binary[i]);
            fprintf(ob_extension_file, "%04d %s\n", j++, data_in_octal);
            free(data_in_octal);
        }
        data_node = data_node->next_node;
    }
    fclose(ob_extension_file);
}

/**
 * Writes the external labels to a file.
 *
 * @param file_name The name of the file to write the external labels to.
 * @param instruction_image A pointer to the instruction image containing the instruction and its characteristics.
 */
void ext_file_usher(const char *file_name, const instruction_image *instruction_image) {
    const instruction_node *instruction_node = instruction_image->first;
    const char *TARGET_BINARY = "000000000000001";
    int max_label_length = 0, label_length;
    char *file_EXT = file_name_extender(file_name, ".ext");
    FILE *ext_extension_file = fopen(file_EXT, "w");
    if (ext_extension_file == NULL) {
        printf("%s %s\n", ERROR_FAILED_TO_OPEN_FILE, file_EXT);
        free(file_EXT);
        return;
    }
    file_inspector(ext_extension_file, file_EXT);

    while (instruction_node != NULL) {
        if (instruction_node->first_operand.word_in_binary[0] != '\0' &&
            (instruction_node->first_operand.word_in_binary[0] == '0' || instruction_node->first_operand.word_in_binary[0] == '1') &&
            strcmp(instruction_node->first_operand.word_in_binary, TARGET_BINARY) == 0) {
            label_length = strlen(instruction_node->first_operand.name);
            if (label_length > max_label_length) {
                max_label_length = label_length;
            }
            fprintf(ext_extension_file, "%-*s %04d\n", max_label_length, instruction_node->first_operand.name, instruction_node->decimal_address_in_machine + 1);
        }
        if (instruction_node->second_operand.word_in_binary[0] != '\0' &&
            (instruction_node->second_operand.word_in_binary[0] == '0' || instruction_node->second_operand.word_in_binary[0] == '1') &&
            strcmp(instruction_node->second_operand.word_in_binary, TARGET_BINARY) == 0) {
            label_length = strlen(instruction_node->second_operand.name);
            if (label_length > max_label_length) {
                max_label_length = label_length;
            }
            fprintf(ext_extension_file, "%-*s %04d\n", max_label_length, instruction_node->second_operand.name, instruction_node->decimal_address_in_machine + 2);
        }
        instruction_node = instruction_node->next_node;
    }
    fclose(ext_extension_file);
    free(file_EXT);
}

/**
 * Writes the entry labels to a file.
 *
 * @param file_name The name of the file to write the entry labels to.
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 */
void ent_file_usher(const char *file_name, const label_array *label_table) {
    int entry_index, max_label_length = 0, label_length;
    char *file_ENT = file_name_extender(file_name, ".ent");
    FILE *ent_extension_file = fopen(file_ENT, "w");
    if (ent_extension_file == NULL) {
        printf("%s %s\n", ERROR_FAILED_TO_OPEN_FILE, file_ENT);
        free(file_ENT);
        return;
    }
    for (entry_index = 0; entry_index < label_table->rep; entry_index++) {
        label_length = strlen(label_table->label_element[entry_index].name);
        if (label_length > max_label_length) {
            max_label_length = label_length;
        }
    }

    for (entry_index = 0; entry_index < label_table->rep; entry_index++) {
        if (label_table->label_element[entry_index].characteristic == ENTRY) {
            fprintf(ent_extension_file, "%-*s %04d\n", max_label_length, label_table->label_element[entry_index].name, label_table->label_element[entry_index].address);
        }
    }
    fclose(ent_extension_file);
    free(file_ENT);
}

/**
 * Frees the memory allocated for a data node and its elements.
 *
 * @param node A pointer to the data node to be freed.
 */
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

/**
 * Frees the memory allocated for a data image and its elements.
 *
 * @param image A pointer to the data image to be freed.
 */
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

/**
 * Frees the memory allocated for an operand.
 *
 * @param operand A pointer to the operand whose memory is to be freed.
 */
void free_operand(const operand *operand) {
    free(operand->name);
    free(operand->word_in_binary);
}

/**
 * Frees the memory allocated for an instruction node and its elements.
 *
 * @param node A pointer to the instruction node to be freed.
 */
void free_instruction_node(instruction_node *node) {
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

/**
 * Frees the memory allocated for an instruction image and its elements.
 *
 * @param instruction_image A pointer to the instruction image to be freed.
 */
void free_instruction_image(instruction_image *instruction_image) {
    instruction_node *current_node = instruction_image->first;
    instruction_node *next;
    while (current_node != NULL) {
        next = current_node->next_node;
        free_instruction_node(current_node);
        current_node = next;
    }
    free(instruction_image);
}

/**
 * Frees the memory allocated for a macro name image and its elements.
 *
 * @param macro_name_image A pointer to the macro name image to be freed.
 */
void free_macro_name_image(macro_name_image *macro_name_image) {
    macro_name *current = macro_name_image->first;
    macro_name *next;
    while (current != NULL) {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(macro_name_image);
}

/**
 * Allocates memory for a label array and initializes its elements.
 *
 * @param size The initial size of the label array.
 * @return A pointer to the allocated label array.
 *         Exits the program if memory allocation fails.
 */
label_array *label_array_allocator(const int size) {
    label_array *array = malloc(sizeof(label_array));
    array->label_element = (label *) malloc(size * sizeof(label));
    if (array->label_element == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    array->rep = 0;
    array->length = size;
    return array;
}

/**
 * Adds a new label to the label array.
 *
 * @param array A pointer to the label array.
 * @param name The name of the label to be added.
 * @param address The address of the label.
 * @param label_characteristic The characteristic of the label.
 * @return Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
void add_label_to_array(label_array *array, const char *name, const int address, const line_characteristic label_characteristic) {
    label *new_label;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        if (length_of_array == 0)
            length_of_array = 1;
        else
            length_of_array = (length_of_array) * 2;
        new_label = realloc(array->label_element, length_of_array * sizeof(label));
        if (new_label == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            exit(1);
        }
        array->label_element = new_label;
        array->length = length_of_array; /* Update the length in the array structure*/
    }
    array->label_element[number_of_reps].name = malloc(strlen(name) + LENGTH_OF_NULL_TERMINATOR);
    if (array->label_element[number_of_reps].name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    strcpy(array->label_element[number_of_reps].name, name);
    array->label_element[number_of_reps].address = address;
    array->label_element[number_of_reps].characteristic = label_characteristic;

    array->rep++;
}

/**
 * Checks if a given name exists in the label array.
 *
 * @param label_table A pointer to the label array.
 * @param name The name to be checked.
 * @return Returns 1 if the name exists in the label array, otherwise returns 0.
 */
int is_label(const label_array *label_table, const char *name) {
    int i;
    for (i = 0; i < label_table->rep; i++) {
        if (strcmp(name, label_table->label_element[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Converts a label operand to its binary representation.
 *
 * @param operand The operand containing the label to be converted.
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 * @return A string containing the binary representation of the label operand.
 *         Returns NULL if the label is not found or if memory allocation fails.
 */
char* label_operand_to_binary(const operand operand, const label_array *label_table) {
    int i, operand_address = UNKNOWN_ADDRESS;
    char *part_operand_address_in_binary, *full_operand_address_in_binary = malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(full_operand_address_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    memset(full_operand_address_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    for(i = 0; i < label_table->rep; i++) {
        if(strcmp(operand.name, label_table->label_element[i].name) == 0) {
            operand_address = label_table->label_element[i].address;
            break;
        }
    }
    if(operand_address == -1) {
        /* speical case for external type label */
        operand_address = 0;
        part_operand_address_in_binary = decimal_to_binary(operand_address);
        strcpy(full_operand_address_in_binary, part_operand_address_in_binary);
        strcat(full_operand_address_in_binary, "001");
        free(part_operand_address_in_binary);
        return full_operand_address_in_binary;
    }
    if(operand_address == UNKNOWN_ADDRESS)
        return NULL;

    part_operand_address_in_binary = decimal_to_binary(operand_address);
    strcpy(full_operand_address_in_binary, part_operand_address_in_binary);
    strcat(full_operand_address_in_binary, "010");
    free(part_operand_address_in_binary);
    return full_operand_address_in_binary;
}

/**
 * Converts the given operands to their binary representation.
 *
 * @param first_operand The first operand to be converted.
 * @param second_operand The second operand to be converted.
 * @param first_operand_in_binary A pointer to a string where the binary representation of the first operand will be stored.
 * @param second_operand_in_binary A pointer to a string where the binary representation of the second operand will be stored.
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 */
void convert_operands_to_binary(const operand first_operand, const operand second_operand, char **first_operand_in_binary, char **second_operand_in_binary, const label_array *label_table) {
    switch(first_operand.type) {
        case IMMEDIATE:
            *first_operand_in_binary = immediate_operand_to_binary(first_operand);
        break;
        case LABEL_VALUE:
            *first_operand_in_binary = label_operand_to_binary(first_operand, label_table);
        break;
        case REGISTER_PTR:
        case REGISTER:
            *first_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
        break;
        case UNKNOWN:
            break;
    }
    switch(second_operand.type) {
        case IMMEDIATE:
            *second_operand_in_binary = immediate_operand_to_binary(second_operand);
        break;
        case LABEL_VALUE:
            *second_operand_in_binary = label_operand_to_binary(second_operand, label_table);
        break;
        case REGISTER_PTR:
        case REGISTER:
            if(first_operand.type == REGISTER_PTR || first_operand.type == REGISTER) {

                break;
            }
        *second_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
        break;
        case UNKNOWN:
            break;
    }
}