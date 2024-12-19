# test lib by GG :D
add_library(ggtest STATIC ggtest.cxx)

# 设置头文件路径
target_include_directories(ggtest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)