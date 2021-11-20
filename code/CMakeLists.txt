cmake_minimum_required(VERSION 2.8)
project(5821)

set(CMAKE_CXX_STANDARD 11)
set(CXX_FLAGS -std=c++11)

add_executable(mesher main.cpp face2faceindex.cpp face2faceindex.h  Cartesian3.cpp Cartesian3.h faceindex2directededge.cpp faceindex2directededge.h)
