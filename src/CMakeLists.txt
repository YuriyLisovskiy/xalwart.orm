# Search for sources.
set(LIB_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true ${LIB_SRC_DIR}/*.h ${LIB_SRC_DIR}/*.cpp)
foreach(entry ${SOURCES})
    if (IS_DIRECTORY ${entry})
        list(REMOVE_ITEM SOURCES ${entry})
    endif()
endforeach()
list(LENGTH SOURCES SOURCES_COUNT)
message(STATUS "[INFO] Sources found: ${SOURCES_COUNT}")

option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." ON)

# Add the library.
add_library(${LIBRARY_NAME} ${SOURCES})

option(XW_USE_SQLITE3 "Link sqlite3 library and add compile XW_USE_SQLITE3 compile definition." OFF)
if (${XW_USE_SQLITE3})
    # Use sqlite3 driver.
    add_compile_definitions(USE_SQLITE3)
    target_link_libraries(${LIBRARY_NAME} PUBLIC sqlite3)
endif()

# Link dependencies.
target_link_libraries(${LIBRARY_NAME} PUBLIC ${FRAMEWORK_NAME}.base)

# Install the library.
install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION ${LIB_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
)
install(
    DIRECTORY "${LIB_SRC_DIR}/"
    DESTINATION ${INCLUDE_DIR}/${LIBRARY_NAME}
    FILES_MATCHING
    PATTERN "*.h"
)
