cmake_minimum_required(VERSION 3.16)
project(projekt_ki LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0")

set(BUILD_SHARED_LIBS OFF)

# ----------------------------------------------------------------------------
# Source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
file(GLOB_RECURSE BENCHMARKS benchmark/*.cpp)

# ----------------------------------------------------------------------------
# Multi Threading
# find_package(TBB REQUIRED)

# ----------------------------------------------------------------------------
# Exclude main.cpp for tests
set(SRC_FILES_NO_MAIN "")
foreach(file ${SRC_FILES})
    if(NOT file MATCHES "src/main\\.cpp$")
        list(APPEND SRC_FILES_NO_MAIN ${file})
    endif()
endforeach()

# ----------------------------------------------------------------------------
# Main application

add_library(core STATIC ${SRC_FILES_NO_MAIN})
target_include_directories(core PUBLIC include)

add_executable(projekt_ki src/main.cpp)
target_link_libraries(projekt_ki PRIVATE core)

# ----------------------------------------------------------------------------
# client application
add_executable(Client clientsrc/Client.cpp)
target_include_directories(Client PRIVATE ext/headerOnly/json)
target_link_libraries(Client PRIVATE core)


# ----------------------------------------------------------------------------
# client application
add_executable(DummyClient clientsrc/dummyClient.cpp)
target_include_directories(DummyClient PRIVATE ext/headerOnly/json)
target_link_libraries(DummyClient PRIVATE core)

# ----------------------------------------------------------------------------
# External dependencies
add_subdirectory(ext/Catch2)

# ----------------------------------------------------------------------------
# Test executable
add_executable(tests ${TEST_FILES})

target_include_directories(tests PRIVATE ext/Catch2/single_include ext/headerOnly/csv tests)

target_link_libraries(tests
        PRIVATE
        core
        # TBB::tbb
        Catch2::Catch2WithMain
)

add_executable(benchmarks ${BENCHMARKS})

target_include_directories(benchmarks PRIVATE ext/Catch2/single_include ext/headerOnly/csv tests)

target_link_libraries(benchmarks
        PRIVATE
        core
        # TBB::tbb
        Catch2::Catch2WithMain
)

# ----------------------------------------------------------------------------
# Enable test discovery
include(CTest)
include(Catch)

catch_discover_tests(tests
        TEST_PREFIX "UnitTests."
        EXTRA_ARGS
        --benchmark-samples 100
        --benchmark-warmup-time 100ms
)

catch_discover_tests(benchmarks
        TEST_PREFIX "Benchmarks"
        EXTRA_ARGS
        --benchmark-samples 10
        --benchmark-warmup-time 500ms
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(tests PRIVATE gcov)
    target_link_libraries(benchmarks PRIVATE gcov)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Enabling GCC coverage flags on macOS")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage -lgcov)
        target_link_libraries(tests PRIVATE gcov)
        target_link_libraries(benchmarks PRIVATE gcov)
    endif ()
endif()
