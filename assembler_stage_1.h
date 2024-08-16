#ifndef ASSEMBLER_STAGE_1_H
#define ASSEMBLER_STAGE_1_H

#include "table_utils.h"

/* Declarations */
int stage_1_process_file(const char* file_name, label_array* label_table, instruction_image* instruction_image, data_image* data_image, const macro_name_image *macro_name_image, int* error_flag);
int is_duplicate_label_name(const label_array *label_table, const char *label_name);
void validate_label_name(char *label_header, const label_array *label_table, const macro_name *macro_name, const char *am_version, int line_counter, int *error_flag);

#endif /* ASSEMBLER_STAGE_1_H */
