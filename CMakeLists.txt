cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}.orm")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

set(GNU_INCLUDE_DIR /usr/local/include)
set(GNU_LIB_DIR /usr/local/lib)

function(print SRC_LIST)
    foreach(entry ${SRC_LIST})
        message(${entry})
    endforeach(entry)
endfunction(print)

set(LIB_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true ${LIB_SRC_DIR}/*.h ${LIB_SRC_DIR}/*.cpp)
foreach(entry ${SOURCES})
    if (IS_DIRECTORY ${entry})
        list(REMOVE_ITEM SOURCES ${entry})
    endif()
endforeach()
list(LENGTH SOURCES SOURCES_COUNT)
message(STATUS "[INFO] Sources found: ${SOURCES_COUNT}")

if (NOT ${SOURCES_COUNT} EQUAL 0)
    if (NOT DEFINED USE_SQLITE3)
        set(USE_SQLITE3 no)
    endif()

    # Build the library.
    if (NOT BUILD_SHARED_LIB)
        set(BUILD_SHARED_LIB off)
    endif()

    if (${BUILD_SHARED_LIB})
        add_library(${LIBRARY_NAME} SHARED ${SOURCES})
    else()
        add_library(${LIBRARY_NAME} STATIC ${SOURCES})
    endif()

    if (USE_SQLITE3)
        # Use sqlite3 driver.
        add_compile_definitions(USE_SQLITE3)
        target_link_libraries(${LIBRARY_NAME} PUBLIC sqlite3)
    endif()

    target_link_libraries(${LIBRARY_NAME} PUBLIC ${FRAMEWORK_NAME}.base)

    # Install library.
    install(
        TARGETS ${LIBRARY_NAME}
        ARCHIVE DESTINATION ${GNU_LIB_DIR}
        LIBRARY DESTINATION ${GNU_LIB_DIR}
    )

    install(
        DIRECTORY "${LIB_SRC_DIR}/"
        DESTINATION ${GNU_INCLUDE_DIR}/${LIBRARY_NAME}
        FILES_MATCHING
        PATTERN "*.h"
    )
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_subdirectory(tests)
endif()
