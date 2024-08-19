#include "assembler_stage_1.h"

/**
 * Processes the given file in the first stage of the assembler.
 *
 * This function reads the file, processes each line, and updates the instruction and data images.
 * It also handles labels, macros, and commands, and generates the necessary intermediate data structures.
 *
 * @param file_name The name of the file to process.
 * @param label_table A pointer to the array of labels.
 * @param instruction_image A pointer to the instruction image.
 * @param data_image A pointer to the data image.
 * @param macro_name_image A pointer to the macro name image.
 * @param error_flag A pointer to an integer that will be set to 1 if an error occurs, otherwise 0.
 * @return Returns 0 on success, otherwise returns 1 if an error occurs.
 */
int stage_1_process_file(const char *file_name, label_array *label_table, instruction_image *instruction_image, data_image *data_image, const macro_name_image *macro_name_image, int *error_flag) {
    int i, IC = 0, DC = 0, L = 0, ptr_location, first_word_in_line_length, label_flag, command_in_line, array_size, total_memory_size, line_counter = 0;
    int *parced_array = NULL;
    char line[MAX_LEGAL_LENGTH_OF_LINE + LENGTH_OF_NULL_TERMINATOR], label_header[MAX_LENGTH_OF_LABEL_HEADER], word_in_binary[SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR];
    char *ptr_in_line = NULL;
    const macro_name *macro_name = NULL;
    data_node *data_node = NULL;
    instruction_node *instruction_node = NULL;
    char *am_version = file_name_extender(file_name, ".am");
    FILE *am_extension_file = fopen(am_version, "r");
    if (am_extension_file == NULL) {
        printf("%s\n", ERROR_FAILED_TO_OPEN_FILE);
        free(am_version);
        am_version = NULL;
        return EXIT_FAILURE;
    }

    label_header[0] = NULL_TERMINATOR;
    while (fgets(line, MAX_ILLEGAL_LENGTH_OF_LINE, am_extension_file)) {
        total_memory_size = IC + DC + STARTING_POINT_OF_MEMORY; /* calculate the total memory size */
        if (total_memory_size >= MAX_SIZE_OF_MEMORY) {
            /* check if the memory usage surpassed memory size */
            *error_flag = TRUE;
            break;
        }
        line_counter++;
        if (is_line_length_overlimit(line)){ /*check if the line is longer than the max of 80 characters*/
            error_handler(ERROR_LINE_TOO_LONG, am_version, line_counter);
            *error_flag = TRUE;
            continue;
        }

        label_flag = FALSE;
        memset(label_header, NULL_TERMINATOR, sizeof(label_header));
        memset(word_in_binary, NULL_TERMINATOR, sizeof(word_in_binary));
        ptr_in_line = skip_whitespace(line);
        if (ptr_in_line[0] == SEMI_COLON) { /* if the line is a comment */
            error_handler(ERROR_COMMENT_NOT_AT_THE_BEGINNING_OF_THE_LINE, am_version, line_counter);
            *error_flag = TRUE;
            continue;
        }
        first_word_in_line_length = first_word_length_counter(ptr_in_line);
        ptr_location = line_location(ptr_in_line); /* check the location of the first word in the line */
        if(!is_location_valid(ptr_location, line_counter, am_version, error_flag)) /* check if the location is valid */
            continue;

        if (ptr_location == LABEL_DEFINITION) {
            label_flag = TRUE;
            macro_name = macro_name_image->first;
            strncpy(label_header, ptr_in_line, first_word_in_line_length); /* copy the label name to the label_header */
            validate_label_name(label_header, label_table, macro_name, am_version, line_counter, error_flag); /* validate the label name */
            ptr_in_line = skip_to_next_word(ptr_in_line, first_word_in_line_length + LENGTH_OF_COLON); /* skip the label name in the ptr*/
            if (is_end_of_line(ptr_in_line)) { /* if the line is empty after the label */
                error_handler(ERROR_LABEL_CANNOT_BE_EMPTY_OF_COMMAND, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            ptr_location = line_location(ptr_in_line); /* check again the location of the new first word in the line */
            if(!is_location_valid(ptr_location, line_counter, am_version, error_flag)) /* check if the location is valid */
                continue;
        }
        if (ptr_location == DATA || ptr_location == STRING) {
            if (label_flag == 1) /* if the line has a label */
                add_label_to_array(label_table, label_header, DC, ptr_location);
            if (ptr_location == DATA) {
                if (parse_dot_data(ptr_in_line, &parced_array, &array_size, &DC, am_version, line_counter)) {
                    *error_flag = TRUE;
                    continue;
                }
            }
            else {
                if (parse_dot_string(ptr_in_line, &parced_array, &array_size, &DC, am_version, line_counter)){
                    *error_flag = TRUE;
                    free(parced_array);
                    continue;
                }
            }
            data_node = new_data_node(ptr_in_line, array_size, parced_array);
            data_node_add(data_image, data_node);
            free(parced_array);
            parced_array = NULL;
        } else if (ptr_location == EXTERN) {
            ptr_in_line = skip_to_next_word(ptr_in_line, strlen(".extern")); /* skip the ".extern" command */
            if(ptr_in_line == NULL){
                error_handler(ERROR_NO_LABEL_ENTERED, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            first_word_in_line_length = first_word_length_counter(ptr_in_line);
            ptr_location = line_location(ptr_in_line); /* check the location of the first word in the line after skipping ".extern" command*/
            if (ptr_location == LABEL_DEFINITION) { /* if the location is a defining a new label with a command after it */
                error_handler(ERROR_LABEL_CANNOT_BE_DEFINED_IN_EXTERN_COMMAND, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            strncpy(label_header, ptr_in_line, first_word_in_line_length); /* copy the label name to the label_header */
            validate_label_name(label_header, label_table, macro_name, am_version, line_counter, error_flag); /* validate the label name */
            ptr_in_line = skip_to_next_word(ptr_in_line, first_word_in_line_length); /* skip the label name */

            if (!is_end_of_line(ptr_in_line)) { /* if there are characters after the label */
                error_handler(ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            add_label_to_array(label_table, label_header, EXTERN_ADDRESS, EXTERN);
        } else if (ptr_location == ENTRY) {
            ptr_in_line = skip_to_next_word(ptr_in_line, strlen(".entry")); /* skip the ".entry" command */
            if(ptr_in_line == NULL){
                error_handler(ERROR_NO_LABEL_ENTERED, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            ptr_location = line_location(ptr_in_line); /* check the location of the first word in the line after skipping ".entry" command*/
            if (ptr_location == LABEL_DEFINITION) { /* if the location is a defining a new label with a command after it */
                error_handler(ERROR_LABEL_CANNOT_BE_DEFINED_IN_ENTRY_COMMAND, am_version, line_counter);
                *error_flag = TRUE;
                continue;
            }
            first_word_in_line_length = first_word_length_counter(ptr_in_line);
            ptr_in_line[first_word_in_line_length] = NULL_TERMINATOR;
            for (i = 0; i < label_table->rep; i++) { /* check if the label is already defined as an extern */
                if (!strcmp(label_table->label_element[i].name, ptr_in_line)) {
                    if (label_table -> label_element[i].characteristic == EXTERN) {
                        error_handler(ERROR_LABEL_IS_EXTERN, am_version, line_counter);
                        *error_flag = TRUE;
                        break;
                    }
                }
            }
            ptr_in_line = skip_to_next_word(ptr_in_line, first_word_in_line_length);
            if (!is_end_of_line(ptr_in_line)) { /* if there are characters after the label */
                error_handler(ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL, am_version, line_counter);
                *error_flag = TRUE;
            }
        } else if (ptr_location == INSTRUCTION) {
            if (label_flag == 1)
                add_label_to_array(label_table, label_header, IC + STARTING_POINT_OF_MEMORY, IRRLEVANT);
            command_in_line = which_command(ptr_in_line); /* check which command is in the line */
            if (command_in_line == unknown_command) {
                error_handler(ERROR_COMMAND_NOT_FOUND, am_version, line_counter);
                *error_flag = TRUE;
            }
            else {
                if (analyze_command(ptr_in_line, command_in_line, &L, word_in_binary, am_version, line_counter))
                    *error_flag = TRUE;
            }
            instruction_node = new_instruction_node(ptr_in_line, L, word_in_binary);
            instruction_node_add(instruction_image, instruction_node);
            IC += L; /* update the IC */
            L = 0; /* reset the L */
        }
    }

    free(am_version);
    am_version = NULL;
    ptr_in_line = NULL;
    fclose(am_extension_file);
    if (*error_flag)
        return EXIT_FAILURE;
    for (i = 0; i < label_table->rep; i++) { /* update the addresses of the labels */
        if (label_table->label_element[i].characteristic == DATA || label_table->label_element[i].characteristic == STRING)
            label_table->label_element[i].address += (IC + STARTING_POINT_OF_MEMORY);
    }
    return EXIT_SUCCESS;
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
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
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
    if (strlen(label_header) > MAX_LENGTH_OF_LABEL_HEADER) { /* check if the label name is too long */
        error_handler(ERROR_LABEL_NAME_TOO_LONG, am_version, line_counter);
        *error_flag = TRUE;
    }
    if (is_reserved_word(label_header, strlen(label_header))) { /* check if the label name is a reserved word */
        error_handler(ERROR_LABEL_NAME_IS_A_RESERVED_WORD, am_version, line_counter);
        *error_flag = TRUE;
    }
    if (is_duplicate_label_name(label_table, label_header)) { /* check if the label name was already set up else where */
        error_handler(ERROR_DUPLICATE_LABEL_NAME, am_version, line_counter);
        *error_flag = TRUE;
    }
    while (macro_name != NULL) {
        if (strcmp(macro_name->name, label_header) == 0) { /* check if the label name is a macro name */
            error_handler(ERROR_LABEL_NAME_IS_A_MACRO_NAME, am_version, line_counter);
            *error_flag = TRUE;
        }
        macro_name = macro_name->next;
    }
}