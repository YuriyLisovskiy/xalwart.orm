cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}.orm")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

set(GNU_INCLUDE_DIR /usr/local/include)
set(GNU_LIB_DIR /usr/local/lib)

option(XW_BUILD_LIB "Build the library." ON)
if (${XW_BUILD_LIB})
    add_subdirectory(src)
endif()

option(XW_BUILD_TESTS "Build the library." OFF)
if (${XW_BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
