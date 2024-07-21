#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define EXTERN_ADDRESS -1

typedef struct data_node {
    char* original_line;
    int* word;
    int length;

    struct data_node *next_node;
} data_node;

typedef struct {
    data_node *first;
    data_node *last;
} data_image;

typedef struct code_node{
    char* original_line;
    char* word;
    int length;

    struct code_node *next_node;
} code_node;

typedef struct {
    code_node *first;
    code_node *last;
} code_image;
/*
typedef struct code_node{
    char *original_line;
    int *word;
    int L;
} code_node;

typedef struct code_image{
    code_node *code_element;
    int rep;
    int length;
} code_image;

typedef struct data_node {
    char *original_line;
    int *word;
    int L;
} data_node;

typedef struct data_image{
    data_node *data_element;
    int rep;
    int length;
} data_image;
*/

typedef struct extern_node {
    char *symbol_name;
    int symbol_address;

    struct extern_node *next;
} extern_node;

typedef struct extern_table {
    extern_node *first;
    extern_node *last;
} extern_table;


/*declarations*/
code_image *codeImageAllocator();
code_node *newCodeNode(const char *line, const int L, const char* word_in_binary);
void codeNodeAdd(code_image *code_image, code_node *new_node);
data_image *dataImageAllocator();
data_node *newDataNode(const char *line, const int command_type, const int **data, const int array_size);
void dataNodeAdd(data_image *data_image, data_node *new_node);


#endif //TABLE_UTILS_H
