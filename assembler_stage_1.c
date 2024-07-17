#include "assembler_stage_1.h"

char* commands_list[16] = {"mov"/*0*/, "cmp"/*1*/, "add"/*2*/, "sub"/*3*/, "lea"/*4*/, "clr"/*5*/,"not"/*6*/, "inc"/*7*/,
            "dec"/*8*/, "jmp"/*9*/, "bne"/*10*/, "red"/*11*/, "prn"/*12*/, "jsr"/*13*/, "rts"/*14*/, "stop"/*15*/};

char* register_list[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

void printLabelTable(const label_array* array) {
    int i;
    printf("Label Table:\n");
    for (i = 0; i < array->rep; i++) {
        printf("Label %d: %s and the address is :%d\n ", i + 1, array->label_element[i].name, array -> label_element[i].address);
    }
}

void printDataImage(const data_image* image) {
    data_node* current = image->first;
    int i;

    while (current != NULL) {
        printf("Original line: %s\n", current->original_line);
        printf("Data: ");
        for (i = 0; i < current->L; i++) {
            printf("%d ", current->word[i]);
        }
        printf("\n");
        current = current->next_node;
    }
}

int stage_1_process_file(const char* am_version, label_array* label_table, data_image* data_image, code_image* code_image) {
    int IC = 0, DC = 0, L = 0, location, firstWordInLineLength,  labelFlag = 0, is_error = 0, command_name, array_length;
    const int LENGTH_OF_SPACE = 1;
    int* parced_array;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER], word_in_binary[LENGTH_OF_BINARY_WORD];
    char* non_space_line, *command_type = "\0";
    data_node* data_node;
    code_node* code_node;
    FILE* am_extension;

    label_header[0] = '\0';
    //labelArrayAllocator(&IC_array, MIN_LENGTH_OF_LABEL_BODY);
    //labelArrayAllocator(&DC_array, MIN_LENGTH_OF_LABEL_BODY);

    am_version = fileTypeCreator(am_version,".am");
    am_extension = fopen(am_version, "r");
    if (am_extension == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_READING_AM_FILE);
        return 0;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension)) {
        labelFlag = 0;
        non_space_line = firstWordInLine(line);
        firstWordInLineLength = firstWordLengthCounter(non_space_line); // possible error might be tht the line is empty, maybe should place isEndOfLine at start
        if(isEndOfLine(non_space_line)) {
            return 1;   // check for correct output
        }
        location = lineLocation(non_space_line);

        if(location == LABEL) {
            labelFlag = 1;
            memset(label_header, '\0', sizeof(label_header));
            strncpy(label_header, non_space_line, firstWordInLineLength);
            if (isReservedWord(label_header, strlen(label_header))) {
                //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_INVALID_label_NAME);
                //free_label_array(&array);
            }
            non_space_line += firstWordInLineLength + LENGTH_OF_SPACE;
            non_space_line = firstWordInLine(non_space_line);
            firstWordInLineLength = firstWordLengthCounter(non_space_line);
        }
        //strncpy(command, non_space_line, firstWordInLineLength);
        location = lineLocation(non_space_line);
        command_type = malloc(firstWordInLineLength * sizeof(char) + 1);

        strncpy(command_type, non_space_line, firstWordInLineLength);
        command_type[firstWordInLineLength] = '\0';
        if(location == DATA || location == STRING) {
            DC++;
            if(labelFlag == 1) {
                labelArrayAdd(label_table, label_header, DC, location); // maybe make command_type an int, like commands in manan 22
            }
            if(location == DATA)
                parseData(non_space_line, &parced_array, &array_length, &DC);
            else
                parseString(non_space_line, &parced_array, &array_length, &DC);
            data_node = newDataNode(non_space_line, DC, location, &parced_array);
            dataNodeAdd(&data_image, data_node);
            continue;
        }
        else if(location == EXTERN || location == ENTRY) {
            non_space_line += sizeof(".extern");
            non_space_line = firstWordInLine(non_space_line);
            labelArrayAdd(label_table, label_header, 0, EXTERN);
            /*if(valid != 1){
                //HANDLE_AST_ERROR_NON_POINTER(&ast_line_info, ERROR_SYMBOL_ALREADY_EXISTS);
                //is_error = TRUE;
            }*/
            continue;
        }
        else {
            if(labelFlag == 1)
                labelArrayAdd(label_table, label_header, IC + 100, CODE);
            non_space_line = firstWordInLine(non_space_line);
            command_name = commandLocator(non_space_line, &L);

            analyze_command(non_space_line, command_name, &L, *label_table, &word_in_binary);

            code_node = newCodeNode(non_space_line, L, word_in_binary);
            codeNodeAdd(code_image, code_node);
            IC += L;
            L = 0;

        }
    }
    printLabelTable(label_table);
    printDataImage(data_image);

    if (is_error) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_IN_FIRST_PASS);
        fclose(am_extension);
        return 0;
    } else {
        // PRINT_MESSAGE(INFO_MSG_TYPE, INFO_FIRST_PASS);
        fclose(am_extension);
        return 1;
    }
    /*
     *update label table every label of type DATA by inc IC by 100 (IC+=100)
     */
}


