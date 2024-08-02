#include "assembler_stage_1.h"

char *commands_list[16] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/,
    "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/
};

char *register_list[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

void printLabelTable(const label_array *array) { //- testing
    int i;
    printf("Label Table:\n");
    for (i = 0; i < array->rep; i++) {
        printf("Label %d: %s and the address is :%d\n", i + 1, array->label_element[i].name,
               array->label_element[i].address);
    }
}

void printDataImage(const data_image *image) { //-  testing
    data_node *current = image->first;
    int i;

    printf("\n");
    while (current != NULL) {
        printf("Original line: %s", current->original_line);
        printf("Data: ");
        for (i = 0; i < current->length; i++) {
            //if(current->word[i] != 0)
            printf("%d ", current->char_in_ASCII[i]);
        }
        printf("\n\n");
        current = current->next_node;
    }
}

void printCodeImage(const code_image *image) {  //- testing
    code_node *current = image->first;
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
}

int stage_1_process_file(const char *file_name, label_array *label_table, code_image *code_image, data_image *data_image) {

    int i, IC = 0, DC = 0, L = 0, location, first_word_in_line_length, labelFlag, error_found = 0, command_in_line, array_size, line_counter = 0;
    int *parced_array = NULL;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1], word_in_binary[LENGTH_OF_BINARY_WORD];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL;
    data_node *data_node = NULL;
    code_node *code_node = NULL;
    FILE *am_extension = NULL;

    label_header[0] = '\0';
    am_version = file_name_extender(file_name, ".am");
    am_extension = fopen(am_version, "r");

    if (am_extension == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension)) {
        line_counter++;
        labelFlag = 0;
        memset(label_header, '\0', sizeof(label_header));
        non_space_ptr = first_char_in_line(line);
        first_word_in_line_length = first_word_length_counter(non_space_ptr);
        location = lineLocation(non_space_ptr);
        if(location == -1) {
            error_handler("ERROR_UNKNOWN_LINE_TYPE", am_version, line_counter);
            error_found = 1;
        }

        if (location == LABEL) {
            labelFlag = 1;
            strncpy(label_header, non_space_ptr, first_word_in_line_length);
            if (is_reserved_word(label_header, strlen(label_header))) {
                error_handler("ERROR_LABEL_NAME_IS_A_RESERVED_WORD", am_version, line_counter);
                error_found = 1;
                //free label_table here as it might cause double-free issues outside this function
                //might be a better idea to free it ouside this function. not sure why I wrote the comment above...
            }
            ptr = non_space_ptr + first_word_in_line_length + LENGTH_OF_COLON;
            non_space_ptr = first_char_in_line(ptr);
            first_word_in_line_length = first_word_length_counter(non_space_ptr);
            location = lineLocation(non_space_ptr);
        }
        if (location == DATA || location == STRING) {
            if (labelFlag == 1) {
                if(add_label_to_array(label_table, label_header, DC, location, am_version, line_counter))
                    error_found = 1;
            }
            if (location == DATA)
                parse_dot_data(non_space_ptr, &parced_array, &array_size, &DC, am_version, line_counter);
            else
                parse_dot_string(non_space_ptr, &parced_array, &array_size, &DC, am_version, line_counter);
            data_node = newDataNode(non_space_ptr, array_size, parced_array);
            dataNodeAdd(data_image, data_node);
            free(parced_array);
            parced_array = NULL;
        } else if (location == EXTERN) {
            ptr = non_space_ptr + first_word_in_line_length;
            non_space_ptr = first_char_in_line(ptr);
            first_word_in_line_length = first_word_length_counter(non_space_ptr);
            strncpy(label_header, non_space_ptr, first_word_in_line_length);
            if (is_reserved_word(label_header, strlen(label_header))) {
                error_handler("ERROR_LABEL_NAME_IS_A_RESERVED_WORD", am_version, line_counter);
                error_found = 1;
                //free label_table here as it might cause double-free issues outside this function
                //might be a better idea to free it ouside this function. not sure why I wrote the comment above...
            }
            if(add_label_to_array(label_table, label_header, EXTERN_ADDRESS, EXTERN, am_version, line_counter))
                error_found = 1;
        } else if (location == ENTRY) {
        } else { /*CODE*/
            if (labelFlag == 1)
                if(add_label_to_array(label_table, label_header, IC + STARTING_POINT_OF_MEMORY, IRRLEVANT, am_version, line_counter))
                    error_found = 1;
            ptr = non_space_ptr;
            non_space_ptr = first_char_in_line(ptr);
            command_in_line = whichCommand(non_space_ptr);
            if(command_in_line == -1) {
                error_handler("ERROR_COMMAND_NOT_FOUND", am_version, line_counter);
                error_found = 1;
            }
            analyze_command(non_space_ptr, command_in_line, &L, word_in_binary, am_version, line_counter);
            code_node = newCodeNode(non_space_ptr, L, word_in_binary);
            codeNodeAdd(code_image, code_node);
            IC += L;
            L = 0;
        }
    }
    free(am_version);
    am_version = NULL;
    ptr = NULL;
    non_space_ptr = NULL;
    if (error_found) {
        fclose(am_extension);
        return 1;
    }
    for (i = 0; i < label_table->rep; i++) {
        if (label_table->label_element[i].characteristic == DATA || label_table->label_element[i].characteristic ==
            STRING) {
            label_table->label_element[i].address += (IC + STARTING_POINT_OF_MEMORY);
            }
    }

    printLabelTable(label_table); //- testing
    printDataImage(data_image); //- testing
    printCodeImage(code_image); //- testing
    printExternLabels(label_table); //- testing

    fclose(am_extension);
    return 0;
}

