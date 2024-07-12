#include "assembler_stage_1.h"

char* commands_list[16] = {"mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/,"not"/*6*/, "inc"/*7*/,
            "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/};

char* register_list[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

int stage_1_process_file(const char* am_version, symbol_table* symbol_table, data_image* data_image) {
    int IC = 0, DC = 0, L, location, firstWordInLineLength,  isLabel = 0, size, valid, is_error = 0;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER], command_value[MAX_LENGTH_OF_LABEL_VALUE], command[MIN_LENGTH_OF_LABEL_BODY];
    char* non_space_line, *command_type = "\0";
    int* parced_array;
    const int LENGTH_OF_SPACE = 1;
    label_array* IC_array;
    label_array* DC_array;
    data_node* data_node;
    FILE* am_extension;
    
    label_header[0] = '\0', command_value[0] = '\0', command[0] = '\0' ;
    labelArrayAllocator(&IC_array, MIN_LENGTH_OF_LABEL_BODY);
    labelArrayAllocator(&DC_array, MIN_LENGTH_OF_LABEL_BODY);

    am_extension = fopen(am_version, "r");
    if (am_extension == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_READING_AM_FILE);
        return 0;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension)) {
        non_space_line = firstNonSpaceCharacter(line);
        firstWordInLineLength = firstWordLengthCounter(non_space_line); // possible error might be tht the line is empty, maybe should place isEndOfLine at start
        if(isEndOfLine(non_space_line)) {
            return 1;   // check for correct output
        }
        location = lineLocation(non_space_line);

        if(location == LABEL) {
            isLabel = 1;
            strncpy(label_header, non_space_line, firstWordInLineLength + LENGTH_OF_SPACE);
            if (isReservedWord(label_header, strlen(label_header))) {
                //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_INVALID_label_NAME);
                //free_label_array(&array);
            }
            non_space_line += firstWordInLineLength + LENGTH_OF_SPACE;
            non_space_line = firstNonSpaceCharacter(non_space_line);
            firstWordInLineLength = firstWordLengthCounter(non_space_line);
        }
        strncpy(command, non_space_line, firstWordInLineLength + 1);
        //*(command + firstWordInLineLength) = '\n'; // not sure why I did it, need to check if neccecery

        if(location == DATA || location == STRING) {
            if(isLabel == 1) {
                strncmp(command_type, non_space_line, firstWordInLineLength + 1);
                labelArrayAdd(DC_array, label_header, DC, command_type); // maybe make command_type an int, like commands in manan 22
            }
            if(location == DATA) {
                symbolAdd(symbol_table, label_header, &DC, DATA);
                parseData(non_space_line, &parced_array, &size);
                data_node = newDataNode(non_space_line, DC, location, parced_array);
                dataNodeAdd(data_image, data_node);
            }
            else if(location == STRING) {
                symbolAdd(symbol_table, label_header, &DC, STRING);
                parseData(non_space_line, &parced_array, &size);
                data_node = newDataNode(non_space_line, DC, location, parced_array);
                dataNodeAdd(data_image, data_node);
            }
            continue;
        }
        else if(location == EXTERN) {
            valid = symbolAdd(symbol_table, label_header, &DC, EXTERN);
            if(valid != 1){
                //HANDLE_AST_ERROR_NON_POINTER(&ast_line_info, ERROR_SYMBOL_ALREADY_EXISTS);
                //is_error = TRUE;
            }
            continue;
        }
        else {
            non_space_line = firstWordInLine(non_space_line);
            command_type = commandLocator(non_space_line, &L);

            analyze_command(non_space_line, command, &L, *symbol_table);

            //IC += L;
            }
        }
    if (is_error) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_IN_FIRST_PASS);
        fclose(am_extension);
        return 0;
    } else {
       // PRINT_MESSAGE(INFO_MSG_TYPE, INFO_FIRST_PASS);
        fclose(am_extension);
        return 1;
    }
}


int lineLocation(const char *str) {
    char* firstWord = firstWordInLine(str);
    if (firstWord) {
        if (isalpha(*firstWord) == 1 && firstWord[strlen(firstWord)])
            return LABEL;
    }
    else if (strncmp(firstWord, ".data ", 6))
        return DATA;
    else if (strncmp(firstWord, ".string ", 8))
        return STRING;
    else if (strncmp(firstWord, ".entry ", 7))
        return ENTRY;
    else if (strncmp(firstWord, ".extern ", 8))
        return EXTERN;
    return COMMAND;
}

int isCommand(const char* command) {
    int i;

    for(i = 0; i < 16; i++) {
        if(strcmp(command, commands_list[i]) == 1)
            return 1;
    }
    return 0;
}

int commandLocator(const char* command, int *L) {
    int i;
    int str_length = 0;

    for (i = 0; (command[i] && command[i] != ' ' && command[i] != '\t' && command[i] != ',' && command[i] != '\n'); i++) {
        str_length++;
    }

    for (i = 0; i < 5; i++) {
        if (strlen(commands_list[i]) == str_length && !strncmp(commands_list[i], command, str_length)) {
            L += 3;
            return i;
        }
    }
    for (i = 5; i < 14; i++) {
        if (strlen(commands_list[i]) == str_length && !strncmp(commands_list[i], command, str_length)) {
            L += 2;
            return i;
        }
    }
    for (i = 14; i < 16; i++) {
        if (strlen(commands_list[i]) == str_length && !strncmp(commands_list[i], command, str_length)) {
            L++;
            return i;
        }
    }
    //PRINT ERROR, command in not the command_list
    return 0;
}

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

