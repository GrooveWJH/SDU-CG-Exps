#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// 顶点着色器源代码
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)glsl";

// 片段着色器源代码
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Pyramid Animation", NULL, NULL);
    if (window == NULL) {
        std::cerr << "无法创建 GLFW 窗口" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "无法初始化 GLAD" << std::endl;
        return -1;
    }

    // 构建和编译着色器程序
    // 顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查着色器编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // ��查着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 链接着色器
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置顶点数据(和缓冲区)并配置顶点属性
    float vertices[] = {
        // 前面
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶部 - 红色
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // 左前 - 绿色
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // 右前 - 蓝色

        // 后面
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶部 - 红色
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // 左后 - 黄色
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // 右后 - 青色

        // 左侧
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶部 - 红色
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // 左前 - 绿色
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // 左后 - 黄色

        // 右侧
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶部 - 红色
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // 右前 - 蓝色
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f // 右后 - 青色
    };

    // 边的顶点数据
    float edgeVertices[] = {
        // 顶部到底部的边
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // 顶部 - 黑色
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // 左前 - 黑色

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // 顶部 - 黑色
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // 右前 - 黑色

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // 顶部 - 黑色
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左后 - 黑色

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // 顶部 - 黑色
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 右后 - 黑色

        // 底部的边
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // 左前 - 黑色
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // 右前 - 黑色

        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // 右前 - 黑色
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 右后 - 黑色

        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 右后 - 黑色
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左后 - 黑色

        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左后 - 黑色
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f // 左前 - 黑色
    };

    unsigned int VBO, VAO, edgeVBO, edgeVAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 设置边的顶点数据
    glGenVertexArrays(1, &edgeVAO);
    glGenBuffers(1, &edgeVBO);
    glBindVertexArray(edgeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edgeVertices), edgeVertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 启用深度测试以实现 3D 效果
    glEnable(GL_DEPTH_TEST);

    // 变换矩阵
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    float lastFrame = 0.0f; // 上一帧的时间
    float rotationSpeed = glm::radians(20.0f); // 旋转速度

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活着色器
        glUseProgram(shaderProgram);

        // 传递变换矩阵到着色器
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // 使用 deltaTime 计算旋转
        model = glm::rotate(model, rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // 渲染金字塔
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // 渲染边
        glLineWidth(10.0f);
        glBindVertexArray(edgeVAO);
        glDrawArrays(GL_LINES, 0, 16);

        // 交换缓冲区并轮询 IO 事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 可选：在它们超出用途后释放所有资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &edgeVAO);
    glDeleteBuffers(1, &edgeVBO);
    glDeleteProgram(shaderProgram);

    // 终止 GLFW，清除任何由 GLFW 分配的资源。
    glfwTerminate();
    return 0;
}