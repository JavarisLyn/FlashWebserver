cmake_minimum_required(VERSION 3.2)

project(FlashWebserver VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")
set(CXX_FLAGS -g -Wall -std=c++11 -pthread)

find_package(Threads REQUIRED)
INCLUDE_DIRECTORIES(/usr/include/lyf)

add_executable(FlashWebserver src/Utils/Timer.cpp src/Utils/Utils.cpp src/Server/Epoll.cpp src/Server/Channel.cpp src/Server/EventLoop.cpp src/Server/EventLoopThread.cpp src/Server/EventLoopThreadPool.cpp src/Server/Server.cpp src/Server/Http.cpp src/Main.cpp)
#根据指定文件生成可执行文件
#https://www.jianshu.com/p/19765d4932a4

link_libraries(/usr/lib/libLogger.a)

#引入静态库,需要添加这个
target_link_libraries(FlashWebserver /usr/lib/libLogger.a)
target_link_libraries(FlashWebserver ${CMAKE_THREAD_LIBS_INIT})
#为target连接库的头文件路径
#用g++编译单个文件https://blog.csdn.net/qq_50615776/article/details/113097749