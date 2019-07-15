cmake_minimum_required(VERSION 2.8)
project( skeletonization )
find_package( OpenCV REQUIRED )
add_executable( skeletonization main.cpp )

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(CMAKE_COMPILER_IS_GNUCC)

target_link_libraries( skeletonization ${OpenCV_LIBS})