int lineLocation(const char *str) {
    int i = 0;
    char* firstWord = firstWordInLine(str);

    /*if (*firstWord) {
        if (isalpha(*firstWord) == 1 && firstWord[strlen(firstWord)])
            return LABEL;
    }*/
    if (isalpha(firstWord[0])) {
        while (firstWord[i] && firstWord[i] != ':') {
            if (!isalnum(firstWord[i])) {
                break;
            }
            i++;
        }
        if (firstWord[i] == ':' && (i > 0) && (firstWord[i + 1] == ' ' || firstWord[i + 1] == '\0')) {
            return LABEL;
        }
    }
    if (strncmp(firstWord, ".data ", 6) == 0)
        return DATA;
    else if (strncmp(firstWord, ".string ", 8) == 0)
        return STRING;
    else if (strncmp(firstWord, ".entry ", 7) == 0)
        return ENTRY;
    else if (strncmp(firstWord, ".extern ", 8) == 0)
        return EXTERN;
    return CODE;
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



void directiveContentHandler(data_image* data_image, const char *line, const int type, int* parced_array) {
    int i, count = 0, size, array_length = 0, command_length;
    const int LENGTH_OF_SPACE = 1, STRING_DIRECTIVE_LENGTH = 8, DATA_DIRECTIVE_LENGTH = 6;
    char* str = '\0';

    if(type == DATA) {
        for(i = 0; strncmp(line[i], ".data", DATA_DIRECTIVE_LENGTH) !=0; i++) {
            count = i + DATA_DIRECTIVE_LENGTH + LENGTH_OF_SPACE;
        }


        //array_length = sizeof(parced_array); // possible need to add & before array
    }
    if(type == STRING) {
        for(i = 0; strncmp(line[i], ".string ", STRING_DIRECTIVE_LENGTH) != 0; i++) {
            count = i + STRING_DIRECTIVE_LENGTH + LENGTH_OF_SPACE;
        }
        for(i = count; line[i] != '\t' || line[i] != '\n'; i++) {
            array_length++;
        }
        command_length = STRING_DIRECTIVE_LENGTH + array_length;

    }
}

void analyze_command(char* ptr, const int command, int* L, const label_array label_table, char* word_in_binary) {
    int i;
    char* first_operand_name, *second_operand_name;
    operand first_operand, second_operand;
    //symbol_node* current_symbol = symbol_table.first;

    parseCommandString(ptr, command, &first_operand_name, &second_operand_name);
    first_operand.name = malloc(strlen(first_operand_name));
    second_operand.name = malloc(strlen(second_operand_name));
    strcpy(first_operand.name, first_operand_name);
    strcpy(second_operand.name, second_operand_name);

    if(analyze_operand(&first_operand, label_table) == 0 || analyze_operand(&second_operand, label_table) == 0) {
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
    strcpy(word_in_binary, command_to_binary(command, first_operand, second_operand, *L));
}

int analyze_operand(operand* operand, const label_array label_table) {
    int i;
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
        default: {
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name, register_list[i], 2) == 0) {
                    operand->type = REGISTER;
                    return 1;
                }
            }
            if(isLabel(&label_table, operand->name)) {
                operand->type = LABEL_VALUE;
                return 1;
            }
        }
        //it reach here should send error, maybe set defult value of type UNKNOWN
        break;
    }
        // not suppost to reach here.
        //send error for not fitting any of the types above.
        return 0;
}

