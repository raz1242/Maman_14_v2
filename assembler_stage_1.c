#include "assembler_stage_1.h"

#include "assembler_stage_0.h"


/*
void printLabelTable(const label_array *array) { //- testing
    int i;
    printf("Label Table:\n");
    for (i = 0; i < array->rep; i++) {
        printf("Label %d: %s and the address is :%d\n", i + 1, array->label_element[i].name,
               array->label_element[i].address);
    }
}

void printDataImage(const data_image *image) { //-  testing
    const data_node *current = image->first;
    int i;

    printf("\n");
    while (current != NULL) {
        printf("Original line: %s", current->original_line);
        printf("Data: ");
        for (i = 0; i < current->length; i++) {
            //if (current->word[i] != 0)
            printf("%d ", current->char_in_ASCII[i]);
        }
        printf("\n\n");
        current = current->next_node;
    }
}

void printInstructionImage(const instruction_image *image) {  //- testing
    instruction_node *current = image->first;
    while (current != NULL) {
        printf("Original line: %s", current->original_line);
        printf("Binary representation: %s\n\n", current->word_command_in_binary);
        // Add more printf statements here if you need to print more fields
        current = current->next_node;

    }
}

void printExternLabels(const label_array *array) { //- testing
    int i;
    printf("Extern Labels:\n");
    for (i = 0; i < array->rep; i++) {
        if (array->label_element[i].characteristic == EXTERN) {
            printf("Label: %s, Address: %d\n", array->label_element[i].name, array->label_element[i].address);
        }
    }
}*/

int stage_1_process_file(const char *file_name, label_array *label_table, instruction_image *instruction_image, data_image *data_image, const macro_name_image *macro_name_image, int *error_flag) {
    int i, IC = 0, DC = 0, L = 0, location, first_word_in_line_length, label_flag, command, array_size, total_memory_size, line_counter = 0;
    int *parced_array = NULL;
    char line[MAX_LEGAL_LENGTH_OF_LINE + LENGTH_OF_NULL_TERMINATOR], label_header[MAX_LENGTH_OF_LABEL_HEADER], word_in_binary[SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR];
    char *ptr = NULL, *non_space_ptr = NULL;
    const macro_name *macro_name = NULL;
    data_node *data_node = NULL;
    instruction_node *instruction_node = NULL;
    char *am_version = file_name_extender(file_name, ".am");
    FILE *am_extension_file = fopen(am_version, "r");
    if (am_extension_file == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }

    label_header[0] = '\0';
    while (fgets(line, MAX_LEGAL_LENGTH_OF_LINE + LENGTH_OF_NULL_TERMINATOR, am_extension_file)) {
        total_memory_size = IC + DC + STARTING_POINT_OF_MEMORY;
        if (total_memory_size >= MAX_SIZE_OF_MEMORY) {
            printf("%s\n", ERROR_MEMORY_LIMIT_REACHED);
            exit(1);
        }
        line_counter++;
        label_flag = 0;
        memset(label_header, '\0', sizeof(label_header));
        memset(word_in_binary, '\0', sizeof(word_in_binary));
        non_space_ptr = skip_whitespace(line);
        ptr = non_space_ptr;
        if (ptr[0] == ';') {
            error_handler(ERROR_COMMENT_NOT_AT_THE_BEGINNING_OF_THE_LINE, am_version, line_counter);
            *error_flag = 1;
            continue;
        }
        first_word_in_line_length = first_word_length_counter(ptr);
        location = line_location(ptr);
        if (location == -1) {
            error_handler(ERROR_UNKNOWN_COMMAND, am_version, line_counter);
            *error_flag = 1;
        }

        if (location == LABEL_DEFINITION) {
            label_flag = 1;
            macro_name = macro_name_image->first;
            strncpy(label_header, ptr, first_word_in_line_length);
            validate_label_name(label_header, label_table, macro_name, am_version, line_counter, error_flag);
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length + LENGTH_OF_COLON);
            ptr = non_space_ptr;
            if (is_end_of_line(ptr)) {
                error_handler(ERROR_LABEL_CANNOT_BE_EMPTY_OF_COMMAND, am_version, line_counter);
                *error_flag = 1;
                continue;
            }
            location = line_location(ptr);
        }
        if (location == DATA || location == STRING) {
            if (label_flag == 1)
                add_label_to_array(label_table, label_header, DC, location);
            if (location == DATA) {
                if (parse_dot_data(ptr, &parced_array, &array_size, &DC, am_version, line_counter)) {
                    *error_flag = 1;
                    continue;
                }
            }
            else {
                if (parse_dot_string(ptr, &parced_array, &array_size, &DC, am_version, line_counter)){
                    *error_flag = 1;
                    continue;
                }
            }
            data_node = new_data_node(ptr, array_size, parced_array);
            data_node_add(data_image, data_node);
            free(parced_array);
            parced_array = NULL;
        } else if (location == EXTERN) {
            non_space_ptr = skip_to_next_word(ptr, strlen(".extern"));
            ptr = non_space_ptr;
            first_word_in_line_length = first_word_length_counter(ptr);
            location = line_location(ptr);
            if (location == LABEL_DEFINITION) {
                error_handler(ERROR_LABEL_CANNOT_BE_DEFINED_IN_EXTERN_COMMAND, am_version, line_counter);
                *error_flag = 1;
                continue;
            }
            strncpy(label_header, ptr, first_word_in_line_length);
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length);
            ptr = non_space_ptr;
            if (!is_end_of_line(ptr)) {
                error_handler(ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL, am_version, line_counter);
                *error_flag = 1;
                continue;
            }
            if (is_reserved_word(label_header, strlen(label_header))) {
                error_handler(ERROR_LABEL_NAME_IS_A_RESERVED_WORD, am_version, line_counter);
                *error_flag = 1;
            }
            add_label_to_array(label_table, label_header, EXTERN_ADDRESS, EXTERN);
        } else if (location == ENTRY) {
            non_space_ptr = skip_to_next_word(ptr, strlen(".entry"));
            ptr = non_space_ptr;
            first_word_in_line_length = first_word_length_counter(ptr);
            ptr[first_word_in_line_length] = '\0';
            location = line_location(ptr);
            if (location == LABEL_DEFINITION) {
                error_handler(ERROR_LABEL_CANNOT_BE_DEFINED_IN_ENTRY_COMMAND, am_version, line_counter);
                *error_flag = 1;
                continue;
            }
            for (i = 0; i < label_table->rep; i++) {
                if (!strcmp(label_table->label_element[i].name, ptr)) {
                    if (label_table -> label_element[i].characteristic == EXTERN) {
                        error_handler(ERROR_LABEL_IS_EXTERN, am_version, line_counter);
                        *error_flag = 1;
                        break;
                    }
                }
            }
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length);
            ptr = non_space_ptr;
            if (!is_end_of_line(ptr)) {
                error_handler(ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL, am_version, line_counter);
                *error_flag = 1;
            }
        } else if (location == INSTRUCTION) {
            if (label_flag == 1)
                add_label_to_array(label_table, label_header, IC + STARTING_POINT_OF_MEMORY, IRRLEVANT);
            command = which_command(ptr);
            if (command == unknown_command) {
                error_handler(ERROR_COMMAND_NOT_FOUND, am_version, line_counter);
                *error_flag = 1;
            }
            else {
                if (analyze_command(ptr, command, &L, word_in_binary, am_version, line_counter))
                    *error_flag = 1;
            }
            instruction_node = new_instruction_node(ptr, L, word_in_binary);
            instruction_node_add(instruction_image, instruction_node);
            IC += L;
            L = 0;
        }
    }

    free(am_version);
    am_version = NULL;
    ptr = NULL;
    non_space_ptr = NULL;
    fclose(am_extension_file);
    if (*error_flag)
        return 1;
    for (i = 0; i < label_table->rep; i++) {
        if (label_table->label_element[i].characteristic == DATA || label_table->label_element[i].characteristic == STRING)
            label_table->label_element[i].address += (IC + STARTING_POINT_OF_MEMORY);
    }

   /*printLabelTable(label_table); //- testing
    printDataImage(data_image); //- testing
    printInstructionImage(instruction_image); //- testing
    printExternLabels(label_table); //- testing*/

    return 0;
}




