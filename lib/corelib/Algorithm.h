#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "imgui.h"
#include <vector>


// 重载 ImVec2 的运算符
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline ImVec2 operator*(const ImVec2& vec, float scalar) {
    return ImVec2(vec.x * scalar, vec.y * scalar);
}

inline ImVec2 operator*(float scalar, const ImVec2& vec) {
    return ImVec2(vec.x * scalar, vec.y * scalar);
}

inline ImVec2 operator/(const ImVec2& vec, float scalar) {
    return ImVec2(vec.x / scalar, vec.y / scalar);
}

// 判断两个 ImVec2 是否相等
inline bool operator==(const ImVec2& lhs, const ImVec2& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs) {
    return !(lhs == rhs);
}


struct Edge {
    float x; // 当前扫描线与边的交点的 x 坐标
    float dx; // 每条扫描线扫描时 x 坐标的增量
    int ymax; // 边的上限扫描线 y 坐标
};

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius = 1.0f);

// 使用中点算法绘制直线
void DrawLineMidpoint(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius = 1.0f);

// 使用 Bresenham 算法绘制直线
void DrawLineBresenham(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius = 1.0f);

void DrawCircleMidpoint(ImDrawList* draw_list,
    ImVec2 center,
    int radius,
    ImU32 color);

void DrawEllipseMidpoint(ImDrawList* draw_list,
    ImVec2 center,
    int a,
    int b,
    ImU32 color);

void DrawPolygon(ImDrawList* draw_list, const ImVec2& canvas_pos, const std::vector<ImVec2>& polygon, ImU32 color);

// 使用有序边表算法绘制填充多边形
void DrawPolygonWithOrderedEdgeTable(ImDrawList* draw_list,
    ImVec2 canvas_pos,
    const std::vector<float>& x,
    const std::vector<float>& y,
    int vertexCount,
    ImU32 color);

// 使用边标志法（Edge Flag Method）绘制填充多边形
void DrawPolygonWithEdgeFlagMethod(ImDrawList* draw_list,
    ImVec2 canvas_pos,
    const std::vector<float>& x,
    const std::vector<float>& y,
    int vertexCount,
    ImU32 color);

void DrawFilledRegion(ImDrawList* draw_list, ImVec2 canvas_pos, ImVec2 canvas_size, const std::vector<ImVec2>& vertices, int vertex_count, ImVec2 seed_point, ImU32 fill_color);

// 区域码定义
enum OutCode {
    INSIDE = 0, // 点在窗口内
    LEFT = 1, // 点在窗口左侧
    RIGHT = 2, // 点在窗口右侧
    BOTTOM = 4, // 点在窗口下方
    TOP = 8 // 点在窗口上方
};

// 计算点的区域码
OutCode ComputeOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax);

// 使用 Cohen-Sutherland 算法裁剪直线
bool CohenSutherlandLineClip(float& x0, float& y0, float& x1, float& y1, float xmin, float ymin, float xmax, float ymax);

// 定义裁剪窗口
struct ClipWindow {
    float x0 = 100.0f; // 左上角 x 坐标
    float y0 = 100.0f; // 左上角 y 坐标
    float x1 = 200.0f; // 右下角 x 坐标
    float y1 = 200.0f; // 右下角 y 坐标
};

bool IsInside(const ImVec2& point, const ClipWindow& clipWindow, int edge);

// 使用 Sutherland-Hodgman 算法对多边形进行裁剪
std::vector<ImVec2> SutherlandHodgmanPolygonClip(const std::vector<ImVec2>& polygon, const ClipWindow& clipWindow);



#endif // ALGORITHM_H