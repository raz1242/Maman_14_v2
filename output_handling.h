#ifndef OUTPUT_HANDLING_H
#define OUTPUT_HANDLING_H

/* Memory Allocation Errors */
#define ERROR_FAILED_TO_ALLOCATE_MEM "Failed to allocate memory"
#define ERROR_FAILED_TO_REALLOC_MEM "Failed to reallocate memory"
#define ERROR_MEMORY_LIMIT_REACHED "The memory limit has been reached, cannot analyze any more lines of code or proceed into the second stage"

/* File Handling Errors */
#define ERROR_FAILED_TO_OPEN_FILE "Failed to open file"
#define ERROR_NO_FILE_PROVIDED "No file was provided"
#define ERROR_FILE_NAME_IS_TOO_LONG "The name of the file is too long"

/* Macro Handling Errors */
#define ERROR_REDUNDANT_CHARACTERS_AFTER_MACRO_NAME "Redundant characters after macro name"
#define ERROR_MACRO_NAME_IS_RESERVED_WORD "Macro name is a reserved word"
#define ERROR_DUPLICATE_MACRO_NAME "Macro name has already been defined"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_ENDMACRO "Redundant characters after endmacro command"

/* Label Handling Errors */
#define ERROR_LABEL_NOT_FOUND "Label was not found"
#define ERROR_LABEL_IS_ENTRY "Label is of type entry"
#define ERROR_LABEL_IS_EXTERN "Label is of type extern"
#define ERROR_DUPLICATE_LABEL_NAME "The label name has already been defined"
#define ERROR_LABEL_CANNOT_BE_DEFINED_IN_EXTERN_COMMAND "Label cannot be defined in '.extern' command"
#define ERROR_LABEL_CANNOT_BE_DEFINED_IN_ENTRY_COMMAND "Label cannot be defined in '.entry' command"
#define ERROR_LABEL_NAME_IS_A_RESERVED_WORD "Label name is a reserved word"
#define ERROR_LABEL_NAME_TOO_LONG "Label name is too long"
#define ERROR_LABEL_NAME_IS_A_MACRO_NAME "Label name is a macro name"

/* Operand Handling Errors */
#define ERROR_INVALID_FIRST_OPERAND "Invalid first operand"
#define ERROR_INVALID_SECOND_OPERAND "Invalid second operand"
#define ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND "Invalid operand type in first operand"
#define ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND "Invalid operand type in second operand"
#define ERROR_OPERAND_VALUE_OUT_OF_BOUNDS "Operand value is out of bounds"
#define ERROR_MISSING_FIRST_OPERAND "Missing first operand"
#define ERROR_MISSING_SECOND_OPERAND "Missing second operand"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_FIRST_OPERAND "Redundant characters after first operand"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_SECOND_OPERAND "Redundant characters after second operand"

/* Data Handling Errors */
#define ERROR_MISSING_DATA_VALUE "Missing data, No data entered"
#define ERROR_INVALID_DATA_VALUE "Invalid data value"
#define ERROR_NUMBER_IS_OUT_OF_MACHINE_RANGE "Number is out of machine's range"

/* String Handling Errors */
#define ERROR_NO_LABEL_ENTERED "No label entered"
#define ERROR_MISSING_OPENING_QUOTATION_MARK "Missing opening quotation mark"
#define ERROR_MISSING_CLOSING_QUOTATION_MARK "Missing closing quotation mark"
#define ERROR_NO_STRING_FOUND "No string found"
#define ERROR_INVALID_CHARACTER_FOUND_OUTSIDE_OF_QUOTATION_MARKS "Invalid character found outside of quotation marks"

/* Command Errors */
#define ERROR_UNKNOWN_COMMAND "Command is unknown"
#define ERROR_COMMAND_NOT_FOUND "Command not found"
#define ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND "The required command does not support a second operand"
#define ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS "The required command does not support operands"
#define ERROR_BINARY_VERSION_COULD_NOT_BE_CREATED "Binary version could not be created"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_COMMAND "Redundant characters after command"
#define ERROR_LABEL_CANNOT_BE_EMPTY_OF_COMMAND "Label definition cannot be empty of any command"

/* Syntax Errors */
#define ERROR_LINE_TOO_LONG "Line exceeds the maximum allowed length"
#define ERROR_COMMENT_NOT_AT_THE_BEGINNING_OF_THE_LINE "Comment sign is not at the beginning of the line"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL "Redundant characters after label"
#define ERROR_REDUNDANT_COMMA_AFTER_COMMAND "Redundant comma after command"
#define ERROR_MISSING_A_COMMA "Missing a comma after first operand"
#define ERROR_TOO_MANY_COMMAS "Too many commas"
#define ERROR_MULTIPLE_COMMA_FOUND "Multiple commas found"


#endif /*OUTPUT_HANDLING_H*/
