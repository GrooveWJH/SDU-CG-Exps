from qucikdrawlib import QuickDrawLib

# 创建QuickDrawLib实例
DrawBoard = QuickDrawLib(grid_size=20)

A = (0, 0)
B = (15, 2)
# B = (15,40)

DrawBoard.draw_line(A, B)

def Bersenham(DrawBoard: QuickDrawLib, A: tuple, B: tuple):
    # 解构起点和终点的坐标
    x_0, y_0 = A
    x_1, y_1 = B

    # 计算x和y方向上的差值
    dx = abs(x_1 - x_0)
    dy = abs(y_1 - y_0)

    # 确定x和y方向上的步进方向
    sx = 1 if x_0 < x_1 else -1
    sy = 1 if y_0 < y_1 else -1

    # 初始化误差值
    err = dx - dy

    while True:
        # 将当前格子涂成灰色
        DrawBoard.turn_cell_gray(x_0, y_0)

        # 如果当前点是终点，则退出循环
        if x_0 == x_1 and y_0 == y_1:
            break

        # 计算两倍的误差值
        e2 = 2 * err

        # 如果误差值大于-y方向的差值，调整x坐标
        if e2 > -dy:
            err -= dy
            x_0 += sx

        # 如果误差值小于x方向的差值，调整y坐标
        if e2 < dx:
            err += dx
            y_0 += sy


def myBersenham(DrawBoard: QuickDrawLib, A: tuple, B: tuple):
    if (B[1]-A[1])/(B[0]-A[0]) > 0.5:
        x_0, y_0 = B
        x_1, y_1 = A
    else:
        x_0, y_0 = A
        x_1, y_1 = B
    dx = abs(x_1 - x_0)
    dy = abs(y_1 - y_0)

    def ENorE(step, increase):
        return 2*step*dy - (2*increase + 1)*dx < 0

    increase = 0

    for step in range(0, dx+1):
        if ENorE(step, increase):
            DrawBoard.turn_cell_gray(x_0 + step, y_0 + increase)
        else:
            increase += 1
            DrawBoard.turn_cell_gray(x_0 + step, y_0 + increase)


# 显示结果
myBersenham(DrawBoard, A, B)
# Bersenham(DrawBoard, A, B)
DrawBoard.show()
