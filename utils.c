#include "utils.h"

#include "assembler_stage_2.h"

const char *reserved_words[] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/, "dec"/*8*/,
    "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/,
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

/**
 * Creates a new file name by appending a specified file type extension to the given file name.
 *
 * @param str The original file name.
 * @param type The file type extension to be appended.
 * @return A new string containing the original file name with the appended file type extension.
 *         Returns NULL if memory allocation fails.
 */
char *fileTypeCreator(const char *str, const char *type) {
    char *file_type = malloc(strlen(str) + strlen(type) + 1);
    if (!file_type) {
        // Handle memory allocation failure
        return NULL;
    }
    strcpy(file_type, str);
    strcat(file_type, type);
    return file_type;
}

/**
 * Inspects the given file pointer to check if the file is readable.
 *
 * @param file A pointer to the file to be inspected.
 * @return Returns 1 if the file is readable, otherwise returns 0.
 */
int fileInspection(const FILE *file) {
    if (file == NULL) {
        printf("Failed to read file");
        return 0;
    }
    return 1;
}

/**
 * Moves the pointer to the first non-whitespace character in the given string.
 *
 * @param str The input string to be processed.
 * @return A pointer to the first non-whitespace character in the string.
 */
char *firstWordInLine(char *str) {
    if (str == NULL) {
        return NULL;
    }
    while (isspace(*str) || *str == '\t') {
        if(*str == '\n')
            return str;
        str++;
    }
    if (*str == '\0') {
        return NULL;
    }
    return str;
}

/**
 * Counts the length of the first word in the given string
 * while ignoring whitespace characters and colons.
 *
 * @param str The input string to be processed.
 * @return The length of the first word in the string.
 */
int firstWordLengthCounter(const char *str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)) && *(str + counter) != ':'; counter++) {
    };
    return counter;
}

/**
 * Counts the length of the operand in the given string
 * while ignoring whitespace characters and commas.
 *
 * @param str The input string to be processed.
 * @return The length of the operand in the string.
 */
