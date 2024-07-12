#include "assembler_stage_0.h"

int stage_0_process_file(const char* fileName) {
    int i, insideMacroFlag = 0, firstWordOfLineLength, lineLocation, matchFound = 0;
    macro_array array;
    char macro_header[MAX_LENGTH_OF_MACRO_HEADER], macro_body[MAX_LENGTH_OF_MACRO_BODY], line[MAX_LENGTH_OF_MACRO_HEADER];
    char* non_space_line;
    FILE *as_extension = fopen(fileTypeCreator(fileName,".as"), "r");
    FILE *am_extension = fopen(fileTypeCreator(fileName,".am"), "w");

    macro_header[0] = '\0', macro_body[0] = '\0';
    macroArrayAllocator(&array, MIN_LENGTH_OF_MACRO_BODY);

    if (!fileInspection(as_extension)) {
        fclose(as_extension);
        fclose(am_extension);
        return 0;
    }

    while (fgets(line, MAX_LENGTH_OF_MACRO_BODY, as_extension)) {
        non_space_line = firstNonSpaceCharacter(line);
        firstWordOfLineLength = firstWordLengthCounter(non_space_line);
        lineLocation = macroLocation(non_space_line, insideMacroFlag, firstWordOfLineLength);

        if (lineLocation == HEADER) {
            insideMacroFlag = 1;
            non_space_line += firstWordOfLineLength;
            non_space_line = firstNonSpaceCharacter(non_space_line);
            firstWordOfLineLength = firstWordLengthCounter(non_space_line) + 1;
            strncpy(macro_header, non_space_line, firstWordOfLineLength+1);
            *(macro_header + firstWordOfLineLength) = '\n';

            if (isReservedWord(macro_header, firstWordOfLineLength)) {
                //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_INVALID_MACRO_NAME);
                //free_macro_array(&array);
                return 0;
            }
        }
        else if (lineLocation == BODY) {
            strncat(macro_body, line, strlen(line) + 1);
        }
        else if (lineLocation == END) {
            macroArrayAdd(&array, macro_header, macro_body);
            macro_body[0] = 0;
            insideMacroFlag = 0;
        }
        else if (lineLocation == REGULAR) {
            for (i = 0; i < array.rep; i++) {
                if (strncmp(non_space_line, array.macro_element[i].name, firstWordOfLineLength-1) == 0) {
                    if(isEndOfLine(non_space_line + firstWordOfLineLength) == 1) {
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
    //free_macro_array(&array);
    fclose(as_extension);
    fclose(am_extension);
    return 1;
}

void macroArrayAllocator(macro_array *array, const int size) {
    array->macro_element = malloc(size * sizeof(macro));
    if (array->macro_element == NULL) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(array->macro_element);
        exit(1);
    }
    array->rep = 0;
    array->length = size;
}

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

int macroArrayAdd(macro_array* array, const char* name, const char* body) {
    macro* new_array;
    const int number_of_reps = (array->rep);
    int length_of_array = (array->length);

    if (number_of_reps == length_of_array) {
        length_of_array = (length_of_array) * 2;
        new_array = realloc(array->macro_element, length_of_array * sizeof(macro));
        if (!new_array) {
            //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_REALLOCATE_MEM);
            exit(1);
        }
        array->macro_element = new_array;
    }

    array->macro_element[number_of_reps].name = malloc(strlen(name) + 1);
    array->macro_element[number_of_reps].body = malloc(strlen(body) + 1);

    if (!array->macro_element[number_of_reps].body || !array->macro_element[number_of_reps].name) {
        //PRINT_MESSAGE(ERROR_MSG_TYPE, ERROR_FAILED_TO_ALLOCATE_MEM);
        free(array->macro_element[number_of_reps].name);
        free(array->macro_element[number_of_reps].body);
        exit(1);
    }

    strcpy(array->macro_element[number_of_reps].name, name);
    strcpy(array->macro_element[number_of_reps].body, body);
    array->rep++;
    return 0;
}