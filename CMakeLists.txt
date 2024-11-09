# CMakeLists.txt

cmake_minimum_required(VERSION 3.22)
project(objcurses)

set(CMAKE_CXX_STANDARD 20)

# collect all source files recursively, excluding build directory
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}")

# creating executable
add_executable(${PROJECT_NAME} ${SOURCES})

# linking ncurses library
find_package(Curses REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIR})

# linking math library
target_link_libraries(${PROJECT_NAME} PRIVATE m)