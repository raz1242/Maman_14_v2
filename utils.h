#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXTERN_ADDRESS -1
#define SIZE_OF_NUMBER_IN_BITS 12
#define LEANGTH_OF_ARE 3
typedef enum addressing_method{
    IMMEDIATE,
    LABEL_VALUE,
    REGISTER_PTR,
    REGISTER,
    UNKNOWN
} addressing_method;

typedef struct oprator_structure{
    char* name;
    enum addressing_method type;
} operand;

typedef enum code_structure{
    LABEL,  /*0*/
    DATA,   /*1*/
    STRING, /*2*/
    ENTRY,  /*3*/
    EXTERN, /*4*/
    CODE,   /*5*/
    IRRLEVANT /*6*/
} line_type;

typedef struct label{
    char *name;
    int address;
    line_type characteristic;
} label;

typedef struct label_array{
    label *label_element;
    int rep;
    int length;
} label_array;

char* fileTypeCreator(const char* str, const char* type);
int fileInspection(const FILE* file);
char* firstWordInLine(char* str);
int firstWordLengthCounter(const char *str);
int operandLengthCounter(const char* str);
int isReservedWord(char* word, int length);
int isEndOfLine(char *str);
int parseData(const char *input, int **array, int *size, int *DC, const char* file_name, int line_counter);
int parseString(const char *input, int **array, int *size, int *DC, const char *file_name, int line_counter);
int parseCommandString(char *input_ptr, int command, char **source, char **dest, const char *file_name, int line_counter);
label_array *labelArrayAllocator(int size);
int labelArrayAdd(label_array* array, const char* name, int address, line_type label_characteristic, const char* file_name, int line_counter);
int isLabel(const label_array* label_table, const char* name);
char* command_to_binary(int command, operand first_operand, operand second_operand, int L);
void error_handler(const char* error_message, const char* file_name, int line_counter);
char* decimalToBinary(int integer);
int parseCommandString_stage_2(char *input_ptr, int command, char **source, char **dest, const char *file_name, int line_counter);
char* immediate_operand_to_binary(operand operand);
char* register_name_to_binary(const char* register_name);
char* label_operand_to_binary(operand operand, label_array *label_table);
char* register_operand_to_binary(operand first_operand, operand second_operand);
void convert_operands_to_binary(operand first_operand, operand second_operand, char **first_operand_in_binary, char **second_operand_in_binary, label_array *label_table);
char* binary_to_octal(const char *binary_str);

#endif //UTILS_H
