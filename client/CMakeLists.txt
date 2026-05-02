cmake_minimum_required(VERSION 4.2)
project(bot C)

set(CMAKE_C_STANDARD 17)

add_executable(kirknet bot/main.c
        bot/connection.c
        bot/macros.h
        bot/io_loop.c
        bot/ring.h
        bot/ring.h
)