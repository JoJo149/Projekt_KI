cmake_minimum_required(VERSION 3.16)
project(projekt_ki LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----------------------------------------------------------------------------
# Source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE TEST_FILES test/*.cpp)

# ----------------------------------------------------------------------------
# Main application
add_executable(projekt_ki ${SRC_FILES})
target_include_directories(projekt_ki PRIVATE include)

# ----------------------------------------------------------------------------
# Tests
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

# DONT INCLUDE MAIN
set(SRC_FILES_NO_MAIN "")
foreach(file ${SRC_FILES})
    if(NOT file MATCHES "src/main\\.cpp$")
        list(APPEND SRC_FILES_NO_MAIN ${file})
    endif()
endforeach()

add_executable(tests
        ${TEST_FILES}
        ${SRC_FILES_NO_MAIN}
)
target_include_directories(tests PRIVATE include ext/Catch2/single_include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# ----------------------------------------------------------------------------
# Enable test discovery
include(CTest)
include(Catch)
catch_discover_tests(tests)
