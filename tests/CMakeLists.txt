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
    target_link_libraries(${FULL_BIN} PUBLIC gtest)

    if (${XW_USE_SQLITE3})
        add_compile_definitions(USE_SQLITE3)
        target_link_libraries(${FULL_BIN} PUBLIC sqlite3)
    endif()

    target_link_libraries(${FULL_BIN} PUBLIC ${FRAMEWORK_NAME}.base ${LIBRARY_NAME})
endfunction()

function(add_sub_tests SUB_NAME)
    add_tests(${SUB_NAME}/ ${SUB_NAME})
endfunction(add_sub_tests)

add_tests("" all)
add_sub_tests(db)
add_sub_tests(exceptions)
add_sub_tests(queries)
