cmake_minimum_required(VERSION 3.28)
project(Maman_14v2 C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(Maman_14v2
        assembler_main.c
        assembler_stage_0.c
        assembler_stage_0.h
        assembler_stage_1.c
        assembler_stage_1.h
        utils.c
        utils.h
        table_utils.c
        table_utils.h
        assembler_stage_1.c
        assembler_stage_2.c
        assembler_stage_2.h)
