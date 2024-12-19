#ifndef EASYIMGUI_H
#define EASYIMGUI_H

#include "imgui.h"
#include <GLFW/glfw3.h>
// Struct to store window information
struct WindowInfo {
    const char* name;  // Window name
    ImVec2 pos;        // Position of the window
    ImVec2 size;       // Size of the window
};

// Function to display all window information
void ShowWindowsInfos();

void ArrangeWindowsDynamicGrid(int window_count, ImVec2 base_pos = ImVec2(50, 50), ImVec2 window_size = ImVec2(300, 200), int columns = 3);

// 初始化 GLFW 和 ImGui，并返回一个初始化后的 GLFW 窗口
GLFWwindow* InitGLFWAndImGui(const char* window_title, int width, int height);

// 开始一帧 ImGui 渲染
void BeginImGuiFrame();

// 结束一帧 ImGui 渲染并交换缓冲区
void EndImGuiFrame(GLFWwindow* window);

// 清理 GLFW 和 ImGui
void CleanupGLFWAndImGui(GLFWwindow* window);


#endif // EASYIMGUI_H