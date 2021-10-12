cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}.orm")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

set(ROOT_DIR /usr/local)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(LIB_DIR ${ROOT_DIR}/lib)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

option(XW_CONFIGURE_LIB "Build the library." ON)
if (${XW_CONFIGURE_LIB})
    add_subdirectory(src)
endif()

option(XW_CONFIGURE_TESTS "Build the library." OFF)
if (${XW_CONFIGURE_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
