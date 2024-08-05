#ifndef ASSEMBLER_STAGE_2_H
#define ASSEMBLER_STAGE_2_H


#include "table_utils.h"
#include "assembler_stage_1.h"

#define LEANGTH_OF_ARE 3

int stage_2_process_file(const char *file_name, const label_array *label_table, const code_image *code_image, const data_image *data_image);
int validate_operands(int command, operand first_operand, operand second_operand, const char *file_name, int line_counter);

#endif //ASSEMBLER_STAGE_2_H
