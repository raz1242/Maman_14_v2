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

typedef struct label_structure{
    char *name;
    int value;
    char *characteristic;
} label;

typedef struct label_array_structure{
    label *label_element;
    int rep;
    int length;
} label_array;

enum addressing_method{
    IMMEDIATE,
    LABEL_VALUE,
    REGISTER_PTR,
    REGISTER,
    UNKNOWN
};

typedef struct oprator_structure{
    char* name;
    enum addressing_method type;
} operand;


int stage_1_process_file(const char* am_version, symbol_table* symbol_table, data_image* data_image);
int lineLocation(const char *str);
void labelArrayAllocator(label_array *array, const int size);
int labelArrayAdd(label_array* array, const char* name, const int value, const char* characteristic);
char* directiveContent(const char* line, const int type);
int commandLocator(const char* command, int *L);
char* firstWordInLine(char* str);
int isCommand(const char* command);
int analyze_operand(operand* operand, const symbol_table symbol_table);
void analyze_command(char* ptr, const int command, int* L, const symbol_table symbol_table);


#endif //ASSEMBLER_STAGE_1_H
