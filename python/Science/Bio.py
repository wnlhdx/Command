import random
import matplotlib.pyplot as plt

class Cell:
    def __init__(self):
        # 主要化学元素的初始比例
        self.elements = {
            "O": 65.0,  # 氧
            "C": 18.5,  # 碳
            "H": 9.5,   # 氢
            "N": 3.3,   # 氮
            "Ca": 1.5,  # 钙
            "Fe": 0.01  # 铁（微量元素）
        }
        self.health = 100  # 细胞健康值

    def update_environment(self, environment):
        """根据环境调整细胞状态"""
        for element, amount in environment.items():
            if element in self.elements:
                if self.elements[element] < amount:
                    # 元素不足，健康下降
                    self.health -= (amount - self.elements[element]) * 0.1
                elif self.elements[element] > amount:
                    # 元素过多，健康下降
                    self.health -= (self.elements[element] - amount) * 0.1

        # 有毒元素的影响
        if "As" in environment and environment["As"] > 0:
            self.health -= environment["As"] * 0.5  # 砷的毒性

        self.health = max(self.health, 0)  # 健康值不能低于0

    def adapt(self):
        """简单进化模拟：提高适应性"""
        adaptation_factor = random.uniform(0.1, 0.5)
        for element in self.elements:
            self.elements[element] += adaptation_factor  # 增强适应性

    def __str__(self):
        return f"Cell health: {self.health:.2f}, Elements: {self.elements}"


# 环境定义
environment = {
    "O": 60.0,  # 氧气略低
    "C": 20.0,  # 碳稍多
    "H": 9.0,   # 氢稍少
    "N": 4.0,   # 氮多
    "Ca": 1.0,  # 钙少
    "As": 2.0   # 环境中有砷
}

# 模拟过程
cell = Cell()
health_over_time = []
for day in range(10):
    print(f"Day {day + 1}")
    cell.update_environment(environment)
    print(cell)
    health_over_time.append(cell.health)
    if cell.health < 50:
        print("Adapting...")
        cell.adapt()

# 绘制健康变化图
plt.plot(health_over_time)
plt.title("Cell Health Over Time")
plt.xlabel("Days")
plt.ylabel("Health")
plt.show()
