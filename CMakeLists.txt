cmake_minimum_required(VERSION 3.2)

project(FlashWebserver VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")
set(CXX_FLAGS -g -Wall -std=c++11 =pthread)

add_executable(FlashWebserver src/HelloWorld.cpp)
#根据指定文件生成可执行文件
#https://www.jianshu.com/p/19765d4932a4

#用g++编译单个文件https://blog.csdn.net/qq_50615776/article/details/113097749