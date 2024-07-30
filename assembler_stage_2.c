#include "assembler_stage_2.h"
#include "assembler_stage_1.h"

/**
 * Prints all labels with their characteristics.
 *
 * @param label_table A pointer to the label array containing the labels and their characteristics.
 */
void printLabels(const label_array *label_table) {
    int i;
    printf("\n");
    for (i = 0; i < label_table->rep; i++) {
        if(label_table -> label_element[i].characteristic == ENTRY)
            printf("Label: %s, Characteristic: ENTRY\n", label_table->label_element[i].name);
        if(label_table -> label_element[i].characteristic == EXTERN)
            printf("Label: %s, Characteristic: EXTERN\n", label_table->label_element[i].name);
        else
            printf("Label: %s, Characteristic: IRRLEVANT\n", label_table->label_element[i].name);
    }
}

int stage_2_process_file(const char *file_name, label_array *label_table, code_image *code_image, data_image *data_image) {
    int i, IC = 0, DC = 0, L = 0, location, firstWordInLineLength, error_found = 0, command_in_line, array_size, line_counter = 0, is_vaild, extern_flag = 0, entry_flag = 0;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1], word_in_binary[LENGTH_OF_BINARY_WORD];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL, *file_EXT = NULL, *file_ENT = NULL;
    char *first_operand_in_binary = NULL, *second_operand_in_binary = NULL; // for testing
    FILE *am_extension = NULL, *ext_extension = NULL, *ent_extension = NULL;
    operand first_operand, second_operand;

    am_version = fileTypeCreator(file_name, ".am");
    am_extension = fopen(am_version, "r");
    // fix naming of the files in all stages

    if (am_extension == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension)) {
        line_counter++;
        is_vaild = 0;
        non_space_ptr = firstWordInLine(line);
        firstWordInLineLength = firstWordLengthCounter(non_space_ptr);
        location = lineLocation(non_space_ptr);
        if(location == LABEL) {
            strncpy(label_header, non_space_ptr, firstWordInLineLength);
            ptr = non_space_ptr + firstWordInLineLength + LENGTH_OF_COLON;
            non_space_ptr = firstWordInLine(ptr);
            firstWordInLineLength = firstWordLengthCounter(non_space_ptr);
            location = lineLocation(non_space_ptr);
        }
        if(location == ENTRY) {
            entry_flag = 1;
            ptr = non_space_ptr + firstWordInLineLength;
            non_space_ptr = firstWordInLine(ptr);
            firstWordInLineLength = firstWordLengthCounter(non_space_ptr);
            for(i = 0; i <= label_table->rep; i++) {
                if(!strncmp(label_table->label_element[i].name, non_space_ptr, firstWordInLineLength)) {
                    is_vaild = 1;
                    label_table->label_element[i].characteristic = ENTRY;
                    break;
                }
            }
            if(!is_vaild) {
                error_handler("ERROR_LABEL_NOT_FOUND", file_name, line_counter);
                error_found = 1;
            }
        }
        if(location == EXTERN) {
            extern_flag = 1;
        }
        if(location == DATA || location == STRING) {
            continue;
        }
        if(location == CODE) {
            command_in_line = whichCommand(non_space_ptr);
            parseCommandString_stage_2(non_space_ptr, command_in_line, &first_operand.name, &second_operand.name, file_name, line_counter);
            if(first_operand.name)
                analyze_operand(&first_operand);
            if(second_operand.name)
                analyze_operand(&second_operand);
            /*
            switch(first_operand.type) {
                case IMMEDIATE:
                    first_operand_in_binary = immediate_operand_to_binary(first_operand);
                    break;
                case LABEL_VALUE:
                    first_operand_in_binary = label_operand_to_binary(first_operand, label_table);
                    break;
                case REGISTER_PTR:
                    first_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
                    break;
                case REGISTER:
                    first_operand_in_binary = register_operand_to_binary(first_operand, second_operand);
                    break;
                case UNKNOWN:
                    break;
            }
            switch(second_operand.type) {
                case IMMEDIATE:
                    second_operand_in_binary = immediate_operand_to_binary(second_operand);
                    break;
                case LABEL_VALUE:
                    second_operand_in_binary = label_operand_to_binary(second_operand, label_table);
                    break;
                case REGISTER_PTR:
                    if(first_operand.type == REGISTER_PTR | first_operand.type == REGISTER)
                        break;
                    else {
                        second_operand_in_binary = register_operand_to_binary(second_operand, first_operand);
                        break;
                    }
                case REGISTER:
                    if(first_operand.type == REGISTER_PTR | first_operand.type == REGISTER)
                        break;
                    else {
                        second_operand_in_binary = register_operand_to_binary(second_operand, first_operand);
                        break;
                    }
                    break;
                case UNKNOWN:
                    break;
            }*/
            convert_operands_to_binary(first_operand, second_operand, &first_operand_in_binary, &second_operand_in_binary,  label_table);
            printf("\ncommand_line: %sfirst operand: %s\nsecond operand: %s\n",non_space_ptr, first_operand_in_binary, second_operand_in_binary); // for testing


            first_operand.type = UNKNOWN;
            second_operand.type = UNKNOWN;
        }
    }
    /*if(!entry_flag){
        file_ENT = fileTypeCreator(file_name, ".ent");
        ent_extension = fopen(file_ENT, "w");
    }*/ // make another one for extern
    printLabels(label_table);
    return 0;
}

