#include "Algorithm.h"
#include <algorithm>
#include <cmath>

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius)
{
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
void DrawLineMidpoint(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius)
{
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
    int incE = 2 * dy; // 增量（沿主方向）
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

void DrawLineBresenham(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImU32 color, float radius)
{
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
    int incE = 2 * dy; // 沿主方向的增量
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

        draw_list->AddRect(ImVec2(x, y), ImVec2(x + 5, y + 5), color);
    }
}

// 使用中点画圆算法绘制圆
void DrawCircleMidpoint(ImDrawList* draw_list, ImVec2 center, int radius, ImU32 color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius; // 决策变量

    // 辅助函数：绘制八分对称点
    auto draw_symmetric_points = [&](int x, int y) {
        draw_list->AddCircleFilled(ImVec2(center.x + x, center.y + y), 1.0f, color); // 第一象限
        draw_list->AddCircleFilled(ImVec2(center.x - x, center.y + y), 1.0f, color); // 第二象限
        draw_list->AddCircleFilled(ImVec2(center.x + x, center.y - y), 1.0f, color); // 第四象限
        draw_list->AddCircleFilled(ImVec2(center.x - x, center.y - y), 1.0f, color); // 第三象限
        draw_list->AddCircleFilled(ImVec2(center.x + y, center.y + x), 1.0f, color); // 垂直对称（第一）
        draw_list->AddCircleFilled(ImVec2(center.x - y, center.y + x), 1.0f, color); // 垂直对称（第二）
        draw_list->AddCircleFilled(ImVec2(center.x + y, center.y - x), 1.0f, color); // 垂直对称（第四）
        draw_list->AddCircleFilled(ImVec2(center.x - y, center.y - x), 1.0f, color); // 垂直对称（第三）
    };

    // 绘制初始点
    draw_symmetric_points(x, y);

    // 使用中点画圆算法绘制圆
    while (x < y) {
        if (d < 0) { // 选择右侧点
            d += 2 * x + 3;
        } else { // 选择右下方点
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        draw_symmetric_points(x, y); // 绘制对称点
    }
}

// 使用中点画椭圆算法绘制椭圆
void DrawEllipseMidpoint(ImDrawList* draw_list, ImVec2 center, int a, int b, ImU32 color)
{
    int x = 0;
    int y = b;

    // 决策变量
    double d1 = b * b - a * a * b + 0.25 * a * a;
    int dx = 2 * b * b * x;
    int dy = 2 * a * a * y;

    // 辅助函数：绘制四象限对称点
    auto draw_symmetric_points = [&](int x, int y) {
        draw_list->AddCircleFilled(ImVec2(center.x + x, center.y + y), 1.0f, color);
        draw_list->AddCircleFilled(ImVec2(center.x - x, center.y + y), 1.0f, color);
        draw_list->AddCircleFilled(ImVec2(center.x + x, center.y - y), 1.0f, color);
        draw_list->AddCircleFilled(ImVec2(center.x - x, center.y - y), 1.0f, color);
    };

    // 绘制初始点
    draw_symmetric_points(x, y);

    // 第一区域
    while (dx < dy) {
        if (d1 < 0) {
            x++;
            dx += 2 * b * b;
            d1 += dx + b * b;
        } else {
            x++;
            y--;
            dx += 2 * b * b;
            dy -= 2 * a * a;
            d1 += dx - dy + b * b;
        }
        draw_symmetric_points(x, y);
    }

    // 第二区域
    double d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
    while (y >= 0) {
        if (d2 > 0) {
            y--;
            dy -= 2 * a * a;
            d2 += a * a - dy;
        } else {
            x++;
            y--;
            dx += 2 * b * b;
            dy -= 2 * a * a;
            d2 += dx - dy + a * a;
        }
        draw_symmetric_points(x, y);
    }
}

// 使用有序边表算法绘制填充多边形
// 边结构体
void DrawPolygonWithOrderedEdgeTable(ImDrawList* draw_list, ImVec2 canvas_pos,
    const std::vector<float>& x, const std::vector<float>& y,
    int vertexCount, ImU32 color)
{
    if (vertexCount < 3)
        return; // 至少需要 3 个顶点

    // 首先找到多边形的y范围
    int ymin = std::numeric_limits<int>::max();
    int ymax = std::numeric_limits<int>::min();

    for (int i = 0; i < vertexCount; ++i) {
        ymin = std::min(ymin, static_cast<int>(std::floor(y[i])));
        ymax = std::max(ymax, static_cast<int>(std::ceil(y[i])));
    }

    // 创建边表，根据y范围动态调整大小
    std::vector<std::vector<Edge>> edgeTable(ymax - ymin + 1);

    for (int i = 0; i < vertexCount; ++i) {
        int next = (i + 1) % vertexCount;
        float x0f = x[i];
        float y0f = y[i];
        float x1f = x[next];
        float y1f = y[next];

        // 忽略水平边
        if (y0f == y1f)
            continue;

        // 确保 y0 < y1
        if (y0f > y1f) {
            std::swap(x0f, x1f);
            std::swap(y0f, y1f);
        }

        // 计算边的增量
        float dx = (x1f - x0f) / (y1f - y0f);

        // 根据扫描线规则处理顶点共享情况
        // 如果顶点是局部最小或最大，只包含一次
        if (std::ceil(y0f) == std::floor(y1f)) {
            y1f -= 1.0f; // 让上端点不包含在当前边
        }
        // Edge(float x_val, float dx_val, int ymax_val): x(x_val), dx(dx_val), ymax(ymax_val) {}
        Edge edge;
        edge.x = x0f;
        edge.dx = dx;
        edge.ymax = static_cast<int>(std::ceil(y1f));

        // 将边添加到对应的边表
        int edgeTableIndex = static_cast<int>(std::floor(y0f)) - ymin;
        if (edgeTableIndex >= 0 && edgeTableIndex < edgeTable.size()) {
            edgeTable[edgeTableIndex].push_back(edge);
        }
    }

    // 初始化活动边表 (Active Edge Table, AET)
    std::vector<Edge> activeEdgeTable;

    // 扫描线填充
    for (int y = ymin; y <= ymax; ++y) {
        int scanLine = y;

        // 将当前扫描线的边加入 AET
        if (scanLine - ymin >= 0 && scanLine - ymin < edgeTable.size()) {
            activeEdgeTable.insert(activeEdgeTable.end(),
                edgeTable[scanLine - ymin].begin(),
                edgeTable[scanLine - ymin].end());
        }

        // 移除 y >= ymax 的边
        activeEdgeTable.erase(
            std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(),
                [scanLine](const Edge& e) {
                    return e.ymax <= scanLine;
                }),
            activeEdgeTable.end());

        // 按 x 排序 AET
        std::sort(activeEdgeTable.begin(), activeEdgeTable.end(),
            [](const Edge& a, const Edge& b) -> bool {
                return a.x < b.x;
            });

        // 使用偶数奇数规则绘制扫描线
        for (size_t i = 0; i + 1 < activeEdgeTable.size(); i += 2) {
            float xStart = activeEdgeTable[i].x;
            float xEnd = activeEdgeTable[i + 1].x;

            // 转换为整数像素坐标，考虑画布偏移
            int pixelStart = static_cast<int>(std::ceil(xStart));
            int pixelEnd = static_cast<int>(std::floor(xEnd));

            if (pixelEnd >= pixelStart) {
                draw_list->AddLine(
                    ImVec2(canvas_pos.x + pixelStart, canvas_pos.y + scanLine),
                    ImVec2(canvas_pos.x + pixelEnd, canvas_pos.y + scanLine),
                    color);
            }
        }

        // 更新 AET 中每条边的 x
        for (auto& edge : activeEdgeTable) {
            edge.x += edge.dx;
        }
    }
}

// 使用边标志法（Edge Flag Method）绘制填充多边形
// 使用边标志法（Edge Flag Method）绘制填充多边形
void DrawPolygonWithEdgeFlagMethod(ImDrawList* draw_list, ImVec2 canvas_pos, 
                                   const std::vector<float>& x, const std::vector<float>& y, 
                                   int vertexCount, ImU32 color) {
    if (vertexCount < 3) return; // 多边形至少需要3个顶点

    // 1. 确定多边形的y范围
    int ymin = std::numeric_limits<int>::max();
    int ymax = std::numeric_limits<int>::min();

    for (int i = 0; i < vertexCount; ++i) {
        ymin = std::min(ymin, static_cast<int>(std::floor(y[i])));
        ymax = std::max(ymax, static_cast<int>(std::ceil(y[i])));
    }

    // 2. 创建边表，根据y范围动态调整大小
    std::vector<std::vector<Edge>> edgeTable(ymax - ymin + 1);

    for (int i = 0; i < vertexCount; ++i) {
        int next = (i + 1) % vertexCount;
        float x0f = x[i];
        float y0f = y[i];
        float x1f = x[next];
        float y1f = y[next];

        // 忽略水平边
        if (y0f == y1f) continue;

        // 确保 y0 < y1
        if (y0f > y1f) {
            std::swap(x0f, x1f);
            std::swap(y0f, y1f);
        }

        // 计算边的增量
        float dx = (x1f - x0f) / (y1f - y0f);

        // 根据扫描线规则处理顶点共享情况
        // 如果顶点是局部最小或最大，只包含一次
        if (std::ceil(y0f) == std::floor(y1f)) {
            y1f -= 1.0f; // 让上端点不包含在当前边
        }

        Edge edge;
        edge.x = x0f;
        edge.dx = dx;
        edge.ymax = static_cast<int>(std::ceil(y1f));

        // 将边添加到对应的边表
        int edgeTableIndex = static_cast<int>(std::floor(y0f)) - ymin;
        if (edgeTableIndex >= 0 && edgeTableIndex < edgeTable.size()) {
            edgeTable[edgeTableIndex].push_back(edge);
        }
    }

    // 3. 初始化活动边表 (Active Edge Table, AET)
    std::vector<Edge> activeEdgeTable;

    // 4. 扫描线填充
    for (int y = ymin; y <= ymax; ++y) {
        int scanLine = y;

        // 将当前扫描线的边加入 AET
        if (scanLine - ymin >= 0 && scanLine - ymin < edgeTable.size()) {
            activeEdgeTable.insert(activeEdgeTable.end(), 
                                   edgeTable[scanLine - ymin].begin(), 
                                   edgeTable[scanLine - ymin].end());
        }

        // 移除 y >= ymax 的边
        activeEdgeTable.erase(
            std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(), 
                           [scanLine](const Edge& e) {
                               return e.ymax <= scanLine;
                           }), 
            activeEdgeTable.end());

        // 按 x 排序 AET
        std::sort(activeEdgeTable.begin(), activeEdgeTable.end(), 
                  [](const Edge& a, const Edge& b) -> bool {
                      return a.x < b.x;
                  });

        // 使用边标志法（Edge Flag Method）进行填充
        bool fill = false;
        float xStart = 0.0f, xEnd = 0.0f;

        for (size_t i = 0; i < activeEdgeTable.size(); ++i) {
            if (!fill) {
                xStart = activeEdgeTable[i].x;
                fill = true;
            } else {
                xEnd = activeEdgeTable[i].x;
                // 绘制从 xStart 到 xEnd 的线
                draw_list->AddLine(
                    ImVec2(canvas_pos.x + xStart, canvas_pos.y + scanLine),
                    ImVec2(canvas_pos.x + xEnd, canvas_pos.y + scanLine),
                    color
                );
                fill = false;
            }
        }

        // 更新 AET 中每条边的 x
        for (auto& edge : activeEdgeTable) {
            edge.x += edge.dx;
        }
    }
}