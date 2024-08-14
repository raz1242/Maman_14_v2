#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "output_handling.h"

#define EXTERN_ADDRESS -1
#define STARTING_POINT_OF_MEMORY 100
#define LENGTH_OF_COMMA 1
#define LENGTH_OF_NULL_TERMINATOR 1
#define LENGTH_OF_ARE 3
#define SIZE_OF_NUMBER_IN_BITS 12
#define SIZE_OF_WORD_IN_OCTAL 5
#define SIZE_OF_REGISTER_IN_BITS 15
#define MAX_POSSIBLE_NUMBER 2047
#define MIN_POSSIBLE_NUMBER -2048


typedef enum addressing_method {
    IMMEDIATE,
    LABEL_VALUE,
    REGISTER_PTR,
    REGISTER,
    UNKNOWN
} addressing_method;

typedef struct oprator_structure {
    char *name;
    char *word_in_binary;
    enum addressing_method type;
} operand;

typedef enum code_structure {
    LABEL_DEFINITION, /*0*/
    DATA, /*1*/
    STRING, /*2*/
    ENTRY, /*3*/
    EXTERN, /*4*/
    INSTRCTION, /*5*/
    IRRLEVANT /*6*/
} line_type;

typedef struct label {
    char *name;
    int address;
    line_type characteristic;
} label;

typedef struct label_array {
    label *label_element;
    int rep;
    int length;
} label_array;

/* Declarations */
char *file_name_extender(const char *str, const char *type);
int file_inspector(const FILE *file, const char *fileName);
char *skip_whitespace(char *str);
char *skip_to_next_word(char *str, int length);
int first_word_length_counter(const char *str);
int operand_length_counter(const char *str);
int is_reserved_word(char *word, int length);
int is_end_of_line(char *str);
int parse_dot_data(const char *input, int **array, int *size, int *DC, const char *file_name, int line_counter);
int parse_dot_string(const char *input, int **array, int *size, int *DC, const char *file_name, int line_counter);
int parse_instruction(char *input_ptr, int command, char **source, char **dest, const char *file_name,
                      int line_counter);
int which_register(const char *operand_name);
char *command_to_binary(int command, operand first_operand, operand second_operand);
void error_handler(const char *error_message, const char *file_name, int line_counter);
char *decimal_to_binary(int integer);
char *immediate_operand_to_binary(operand operand);
char *register_name_to_binary(const char *register_name);
char *register_operand_to_binary(operand first_operand, operand second_operand);
char *binary_to_octal(const char *binary_str);
void free_label_array(label_array *array);
void reset_opernads_type(operand *first_operand, operand *second_operand);
int is_immidiate_out_of_bounds(operand operand);
int line_location(char *str);
int is_command(const char* command);
int which_command(const char* command);
int analyze_command(char *ptr, int command, int *L, char *word_in_binary, const char *file_name, int line_counter);
int analyze_operand(operand *operand);

#endif /* UTILS_H */
