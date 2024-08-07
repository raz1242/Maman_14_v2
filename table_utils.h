#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include "utils.h"

#define EXTERN_ADDRESS -1

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

typedef struct code_node{
    char* original_line; // need to remove, only for debugging
    char* word_command_in_binary;
    int decimal_address_in_machine;
    operand first_operand;
    operand second_operand;
    int length;

    struct code_node *next_node;
} code_node;

typedef struct {
    code_node *first;
    code_node *last;
} code_image;

/*declarations*/
code_image *code_image_allocator();
code_node *new_code_node(const char *line, int L, const char* word_in_binary);
void code_node_add(code_image *code_image, code_node *new_node);
data_image *data_image_allocator();
data_node *new_data_node(const char *line, int array_size, const int *data);
void data_node_add(data_image *data_image, data_node *new_node);
void convert_ascii_to_binary(data_node *data_node);
void ob_file_usher(const char* file_name, const data_image *data_image, const code_image *code_image, int IC, int DC);
void ext_file_usher(const char *file_name, const code_image *code_image);
void ent_file_usher(const char *file_name, const label_array *label_table);
void free_data_node(data_node *node);
void free_data_image(data_image *image);
void free_operand(const operand *operand);
void free_code_node(code_node *node);
void free_code_image(code_image *code_image);
void free_code_node_temp(code_node *node);

#endif
