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

int stage_0_process_file(const char* fileName);
void macroArrayAllocator(macro_array *array, int size);
int macroLocation(const char *str, int flag, int length);
int macroArrayAdd(macro_array *array, const char* name, const char* body);


#endif //ASSEMBLER_STAGE_0_H