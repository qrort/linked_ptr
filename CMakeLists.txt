cmake_minimum_required(VERSION 3.13)
project(linked_ptr)

include_directories(${linked_ptr_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(linked_ptr
        main.cpp
        linked_ptr.hpp linked_ptr.hpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -D_GLIBCXX_RELEASE")
endif()