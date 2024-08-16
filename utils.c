#include "utils.h"

#include "assembler_stage_2.h"

const char *reserved_words[] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/, "dec"/*8*/,
    "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/,
    ".data", ".string", ".entry", ".extern", "macr", "endmacr", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

char *commands_list[AMOUNT_OF_COMMANDS] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/,
    "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/
};

char *register_list[AMOUNT_OF_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};



/**
 * Creates a new file name by appending a specified file type extension to the given file name.
 *
 * @param str The original file name.
 * @param type The file type extension to be appended.
 * @return A new string containing the original file name with the appended file type extension.
 *         Returns NULL if memory allocation fails.
 */
char *file_name_extender(const char *str, const char *type) {
    char *file_type = malloc(strlen(str) + strlen(type) + LENGTH_OF_NULL_TERMINATOR);
    if (file_type == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    strcpy(file_type, str);
    strcat(file_type, type);
    return file_type;
}

/**
 * Inspects the given file pointer to check if the file is readable.
 *
 * @param file A pointer to the file to be inspected.
 * @param fileName The name of the file being inspected.
 * @return Returns 0 if the file is readable, otherwise returns 1.
 */
int file_inspector(const FILE *file, const char *fileName) {
    if (file == NULL) {
        printf("Failed to read file: %s\n", fileName);
        return 1;
    }
    return 0;
}

/**
 * Moves the pointer to the first non-whitespace character in the given string.
 *
 * @param str The input string to be processed.
 * @return A pointer to the first non-whitespace character in the string.
 */
char *skip_whitespace(char *str) {
    if (str == NULL) {
        return NULL;
    }
    while (isspace(*str) || *str == '\t') {
        if(*str == '\n' || *str == '\r')
            return str;
        str++;
    }
    if (*str == '\0') {
        return NULL;
    }
    return str;
}

/**
 * Moves the pointer to the next word in the given string after skipping a specified number of characters.
 *
 * @param str The input string to be processed.
 * @param length The number of characters to skip before moving to the next word.
 * @return A pointer to the first non-whitespace character after the specified number of characters.
 */
char *skip_to_next_word(char *str, const int length) {
    int i;
    for(i = 0; i < length; i++ )
        str++;

    if (str == NULL) {
        return NULL;
    }

    return skip_whitespace(str);
}

/**
 * Counts the length of the first word in the given string
 * while ignoring whitespace characters and colons.
 *
 * @param str The input string to be processed.
 * @return The length of the first word in the string.
 */
int first_word_length_counter(const char *str) {
    int counter;
    if (str == NULL) {
        return 0;
    }
    for (counter = 0; *(str + counter) && !isspace(*(str + counter)) && *(str + counter) != ':'; counter++) {};
    return counter;
}

/**
 * Counts the length of the operand in the given string
 * while ignoring whitespace characters and commas.
 *
 * @param str The input string to be processed.
 * @return The length of the operand in the string.
 */
int operand_length_counter(const char *str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)) && (strncmp(&str[counter], ",", 1) != 0); counter
         ++) {
    };
    return counter;
}

/**
 * Checks if the given word is a reserved word from the reserved_words array.
 *
 * @param word The word to be checked.
 * @param length The length of the word.
 * @return Returns 1 if the word is a reserved word, otherwise returns 0.
 */
