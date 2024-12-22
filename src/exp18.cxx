#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>
#include <iostream>

// ------------- 顶点着色器 (基础) ------------- //
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;    // 位置
layout (location = 1) in vec3 aColor;  // 颜色

uniform mat4 MVP;  // 投影 * 视图 * 模型

out vec3 vertColor;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    vertColor = aColor;
}
)";

// ------------- 片段着色器 (基础) ------------- //
const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vertColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertColor, 1.0);
}
)";

// ------------- 阴影片段着色器 (半透明黑) ------------- //
const char* shadowFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    // 半透明黑色阴影
    FragColor = vec4(0.0, 0.0, 0.0, 0.5);
}
)";

// ------------- 辅助函数：检查编译和链接错误 ------------- //
void checkCompileErrors(unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n"
                      << "-------------------------------------------------------\n";
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n"
                      << "-------------------------------------------------------\n";
        }
    }
}

// ------------- 立方体的顶点数据 ------------- //
// 立方体中心在原点，边长为1，底面在 y=0
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// 立方体的8个顶点
std::vector<Vertex> cubeVertices = {
    // 底面
    { glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 0.5f, 0.31f) },
    { glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 0.5f, 0.31f) },
    { glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3(1.0f, 0.5f, 0.31f) },
    { glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(1.0f, 0.5f, 0.31f) },
    // 顶面
    { glm::vec3(-0.5f, 1.0f, -0.5f), glm::vec3(0.8f, 0.3f, 0.02f) },
    { glm::vec3( 0.5f, 1.0f, -0.5f), glm::vec3(0.8f, 0.3f, 0.02f) },
    { glm::vec3( 0.5f, 1.0f,  0.5f), glm::vec3(0.8f, 0.3f, 0.02f) },
    { glm::vec3(-0.5f, 1.0f,  0.5f), glm::vec3(0.8f, 0.3f, 0.02f) },
};

// 立方体的索引（12个三角形）
std::vector<unsigned int> cubeIndices = {
    // 底面
    0, 1, 2,
    2, 3, 0,
    // 顶面
    4, 5, 6,
    6, 7, 4,
    // 前面
    1, 5, 6,
    6, 2, 1,
    // 后面
    0, 4, 7,
    7, 3, 0,
    // 左面
    0, 1, 5,
    5, 4, 0,
    // 右面
    3, 2, 6,
    6, 7, 3
};

// ------------- 地面的顶点数据 ------------- //
std::vector<Vertex> groundVertices = {
    { glm::vec3(-10.0f, 0.0f, -10.0f), glm::vec3(0.3f, 0.8f, 0.3f) },
    { glm::vec3( 10.0f, 0.0f, -10.0f), glm::vec3(0.3f, 0.8f, 0.3f) },
    { glm::vec3( 10.0f, 0.0f,  10.0f), glm::vec3(0.3f, 0.8f, 0.3f) },
    { glm::vec3(-10.0f, 0.0f,  10.0f), glm::vec3(0.3f, 0.8f, 0.3f) },
};

// 地面的索引（两个三角形）
std::vector<unsigned int> groundIndices = {
    0, 1, 2,
    2, 3, 0
};

// ------------- 光源的初始参数 ------------- //
float lightRadius = 3.0f;   // 光源绕原点的半径
float lightHeight = 4.0f;   // 光源的高度
float lightSpeed  = 1.0f;   // 光源的旋转速度（弧度/秒）

// ------------- 相机控制相关变量 ------------- //
bool firstMouse = true;
double lastX = 400, lastY = 300; // 初始鼠标位置（窗口中心）
float yaw   = -90.0f; // 初始偏航角（向左看）
float pitch = 0.0f;   // 初始俯仰角

float fov = 45.0f;     // 视野角度

// 相机位置和方向
glm::vec3 cameraPos   = glm::vec3(3.0f, 4.0f, 5.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// 添加全局变量用于鼠标控制
bool mouseLeftButtonPressed = false;

// 添加相机移动速度
float cameraSpeed = 5.0f;  // 每秒移动的单位

// 添加全局变量存储窗口尺寸
int g_width = 800, g_height = 600;

// ------------- 鼠标移动回调函数 ------------- //
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // 只在鼠标左键按下时处理视角转动
    if (!mouseLeftButtonPressed) {
        lastX = xpos;
        lastY = ypos;
        return;
    }

    static float sensitivity = 0.1f;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 添加鼠标按键回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftButtonPressed = true;
        } else if (action == GLFW_RELEASE) {
            mouseLeftButtonPressed = false;
            firstMouse = true;  // 重置firstMouse状态
        }
    }
}

// 添加处理键盘输入的函数
void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = cameraSpeed * deltaTime;

    // 前后移动
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += velocity * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= velocity * cameraFront;

    // 左右移动
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
}

// ------------- 获取视图矩阵的函数 ------------- //
glm::mat4 getViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

