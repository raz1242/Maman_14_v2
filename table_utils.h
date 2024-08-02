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
    char* operand1_name;
    char* word_operand1_in_binary;
    char* operand2_name;
    char* word_operand2_in_binary;
    int address_in_machine;
    int length;

    struct code_node *next_node;
} code_node;

typedef struct {
    code_node *first;
    code_node *last;
} code_image;
/*
typedef struct extern_node {
    char *label_name;
    int label_address;

    struct extern_node *next;
} extern_node;

typedef struct extern_table {
    extern_node *first;
    extern_node *last;
} extern_table;
*/

/*declarations*/
code_image *codeImageAllocator();
code_node *newCodeNode(const char *line, int L, const char* word_in_binary);
void codeNodeAdd(code_image *code_image, code_node *new_node);
data_image *dataImageAllocator();
data_node *newDataNode(const char *line, int array_size, const int *data);
void dataNodeAdd(data_image *data_image, data_node *new_node);
void convert_ASCII_to_binary(data_node *data_node);
void ob_file_usher(const char* file_name, data_image *data_image, code_image *code_image, int IC, int DC);
void ext_file_usher(const char *file_name, const code_image *code_image, const label_array *label_table);
void ent_file_usher(const char *file_name, const label_array *label_table);
#endif //TABLE_UTILS_H
