#include "assembler_stage_2.h"


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

int stage_2_process_file(const char *file_name, const label_array *label_table, const code_image *code_image, const data_image *data_image) {
    int i, IC = 0, DC = 0, location, firstWordInLineLength, command_in_line, is_vaild, line_counter = 0, extern_flag = 0, entry_flag = 0, error_found = 0;
    int j = STARTING_POINT_OF_MEMORY;// for testing
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL, *first_operand_in_binary = NULL, *second_operand_in_binary = NULL;
    FILE *am_extension_file = NULL;
    operand first_operand, second_operand;

    code_node *code_node = code_image ->first;
    data_node *data_node = data_image ->first;
    am_version = file_name_extender(file_name, ".am");
    am_extension_file = fopen(am_version, "r");
    // fix naming of the files in all stages for consistency, also in at the function at the end
    if (am_extension_file == NULL) {
        printf("Error opening file\n");
        free(am_version);
        am_version = NULL;
        return 1;
    }

    while (fgets(line, MAX_LENGTH_OF_LINE, am_extension_file)) {
        line_counter++;
        is_vaild = 0;
        non_space_ptr = first_char_in_line(line);
        firstWordInLineLength = first_word_length_counter(non_space_ptr);
        location = lineLocation(non_space_ptr);
        if(location == LABEL) {
            strncpy(label_header, non_space_ptr, firstWordInLineLength);
            ptr = non_space_ptr + firstWordInLineLength + LENGTH_OF_COLON;
            non_space_ptr = first_char_in_line(ptr);
            firstWordInLineLength = first_word_length_counter(non_space_ptr);
            location = lineLocation(non_space_ptr);
        }
        if(location == ENTRY) {
            entry_flag = 1;
            ptr = non_space_ptr + firstWordInLineLength;
            non_space_ptr = first_char_in_line(ptr);
            firstWordInLineLength = first_word_length_counter(non_space_ptr);
            for(i = 0; i < label_table->rep; i++) {
                if(!strncmp(label_table->label_element[i].name, non_space_ptr, firstWordInLineLength)) {
                    is_vaild = 1;
                    label_table->label_element[i].characteristic = ENTRY;
                    break;
                }//maybe put the if(is_vailid) in the if else statement
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
            convert_ascii_to_binary(data_node);
            DC += data_node->length;
            data_node = data_node->next_node;
        }
        if(location == CODE) {
            command_in_line = whichCommand(non_space_ptr);
            parse_instruction_stage_2(non_space_ptr, command_in_line, &first_operand.name, &second_operand.name, file_name, line_counter);
            if(first_operand.name)
                analyze_operand(&first_operand);
            if(second_operand.name)
                analyze_operand(&second_operand);
            convert_operands_to_binary(first_operand, second_operand, &first_operand_in_binary, &second_operand_in_binary, label_table);
            if(code_node->length >= 2) {
                code_node->operand1_name = (char *) malloc(strlen(first_operand.name) + 1);
                if( code_node->operand1_name == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }
                code_node->word_operand1_in_binary = (char *) malloc(LENGTH_OF_BINARY_WORD);
                if( code_node->word_operand1_in_binary == NULL) {
                    printf("Failed to allocate memory\n");
                    free(code_node->operand1_name);
                    exit(1);
                }
                strcpy(code_node->operand1_name, first_operand.name);
                strcpy(code_node->word_operand1_in_binary, first_operand_in_binary);
                code_node->address_in_machine = IC + STARTING_POINT_OF_MEMORY;
            } else
                code_node->word_operand1_in_binary = NULL;

            if(code_node->length == 3) {
                code_node->operand2_name = (char *) malloc(strlen(second_operand.name) + 1);
                if( code_node->operand2_name == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }
                code_node->word_operand2_in_binary = (char *) malloc(LENGTH_OF_BINARY_WORD);
                if( code_node->word_operand2_in_binary == NULL) {
                    printf("Failed to allocate memory\n");
                    free(code_node->operand2_name);
                    exit(1);
                }
                strcpy(code_node->operand2_name, second_operand.name);
                strcpy(code_node->word_operand2_in_binary, second_operand_in_binary);
                code_node->address_in_machine = IC + STARTING_POINT_OF_MEMORY;
            } else
                code_node->word_operand2_in_binary = NULL;
            /*printf("\ncommand_line: %sfirst operand: %s\nsecond operand: %s\n",non_space_ptr, first_operand_in_binary, second_operand_in_binary); // for testing
            printf("\ncommand_line: %sfirst operand: %s\nsecond operand: %s\n",non_space_ptr, code_node->word_operand1_in_binary, code_node->word_operand2_in_binary); // for testing*/
            IC += code_node->length;
            if(code_node != code_image->last && code_node->next_node != NULL)
                code_node = code_node->next_node;
        }
        first_operand.type = UNKNOWN;
        second_operand.type = UNKNOWN;
    }
    if(error_found) {
        free(am_version);
        am_version = NULL;
        return 1;
    }
   /* while(1) { // for testing - print the binary code of the commands and operands
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

    for (current_node = code_image->first; current_node != NULL; current_node = current_node->next_node) {
        printf("\n%d %s", j++, current_node->word_command_in_binary);
        if (current_node->length >= 2)
            printf("\n%d %s", j++, current_node->word_operand1_in_binary);
        if (current_node->length == 3)
            printf("\n%d %s", j++, current_node->word_operand2_in_binary);
        printf("\n");
    }

    while(1) { // for testing - print the binary code of the data
        for(i = 0; i < data_node->length; i++) {
            printf("\n%d %s", j++, data_node->word_in_binary[i]);
        }
        printf("\n");
        if(data_node->next_node == NULL)
            break;
        *data_node = *data_node->next_node;
    }*/ // for testing
    if(entry_flag)
        ent_file_usher(file_name, label_table);
    if(extern_flag)
       ext_file_usher(file_name, code_image);
    printLabels(label_table); // for testing
    ob_file_usher(file_name, data_image, code_image , IC, DC);
    return 0;
}

