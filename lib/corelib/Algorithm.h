#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "imgui.h"
#include <vector>

struct Edge {
    float x;       // 当前扫描线与边的交点的 x 坐标
    float dx;      // 每条扫描线扫描时 x 坐标的增量
    int ymax;      // 边的上限扫描线 y 坐标
};

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

// 使用中点算法绘制直线
void DrawLineMidpoint(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

// 使用 Bresenham 算法绘制直线
void DrawLineBresenham(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius = 1.0f);

void DrawCircleMidpoint(ImDrawList* draw_list, ImVec2 center, int radius, ImU32 color);

void DrawEllipseMidpoint(ImDrawList* draw_list, ImVec2 center, int a, int b, ImU32 color);

// 使用有序边表算法绘制填充多边形
void DrawPolygonWithOrderedEdgeTable(ImDrawList* draw_list, ImVec2 canvas_pos, 
                                     const std::vector<float>& x, const std::vector<float>& y, 
                                     int vertexCount, ImU32 color);

// 使用边标志法（Edge Flag Method）绘制填充多边形
void DrawPolygonWithEdgeFlagMethod(ImDrawList* draw_list, ImVec2 canvas_pos, 
                                   const std::vector<float>& x, const std::vector<float>& y, 
                                   int vertexCount, ImU32 color);
#endif // ALGORITHM_H