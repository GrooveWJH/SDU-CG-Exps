#include "Algorithm.h"
#include <cmath>
#include <algorithm>

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius) {
    // 计算增量
    float dx = end.x - start.x;
    float dy = end.y - start.y;

    // 确定步数（绝对值更大的轴方向决定步数）
    float steps = std::max(std::abs(dx), std::abs(dy));

    // 计算每一步的增量
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    // 起始点
    float x = start.x;
    float y = start.y;

    // 绘制每一个点（用圆形代替矩形）
    for (int i = 0; i <= steps; i++) {
        draw_list->AddCircleFilled(ImVec2(x, y), radius, color); // 绘制一个圆形点
        x += x_inc; // 更新 X 坐标
        y += y_inc; // 更新 Y 坐标
    }
}

// 使用中点算法绘制直线
void DrawLineMidpoint(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius) {
    // 起点和终点的整数坐标
    int x0 = static_cast<int>(start.x);
    int y0 = static_cast<int>(start.y);
    int x1 = static_cast<int>(end.x);
    int y1 = static_cast<int>(end.y);

    // 计算增量
    int dx = x1 - x0;
    int dy = y1 - y0;

    // 确定直线的方向
    int x_step = (dx > 0) ? 1 : -1;
    int y_step = (dy > 0) ? 1 : -1;

    dx = std::abs(dx);
    dy = std::abs(dy);

    // 主方向判断
    bool is_steep = dy > dx;

    if (is_steep) {
        std::swap(dx, dy); // 如果斜率较大，交换 dx 和 dy
    }

    int d = 2 * dy - dx; // 初始化决策变量
    int incE = 2 * dy;   // 增量（沿主方向）
    int incNE = 2 * (dy - dx); // 增量（沿次方向）

    int x = x0;
    int y = y0;

    // 绘制第一个点
    draw_list->AddCircleFilled(ImVec2(x, y), radius, color);

    // 绘制其余点
    for (int i = 0; i < dx; ++i) {
        if (d > 0) {
            if (is_steep) {
                y += y_step;
            } else {
                x += x_step;
            }
            d += incNE; // 更新决策变量
        } else {
            d += incE; // 更新决策变量
        }

        if (is_steep) {
            x += x_step;
        } else {
            y += y_step;
        }

        draw_list->AddCircleFilled(ImVec2(x, y), radius, color); // 绘制点
    }
}

void DrawLineBresenham(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius) {
    // 起点和终点的整数坐标
    int x0 = static_cast<int>(start.x);
    int y0 = static_cast<int>(start.y);
    int x1 = static_cast<int>(end.x);
    int y1 = static_cast<int>(end.y);

    // 计算增量
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    // 确定直线的方向
    int x_step = (x1 > x0) ? 1 : -1;
    int y_step = (y1 > y0) ? 1 : -1;

    // 判断直线的主方向
    bool is_steep = dy > dx;
    if (is_steep) {
        std::swap(dx, dy); // 如果斜率大于 1，交换 dx 和 dy
    }

    int d = 2 * dy - dx; // 初始化决策变量
    int incE = 2 * dy;   // 沿主方向的增量
    int incNE = 2 * (dy - dx); // 沿次方向的增量

    int x = x0;
    int y = y0;

    // 绘制第一个点
    draw_list->AddCircleFilled(ImVec2(x, y), radius, color);

    // 按 Bresenham 算法绘制其余的点
    for (int i = 0; i < dx; ++i) {
        if (d > 0) {
            if (is_steep) {
                x += x_step;
            } else {
                y += y_step;
            }
            d += incNE; // 更新决策变量
        } else {
            d += incE; // 更新决策变量
        }

        if (is_steep) {
            y += y_step;
        } else {
            x += x_step;
        }

        draw_list->AddRect(ImVec2(x, y), ImVec2(x+5, y+5), color);
    }
}