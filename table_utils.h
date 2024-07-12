#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EXTERN_ADDRESS -1

typedef struct data_node {
    char *original_line;
    int *word;
    int L;

    struct data_node *next_node;
} data_node;


typedef struct {
    data_node *first;
    data_node *last;
} data_image;


typedef struct code_node{
    char *original_line;
    int *word;
    int L;

    struct code_node *next;
    int been_here;
} code_node;


typedef struct {
    code_node *first;
    code_node *last;
} code_image;


typedef enum code_structure{
    LABEL,  /*0*/
    DATA,   /*1*/
    STRING, /*2*/
    ENTRY,  /*3*/
    EXTERN, /*4*/
    COMMAND /*5*/
} line_type;


typedef struct symbol_node {
    char* symbol_name;
    line_type line_type;
    int symbol_address;
    struct symbol_node* next_symbol;
} symbol_node;


typedef struct extern_node {
    char *symbol_name;
    int symbol_address;

    struct extern_node *next;
} extern_node;


typedef struct {
    symbol_node* first;
    symbol_node* last;
} symbol_table;


typedef struct extern_table {
    extern_node *first;
    extern_node *last;
} extern_table;


/*declarations*/
code_image *codeImageAllocator();
void newCodeNode(code_image *code_image, code_node *new_node);
data_image *dataImageAllocator();
data_node *newDataNode(const char *line, const int L, const int command_type, const int *data);
void dataNodeAdd(data_image *data_image, data_node *new_node);
symbol_node* newSymboleNode(const char *symbol_name, const int local_address, const line_type line_type);
symbol_table *symbolTableAllocator();
int symbolAdd(symbol_table *table, const char *symbol_name, const int *address, const line_type line_type);

#endif //TABLE_UTILS_H
