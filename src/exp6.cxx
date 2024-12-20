#include "Algorithm.h" // 包含绘制算法
#include "easyimgui.h"
#include "imgui.h"
#include <iostream>

struct EllipseParams {
    float centerX = 200.0f; // 椭圆中心 x 坐标
    float centerY = 200.0f; // 椭圆中心 y 坐标
    int a = 100; // 长轴长度
    int b = 50; // 短轴长度
    ImVec4 color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 默认颜色为蓝色
};

int main()
{
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("exp6: Midpoint Ellipse Drawing");
    if (!window)
        return -1;

    EllipseParams ellipseParams;

    bool show_control_window = true;
    bool show_draw_window = true;

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 处理事件
        BeginImGuiFrame(); // 开始新的一帧

        if (show_draw_window) {
            ImGui::Begin("Ellipse Drawing Window", &show_draw_window,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            //  ImGui::SetWindowSize(ImVec2(400,400));
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            ImVec2 center = ImVec2(canvas_pos.x + ellipseParams.centerX, canvas_pos.y + ellipseParams.centerY);

            // 使用中点画椭圆算法绘制椭圆
            DrawEllipseMidpoint(draw_list, center, ellipseParams.a, ellipseParams.b, ImColor(ellipseParams.color));

            if (show_control_window) {
                ImGui::Begin("Parameter Settings", &show_control_window);
                ImGui::SetWindowSize(ImVec2(350, 250));

                // 调整椭圆参数
                ImGui::Text("Ellipse Color:");
                ImGui::ColorEdit3("##EllipseColor", (float*)&ellipseParams.color);

                ImGui::SliderFloat("Center X:", &ellipseParams.centerX, 0.0f, canvas_size.x);
                ImGui::SliderFloat("Center Y:", &ellipseParams.centerY, 0.0f, canvas_size.y);
                ImGui::SliderInt("Semi-major Axis (a):", &ellipseParams.a, 1, 300, "a: %d");
                ImGui::SliderInt("Semi-minor Axis (b):", &ellipseParams.b, 1, 300, "b: %d");

                if (ImGui::Button("Confirm")) {
                    std::cout << "Parameters confirmed: "
                              << "Center(" << ellipseParams.centerX << ", " << ellipseParams.centerY << "), "
                              << "a: " << ellipseParams.a << ", b: " << ellipseParams.b << "\n";
                }
                ImGui::End();
            }

            ImGui::End();
        }
        ShowWindowsInfos();
        EndImGuiFrame(window); // 结束当前帧并交换缓冲区
    }

    CleanupGLFWAndImGui(window); // 清理资源
    return 0;
}