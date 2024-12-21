#include "Algorithm.h" // 包含填充算法的头文件
#include "easyimgui.h" // 自定义 EasyImGui 库
#include <GLFW/glfw3.h>
#include <cmath>
#include <imgui.h> // 导入 ImGui 库
#include <vector>

// 定义存储多边形参数的结构体
struct PolygonParams {
    int vertex_count = 4; // 默认顶点数量为4
    std::vector<ImVec2> vertices; // 存储顶点坐标的数组

    // 初始化多边形（正方形）
    PolygonParams() {
        vertices.resize(8); // 支持最多8个顶点
        vertices[0] = ImVec2(150.0f, 100.0f);
        vertices[1] = ImVec2(250.0f, 100.0f);
        vertices[2] = ImVec2(250.0f, 200.0f);
        vertices[3] = ImVec2(150.0f, 200.0f);
    }
};

// 主程序入口
int main() {
    // 初始化 GLFW 和 ImGui，创建一个窗口
    GLFWwindow* window = InitGLFWAndImGui("Polygon Fill with Seed", 800, 600);
    if (!window)
        return -1; // 如果窗口创建失败，则退出程序

    PolygonParams polygonParams; // 多边形参数对象

    ImVec2 seed_point = ImVec2(200.0f, 150.0f); // 初始化种子点位置
    const float seed_radius = 10.0f; // 种子点的半径

    bool show_control_window = true; // 控制面板是否显示
    static int dragged_vertex = -1; // 当前被拖拽的顶点（-1表示没有顶点被拖拽）
    static bool dragging_seed = false; // 当前是否正在拖拽种子点

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 处理用户输入事件
        BeginImGuiFrame(); // 开始新的一帧

        // 1. 绘制多边形窗口
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Always); // 固定窗口大小
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse; // 禁止移动和调整窗口大小
        ImGui::Begin("exp9: Seed Draw Polygon Display", nullptr, window_flags);

        ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 获取绘图列表
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 获取画布的位置
        ImVec2 canvas_size = ImGui::GetWindowSize(); // 获取窗口大小
        canvas_size.y -= 30; // 考虑窗口标题栏的高度
        ImVec2 canvas_end = ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y); // 计算画布右下角坐标

        // 绘制画布背景（白色）
        draw_list->AddRectFilled(canvas_pos, canvas_end, IM_COL32(255, 255, 255, 255));

        // 根据种子点位置填充区域
        DrawFilledRegion(draw_list, canvas_pos, canvas_size, polygonParams.vertices, polygonParams.vertex_count, seed_point, IM_COL32(255, 0, 0, 255));

        // 绘制多边形边界
        for (int i = 0; i < polygonParams.vertex_count; ++i) {
            int next = (i + 1) % polygonParams.vertex_count; // 计算下一个顶点，闭合多边形
            draw_list->AddLine(
                ImVec2(canvas_pos.x + polygonParams.vertices[i].x, canvas_pos.y + polygonParams.vertices[i].y),
                ImVec2(canvas_pos.x + polygonParams.vertices[next].x, canvas_pos.y + polygonParams.vertices[next].y),
                IM_COL32(0, 0, 0, 255), 2.0f // 绘制边界线，黑色
            );
        }

        // 绘制多边形顶点
        const float vertex_radius = 10.0f; // 顶点圆形半径
        for (int i = 0; i < polygonParams.vertex_count; ++i) {
            ImVec2 vertex_screen_pos = ImVec2(canvas_pos.x + polygonParams.vertices[i].x, canvas_pos.y + polygonParams.vertices[i].y); // 顶点的屏幕位置
            ImVec2 mouse_pos = ImGui::GetIO().MousePos; // 获取鼠标位置

            // 计算鼠标与顶点的距离
            float dist = sqrt(pow(mouse_pos.x - vertex_screen_pos.x, 2) + pow(mouse_pos.y - vertex_screen_pos.y, 2));
            if (dist <= vertex_radius + 2.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && dragged_vertex == -1) {
                dragged_vertex = i; // 如果点击到当前顶点，开始拖拽
            }

            // 如果正在拖拽顶点，更新顶点位置
            if (dragged_vertex == i && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                polygonParams.vertices[i].x = std::clamp(mouse_pos.x - canvas_pos.x, 0.0f, canvas_size.x);
                polygonParams.vertices[i].y = std::clamp(mouse_pos.y - canvas_pos.y, 0.0f, canvas_size.y);
            } else if (dragged_vertex == i && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                dragged_vertex = -1; // 停止拖拽
            }

            // 绘制顶点
            draw_list->AddCircleFilled(vertex_screen_pos, vertex_radius, IM_COL32(0, 255, 0, 255)); // 顶点为绿色
            draw_list->AddCircle(vertex_screen_pos, vertex_radius, IM_COL32(0, 0, 0, 255)); // 黑色边框
        }

        // 绘制种子点
        ImVec2 seed_screen_pos = ImVec2(canvas_pos.x + seed_point.x, canvas_pos.y + seed_point.y); // 种子点的屏幕位置
        ImVec2 mouse_pos = ImGui::GetIO().MousePos; // 获取鼠标位置

        // 计算鼠标与种子点的距离
        float seed_dist = sqrt(pow(mouse_pos.x - seed_screen_pos.x, 2) + pow(mouse_pos.y - seed_screen_pos.y, 2));
        if (seed_dist <= seed_radius + 2.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !dragging_seed) {
            dragging_seed = true; // 开始拖拽种子点
        }

        // 如果正在拖拽种子点，更新种子点位置
        if (dragging_seed && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            seed_point.x = std::clamp(mouse_pos.x - canvas_pos.x, 0.0f, canvas_size.x);
            seed_point.y = std::clamp(mouse_pos.y - canvas_pos.y, 0.0f, canvas_size.y);
        } else if (dragging_seed && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            dragging_seed = false; // 停止拖拽
        }

        // 绘制种子点
        draw_list->AddCircleFilled(seed_screen_pos, seed_radius, IM_COL32(0, 0, 255, 255)); // 种子点为蓝色
        draw_list->AddCircle(seed_screen_pos, seed_radius, IM_COL32(0, 0, 0, 255)); // 黑色边框

        ImGui::End(); // 结束绘制窗口

        // 2. 控制面板窗口：实时显示和调整顶点数量及顶点坐标
        if (show_control_window) {
            ImGui::Begin("Control Panel", &show_control_window);

            // 顶点数量选择
            ImGui::Text("Polygon Vertex Count:");
            const char* items[] = { "3", "4", "5", "6", "7", "8" };
            static int current_item = polygonParams.vertex_count - 3; // 当前选中的顶点数量索引
            if (ImGui::Combo("##VertexCount", &current_item, items, IM_ARRAYSIZE(items))) {
                polygonParams.vertex_count = current_item + 3; // 更新顶点数量
                polygonParams.vertices.resize(polygonParams.vertex_count); // 根据新顶点数量调整数组大小
            }

            // 顶点坐标显示
            ImGui::Text("Vertex Coordinates:");
            for (int i = 0; i < polygonParams.vertex_count; ++i) {
                ImGui::Text("Vertex %d: (%.1f, %.1f)", i + 1, polygonParams.vertices[i].x, polygonParams.vertices[i].y);
            }

            ImGui::End(); // 结束控制面板窗口
        }

        EndImGuiFrame(window); // 结束帧并交换缓冲区
    }

    CleanupGLFWAndImGui(window); // 清理资源并关闭窗口
    return 0;
}