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

    while (current != NULL) {
        printf("Original line: %s\n", current->original_line);
        printf("Data: ");
        for (i = 0; i < current->length; i++) {
            //if(current->word[i] != 0)
            printf("%d ", current->word[i]);
        }
        printf("\n");
        current = current->next_node;
    }
}

void printCodeImage(const code_image *image) {  //- testing
    code_node *current = image->first;
    while (current != NULL) {
        printf("Original line: %s\n", current->original_line);
        printf("Binary representation: %s\n", current->word);
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

int stage_1_process_file(const char *file_name, label_array *label_table, data_image *data_image,
                         code_image *code_image) {
    const int LENGTH_OF_COLON = 1;
    int i, IC = 0, DC = 0, L = 0, location, firstWordInLineLength, labelFlag, is_error = 0, command_in_line, array_size, line_counter = 0;
    int *parced_array;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER], word_in_binary[LENGTH_OF_BINARY_WORD];
    char *am_version, *ptr, *non_space_ptr;
    data_node *data_node;
    code_node *code_node;
    FILE *am_extension;

    label_header[0] = '\0';

    am_version = fileTypeCreator(file_name, ".am");
    am_extension = fopen(am_version, "r");
    free(am_version);
    if (am_extension == NULL) {
        printf("error opening file\n");
        return 0;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension)) {
        line_counter++;
        labelFlag = 0;
        non_space_ptr = firstWordInLine(line);
        firstWordInLineLength = firstWordLengthCounter(non_space_ptr);
        location = lineLocation(non_space_ptr);

        if (location == LABEL) {
            labelFlag = 1;
            memset(label_header, '\0', sizeof(label_header));
            strncpy(label_header, non_space_ptr, firstWordInLineLength);
            if (isReservedWord(label_header, strlen(label_header))) {
                //error_handler(ERROR_LABEL_NAME_IS_RESERVED_WORD, am_version, line_counter);
                //free label_table here as it might cause double-free issues outside this function
                fclose(am_extension);
                return 0;
            }
            ptr = non_space_ptr + firstWordInLineLength + LENGTH_OF_COLON;
            non_space_ptr = firstWordInLine(ptr);
            firstWordInLineLength = firstWordLengthCounter(non_space_ptr);
        }
        location = lineLocation(non_space_ptr);

        if (location == DATA || location == STRING) {
            if (labelFlag == 1) {
                labelArrayAdd(label_table, label_header, DC, location);
            }
            if (location == DATA)
                parseData(non_space_ptr, &parced_array, &array_size, &DC);
            else
                parseString(non_space_ptr, &parced_array, &array_size, &DC);
            data_node = newDataNode(non_space_ptr, location, parced_array, array_size);
            dataNodeAdd(data_image, data_node);
            free(parced_array);
        } else if (location == EXTERN) {
            ptr = non_space_ptr + firstWordInLineLength;
            non_space_ptr = firstWordInLine(ptr);
            strncpy(label_header, non_space_ptr, strlen(non_space_ptr) - 1);
            labelArrayAdd(label_table, label_header, EXTERN_ADDRESS, EXTERN);
            /*if(valid != 1){
                //HANDLE_AST_ERROR_NON_POINTER(&ast_line_info, ERROR_SYMBOL_ALREADY_EXISTS);
                //is_error = TRUE;
            }*/
        } else if (location == ENTRY) {
        } else {
            if (labelFlag == 1)
                labelArrayAdd(label_table, label_header, IC + 100, CODE);
            ptr = non_space_ptr;
            non_space_ptr = firstWordInLine(ptr);
            command_in_line = whichCommand(non_space_ptr);
            analyze_command(non_space_ptr, command_in_line, &L, *label_table, word_in_binary);

            code_node = newCodeNode(non_space_ptr, L, word_in_binary);
            codeNodeAdd(code_image, code_node);
            IC += L;
            L = 0;

        }
    }


    if (is_error) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_IN_FIRST_PASS);
        fclose(am_extension);
        return 0;
    } else {
        for (i = 0; i < label_table->rep; i++) {
            if (label_table->label_element[i].characteristic == DATA || label_table->label_element[i].characteristic ==
                STRING) {
                label_table->label_element[i].address += (IC + 100);
            }
        }

        printLabelTable(label_table); //- testing
        printDataImage(data_image); //- testing
        printCodeImage(code_image); //- testing
        printExternLabels(label_table); //- testing


        // PRINT_MESSAGE(INFO_MSG_TYPE, INFO_FIRST_PASS);

        fclose(am_extension);
        return 1;
    }
}

