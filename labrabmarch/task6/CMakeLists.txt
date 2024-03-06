cmake_minimum_required(VERSION 3.10)

project(Task6)

find_package(fmt CONFIG REQUIRED)

add_executable(Task6 task6.cpp)

target_link_libraries(Task6 PRIVATE fmt::fmt)