int main()
{
    // 初始化 GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    // 设置 OpenGL 版本 (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 对于 MacOS
#endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rotating Cube with Shadow", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 隐藏并锁定鼠标光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 注册鼠标回调
    glfwSetCursorPosCallback(window, mouse_callback);

    // 注册鼠标按键回调
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        return -1;
    }

    // 创建窗口后，获取初始窗口大小
    glfwGetFramebufferSize(window, &g_width, &g_height);
    glViewport(0, 0, g_width, g_height);

    // 修改窗口大小回调函数
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
        g_width = width;
        g_height = height > 0 ? height : 1; // 防止除以零
        glViewport(0, 0, width, height);
    });

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // ------------- 编译和链接着色器程序 ------------- //
    // 立方体和地面的着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // 阴影的着色器
    unsigned int shadowFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shadowFragmentShader, 1, &shadowFragmentShaderSrc, NULL);
    glCompileShader(shadowFragmentShader);
    checkCompileErrors(shadowFragmentShader, "FRAGMENT");

    unsigned int shadowShaderProgram = glCreateProgram();
    glAttachShader(shadowShaderProgram, vertexShader); // 使用相同的顶点着色器
    glAttachShader(shadowShaderProgram, shadowFragmentShader);
    glLinkProgram(shadowShaderProgram);
    checkCompileErrors(shadowShaderProgram, "PROGRAM");

    // 删除不再需要的着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(shadowFragmentShader);

    // ------------- 设置立方体的 VAO 和 VBO ------------- //
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Vertex), cubeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ------------- 设置地面的 VAO 和 VBO ------------- //
    unsigned int groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(Vertex), groundVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, groundIndices.size() * sizeof(unsigned int), groundIndices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ------------- 设置阴影的 VAO 和 VBO ------------- //
    unsigned int shadowVAO, shadowVBO, shadowEBO;
    glGenVertexArrays(1, &shadowVAO);
    glGenBuffers(1, &shadowVBO);
    glGenBuffers(1, &shadowEBO);

    glBindVertexArray(shadowVAO);

    glBindBuffer(GL_ARRAY_BUFFER, shadowVBO);
    // 初始数据为空，动态更新
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性（未使用，可设置为黑色）
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 添加上一帧的时间变量
    float lastFrame = 0.0f;

    // ------------- 主渲染循环 ------------- //
    while (!glfwWindowShouldClose(window))
    {
        // 计算帧间隔时间
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processInput(window, deltaTime);
        glfwPollEvents();

        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ------------- 计算光源位置 ------------- //
        float time = glfwGetTime();
        float angle = lightSpeed * time;
        float lx = lightRadius * std::cos(angle);
        float lz = lightRadius * std::sin(angle);
        glm::vec3 lightPos(lx, lightHeight, lz);

        // ------------- 计算阴影顶点位置 ------------- //
        std::vector<Vertex> shadowVertices(cubeVertices.size());
        for (size_t i = 0; i < cubeVertices.size(); ++i)
        {
            glm::vec3 v = cubeVertices[i].position;
            // 投影到 y=0 平面
            float t = -lightPos.y / (v.y - lightPos.y);
            glm::vec3 shadowPos = lightPos + t * (v - lightPos);

            // 为避免与地面平面重合，稍微提高阴影的 y 值
            shadowPos.y = 0.001f;

            shadowVertices[i].position = shadowPos;
            shadowVertices[i].color = glm::vec3(0.0f, 0.0f, 0.0f); // 阴影为黑色
        }

        // 更新阴影 VBO
        glBindBuffer(GL_ARRAY_BUFFER, shadowVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, shadowVertices.size() * sizeof(Vertex), shadowVertices.data());

        // ------------- 设置视图和投影矩阵 ------------- //
        glm::mat4 view = getViewMatrix();
        float aspect = static_cast<float>(g_width) / static_cast<float>(g_height);
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);

        // ------------- 绘制地面 ------------- //
        glm::mat4 model_ground = glm::mat4(1.0f);
        glm::mat4 MVP_ground = projection * view * model_ground;

        glUseProgram(shaderProgram);
        unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP_ground));

        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, groundIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // ------------- 绘制立方体 ------------- //
        glm::mat4 model_cube = glm::mat4(1.0f); // 立方体已在 y=0
        glm::mat4 MVP_cube = projection * view * model_cube;

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP_cube));

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // ------------- 绘制阴影 ------------- //
        // 使用多边形偏移以防止 Z-fighting
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);

        // 禁用深度写入
        glDepthMask(GL_FALSE);

        // 使用阴影着色器
        glm::mat4 model_shadow = glm::mat4(1.0f);
        glm::mat4 MVP_shadow = projection * view * model_shadow;

        glUseProgram(shadowShaderProgram);
        unsigned int mvpShadowLoc = glGetUniformLocation(shadowShaderProgram, "MVP");
        glUniformMatrix4fv(mvpShadowLoc, 1, GL_FALSE, glm::value_ptr(MVP_shadow));

        glBindVertexArray(shadowVAO);
        // 启用混合以实现半透明效果
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);
        glBindVertexArray(0);

        // 重新启用深度写入和禁用多边形偏移
        glDepthMask(GL_TRUE);
        glDisable(GL_POLYGON_OFFSET_FILL);

        // ------------- 交换缓冲区 ------------- //
        glfwSwapBuffers(window);
    }

    // ------------- 清理资源 ------------- //
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);

    glDeleteVertexArrays(1, &shadowVAO);
    glDeleteBuffers(1, &shadowVBO);
    glDeleteBuffers(1, &shadowEBO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shadowShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}