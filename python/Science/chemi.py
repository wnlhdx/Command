class WaterElectrolysis:
    def __init__(self):
        self.hydrogen = 0  # 氢气分子数量
        self.oxygen = 0    # 氧气分子数量
        self.water = 0     # 水分子数量

    def electrolyze(self, water_molecules):
        # 根据化学方程式，每2个水分子可以分解为2个氢气分子和1个氧气分子
        self.hydrogen += water_molecules * 2
        self.oxygen += water_molecules
        self.water -= water_molecules

    def display_results(self):
        print(f"氢气分子数量: {self.hydrogen}")
        print(f"氧气分子数量: {self.oxygen}")
        print(f"水分子数量: {self.water}")

# 模拟电解过程
electrolysis = WaterElectrolysis()

# 假设有100个水分子
electrolysis.water = 100

# 电解50个水分子
electrolysis.electrolyze(50)

# 显示结果
electrolysis.display_results()
