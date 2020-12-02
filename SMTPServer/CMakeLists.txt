cmake_minimum_required(VERSION 2.6.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(server C)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/..)
#set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_BINARY_DIR  ${dir})

include_directories(${dir}/SMTPShared)
set (SOURCES server.c pollbuf_dictionary.c 
    ../SMTPShared/shared_strings.c ../SMTPShared/error/error.c ../SMTPShared/logger/logger.c)

option(USE_DEBUG "Enter debug mode" OFF)

if (USE_DEBUG)
  add_definitions(-DDEBUG)
endif()

add_compile_options(-D_GNU_SOURCE)
#add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(server ${SOURCES})