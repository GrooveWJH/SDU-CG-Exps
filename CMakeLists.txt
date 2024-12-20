cmake_minimum_required(VERSION 3.10)
project(SDU-ComputerGraphics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SUPPRESS_REGENERATION ON)

# 设置全局可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

configure_file(${CMAKE_SOURCE_DIR}/lib/corelib/ggtest.h.in ${CMAKE_SOURCE_DIR}/lib/corelib/ggtest.h)

# 添加 lib 子目录
add_subdirectory(lib)

# 定义实验列表
set(EXPERIMENTS_LIST)

# 遍历 experiments 目录中的所有源文件
file(GLOB experiments_sources "${CMAKE_SOURCE_DIR}/src/*.cxx")

foreach(src_file ${experiments_sources})
    # 获取文件名（不包含路径和扩展名）
    get_filename_component(target_name ${src_file} NAME_WE)

    # 为每个源文件创建可执行目标
    add_executable(${target_name} ${src_file})
    target_link_libraries(${target_name} PRIVATE groovecglib)

    # 添加到实验列表
    list(APPEND EXPERIMENTS_LIST ${target_name})
endforeach()

# 自定义目标，用于编译所有实验
add_custom_target(all_exps DEPENDS ${EXPERIMENTS_LIST})