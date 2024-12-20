#include <glad.h>
#include "easyimgui.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Global vector to store window info
std::vector<WindowInfo> windowInfos;

void ShowWindowsInfos()
{
    // Begin a new window for displaying window info
    ImGui::Begin("Windows Info");

    // Clear the previous windowInfos
    windowInfos.clear();

    // Iterate through all ImGui windows
    ImGuiContext& g = *ImGui::GetCurrentContext();
    for (ImGuiWindow* window : g.Windows) {
        // Collect the window information
        WindowInfo info;
        info.name = window->Name;
        info.pos = window->Pos;
        info.size = window->Size;
        windowInfos.push_back(info);
    }

    // Display the collected information
    for (const auto& winInfo : windowInfos) {
        ImGui::Text("Window Name: %s", winInfo.name);
        ImGui::Text("Position: (%.1f, %.1f)", winInfo.pos.x, winInfo.pos.y);
        ImGui::Text("Size: (%.1f, %.1f)", winInfo.size.x, winInfo.size.y);
        ImGui::Separator();
    }

    ImGui::End();
}
// easyimgui.cxx
void ArrangeWindowsDynamicGrid(int window_count, ImVec2 base_pos,
    ImVec2 window_size, int columns)
{
    for (int i = 0; i < window_count; ++i) {
        ImVec2 current_pos = base_pos;
        current_pos.x += (i % columns) * (window_size.x + 10);
        current_pos.y += (i / (float)columns) * (window_size.y + 10);

        ImGui::SetNextWindowPos(current_pos);

        ImGui::Begin(("Window " + std::to_string(i + 1)).c_str());
        ImGui::Text("This is Window %d", i + 1);
        ImGui::End();
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GLFWwindow* InitGLFWAndImGui(const char* window_title, int width, int height)
{
    // 设置 GLFW 错误回调
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // 设置 OpenGL 版本和核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建 GLFW 窗口
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    // 初始化 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();

    // 初始化 ImGui 平台和渲染器绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return window;
}

void BeginImGuiFrame()
{
    // 开始一帧 ImGui 渲染
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EndImGuiFrame(GLFWwindow* window)
{
    // 渲染并交换缓冲区
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void CleanupGLFWAndImGui(GLFWwindow* window)
{
    // 清理 ImGui 和 GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}