#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* fileTypeCreator(const char* str, const char* type);
int fileInspection(const FILE* file);
char* firstNonSpaceCharacter(char* str);
int firstWordLengthCounter(const char *str);
int operandLengthCounter(const char* str);
int isReservedWord(char* word, int length);
int isEndOfLine(char *str);
void parseData(const char *input, int **array, int *size);
void parseString(const char *input, int **array, int *size);
void parseCommandString(const char *input_ptr, const int command, char* source, char* dest);

#endif //UTILS_H
