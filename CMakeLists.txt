cmake_minimum_required(VERSION 3.21)
project(corpobitcrush VERSION 1.0 LANGUAGES C)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")

add_library(corpobitcrush SHARED
    src/arraylist.c
    src/init.c
)

target_include_directories(corpobitcrush PUBLIC deps/)

target_link_libraries(corpobitcrush PUBLIC m)