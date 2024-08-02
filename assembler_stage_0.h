#ifndef ASSEMBLER_STAGE_0_H
#define ASSEMBLER_STAGE_0_H


#include "utils.h"

#define MIN_LENGTH_OF_MACRO_BODY 1
#define MAX_LENGTH_OF_MACRO_BODY 80
#define MAX_LENGTH_OF_MACRO_HEADER 50
#define AS_FILE_EXTENTION as
#define AM_FILE_EXTENTION am


typedef struct macro_structure{
    char *name;
    char *body;
} macro;

typedef struct macro_array_structure{
    macro *macro_element;
    int rep;
    int length;
} macro_array;

enum precode_structure{
    HEADER,
    BODY,
    END,
    REGULAR
};

int stage_0_process_file(const char* file_name);
void macro_array_allocator(macro_array *array, int size);
int macro_location(const char *str, int flag, int length);
int macro_array_add(macro_array *array, const char* name, const char* body);
void macro_array_free(const macro_array *array);

#endif //ASSEMBLER_STAGE_0_H