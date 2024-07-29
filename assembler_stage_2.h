#ifndef ASSEMBLER_STAGE_2_H
#define ASSEMBLER_STAGE_2_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "table_utils.h"

#define LEANGTH_OF_ARE 3

int stage_2_process_file(const char *file_name, label_array *label_table, code_image *code_image);

#endif //ASSEMBLER_STAGE_2_H
