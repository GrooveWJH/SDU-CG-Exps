# 创建静态库（也可以创建动态库，使用 SHARED）

add_library(groovecglib INTERFACE)

add_subdirectory(imgui)

add_subdirectory(corelib)

add_subdirectory(glfw)

add_subdirectory(glad)

target_link_libraries(groovecglib INTERFACE glad imgui corelib glfw )