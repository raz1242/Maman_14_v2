#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include "utils.h"



typedef struct macro_name {
    char* name;
    struct macro_name *next;
} macro_name;

typedef struct {
    macro_name *first;
    macro_name *last;
} macro_name_image;

typedef struct data_node {
    char* original_line; // need to remove, only for debugging
    int* char_in_ASCII;
    char** word_in_binary;
    int length;

    struct data_node *next_node;
} data_node;

typedef struct {
    data_node *first;
    data_node *last;
} data_image;

typedef struct instruction_node{
    char* original_line; // need to remove, only for debugging
    char* word_command_in_binary;
    int decimal_address_in_machine;
    operand first_operand;
    operand second_operand;
    int length;

    struct instruction_node *next_node;
} instruction_node;

typedef struct {
    instruction_node *first;
    instruction_node *last;
} instruction_image;

/*Declarations*/
/* Allocation and Initialization Functions */
instruction_image *instruction_image_allocator();
instruction_node *new_instruction_node(const char *line, int L, const char* word_in_binary);
data_image *data_image_allocator();
data_node *new_data_node(const char *line, int array_size, const int *data);
macro_name_image *macro_name_image_allocator();
macro_name *new_macro_name(const char* name);
label_array *label_array_allocator(int size);

/* Addition Functions */
void macro_name_add(macro_name_image *macro_name_image, macro_name *new_node);
void add_label_to_array(label_array *array, const char *name, int address, line_characteristic label_characteristic);
void instruction_node_add(instruction_image *instruction_image, instruction_node *new_node);
void data_node_add(data_image *data_image, data_node *new_node);

/* Conversion Functions */
void convert_ascii_to_binary(data_node *data_node);
char *label_operand_to_binary(operand operand, const label_array *label_table);
void convert_operands_to_binary(operand first_operand, operand second_operand, char **first_operand_in_binary, char **second_operand_in_binary, const label_array *label_table);

/* File Handling Functions */
void ob_file_usher(const char* file_name, const data_image *data_image, const instruction_image *instruction_image, int IC, int DC);
void ext_file_usher(const char *file_name, const instruction_image *instruction_image);
void ent_file_usher(const char *file_name, const label_array *label_table);

/* Validator Function */
int is_label(const label_array *label_table, const char *name);

/* free Functions */
void free_data_node(data_node *node);
void free_data_image(data_image *image);
void free_operand(const operand *operand);
void free_instruction_node(instruction_node *node);
void free_instruction_image(instruction_image *instruction_image);
void free_macro_name_image(macro_name_image *macro_name_image);


#endif /* TABLE_UTILS_H */
