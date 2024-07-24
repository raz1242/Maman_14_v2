#include "utils.h"

const char* reserved_words[] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/, "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/,
    "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/, "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

char* fileTypeCreator(const char* str, const char* type) {
    char* file_type = malloc(strlen(str) + strlen(type) + 1);
    if (!file_type) {
        // Handle memory allocation failure
        return NULL;
    }
    strcpy(file_type, str);
    strcat(file_type, type);
    return file_type;
}

int fileInspection(const FILE* file) {
    if (file == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_READING_AS_FILE);
        return 0;
    }
    return 1;
}

/* moves string to the beginning of the code */
char* firstWordInLine(char* str) {
    if (str == NULL) {
        return NULL;
    }
    while (isspace(*str)) {
        str++;
    }
    if (*str == '\0') {
        return NULL;
    }
    return str;
}

int firstWordLengthCounter(const char *str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)) && *(str + counter) != ':'; counter++){};
    return counter;
}

int operandLengthCounter(const char* str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)) && (strncmp(&str[counter], ",", 1) != 0); counter++){};
    /*if(strncmp(&str[counter], ",", 1) == 0)
        counter++;*/
    return counter;
}

int isReservedWord(char* word, const int length) {
    int i, size;

    size = sizeof(reserved_words) / sizeof(reserved_words[0]);
    for (i = 0; i < size; i++) {
        if (strncmp(word, reserved_words[i], length) == 0 && isEndOfLine(word + length)) {
            return 1;
        }
    }
    return 0;
}

int isEndOfLine(char *str) {
    const char* new_str = firstWordInLine(str);

    if (new_str == NULL)
        return 1;  // Return 1 if it's the end of line (i.e., string was empty or all whitespace)
    else
        return 0;
}

void parseData(const char *input, int **array, int *size, int* DC) {
    const int DATA_COMMAND_LENGTH = strlen(".data ");
    const char *dataStart;
    int count = 0, index = 0;
    char *ptr;
    *size = 0;

    dataStart = strstr(input, ".data ");

    if(dataStart)
        dataStart += DATA_COMMAND_LENGTH;
    else{
        *array = NULL;
        *size = 0;
        return;
    }

    ptr = (char*)dataStart;
    while (*ptr) {
        if(*ptr == '\n')
            break;
        while (isspace(*ptr)) {
            ptr++;
        }
        if (*ptr == ',' || *ptr == '\0') {
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            count++;
            while (isdigit(*ptr) || *ptr == '-' || *ptr == '+') {
                ptr++;
            }
        } else {
            ptr++;
        }
    }

    *array = (int*)malloc(count * sizeof(int));
    if (*array == NULL) {
        *size = 0;
        return;
    }

    ptr = (char*)dataStart;
    while (*ptr) {
        if(*ptr == '\n')
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
        }
        else
            ptr++;
    }
    *size = count;
    *DC += count;
}

void parseString(const char *input, int  **array, int *size, int *DC) {
    const int STRING_COMMAND_LENGTH = strlen(".string ");
    const char *stringStart;
    char *ptr;
    int count = 0, index = 0;
    *size = 0;

    stringStart = strstr(input, ".string ");
    if(stringStart)
        stringStart += STRING_COMMAND_LENGTH;
    else{
        *array = NULL;
        *size = 0;
        return;
    }
    ptr = (char*)stringStart;
    while (*stringStart && *stringStart != '"' && *stringStart != '“') {
        stringStart++;
    }
    if (*stringStart != '"' && *stringStart != '“') {
        *array = NULL;
        *size = 0;
        return;
    }
    ptr = (char *)(stringStart + 1);
    while (*ptr && *ptr != '"' && *ptr != '”') {
        count++;
        ptr++;
    }

    *array = (int*)malloc((count + 1) * sizeof(int));
    if (*array == NULL) {
        *size = 0;
        return;
    }

    ptr = (char *)(stringStart + 1);
    for(index = 0; index < count; index++){
        (*array)[index] = (int)*ptr;
        (*DC)++;
        ptr++;
    }

    (*array)[index] = 0;
    (*DC)++;

    *size = count + 1;
}

