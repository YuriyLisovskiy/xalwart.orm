set(CMAKE_CXX_FLAGS "-pthread")

set(BINARY unittests)

function(add_tests SUB_DIR SUB_NAME)
    set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests/${SUB_DIR})
    file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true ${TESTS_DIR}*.h ${TESTS_DIR}*.cpp)
    foreach(entry ${SOURCES})
        if (IS_DIRECTORY ${entry})
            list(REMOVE_ITEM SOURCES ${entry})
        endif()
    endforeach()
    list(FILTER SOURCES EXCLUDE REGEX ".*main.cpp$")
    set(FULL_BIN ${BINARY}-${SUB_NAME})
    add_executable(${FULL_BIN} ${SOURCES} ${TESTS_DIR}/main.cpp)

    if (NOT APPLE)
        target_link_libraries(${FULL_BIN} PUBLIC stdc++fs)
    endif()

    target_link_libraries(${FULL_BIN} PUBLIC gtest)

    if (${XW_USE_SQLITE3})
        # Search for sqlite3
        find_library(
            SQLITE3 sqlite3 REQUIRED
            PATHS ${DEFAULT_INCLUDE_PATHS}
        )
        target_link_libraries(${FULL_BIN} PUBLIC ${SQLITE3})
        add_compile_definitions(USE_SQLITE3)
    endif()

    if (${XW_USE_POSTGRESQL})
        # Search for PostgreSQL
        find_package(PostgreSQL REQUIRED)
        target_link_libraries(${FULL_BIN} PUBLIC ${PostgreSQL_LIBRARIES})
        include_directories(${PostgreSQL_INCLUDE_DIRS})
        add_compile_definitions(USE_POSTGRESQL)
    endif()

    target_link_libraries(${FULL_BIN} PUBLIC ${XALWART_BASE} ${LIBRARY_NAME})
endfunction()

function(add_sub_tests SUB_NAME)
    add_tests(${SUB_NAME}/ ${SUB_NAME})
endfunction(add_sub_tests)

add_tests("" all)
add_sub_tests(db)
add_sub_tests(exceptions)
add_sub_tests(queries)
