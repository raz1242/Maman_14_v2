#ifndef OUTPUT_HANDLING_H
#define OUTPUT_HANDLING_H

/* Memory Allocation Errors */
#define ERROR_FAILED_TO_ALLOCATE_MEM "Error: Failed to allocate memory"
#define ERROR_FAILED_TO_REALLOC_MEM "Error: Failed to reallocate memory"
#define ERROR_MEMORY_LIMIT_REACHED "Error: The memory limit has been reached"

/* File Handling Errors */
#define ERROR_FAILED_TO_OPEN_FILE "Error: Failed to open file"
#define ERROR_NO_FILE_PROVIDED "Error: No file was provided"
#define ERROR_FILE_NAME_IS_TOO_LONG "Error: The name of the file is too long"

/* Macro Handling Errors */
#define ERROR_REDUNDANT_CHARACTERS_AFTER_MACRO_NAME "Error: Redundant characters after macro name"
#define ERROR_MACRO_NAME_IS_RESERVED_WORD "Error: Macro name is a reserved word"
#define ERROR_DUPLICATE_MACRO_NAME "Error: Macro name has already been defined"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_ENDMACRO "Error: Redundant characters after endmacro command"

/* Label Handling Errors */
#define ERROR_LABEL_NOT_FOUND "Error: Label was not found"
#define ERROR_LABEL_IS_ENTRY "Error: Label is of type entry"
#define ERROR_LABEL_IS_EXTERN "Error: Label is of type extern"
#define ERROR_DUPLICATE_LABEL_NAME "Error: Duplicate label name"
#define ERROR_LABEL_CANNOT_BE_DEFINED_IN_EXTERN_COMMAND "Error: Label cannot be defined in '.extern' command"
#define ERROR_LABEL_CANNOT_BE_DEFINED_IN_ENTRY_COMMAND "Error: Label cannot be defined in '.entry' command"
#define ERROR_LABEL_NAME_IS_A_RESERVED_WORD "Error: Label name is a reserved word"
#define ERROR_LABEL_NAME_TOO_LONG "Error: Label name is too long"
#define ERROR_LABEL_NAME_IS_A_MACRO_NAME "Error: Label name is a macro name"

/* Operand Handling Errors */
#define ERROR_INVALID_FIRST_OPERAND "Error: Invalid first operand"
#define ERROR_INVALID_SECOND_OPERAND "Error: Invalid second operand"
#define ERROR_INVALID_OPERAND_TYPE_IN_FIRST_OPERAND "Error: Invalid operand type in first operand"
#define ERROR_INVALID_OPERAND_TYPE_IN_SECOND_OPERAND "Error: Invalid operand type in second operand"
#define ERROR_OPERAND_VALUE_OUT_OF_BOUNDS "Error: Operand value is out of bounds"
#define ERROR_MISSING_FIRST_OPERAND "Error: Missing first operand"
#define ERROR_MISSING_SECOND_OPERAND "Error: Missing second operand"
#define ERROR_REDUNDENT_CHARACTERS_AFTER_FIRST_OPERAND "Error: Redundant characters after first operand"
#define ERROR_REDUNDENT_CHARACTERS_AFTER_SECOND_OPERAND "Error: Redundant characters after second operand"

/* Data Handling Errors */
#define ERROR_MISSING_DATA_VALUE "Error: Missing data value"
#define ERROR_INVALID_DATA_VALUE "Error: Invalid data value"
#define ERROR_NUMBER_IS_OUT_OF_MACHINE_RANGE "Error: Number is out of machine range"

/* String Handling Errors */
#define ERROR_STRING_MUST_START_WITH_QUOTATION_MARK "Error: String must start with quotation mark"
#define ERROR_NO_STRING_FOUND "Error: No string found"
#define ERROR_MISSING_OPENING_QUOTATION_MARK "Error: Missing opening quotation mark"
#define ERROR_INVALID_CHARATER_FOUND_OUTSIDE_OF_QUOTATION_MARKS "Error: Invalid character found outside of quotation marks"

/* Command Errors */
#define ERROR_UNKNOWN_COMMAND "Error: Command is unknown"
#define ERROR_COMMAND_NOT_FOUND "Error: Command not found"
#define ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_A_SECOND_OPERAND "Error: The required command does not support a second operand"
#define ERROR_REQUIERED_COMMAND_DOES_NOT_SUPPORT_OPERANDS "Error: The required command does not support operands"
#define ERROR_BINARY_VERSION_COULD_NOT_BE_CREATED "Error: Binary version could not be created"
#define ERROR_REDUNDENT_CHARACTERS_AFTER_COMMAND "Error: Redundant characters after command"
#define ERROR_LABEL_CANNOT_BE_EMPTY_OF_COMMAND "Error: Label cannot be empty of command"

/* Syntax Errors */
#define ERROR_LINE_TOO_LONG "Error: The line is too long"
#define ERROR_COMMENT_NOT_AT_THE_BEGINNING_OF_THE_LINE "Error: Comment sign is not at the beginning of the line"
#define ERROR_REDUNDANT_CHARACTERS_AFTER_LABEL "Error: Redundant characters after label"
#define ERROR_REDUNDENT_COMMA_AFTER_COMMAND "Error: Redundant comma after command"
#define ERROR_MISSING_A_COMMA "Error: Missing a comma"
#define ERROR_TOO_MANY_COMMAS "Error: Too many commas"
#define ERROR_MULTIPLE_COMMA_FOUND "Error: Multiple commas found"


#endif /*OUTPUT_HANDLING_H*/
