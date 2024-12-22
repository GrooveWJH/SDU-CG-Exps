#include "Algorithm.h"

// 使用 DDA 算法绘制直线
void DrawLineDDA(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius)
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
void DrawLineMidpoint(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius)
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

void DrawLineBresenham(ImDrawList* draw_list,
    ImVec2 start,
    ImVec2 end,
    ImU32 color,
    float radius)
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
void DrawCircleMidpoint(ImDrawList* draw_list,
    ImVec2 center,
    int radius,
    ImU32 color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius; // 决策变量

    // 辅助函数：绘制八分对称点
    auto draw_symmetric_points = [&](int x, int y) {
        draw_list->AddCircleFilled(
            ImVec2(center.x + x, center.y + y), 1.0f, color); // 第一象限
        draw_list->AddCircleFilled(
            ImVec2(center.x - x, center.y + y), 1.0f, color); // 第二象限
        draw_list->AddCircleFilled(
            ImVec2(center.x + x, center.y - y), 1.0f, color); // 第四象限
        draw_list->AddCircleFilled(
            ImVec2(center.x - x, center.y - y), 1.0f, color); // 第三象限
        draw_list->AddCircleFilled(
            ImVec2(center.x + y, center.y + x), 1.0f, color); // 垂直对称（第一）
        draw_list->AddCircleFilled(
            ImVec2(center.x - y, center.y + x), 1.0f, color); // 垂直对称（第二）
        draw_list->AddCircleFilled(
            ImVec2(center.x + y, center.y - x), 1.0f, color); // 垂直对称（第四）
        draw_list->AddCircleFilled(
            ImVec2(center.x - y, center.y - x), 1.0f, color); // 垂直对称（第三）
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
void DrawEllipseMidpoint(ImDrawList* draw_list,
    ImVec2 center,
    int a,
    int b,
    ImU32 color)
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

void DrawPolygon(ImDrawList* draw_list, const ImVec2& canvas_pos, const std::vector<ImVec2>& polygon, ImU32 color) {
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t next = (i + 1) % polygon.size();
        draw_list->AddLine(
            ImVec2(canvas_pos.x + polygon[i].x, canvas_pos.y + polygon[i].y),
            ImVec2(canvas_pos.x + polygon[next].x, canvas_pos.y + polygon[next].y),
            color, 2.0f
        );
    }
}

// 使用有序边表算法绘制填充多边形
// 边结构体
void DrawPolygonWithOrderedEdgeTable(ImDrawList* draw_list,
    ImVec2 canvas_pos,
    const std::vector<float>& x,
    const std::vector<float>& y,
    int vertexCount,
    ImU32 color)
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
        // Edge(float x_val, float dx_val, int ymax_val): x(x_val), dx(dx_val),
        // ymax(ymax_val) {}
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
            std::remove_if(activeEdgeTable.begin(),
                activeEdgeTable.end(),
                [scanLine](const Edge& e) { return e.ymax <= scanLine; }),
            activeEdgeTable.end());

        // 按 x 排序 AET
        std::sort(activeEdgeTable.begin(),
            activeEdgeTable.end(),
            [](const Edge& a, const Edge& b) -> bool { return a.x < b.x; });

        // 使用偶数奇数规则绘制扫描线
        for (size_t i = 0; i + 1 < activeEdgeTable.size(); i += 2) {
            float xStart = activeEdgeTable[i].x;
            float xEnd = activeEdgeTable[i + 1].x;

            // 转换为整数像素坐标，考虑画布偏���
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
void DrawPolygonWithEdgeFlagMethod(ImDrawList* draw_list,
    ImVec2 canvas_pos,
    const std::vector<float>& x,
    const std::vector<float>& y,
    int vertexCount,
    ImU32 color)
{
    if (vertexCount < 3)
        return; // 多边形至少需要3个顶点

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
            std::remove_if(activeEdgeTable.begin(),
                activeEdgeTable.end(),
                [scanLine](const Edge& e) { return e.ymax <= scanLine; }),
            activeEdgeTable.end());

        // 按 x 排序 AET
        std::sort(activeEdgeTable.begin(),
            activeEdgeTable.end(),
            [](const Edge& a, const Edge& b) -> bool { return a.x < b.x; });

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
                    color);
                fill = false;
            }
        }

        // 更新 AET 中每条边的 x
        for (auto& edge : activeEdgeTable) {
            edge.x += edge.dx;
        }
    }
}