/**
 * Checks if a given label name already exists in the label array.
 *
 * This function iterates through the label array and compares each label's name
 * with the provided label name. Returns a number based on the match result.
 *
 * @param label_table A pointer to the label array to be checked.
 * @param label_name The name of the label to check for duplicates.
 * @return 1 if the label name is a duplicate, 0 otherwise.
 */
int is_duplicate_label_name(const label_array *label_table, const char *label_name) {
    int i;
    for (i = 0; i < label_table->rep; i++) {
        if (strcmp(label_table->label_element[i].name, label_name) == 0)
            return 1;
    }
    return 0;
}

/**
 * Validates a label name by checking its length, whether it is a reserved word,
 * and if it is a duplicate in the label table.
 *
 * @param label_header A pointer to the label name to be validated.
 * @param label_table A pointer to the label array containing existing labels.
 * @param macro_name A pointer to the macro name structure.
 * @param am_version The name of the assembly file being processed.
 * @param line_counter The current line number in the assembly file.
 * @param error_flag A pointer to an integer flag that indicates if an error has occurred.
 */
void validate_label_name(char *label_header, const label_array *label_table, const macro_name *macro_name, const char *am_version, const int line_counter, int *error_flag) {
    if (strlen(label_header) > MAX_LENGTH_OF_LABEL_HEADER) {
        error_handler(ERROR_LABEL_NAME_TOO_LONG, am_version, line_counter);
        *error_flag = 1;
    }
    if (is_reserved_word(label_header, strlen(label_header))) {
        error_handler(ERROR_LABEL_NAME_IS_A_RESERVED_WORD, am_version, line_counter);
        *error_flag = 1;
    }
    if (is_duplicate_label_name(label_table, label_header)) {
        error_handler(ERROR_DUPLICATE_LABEL_NAME, am_version, line_counter);
        *error_flag = 1;
    }
    while (macro_name != NULL) {
        if (strcmp(macro_name->name, label_header) == 0) {
            error_handler(ERROR_LABEL_NAME_IS_A_MACRO_NAME, am_version, line_counter);
            *error_flag = 1;
        }
        macro_name = macro_name->next;
    }
}