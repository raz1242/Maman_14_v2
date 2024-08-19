#include "assembler_stage_2.h"

/**
 * Processes the given file in the second stage of the assembler.
 *
 * This function reads the file, processes each line, and updates the instruction and data images.
 * It also handles labels, entries, and externs, and generates the necessary output files.
 *
 * @param file_name The name of the file to process.
 * @param label_table A pointer to the array of labels.
 * @param instruction_image A pointer to the instruction image.
 * @param data_image A pointer to the data image.
 * @param error_flag A pointer to an integer that will be set to 1 if an error occurs, otherwise 0.
 * @return Returns 0 on success, otherwise returns 1 if an error occurs.
 */
int stage_2_process_file(const char *file_name, const label_array *label_table, const instruction_image *instruction_image,
                         const data_image *data_image, int *error_flag) {
    int i, IC = 0, DC = 0, ptr_location, total_memory_size, first_word_in_line_length, command_in_line, label_match_found, line_counter = 0, extern_flag = 0
            , entry_flag = FALSE;
    char line[MAX_LEGAL_LENGTH_OF_LINE];
    char *ptr_in_line = NULL, *first_operand_in_binary = NULL, *second_operand_in_binary = NULL;
    instruction_node *instruction_node = instruction_image->first;
    data_node *data_node = data_image->first;
    char *am_version = file_name_extender(file_name, ".am");
    FILE *am_extension_file = fopen(am_version, "r");
    if (am_extension_file == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return EXIT_FAILURE;
    }


    while (fgets(line, MAX_LEGAL_LENGTH_OF_LINE + LENGTH_OF_NULL_TERMINATOR, am_extension_file)) {
        total_memory_size = IC + DC + STARTING_POINT_OF_MEMORY;
        if (total_memory_size >= MAX_SIZE_OF_MEMORY) {
            printf("%s\n", ERROR_MEMORY_LIMIT_REACHED);
            *error_flag = TRUE;
            break;
        }
        line_counter++;
        label_match_found = FALSE;
        ptr_in_line = skip_whitespace(line);
        first_word_in_line_length = first_word_length_counter(ptr_in_line);
        ptr_location = line_location(ptr_in_line);
        if (ptr_location == LABEL_DEFINITION) {
            ptr_in_line = skip_to_next_word(ptr_in_line, first_word_in_line_length + LENGTH_OF_COLON); /* skipping the label header */
            ptr_location = line_location(ptr_in_line); /* checking the location of the first word after the label */
        }
        if(ptr_location == DATA || ptr_location == STRING) {
            if(data_node && !strcmp(data_node -> original_line, ptr_in_line)) { /* compare *ptr_in_line against a valid .data || .string commands to calculate DC */
                DC += data_node->length; /* increase the DC */
                data_node = data_node->next_node;
            }
        }
        if (ptr_location == ENTRY) {
            entry_flag = TRUE; /* notify that there is an entry in the file */
            ptr_in_line = skip_to_next_word(ptr_in_line, strlen(".entry")); /* skip the ".entry" command */
            if(ptr_in_line == NULL) {
                continue;
            }
            first_word_in_line_length = first_word_length_counter(ptr_in_line);
            ptr_in_line[first_word_in_line_length] = NULL_TERMINATOR;
            for (i = 0; i < label_table->rep; i++) {/* checking if the label is in the label table */
                if (!strcmp(label_table->label_element[i].name, ptr_in_line)) {
                    label_match_found = TRUE;
                    label_table->label_element[i].characteristic = ENTRY;
                    break;
                }
            }
            if (!label_match_found) { /* if the label is not in the label table, sends error */
                error_handler(ERROR_LABEL_NOT_FOUND, am_version, line_counter);
                *error_flag = TRUE;
            }
        }
        if (ptr_location == EXTERN) {
            extern_flag = TRUE; /* notify that there is an extern in the file */
            ptr_in_line = skip_to_next_word(ptr_in_line, strlen(".extern")); /* skip the ".extern" command */
            if(ptr_in_line == NULL) {
                continue;
            }
            first_word_in_line_length = first_word_length_counter(ptr_in_line);
            ptr_in_line[first_word_in_line_length] = NULL_TERMINATOR;
            for (i = 0; i < label_table->rep; i++) { /* check if the label is already defined as an entry */
                if (!strcmp(label_table->label_element[i].name, ptr_in_line)) {
                    if (label_table -> label_element[i].characteristic == ENTRY) {
                        error_handler(ERROR_LABEL_IS_ENTRY, am_version, line_counter);
                        *error_flag = TRUE;
                    }
                }
            }
        }
        if (ptr_location == INSTRUCTION) {
            command_in_line = which_command(ptr_in_line); /* check which command is in the line */
            parse_instruction_stage_2(ptr_in_line, command_in_line, &instruction_node->first_operand.name, &instruction_node->second_operand.name);
            reset_opernads_type(&instruction_node->first_operand, &instruction_node->second_operand); /* reset the operands type */
            if (instruction_node->first_operand.name && strcmp(instruction_node->first_operand.name, "") != 0) /* if there is a first operand */
                if (analyze_operand_stage_2(&instruction_node->first_operand, *label_table)) { /* analyze the first operand */
                    error_handler(ERROR_INVALID_FIRST_OPERAND, am_version, line_counter);
                    *error_flag = TRUE;
                }
            if (instruction_node->second_operand.name && strcmp(instruction_node->second_operand.name, "") != 0) /* if there is a second operand */
                if (analyze_operand_stage_2(&instruction_node->second_operand, *label_table)){ /* analyze the second operand */
                    error_handler(ERROR_INVALID_SECOND_OPERAND, am_version, line_counter);
                    *error_flag = TRUE;
                }
            validate_operands(command_in_line, instruction_node->first_operand, instruction_node->second_operand, am_version, line_counter); /* validate the operands types fit command requirements*/
            if (instruction_node->length >= 2) {
                convert_operands_to_binary(instruction_node->first_operand, instruction_node->second_operand, &first_operand_in_binary, &second_operand_in_binary, label_table); /* convert the operands to binary */

                if (instruction_node->first_operand.type != UNKNOWN && first_operand_in_binary) { /* if the first operand is not unknown and the binary is not null */
                    strcpy(instruction_node->first_operand.word_in_binary, first_operand_in_binary); /* copy the binary to the instruction node */
                    free(first_operand_in_binary);
                }
            }
            if (instruction_node->length == 3) {
                if (instruction_node->second_operand.type != UNKNOWN && second_operand_in_binary) { /* if the second operand is not unknown and the binary is not null */
                    strcpy(instruction_node->second_operand.word_in_binary, second_operand_in_binary); /* copy the binary to the instruction node */
                    free(second_operand_in_binary);
                }
            }

            instruction_node->decimal_address_in_machine = IC + STARTING_POINT_OF_MEMORY; /* set the decimal address in the machine */
            IC += instruction_node->length; /* increase the IC */
            if (instruction_node != instruction_image->last && instruction_node->next_node != NULL) /* if the instruction node is not the last node */
                instruction_node = instruction_node->next_node;
        }
    }
    data_node = data_image->first;
    while(data_node != NULL) { /* converting all ascii data inside data_image into binary */
        convert_ascii_to_binary(data_node);
        data_node = data_node->next_node;
    }

    fclose(am_extension_file);
    free(am_version);
    am_version = NULL;
    if (*error_flag) /* if there is an error in the file, stops here */
        return EXIT_FAILURE;

    if (entry_flag) /* if there is an entry in the file */
        ent_file_usher(file_name, label_table);
    if (extern_flag) /* if there is an extern in the file */
        ext_file_usher(file_name, instruction_image);
    ob_file_usher(file_name, data_image, instruction_image, IC, DC);
    return EXIT_SUCCESS;
}