// 填充多边形区域（修复偏移问题）
void DrawFilledRegion(ImDrawList* draw_list, ImVec2 canvas_pos, ImVec2 canvas_size, const std::vector<ImVec2>& vertices, int vertex_count, ImVec2 seed_point, ImU32 fill_color) {
    // 创建一个临时数组，用于存储转换后的顶点坐标
    std::vector<ImVec2> screen_vertices;
    screen_vertices.reserve(vertex_count);

    // 将顶点坐标从多边形坐标系转换为屏幕坐标
    for (int i = 0; i < vertex_count; ++i) {
        screen_vertices.push_back(ImVec2(canvas_pos.x + vertices[i].x, canvas_pos.y + vertices[i].y));
    }

    // 判断种子点是否在多边形内（射线法）
    bool inside_polygon = false;
    int intersections = 0;
    for (int i = 0; i < vertex_count; ++i) {
        ImVec2 p1 = vertices[i];
        ImVec2 p2 = vertices[(i + 1) % vertex_count];
        if ((seed_point.y > p1.y) != (seed_point.y > p2.y)) {
            float x_intersect = p1.x + (seed_point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            if (seed_point.x < x_intersect) {
                intersections++;
            }
        }
    }
    inside_polygon = (intersections % 2 == 1);

    // 如果种子点在多边形内部，则填充内部
    if (inside_polygon) {
        draw_list->AddConvexPolyFilled(screen_vertices.data(), vertex_count, fill_color);
    }
}


// 计算点的区域码
OutCode ComputeOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax)
{
    OutCode code = INSIDE;

    if (x < xmin)
        code = static_cast<OutCode>(code | LEFT);
    else if (x > xmax)
        code = static_cast<OutCode>(code | RIGHT);
    if (y < ymin)
        code = static_cast<OutCode>(code | BOTTOM);
    else if (y > ymax)
        code = static_cast<OutCode>(code | TOP);

    return code;
}

// 使用 Cohen-Sutherland 算法裁剪直线
bool CohenSutherlandLineClip(float& x0, float& y0, float& x1, float& y1, float xmin, float ymin, float xmax, float ymax)
{
    OutCode outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
    OutCode outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);

    bool accept = false;

    while (true) {
        if (!(outcode0 | outcode1)) {
            // 两个端点都在窗口内
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            // 两个端点都在窗口外，并且在相同区域
            break;
        } else {
            // 至��一个端点在窗口外
            float x, y;

            // 选择一个在窗口外的端点
            OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

            if (outcodeOut & TOP) { // 点在窗口上方
                x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                y = ymax;
            } else if (outcodeOut & BOTTOM) { // 点在窗口下方
                x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                y = ymin;
            } else if (outcodeOut & RIGHT) { // 点在窗口右侧
                y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                x = xmax;
            } else if (outcodeOut & LEFT) { // 点在窗口左侧
                y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                x = xmin;
            }

            // 更新点的位置，并重新计算区域码
            if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
            } else {
                x1 = x;
                y1 = y;
                outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            }
        }
    }

    return accept;
}

// 判断点是否在裁剪窗口内
bool IsInside(const ImVec2& point, const ClipWindow& clipWindow, int edge) {
    switch (edge) {
    case 0: // 左边界
        return point.x >= clipWindow.x0;
    case 1: // 右边界
        return point.x <= clipWindow.x1;
    case 2: // 下边界
        return point.y >= clipWindow.y0;
    case 3: // 上边界
        return point.y <= clipWindow.y1;
    }
    return false;
}

// 计算交点
ImVec2 ComputeIntersection(const ImVec2& p1, const ImVec2& p2, const ClipWindow& clipWindow, int edge) {
    float x, y;

    switch (edge) {
    case 0: // 左边界
        x = clipWindow.x0;
        y = p1.y + (p2.y - p1.y) * (clipWindow.x0 - p1.x) / (p2.x - p1.x);
        break;
    case 1: // 右边界
        x = clipWindow.x1;
        y = p1.y + (p2.y - p1.y) * (clipWindow.x1 - p1.x) / (p2.x - p1.x);
        break;
    case 2: // 下边界
        y = clipWindow.y0;
        x = p1.x + (p2.x - p1.x) * (clipWindow.y0 - p1.y) / (p2.y - p1.y);
        break;
    case 3: // 上边界
        y = clipWindow.y1;
        x = p1.x + (p2.x - p1.x) * (clipWindow.y1 - p1.y) / (p2.y - p1.y);
        break;
    }

    return ImVec2(x, y);
}