int operandLengthCounter(const char *str) {
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
int isReservedWord(char *word, const int length) {
    int i, size;

    size = sizeof(reserved_words) / sizeof(reserved_words[0]);
    for (i = 0; i < size; i++) {
        if (strncmp(word, reserved_words[i], length) == 0 && isEndOfLine(word + length)) {
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
int isEndOfLine(char *str) {
    const char *new_str = firstWordInLine(str);

    if (new_str == NULL)
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
 */
int parseData(const char *input, int **array, int *size, int *DC, const char *file_name, const int line_counter) {
    const int DATA_COMMAND_LENGTH = strlen(".data ");
    const char *dataStart = NULL;
    int count = 0, index = 0, commaFlag = 0, numberFlag = 0;
    char *ptr;
    *size = 0;

    dataStart = strstr(input, ".data ");

    if (dataStart)
        dataStart += DATA_COMMAND_LENGTH;
    else {
        *array = NULL;
        *size = 0;
        return 1;
    }

    ptr = (char *) dataStart;
    while (*ptr) {
        if(*ptr != '-' && *ptr != '+' && !isdigit(*ptr) && *ptr != ',' && *ptr != '\n' && !isspace(*ptr)) { /* if character is not a digit, comma, newline, or whitespace*/
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            return 1;
        }
        if(numberFlag == 1 && !isspace(*ptr) && *ptr != ',' && *ptr != '\n') { /* if a number is found and the next character is not a whitespace, comma, or newline*/
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            return 1;
        }
        if (*ptr == '\n') {
            if(commaFlag == 1) { // if a comma is found at the end of the line
                error_handler("ERROR_MISSING_DATA_VALUE", file_name, line_counter);
                return 1;
            }
            if(count == 0) { /* if no data values are found*/
                error_handler("ERROR_MISSING_DATA_VALUE", file_name, line_counter);
                return 1;
            }
            break;
        }
        if(isspace(*ptr)) {
            ptr++;
            continue;
        }
        if(commaFlag == 1) {
            if (*ptr == ',') { /* if multiple commas are found*/
                error_handler("ERROR_MULTIPLE_COMMA_FOUND", file_name, line_counter);
                return 1;
            }
        }
        if (*ptr == ',') {
            commaFlag = 1;
            numberFlag = 0;
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            count++;
            commaFlag = 0;
            numberFlag = 1;
            while(*ptr == '-' || *ptr == '+')
                ptr++;
            while (isdigit(*ptr))
                ptr++;
        } else
            ptr++;
    }

    *array = (int *) malloc(count * sizeof(int));
    if (*array == NULL) {
        error_handler("Failed to allocate memory", file_name, line_counter);
        *size = 0;
        return 1;
    }

    ptr = (char *) dataStart;
    while (*ptr) {
        if (*ptr == '\n')
            break;
        while (isspace(*ptr)) {
            ptr++;
        }
        if (*ptr == ',' || *ptr == '\0') {
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            (*array)[index++] = strtol(ptr, &ptr, 10);
        } else
            ptr++;
    }
    *size = count;
    *DC += count;
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
 */
int parseString(const char *input, int **array, int *size, int *DC, const char *file_name, const int line_counter) {
    const int STRING_COMMAND_LENGTH = strlen(".string ");
    const char *stringStart;
    char *ptr;
    int count = 0, index = 0;
    *size = 0;

    stringStart = strstr(input, ".string ");
    if (stringStart)
        stringStart += STRING_COMMAND_LENGTH;
    else {
        *array = NULL;
        *size = 0;
        return 1;
    }
    ptr = (char *) stringStart;
    while (*stringStart && *stringStart != '"') {
        if(isalpha(*stringStart) || isdigit(*stringStart)){ /* if a character is found outside of quotation marks*/
            error_handler("ERROR_INVALID_CHARATER_FOUND_OUTSIDE_OF_QUOTATION_MARK", file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        if(*stringStart == '\n') { /* if no string is found*/
            error_handler("ERROR_NO_STRING_FOUND", file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        count++;
        stringStart++;
    }

    if (*stringStart != '"') { /* if the closing quotation mark is missing. */
        error_handler("ERROR_MISSING_OPENING_QUOTATION_MARK", file_name, line_counter);
        *array = NULL;
        *size = 0;
        return 1;
    }
    ptr = (char *) (stringStart + 1);
    while (*ptr && *ptr != '"') {
        count++;
        ptr++;
    }
    ptr++; /* skip the closing quotation mark */

    while(*ptr != '\n') {
        if(*ptr != ' ' && *ptr != '\t') { /* if a character is found outside of quotation marks. */
            error_handler("ERROR_INVALID_CHARATER_FOUND_OUTSIDE_OF_QUOTATION_MARK", file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        ptr++;
    }

    *array = (int *) malloc((count + 1) * sizeof(int));
    if (*array == NULL) {
        error_handler("Failed to allocate memory", file_name, line_counter);
        *size = 0;
        return 1;
    }

    ptr = (char *) (stringStart + 1);
    for (index = 0; index < count; index++) {
        (*array)[index] = (int) *ptr;
        (*DC)++;
        ptr++;
    }

    (*array)[index] = 0;
    (*DC)++;
    *size = count + 1;
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
 */
int parseCommandString(char *input_ptr, const int command, char **source, char **dest, const char *file_name,
                       const int line_counter) {
    int command_length, first_operand_length , second_operand_length;
    char *first_operand = NULL, *second_operand = NULL;

    if (command < 15)
        command_length = 3;
    else
        command_length = 4;

    *source = NULL;
    *dest = NULL;

    input_ptr += command_length;
    input_ptr = firstWordInLine(input_ptr);

    if (command == 14 || command == 15) {
        if(*input_ptr != '\n') { /* redundent characters after stop or rts command */
            error_handler("REDUNDENT_CHARACTERS_AFTER_COMMAND", file_name, line_counter);
            return 1;
        }
        return 0;
    }
    if (strncmp(input_ptr, ",", 1) == 0 || *input_ptr == '\n') { /* lacks the first operand */
        error_handler("ERROR_MISSING_FIRST_OPERAND", file_name, line_counter);
        return 1;
    }
    if(!isalpha(*input_ptr) && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid first operand */
        error_handler("ERROR_INVALID_FIRST_OPERAND", file_name, line_counter);
        return 1;
    }
    first_operand_length = operandLengthCounter(input_ptr);
    first_operand = (char *) malloc(first_operand_length + 1);
    if (first_operand == NULL) {
        error_handler( "Failed to allocate memory", file_name, line_counter);
        return 1;
    }
    strncpy(first_operand, input_ptr, first_operand_length);
    first_operand[first_operand_length] = '\0';

    if (command < 5 /*mov, cmp, add, sub, lea*/) {
        input_ptr += first_operand_length;
        input_ptr = firstWordInLine(input_ptr);
        if (strncmp(input_ptr, ",", 1) == 0) {
            input_ptr++;
            input_ptr = firstWordInLine(input_ptr);
        }
        else { /* missing a comma between the two operands */
            error_handler("ERRROR_MISSING_A_COMMA", file_name, line_counter);
            return 1;
        }
        input_ptr = firstWordInLine(input_ptr);
        if (strncmp(input_ptr, ",", 1) == 0) { /* too many commas */
            error_handler("ERROR_TOO_MANY_COMMAS", file_name, line_counter);
            free(first_operand);
            return 1;
        }
        if(*input_ptr == '\n') { /* lacks the second operand */
            error_handler("ERROR_MISSING_SECOND_OPERAND", file_name, line_counter);
            free(first_operand);
            return 1;
        }
        if(isalpha(*input_ptr) == 0 && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid second operand */
            error_handler("ERROR_INVALID_SECOND_OPERAND", file_name, line_counter);
            free(first_operand);
            return 1;
        }

        second_operand_length = operandLengthCounter(input_ptr);
        second_operand = (char *) malloc(second_operand_length + 1);
        if (second_operand == NULL) {
            error_handler( "Failed to allocate memory", file_name, line_counter);
            free(first_operand);
            return 1;
        }
        strncpy(second_operand, input_ptr, second_operand_length);
        second_operand[second_operand_length] = '\0';

        input_ptr += second_operand_length;
        input_ptr = firstWordInLine(input_ptr);
        if(*input_ptr != '\n') { /* redundent chraters after seond operand*/
            error_handler("ERROR_REDUNDENT_CHARACTERS_AFTER_SECOND_OPERAND", file_name, line_counter);
            free(first_operand);
            free(second_operand);
            return 1;
        }
    }
    if(command > 4 && command < 14) {
        input_ptr += first_operand_length;
        input_ptr = firstWordInLine(input_ptr);
        if(*input_ptr != '\n') { /* redundent charaters after first operand*/
            error_handler("ERROR_REDUNDENT_CHARACTERS_AFTER_FIRST_OPERAND", file_name, line_counter);
            free(first_operand);
            free(second_operand);
            return 1;
        }
    }

    *source = (char *) malloc(first_operand_length + 1);
    if (*source == NULL) {
        error_handler( "Failed to allocate memory", file_name, line_counter);
        free(first_operand);
        if (second_operand)
            free(second_operand);
        return 1;
    }
    strncpy(*source, first_operand, first_operand_length + 1);
    free(first_operand);
    first_operand = NULL;

    if (command < 5) {
        *dest = (char *) malloc(second_operand_length + 1);
        if (*dest == NULL) {
            error_handler( "Failed to allocate memory", file_name, line_counter);
            free(*source);
            free(second_operand);
            return 1;
        }
        strncpy(*dest, second_operand, second_operand_length + 1);
        free(second_operand);
        second_operand = NULL;

    }
    return 0;
}

int parseCommandString_stage_2(char *input_ptr, const int command, char **source, char **dest, const char *file_name, const int line_counter) {
    int command_length, first_operand_length , second_operand_length;
    char *first_operand = NULL, *second_operand = NULL;

    if (command < 15)
        command_length = 3;
    else
        command_length = 4;

    *source = NULL;
    *dest = NULL;

    input_ptr += command_length;
    input_ptr = firstWordInLine(input_ptr);
    if (command == 14 || command == 15) {
        return 0;
    }
    first_operand_length = operandLengthCounter(input_ptr);
    first_operand = (char *) malloc(first_operand_length + 1);
    if (first_operand == NULL) {
        error_handler( "Failed to allocate memory", file_name, line_counter);
        return 1;
    }
    strncpy(first_operand, input_ptr, first_operand_length);
    first_operand[first_operand_length] = '\0';

    if (command < 5 /*mov, cmp, add, sub, lea*/) {
        input_ptr += first_operand_length;
        input_ptr = firstWordInLine(input_ptr);
        if (strncmp(input_ptr, ",", 1) == 0) {
            input_ptr++;
            input_ptr = firstWordInLine(input_ptr);
        }
        input_ptr = firstWordInLine(input_ptr);
        second_operand_length = operandLengthCounter(input_ptr);
        second_operand = (char *) malloc(second_operand_length + 1);
        strncpy(second_operand, input_ptr, second_operand_length);
        second_operand[second_operand_length] = '\0';
        input_ptr += second_operand_length;
        input_ptr = firstWordInLine(input_ptr);
    }/*
    if(command > 4 && command < 14) {
        input_ptr += first_operand_length;
        input_ptr = firstWordInLine(input_ptr);
    }*/
    *source = (char *) malloc(first_operand_length + 1);
    if (*source == NULL) {
        error_handler( "Failed to allocate memory", file_name, line_counter);
        free(first_operand);
        if (second_operand)
            free(second_operand);
        return 1;
    }
    strncpy(*source, first_operand, first_operand_length + 1);
    free(first_operand);
    first_operand = NULL;

    if (command < 5) {
        *dest = (char *) malloc(second_operand_length + 1);
        if (*dest == NULL) {
            error_handler( "Failed to allocate memory", file_name, line_counter);
            free(*source);
            free(second_operand);
            return 1;
        }
        strncpy(*dest, second_operand, second_operand_length + 1);
        free(second_operand);
        second_operand = NULL;
    }
    return 0;
}


/**
 * Allocates memory for a label array and initializes its elements.
 *
 * @param size The initial size of the label array.
 * @return A pointer to the allocated label array.
 *         Exits the program if memory allocation fails.
 */
label_array *labelArrayAllocator(const int size) {
    label_array *array = (label_array *) malloc(sizeof(label_array));
    array->label_element = (label *) malloc(size * sizeof(label));
    if (array->label_element == NULL) {
        printf( "Failed to reallocate memory");
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
 * @param file_name The name of the file where the label is being added.
 * @param line_counter The line number in the file where the label is being added.
 * @return Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int labelArrayAdd(label_array *array, const char *name, const int address, const line_type label_characteristic, const char* file_name,
                  int line_counter) {
    label *new_label;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        if (length_of_array == 0)
            length_of_array = 1;
        else
            length_of_array = (length_of_array) * 2;
        new_label = realloc(array->label_element, length_of_array * sizeof(label));
        if (!new_label) {
            error_handler( "Failed to reallocate memory", file_name, line_counter);
            return 1;
        }
        array->label_element = new_label;
        array->length = length_of_array; /* Update the length in the array structure*/
    }
    array->label_element[number_of_reps].name = malloc(strlen(name) + 1);
    if (array->label_element[number_of_reps].name == NULL) {
        error_handler( "Failed to reallocate memory", file_name, line_counter);
        return 1;
    }
    array->label_element[number_of_reps].address = address;
    strcpy(array->label_element[number_of_reps].name, name);
    array->label_element[number_of_reps].characteristic = label_characteristic;

    array->rep++;
    return 0;
}

/**
 * Checks if a given name exists in the label array.
 *
 * @param label_table A pointer to the label array.
 * @param name The name to be checked.
 * @return Returns 1 if the name exists in the label array, otherwise returns 0.
 */
int isLabel(const label_array *label_table, const char *name) {
    int i;
    for (i = 0; i < label_table->rep; i++) {
        if (strcmp(name, label_table->label_element[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Converts a command and its operands into a binary string representation.
 *
 * @param command The command value indicating the type of command.
 * @param first_operand The first operand of the command.
 * @param second_operand The second operand of the command.
 * @param L The length of the command.
 * @return A string containing the binary representation of the command and its operands.
 *         Returns NULL if memory allocation fails.
 */
char *command_to_binary(const int command, const operand first_operand, const operand second_operand, const int L) {
    char *str;
    str = (char *) malloc(15 * sizeof(char) + 1);
    if (str == NULL) {
        printf("Failed to allocate memory\n");

        return NULL;
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


void convert_operands_to_binary(operand first_operand, operand second_operand, char **first_operand_in_binary, char **second_operand_in_binary, const label_array *label_table) {
    *first_operand_in_binary = (char *)malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if (!*first_operand_in_binary) {
        // handle error
        return;
    }
    memset(*first_operand_in_binary, 0, SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);

    *second_operand_in_binary = (char *)malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if (!*second_operand_in_binary) {
        // handle error
        return;
    }
    memset(*second_operand_in_binary, 0, SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    switch(first_operand.type) {
        case IMMEDIATE:
            *first_operand_in_binary = immediate_operand_to_binary(first_operand);
        break;
        case LABEL_VALUE:
            *first_operand_in_binary = label_operand_to_binary(first_operand, label_table);
        break;
        case REGISTER_PTR:
            *first_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
        break;
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
            if(first_operand.type == REGISTER_PTR || first_operand.type == REGISTER)
                break;
            else {
                *second_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
                break;
            }
        case REGISTER:
            if(first_operand.type == REGISTER_PTR || first_operand.type == REGISTER)
                break;
            else {
                *second_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
                break;
            }
        case UNKNOWN:
            break;
    }
}

char* label_operand_to_binary(operand operand, label_array *label_table) {
    int i, operand_address = -2;
    char* operand_address_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    for(i = 0; i < label_table->rep; i++) {
        if(strcmp(operand.name, label_table->label_element[i].name) == 0) {
            operand_address = label_table->label_element[i].address;
            break;
        }
    }
    if(operand_address == -1) {
        /* speical case for external label */
        operand_address = 0;
        strcpy(operand_address_in_binary, decimalToBinary(operand_address));
        strcat(operand_address_in_binary, "001");
        return operand_address_in_binary;
    }
    if(operand_address == -2)//need to make error for this case
        return NULL;

    strcpy(operand_address_in_binary, decimalToBinary(operand_address));
    strcat(operand_address_in_binary, "010");
    return operand_address_in_binary;
}

char* immediate_operand_to_binary(operand operand) {
    char* operand_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(!operand_number_in_binary) {// handle error
        return NULL;
    }
    char* operand_name = malloc( strlen(operand.name) + 1);
    if(!operand_name) {// handle error
        return NULL;
    }
    strcpy(operand_name, operand.name);
    operand_name++;
    operand_name = firstWordInLine(operand_name);
    if (isdigit(operand_name[0]) || ((*operand_name == '-' || *operand_name == '+') && isdigit(*(operand_name + 1)))) {
        strcpy(operand_number_in_binary, decimalToBinary(atoi(operand_name)));
        strcat(operand_number_in_binary, "100");
        return operand_number_in_binary;
    }
    return NULL;
}

char* register_operand_to_binary(operand first_operand, operand second_operand) {
    char* operand_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(!operand_number_in_binary) {// handle error
        return NULL;
    }
    memset(operand_number_in_binary, 0, SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    strcat(operand_number_in_binary, "000000");
    if(second_operand.type == UNKNOWN) {
        strcat(operand_number_in_binary, "000");
        strcat(operand_number_in_binary, register_name_to_binary(first_operand.name));
    }
    else if(first_operand.type == second_operand.type || (first_operand.type == REGISTER_PTR && second_operand.type == REGISTER) || (first_operand.type == REGISTER && second_operand.type == REGISTER_PTR)) {
        strcat(operand_number_in_binary, register_name_to_binary(first_operand.name));
        strcat(operand_number_in_binary, register_name_to_binary(second_operand.name));
    }
    else if(first_operand.type == REGISTER_PTR || first_operand.type == REGISTER) {
        strcat(operand_number_in_binary, register_name_to_binary(first_operand.name));
        strcat(operand_number_in_binary, "000");
    }
    else if(second_operand.type == REGISTER_PTR || second_operand.type == REGISTER) {
        strcat(operand_number_in_binary, "000");
        strcat(operand_number_in_binary, register_name_to_binary(second_operand.name));
    }
    strcat(operand_number_in_binary, "100");
    return operand_number_in_binary;
    return NULL;
}

char* register_name_to_binary(const char* register_name) {
    char* register_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(!register_number_in_binary) {// handle error
        return NULL;
    }
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
    printf("Error: %s in file %s at line %d\n", error_message, file_name, line_counter);
    exit(1);
}

char* decimalToBinary(int integer) {
    int i,  number_in_bits = SIZE_OF_NUMBER_IN_BITS;
    unsigned int mask;
    char* binary_string = (char*)malloc(number_in_bits + 1);
    if(!binary_string) {
        return NULL;
    }
    binary_string[number_in_bits] = '\0';
    mask = 1 << (number_in_bits - 1);
    for(i = 0; i < number_in_bits; i++) {
        if(integer & mask)
            binary_string[i] = '1';
        else
            binary_string[i] = '0';
        mask >>= 1;
    }
    return binary_string;
}

void ob_file_usher( FILE* ob_file, const char* file_name, const int IC, const int DC) {
    fprintf(ob_file, "%d %d\n", IC, DC);
    fclose(ob_file);
}