/**
 * Validates the operands types for the given command.
 *
 * @param command The command index in the commands_list.
 * @param first_operand The first operand to validate.
 * @param second_operand The second operand to validate.
 * @param file_name The name of the file being processed.
 * @param line_counter The current line number in the source code.
 * @return 0 if the operands are valid, 1 otherwise.
 */
int validate_operands(const int command, const operand first_operand, const operand second_operand, const char *file_name, const int line_counter) {
    int error_flag = FALSE;

    if (command == mov || command == add || command == sub) {
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    if (command == lea) {
        if (first_operand.type != LABEL_VALUE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
        if (second_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    if (command == clr || command == not || command == inc || command == dec || command == red) {
        if (first_operand.type == IMMEDIATE) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    if (command == jmp || command == bne || command == jsr) {
        if (first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            error_handler(ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    if (command == prn) {
        if (second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    if (command == rts || command == stop) {
        if (first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            error_handler(ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS, file_name, line_counter);
            error_flag = TRUE;
        }
    }
    return error_flag;
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
        command_length = STOP_COMMAND_LENGTH; /* stop command has a different length */
    else
        command_length = OTHER_COMMAND_LENGTH; /*all other commands have the same length */

    input_ptr = skip_to_next_word(input_ptr, command_length); /* skip the command */
    if (command == rts || command == stop) {/* if the command is rts or stop, there are no operands */
        return EXIT_SUCCESS;
    }
    if (input_ptr == NULL) { /* if there are no operands */
        return EXIT_FAILURE;
    }
    first_operand_length = operand_length_counter(input_ptr);
    first_operand_name = (char *)malloc(first_operand_length + LENGTH_OF_NULL_TERMINATOR);
    if (first_operand_name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(EXIT_FAILURE);
    }
    strncpy(first_operand_name, input_ptr, first_operand_length);
    first_operand_name[first_operand_length] = NULL_TERMINATOR;
    if (command == mov || command == cmp || command == add || command == sub || command == lea) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if(is_end_of_line(input_ptr)) { /* if there is no comma after first operand */
            free(first_operand_name);
            return EXIT_FAILURE;
        }
        if (strncmp(input_ptr, ",", 1) == 0)
            input_ptr = skip_whitespace(input_ptr + LENGTH_OF_COMMA); /* skip the comma */
        if(is_end_of_line(input_ptr)) { /* if there is no second operand */
            free(first_operand_name);
            return EXIT_FAILURE;
        }
        second_operand_length = operand_length_counter(input_ptr);
        second_operand_name = (char *)malloc(second_operand_length + LENGTH_OF_NULL_TERMINATOR);
        if (second_operand_name == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(first_operand_name);
            exit(EXIT_FAILURE);
        }
        strncpy(second_operand_name, input_ptr, second_operand_length);
        second_operand_name[second_operand_length] = NULL_TERMINATOR;
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
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }
        strcpy(*dest, second_operand_name);
        free(second_operand_name);
        second_operand_name = NULL;
    }
    return EXIT_SUCCESS;
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
            if (!(operand_name[1] == NULL_TERMINATOR || isdigit(operand_name[1]) || operand_name[1] == MINUS_SIGN || operand_name[1] == PLUS_SIGN)) { /* check if the immediate operand is valid */
                operand->type = UNKNOWN;
                return EXIT_FAILURE;
            }
        for(i = 2; operand_name[i] != NULL_TERMINATOR; i++) {
            if (!isdigit(operand_name[i])) {/* check if the immediate operand is valid */
                operand->type = UNKNOWN;
                return EXIT_FAILURE;
            }
        }
        operand->type = IMMEDIATE;
        return EXIT_SUCCESS;
        case '*':
            if (which_register(operand_name) != NOT_A_REGISTER) { /* check if the register pointer operand is valid */
                operand->type = REGISTER_PTR;
                return EXIT_SUCCESS;
            }
        return EXIT_FAILURE;
        case 'r':
            if (which_register(operand_name) != NOT_A_REGISTER) { /* check if the register operand is valid */
                operand->type = REGISTER;
                return EXIT_SUCCESS;
            }
        default: {
            if (is_label(&label_array, operand_name)) { /* check if the label operand is valid */
                operand->type = LABEL_VALUE;
                return EXIT_SUCCESS;
            }
        }
    }
    return EXIT_FAILURE;
}