#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// 顶点着色器源码
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)glsl";

// 片段着色器源码
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform bool enabled; // 是否启用光照
uniform int mode;      // 光照模式

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    if (!enabled) {
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    // 环境光
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    if (mode == 1) {
        // 无限远方向光
        lightDir = normalize(-lightPos);
    } else {
        // 点光源
        lightDir = normalize(lightPos - FragPos);
    }
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // 合并结果
    vec3 result = (ambient + diffuse + specular) * objectColor;

    // 根据模式调整结果
    if (mode == 0) {
        // 双面光照
        if (dot(norm, lightDir) < 0.0) {
            norm = -norm;
            diff = max(dot(norm, lightDir), 0.0);
            diffuse = diffuseStrength * diff * lightColor;
            reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            specular = specularStrength * spec * lightColor;
            result = (ambient + diffuse + specular) * objectColor;
        }
    } else if (mode == 2) {
        // 仅环境光
        result = ambient * objectColor;
    }

    FragColor = vec4(result, 1.0);
}
)glsl";

// 生成圆环顶点数据
void generateTorus(std::vector<float>& vertices, std::vector<unsigned int>& indices, float innerRadius, float outerRadius, int numSides, int numRings) {
    for (int ring = 0; ring <= numRings; ++ring) {
        float theta = static_cast<float>(ring) / numRings * 2.0f * M_PI;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int side = 0; side <= numSides; ++side) {
            float phi = static_cast<float>(side) / numSides * 2.0f * M_PI;
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);

            float x = (outerRadius + innerRadius * cosPhi) * cosTheta;
            float y = (outerRadius + innerRadius * cosPhi) * sinTheta;
            float z = innerRadius * sinPhi;

            // 法线
            float nx = cosPhi * cosTheta;
            float ny = cosPhi * sinTheta;
            float nz = sinPhi;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // 索引
            if (ring < numRings && side < numSides) {
                int curRow = ring * (numSides + 1);
                int nextRow = (ring + 1) * (numSides + 1);

                indices.push_back(curRow + side);
                indices.push_back(nextRow + side);
                indices.push_back(nextRow + side + 1);

                indices.push_back(curRow + side);
                indices.push_back(nextRow + side + 1);
                indices.push_back(curRow + side + 1);
            }
        }
    }
}

// 检查着色器编译和程序链接错误
void checkShaderCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}

// 光照控制参数结构体
struct LightingParams {
    bool enabled = true;
    int mode = 0;  // 0: 双面光照, 1: 无限远光, 2: 全局环境光
    glm::vec3 lightColor = glm::vec3(1.0f);
    glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    float ambientStrength = 0.1f;
    float diffuseStrength = 0.7f;
    float specularStrength = 0.5f;
    float shininess = 32.0f;
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
} lightParams;

// 旋转控制参数结构体
struct RotationParams {
    bool enabled = false;
    float speed = 50.0f; // 旋转速度，度/秒
} rotationParams;

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置 GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 对于 MacOS
#endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "exp17: Illumination Model", NULL, NULL);
    if (!window) {
        std::cerr << "创建 GLFW 窗口失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "初始化 GLAD 失败" << std::endl;
        return -1;
    }

    // 获取实际的帧缓冲大小并设置视口
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // 设置窗口大小回调
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompileErrors(vertexShader, "VERTEX");

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileErrors(fragmentShader, "FRAGMENT");

    // 创建着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderCompileErrors(shaderProgram, "PROGRAM");

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 获取 uniform 位置（优化：一次性获取）
    int enabledLoc = glGetUniformLocation(shaderProgram, "enabled");
    int modeLoc = glGetUniformLocation(shaderProgram, "mode");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    int ambientStrengthLoc = glGetUniformLocation(shaderProgram, "ambientStrength");
    int diffuseStrengthLoc = glGetUniformLocation(shaderProgram, "diffuseStrength");
    int specularStrengthLoc = glGetUniformLocation(shaderProgram, "specularStrength");
    int shininessLoc = glGetUniformLocation(shaderProgram, "shininess");
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // 生成圆环几何数据
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateTorus(vertices, indices, 0.3f, 0.7f, 50, 50);

    // 设置 VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定 VAO
    glBindVertexArray(VAO);

    // 绑定并设置 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 绑定并设置 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 配置顶点属性
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // 设置 ImGui 样式
    ImGui::StyleColorsDark();
    // 初始化 ImGui GLFW 和 OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 定义视点位置
    glm::vec3 viewPos(0.0f, 0.0f, 3.0f);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        glfwPollEvents();

        // 清除颜色和深度缓冲
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 启动 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 创建控制面板
        ImGui::Begin("Lighting Control");

        ImGui::Checkbox("Enable Lighting", &lightParams.enabled);

        ImGui::Text("Lighting Mode");
        const char* modes[] = { "Two-sided Lighting", "Directional Light", "Ambient Light Only" };
        ImGui::Combo("Lighting Mode", &lightParams.mode, modes, IM_ARRAYSIZE(modes));

        ImGui::ColorEdit3("Light Color", glm::value_ptr(lightParams.lightColor));
        ImGui::ColorEdit3("Object Color", glm::value_ptr(lightParams.objectColor));

        ImGui::SliderFloat("Ambient Strength", &lightParams.ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse Strength", &lightParams.diffuseStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular Strength", &lightParams.specularStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &lightParams.shininess, 1.0f, 256.0f);

        ImGui::DragFloat3("Light Position", glm::value_ptr(lightParams.lightPos), 0.1f, -5.0f, 5.0f);

        // 旋转控制
        ImGui::Checkbox("Enable Rotation", &rotationParams.enabled);
        if (rotationParams.enabled) {
            ImGui::SliderFloat("Rotation Speed (deg/s)", &rotationParams.speed, -360.0f, 360.0f);
        }

        if (ImGui::Button("Reset")) {
            lightParams = LightingParams();       // 重置光照参数
            rotationParams = RotationParams();   // 重置旋转参数
        }

        ImGui::End();

        // 使用着色器程序
        glUseProgram(shaderProgram);

        // 设置 uniform 变量
        glUniform1i(enabledLoc, lightParams.enabled ? 1 : 0);
        glUniform1i(modeLoc, lightParams.mode);
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightParams.lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightParams.lightColor));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(lightParams.objectColor));
        glUniform1f(ambientStrengthLoc, lightParams.ambientStrength);
        glUniform1f(diffuseStrengthLoc, lightParams.diffuseStrength);
        glUniform1f(specularStrengthLoc, lightParams.specularStrength);
        glUniform1f(shininessLoc, lightParams.shininess);

        // 设置 MVP 矩阵
        glm::mat4 model = glm::mat4(1.0f);
        if (rotationParams.enabled) {
            float angle = rotationParams.speed * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            // 如果不旋转，可以保持模型矩阵为单位矩阵或其他固定状态
        }

        glm::mat4 view = glm::translate(glm::mat4(1.0f), -viewPos);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                                static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight), 
                                                0.1f, 100.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 绑定 VAO 并绘制圆环
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 清理 ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 删除 OpenGL 对象
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // 终止 GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}