# 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(GrooveCGLib VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(GLM_INCLUDE_DIR "/opt/homebrew/Cellar/glm/1.0.1/include") # use for rayTracing test.
endif()

# 创建静态库（也可以创建动态库，使用 SHARED）
add_library(groovecglib INTERFACE)

# 子目录添加
add_subdirectory(imgui)
add_subdirectory(corelib)
add_subdirectory(glfw)
add_subdirectory(glad)

# 添加 GLM 的 include 路径（直接指定头文件路径）
target_include_directories(groovecglib INTERFACE ${GLM_INCLUDE_DIR})

# 添加其他库的 include 路径（如果需要）
target_include_directories(groovecglib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/corelib>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/glad>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/imgui>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/glfw/include>
)

# 链接库
target_link_libraries(groovecglib INTERFACE
    glad
    imgui
    corelib
    glfw
)

# 定义目标（例如 imgui、corelib 等子目录的依赖库）
target_compile_definitions(groovecglib PRIVATE ${GLM_DEFINITIONS})