void labelArrayAllocator(label_array *array, const int size) {
    array->label_element = malloc(size * sizeof(label));
    if (array->label_element == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(array->label_element);
        exit(1);
    }
    array->rep = 0;
    array->length = size;
}

int labelArrayAdd(label_array* array, const char* name, const int value, const char* characteristic) { // neeed to check if rep is neccecery
    label* new_array;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        length_of_array = (length_of_array) * 2;
        new_array = realloc(array->label_element, length_of_array * sizeof(label));
        if (!new_array) {
            //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_REALLOCATE_MEM);
            exit(1);
        }
        array->label_element = new_array;
    }

    array->label_element[number_of_reps].name = malloc(strlen(name) + 1);
    array->label_element[number_of_reps].value = malloc(strlen(value) + 1);
    array->label_element[number_of_reps].characteristic = malloc(strlen(characteristic) + 1);


    if (!array->label_element[number_of_reps].value || !array->label_element[number_of_reps].name || !array->label_element[number_of_reps].characteristic ) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(array->label_element[number_of_reps].name);
        free(array->label_element[number_of_reps].value);
        free(array->label_element[number_of_reps].characteristic);

        exit(1);
    }

    strcpy(array->label_element[number_of_reps].name, name);
    strcpy(array->label_element[number_of_reps].value, value);
    strcpy(array->label_element[number_of_reps].characteristic, characteristic);

    array->rep++;
    return 0;
}

char* directiveContent(const char *line, const int type) {
    int i, count = 0, size, array_length = 0, command_length;
    int *parced_array;
    const int LENGTH_OF_SPACE = 1, STRING_DIRECTIVE_LENGTH = 8, DATA_DIRECTIVE_LENGTH = 6;
    char* str = '\0';


    if(type == DATA) {
        for(i = 0; strncmp(line[i], ".data", DATA_DIRECTIVE_LENGTH) !=0; i++) {
            count = i + DATA_DIRECTIVE_LENGTH + LENGTH_OF_SPACE;
        }
        parseData(str, &parced_array, &array_length);
        //array_length = sizeof(parced_array); // possible need to add & before array
        command_length = DATA_DIRECTIVE_LENGTH + array_length;
        strncpy(str, line[count] , command_length);
        return str;
    }
    if(type == STRING) {
        for(i = 0; strncmp(line[i], ".string ", STRING_DIRECTIVE_LENGTH) != 0; i++) {
            count = i + STRING_DIRECTIVE_LENGTH + LENGTH_OF_SPACE;
        }
        for(i = count; line[i] != '\t' || line[i] != '\n'; i++) {
            array_length++;
        }
        command_length = STRING_DIRECTIVE_LENGTH + array_length;
        strncpy(str, line[count], command_length);
        return str;
    }
}

void analyze_command(char* ptr, const int command, int* L, const symbol_table symbol_table) {
    int i;
    char* first_operand_name, *second_operand_name;
    operand first_operand, second_operand;
    symbol_node* current_symbol = symbol_table.first;

    parseCommandString(ptr, command, &first_operand_name, &second_operand_name);
    first_operand.name = first_operand_name;
    second_operand.name = second_operand_name;

    if(analyze_operand(&first_operand, symbol_table) == 0 || analyze_operand(&second_operand, symbol_table) == 0) {
        //sending error
    }

    if(command <= 15) {
        (*L)++;
        if(command <= 13) {
            (*L)++;
            if(command <= 4)
                (*L)++;
        }
    }

    if(command == 1) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            //error
        }
    }
    else if(command <= 3) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            if(second_operand.type == IMMEDIATE) {
                //error
            }
        }
    }
    else if(command == 4) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            if(first_operand.type != LABEL_VALUE) {
                //error
            }
            if(second_operand.type == IMMEDIATE) {
                //error
            }
        }
    }
    else if((command <= 8 && command >= 5) || command == 11) {
        if(second_operand.type != UNKNOWN) {
            //error
        }
        if(first_operand.type == IMMEDIATE) {
            //error
        }
    }
    else if(command == 9 || command == 10 || command == 13) {
        if(second_operand.type != UNKNOWN) {
            //error
        }
        if(first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            //error
        }
    }
    else if(command == 12) {
        if(second_operand.type != UNKNOWN) {
            //error
        }
    }
    else if(command == 14 || command == 15) {
        if(first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            //error
        }
    }
}

int analyze_operand(operand* operand, const symbol_table symbol_table) {
    int i;
    symbol_node* current_symbol = symbol_table.first;
    operand -> type = UNKNOWN;
    switch (operand->name[0]) {
        case '#':
            operand->type = IMMEDIATE;
        return 1;
        case '*':
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name + 1, register_list[i], 2) == 0) {
                    operand->type = REGISTER_PTR;
                    return 1;
                }
            }
        break;
        default:
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name, register_list[i], 2) == 0) {
                    operand->type = REGISTER;
                    return 1;
                }
            }
        while (current_symbol != NULL) {
            if (strcmp(operand->name, current_symbol->symbol_name) == 0) {
                operand->type = LABEL_VALUE;
                return 1;
            }
            current_symbol = current_symbol->next_symbol;
        }
        //it reach here should send error, maybe set defult value of type UNKNOWN
        break;
    }
        // not suppost to reach here.
        //send error for not fitting any of the types above.
        return 0;
}
//dfsdfdsfds