#include "easyimgui.h"
#include "Algorithm.h" // 包含绘制算法
#include <iostream>
#include "imgui.h"

struct CircleParams {
    float centerX = 100.0f; // 圆心 x
    float centerY = 100.0f; // 圆心 y
    int radius = 50;        // 圆半径
    ImVec4 color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 默认颜色为蓝色
};

int main() {
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("Midpoint Circle Drawing", 800, 600);
    if (!window) return -1;

    CircleParams circleParams;

    bool show_control_window = true;
    bool show_draw_window = true;

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 处理事件
        BeginImGuiFrame(); // 开始新的一帧

        if (show_draw_window) {
            ImGui::Begin("Circle Drawing Window", &show_draw_window,
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            ImVec2 center = ImVec2(canvas_pos.x + circleParams.centerX, canvas_pos.y + circleParams.centerY);

            // 使用中点画圆算法绘制圆
            DrawCircleMidpoint(draw_list, center, circleParams.radius, ImColor(circleParams.color));

            if (show_control_window) {
                ImGui::Begin("Parameter Settings", &show_control_window);
                ImGui::SetWindowSize(ImVec2(350, 200));

                // 调整圆参数
                ImGui::Text("Circle Color:");
                ImGui::ColorEdit3("##CircleColor", (float*)&circleParams.color);

                ImGui::InputFloat("Center X:", &circleParams.centerX);
                ImGui::InputFloat("Center Y:", &circleParams.centerY);
                ImGui::SliderInt("Radius:", &circleParams.radius, 1, 300, "Radius: %d");

                if (ImGui::Button("Confirm")) {
                    std::cout << "Parameters confirmed: "
                              << "Center(" << circleParams.centerX << ", " << circleParams.centerY << "), "
                              << "Radius: " << circleParams.radius << "\n";
                }
                ImGui::End();
            }

            ImGui::End();
        }

        EndImGuiFrame(window); // 结束当前帧并交换缓冲区
    }

    CleanupGLFWAndImGui(window); // 清理资源
    return 0;
}