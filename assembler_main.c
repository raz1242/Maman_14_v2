#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler_stage_0.h"
#include "assembler_stage_1.h"
#include "assembler_stage_2.h"


int main(const int argc, char *argv[]) {
    int index, error_flag = 0;
    macro_name_image *my_macro_name_image;
    data_image *my_data_image;
    instruction_image *my_instruction_image;
    label_array* label_table;

    /* Check that at least one file name was passed to the program */
    if (argc == 1) {
        printf("%s\n", ERROR_NO_FILE_PROVIDED);
        exit(1);
    }
    for (index = 1; index < argc; index++) {
        if (strlen(argv[index]) > 80) {
            printf("%s\n", ERROR_FILE_NAME_IS_TOO_LONG);
            return 1;
        }
        my_macro_name_image = macro_name_image_allocator();

        if(stage_0_process_file(argv[index], my_macro_name_image)) {
            printf("pre stage failed\n"); // for testing
            return 1;
        }
        printf("pre stage success\n"); // for testing

        label_table = label_array_allocator(MIN_LENGTH_OF_LABEL_BODY);
        my_data_image = data_image_allocator();
        my_instruction_image = instruction_image_allocator();

        if(stage_1_process_file(argv[index], label_table, my_instruction_image, my_data_image, my_macro_name_image, &error_flag))
            printf("stage 1 failed\n"); // for testing
        else
            printf("stage 1 success\n"); // for testing

        free_macro_name_image(my_macro_name_image);

        if(stage_2_process_file(argv[index], label_table, my_instruction_image, my_data_image, &error_flag))
            printf("stage 2 failed\n"); // for testing
        else
            printf("stage 2 success\n"); // for testing

        free_label_array(label_table);
        free_instruction_image(my_instruction_image);
        free_data_image(my_data_image);
    }
    return 0;
}