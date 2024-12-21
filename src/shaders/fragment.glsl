#version 330 core
out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform float aspectRatio;

uniform int numSpheres;
uniform vec3 spheres[1].center;
uniform float spheres[1].radius;
uniform vec3 spheres[1].color;

void main()
{
    // 用纯色填充屏幕作为测试
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色
}