// Sutherland-Hodgman 多边形裁剪算法
std::vector<ImVec2> SutherlandHodgmanPolygonClip(const std::vector<ImVec2>& polygon, const ClipWindow& clipWindow) {
    std::vector<ImVec2> inputList = polygon;
    std::vector<ImVec2> outputList;

    for (int edge = 0; edge < 4; ++edge) { // 对每条裁剪边进行裁剪
        outputList.clear();

        ImVec2 prevPoint = inputList.back(); // 多边形最后一个点
        for (const auto& curPoint : inputList) {
            if (IsInside(curPoint, clipWindow, edge)) { // 当前点在裁剪窗口内
                if (!IsInside(prevPoint, clipWindow, edge)) {
                    // 上一个点在裁剪窗口外，计算交点
                    outputList.push_back(ComputeIntersection(prevPoint, curPoint, clipWindow, edge));
                }
                // 添加当前点
                outputList.push_back(curPoint);
            } else if (IsInside(prevPoint, clipWindow, edge)) {
                // 当前点在裁剪窗口外，上一个点在窗口内，计算交点
                outputList.push_back(ComputeIntersection(prevPoint, curPoint, clipWindow, edge));
            }
            prevPoint = curPoint;
        }

        inputList = outputList; // 更新输入点集
    }

    return outputList;
}

//exp13
// 判断两条线段是否相交并计算交点
bool IsIntersect(
    const ImVec2& A, const ImVec2& B,
    const ImVec2& C, const ImVec2& D,
    ImVec2& intersection, float& t) 
{
    ImVec2 b = ImVec2(B.x - A.x, B.y - A.y);  // 向量 AB
    ImVec2 d = ImVec2(D.x - C.x, D.y - C.y);  // 向量 CD
    ImVec2 c = ImVec2(C.x - A.x, C.y - A.y);
    ImVec2 d_perp = ImVec2(-d.y, d.x); // CD 的垂直向量

    float denom = b.x * d_perp.x + b.y * d_perp.y;
    if (std::fabs(denom) < 1e-6f)
        return false; // 平行或共线

    t = (c.x * d_perp.x + c.y * d_perp.y) / denom;
    float u = (c.x * (-b.y) + c.y * b.x) / denom;

    if (t < 0.0f || t > 1.0f)
        return false; // 交点不在线段 AB 内

    if (u < 0.0f || u > 1.0f)
        return false; // 交点不在线段 CD 内

    intersection = ImVec2(A.x + t * b.x, A.y + t * b.y);
    return true;
}