int is_reserved_word(char *word, const int length) {
    int i, size;

    size = sizeof(reserved_words) / sizeof(reserved_words[0]);
    for (i = 0; i < size; i++) {
        if (strncmp(word, reserved_words[i], length) == 0 && is_end_of_line(word + length)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if the given string points to the end of a line.
 *
 * @param str The input string to be checked.
 * @return Returns 1 if the string is at the end of a line otherwise returns 0.
 */
int is_end_of_line(char *str) {
    const char *new_str = skip_whitespace(str);

    if (new_str == NULL || *new_str == '\n' || *new_str == '\r')
        return 1;
    return 0;
}

/**
 * Parses the input string to extract data values and stores them in an array.
 *
 * @param input The input string containing the data values.
 * @param array A pointer to the array where the extracted data values will be stored.
 * @param size A pointer to an integer where the size of the array will be stored.
 * @param DC A pointer to an integer that will be incremented by the number of data values extracted.
 * @param file_name The name of the file where the data is being parsed.
 * @param line_counter The line number in the file where the data is being parsed.
 * @return  Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_dot_data(const char *input, int **array, int *size, int *DC, const char *file_name, const int line_counter) {
    const int DATA_COMMAND_LENGTH = strlen(".data ");
    const char *dataStart = NULL;
    int index = 0, commaFlag = 0, numberFlag = 0, current_number;
    char *ptr;

    *size = 0;
    *array = NULL;  /* Initialize as NULL for realloc */
    dataStart = strstr(input, ".data ");

    if (dataStart)
        dataStart += DATA_COMMAND_LENGTH;
    else {
        return 1;
    }

    ptr = (char *)dataStart;
    while (*ptr) {
        if (*ptr != '-' && *ptr != '+' && !isdigit(*ptr) && *ptr != ',' && *ptr != '\n' && *ptr != '\r' && !isspace(*ptr)) {
            error_handler(ERROR_INVALID_DATA_VALUE, file_name, line_counter);
            free(*array);
            return 1;
        }
        if (numberFlag == 1 && !isspace(*ptr) && *ptr != ',' && *ptr != '\n' && *ptr != '\r') {
            error_handler(ERROR_INVALID_DATA_VALUE, file_name, line_counter);
            free(*array);
            return 1;
        }
        if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0') {
            if (commaFlag == 1) {
                error_handler(ERROR_MISSING_DATA_VALUE, file_name, line_counter);
                free(*array);
                return 1;
            }
            if (index == 0) {  /* No valid data found */
                error_handler(ERROR_MISSING_DATA_VALUE, file_name, line_counter);
                free(*array);
                return 1;
            }
            break;
        }
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        if (commaFlag == 1 && *ptr == ',') {
            error_handler(ERROR_MULTIPLE_COMMA_FOUND, file_name, line_counter);
            free(*array);
            return 1;
        }
        if (*ptr == ',') {
            commaFlag = 1;
            numberFlag = 0;
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            current_number = strtol(ptr, &ptr, 10);
            if (current_number > MAX_POSSIBLE_NUMBER || current_number < MIN_POSSIBLE_NUMBER) {
                error_handler(ERROR_NUMBER_IS_OUT_OF_MACHINE_RANGE, file_name, line_counter);
            } else {
                int *temp_array = realloc(*array, (index + 1) * sizeof(int));
                if (temp_array == NULL) {
                    printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
                    free(*array);
                    exit(1);
                }
                *array = temp_array;
                (*array)[index++] = current_number;
            }
            commaFlag = 0;
            numberFlag = 1;
        } else
            ptr++;
    }
    *size = index;
    *DC += index;
    return 0;
}

/**
 * Parses the input string to extract characters and stores them in an array.
 *
 * @param input The input string containing the characters.
 * @param array A pointer to the array where the extracted characters will be stored.
 * @param size A pointer to an integer where the size of the array will be stored.
 * @param DC A pointer to an integer that will be incremented by the number of characters extracted.
 * @param file_name The name of the file where the characters are being parsed.
 * @param line_counter The line number in the file where the characters are being parsed.
*  @return  Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_dot_string(const char *input, int **array, int *size, int *DC, const char *file_name, const int line_counter) {
    const int STRING_COMMAND_LENGTH = strlen(".string ");
    const char *stringStart;
    int count = 0, index;
    char *ptr;

    *size = 0;

    stringStart = strstr(input, ".string ");
    if (stringStart)
        stringStart += STRING_COMMAND_LENGTH;
    else {
        *array = NULL;
        *size = 0;
        return 1;
    }
    while (*stringStart && *stringStart != '"') {
        if(isalpha(*stringStart) || isdigit(*stringStart)){ /* if a character is found outside of quotation marks*/
            error_handler(ERROR_STRING_MUST_START_WITH_QUOTATION_MARK, file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        if(*stringStart == '\n' || *stringStart == '\r') { /* if no string is found*/
            error_handler(ERROR_NO_STRING_FOUND, file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        count++;
        stringStart++;
    }

    if (*stringStart != '"') { /* if the closing quotation mark is missing. */
        error_handler(ERROR_MISSING_CLOSING_QUOTATION_MARK, file_name, line_counter);
        *array = NULL;
        *size = 0;
        return 1;
    }
    ptr = (char *)(stringStart + 1);
    while (*ptr && *ptr != '"') {
        count++;
        ptr++;
    }
    ptr++; /* skips the closing quotation mark */

    while(*ptr != '\n' && *ptr != '\r') {
        if(*ptr != ' ' && *ptr != '\t') { /* if a character is found outside of quotation marks. */
            error_handler(ERROR_INVALID_CHARACTER_FOUND_OUTSIDE_OF_QUOTATION_MARKS, file_name, line_counter);
            break;
        }
        ptr++;
    }

    *array = (int *)malloc((count + LENGTH_OF_NULL_TERMINATOR) * sizeof(int));
    if (*array == NULL) { /* if memory allocation fails. */
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        *size = 0;
        exit(1);
    }

    ptr = (char *) (stringStart + 1);
    for (index = 0; index < count; index++) {
        (*array)[index] = (int)*ptr;
        (*DC)++;
        ptr++;
    }

    (*array)[index] = 0;
    (*DC)++;
    *size = ++count;
    return 0;
}

/**
 * Parses the input string to extract the source and destination operands based on the given command.
 *
 * @param input_ptr A pointer to the input string containing the command and operands.
 * @param command The command value indicating the type of command to be parsed.
 * @param source A pointer to a string where the source operand will be stored.
 * @param dest A pointer to a string where the destination operand will be stored.
 * @param file_name The name of the file where the command is being parsed.
 * @param line_counter The line number in the file where the command is being parsed.
 * @return  Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_instruction(char *input_ptr, const int command, char **source, char **dest, const char *file_name,
                       const int line_counter) {
    int command_length, first_operand_length, second_operand_length = 0;
    char *first_operand_name = NULL, *second_operand_name = NULL;

    if (command == stop)
        command_length = 4;
    else
        command_length = 3;

    *source = NULL;
    *dest = NULL;

    input_ptr = skip_to_next_word(input_ptr, command_length);

    if (command == rts || command == stop) {
        if(*input_ptr != '\n' && *input_ptr != '\r') { /* redundent characters after stop or rts command */
            error_handler(ERROR_REDUNDENT_CHARACTERS_AFTER_COMMAND, file_name, line_counter);
            return 1;
        }
        return 0;
    }
    if(*input_ptr == '\n' || *input_ptr == '\r') { /* lacks the first operand */
        error_handler(ERROR_MISSING_FIRST_OPERAND, file_name, line_counter);
        return 1;
    }
    if (strncmp(input_ptr, ",", 1) == 0) { /* lacks the first operand */
        error_handler(ERROR_REDUNDENT_COMMA_AFTER_COMMAND, file_name, line_counter);
        return 1;
    }
    if(!isalpha(*input_ptr) && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid first operand */
        error_handler(ERROR_INVALID_FIRST_OPERAND, file_name, line_counter);
        return 1;
    }
    first_operand_length = operand_length_counter(input_ptr);
    first_operand_name = (char *) malloc(first_operand_length + LENGTH_OF_NULL_TERMINATOR);
    if (first_operand_name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    strncpy(first_operand_name, input_ptr, first_operand_length);
    first_operand_name[first_operand_length] = '\0';

    if (command == mov || command == cmp || command == add || command == sub || command == lea) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if (strncmp(input_ptr, ",", 1) == 0) {
            input_ptr = skip_to_next_word(input_ptr, LENGTH_OF_COMMA);
        }
        else { /* missing a comma between the two operands */
            error_handler(ERROR_MISSING_A_COMMA, file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if (strncmp(input_ptr, ",", 1) == 0) { /* too many commas */
            error_handler(ERROR_TOO_MANY_COMMAS, file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if(*input_ptr == '\n' || *input_ptr == '\r') { /* lacks the second operand */
            error_handler(ERROR_MISSING_SECOND_OPERAND, file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if(isalpha(*input_ptr) == 0 && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid second operand */
            error_handler(ERROR_INVALID_SECOND_OPERAND, file_name, line_counter);
            free(first_operand_name);
            return 1;
        }

        second_operand_length = operand_length_counter(input_ptr);
        second_operand_name = (char *) malloc(second_operand_length + LENGTH_OF_NULL_TERMINATOR);
        if (second_operand_name == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(first_operand_name);
            exit(1);
        }
        strncpy(second_operand_name, input_ptr, second_operand_length);
        second_operand_name[second_operand_length] = '\0';

        input_ptr = skip_to_next_word(input_ptr, second_operand_length);
        if(input_ptr && *input_ptr != '\n' && *input_ptr != '\r') { /* redundent chraters after seond operand*/
            error_handler(ERROR_REDUNDENT_CHARACTERS_AFTER_SECOND_OPERAND, file_name, line_counter);
        }
    }
    if(command == clr || command == not || command == inc || command == dec || command == jmp || command == bne || command == red || command == prn || command == jsr) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if(input_ptr && *input_ptr != '\n' && *input_ptr != '\r' && *input_ptr != '\0') { /* redundent charaters after first operand*/
            error_handler(ERROR_REDUNDENT_CHARACTERS_AFTER_FIRST_OPERAND, file_name, line_counter);
            free(first_operand_name);
            free(second_operand_name);
            return 1;
        }
    }

    *source = (char *) malloc(first_operand_length + LENGTH_OF_NULL_TERMINATOR);
    if (*source == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        free(first_operand_name);
        if (second_operand_name)
            free(second_operand_name);
        exit(1);
    }
    strncpy(*source, first_operand_name, first_operand_length + 1);
    free(first_operand_name);
    first_operand_name = NULL;

    if (command == mov || command == cmp || command == add || command == sub || command == lea) {
        *dest = (char *) malloc(second_operand_length + LENGTH_OF_NULL_TERMINATOR);
        if (*dest == NULL) {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(*source);
            free(second_operand_name);
            exit(1);
        }
        strncpy(*dest, second_operand_name, second_operand_length + 1);
        free(second_operand_name);
        second_operand_name = NULL;

    }
    return 0;
}

/**
 * Determines the index of a register in the register list.
 *
 * @param operand_name The name of the operand to be checked.
 * @return The index of the register in the register list if found, otherwise returns -1.
 */
int which_register(const char* operand_name) {
    int i;
    const int size_of_array = sizeof(register_list) / sizeof(register_list[0]);

    if(operand_name[0] == '*')
        operand_name++;
    for(i = 0; i < size_of_array; i++) {
        if(strcmp(operand_name, register_list[i]) == 0)
            return i;
    }
    return -1;
}

/**
 * Converts a command and its operands into a binary string representation.
 *
 * @param command The command value indicating the type of command.
 * @param first_operand The first operand of the command.
 * @param second_operand The second operand of the command.
 * @return A string containing the binary representation of the command and its operands.
 *         Returns NULL if memory allocation fails.
 */
char *command_to_binary(const int command, const operand first_operand, const operand second_operand) {
    char *str;
    str = (char *) malloc(15 * sizeof(char) + LENGTH_OF_NULL_TERMINATOR);
    if (str == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    str[0] = '\0';
    switch (command) {
        case 0:
            strcpy(str, "0000");
            break;
        case 1:
            strcpy(str, "0001");
            break;
        case 2:
            strcpy(str, "0010");
            break;
        case 3:
            strcpy(str, "0011");
            break;
        case 4:
            strcpy(str, "0100");
            break;
        case 5:
            strcpy(str, "01010000");
            break;
        case 6:
            strcpy(str, "01100000");
            break;
        case 7:
            strcpy(str, "01110000");
            break;
        case 8:
            strcpy(str, "10000000");
            break;
        case 9:
            strcpy(str, "10010000");
            break;
        case 10:
            strcpy(str, "10100000");
            break;
        case 11:
            strcpy(str, "10110000");
            break;
        case 12:
            strcpy(str, "11000000");
            break;
        case 13:
            strcpy(str, "11010000");
            break;
        case 14:
            strcpy(str, "111000000000100");
            return str;
        case 15:
            strcpy(str, "111100000000100");
            return str;
        default:
            str = NULL;
            break;
    }

    switch (first_operand.type) {
        case IMMEDIATE:
            strcat(str, "0001");
            break;
        case LABEL_VALUE:
            strcat(str, "0010");
            break;
        case REGISTER_PTR:
            strcat(str, "0100");
            break;
        case REGISTER:
            strcat(str, "1000");
            break;
        default:
            break;
    }
    switch (second_operand.type) {
        case IMMEDIATE:
            strcat(str, "0001");
            break;
        case LABEL_VALUE:
            strcat(str, "0010");
            break;
        case REGISTER_PTR:
            strcat(str, "0100");
            break;
        case REGISTER:
            strcat(str, "1000");
            break;
        default:
            break;
    }
    strcat(str, "100");
    return str;
}

/**
 * Converts an immediate operand to its binary representation.
 *
 * @param operand The operand containing the immediate value to be converted.
 * @return A string containing the binary representation of the immediate operand.
 *         Returns NULL if memory allocation fails or if the operand is not a valid immediate value.
 */
char* immediate_operand_to_binary(const operand operand) {
    char *operand_name, *binary_representation;
    char *operand_number_in_binary = malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(operand_number_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    memset(operand_number_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    operand_name = malloc(strlen(operand.name) + LENGTH_OF_NULL_TERMINATOR);
    if(operand_name == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    strcpy(operand_name, operand.name);
    operand_name = skip_whitespace(++operand_name);
    if (isdigit(operand_name[0]) || ((*operand_name == '-' || *operand_name == '+') && isdigit(*(operand_name + 1)))) {
        binary_representation = decimal_to_binary(atoi(operand_name));
        strcpy(operand_number_in_binary, binary_representation);
        strcat(operand_number_in_binary, "100");
        free(binary_representation);
        free(--operand_name);
        return operand_number_in_binary;
    }
    free(operand_name);
    return NULL;
}

/**
 * Converts register operands to their binary representation.
 *
 * @param first_operand The first operand to be converted.
 * @param second_operand The second operand to be converted.
 * @return A string containing the binary representation of the register operands.
 *         Returns NULL if memory allocation fails.
 */
char* register_operand_to_binary(const operand first_operand, const operand second_operand) {
    char *first_operand_binary, *second_operand_binary, *operand_number_in_binary = malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if (operand_number_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    memset(operand_number_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    strcat(operand_number_in_binary, "000000");
    if (second_operand.type == UNKNOWN) {
        first_operand_binary = register_name_to_binary(first_operand.name);
        strcat(operand_number_in_binary, "000");
        strcat(operand_number_in_binary, first_operand_binary);
        free(first_operand_binary);
    }
    else if (first_operand.type == second_operand.type ||
        (first_operand.type == REGISTER_PTR && second_operand.type == REGISTER) ||
        (first_operand.type == REGISTER && second_operand.type == REGISTER_PTR)) {
        first_operand_binary = register_name_to_binary(first_operand.name);
        second_operand_binary = register_name_to_binary(second_operand.name);
        strcat(operand_number_in_binary, first_operand_binary);
        strcat(operand_number_in_binary, second_operand_binary);
        free(first_operand_binary);
        free(second_operand_binary);
    }
    else if(first_operand.type == REGISTER_PTR || first_operand.type == REGISTER) {
        first_operand_binary = register_name_to_binary(first_operand.name);
        strcat(operand_number_in_binary, first_operand_binary);
        strcat(operand_number_in_binary, "000");
        free(first_operand_binary);
    }
    else if(second_operand.type == REGISTER_PTR || second_operand.type == REGISTER) {
        second_operand_binary = register_name_to_binary(second_operand.name);
        strcat(operand_number_in_binary, "000");
        strcat(operand_number_in_binary, second_operand_binary);
        free(second_operand_binary);
    }
    strcat(operand_number_in_binary, "100");
    return operand_number_in_binary;
}

/**
 * Converts a register name to its binary representation.
 *
 * @param register_name The name of the register to be converted.
 * @return A string containing the binary representation of the register name.
 *         Returns NULL if memory allocation fails or if the register name is invalid.
 */
char* register_name_to_binary(const char* register_name) {
    char* register_number_in_binary = malloc(SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(register_number_in_binary == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    memset(register_number_in_binary, '\0', SIZE_OF_REGISTER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(strncmp(register_name, "*", 1) == 0)
        register_name++;
    if(strcmp(register_name, "r0") == 0) {
        strcpy(register_number_in_binary, "000");
    }
    else if(strcmp(register_name, "r1") == 0) {
        strcpy(register_number_in_binary, "001");
    }
    else if( strcmp(register_name, "r2") == 0) {
        strcpy(register_number_in_binary, "010");
    }
    else if(strcmp(register_name, "r3") == 0) {
        strcpy(register_number_in_binary, "011");
    }
    else if(strcmp(register_name, "r4") == 0) {
        strcpy(register_number_in_binary, "100");
    }
    else if(strcmp(register_name, "r5") == 0) {
        strcpy(register_number_in_binary, "101");
    }
    else if(strcmp(register_name, "r6") == 0) {
        strcpy(register_number_in_binary, "110");
    }
    else if(strcmp(register_name, "r7") == 0) {
        strcpy(register_number_in_binary, "111");
    }
    else {
        free(register_number_in_binary);
        return NULL;
    }
    return register_number_in_binary;
}

/**
 * Handles errors by printing an error message along with the file name and line number where the error occurred.
 * Exits the program after printing the error message.
 *
 * @param error_message The error message to be printed.
 * @param file_name The name of the file where the error occurred.
 * @param line_counter The line number in the file where the error occurred.
 */
void error_handler(const char *error_message, const char *file_name, const int line_counter) {
    printf("%s in file %s at line %d\n", error_message, file_name, line_counter);
}

/**
 * Converts a decimal number to a binary string representation.
 *
 * @param integer The decimal number to be converted.
 * @return A string containing the binary representation of the decimal number.
 *         Returns NULL if memory allocation fails.
 */
char* decimal_to_binary(const int integer) {
    int i;
    unsigned int mask;
    char* binary_string = malloc(SIZE_OF_NUMBER_IN_BITS + LENGTH_OF_NULL_TERMINATOR);
    if(binary_string == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    binary_string[SIZE_OF_NUMBER_IN_BITS] = '\0';
    mask = 1 << (SIZE_OF_NUMBER_IN_BITS - 1);
    for(i = 0; i < SIZE_OF_NUMBER_IN_BITS; i++) {
        if(integer & mask)
            binary_string[i] = '1';
        else
            binary_string[i] = '0';
        mask >>= 1;
    }
    return binary_string;
}

/**
 * Converts a binary string to its octal representation.
 *
 * @param binary_str The binary string to be converted. Must be 15 bits long.
 * @return A string containing the octal representation of the binary string.
 *         Returns NULL if memory allocation fails or if the binary string is not 15 bits long.
 */
char* binary_to_octal(const char *binary_str) {
    int  i, j, value;
    char *octal_str;

    if (strlen(binary_str) != SIZE_OF_REGISTER_IN_BITS) {
        fprintf(stdout, "Error: binary_str must be 15 bits long.\n"); // for testing
        return NULL;
    }

    octal_str = malloc(SIZE_OF_WORD_IN_OCTAL + LENGTH_OF_NULL_TERMINATOR);
    if (octal_str == NULL) {
        printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }

    octal_str[SIZE_OF_WORD_IN_OCTAL] = '\0';
    for (i = 0; i < SIZE_OF_WORD_IN_OCTAL; i++) {
        value = 0;
        for (j = 0; j < 3; j++)
            value = (value << 1) | (binary_str[3 * i + j] - '0');
        octal_str[i] = value + '0';
    }
    return octal_str;
}

/**
 * Frees the memory allocated for the label array.
 *
 * @param array A pointer to the label array to be freed.
 */
void free_label_array(label_array *array) {
    int i;
    if (array != NULL) {
        for (i = 0; i < array->rep; i++) {
            free(array->label_element[i].name);
        }
        free(array->label_element);
        free(array);
    }
}

/**
 * Resets the types of the given operands to UNKNOWN.
 *
 * @param first_operand A pointer to the first operand whose type will be reset.
 * @param second_operand A pointer to the second operand whose type will be reset.
 */
void reset_opernads_type( operand *first_operand, operand *second_operand) {
    first_operand->type = UNKNOWN;
    second_operand->type = UNKNOWN;
}

/**
 * Checks if the immediate operand value is out of bounds.
 *
 * @param operand The operand containing the immediate value to be checked.
 * @return Returns 1 if the immediate value is out of bounds, otherwise returns 0.
 */
int is_immidiate_out_of_bounds(const operand operand) {
    const char *operand_name = operand.name;
    operand_name++;
    if(operand_name[0] == '+' || operand_name[0] == '-') {
        operand_name++;
    }
    if(atoi(operand_name) > MAX_POSSIBLE_NUMBER || atoi(operand_name) < MIN_POSSIBLE_NUMBER) {
        return 1;
    }

    return 0;
}

/**
 * Determines the type of a line in the source code.
 *
 * @param str Pointer to the line string.
 * @return An integer representing the type of the line.
 */
int line_location(char *str) {
    int i = 0;
    const char *ptr_to_firstWord = skip_whitespace(str);
    if(ptr_to_firstWord){
        if (isalpha(ptr_to_firstWord[0])) {
            while (ptr_to_firstWord[i] && ptr_to_firstWord[i] != ':') {
                if (!isalnum(ptr_to_firstWord[i])) {
                    break;
                }
                i++;
            }
            if (ptr_to_firstWord[i] == ':' && i > 0) {
                return LABEL_DEFINITION;
            }
        }
        if (strncmp(ptr_to_firstWord, ".data ", 6) == 0)
            return DATA;
        if (strncmp(ptr_to_firstWord, ".string ", 8) == 0)
            return STRING;
        if (strncmp(ptr_to_firstWord, ".entry ", 7) == 0)
            return ENTRY;
        if (strncmp(ptr_to_firstWord, ".extern ", 8) == 0)
            return EXTERN;
        if(is_command(ptr_to_firstWord))
            return INSTRUCTION;
    }
    return -1;
}

/**
 * Checks if a given command is in the list of valid commands.
 *
 * @param command Pointer to the command string to check.
 * @return 1 if the command is found in the commands_list, 0 otherwise.
 */
int is_command(const char *command) {
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
int which_command(const char *command) {
    int i;
    int str_length = 0;

    for (i = 0; (command[i] && command[i] != ' ' && command[i] != '\t' && command[i] != ',' && command[i] != '\n'  && command[i] != '\r'); i++) {
        str_length++;
    }
    for (i = 0; i < 16; i++) {
        if (strlen(commands_list[i]) == str_length && !strncmp(commands_list[i], command, str_length)) {
            return i;
        }
    }
    return unknown_command;
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
 * @return 0 if the command was successfully analyzed, 1 otherwise.
 */
int analyze_command(char *ptr, const int command, int *L, char *word_in_binary, const char *file_name, const int line_counter) {
    int is_error = 0;
    char *first_operand_name = NULL, *second_operand_name = NULL,  *command_in_binary;
    operand first_operand, second_operand;

    if (parse_instruction(ptr, command, &first_operand_name, &second_operand_name, file_name, line_counter))
        is_error = 1;

    if (first_operand_name) {
        first_operand.name = malloc(strlen(first_operand_name) + LENGTH_OF_NULL_TERMINATOR);
        if (first_operand.name) {
            strcpy(first_operand.name, first_operand_name);
        } else {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(first_operand_name);
            exit(1);
        }
    }

    if (second_operand_name) {
        second_operand.name = malloc(strlen(second_operand_name) + LENGTH_OF_NULL_TERMINATOR);
        if (second_operand.name) {
            strcpy(second_operand.name, second_operand_name);
        } else {
            printf("%s\n", ERROR_FAILED_TO_ALLOCATE_MEM);
            free(first_operand_name);
            free(second_operand_name);
            exit(1);
        }
    }

    first_operand.type = UNKNOWN;
    second_operand.type = UNKNOWN;
    if (command <= 15) {
        (*L)++;
        if (command <= 13) {
            if (!is_error) {
                if(analyze_operand(&first_operand)) {
                    error_handler(ERROR_INVALID_FIRST_OPERAND, file_name, line_counter);
                    is_error = 1;
                }
                if (first_operand.type == IMMEDIATE) {
                    if(is_immidiate_out_of_bounds(first_operand)) {
                        error_handler(ERROR_OPERAND_VALUE_OUT_OF_BOUNDS, file_name, line_counter);
                        is_error = 1;
                    }
                }
            }

            (*L)++;
            if (command <= 4) {
                if (!is_error) {
                    if (analyze_operand(&second_operand)) {
                        error_handler(ERROR_INVALID_SECOND_OPERAND, file_name, line_counter);
                        is_error = 1;
                    }
                    if (second_operand.type == IMMEDIATE) {
                        if(is_immidiate_out_of_bounds(second_operand)) {
                            error_handler(ERROR_OPERAND_VALUE_OUT_OF_BOUNDS, file_name, line_counter);
                            is_error = 1;
                        }
                    }
                }
                if (!(first_operand.type == REGISTER_PTR && second_operand.type == REGISTER) &&
                    !(first_operand.type == REGISTER && second_operand.type == REGISTER_PTR) &&
                    !(first_operand.type == REGISTER && second_operand.type == REGISTER) &&
                    !(first_operand.type == REGISTER_PTR && second_operand.type == REGISTER_PTR))
                    (*L)++;
            }
        }
    }

    if (is_error) {
        if (first_operand_name) {
            free(first_operand.name);
            free(first_operand_name);
        }
        if (second_operand_name) {
            free(second_operand.name);
            free(second_operand_name);
        }
        return 1;
    }
    command_in_binary = command_to_binary(command, first_operand, second_operand);
    strcpy(word_in_binary, command_in_binary);
    free(command_in_binary);
    if (word_in_binary == NULL) {
        error_handler(ERROR_BINARY_VERSION_COULD_NOT_BE_CREATED, file_name, line_counter);
        return 1;
    }
    if (first_operand_name) {
        free(first_operand.name);
        free(first_operand_name);
    }
    if (second_operand_name) {
        free(second_operand.name);
        free(second_operand_name);
    }
    return 0;
}

/**
 * Analyzes an operand and determines its type.
 *
 * This function examines the operand string to determine if it is an immediate value,
 * a register, a register pointer, or a label address(label value).
 *
 * @param operand Pointer to the operand structure to analyze.
 * @return 0 if the operand was successfully analyzed, 1 otherwise.
 */
int analyze_operand(operand *operand) {
    int i;
    const char *operand_name = operand->name;

    operand->type = UNKNOWN;
    switch (operand_name[0]) {
        case '#':
            if(!(operand_name[1] == '\0' || isdigit(operand_name[1]) || operand_name[1] == '-' || operand_name[1] == '+')) {
                operand->type = UNKNOWN;
                return 1;
            }
            for(i = 2; operand_name[i] != '\0'; i++) {
                if(!isdigit(operand_name[i])) {
                    operand->type = UNKNOWN;
                    return 1;
                }
            }
            operand->type = IMMEDIATE;
        return 0;
        case '*':
            if(which_register(operand_name) != -1) {
                operand->type = REGISTER_PTR;
                return 0;
            }
        return 1;
        case 'r':
            if(which_register(operand_name) != -1) {
                operand->type = REGISTER;
                return 0;
            }
        default: {
            if(strlen(operand_name) <= MAX_LENGTH_OF_LABEL_VALUE && isalpha(operand_name[0])) {
                for(i = 1; i < strlen(operand_name); i++) {
                    if(!isalnum(operand_name[i])) {
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