#include "Algorithm.h" // 引入算法文件
#include "easyimgui.h" // 引入 EasyImGui 库
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <cmath>

struct Line {
    float x0 = 120.0f, y0 = 80.0f; // 起点
    float x1 = 180.0f, y1 = 230.0f; // 终点
};

int main()
{
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("Line Clipping Demo");
    if (!window)
        return -1;

    ClipWindow clipWindow; // 裁剪窗口
    Line line; // 直线

    ImVec4 beforeColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 裁剪前直线颜色
    ImVec4 afterColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 裁剪后直线颜色

    const float point_radius = 6.0f; // 拖拽点的半径
    bool show_control_window = true;
    bool dragging_point = false;    // 标记是否在拖拽
    int dragged_point = -1;         // 被拖拽的点的索引（0：窗口左上，1：窗口右下，2：线段起点，3：线段终点）

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        BeginImGuiFrame();

        // 绘制裁剪窗口和直线
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        ImGui::Begin("Line Clipping", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 获取画布位置
        ImVec2 canvas_size = ImGui::GetWindowSize();     // 获取画布大小

        // 绘制画布背景
        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

        // 绘制裁剪窗口
        draw_list->AddRect(ImVec2(canvas_pos.x + clipWindow.x0, canvas_pos.y + clipWindow.y0),
            ImVec2(canvas_pos.x + clipWindow.x1, canvas_pos.y + clipWindow.y1),
            IM_COL32(0, 0, 255, 255));

        // 绘制裁剪前的直线
        draw_list->AddLine(ImVec2(canvas_pos.x + line.x0, canvas_pos.y + line.y0),
            ImVec2(canvas_pos.x + line.x1, canvas_pos.y + line.y1),
            ImColor(beforeColor), 2.0f);

        // 绘制拖拽点（裁剪窗口的两个对角点和线段的两端）
        ImVec2 drag_points[4] = {
            ImVec2(canvas_pos.x + clipWindow.x0, canvas_pos.y + clipWindow.y0),
            ImVec2(canvas_pos.x + clipWindow.x1, canvas_pos.y + clipWindow.y1),
            ImVec2(canvas_pos.x + line.x0, canvas_pos.y + line.y0),
            ImVec2(canvas_pos.x + line.x1, canvas_pos.y + line.y1),
        };

        for (int i = 0; i < 4; ++i) {
            draw_list->AddCircleFilled(drag_points[i], point_radius, IM_COL32(0, 255, 0, 255)); // 绿色点
            draw_list->AddCircle(drag_points[i], point_radius, IM_COL32(0, 0, 0, 255));         // 黑色边框
        }

        // 检测拖拽
        ImVec2 mouse_pos = ImGui::GetIO().MousePos; // 当前鼠标位置
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            for (int i = 0; i < 4; ++i) {
                float dist = std::sqrt(std::pow(mouse_pos.x - drag_points[i].x, 2) + std::pow(mouse_pos.y - drag_points[i].y, 2));
                if (dist <= point_radius) {
                    dragging_point = true;
                    dragged_point = i;
                    break;
                }
            }
        }

        // 拖拽点更新位置
        if (dragging_point && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImVec2(mouse_pos.x - canvas_pos.x, mouse_pos.y-canvas_pos.y);

            switch (dragged_point) {
            case 0: // 裁剪窗口左上角
                clipWindow.x0 = delta.x;
                clipWindow.y0 = delta.y;
                break;
            case 1: // 裁剪窗口右下角
                clipWindow.x1 = delta.x;
                clipWindow.y1 = delta.y;
                break;
            case 2: // 线段起点
                line.x0 = delta.x;
                line.y0 = delta.y;
                break;
            case 3: // 线段终点
                line.x1 = delta.x;
                line.y1 = delta.y;
                break;
            }
        } else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            dragging_point = false;
            dragged_point = -1;
        }

        // 裁剪直线
        float x0 = line.x0, y0 = line.y0, x1 = line.x1, y1 = line.y1;
        if (CohenSutherlandLineClip(x0, y0, x1, y1, clipWindow.x0, clipWindow.y0,
                clipWindow.x1, clipWindow.y1)) {
            // 绘制裁剪后的直线
            draw_list->AddLine(ImVec2(canvas_pos.x + x0, canvas_pos.y + y0),
                ImVec2(canvas_pos.x + x1, canvas_pos.y + y1),
                ImColor(afterColor), 2.0f);
        }

        ImGui::End();

        // 控制面板
        if (show_control_window) {
            ImGui::Begin("Control Panel", &show_control_window);

            ImGui::Text("Clipping Colors:");
            ImGui::ColorEdit3("Before Clipping", (float*)&beforeColor);
            ImGui::ColorEdit3("After Clipping", (float*)&afterColor);

            ImGui::Text("Clipping Window:");
            ImGui::InputFloat("Top Left x", &clipWindow.x0);
            ImGui::InputFloat("Top Left y", &clipWindow.y0);
            ImGui::InputFloat("Bottom Right x", &clipWindow.x1);
            ImGui::InputFloat("Bottom Right y", &clipWindow.y1);

            ImGui::Text("Line Coordinates:");
            ImGui::InputFloat("Start x0", &line.x0);
            ImGui::InputFloat("Start y0", &line.y0);
            ImGui::InputFloat("End x1", &line.x1);
            ImGui::InputFloat("End y1", &line.y1);

            ImGui::End();
        }

        EndImGuiFrame(window);
    }

    CleanupGLFWAndImGui(window);
    return 0;
}