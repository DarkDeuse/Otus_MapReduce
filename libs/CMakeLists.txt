cmake_minimum_required(VERSION 3.2)

add_library(mapReduceLib STATIC MapReduce.h MapReduce.cpp)
set_target_properties(mapReduceLib  PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(mapReduceLib PRIVATE)
target_include_directories(mapReduceLib PRIVATE ${CMAKE_BINARY_DIR})

if(MSVC)
	target_compile_options(mapReduceLib PRIVATE /W4)
else()
	target_compile_options(mapReduceLib PRIVATE -Wall -Wextra -Werror)
endif()
