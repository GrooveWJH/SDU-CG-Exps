#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "imgui.h"

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

// 使用中点算法绘制直线
void DrawLineMidpoint(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

// 使用 Bresenham 算法绘制直线
void DrawLineBresenham(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

#endif // ALGORITHM_H