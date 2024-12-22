#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "easyimgui.h"

// ------------------------------ Bézier 相关函数 ------------------------------ //

// 计算三次贝塞尔曲线某个 t 参数（0 ~ 1）下的点
// controlPoints.size() = 4，即有 4 个控制点：P0, P1, P2, P3
glm::vec3 evaluateCubicBezier(const std::vector<glm::vec3>& controlPoints, float t)
{
    // 三次 Bézier 基础公式：
    // B(t) = (1 - t)^3 * P0 + 3(1 - t)^2 * t * P1 + 3(1 - t) * t^2 * P2 + t^3 * P3
    float u = 1.0f - t;
    float u2 = u * u;
    float u3 = u2 * u;
    float t2 = t * t;
    float t3 = t2 * t;

    glm::vec3 p =
        u3 * controlPoints[0] +
        3.0f * u2 * t * controlPoints[1] +
        3.0f * u * t2 * controlPoints[2] +
        t3 * controlPoints[3];
    return p;
}

// 生成 Bézier 曲线采样点（用于绘制）
std::vector<glm::vec3> generateBezierCurveVertices(const std::vector<glm::vec3>& controlPoints, int segments)
{
    std::vector<glm::vec3> curve;
    curve.reserve(segments + 1);

    for (int i = 0; i <= segments; i++) {
        float t = static_cast<float>(i) / static_cast<float>(segments);
        curve.push_back(evaluateCubicBezier(controlPoints, t));
    }
    return curve;
}

// ------------------------------ OpenGL 辅助函数 ------------------------------ //

static void checkShaderCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n"
                      << "-------------------------------------------------------" << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n"
                      << "-------------------------------------------------------" << std::endl;
        }
    }
}

// ------------------------------ 顶点和片段着色器 ------------------------------ //

const char* lineVertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)glsl";

const char* lineFragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor;

void main()
{
    FragColor = vec4(uColor, 1.0);
}
)glsl";

// ------------------------------ 主程序 ------------------------------ //

