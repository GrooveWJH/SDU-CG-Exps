#include "Algorithm.h" // 导入算法相关头文件
#include "easyimgui.h" // 导入自定义的 EasyImGui 库
#include <cstdio>
#include <imgui.h> // 导入 ImGui 库

#include <vector> // 导入 STL 的向量库
// 存储多边形参数的结构体
struct PolygonParams {
    ImVec4 color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 默认多边形颜色为蓝色
    int vertexCount = 6; // 默认顶点数为6
    std::vector<float> x; // 存储顶点的 x 坐标
    std::vector<float> y; // 存储顶点的 y 坐标

    // 构造函数：初始化多边形，最多支持8个顶点
    PolygonParams(int maxVertices = 8)
    {
        x.resize(maxVertices, 0.0f); // x 坐标数组初始化，大小为 maxVertices，默认值为 0.0f
        y.resize(maxVertices, 0.0f); // y 坐标数组初始化，大小为 maxVertices，默认值为 0.0f
    }
};

int main()
{
    // 初始化 GLFW 和 ImGui，创建一个窗口
    GLFWwindow* window = InitGLFWAndImGui("exp8: Polygon Ordered Edge Table", 2000, 1000);
    if (!window)
        return -1; // 如果窗口创建失败，则退出程序

    PolygonParams polygonParams; // 创建多边形参数对象

    // 初始化默认的多边形顶点
    polygonParams.x = { 394.0f, 322.0f, 178.0f, 106.0f, 178.0f, 322.0f };
    polygonParams.y = { 250.0f, 323.0f, 323.0f, 250.0f, 177.0f, 177.0f };
    
    bool show_control_window = true; // 控制面板是否显示

    // 主循环：处理窗口事件和渲染
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 处理用户输入事件
        BeginImGuiFrame(); // 开始新的一帧

        // 绘制多边形的窗口
        ImGui::Begin("Drawing Window");
        ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 获取绘图列表
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 获取画布的位置
        ImU32 color = ImColor(polygonParams.color); // 获取多边形的颜色

        ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // 获取画布的大小
        // 使用有序边表算法绘制填充多边形
        DrawPolygonWithEdgeFlagMethod(draw_list, canvas_pos, polygonParams.x, polygonParams.y, polygonParams.vertexCount, color);

        ImGui::End(); // 结束绘制窗口

        // 控制面板窗口：允许用户调整多边形参数
        if (show_control_window) {
            ImGui::Begin("Parameter Settings", &show_control_window); // 创建并显示控制面板
            ImGui::SetWindowSize(ImVec2(500, 500));
            // 多边形颜色调整
            ImGui::Text("Polygon Color:"); // 显示颜色标签
            ImGui::ColorEdit3("Change Color", (float*)&polygonParams.color); // 编辑颜色

            // 顶点数调整
            ImGui::Text("Number of Vertices:"); // 显示顶点数标签
            ImGui::SliderInt("##Vertex Count", &polygonParams.vertexCount, 3, 8); // 调整顶点数，最少3个，最多8个

            // 调整每个顶点的坐标
            ImGui::Text("Polygon Vertex Coordinates:"); // 显示顶点坐标标签

            for (int i = 0; i < polygonParams.vertexCount; ++i) {
                ImGui::PushID(i); // 确保每个控件有唯一的 ID
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                // 设置第一个控件（x坐标）宽度为窗口的一半
                // 调整 x 坐标
                ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &polygonParams.x[i], 0.0f, canvas_size.x, "%0.1f");
                ImGui::SameLine(); // 同行显示 y 坐标的调整
                // 调整 y 坐标
                ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &polygonParams.y[i], 0.0f, canvas_size.y);
                ImGui::PopItemWidth();
                ImGui::PopID(); // 恢复控件 ID
            }

            // 确认按钮：按下后打印当前的顶点坐标
            if (ImGui::Button("Confirm")) {
                for (int i = 0; i < polygonParams.vertexCount; ++i) {
                    printf("Vertex %d: (%.1f, %.1f)\n", i, polygonParams.x[i], polygonParams.y[i]); // 打印顶点坐标
                }
            }
            ImGui::End(); // 结束控制面板窗口
        }

        EndImGuiFrame(window); // 结束当前帧并交换缓冲区，渲染结果
    }

    CleanupGLFWAndImGui(window); // 清理资源，关闭窗口
    return 0;
}