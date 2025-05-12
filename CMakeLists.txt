cmake_minimum_required(VERSION 3.16)
project(projekt_ki LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_DIR "/opt/homebrew/Cellar/llvm/20.1.4/lib/cmake/llvm")

# ----------------------------------------------------------------------------
# Source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
file(GLOB_RECURSE BENCHMARKS benchmark/*.cpp)
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
set(AdaptiveCpp_DIR "compiler/lib/cmake/AdaptiveCpp")
find_package(AdaptiveCpp REQUIRED CONFIG)

# ----------------------------------------------------------------------------
# Main application
add_executable(projekt_ki ${SRC_FILES})
target_include_directories(projekt_ki PRIVATE include)
add_sycl_to_target(TARGET projekt_ki)

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

add_executable(benchmarks
        ${BENCHMARKS}
        ${SRC_FILES_NO_MAIN}
)
add_sycl_to_target(TARGET tests)

target_include_directories(benchmarks PRIVATE include ext/Catch2/single_include)

target_link_libraries(benchmarks
        PRIVATE
        csv
        Catch2::Catch2WithMain
)
add_sycl_to_target(TARGET benchmarks)
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

catch_discover_tests(benchmarks
        TEST_PREFIX "benchmarks_abgabe"
        EXTRA_ARGS
        --benchmark-samples 10000
        --benchmark-warmup-time 500ms
)

# ----------------------------------------------------------------------------
# Code coverage (optional per platform)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(tests PRIVATE gcov)
    target_link_libraries(benchmarks PRIVATE gcov)
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
    target_link_libraries(benchmarks
            PRIVATE
            ${CLANG_RESOURCE_DIR}/lib/darwin/libclang_rt.profile_osx.a
    )
endif()
