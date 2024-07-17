#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXTERN_ADDRESS -1

typedef enum code_structure{
    LABEL,  /*0*/
    DATA,   /*1*/
    STRING, /*2*/
    ENTRY,  /*3*/
    EXTERN, /*4*/
    CODE    /*5*/
} line_type;

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


typedef struct label_structure{
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
void parseData(const char *input, int **array, int *size, int* DC);
void parseString(const char *input, int  **array, int *size, int *DC);
void parseCommandString(const char *input_ptr, const int command, char** source, char** dest);
void labelArrayAllocator(label_array *array, const int size);
int labelArrayAdd(label_array* array, const char* name, const int address, const line_type label_characteristic);
int isLabel(const label_array* label_table, const char* name);

#endif //UTILS_H
