#ifndef ASSEMBLER_STAGE_1_H
#define ASSEMBLER_STAGE_1_H

#include "table_utils.h"

#define MAX_SIZE_OF_MEMORY 4096
#define MIN_LENGTH_OF_LABEL_BODY 1
#define MAX_LENGTH_OF_LINE 81
#define MAX_LENGTH_OF_LABEL_HEADER 31
#define MAX_LENGTH_OF_LABEL_VALUE 50
#define MAX_LENGTH_OF_COMMAND_NAME 5
#define LENGTH_OF_BINARY_WORD 16
#define LENGTH_OF_COLON 1

int stage_1_process_file(const char* file_name, label_array* label_table, code_image* code_image, data_image* data_image, int* error_flag);
int line_location(char *str);
int which_command(const char* command);
char* skip_whitespace(char* str);
int is_command(const char* command);
int analyze_command(char *ptr, int command, int *L, char *word_in_binary, const char *file_name, int line_counter);
int analyze_operand(operand *operand);
int is_duplicate_label_name(const label_array *label_table, const char *label_name);

#endif /* ASSEMBLER_STAGE_1_H */
