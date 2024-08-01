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
    int i, IC = 0, DC = 0, L = 0, location, firstWordInLineLength, error_found = 0, command_in_line, line_counter = 0, is_vaild, extern_flag = 0, entry_flag = 0;
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL, *file_EXT = NULL, *file_ENT = NULL;
    char *first_operand_in_binary = NULL, *second_operand_in_binary = NULL; // for testing
    FILE *am_extension = NULL, *ext_extension = NULL, *ent_extension = NULL;
    operand first_operand, second_operand;
    int j = 100;// take care about it later, it's the starting point of the loop at the end
    char* binary_str;
    am_version = fileTypeCreator(file_name, ".am");
    am_extension = fopen(am_version, "r");
    // fix naming of the files in all stages
    code_node *code_node = code_image ->first;
    data_node *data_node = data_image ->first;

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

            convert_ASCII_to_binary(data_node);
            DC += data_node->length;
            data_node = data_node->next_node;
        }
        if(location == CODE) {
            command_in_line = whichCommand(non_space_ptr);
            parseCommandString_stage_2(non_space_ptr, command_in_line, &first_operand.name, &second_operand.name, file_name, line_counter);
            if(first_operand.name)
                analyze_operand(&first_operand);
            if(second_operand.name)
                analyze_operand(&second_operand);
            convert_operands_to_binary(first_operand, second_operand, &first_operand_in_binary, &second_operand_in_binary,  label_table);
            if(code_node->length >= 2) {

                code_node->word_operand1_in_binary = (char *) malloc(LENGTH_OF_BINARY_WORD);
                if( code_node->word_operand1_in_binary == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }
                strcpy(code_node->word_operand1_in_binary, first_operand_in_binary);
            } else {
                code_node->word_operand1_in_binary = NULL;
            }

            if(code_node->length == 3) {
                code_node->word_operand2_in_binary = (char *) malloc(LENGTH_OF_BINARY_WORD);
                if( code_node->word_operand2_in_binary == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }
                strcpy(code_node->word_operand2_in_binary, second_operand_in_binary);
            } else {
                code_node->word_operand2_in_binary = NULL;

            }
            //printf("\ncommand_line: %sfirst operand: %s\nsecond operand: %s\n",non_space_ptr, first_operand_in_binary, second_operand_in_binary); // for testing
            //printf("\ncommand_line: %sfirst operand: %s\nsecond operand: %s\n",non_space_ptr, code_node->word_operand1_in_binary, code_node->word_operand2_in_binary); // for testing
            IC += code_node->length;
            if(code_node != code_image->last && code_node->next_node != NULL)
                code_node = code_node->next_node;

        }


        first_operand.type = UNKNOWN;
        second_operand.type = UNKNOWN;
    }
    /*code_node = code_image ->first;
    data_node = data_image ->first;
    while(1) { // for testing - print the binary code of the commands and operands
        printf("\n%d %s", j++, code_node->word_command_in_binary);
        if(code_node->length >= 2)
            printf("\n%d %s", j++, code_node->word_operand1_in_binary);
        if(code_node->length == 3)
            printf("\n%d %s", j++, code_node->word_operand2_in_binary);
        printf("\n");
        if(code_node->next_node == NULL)
            break;
        *code_node = *code_node->next_node;
    }

    while(1) { // for testing - print the binary code of the data
        for(i = 0; i < data_node->length; i++) {
            printf("\n%d %s", j++, data_node->word_in_binary[i]);
        }
        printf("\n");
        if(data_node->next_node == NULL)
            break;
        *data_node = *data_node->next_node;
    }*/
    /*if(!entry_flag){
        file_ENT = fileTypeCreator(file_name, ".ent");
        ent_extension = fopen(file_ENT, "w");
    }*/ // make another one for extern
    printLabels(label_table);
    ob_file_usher(file_name, data_image, code_image , IC, DC);
    return 0;
}

