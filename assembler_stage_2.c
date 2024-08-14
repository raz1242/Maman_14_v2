#include "assembler_stage_2.h"


/**
 * Prints all labels with their characteristics.
 *
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 */
void printLabels(const label_array *label_table) {
    int i;
    printf("\n");
    for (i = 0; i < label_table->rep; i++) {
        if (label_table->label_element[i].characteristic == ENTRY)
            printf("Label: %s, Characteristic: ENTRY\n", label_table->label_element[i].name);
        if (label_table->label_element[i].characteristic == EXTERN)
            printf("Label: %s, Characteristic: EXTERN\n", label_table->label_element[i].name);
        else
            printf("Label: %s, Characteristic: IRRLEVANT\n", label_table->label_element[i].name);
    }
}

int stage_2_process_file(const char *file_name, const label_array *label_table, const code_image *code_image,
                         const data_image *data_image, int *error_flag) {
    int i, IC = 0, DC = 0, location, first_word_in_line_length, command_in_line, label_match_found, line_counter = 0, extern_flag = 0
            , entry_flag = 0;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL, *first_operand_in_binary = NULL, *second_operand_in_binary = NULL;
    FILE *am_extension_file = NULL;

    code_node *code_node = code_image->first;
    data_node *data_node = data_image->first;
    am_version = file_name_extender(file_name, ".am");
    am_extension_file = fopen(am_version, "r");
    // fix naming of the files in all stages for consistency, also in at the function at the end
    if (am_extension_file == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension_file)) {
        line_counter++;
        label_match_found = 0;
        non_space_ptr = skip_whitespace(line);
        ptr = non_space_ptr;
        first_word_in_line_length = first_word_length_counter(ptr);
        location = line_location(ptr);
        if (location == LABEL_DEFINITION) {
            strncpy(label_header, ptr, first_word_in_line_length);
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length + LENGTH_OF_COLON);
            ptr = non_space_ptr;
            first_word_in_line_length = first_word_length_counter(ptr);
            location = line_location(ptr);
        }
        if (location == ENTRY) {
            entry_flag = 1;
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length);
            ptr = non_space_ptr;
            first_word_in_line_length = first_word_length_counter(ptr);
            for (i = 0; i < label_table->rep; i++) {
                if (!strncmp(label_table->label_element[i].name, ptr, first_word_in_line_length)) {
                    label_match_found = 1;
                    label_table->label_element[i].characteristic = ENTRY;
                    break;
                }
            }
            if (!label_match_found) {
                error_handler(ERROR_LABEL_NOT_FOUND, am_version, line_counter);
                *error_flag = 1;
            }
        }
        if (location == EXTERN) {
            extern_flag = 1;
            non_space_ptr = skip_to_next_word(ptr, first_word_in_line_length);
            ptr = non_space_ptr;
            first_word_in_line_length = first_word_length_counter(ptr);
            for (i = 0; i < label_table->rep; i++) {
                if (!strncmp(label_table->label_element[i].name, ptr, first_word_in_line_length)) {
                    if(label_table -> label_element[i].characteristic == ENTRY) {
                        error_handler(ERROR_LABEL_IS_ENTRY, am_version, line_counter);
                        *error_flag = 1;
                    }
                }
            }
        }
        if (location == CODE) {
            command_in_line = which_command(ptr);
            parse_instruction_stage_2(ptr, command_in_line, &code_node->first_operand.name, &code_node->second_operand.name,
                                      am_version, line_counter);
            reset_opernads_type(&code_node->first_operand, &code_node->second_operand);
            if (code_node->first_operand.name && strcmp(code_node->first_operand.name, "") != 0)
                if(analyze_operand_stage_2(&code_node->first_operand, *label_table)) {
                    error_handler(ERROR_INVALID_FIRST_OPERAND, am_version, line_counter);
                    *error_flag = 1;
                }
            if (code_node->second_operand.name && strcmp(code_node->second_operand.name, "") != 0)
                if(analyze_operand_stage_2(&code_node->second_operand, *label_table)){
                    error_handler(ERROR_INVALID_SECOND_OPERAND, am_version, line_counter);
                    *error_flag = 1;
                }
            validate_operands(command_in_line, code_node->first_operand, code_node->second_operand, am_version, line_counter);
            convert_operands_to_binary(code_node->first_operand, code_node->second_operand, &first_operand_in_binary, &second_operand_in_binary, label_table);
            if (code_node->length >= 2) {
                if(code_node->first_operand.type != UNKNOWN && first_operand_in_binary) {
                    strcpy(code_node->first_operand.word_in_binary, first_operand_in_binary);
                    free(first_operand_in_binary);
                }
            }

            if (code_node->length == 3) {
                if(code_node->second_operand.type != UNKNOWN && second_operand_in_binary) {
                    strcpy(code_node->second_operand.word_in_binary, second_operand_in_binary);
                    free(second_operand_in_binary);
                }
            }

            code_node->decimal_address_in_machine = IC + STARTING_POINT_OF_MEMORY;
            IC += code_node->length;
            if (code_node != code_image->last && code_node->next_node != NULL)
                code_node = code_node->next_node;
        }
    }
    while(data_node != NULL) {
        convert_ascii_to_binary(data_node);
        DC += data_node->length;
        data_node = data_node->next_node;
    }

    fclose(am_extension_file);
    free(am_version);
    am_version = NULL;
    if (*error_flag)
        return 1;

    if (entry_flag)
        ent_file_usher(file_name, label_table);
    if (extern_flag)
        ext_file_usher(file_name, code_image);
    ob_file_usher(file_name, data_image, code_image, IC, DC);
    return 0;
}

/**
 * Validates the operands for a given command.
 *
 * @param command The command index in the commands_list.
 * @param first_operand The first operand to validate.
 * @param second_operand The second operand to validate.
 * @param file_name The name of the file being processed.
 * @param line_counter The current line number in the source code.
 * @return 0 if the operands are valid, 1 otherwise.
 */
int validate_operands(const int command, const operand first_operand, const operand second_operand, const char *file_name, const int line_counter) {
    int is_error = 0;

    if (command == 1) {}
    else if (command <= 3) {
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 4) {
        if (first_operand.type != LABEL_VALUE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if ((command >= 5 && command <= 8) || command == 11) {
        if (first_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 9 || command == 10 || command == 13) {
        if (first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 12) {
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 14 || command == 15) {
        if (first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS, file_name, line_counter);
            is_error = 1;
        }
    }
    return is_error;
}