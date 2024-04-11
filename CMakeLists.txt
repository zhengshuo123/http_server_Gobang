# 指定 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(learn_linux_system_programming VERSION 1.0)

# 指定 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# 添加可执行文件
add_executable(server server.c )
