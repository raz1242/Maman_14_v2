#ifndef ASSEMBLER_STAGE_1_H
#define ASSEMBLER_STAGE_1_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "table_utils.h"

#define MIN_LENGTH_OF_LABEL_BODY 1
#define MAX_LENGTH_OF_LINE 50
#define MAX_LENGTH_OF_LABEL_HEADER 32
#define MAX_LENGTH_OF_LABEL_VALUE 50
#define MAX_LENGTH_OF_COMMAND_NAME 5
#define LENGTH_OF_BINARY_WORD 16


int stage_1_process_file(const char* am_version, label_array* label_table, data_image* data_image, code_image* code_image);
int lineLocation(const char *str);
char* directiveContent(const char* line, const int type);
int commandLocator(const char* command, int *L);
char* firstWordInLine(char* str);
int isCommand(const char* command);
int analyze_operand(operand* operand, const label_array label_table);
void analyze_command(char* ptr, const int command, int* L, const label_array label_table, char* word_in_binary);

#endif //ASSEMBLER_STAGE_1_H