int main()
{
    // 使用 easyimgui 初始化
    GLFWwindow* window = InitGLFWAndImGui("Bézier Curve Interactive Demo", 1280, 720);
    if (!window) {
        return -1;
    }

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    // 获取帧缓冲大小并设置视口
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    // 注册回调，保持视口跟随窗口大小变化
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
    });

    // 启用混合以显示透明颜色
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 创建并编译着色器
    // 顶点着色器
    unsigned int lineVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lineVertexShader, 1, &lineVertexShaderSource, NULL);
    glCompileShader(lineVertexShader);
    checkShaderCompileErrors(lineVertexShader, "VERTEX");

    // 片段着色器
    unsigned int lineFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lineFragmentShader, 1, &lineFragmentShaderSource, NULL);
    glCompileShader(lineFragmentShader);
    checkShaderCompileErrors(lineFragmentShader, "FRAGMENT");

    // 着色器程序
    unsigned int lineShaderProgram = glCreateProgram();
    glAttachShader(lineShaderProgram, lineVertexShader);
    glAttachShader(lineShaderProgram, lineFragmentShader);
    glLinkProgram(lineShaderProgram);
    checkShaderCompileErrors(lineShaderProgram, "PROGRAM");

    // 删除着色器对象
    glDeleteShader(lineVertexShader);
    glDeleteShader(lineFragmentShader);

    // 获取着色器中 uniform 位置
    int uMVPLoc = glGetUniformLocation(lineShaderProgram, "uMVP");
    int uColorLoc = glGetUniformLocation(lineShaderProgram, "uColor");

    // 初始化控制点
    std::vector<glm::vec3> controlPoints = {
        glm::vec3(-0.8f, -0.4f, 0.0f),
        glm::vec3(-0.2f,  0.6f,  0.0f),
        glm::vec3( 0.3f,  0.6f,  0.0f),
        glm::vec3( 0.8f, -0.4f,  0.0f)
    };

    // 设置贝塞尔曲线窗口的固定位置和大小
    const ImVec2 bezierWindowPos(50, 50);
    const ImVec2 bezierWindowSize(800, 600);

    bool isDragging = false;
    int selectedPoint = -1;
    float dragThreshold = 10.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        BeginImGuiFrame();

        // 控制面板窗口
        ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Drag the control points directly in the Bezier Curve window.");
        if (ImGui::Button("Reset Points")) {
            controlPoints = {
                glm::vec3(-0.8f, -0.4f, 0.0f),
                glm::vec3(-0.2f,  0.6f,  0.0f),
                glm::vec3( 0.3f,  0.6f,  0.0f),
                glm::vec3( 0.8f, -0.4f,  0.0f)
            };
        }
        ImGui::End();

        // 贝塞尔曲线窗口 - 设置为固定位置和大小
        ImGui::SetNextWindowPos(bezierWindowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(bezierWindowSize, ImGuiCond_Always);
        ImGui::Begin("Bézier Curve", nullptr, 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoCollapse);

        // 获取绘制列表
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // 获取窗口位置 and size
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();

        // 定义绘图区域内的坐标系 (以窗口中心为原点，范围 [-1,1] x [-1,1])
        float scale = std::min(window_size.x, window_size.y) / 2.0f * 0.8f; // 留边距
        ImVec2 center = ImVec2(window_pos.x + window_size.x / 2.0f, window_pos.y + window_size.y / 2.0f);

        // 定义映射函数
        auto worldToScreen = [&](const glm::vec3& p) -> ImVec2 {
            return ImVec2(center.x + p.x * scale, center.y - p.y * scale);
        };

        // 检测鼠标点击和拖动
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mouse_pos = io.MousePos;
        bool mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

        // Handle dragging
        if (!isDragging && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            // Check if mouse is near any control point
            for (int i = 0; i < controlPoints.size(); ++i) {
                ImVec2 screen_pos = worldToScreen(controlPoints[i]);
                float distance = sqrtf((mouse_pos.x - screen_pos.x) * (mouse_pos.x - screen_pos.x) +
                                       (mouse_pos.y - screen_pos.y) * (mouse_pos.y - screen_pos.y));
                if (distance <= dragThreshold) {
                    isDragging = true;
                    selectedPoint = i;
                    break;
                }
            }
        }

        if (isDragging && mouse_down) {
            if (selectedPoint >= 0 && selectedPoint < controlPoints.size()) {
                // Convert screen coordinates back to world coordinates
                glm::vec3 new_pos(
                    (mouse_pos.x - center.x) / scale,
                    (center.y - mouse_pos.y) / scale,
                    controlPoints[selectedPoint].z
                );
                controlPoints[selectedPoint] = new_pos;
            }
        }
        else {
            isDragging = false;
            selectedPoint = -1;
        }

        // 生成曲线顶点
        const int curveSegments = 100;  // 采样分段数越多越平滑
        std::vector<glm::vec3> curveVertices = generateBezierCurveVertices(controlPoints, curveSegments);

        // 绘制控制折线（蓝色）
        for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
            ImVec2 p1 = worldToScreen(controlPoints[i]);
            ImVec2 p2 = worldToScreen(controlPoints[i + 1]);
            draw_list->AddLine(p1, p2, IM_COL32(0, 0, 255, 255), 2.0f);
        }

        // 绘制贝塞尔曲线（红色）
        for (size_t i = 0; i < curveVertices.size() - 1; ++i) {
            ImVec2 p1 = worldToScreen(curveVertices[i]);
            ImVec2 p2 = worldToScreen(curveVertices[i + 1]);
            draw_list->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 2.0f);
        }

        // 绘制控制点（黄色圆点）
        for (size_t i = 0; i < controlPoints.size(); ++i) {
            ImVec2 p = worldToScreen(controlPoints[i]);
            // 颜色根据是否被选中
            ImU32 color = (i == selectedPoint && isDragging) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 0, 200);
            draw_list->AddCircleFilled(p, 6.0f, color);
            draw_list->AddCircle(p, 6.0f, IM_COL32(0, 0, 0, 255), 12, 2.0f); // 边框
        }

        ImGui::End();

        // 使用 easyimgui 结束帧
        EndImGuiFrame(window);
    }

    // 使用 easyimgui 清理
    CleanupGLFWAndImGui(window);

    // 删除 OpenGL 对象
    glDeleteProgram(lineShaderProgram);

    return 0;
}