#include "easyimgui.h"
#include "easyimgui.h"
#include <iostream>
#include "imgui.h"

struct LineParams {
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 300.0f;
    float y1 = 200.0f;
    ImVec4 color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Default to blue
};

int main() {
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("Line Drawing with ImGui", 1400, 900);
    if (!window) return -1;

    LineParams lineParams;

    bool show_control_window = true;
    bool show_draw_window = true;
    bool show_windows_infos = true;

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 处理事件
        BeginImGuiFrame(); // 开始新的一帧

        // 绘制直线窗口
        if (show_draw_window) {
            ImGui::Begin("Line Drawing Window", &show_draw_window,
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 窗口内容区左上角坐标
            ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // 内容区大小

            ImVec2 p1 = ImVec2(canvas_pos.x + lineParams.x0, canvas_pos.y + lineParams.y0);
            ImVec2 p2 = ImVec2(canvas_pos.x + lineParams.x1, canvas_pos.y + lineParams.y1);
            draw_list->AddLine(p1, p2, ImColor(lineParams.color), 2.0f);

            if (show_control_window) {
                ImGui::Begin("Parameter Settings", &show_control_window);
                ImGui::SetWindowSize(ImVec2(350, 200));
                ImGui::Text("Line Color:");
                ImGui::ColorEdit3("##lineColor", (float*)&lineParams.color);

                ImGui::SliderFloat("Start Point x0:", &lineParams.x0, 0.0f, canvas_size.x);
                ImGui::SliderFloat("Start Point y0:", &lineParams.y0, 0.0f, canvas_size.y);
                ImGui::SliderFloat("End Point x1:", &lineParams.x1, 0.0f, canvas_size.x);
                ImGui::SliderFloat("End Point y1:", &lineParams.y1, 0.0f, canvas_size.y);

                if (ImGui::Button("Confirm")) {
                    std::cout << "Parameters confirmed: "
                              << "Start(" << lineParams.x0 << ", " << lineParams.y0 << "), "
                              << "End(" << lineParams.x1 << ", " << lineParams.y1 << ")\n";
                }
                ImGui::End();
            }

            if (show_windows_infos) {
                ShowWindowsInfos();
            }

            ImGui::End();
        }

        EndImGuiFrame(window); // 结束当前帧并交换缓冲区
    }

    CleanupGLFWAndImGui(window); // 清理资源
    return 0;
}