// Weiler-Atherton 多边形裁剪���法实现
std::vector<std::vector<ImVec2>> WeilerAthertonPolygonClip(
    const std::vector<ImVec2>& subjectPolygon,
    const std::vector<ImVec2>& clipPolygon) 
{
    // 复制原始多边形以插入交点
    std::vector<ImVec2> subject = subjectPolygon;
    std::vector<ImVec2> clip = clipPolygon;

    // 存储交点信息
    std::vector<IntersectionPoint> subjectIntersections;
    std::vector<IntersectionPoint> clipIntersections;

    // 查找并插入交点
    for (size_t i = 0; i < subject.size(); ++i) {
        size_t next_i = (i + 1) % subject.size();
        ImVec2 A = subject[i];
        ImVec2 B = subject[next_i];
        for (size_t j = 0; j < clip.size(); ++j) {
            size_t next_j = (j + 1) % clip.size();
            ImVec2 C = clip[j];
            ImVec2 D = clip[next_j];
            ImVec2 intersection;
            float t;
            if (IsIntersect(A, B, C, D, intersection, t)) {
                // 插入交点到 subject 多边形
                subject.insert(subject.begin() + i + 1, intersection);
                subjectIntersections.emplace_back(intersection, false); // 初始未标记
                ++i; // 跳过刚插入的交点

                // 插入交点到 clip 多边形
                clip.insert(clip.begin() + j + 1, intersection);
                clipIntersections.emplace_back(intersection, false);
                ++j; // 跳过刚插入的交点
            }
        }
    }

    // 如果没有交点，返回空结果或根据情况返回完整主多边形
    if (subjectIntersections.empty()) {
        // 检查是否完全 inside
        // 简单判断第一个点是否在裁剪窗口内
        // 可以使用射线法或其他方法
        // 这里假设使用 Sutherland-Hodgman 内部判断
        // 为简化，此处返回空
        return {};
    }

    // 标记交点为入口或出口
    // 假设裁剪多边形是逆时针排列
    for (auto& inter : subjectIntersections) {
        // 找出当前交点在 subject 和 clip 中的位置
        size_t subj_idx = std::distance(subject.begin(),
            std::find(subject.begin(), subject.end(), inter.point));
        size_t clip_idx = std::distance(clip.begin(),
            std::find(clip.begin(), clip.end(), inter.point));

        // 获取前一个点和后一个点
        ImVec2 prev = (subj_idx == 0) ? subject.back() : subject[subj_idx - 1];
        ImVec2 next = (subj_idx + 1 < subject.size()) ? subject[subj_idx + 1] : subject[0];

        // 计算向量 AB 和 BC 的叉积
        ImVec2 AB = ImVec2(next.x - inter.point.x, next.y - inter.point.y);
        ImVec2 BC = ImVec2(clip[(clip_idx + 1) % clip.size()].x - inter.point.x,
                           clip[(clip_idx + 1) % clip.size()].y - inter.point.y);
        float cross = AB.x * BC.y - AB.y * BC.x;

        // 如果叉积 > 0，则是进入
        inter.isEntering = (cross > 0);
    }

    // 遍历交点，构建裁剪后的多边形
    std::vector<std::vector<ImVec2>> resultPolygons;

    for (auto& startInter : subjectIntersections) {
        if (startInter.visited || !startInter.isEntering)
            continue;

        std::vector<ImVec2> clippedPolygon;
        IntersectionPoint* currentInter = &startInter;
        bool traverseSubject = true;

        while (currentInter != nullptr) {
            if (currentInter->visited)
                break;

            currentInter->visited = true;
            clippedPolygon.emplace_back(currentInter->point);

            if (traverseSubject) {
                // 从当前交点开始，遍历 subject 多边形
                size_t subj_idx = std::distance(subject.begin(),
                    std::find(subject.begin(), subject.end(), currentInter->point));
                size_t next_subj_idx = (subj_idx + 1) % subject.size();

                while (next_subj_idx != subj_idx) {
                    ImVec2 currentPoint = subject[next_subj_idx];
                    clippedPolygon.emplace_back(currentPoint);

                    // 检查是否是另一个交点
                    auto it = std::find_if(subjectIntersections.begin(),
                        subjectIntersections.end(),
                        [&](const IntersectionPoint& ip) {
                            return ip.point.x == currentPoint.x && ip.point.y == currentPoint.y;
                        });

                    if (it != subjectIntersections.end()) {
                        currentInter = &(*it);
                        traverseSubject = false;
                        break;
                    }

                    next_subj_idx = (next_subj_idx + 1) % subject.size();
                }

                if (next_subj_idx == subj_idx) {
                    currentInter = nullptr;
                }
            }
            else {
                // 从当前交点开始，遍历 clip 多边形
                size_t clip_idx = std::distance(clip.begin(),
                    std::find(clip.begin(), clip.end(), currentInter->point));
                size_t next_clip_idx = (clip_idx + 1) % clip.size();

                while (next_clip_idx != clip_idx) {
                    ImVec2 currentPoint = clip[next_clip_idx];
                    clippedPolygon.emplace_back(currentPoint);

                    // 检查是否是另一个交点
                    auto it = std::find_if(clipIntersections.begin(),
                        clipIntersections.end(),
                        [&](const IntersectionPoint& ip) {
                            return ip.point.x == currentPoint.x && ip.point.y == currentPoint.y;
                        });

                    if (it != clipIntersections.end()) {
                        currentInter = &(*it);
                        traverseSubject = true;
                        break;
                    }

                    next_clip_idx = (next_clip_idx + 1) % clip.size();
                }

                if (next_clip_idx == clip_idx) {
                    currentInter = nullptr;
                }
            }
        }

        if (clippedPolygon.size() >= 3)
            resultPolygons.push_back(clippedPolygon);
    }

    return resultPolygons;
}

// Clip rectangles to a window
std::vector<std::pair<ImVec2, ImVec2>> ClipRectanglesToWindow(
    const std::vector<std::pair<ImVec2, ImVec2>>& rectangles,
    const ImVec2& windowTopLeft,
    const ImVec2& windowBottomRight,
    const ImVec2& windowSize,
    const ImVec2& viewportTopLeft,
    const ImVec2& viewportSize) {
    
    std::vector<std::pair<ImVec2, ImVec2>> clippedRectangles;

    for (const auto& rect : rectangles) {
        ImVec2 topLeft = windowTopLeft + rect.first;
        ImVec2 bottomRight = windowTopLeft + rect.second;

        // Clip to window boundaries
        ImVec2 clippedTopLeft = ImVec2(
            std::max(topLeft.x, windowTopLeft.x),
            std::max(topLeft.y, windowTopLeft.y));
        ImVec2 clippedBottomRight = ImVec2(
            std::min(bottomRight.x, windowBottomRight.x),
            std::min(bottomRight.y, windowBottomRight.y));

        // Ensure valid rectangle
        if (clippedTopLeft.x < clippedBottomRight.x && clippedTopLeft.y < clippedBottomRight.y) {
            clippedRectangles.emplace_back(clippedTopLeft, clippedBottomRight);
        }
    }

    return clippedRectangles;
}