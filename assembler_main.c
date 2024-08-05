#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler_stage_0.h"
#include "assembler_stage_1.h"
#include "assembler_stage_2.h"


int main(const int argc, char *argv[]) {
    int i;
    data_image *my_data_image;
    code_image *my_code_image;
    label_array* label_table;

    /* Check that at least one file name was passed to the program */
    if (argc == 1) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_NO_FILES_PROVIDED);
        //exit(1);
    }

    /* Process each file if the file name does not exceed the maximum length */
    //PRINT_MESSAGE(INFO_MSG_TYPE, INFO_START_OF_ASSEMBLER);
    for (i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 80) {
            printf("ERROR_FILE_NAME_IS_TOO_LONG");
            return 1;
        }
        if(stage_0_process_file(argv[i])) {
            printf("pre stage failed\n"); // for testing
            return 1 ;
        }
            printf("pre stage success\n"); // for testing

        label_table = label_array_allocator(MIN_LENGTH_OF_LABEL_BODY);
        my_data_image = data_image_allocator();
        my_code_image = code_image_allocator();

        if(stage_1_process_file(argv[i], label_table, my_code_image, my_data_image))
            printf("stage 1 failed\n"); // for testing
        else
            printf("stage 1 success\n"); // for testing
        if(stage_2_process_file(argv[i], label_table, my_code_image, my_data_image))
            printf("stage 2 failed\n"); // for testing
        else
            printf("stage 2 success\n"); // for testing

        free_label_array(label_table);
        free_code_image(my_code_image);
        free_data_image(my_data_image);
    }
    printf("\n"); // for testing
    return 0;
}