/**
 * Determines the type of a line in the source code.
 *
 * @param str Pointer to the line string.
 * @return An integer representing the type of the line.
 */
int lineLocation(char *str) {
    int i = 0;
    const char *ptr_to_firstWord = first_char_in_line(str);

    if (isalpha(ptr_to_firstWord[0])) {
        while (ptr_to_firstWord[i] && ptr_to_firstWord[i] != ':') {
            if (!isalnum(ptr_to_firstWord[i])) {
                break;
            }
            i++;
        }
        if (ptr_to_firstWord[i] == ':' && (i > 0) && (
                ptr_to_firstWord[i + 1] == ' ' || ptr_to_firstWord[i + 1] == '\t' || ptr_to_firstWord[i + 1] == '\0')) {
            return LABEL;
        }
    }
    if (strncmp(ptr_to_firstWord, ".data ", 6) == 0)
        return DATA;
    else if (strncmp(ptr_to_firstWord, ".string ", 8) == 0)
        return STRING;
    else if (strncmp(ptr_to_firstWord, ".entry ", 7) == 0)
        return ENTRY;
    else if (strncmp(ptr_to_firstWord, ".extern ", 8) == 0)
        return EXTERN;
    else if(isCommand(ptr_to_firstWord))
        return CODE;
    return -1;
}

/**
 * Checks if a given command is in the list of valid commands.
 *
 * @param command Pointer to the command string to check.
 * @return 1 if the command is found in the commands_list, 0 otherwise.
 */
int isCommand(const char *command) {
    int i;

    for (i = 0; i < 16; i++) {
        if (strcmp(command, commands_list[i]) == 1)
            return 1;
    }
    return 0;
}

/**
 * This function calculates the length of the command and compares it with each command
 * in the commands_list to find a match, so it can return the index of the command.
 *
 * @param command Pointer to the command string to check.
 * @return The index of the command in the commands_list if found, -1 otherwise.
 */
int whichCommand(const char *command) {
    int i;
    int str_length = 0;

    for (i = 0; (command[i] && command[i] != ' ' && command[i] != '\t' && command[i] != ',' && command[i] != '\n'); i
         ++) {
        str_length++;
    }

    for (i = 0; i < 16; i++) {
        if (strlen(commands_list[i]) == str_length && !strncmp(commands_list[i], command, str_length)) {
            return i;
        }
    }
    return -1;
}

/**
 * This function parses the command string to extract the operands, analyzes each operand,
 * and converts the command to it's binary representation.
 *
 * @param ptr Pointer to the command string.
 * @param command The command index in the commands_list.
 * @param L Pointer to the length of the command in memory.
 * @param word_in_binary Pointer to the string where the binary representation will be stored.
 * @param file_name The name of the file being processed.
 * @param line_counter The current line number in the source code.
 */
