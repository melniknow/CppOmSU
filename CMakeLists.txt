cmake_minimum_required(VERSION 3.20)
project(IMIT_CPP)

#SET(MYCOMPILE_FLAGS "-Wall -Wextra")

set(Sanitizers 1)

if (Sanitizers EQUAL 1)
    message("Compile with Sanitizers")
    set(CMAKE_C_COMPILER "clang++-9")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++-9")

    set(MYCOMPILE_FLAGS "-std=c++17 -pedantic  -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS ${MYCOMPILE_FLAGS})

    add_definitions("-D GLIBCXX_DEBUG -D GLIBCXX_DEBUG_PEDANTIC")
endif ()

include_directories(include/task_1)
include_directories(include/task_2)
include_directories(include/task_3)
include_directories(include/task_4)
include_directories(include/task_5)
include_directories(include/task_6)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# ------------ BOOST ------------------
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS *boost libraries here*)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(progname file1.cxx file2.cxx)
    target_link_libraries(progname ${Boost_LIBRARIES})
endif ()
# ----------------------------------

# ------------------TEST-------------------------------
add_executable(
        all_tests
        test/test_main.cpp
)

target_include_directories(
        all_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
        all_tests PRIVATE
        gtest gtest_main
)
# ---------------------------------------------------

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)