/**
 * Determines the type of a line in the source code.
 * @param str Pointer to the line to analyze.
 * @return An integer representing the type of the line.
 */
int lineLocation(char *str) {
    int i = 0;
    char *ptr_to_firstWord = firstWordInLine(str);

    if (isalpha(ptr_to_firstWord[0])) {
        while (ptr_to_firstWord[i] && ptr_to_firstWord[i] != ':') {
            if (!isalnum(ptr_to_firstWord[i])) {
                break;
            }
            i++;
        }
        if (ptr_to_firstWord[i] == ':' && (i > 0) && (
                ptr_to_firstWord[i + 1] == ' ' || ptr_to_firstWord[i + 1] == '\0')) {
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
    return CODE;
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
    //PRINT ERROR, command in not the command_list
    return -1;
}

/**
 * This function parses the command string to extract the operands, analyzes each operand,
 * and converts the command to it's binary representation.
 *
 * @param ptr Pointer to the command string.
 * @param command The command index in the commands_list.
 * @param L Pointer to the length of the command in memory.
 * @param label_table The table of labels.
 * @param word_in_binary Pointer to the string where the binary representation will be stored.
 */
void analyze_command(const char *ptr, const int command, int *L, const label_array label_table, char *word_in_binary) {
    char *first_operand_name, *second_operand_name;
    operand first_operand, second_operand;

    parseCommandString(ptr, command, &first_operand_name, &second_operand_name);

    first_operand.name = malloc(strlen(first_operand_name) + 1);
    second_operand.name = malloc(strlen(second_operand_name) + 1);
    strcpy(first_operand.name, first_operand_name);
    strcpy(second_operand.name, second_operand_name);
    free(first_operand_name);
    free(second_operand_name);

    if (command <= 15) {
        (*L)++;
        if (command <= 13) {
            analyze_operand(&first_operand, label_table);
            second_operand.type = UNKNOWN;
            (*L)++;
            if (command <= 4) {
                analyze_operand(&second_operand, label_table);
                if (first_operand.type != second_operand.type)
                    (*L)++;
            }
        }
    }
    if (second_operand.type == UNKNOWN) {
        strcpy(second_operand.name, first_operand.name);
        second_operand.type = first_operand.type;
        first_operand.type = UNKNOWN;
    }
    if (command == 1) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            //error
        }
    } else if (command <= 3) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            if (second_operand.type == IMMEDIATE) {
                //error
            }
        }
    } else if (command == 4) {
        if (first_operand.name[0] == '\0' || second_operand.name[0] == '\0') {
            if (first_operand.type != LABEL_VALUE) {
                //error
            }
            if (second_operand.type == IMMEDIATE) {
                //error
            }
        }
    } else if ((command <= 8 && command >= 5) || command == 11) {
        if (second_operand.type != UNKNOWN) {
            //error
        }
        if (first_operand.type == IMMEDIATE) {
            //error
        }
    } else if (command == 9 || command == 10 || command == 13) {
        if (second_operand.type != UNKNOWN) {
            //error
        }
        if (first_operand.type == IMMEDIATE || first_operand.type == REGISTER) {
            //error
        }
    } else if (command == 12) {
        if (second_operand.type != UNKNOWN) {
            //error
        }
    } else if (command == 14 || command == 15) {
        if (first_operand.type != UNKNOWN || second_operand.type != UNKNOWN) {
            //error
        }
    }
    strcpy(word_in_binary, command_to_binary(command, first_operand, second_operand, *L));
    free(first_operand.name);
    free(second_operand.name);
}

/**
 * Analyzes an operand and determines its type.
 *
 * This function examines the operand string to determine if it is an immediate value,
 * a register, a register pointer, or a label value.
 *
 * @param operand Pointer to the operand structure to analyze.
 * @param label_table The table of labels.
 */
void analyze_operand(operand *operand, const label_array label_table) {
    int i;
    operand->type = UNKNOWN;
    switch (operand->name[0]) {
        case '#':
            operand->type = IMMEDIATE;
            return;
        case '*':
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name + 1, register_list[i], 2) == 0) {
                    operand->type = REGISTER_PTR;
                    return;
                }
            }
            break;
        default: {
            for (i = 0; i < 8; i++) {
                if (strncmp(operand->name, register_list[i], 2) == 0) {
                    operand->type = REGISTER;
                    return;
                }
            }
            operand->type = LABEL_VALUE;
        }
    }
}