cmake_minimum_required(VERSION 2.9)

project(selfmuduo)
#设置最终动态库路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#设置附加调试选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

#定义参与编译的源代码 .下文件 放到SRC变量
aux_source_directory(. SRC_LIST)
#编译动态库
add_library(mymuduo SHARED ${SRC_LIST})