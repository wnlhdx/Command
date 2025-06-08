import math


class ForceVector:
    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y

    def magnitude(self):
        return math.hypot(self.x, self.y)

    def direction_deg(self):
        return math.degrees(math.atan2(self.y, self.x))

    def __add__(self, other):
        return ForceVector(self.x + other.x, self.y + other.y)

    def dot(self, other):
        return self.x * other.x + self.y * other.y

    def angle_with(self, other):
        """返回夹角，单位为度"""
        mag1 = self.magnitude()
        mag2 = other.magnitude()
        if mag1 == 0 or mag2 == 0:
            raise ValueError("不能对零向量求夹角")
        cos_theta = self.dot(other) / (mag1 * mag2)
        # 防止浮点误差越界
        cos_theta = max(-1.0, min(1.0, cos_theta))
        angle_rad = math.acos(cos_theta)
        return math.degrees(angle_rad)

    def __repr__(self):
        return f"ForceVector(x={self.x:.2f}, y={self.y:.2f})"


class ForceSystem:
    def __init__(self):
        self.forces = []

    def add_force(self, force: ForceVector):
        self.forces.append(force)

    def net_force(self):
        total_x = sum(f.x for f in self.forces)
        total_y = sum(f.y for f in self.forces)
        return ForceVector(total_x, total_y)

    def is_equilibrium(self, tolerance=1e-6):
        net = self.net_force()
        return abs(net.x) < tolerance and abs(net.y) < tolerance




# 三个力矢量（均为笛卡尔坐标）
f1 = ForceVector(5, 0)       # 向右
f2 = ForceVector(-3, 2.598)  # 向左上（60°）
f3 = ForceVector(-2, -2.598) # 向左下（-60°）

fs = ForceSystem()
fs.add_force(f1)
fs.add_force(f2)
fs.add_force(f3)

net = fs.net_force()
print("合力为：", net)
print("系统是否平衡：", fs.is_equilibrium())

f1 = ForceVector(1, 0)    # x 轴正方向
f2 = ForceVector(0, 1)    # y 轴正方向

angle = f1.angle_with(f2)
print(f"f1 与 f2 的夹角为：{angle:.2f}°")  # 输出：90.00°