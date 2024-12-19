#version 330 core

// 输入顶点位置
layout(location = 0) in vec3 aPos;

void main()
{
    // 将顶点位置传递给裁剪空间
    gl_Position = vec4(aPos, 1.0);
}