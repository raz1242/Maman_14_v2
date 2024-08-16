#include "assembler_stage_2.h"


int stage_2_process_file(const char *file_name, const label_array *label_table, const instruction_image *instruction_image,
                         const data_image *data_image, int *error_flag) {
    int i, IC = 0, DC = 0, location, first_word_in_line_length, command_in_line, label_match_found, line_counter = 0, extern_flag = 0
            , entry_flag = 0;
    char line[MAX_LEGAL_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + LENGTH_OF_NULL_TERMINATOR];
    char *ptr = NULL, *non_space_ptr = NULL, *first_operand_in_binary = NULL, *second_operand_in_binary = NULL;
    char *am_version = file_name_extender(file_name, ".am");
    FILE *am_extension_file = fopen(am_version, "r");
    if (am_extension_file == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }
    instruction_node *instruction_node = instruction_image->first;
    data_node *data_node = data_image->first;

    while (fgets(line, MAX_LEGAL_LENGTH_OF_LINE + LENGTH_OF_NULL_TERMINATOR, am_extension_file)) {
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
            ptr[first_word_in_line_length] = '\0';
            for (i = 0; i < label_table->rep; i++) {
                if (!strcmp(label_table->label_element[i].name, ptr)) {
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
            ptr[first_word_in_line_length] = '\0';
            for (i = 0; i < label_table->rep; i++) {
                if (!strcmp(label_table->label_element[i].name, ptr)) {
                    if (label_table -> label_element[i].characteristic == ENTRY) {
                        error_handler(ERROR_LABEL_IS_ENTRY, am_version, line_counter);
                        *error_flag = 1;
                    }
                }
            }
        }
        if (location == INSTRUCTION) {
            command_in_line = which_command(ptr);
            parse_instruction_stage_2(ptr, command_in_line, &instruction_node->first_operand.name, &instruction_node->second_operand.name);
            reset_opernads_type(&instruction_node->first_operand, &instruction_node->second_operand);
            if (instruction_node->first_operand.name && strcmp(instruction_node->first_operand.name, "") != 0)
                if (analyze_operand_stage_2(&instruction_node->first_operand, *label_table)) {
                    error_handler(ERROR_INVALID_FIRST_OPERAND, am_version, line_counter);
                    *error_flag = 1;
                }
            if (instruction_node->second_operand.name && strcmp(instruction_node->second_operand.name, "") != 0)
                if (analyze_operand_stage_2(&instruction_node->second_operand, *label_table)){
                    error_handler(ERROR_INVALID_SECOND_OPERAND, am_version, line_counter);
                    *error_flag = 1;
                }
            validate_operands(command_in_line, instruction_node->first_operand, instruction_node->second_operand, am_version, line_counter);
            convert_operands_to_binary(instruction_node->first_operand, instruction_node->second_operand, &first_operand_in_binary, &second_operand_in_binary, label_table);
            if (instruction_node->length >= 2) {
                if (instruction_node->first_operand.type != UNKNOWN && first_operand_in_binary) {
                    strcpy(instruction_node->first_operand.word_in_binary, first_operand_in_binary);
                    free(first_operand_in_binary);
                }
            }
            if (instruction_node->length == 3) {
                if (instruction_node->second_operand.type != UNKNOWN && second_operand_in_binary) {
                    strcpy(instruction_node->second_operand.word_in_binary, second_operand_in_binary);
                    free(second_operand_in_binary);
                }
            }
            instruction_node->decimal_address_in_machine = IC + STARTING_POINT_OF_MEMORY;
            IC += instruction_node->length;
            if (instruction_node != instruction_image->last && instruction_node->next_node != NULL)
                instruction_node = instruction_node->next_node;
        }
    }
    while(data_node != NULL) { /* converting all ascii data inside data_image into binary */
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
        ext_file_usher(file_name, instruction_image);
    ob_file_usher(file_name, data_image, instruction_image, IC, DC);
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

    if (command == mov || command == add || command == sub) {
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == lea) {
        if (first_operand.type != LABEL_VALUE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == clr || command == not || command == inc || command == dec || command == red) {
        if (first_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == jmp || command == bne || command == jsr) {
        if (first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == prn) {
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == rts || command == stop) {
        if (first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS, file_name, line_counter);
            is_error = 1;
        }
    }
    return is_error;
}

/**
 * Parses the input string to extract the source and destination operands based on the given command.
 * like parse_instruction but without error handling
 *
 * @param input_ptr A pointer to the input string containing the command and operands.
 * @param command The command value indicating the type of command to be parsed.
 * @param source A pointer to a string where the source operand will be stored.
 * @param dest A pointer to a string where the destination operand will be stored.
 * @return Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_instruction_stage_2(char *input_ptr, const int command, char **source, char **dest) {
    int command_length, first_operand_length , second_operand_length = 0;
    char *first_operand_name = NULL, *second_operand_name = NULL;
    *source = NULL;
    *dest = NULL;

    if (command == stop)
        command_length = STOP_COMMAND_LENGTH;
    else
        command_length = OTHER_COMMAND_LENGTH;

    input_ptr = skip_to_next_word(input_ptr, command_length);
    if (command == rts || command == stop) {
        return 0;
    }
    if (input_ptr == NULL) {
        return 1;
    }
    first_operand_length = operand_length_counter(input_ptr);
    first_operand_name = (char *)malloc(first_operand_length + LENGTH_OF_NULL_TERMINATOR);
    if (first_operand_name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    strncpy(first_operand_name, input_ptr, first_operand_length);
    first_operand_name[first_operand_length] = '\0';
    fflush(stdout);
    if (command == mov || command == cmp || command == add || command == sub || command == lea) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if (input_ptr != NULL) {
            if (strncmp(input_ptr, ",", 1) == 0)
                input_ptr = skip_whitespace(++input_ptr);
        }
        else {
            free(first_operand_name);
            return 1;
        }
        second_operand_length = operand_length_counter(input_ptr);
        second_operand_name = (char *)malloc(second_operand_length + LENGTH_OF_NULL_TERMINATOR);
        if (second_operand_name == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(first_operand_name);
            exit(1);
        }
        strncpy(second_operand_name, input_ptr, second_operand_length);
        second_operand_name[second_operand_length] = '\0';
    }
    *source = (char *)malloc(first_operand_length + LENGTH_OF_NULL_TERMINATOR);
    if (*source == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(first_operand_name);
        first_operand_name = NULL;
        if (second_operand_name) {
            free(second_operand_name);
            second_operand_name = NULL;
        }
        exit(1);
    }
    strcpy(*source, first_operand_name);
    free(first_operand_name);
    first_operand_name = NULL;

    if (command == mov || command == cmp || command == add || command == sub || command == lea) {
        *dest = (char *)malloc(second_operand_length + LENGTH_OF_NULL_TERMINATOR);
        if (*dest == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(*source);
            free(second_operand_name);
            second_operand_name = NULL;
            exit(1);
        }
        strcpy(*dest, second_operand_name);
        free(second_operand_name);
        second_operand_name = NULL;
    }
    return 0;
}

/**
 * Analyzes the given operand and determines its type based on its name.
 *
 * @param operand A pointer to the operand to be analyzed.
 * @param label_array The array of labels to check against for label type operands.
 * @return Returns 0 if the operand type is successfully determined, otherwise returns 1.
 */
int analyze_operand_stage_2(operand *operand, const label_array label_array) {
    int i;
    const char *operand_name = operand->name;

    operand->type = UNKNOWN;
    switch (operand_name[0]) {
        case '#':
            if (!(operand_name[1] == '\0' || isdigit(operand_name[1]) || operand_name[1] == '-' || operand_name[1] == '+')) {
                operand->type = UNKNOWN;
                return 1;
            }
        for(i = 2; operand_name[i] != '\0'; i++) {
            if (!isdigit(operand_name[i])) {
                operand->type = UNKNOWN;
                return 1;
            }
        }
        operand->type = IMMEDIATE;
        return 0;
        case '*':
            if (which_register(operand_name) != -1) {
                operand->type = REGISTER_PTR;
                return 0;
            }
        return 1;
        case 'r':
            if (which_register(operand_name) != -1) {
                operand->type = REGISTER;
                return 0;
            }
        default: {
            if (is_label(&label_array, operand_name)) {
                operand->type = LABEL_VALUE;
                return 0;
            }
        }
    }
    return 1;
}