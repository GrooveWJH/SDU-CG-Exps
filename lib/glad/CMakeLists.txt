# 声明 CMake 的最低版本
cmake_minimum_required(VERSION 3.10)

# 添加 glad 静态库
add_library(glad STATIC
    glad.c          # glad 的实现文件
)

# 设置 glad 的头文件路径
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}  # 包括 glad.h 和 khrplatform.h 所在目录
)