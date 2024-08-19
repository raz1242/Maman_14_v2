#include "assembler_stage_0.h"
#include "assembler_stage_1.h"
#include "assembler_stage_2.h"


int main(const int argc, char *argv[]) {
    int index, error_flag = FALSE;
    macro_name_image *my_macro_name_image;
    data_image *my_data_image;
    instruction_image *my_instruction_image;
    label_array* label_table;

    /* Check that at least one file name was passed to the program */
    if (argc == 1) {
        printf("%s\n", ERROR_NO_FILE_PROVIDED);
        exit(EXIT_FAILURE);
    }
    for (index = 1; index < argc; index++) {
        error_flag = FALSE; /* reset the error flag */
        if (strlen(argv[index]) > MAX_LENGTH_OF_FILE_NAME) { /* Check if the file name's too long */
            printf("%s\n", ERROR_FILE_NAME_IS_TOO_LONG);
            continue;
        }
        my_macro_name_image = macro_name_image_allocator();

        if (stage_0_process_file(argv[index], my_macro_name_image)) { /* The Pre-Stage */
            printf("%s\n", REPORT_STAGE_0_FAILED);
            free_macro_name_image(my_macro_name_image);
            continue;
        }

        label_table = label_array_allocator();
        my_data_image = data_image_allocator();
        my_instruction_image = instruction_image_allocator();

        stage_1_process_file(argv[index], label_table, my_instruction_image, my_data_image, my_macro_name_image, &error_flag); /* The First Stage */
        free_macro_name_image(my_macro_name_image); /* Macro_name_image is no longer needed after first stage*/

        if (stage_2_process_file(argv[index], label_table, my_instruction_image, my_data_image, &error_flag)) /* The Second Stage */
            printf("%s\n", REPORT_STAGE_FAILED);
        else
            printf("%s\n", REPORT_ALL_STAGE_SUCCESS);

        /* Free the rest of the memory */
        free_label_array(label_table);
        free_instruction_image(my_instruction_image);
        free_data_image(my_data_image);
    }
    if(error_flag)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}