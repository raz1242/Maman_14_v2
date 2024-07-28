#include "assembler_stage_0.h"

/**
 * the function receives a file name and processes it to create a .am file with the macros expanded
 * @param fileName - the name of the file to process
 * @return 1 if the file was processed successfully, 0 otherwise
 */
int stage_0_process_file(const char *fileName) {
    int i, insideMacroFlag = 0, firstWordOfLineLength, lineLocation, matchFound = 0, line_counter = 0;
    macro_array array;
    char macro_header[MAX_LENGTH_OF_MACRO_HEADER], macro_body[MAX_LENGTH_OF_MACRO_BODY], line[
        MAX_LENGTH_OF_MACRO_HEADER];
    char *non_space_line, *ptr_line;
    const char* file_AS = fileTypeCreator(fileName, ".as"), *file_AM = fileTypeCreator(fileName, ".am");
    FILE *as_extension = fopen(file_AS, "r");
    FILE *am_extension = fopen(file_AM, "w");

    macro_header[0] = '\0', macro_body[0] = '\0';
    macroArrayAllocator(&array, MIN_LENGTH_OF_MACRO_BODY);

    if (!fileInspection(as_extension)) {
        fclose(as_extension);
        fclose(am_extension);
        return 0;
    }

    while (fgets(line, MAX_LENGTH_OF_MACRO_BODY, as_extension)) {
        line_counter++;
        non_space_line = firstWordInLine(line);
        ptr_line = non_space_line;
        firstWordOfLineLength = firstWordLengthCounter(non_space_line);
        lineLocation = macroLocation(non_space_line, insideMacroFlag, firstWordOfLineLength);

        if (lineLocation == HEADER) {
            insideMacroFlag = 1;
            ptr_line += firstWordOfLineLength;
            non_space_line = firstWordInLine(ptr_line);
            ptr_line = non_space_line;
            firstWordOfLineLength = firstWordLengthCounter(non_space_line);
            strncpy(macro_header, non_space_line, firstWordOfLineLength);
            ptr_line += firstWordOfLineLength;
            non_space_line = firstWordInLine(ptr_line);
            if(*non_space_line != '\n') { /* check if there are redundant characters after setting up the macro name */
                error_handler(" ERROR_REDUNDANT_CHARACTERS_AFTER_MACRO_NAME", file_AS, line_counter);
                free(&array);
                fclose(as_extension);
                fclose(am_extension);
                return 1;
            }
            *(macro_header + firstWordOfLineLength) = '\0';
            if (isReservedWord(macro_header, firstWordOfLineLength)) {
                error_handler(" ERROR_MACRO_NAME_IS_RESERVED_WORD", 0, 0);
                free(&array);
                fclose(as_extension);
                fclose(am_extension);
                return 1;
            }
        } else if (lineLocation == BODY) {
            strncat(macro_body, line, strlen(line) + 1);
        } else if (lineLocation == END) {
            ptr_line += firstWordOfLineLength;
            non_space_line = firstWordInLine(ptr_line);
            if(*non_space_line != '\n') { /* check if there are redundant characters after macro end command */
                error_handler(" ERROR_REDUNDANT_CHARACTERS_AFTER_ENDMACRO", fileTypeCreator(file_AS, ".as"), line_counter);
                free(&array);
                fclose(as_extension);
                fclose(am_extension);
                return 1;
            }
            if(macroArrayAdd(&array, macro_header, macro_body)) {
                if(&array)
                    free(&array);
                fclose(as_extension);
                fclose(am_extension);
                return 1;
            }
            macro_body[0] = 0;
            insideMacroFlag = 0;
        } else  { /* REGULAR */
            for (i = 0; i < array.rep; i++) {
                if (strncmp(non_space_line, array.macro_element[i].name, firstWordOfLineLength) == 0) {
                     if (isEndOfLine(non_space_line + firstWordOfLineLength + 1)) {
                        matchFound = 1;
                        fputs(array.macro_element[i].body, am_extension);
                        break;
                    }
                }
            }
            if (!matchFound)
                fputs(line, am_extension);
            matchFound = 0;
        }
    }
    // add free()
    fclose(as_extension);
    fclose(am_extension);
    return 0;
}

/**
 * Allocates memory for the macro array and initializes its properties.
 * This function is responsible for initializing a macro array with a given size.
 * @param array - Pointer to the macro_array to allocate memory for.
 * @param size - The initial size of the macro array.
 */
void macroArrayAllocator(macro_array *array, const int size) {
    array->macro_element = malloc(size * sizeof(macro));
    if (array->macro_element == NULL) {
        printf("ERROR_FAILED_TO_ALLOCATE_MEM");
        free(array->macro_element);
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
int macroLocation(const char *str, const int flag, const int length) {
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
int macroArrayAdd(macro_array *array, const char *name, const char *body) {
    macro *new_array;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        length_of_array = (length_of_array) * 2;
        new_array = realloc(array->macro_element, length_of_array * sizeof(macro));
        if (!new_array) {
            printf(" ERROR_FAILED_TO_ALLOCATE_MEM");
            return 1;
        }
        array->macro_element = new_array;
    }

    array->macro_element[number_of_reps].name = malloc(strlen(name) + 1);
    array->macro_element[number_of_reps].body = malloc(strlen(body) + 1);

    if (!array->macro_element[number_of_reps].body || !array->macro_element[number_of_reps].name) {
        printf(" ERROR_FAILED_TO_ALLOCATE_MEM");
        free(array->macro_element[number_of_reps].name);
        free(array->macro_element[number_of_reps].body);
        return 1;
    }

    strcpy(array->macro_element[number_of_reps].name, name);
    strcpy(array->macro_element[number_of_reps].body, body);
    array->rep++;
    return 0;
}
