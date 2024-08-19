assembler_main: assembler_main.o assembler_stage_0.o assembler_stage_1.o assembler_stage_2.o table_utils.o utils.o
	gcc -g -Wall -ansi -pedantic assembler_main.o assembler_stage_0.o assembler_stage_1.o assembler_stage_2.o table_utils.o utils.o -o assembler_main

assembler_main.o: assembler_main.c
	gcc -c -Wall -ansi -pedantic assembler_main.c -o assembler_main.o

assembler_stage_0.o: assembler_stage_0.c assembler_stage_0.h 
	gcc -c -Wall -ansi -pedantic assembler_stage_0.c -o assembler_stage_0.o

assembler_stage_1.o: assembler_stage_1.c assembler_stage_1.h 
	gcc -c -Wall -ansi -pedantic assembler_stage_1.c -o assembler_stage_1.o
	
assembler_stage_2.o: assembler_stage_2.c assembler_stage_2.h 
	gcc -c -Wall -ansi -pedantic assembler_stage_2.c -o assembler_stage_2.o

table_utils.o: table_utils.c table_utils.h
	gcc -c -Wall -ansi -pedantic table_utils.c -o table_utils.o
	
utils.o: utils.c utils.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o
	
