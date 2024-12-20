#include "Algorithm.h" // 引入算法文件
#include "easyimgui.h" // 引入 EasyImGui 库
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vector>

std::vector<ImVec2> polygon = {
    ImVec2(120, 100), ImVec2(180, 80), ImVec2(220, 140), ImVec2(90, 180), ImVec2(100, 150)
};

ClipWindow clipWindow = {50, 50, 300, 300}; // 裁剪窗口初始值

const float vertexRadius = 6.0f; // 顶点圆的半径

int main() {
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("exp11: Clip Polygon");
    if (!window)
        return -1;

    ImVec4 polygonColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 原始多边形颜色
    ImVec4 clippedPolygonColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 裁剪后多边形颜色

    int draggedVertex = -1; // 当前被拖拽的顶点索引

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        BeginImGuiFrame();

        // 绘制裁剪窗口和多边形
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        ImGui::Begin("Polygon Clipping", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 获取画布位置

        // 绘制画布背景
        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + 600, canvas_pos.y + 400), IM_COL32(255, 255, 255, 255));

        // 绘制裁剪窗口
        draw_list->AddRect(ImVec2(canvas_pos.x + clipWindow.x0, canvas_pos.y + clipWindow.y0),
            ImVec2(canvas_pos.x + clipWindow.x1, canvas_pos.y + clipWindow.y1),
            IM_COL32(0, 255, 0, 255));

        // 检测和处理拖拽多边形顶点
        ImVec2 mousePos = ImGui::GetIO().MousePos; // 当前鼠标位置
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            for (size_t i = 0; i < polygon.size(); ++i) {
                float dist = std::sqrt(std::pow(mousePos.x - (canvas_pos.x + polygon[i].x), 2) +
                                       std::pow(mousePos.y - (canvas_pos.y + polygon[i].y), 2));
                if (dist <= vertexRadius) {
                    draggedVertex = i; // 设置为当前拖拽的顶点
                    break;
                }
            }
        }

        // 更新拖拽顶点的位置
        if (draggedVertex != -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            polygon[draggedVertex].x = mousePos.x - canvas_pos.x;
            polygon[draggedVertex].y = mousePos.y - canvas_pos.y;
        } else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            draggedVertex = -1; // 停止拖拽
        }

        // 绘制原始多边形
        DrawPolygon(draw_list, canvas_pos, polygon, ImColor(polygonColor));

        // 绘制多边形顶点
        for (const auto& vertex : polygon) {
            ImVec2 vertexPos = ImVec2(canvas_pos.x + vertex.x, canvas_pos.y + vertex.y);
            draw_list->AddCircleFilled(vertexPos, vertexRadius, IM_COL32(0, 255, 0, 255)); // 绿色顶点
            draw_list->AddCircle(vertexPos, vertexRadius, IM_COL32(0, 0, 0, 255)); // 黑色边框
        }

        // 裁剪多边形
        std::vector<ImVec2> clippedPolygon = SutherlandHodgmanPolygonClip(polygon, clipWindow);

        // 绘制裁剪后的多边形
        DrawPolygon(draw_list, canvas_pos, clippedPolygon, ImColor(clippedPolygonColor));

        ImGui::End();

        // 控制面板
        ImGui::Begin("Control Panel");

        ImGui::ColorEdit3("Polygon Color", (float*)&polygonColor);
        ImGui::ColorEdit3("Clipped Polygon Color", (float*)&clippedPolygonColor);

        ImGui::Text("Clipping Window:");
        ImGui::SliderFloat("Top Left x", &clipWindow.x0, 0.0f, 600.0f);
        ImGui::SliderFloat("Top Left y", &clipWindow.y0, 0.0f, 400.0f);
        ImGui::SliderFloat("Bottom Right x", &clipWindow.x1, 0.0f, 600.0f);
        ImGui::SliderFloat("Bottom Right y", &clipWindow.y1, 0.0f, 400.0f);

        ImGui::Text("Polygon Vertices:");
        for (size_t i = 0; i < polygon.size(); ++i) {
            ImGui::SliderFloat(("Vertex " + std::to_string(i) + " x").c_str(), &polygon[i].x, 0.0f, 600.0f);
            ImGui::SliderFloat(("Vertex " + std::to_string(i) + " y").c_str(), &polygon[i].y, 0.0f, 400.0f);
        }

        ImGui::End();

        EndImGuiFrame(window);
    }

    CleanupGLFWAndImGui(window);
    return 0;
}