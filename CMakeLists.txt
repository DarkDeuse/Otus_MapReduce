cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(mapReduce VERSION ${PROJECT_VESRION})

add_executable(mapReduce main.cpp)
set_target_properties(mapReduce PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQIRED ON)
target_link_libraries(mapReduce PRIVATE boost_system mapReduceLib)

if(MSVC)
	target_compile_options(mapReduce PRIVATE /W4)
else()
	target_compile_options(mapReduce PRIVATE -Wall -Wextra -Werror)
endif()

add_subdirectory (${PROJECT_SOURCE_DIR}/libs)

install(TARGETS mapReduce RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

include(CPack)