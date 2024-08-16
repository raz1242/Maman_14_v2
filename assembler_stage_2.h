#ifndef ASSEMBLER_STAGE_2_H
#define ASSEMBLER_STAGE_2_H

#include "table_utils.h"

/* Declarations */
int stage_2_process_file(const char *file_name, const label_array *label_table, const instruction_image *instruction_image, const data_image *data_image, int *error_flag);
int validate_operands(int command, operand first_operand, operand second_operand, const char *file_name, int line_counter);
int parse_instruction_stage_2(char *input_ptr, int command, char **source, char **dest);
int analyze_operand_stage_2(operand *operand, label_array label_array);

#endif /* ASSEMBLER_STAGE_2_H */
