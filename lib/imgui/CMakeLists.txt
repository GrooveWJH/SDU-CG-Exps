add_library(imgui STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp

    # 后端实现文件
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

# 设置头文件路径
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}       # ImGui 核心头文件路径
    ${CMAKE_CURRENT_SOURCE_DIR}/backends  # 后端实现头文件路径
    ${CMAKE_SOURCE_DIR}/lib/glfw/include  # 添加 GLFW 头文件路径
)
# 链接 GLFW 和 OpenGL 库
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)


# # 如果需要使用 backends 的文件，可以添加
# target_sources(imgui PRIVATE
#     backends/imgui_impl_glfw.cpp
#     backends/imgui_impl_opengl3.cpp
# )

# # 如果需要 OpenGL 或其他库，链接它们
# find_package(OpenGL REQUIRED)
# target_link_libraries(imgui PRIVATE OpenGL::GL)