int analyze_command(char *ptr, const int command, int *L, char *word_in_binary, const char *file_name, const int line_counter) {
    int is_error = 0;
    char *first_operand_name = NULL, *second_operand_name = NULL;
    operand first_operand, second_operand;

    parse_instruction(ptr, command, &first_operand_name, &second_operand_name, file_name, line_counter);

    if (first_operand_name) {
        first_operand.name = malloc(strlen(first_operand_name) + 1);
        if (first_operand.name) {
            strcpy(first_operand.name, first_operand_name);
        }
        else {
            error_handler("Failed to allocate memory ", file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
    } else {
        first_operand.name = NULL;
    }
    free(first_operand_name);

    if (second_operand_name) {
        second_operand.name = malloc(strlen(second_operand_name) + 1);
        if (second_operand.name) {
            strcpy(second_operand.name, second_operand_name);
        }
        else {
            error_handler("Failed to allocate memory", file_name, line_counter);
            free(first_operand_name);
            free(second_operand_name);
            return 1;
        }
    } else {
        second_operand.name = NULL;
    }
    free(second_operand_name);

    first_operand.type = UNKNOWN;
    second_operand.type = UNKNOWN;
    if (command <= 15) {
        (*L)++;
        if (command <= 13) {
            analyze_operand(&first_operand);
            (*L)++;
            if (command <= 4) {
                analyze_operand(&second_operand);
                if (!(first_operand.type == REGISTER_PTR && second_operand.type == REGISTER) &&
                    !(first_operand.type == REGISTER && second_operand.type == REGISTER_PTR) &&
                    !(first_operand.type == REGISTER && second_operand.type == REGISTER) &&
                    !(first_operand.type == REGISTER_PTR && second_operand.type == REGISTER_PTR))
                    (*L)++;
            }
        }
    }
    if (command == 1){}
    else if (command <= 3) {
        if (second_operand.type == IMMEDIATE) {
            error_handler("ERROR_INVALID_TYPE_IN_SECOND_OPERAND", file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 4) {
        if (first_operand.type != LABEL_VALUE) {
            error_handler("ERROR_INVALID_TYPE_IN_FIRST_OPERAND", file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type == IMMEDIATE) {
            error_handler("ERROR_INVALID_TYPE_IN_SECOND_OPERAND", file_name, line_counter);
            is_error = 1;
        }
    }
    if ((command >= 5 &&command <= 8) || command == 11) {
        if (first_operand.type == IMMEDIATE) {
            error_handler("ERROR_INVALID_TYPE_IN_FIRST_OPERAND", file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler("ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND", file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 9 || command == 10 || command == 13) {
        if (first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            error_handler("ERROR_INVALID_TYPE_IN_FIRST_OPERAND", file_name, line_counter);
            is_error = 1;
        }
        if (second_operand.type != UNKNOWN) {
            error_handler("ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND", file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 12) {
        if (second_operand.type != UNKNOWN) {
            error_handler("ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND", file_name, line_counter);
            is_error = 1;
        }
    }
    if (command == 14 || command == 15) {
        if (first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            error_handler("ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS", file_name, line_counter);
            is_error = 1;
        }
    }
    if (is_error) {
        if (first_operand.name)
            free(first_operand.name);
        if (second_operand.name)
            free(second_operand.name);
        return 1;
    }

    strcpy(word_in_binary, command_to_binary(command, first_operand, second_operand, *L));
    if (word_in_binary == NULL) {
        error_handler("ERROR_BINARY_VERSION_CLOUD_NOT_BE_CREATED", file_name, line_counter);
        return 1;
    }
    if (first_operand.name)
        free(first_operand.name);
    if (second_operand.name)
        free(second_operand.name);
    return 0;
}

/**
 * Analyzes an operand and determines its type.
 *
 * This function examines the operand string to determine if it is an immediate value,
 * a register, a register pointer, or a label address(label value).
 *
 * @param operand Pointer to the operand structure to analyze.
 */
int analyze_operand(operand *operand) {
    int i;
    operand->type = UNKNOWN;
    switch (operand->name[0]) {
        case '#':
            operand->type = IMMEDIATE;
        return 0;
        case '*':
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name + 1, register_list[i], 2) == 0) {
                    operand->type = REGISTER_PTR;
                    return 0;
                }
            }
        break;
        default: {
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name, register_list[i], 2) == 0) {
                    operand->type = REGISTER;
                    return 0;
                }
            }
            if(strlen(operand->name) <= MAX_LENGTH_OF_LABEL_VALUE && isalpha(operand->name[0])) {
                for(i = 1; i < strlen(operand->name); i++) {
                    if(!isalnum(operand->name[i])) {
                        operand->type = UNKNOWN;
                        return 1;
                    }
                }
                operand->type = LABEL_VALUE;
            }
        }
    }
    return 0;

}