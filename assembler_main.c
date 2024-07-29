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
        //PRINT_MESSAGE(INFO_MSG_TYPE, argv[i]);
        if(stage_0_process_file(argv[i])) {
            printf(" pre stage failed\n");
            return 1 ;
        }

        my_data_image = dataImageAllocator();
        my_code_image = codeImageAllocator();
        label_table = labelArrayAllocator(MIN_LENGTH_OF_LABEL_BODY);

        stage_1_process_file(argv[i], label_table, my_data_image, my_code_image);
        stage_2_process_file(argv[i], label_table, my_code_image);
    }
    printf("\n");
    //PRINT_MESSAGE(INFO_MSG_TYPE, INFO_ALL_FILES_HAVE_BEEN_PROCESSED);
    return 0;
}
// reminder to free all the memory like label_table, my_data_image, my_code_image