void parseCommandString(const char *input_ptr, const int command, char** source, char** dest) {
    int command_length, first_operand_length = 0, second_operand_length = 0;
    char* first_operand, *second_operand;

    first_operand = malloc(5); // Allocate space for the first operand
    second_operand = malloc(5); // Allocate space for the second operand
    second_operand[0] = '\0';
    if(command < 15)
        command_length = 3;
    else
        command_length = 4;

    if(!input_ptr) {
        return;
    }
    if(command != 14 /*rts*/ && command != 15 /*stop*/) {
        if(!input_ptr) {
            //print error - missing first operand
            return;
        }
        input_ptr += command_length;
        input_ptr = firstWordInLine(input_ptr);
        first_operand_length = operandLengthCounter(input_ptr);
        first_operand = (char*)malloc(first_operand_length + 1);
        if (first_operand == NULL) {
            // Handle memory allocation failure
            return;
        }
        strncpy(first_operand, input_ptr, first_operand_length);
        first_operand[first_operand_length] = '\0';
        if(command < 5 /*mov, cmp, add, sub, lea*/) {
            input_ptr += first_operand_length;
            if(!input_ptr) {
                //print error - missing second operand
                return;
            }
            input_ptr = firstWordInLine(input_ptr);
            if(strncmp(input_ptr, ",", 1) == 0) {
                input_ptr++;
                input_ptr = firstWordInLine(input_ptr);
            }
            second_operand_length = operandLengthCounter(input_ptr);
            second_operand = (char*)malloc(second_operand_length + 1);
            if (second_operand == NULL) {
                // Handle memory allocation failure
                free(first_operand);
                return;
            }
            strncpy(second_operand, input_ptr, second_operand_length);
            second_operand[second_operand_length] = '\0';
        }
    }
    *source = malloc(first_operand_length + 1);
    *dest = malloc(second_operand_length + 1);
    if (!*source || !*dest) {
        free(first_operand);
        free(second_operand);
        // Handle memory allocation failure
        return;
    }
    strncpy(*source, first_operand, first_operand_length + 1);
    strncpy(*dest, second_operand, second_operand_length + 1);
    free(first_operand);
    free(second_operand);
}

label_array *labelArrayAllocator(const int size) {
    label_array* array = (label_array*) malloc(sizeof(label_array));
    array->label_element = (label*) malloc(size * sizeof(label));
    if (array->label_element == NULL) {
        //PRINT_MESSAGE(ERROR_FAILED_TO_ALLOCATE_MEM);
        exit(1);
    }
    array->rep = 0;
    array->length = size;
    return array;
}


int labelArrayAdd(label_array* array, const char* name, const int address, const line_type label_characteristic) {
    label* new_label;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        if(length_of_array == 0)
            length_of_array = 1;
        else
            length_of_array = (length_of_array) * 2;
        new_label = realloc(array->label_element, length_of_array * sizeof(label));
        if (!new_label) {
            //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_REALLOCATE_MEM);
            exit(1);
        }
        array->label_element = new_label;
        array->length = length_of_array; /* Update the length in the array structure*/
    }
    array->label_element[number_of_reps].name = malloc(strlen(name) + 1);
    if (!array->label_element[number_of_reps].name) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        return -1;
    }
    array->label_element[number_of_reps].address = address;
    strcpy(array->label_element[number_of_reps].name, name);
    array->label_element[number_of_reps].characteristic = label_characteristic;

    array->rep++;
    return 0;
}

int isLabel(const label_array* label_table, const char* name) {
    int i;
    for (i = 0; i < label_table->rep; i++){
        if (strcmp(name, label_table->label_element[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

char* command_to_binary(const int command, const operand first_operand, const operand second_operand, const int L) {
    char* str;
    str = (char*) malloc(16 * sizeof(char));
    if (str == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation failed\n");
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
            strcpy(str, "0101");
        break;
        case 6:
            strcpy(str, "0110");
        break;
        case 7:
            strcpy(str, "0111");
        break;
        case 8:
            strcpy(str, "1000");
        break;
        case 9:
            strcpy(str, "1001");
        break;
        case 10:
            strcpy(str, "1010");
        break;
        case 11:
            strcpy(str, "1011");
        break;
        case 12:
            strcpy(str, "1100");
        break;
        case 13:
            strcpy(str, "1101");
        break;
        case 14:
            strcpy(str, "1110");
        break;
        case 15:
            strcpy(str, "1111");
        break;
        default:
            strcpy(str, "Invalid command");
        break;
    }
    if(L >= 2 && first_operand.type != UNKNOWN) {
        if(first_operand.type == IMMEDIATE)
            strcat(str, "0001");
        else if(first_operand.type == LABEL_VALUE)
            strcat(str, "0010");
        else if(first_operand.type == REGISTER_PTR)
            strcat(str, "0100");
        else if(first_operand.type == REGISTER)
            strcat(str, "1000");
    }
    else
        strcat(str, "0000");

    if(L == 3 || first_operand.type == UNKNOWN || first_operand.type == second_operand.type) {
        if(second_operand.type == IMMEDIATE)
            strcat(str, "0001");
        else if(second_operand.type == LABEL_VALUE)
            strcat(str, "0010");
        else if(second_operand.type == REGISTER_PTR)
            strcat(str, "0100");
        else if(second_operand.type == REGISTER)
            strcat(str, "1000");
    }
    else
        strcat(str, "0000");

    strcat(str, "100");
    *(str + 15) = '\0';
    return str;
}
/*
char* operand_to_binary(operand operand) {

}*/

void error_handler(const char* error_message, const char* file_name, const int line_number) {
    printf("Error: %s in file %s at line %d\n", error_message, file_name, line_number);
    exit(1);
}