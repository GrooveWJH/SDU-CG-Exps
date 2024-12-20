#include <glad.h>       // OpenGL 函数加载器 (glad)，用于加载 OpenGL 的函数指针
#include <GLFW/glfw3.h> // GLFW 库，用于创建窗口、处理用户输入等
#include <imgui.h>      // ImGui 核心库，用于构建图形界面
#include <backends/imgui_impl_glfw.h>   // ImGui 的 GLFW 后端，负责与 GLFW 集成
#include <backends/imgui_impl_opengl3.h> // ImGui 的 OpenGL3 后端，负责与 OpenGL 集成
#include <iostream> // 标准输入输出库

// 窗口大小回调函数：当窗口大小改变时，更新 OpenGL 的视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // 设置视口大小，范围为窗口的新宽度和高度
}

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1; // 如果初始化失败，输出错误信息并退出程序
    }
    
    // 配置 GLFW：设置 OpenGL 的主要版本和次要版本为 3.3，启用核心模式 (Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 次版本号 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui + GLFW Example", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // 如果窗口创建失败，释放 GLFW 资源并退出程序
        return -1;
    }

    // 设置当前窗口上下文为 OpenGL 上下文
    glfwMakeContextCurrent(window);

    // 设置窗口大小变化时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 glad（加载 OpenGL 的函数指针）
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return -1; // 如果加载失败，输出错误信息并退出程序
    }

    // 设置 OpenGL 的初始视口大小，范围为整个窗口
    glViewport(0, 0, 800, 600);

    // 初始化 ImGui 上下文
    IMGUI_CHECKVERSION(); // 检查 ImGui 版本是否匹配
    ImGui::CreateContext(); // 创建 ImGui 上下文
    ImGuiIO& io = ImGui::GetIO(); (void)io; // 获取 ImGui 的输入输出对象（通常用于配置）
    ImGui::StyleColorsDark(); // 设置 ImGui 的样式为深色风格

    // 初始化 ImGui 的 GLFW 和 OpenGL3 后端
    ImGui_ImplGlfw_InitForOpenGL(window, true); // 初始化 GLFW 后端
    ImGui_ImplOpenGL3_Init("#version 330");     // 初始化 OpenGL3 后端，使用 GLSL 版本 330

    // 主循环：持续运行直到窗口关闭
    while (!glfwWindowShouldClose(window)) {
        // 处理用户输入事件（如按键、鼠标事件等）
        glfwPollEvents();

        // 开始新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 创建简单的 GUI 界面
        static float slider_value = 0.0f; // 定义滑块的值，初始值为 0.0
        static int counter = 0; // 定义按钮点击计数器，初始值为 0

        ImGui::Begin("Hello, ImGui!");                // 创建一个窗口，标题为 "Hello, ImGui!"
        ImGui::Text("This is a simple example.");    // 添加一行文本
        if (ImGui::Button("Click Me")) {             // 添加一个按钮，当点击时
            counter++;                               // 计数器自增
        }
        ImGui::Text("Button clicked %d times", counter); // 显示按钮点击的次数
        ImGui::SliderFloat("Slider", &slider_value, 0.0f, 1.0f); // 添加一个滑块，范围是 0.0 到 1.0
        ImGui::End(); // 结束窗口定义

        // 渲染 ImGui 界面
        ImGui::Render(); // 准备渲染数据
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置 OpenGL 的背景颜色（深绿色）
        glClear(GL_COLOR_BUFFER_BIT);         // 清除颜色缓冲区
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // 使用 OpenGL 渲染 ImGui 绘制数据

        // 交换前后缓冲区，显示渲染结果
        glfwSwapBuffers(window);
    }

    // 清理资源
    ImGui_ImplOpenGL3_Shutdown(); // 关闭 ImGui 的 OpenGL3 后端
    ImGui_ImplGlfw_Shutdown();    // 关闭 ImGui 的 GLFW 后端
    ImGui::DestroyContext();      // 销毁 ImGui 上下文
    glfwDestroyWindow(window);    // 销毁 GLFW 窗口
    glfwTerminate();              // 释放 GLFW 资源

    return 0; // 程序正常退出
}

