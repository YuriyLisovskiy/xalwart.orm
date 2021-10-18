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

if (NOT APPLE)
    target_link_libraries(${LIBRARY_NAME} PUBLIC stdc++fs)
endif()

option(XW_USE_SQLITE3 "Link sqlite3 library and add compile XW_USE_SQLITE3 compile definition." OFF)
if (${XW_USE_SQLITE3})
    # Use sqlite3 driver.
    add_compile_definitions(USE_SQLITE3)

    # Search for sqlite3
    find_library(
        SQLITE3 sqlite3 REQUIRED
        PATHS ${DEFAULT_INCLUDE_PATHS}
    )
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${SQLITE3})
endif()

# Link dependencies.
target_link_libraries(${LIBRARY_NAME} PUBLIC ${XALWART_BASE})

set(LIBRARY_ROOT /usr/local CACHE STRING "Installation root directory.")
set(LIBRARY_INCLUDE_DIR ${LIBRARY_ROOT}/include CACHE STRING "Include installation directory.")
set(LIBRARY_LINK_DIR ${LIBRARY_ROOT}/lib CACHE STRING "Library installation directory.")

# Install the library.
install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION ${LIBRARY_LINK_DIR}
    LIBRARY DESTINATION ${LIBRARY_LINK_DIR}
)
install(
    DIRECTORY "${LIB_SRC_DIR}/"
    DESTINATION ${LIBRARY_INCLUDE_DIR}/${LIBRARY_NAME}
    FILES_MATCHING
    PATTERN "*.h"
)
