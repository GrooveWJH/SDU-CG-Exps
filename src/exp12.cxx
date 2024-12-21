// main.cpp 
// Failed
#include "Algorithm.h"
#include "easyimgui.h"
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cmath>
#include <vector>


// 函数用于绘制多边形
void DrawPolygon(ImDrawList* draw_list, const ImVec2& canvas_pos, const std::vector<ImVec2>& polygon, ImU32 color, float thickness = 1.0f)
{
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t next = (i + 1) % polygon.size();
        draw_list->AddLine(
            ImVec2(canvas_pos.x + polygon[i].x, canvas_pos.y + polygon[i].y),
            ImVec2(canvas_pos.x + polygon[next].x, canvas_pos.y + polygon[next].y),
            color, thickness);
    }
}

// 函数用于绘制拖拽点
void DrawDragPoint(ImDrawList* draw_list, const ImVec2& point, const ImVec2& canvas_pos, ImU32 color)
{
    ImVec2 screen_pos = ImVec2(canvas_pos.x + point.x, canvas_pos.y + point.y);
    draw_list->AddCircleFilled(screen_pos, 8.0f, color); // 圆形填充
    draw_list->AddCircle(screen_pos, 8.0f, IM_COL32(0, 0, 0, 255), 16, 2.0f); // 圆形边框
}


int main()
{
    // 初始化 GLFW 和 ImGui
    GLFWwindow* window = InitGLFWAndImGui("Weiler-Atherton Polygon Clipping Demo", 800, 600);
    if (!window)
        return -1;

    // 初始化主多边形和裁剪窗口（相对于画布）
    std::vector<ImVec2> subjectPolygon = {
        ImVec2(200, 100), ImVec2(400, 200), ImVec2(400, 500), ImVec2(300, 400), ImVec2(100, 300)
    };
    ClipWindow clipWindow = {100.0f, 150.0f, 350.0f, 400.0f}; // 初始裁剪窗口

    // 拖拽状态变量
    int draggedVertex = -1; // 被拖拽的多边形顶点索引，-1 表示未拖拽
    bool draggingTopLeft = false, draggingBottomRight = false; // 是否正在拖拽裁剪窗口的两个对角点

    // 颜色设置
    ImVec4 subjectColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // 原始多边形颜色
    ImVec4 clipColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);    // 裁剪窗口颜色
    ImVec4 overlapColor = ImVec4(0.5f, 0.0f, 1.0f, 0.5f); // 重叠区域颜色

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        BeginImGuiFrame();

        // 控制面板
        ImGui::Begin("Control Panel");

        ImGui::ColorEdit3("Polygon Color", (float*)&subjectColor);
        ImGui::ColorEdit3("Overlap Color", (float*)&overlapColor);
        ImGui::ColorEdit3("Clip Window Color", (float*)&clipColor);

        static int vertexCount = subjectPolygon.size();
        if (ImGui::SliderInt("Vertex Count", &vertexCount, 3, 8)) {
            if (vertexCount < subjectPolygon.size()) {
                subjectPolygon.resize(vertexCount);
            }
            else {
                // 添加新的顶点，保持最后一个顶点的位置
                while (subjectPolygon.size() < vertexCount) {
                    subjectPolygon.emplace_back(subjectPolygon.back());
                }
            }
        }

        ImGui::End();

        // 主裁剪窗口
        ImGui::Begin("Polygon Clipping", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

        // 定义画布大小
        ImVec2 canvas_size = ImVec2(600, 500);
        // 绘制画布背景
        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

        // 定义裁剪多边形（矩形）
        std::vector<ImVec2> clipPolygon = {
            ImVec2(clipWindow.x0, clipWindow.y0),
            ImVec2(clipWindow.x1, clipWindow.y0),
            ImVec2(clipWindow.x1, clipWindow.y1),
            ImVec2(clipWindow.x0, clipWindow.y1)
        };

        // 绘制裁剪窗口矩形
        draw_list->AddPolyline(
            clipPolygon.data(),
            clipPolygon.size(),
            IM_COL32(0, 255, 0, 255),
            true,
            2.0f
        );

        // 绘制裁剪窗口的拖拽点
        DrawDragPoint(draw_list, ImVec2(clipWindow.x0, clipWindow.y0), canvas_pos, IM_COL32(0, 0, 255, 255));
        DrawDragPoint(draw_list, ImVec2(clipWindow.x1, clipWindow.y1), canvas_pos, IM_COL32(0, 0, 255, 255));

        // 绘制主多边形
        DrawPolygon(draw_list, canvas_pos, subjectPolygon, ImColor(subjectColor), 2.0f);

        // 绘制多边形的拖拽点
        for (const auto& vertex : subjectPolygon) {
            DrawDragPoint(draw_list, vertex, canvas_pos, IM_COL32(0, 255, 0, 255));
        }

        // 进行裁剪
        std::vector<std::vector<ImVec2>> clippedPolygons = WeilerAthertonPolygonClip(subjectPolygon, clipPolygon);

        // 绘制裁剪后的多边形
        for (const auto& poly : clippedPolygons) {
            if (poly.size() < 3)
                continue; // 忽略非法多边形

            // 填充多边形
            draw_list->AddConvexPolyFilled(
                poly.data(),
                poly.size(),
                ImColor(overlapColor)
            );

            // 绘制多边形边界
            draw_list->AddPolyline(
                poly.data(),
                poly.size(),
                IM_COL32(255, 0, 0, 255),
                true,
                2.0f
            );
        }

        ImGui::End();

        // 显示窗口信息
        ShowWindowsInfos();

        // 处理拖拽事件
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        ImVec2 relativeMousePos = ImVec2(mousePos.x - canvas_pos.x, mousePos.y - canvas_pos.y);

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            // 检查是否点击了裁剪窗口的拖拽点
            if (std::hypot(relativeMousePos.x - clipWindow.x0, relativeMousePos.y - clipWindow.y0) < 8.0f) {
                draggingTopLeft = true;
            }
            else if (std::hypot(relativeMousePos.x - clipWindow.x1, relativeMousePos.y - clipWindow.y1) < 8.0f) {
                draggingBottomRight = true;
            }
            else {
                // 检查是否点击了主多边形的顶点
                for (size_t i = 0; i < subjectPolygon.size(); ++i) {
                    if (std::hypot(relativeMousePos.x - subjectPolygon[i].x, relativeMousePos.y - subjectPolygon[i].y) < 8.0f) {
                        draggedVertex = i;
                        break;
                    }
                }
            }
        }

        // 更新裁剪窗口和多边形顶点的位置
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            if (draggingTopLeft) {
                // 限制裁剪窗口的最小尺寸
                clipWindow.x0 = std::min(relativeMousePos.x, clipWindow.x1 - 10.0f);
                clipWindow.y0 = std::min(relativeMousePos.y, clipWindow.y1 - 10.0f);
            }
            else if (draggingBottomRight) {
                clipWindow.x1 = std::max(relativeMousePos.x, clipWindow.x0 + 10.0f);
                clipWindow.y1 = std::max(relativeMousePos.y, clipWindow.y0 + 10.0f);
            }
            else if (draggedVertex != -1) {
                subjectPolygon[draggedVertex].x = relativeMousePos.x;
                subjectPolygon[draggedVertex].y = relativeMousePos.y;
            }
        }
        else {
            draggingTopLeft = draggingBottomRight = false;
            draggedVertex = -1;
        }

        EndImGuiFrame(window);
    }

    CleanupGLFWAndImGui(window);
    return 0;
}