#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXTERN_ADDRESS -1
#define SIZE_OF_NUMBER_IN_BITS 12
#define LEANGTH_OF_ARE 3
#define STARTING_POINT_OF_MEMORY 100
typedef enum addressing_method{
    IMMEDIATE,
    LABEL_VALUE,
    REGISTER_PTR,
    REGISTER,
    UNKNOWN
} addressing_method;

typedef struct oprator_structure{
    char* name;
    char* word_in_binary;
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

/*declarations*/
char* file_name_extender(const char* str, const char* type);
int file_inspector(const FILE *file, const char *fileName);
char* skip_whitespace(char* str);
int first_word_length_counter(const char *str);
int operand_length_counter(const char* str);
int is_reserved_word(char* word, int length);
int is_end_of_line(char *str);
int parse_dot_data(const char *input, int **array, int *size, int *DC, const char* file_name, int line_counter);
int parse_dot_string(const char *input, int **array, int *size, int *DC, const char *file_name, int line_counter);
int parse_instruction(char *input_ptr, int command, char **source, char **dest, const char *file_name, int line_counter);
label_array *label_array_allocator(int size);// might move to table_utils
int add_label_to_array(label_array* array, const char* name, int address, line_type label_characteristic, const char* file_name, int line_counter);// might move to table_utils
int is_label(const label_array* label_table, const char* name);// might move to table_utils
char* command_to_binary(int command, operand first_operand, operand second_operand);
void error_handler(const char* error_message, const char* file_name, int line_counter);
char* decimal_to_binary(int integer);
int parse_instruction_stage_2(char *input_ptr, int command, char **source, char **dest, const char *file_name, int line_counter);
char* immediate_operand_to_binary(operand operand);
char* register_name_to_binary(const char* register_name);
char* label_operand_to_binary(operand operand, const label_array *label_table);// might move to table_utils
char* register_operand_to_binary(operand first_operand, operand second_operand);
void convert_operands_to_binary(operand first_operand, operand second_operand, char **first_operand_in_binary, char **second_operand_in_binary, const label_array *label_table);// might move to table_utils
char* binary_to_octal(const char *binary_str);
//void copy_operand(operand *operand_dest, const operand *operand_src, const char *binary_str);
void free_label_array(label_array *array);
void reset_opernads_type( operand *first_operand, operand *second_operand);


#endif //UTILS_H
