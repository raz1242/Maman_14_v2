#include "assembler_stage_0.h"

#include "assembler_stage_1.h"

/**
 * the function receives a file name and processes it to create a .am file with the macros expanded
 * @param file_name - the name of the file to process
 * @return 1 if the file was processed successfully, 0 otherwise
 */
int stage_0_process_file(const char *file_name) {
    int i, is_inside_macro = 0, first_word_of_line_length, macro_body_length = 0, macro_body_allocated_size = MIN_LENGTH_OF_MACRO_BODY, line_location, is_error = 0, match_found = 0, line_counter = 0;
    macro_array macro_array;
    char macro_header[MAX_LENGTH_OF_MACRO_HEADER], line[MAX_LENGTH_OF_LINE_2];
    char *non_space_line, *ptr_line, *macro_body,  *output_buffer = NULL;
    char *as_version = file_name_extender(file_name, ".as"), *am_version = NULL;
    FILE *as_extension_file = fopen(as_version, "r");
    FILE *am_extension_file = NULL;

    if (file_inspector(as_extension_file, am_version)) {
        fclose(as_extension_file);
        return 1;
    }
    macro_header[0] = '\0',
    macro_body = malloc(MIN_LENGTH_OF_MACRO_BODY);
    if (macro_body == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    macro_array_allocator(&macro_array, MIN_LENGTH_OF_MACRO_BODY);

    while (fgets(line, MAX_LENGTH_OF_LINE_2, as_extension_file)) {
        line_counter++;
        if(strlen(line) >= MAX_LENGTH_OF_LINE_LENGTH){ /*check if the line is longer than the max of 80 characters*/
            error_handler("ERROR_LINE_TOO_LONG", as_version, line_counter);
            is_error = 1;
            continue;
        }
        if(strncmp(line, ";", 1)==0 || strncmp(line, "\n", 1)==0)
            continue;
        non_space_line = skip_whitespace(line);
        if(non_space_line == NULL || *non_space_line == '\n' || *non_space_line == '\r')
            continue;
        ptr_line = non_space_line;
        first_word_of_line_length = first_word_length_counter(non_space_line);
        line_location = macro_location(non_space_line, is_inside_macro, first_word_of_line_length);

        if (line_location == HEADER) {
            is_inside_macro = 1;
            non_space_line = skip_to_next_word(non_space_line, first_word_of_line_length);
            ptr_line = non_space_line;
            first_word_of_line_length = first_word_length_counter(ptr_line);
            strncpy(macro_header, non_space_line, first_word_of_line_length);
            non_space_line = skip_to_next_word(non_space_line, first_word_of_line_length);
            if (*non_space_line != '\n' && *non_space_line != '\r') { /*check if there're redundant characters after the macro name set up*/
                error_handler("ERROR_REDUNDANT_CHARACTERS_AFTER_MACRO_NAME", as_version, line_counter);
                is_error = 1;
            }
            macro_header[first_word_of_line_length] = '\0';
            if (is_reserved_word(macro_header, first_word_of_line_length)) { /*check if the macro name is a reserved word*/
                error_handler("ERROR_MACRO_NAME_IS_RESERVED_WORD", as_version, line_counter);
                is_error = 1;
            }
            if(is_duplicate_macro_name(&macro_array, macro_header)){ /*check if the macro name already exists*/
                error_handler("ERROR_DUPLICATE_MACRO_NAME", as_version, line_counter);
                is_error = 1;
            }
        } else if (line_location == BODY) {
            add_line_to_macro_body(&macro_body, non_space_line, &macro_body_length, &macro_body_allocated_size);
        } else if (line_location == END) {
            non_space_line = skip_to_next_word(ptr_line, first_word_of_line_length);
            if (*non_space_line != '\n' && *non_space_line != '\r') { /*check if there're redundant characters after the end of the macro*/
                error_handler("ERROR_REDUNDANT_CHARACTERS_AFTER_ENDMACRO", as_version, line_counter);
                is_error = 1;
            }
            macro_array_add(&macro_array, macro_header, macro_body);
            macro_body_length = 0;
            macro_body_allocated_size = MIN_LENGTH_OF_MACRO_BODY;
            memset(macro_body, 0, macro_body_length);
            is_inside_macro = 0;
        } else /* REGULAR */{
            if (is_error)
                continue;
            for (i = 0; i < macro_array.rep; i++) {
                if (strncmp(non_space_line, macro_array.macro_element[i].name, first_word_of_line_length) == 0) {
                    if (is_end_of_line(non_space_line + first_word_of_line_length + 1)) {
                        match_found = 1;
                        append_to_buffer(&output_buffer, macro_array.macro_element[i].body);
                        break;
                    }
                }
            }
            if (!match_found)
                append_to_buffer(&output_buffer, line);
            match_found = 0;
        }
    }
    fclose(as_extension_file);
    free(macro_body);
    free(as_version);

    if (!is_error) {
        am_version = file_name_extender(file_name, ".am");
        am_extension_file = fopen(am_version, "w");
        if (am_extension_file == NULL) {
            printf("ERROR_FAILED_TO_OPEN_FILE\n");
            free(output_buffer);
            macro_array_free(&macro_array);
            return 0;
        }
        free(am_version);
        fputs(output_buffer, am_extension_file);
        fclose(am_extension_file);
    }
    free(output_buffer);
    macro_array_free(&macro_array);
    return is_error;
}

/**
 * Allocates memory for the macro array and initializes its properties.
 * This function is responsible for initializing a macro array with a given size.
 *
 * @param array - Pointer to the macro_array to allocate memory for.
 * @param size - The initial size of the macro array.
 */
void macro_array_allocator(macro_array *array, const int size) {
    array->macro_element = malloc(size * sizeof(macro));
    if (array->macro_element == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    array->rep = 0;
    array->length = size;
}

/**
 * This function checks if a given line is the start of a macro definition (HEADER),
 * part of a macro body (BODY), the end of a macro (END), or a regular line (REGULAR).
 *
 * @param str The string to be analyzed.
 * @param flag An integer flag indicating whether the current processing context is within a macro definition.
 * @param length The length of the first word in the line. This is used to help identify the end of
 *               a macro definition by comparing against the length of the "endmacr" keyword.
 * @return An integer representing the location type of the line.
 */
int macro_location(const char *str, const int flag, const int length) {
    if (str && str[4]) {
        if (str[4] == ' ' && strncmp(str, "macr", 4) == 0)
            return HEADER;
    }
    if (flag && strncmp(str, "endmacr", length) != 0)
        return BODY;
    if (flag && strncmp(str, "endmacr", length) == 0)
        return END;
    return REGULAR;
}

/**
 * Adds a macro to the macro array, resizing the array size if needed.
 * @param array Macro array to add to.
 * @param name Name of the macro.
 * @param body Content of the macro.
 * @return 0 on success.
 */
int macro_array_add(macro_array *array, const char *name, const char *body) {
    macro *new_array;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        length_of_array = length_of_array * 2;
        new_array = realloc(array->macro_element, length_of_array * sizeof(macro));
        if (new_array == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            exit(1);
        }
        array->macro_element = new_array;
        array->length = length_of_array;
    }

    array->macro_element[number_of_reps].name = malloc(strlen(name) + 1);
    if (array->macro_element[number_of_reps].name == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        exit(1);
    }
    array->macro_element[number_of_reps].body = malloc(strlen(body) + 1);
    if (array->macro_element[number_of_reps].body == NULL) {
        printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
        free(array->macro_element[number_of_reps].name);
        exit(1);
    }

    strcpy(array->macro_element[number_of_reps].name, name);
    strcpy(array->macro_element[number_of_reps].body, body);
    array->rep++;
    return 0;
}
/**
 * Frees the memory allocated for the macro array.
 *
 * @param array A pointer to the macro array to be freed.
 */
void macro_array_free(const macro_array *array) {
    int i;
    for (i = 0; i < array->rep; i++) {
        if(array->macro_element[i].name)
            free(array->macro_element[i].name);
        if(array->macro_element[i].body)
            free(array->macro_element[i].body);
    }
    free(array->macro_element);
}

/**
 * Adds a line to the macro body, reallocating memory if necessary.
 *
 * @param macro_body Pointer to the macro body string. This will be reallocated if needed.
 * @param line The line to be added to the macro body.
 * @param current_length Pointer to the current length of the macro body.
 * @param allocated_size Pointer to the allocated size of the macro body.
 */
void add_line_to_macro_body(char **macro_body, const char *line, int *current_length, int *allocated_size) {
    const int line_length = strlen(line);
    char *new_body;

    if (*current_length + line_length + 1 > *allocated_size) {
        *allocated_size += line_length;
        new_body = realloc(*macro_body, *allocated_size);
        if (new_body == NULL) {
            printf("ERROR_FAILED_TO_REALLOC_MEM\n");
            free(*macro_body);
            exit(1);
        }
        *macro_body = new_body;
    }

    strcpy(*macro_body + *current_length, line);
    *current_length += line_length;
}

/**
 * Appends content to the output buffer.
 *
 * @param buffer Pointer to the output buffer.
 * @param content Content to append to the buffer.
 */
void append_to_buffer(char **buffer, const char *content) {
    int new_size;
    if (*buffer == NULL) {
        new_size = strlen(content) + 1;
        *buffer = malloc(new_size);
        if (*buffer == NULL) {
            printf("\nERROR_FAILED_TO_ALLOCATE_MEM");
            exit(1);
        }
        strcpy(*buffer, content);
    } else {
        new_size = strlen(*buffer) + strlen(content) + 1;
        *buffer = realloc(*buffer, new_size);
        if (*buffer == NULL) {
            printf("ERROR_FAILED_TO_REALLOC_MEM\n");
            exit(1);
        }
        strcat(*buffer, content);
    }
}

/**
 * Checks if a given macro name already exists in the macro array.
 *
 * This function iterates through the macro array and compares each macro's name
 * with the provided name. Returns a number based on the match result.
 *
 * @param array A pointer to the macro array to be checked.
 * @param name The name of the macro to check for duplicates.
 * @return 1 if the macro name is a duplicate, 0 otherwise.
 */
int is_duplicate_macro_name (const macro_array *array, const char *name){
    int i;
    for(i = 0; i < array->rep; i++){
        if(strcmp(array->macro_element[i].name, name) == 0)
            return 1;
    }
    return 0;
}