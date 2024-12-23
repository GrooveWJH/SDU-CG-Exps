import matplotlib.pyplot as plt
import matplotlib.patches as patches

class QuickDrawLib:
    def __init__(self, grid_size=10, cell_size=1):
        """
        初始化QuickDrawLib类.
        
        参数:
        - grid_size: 格子的数量(每行每列)
        - cell_size: 每个格子的大小
        """
        self.grid_size = grid_size
        self.cell_size = cell_size
        self.fig, self.ax = plt.subplots(figsize=(6, 6))
        self._setup_grid()

    def _setup_grid(self):
        """设置背景格子."""
        for x in range(self.grid_size + 1):
            self.ax.axhline(x, color='lightgray', linewidth=0.5)
            self.ax.axvline(x, color='lightgray', linewidth=0.5)
        
        self.ax.set_xlim(0, self.grid_size)
        self.ax.set_ylim(0, self.grid_size)
        self.ax.set_aspect('equal')
        self.ax.set_xticks([])
        self.ax.set_yticks([])

    def turn_cell_gray(self, cell_x, cell_y):
        """
        将指定的格子变为灰色.
        
        参数:
        - cell_x: 格子的x坐标
        - cell_y: 格子的y坐标
        """
        if cell_x == int(cell_x) and cell_y == int(cell_y):
            rect = patches.Rectangle((cell_x, cell_y), self.cell_size, self.cell_size, linewidth=1, edgecolor='black', facecolor='black', alpha=1)
            self.ax.add_patch(rect)

    def draw_line(self, start: tuple, end: tuple):
        """
        在格子上绘制一条线。
        
        参数:
        - start: 起点坐标 (x, y)
        - end: 终点坐标 (x, y)
        """
        x1, y1 = start
        x2, y2 = end
        self.ax.plot(x1 + 0.5, y1 + 0.5, 'ro')  # 在起点放置红色圆球
        self.ax.plot(x2 + 0.5, y2 + 0.5, 'go')  # 在终点放置红色圆球
        self.ax.plot([x1+0.5, x2+0.5], [y1+0.5, y2+0.5], color='yellow', linewidth=1)
        

    def show(self):
        """显示绘制的图形。"""
        plt.show()
