cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
project(3c)

include_directories(./include)

# add_library(3cTest STATIC src/auction.cpp src/types.cpp)

set(MAIN_FILES src/main.cpp)
set(TEST_FILES test/test.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wpedantic -Wextra")


add_executable(proj ${MAIN_FILES})
add_executable(test ${TEST_FILES})
# target_link_libraries(proj 3cTest)
# target_link_libraries(test 3cTest)