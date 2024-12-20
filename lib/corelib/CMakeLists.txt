# test lib by GG :D
# 查找当前目录下所有 .cxx 文件
file(GLOB SOURCES "*.cxx")

add_library(corelib STATIC ${SOURCES})

# 设置头文件路径
target_include_directories(corelib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/glad/
    # ${CMAKE_SOURCE_DIR}/lib/imgui/backends
)

target_link_libraries(corelib PUBLIC imgui)
