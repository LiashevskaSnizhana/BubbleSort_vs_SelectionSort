cmake_minimum_required(VERSION 3.16)
project(vsbtest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")

find_package (Threads)

add_executable(vsbtest main.cpp)
target_link_libraries(vsbtest ${CMAKE_THREAD_LIBS_INIT})