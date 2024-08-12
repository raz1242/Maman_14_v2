#include "utils.h"

#include "assembler_stage_2.h"

const char *reserved_words[] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/, "dec"/*8*/,
    "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/,
    ".data", ".string", ".entry", ".extern", "macr", "endmacr", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};
char *register_list[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
/**
 * Creates a new file name by appending a specified file type extension to the given file name.
 *
 * @param str The original file name.
 * @param type The file type extension to be appended.
 * @return A new string containing the original file name with the appended file type extension.
 *         Returns NULL if memory allocation fails.
 */
char *file_name_extender(const char *str, const char *type) {
    char *file_type = malloc(strlen(str) + strlen(type) + 1);
    if (file_type == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
int parse_dot_data2(const char *input, int **array, int *size, int *DC, const char *file_name, const int line_counter) {
    const int DATA_COMMAND_LENGTH = strlen(".data ");
    const char *dataStart = NULL;
    int count = 0, index = 0, commaFlag = 0, numberFlag = 0, current_number = 0;
    int *temp_array;
    char *ptr;

    *size = 0;

    dataStart = strstr(input, ".data ");

    if (dataStart)
        dataStart += DATA_COMMAND_LENGTH;
    else {
        *array = NULL;
        return 1;
    }

    ptr = (char *) dataStart;
    while (*ptr) {
        if(*ptr != '-' && *ptr != '+' && !isdigit(*ptr) && *ptr != ',' && *ptr != '\n' && *ptr != '\r' && !isspace(*ptr)) { /* if character is not a digit, comma, newline, or whitespace*/
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            return 1;
        }
        if(numberFlag == 1 && !isspace(*ptr) && *ptr != ',' && *ptr != '\n' && *ptr != '\r') { /* if a number is found and the next character is not a whitespace, comma, or newline*/
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            return 1;
        }
        if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0') {
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

    *array = NULL;

    ptr = (char *) dataStart;
    while (*ptr) {
        if (*ptr == '\n' || *ptr == '\r')
            break;
        while (isspace(*ptr)) {
            ptr++;
        }
        if (*ptr == ',' || *ptr == '\0') {
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            current_number = strtol(ptr, &ptr, 10);
            if (current_number > MAX_POSSIBLE_NUMBER || current_number < MIN_POSSIBLE_NUMBER) {
                error_handler("ERROR_NUMBER_IS_OUT_OF_MACHINE_RANGE", file_name, line_counter);
            } else {
                temp_array = realloc(*array, (index + 1) * sizeof(int));
                if (temp_array == NULL) {
                    printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
                    free(*array);
                    exit(1);
                }
                *array = temp_array;
                (*array)[index++] = current_number;
            }
        } else {
            ptr++;
        }
    }

    *size = index;
    *DC += index;
    return 0;
}

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
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            free(*array);
            return 1;
        }
        if (numberFlag == 1 && !isspace(*ptr) && *ptr != ',' && *ptr != '\n' && *ptr != '\r') {
            error_handler("INVALID_DATA_VALUE", file_name, line_counter);
            free(*array);
            return 1;
        }
        if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0') {
            if (commaFlag == 1) {
                error_handler("ERROR_MISSING_DATA_VALUE", file_name, line_counter);
                free(*array);
                return 1;
            }
            if (index == 0) {  /* No valid data found */
                error_handler("ERROR_MISSING_DATA_VALUE", file_name, line_counter);
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
            error_handler("ERROR_MULTIPLE_COMMA_FOUND", file_name, line_counter);
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
                error_handler("ERROR_NUMBER_IS_OUT_OF_MACHINE_RANGE", file_name, line_counter);
            } else {
                int *temp_array = realloc(*array, (index + 1) * sizeof(int));
                if (temp_array == NULL) {
                    printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
            error_handler("ERROR_STRING_MUST_START_WITH_QUOTATION_MARK", file_name, line_counter);
            *array = NULL;
            *size = 0;
            return 1;
        }
        if(*stringStart == '\n' || *stringStart == '\r') { /* if no string is found*/
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

    while(*ptr != '\n'  && *ptr != '\r') {
        if(*ptr != ' ' && *ptr != '\t') { /* if a character is found outside of quotation marks. */
            error_handler("ERROR_INVALID_CHARATER_FOUND_OUTSIDE_OF_QUOTATION_MARKS", file_name, line_counter);
            break;
        }
        ptr++;
    }

    *array = (int *) malloc((count + 1) * sizeof(int));
    if (*array == NULL) { /* if memory allocation fails. */
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        *size = 0;
        exit(1);
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
 * @return  Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_instruction(char *input_ptr, const int command, char **source, char **dest, const char *file_name,
                       const int line_counter) {// make sure the function stop after command 14 and 15
    int command_length, first_operand_length, second_operand_length = 0;
    char *first_operand_name = NULL, *second_operand_name = NULL;

    if (command < 15)
        command_length = 3;
    else
        command_length = 4;

    *source = NULL;
    *dest = NULL;

    input_ptr = skip_to_next_word(input_ptr, command_length);

    if (command == 14 || command == 15) {
        if(*input_ptr != '\n' && *input_ptr != '\r') { /* redundent characters after stop or rts command */
            error_handler("ERROR_REDUNDENT_CHARACTERS_AFTER_COMMAND", file_name, line_counter);
            return 1;
        }
        return 0;
    }
    if(*input_ptr == '\n' || *input_ptr == '\r') { /* lacks the first operand */
        error_handler("ERROR_MISSING_FIRST_OPERAND", file_name, line_counter);
        return 1;
    }
    if (strncmp(input_ptr, ",", 1) == 0) { /* lacks the first operand */
        error_handler("ERROR_REDUNDENT_COMMA_AFTER_COMMAND", file_name, line_counter);
        return 1;
    }
    if(!isalpha(*input_ptr) && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid first operand */
        error_handler("ERROR_INVALID_FIRST_OPERAND", file_name, line_counter);
        return 1;
    }
    first_operand_length = operand_length_counter(input_ptr);
    first_operand_name = (char *) malloc(first_operand_length + 1);
    if (first_operand_name == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    strncpy(first_operand_name, input_ptr, first_operand_length);
    first_operand_name[first_operand_length] = '\0';

    if (command < 5 /*mov, cmp, add, sub, lea*/) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if (strncmp(input_ptr, ",", 1) == 0) {
            input_ptr = skip_to_next_word(input_ptr, LENGTH_OF_COMMA);
        }
        else { /* missing a comma between the two operands */
            error_handler("ERROR_MISSING_A_COMMA", file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if (strncmp(input_ptr, ",", 1) == 0) { /* too many commas */
            error_handler("ERROR_TOO_MANY_COMMAS", file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if(*input_ptr == '\n' || *input_ptr == '\r') { /* lacks the second operand */
            error_handler("ERROR_MISSING_SECOND_OPERAND", file_name, line_counter);
            free(first_operand_name);
            return 1;
        }
        if(isalpha(*input_ptr) == 0 && !isdigit(*input_ptr) && *input_ptr != '#' && *input_ptr != '*') { /* invalid second operand */
            error_handler("ERROR_INVALID_SECOND_OPERAND", file_name, line_counter);
            free(first_operand_name);
            return 1;
        }

        second_operand_length = operand_length_counter(input_ptr);
        second_operand_name = (char *) malloc(second_operand_length + 1);
        if (second_operand_name == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            free(first_operand_name);
            exit(1);
        }
        strncpy(second_operand_name, input_ptr, second_operand_length);
        second_operand_name[second_operand_length] = '\0';

        input_ptr = skip_to_next_word(input_ptr, second_operand_length);
        if(input_ptr && *input_ptr != '\n' && *input_ptr != '\r') { /* redundent chraters after seond operand*/
            error_handler("ERROR_REDUNDENT_CHARACTERS_AFTER_SECOND_OPERAND", file_name, line_counter);
        }
    }
    if(command > 4 && command < 14) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if(input_ptr && *input_ptr != '\n' && *input_ptr != '\r' && *input_ptr != '\0') { /* redundent charaters after first operand*/
            error_handler("ERROR_REDUNDENT_CHARACTERS_AFTER_FIRST_OPERAND", file_name, line_counter);
            free(first_operand_name);
            free(second_operand_name);
            return 1;
        }
    }

    *source = (char *) malloc(first_operand_length + 1);
    if (*source == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        free(first_operand_name);
        if (second_operand_name)
            free(second_operand_name);
        exit(1);
    }
    strncpy(*source, first_operand_name, first_operand_length + 1);
    free(first_operand_name);
    first_operand_name = NULL;

    if (command < 5) {
        *dest = (char *) malloc(second_operand_length + 1);
        if (*dest == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
 * Parses the input string to extract the source and destination operands based on the given command.
 * like parse_instruction but without error handling
 *
 * @param input_ptr A pointer to the input string containing the command and operands.
 * @param command The command value indicating the type of command to be parsed.
 * @param source A pointer to a string where the source operand will be stored.
 * @param dest A pointer to a string where the destination operand will be stored.
 * @param file_name The name of the file where the command is being parsed.
 * @param line_counter The line number in the file where the command is being parsed.
 * @return Returns 0 on success, otherwise returns 1 if memory allocation fails.
 */
int parse_instruction_stage_2(char *input_ptr, const int command, char **source, char **dest, const char *file_name, const int line_counter) {
    int command_length, first_operand_length = 0 , second_operand_length = 0 ;
    char *first_operand_name = NULL, *second_operand_name = NULL;

    if (command < 15)
        command_length = 3;
    else
        command_length = 4;

    *source = NULL;
    *dest = NULL;

    input_ptr = skip_to_next_word(input_ptr, command_length);
    if (command == 14 || command == 15) {
        return 0;
    }
    if(input_ptr == NULL) {
        return 1;
    }
    first_operand_length = operand_length_counter(input_ptr);
    first_operand_name = (char *) malloc(first_operand_length + 1);
    if (first_operand_name == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    strncpy(first_operand_name, input_ptr, first_operand_length);
    first_operand_name[first_operand_length] = '\0';
    fflush(stdout);
    if (command < 5 /*mov, cmp, add, sub, lea*/) {
        input_ptr = skip_to_next_word(input_ptr, first_operand_length);
        if(input_ptr != NULL)
            if (strncmp(input_ptr, ",", 1) == 0)
                input_ptr = skip_whitespace(++input_ptr);
        if(input_ptr == NULL) {
            free(first_operand_name);
            return 1;
        }
        second_operand_length = operand_length_counter(input_ptr);
        second_operand_name = (char *) malloc(second_operand_length + 1);
        if (second_operand_name == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            free(first_operand_name);
            exit(1);
        }
        strncpy(second_operand_name, input_ptr, second_operand_length);
        second_operand_name[second_operand_length] = '\0';
    }
    *source = (char *) malloc(first_operand_length + 1);
    if (*source == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        free(first_operand_name);
        if (second_operand_name)
            free(second_operand_name);
        exit(1);
    }
    if (first_operand_name != NULL)
    strncpy(*source, first_operand_name, first_operand_length + 1);
    free(first_operand_name);
    first_operand_name = NULL;

    if (command < 5) {
        *dest = (char *) malloc(second_operand_length + 1);
        if (*dest == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            free(*source);
            free(second_operand_name);
            exit(1);
        }
        if (second_operand_name != NULL)
        strncpy(*dest, second_operand_name, second_operand_length + 1);
        free(second_operand_name);
        second_operand_name = NULL;
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
label_array *label_array_allocator(const int size) {
    label_array *array = malloc(sizeof(label_array));
    array->label_element = (label *) malloc(size * sizeof(label));
    if (array->label_element == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
void add_label_to_array(label_array *array, const char *name, const int address, const line_type label_characteristic) {
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
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            exit(1);
        }
        array->label_element = new_label;
        array->length = length_of_array; /* Update the length in the array structure*/
    }
    array->label_element[number_of_reps].name = malloc(strlen(name) + 1);
    if (array->label_element[number_of_reps].name == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
    str = (char *) malloc(15 * sizeof(char) + 1);
    if (str == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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

/**
 * Converts a label operand to its binary representation.
 *
 * @param operand The operand containing the label to be converted.
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 * @return A string containing the binary representation of the label operand.
 *         Returns NULL if the label is not found or if memory allocation fails.
 */
char* label_operand_to_binary(const operand operand, const label_array *label_table) {
    int i, operand_address = -2; // instead of -2 make #define for it
    char *part_operand_address_in_binary, *full_operand_address_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(full_operand_address_in_binary == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    memset(full_operand_address_in_binary, '\0', SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
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
    if(operand_address == -2)//need to make error for this case, label not found
        return NULL;

    part_operand_address_in_binary = decimal_to_binary(operand_address);
    strcpy(full_operand_address_in_binary, part_operand_address_in_binary);
    strcat(full_operand_address_in_binary, "010");
    free(part_operand_address_in_binary);
    return full_operand_address_in_binary;
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
    char *operand_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(operand_number_in_binary == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    memset(operand_number_in_binary, '\0', SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    operand_name = malloc(strlen(operand.name) + 1);
    if(operand_name == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
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
    char *first_operand_binary, *second_operand_binary, *operand_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if (operand_number_in_binary == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    memset(operand_number_in_binary, '\0', SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
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
    char* register_number_in_binary = malloc(SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
    if(register_number_in_binary == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    memset(register_number_in_binary, '\0', SIZE_OF_NUMBER_IN_BITS + LEANGTH_OF_ARE + 1);
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
    const int number_in_bits = SIZE_OF_NUMBER_IN_BITS;
    unsigned int mask;
    char* binary_string = malloc(number_in_bits + 1);
    if(binary_string == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
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
    if (strlen(binary_str) != 15) {// make #define
        fprintf(stdout, "Error: binary_str must be 15 bits long.\n"); // for testing  might make real error sign
        return NULL;
    }
    octal_str = malloc(6); // make #define for 6
    if (octal_str == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    octal_str[5] = '\0';
    for (i = 0; i < 5; i++) {
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
        case 'r':
            if(which_register(operand_name) != -1) {
                operand->type = REGISTER;
                return 0;
            }
        default: {
            if(is_label(&label_array, operand_name)) {
                operand->type = LABEL_VALUE;
                return 0;
            }
        }
    }
    return 1;
}

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