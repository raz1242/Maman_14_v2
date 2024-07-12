#include "utils.h"

const char* reserved_words[] = {
    "mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/, "not"/*6*/, "inc"/*7*/, "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/,
    "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/, "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

char* fileTypeCreator(const char* str, const char* type) {
    char* file_type = malloc(strlen(str) + strlen(type) + 1);

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
char* firstNonSpaceCharacter(char* str) {
    int n;
    for (n = 0; *(str + n) != 0 && (str[n] == ' ' || str[n] == '\t'); n++) {}
    return (str + n);
}

int firstWordLengthCounter(const char *str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)); counter++){};
    return counter;
}

int operandLengthCounter(const char* str) {
    int counter;

    for (counter = 0; *(str + counter) && !isspace(*(str + counter)); counter++){};
    if(strncmp(str[counter], ',', 1) == 0)
        counter--;
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
    const char* new_str = firstNonSpaceCharacter(str);

    if (strncmp(new_str, "\n", 1) == 0 || !*new_str)
        return 1;
    else
        return 0;
}

void parseData(const char *input, int **array, int *size) {
    const int command_length = strlen(".data ");
    const char *dataStart = strstr(input, ".data "), *ptr = dataStart;
    int count = 0, index = 0;

    if (!dataStart) {
        *array = NULL;
        *size = 0;
        return;
    }
    dataStart += command_length;

    while (*ptr) {
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


    ptr = dataStart;
    while (*ptr) {
        while (isspace(*ptr)) {
            ptr++;
        }
        if (*ptr == ',' || *ptr == '\0') {
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            (*array)[index++] = strtol(ptr, (char **)&ptr, 10);
        } else {
            ptr++;
        }
    }
    *size = count;
}

void parseString(const char *input, int **array, int *size) {
    const int command_length = strlen(".string ");
    const char *stringStart = strstr(input, ".string "), *ptr = stringStart;
    int count = 0, index = 0;

    if (!stringStart) {
        *array = NULL;
        *size = 0;
        return;
    }
    stringStart += command_length;

    while (*ptr) {
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


    ptr = stringStart;
    while (*ptr) {
        while (isspace(*ptr)) {
            ptr++;
        }
        if (*ptr == ',' || *ptr == '\0') {
            ptr++;
            continue;
        }
        if (isdigit(*ptr) || ((*ptr == '-' || *ptr == '+') && isdigit(*(ptr + 1)))) {
            (*array)[index++] = strtol(ptr, (char **)&ptr, 10);
        } else {
            ptr++;
        }
    }
    *size = count;
}

void parseCommandString(const char *input_ptr, const int command, char* source, char* dest) {
    int command_length, first_operand_length = 0, second_operand_length = 0;
    char* first_operand = '\0', *second_operand = '\0';

    if(command < 15)
        command_length = 3;
    else
        command_length = 4;

    if(!input_ptr) {
        //print error - missing command operand
        //return;
    }
    if(command != 14 /*rts*/ && command != 15 /*stop*/) {
        if(!input_ptr) {
            //print error - missing first operand
            //return;
        }
        input_ptr += command_length;
        input_ptr = firstNonSpaceCharacter(input_ptr);
        first_operand_length = operandLengthCounter(input_ptr);
        strncpy(first_operand, input_ptr, first_operand_length);
        if(command < 5 /*mov, cmp, add, sub, lea*/) {
            input_ptr += first_operand_length;
            if(!input_ptr) {
                //print error - missing second operand
                //return;
            }
            input_ptr = firstNonSpaceCharacter(input_ptr);
            if(strncmp(input_ptr, ',', 1) == 0) {
                input_ptr++;
                input_ptr = firstNonSpaceCharacter(input_ptr);
            }
            second_operand_length = operandLengthCounter(input_ptr);
            strncpy(second_operand, input_ptr, second_operand_length);
        }
    }
    strncpy(source, first_operand, first_operand_length);
    strncpy(dest, second_operand, second_operand_length);
    return;
}