cmake_minimum_required(VERSION 3.16)
project(projekt_ki LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------------------
# Source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/*.cpp)

# ----------------------------------------------------------------------------
# Main application
add_executable(projekt_ki ${SRC_FILES})
target_include_directories(projekt_ki PRIVATE include)

# ----------------------------------------------------------------------------
# Exclude main.cpp for tests
set(SRC_FILES_NO_MAIN "")
foreach(file ${SRC_FILES})
    if(NOT file MATCHES "src/main\\.cpp$")
        list(APPEND SRC_FILES_NO_MAIN ${file})
    endif()
endforeach()

# ----------------------------------------------------------------------------
# External dependencies
add_subdirectory(ext/csv-parser)
add_subdirectory(ext/Catch2)

# ----------------------------------------------------------------------------
# Test executable
add_executable(tests
        ${TEST_FILES}
        ${SRC_FILES_NO_MAIN}
)

target_include_directories(tests PRIVATE include ext/Catch2/single_include)

target_link_libraries(tests
        PRIVATE
        csv
        Catch2::Catch2WithMain
)

# ----------------------------------------------------------------------------
# Enable test discovery
include(CTest)
include(Catch)

catch_discover_tests(tests
        TEST_PREFIX "UnitTests."
        EXTRA_ARGS
        --benchmark-samples 500
        --benchmark-warmup-time 100ms
)

# ----------------------------------------------------------------------------
# Code coverage (optional per platform)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(tests PRIVATE gcov)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_program(CLANG_PATH clang++)
    execute_process(
            COMMAND ${CLANG_PATH} --print-resource-dir
            OUTPUT_VARIABLE CLANG_RESOURCE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    target_link_libraries(tests
            PRIVATE
            ${CLANG_RESOURCE_DIR}/lib/darwin/libclang_rt.profile_osx.a
    )
endif()
