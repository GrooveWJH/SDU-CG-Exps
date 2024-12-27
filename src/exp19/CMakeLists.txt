cmake_minimum_required(VERSION 3.14)
project(MyQtProject)

# 设置 CMAKE_PREFIX_PATH，指向 Qt 的安装路径
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")  # 根据您的实际路径进行调整

# 设置 CMake 策略 CMP0084 为 NEW，以使用现代的 Qt 查找模块
cmake_policy(SET CMP0084 NEW)

# 查找 Qt6 包（如果您使用的是 Qt6）
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# 如果使用的是 Qt5，请使用以下行替换上面的行
# find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 添加可执行文件
add_executable(MyQtApp main.cpp)

# 链接 Qt 库
target_link_libraries(MyQtApp Qt6::Widgets)
# 如果使用的是 Qt5，请使用以下行替换上面的行
# target_link_libraries(MyQtApp Qt5::Widgets)