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
        if (label_table->label_element[i].characteristic == ENTRY)
            printf("Label: %s, Characteristic: ENTRY\n", label_table->label_element[i].name);
        if (label_table->label_element[i].characteristic == EXTERN)
            printf("Label: %s, Characteristic: EXTERN\n", label_table->label_element[i].name);
        else
            printf("Label: %s, Characteristic: IRRLEVANT\n", label_table->label_element[i].name);
    }
}

int stage_2_process_file(const char *file_name, const label_array *label_table, const code_image *code_image,
                         const data_image *data_image) {
    int i, IC = 0, DC = 0, location, firstWordInLineLength, command_in_line, is_vaild, line_counter = 0, extern_flag = 0
            , entry_flag = 0, error_found = 0;
    int j = STARTING_POINT_OF_MEMORY; // for testing
    char line[MAX_LENGTH_OF_LINE], label_header[MAX_LENGTH_OF_LABEL_HEADER + 1];
    char *am_version = NULL, *ptr = NULL, *non_space_ptr = NULL, *first_operand_in_binary = NULL, *
            second_operand_in_binary = NULL;
    FILE *am_extension_file = NULL;

    code_node *code_node = code_image->first;
    data_node *data_node = data_image->first;
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
        //printf(" \n\n%s", line); fflush(stdout); // for testing
        is_vaild = 0;
        non_space_ptr = skip_whitespace(line);
        ptr = non_space_ptr;
        firstWordInLineLength = first_word_length_counter(ptr);
        location = line_location(ptr);
        if (location == LABEL) {
            strncpy(label_header, ptr, firstWordInLineLength);
            non_space_ptr = skip_to_next_word(ptr, firstWordInLineLength + LENGTH_OF_COLON);
            ptr = non_space_ptr;
            firstWordInLineLength = first_word_length_counter(ptr);
            location = line_location(ptr);
        }
        if (location == ENTRY) {
            entry_flag = 1;
            non_space_ptr = skip_to_next_word(ptr, firstWordInLineLength);
            ptr = non_space_ptr;
            firstWordInLineLength = first_word_length_counter(ptr);
            for (i = 0; i < label_table->rep; i++) {
                if (!strncmp(label_table->label_element[i].name, ptr, firstWordInLineLength)) {
                    is_vaild = 1;
                    label_table->label_element[i].characteristic = ENTRY;
                    break;
                } //maybe put the if(is_vailid) in the if else statement
            }
            if (!is_vaild) {
                error_handler("ERROR_LABEL_NOT_FOUND", am_version, line_counter);
                error_found = 1;
            }
        }
        if (location == EXTERN) {
            extern_flag = 1;
        }
        if (location == DATA || location == STRING) {
            convert_ascii_to_binary(data_node);
            DC += data_node->length;
            data_node = data_node->next_node;
        }
        if (location == CODE) {
            //printf("\ncheckpoint1"); fflush(stdout); // for testing
            command_in_line = which_command(ptr);
            parse_instruction_stage_2(ptr, command_in_line, &code_node->first_operand.name, &code_node->second_operand.name,
                                      am_version, line_counter);
            reset_opernads_type(&code_node->first_operand, &code_node->second_operand);
            if (code_node->first_operand.name)
                if(analyze_operand_stage_2(&code_node->first_operand, *label_table)) {
                    error_handler("ERROR_INVALID_FIRST_OPERAND", am_version, line_counter);
                    error_found = 1;
                }
           //printf("\ncheckpoint2"); fflush(stdout);// for testing
            if (code_node->second_operand.name)
                if(analyze_operand_stage_2(&code_node->second_operand, *label_table)){
                //if(analyze_operand(&code_node->second_operand)) {
                    error_handler("ERROR_INVALID_SECOND_OPERAND", am_version, line_counter);
                    error_found = 1;
                }
            //printf("\ncheckpoint3"); fflush(stdout); // for testing
            convert_operands_to_binary(code_node->first_operand, code_node->second_operand, &first_operand_in_binary, &second_operand_in_binary, label_table);
            //printf("\ncheckpoint4"); fflush(stdout); // for testing
            if (code_node->length >= 2) {
                if(code_node->first_operand.type != UNKNOWN && first_operand_in_binary)
                    strcpy(code_node->first_operand.word_in_binary, first_operand_in_binary);
            } else
                code_node->first_operand.word_in_binary = NULL;
            if (code_node->length == 3) {
                if(code_node->second_operand.type != UNKNOWN && second_operand_in_binary)
                    strcpy(code_node->second_operand.word_in_binary, second_operand_in_binary);
            } else
                code_node->second_operand.word_in_binary = NULL;
            code_node->decimal_address_in_machine = IC + STARTING_POINT_OF_MEMORY;
            IC += code_node->length;
            if (code_node != code_image->last && code_node->next_node != NULL)
                code_node = code_node->next_node;
        }
    }
    if (error_found) {
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
    //printLabels(label_table); // for testing
    if (entry_flag)
        ent_file_usher(file_name, label_table);
    if (extern_flag)
        ext_file_usher(file_name, code_image);
    ob_file_usher(file_name, data_image, code_image, IC, DC);
    return 0;
}
