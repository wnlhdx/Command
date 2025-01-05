import numpy as np
import matplotlib.pyplot as plt
from scipy import stats


# 创建一个类来模拟物质
class Substance:
    def __init__(self, name, state, density, is_pure):
        self.name = name
        self.state = state
        self.density = density
        self.is_pure = is_pure

    def display_properties(self):
        print(f"Substance: {self.name}")
        print(f"State: {self.state}")
        print(f"Density: {self.density} g/cm³")
        print(f"Pure Substance: {self.is_pure}")
        print("-" * 30)


# 示例物质
water = Substance("Water", "Liquid", 1.0, True)
salt = Substance("Salt", "Solid", 2.165, True)
air = Substance("Air", "Gas", 0.001225, False)

# 显示物质属性
water.display_properties()
salt.display_properties()
air.display_properties()


# 计算动能和势能
def calculate_energy(mass, height):
    g = 9.81  # 重力加速度 (m/s²)

    # 势能（PE = m * g * h）
    potential_energy = mass * g * height

    # 动能（KE = 0.5 * m * v²）
    velocity = np.sqrt(2 * g * height)  # 自由落体下落的速度
    kinetic_energy = 0.5 * mass * velocity ** 2

    return potential_energy, kinetic_energy


# 物体的质量和高度
mass = 5  # 质量（kg）
heights = np.linspace(0, 100, 100)  # 从0到100米的高度

# 计算每个高度下的势能和动能
potential_energies = []
kinetic_energies = []
for height in heights:
    PE, KE = calculate_energy(mass, height)
    potential_energies.append(PE)
    kinetic_energies.append(KE)

# 绘制能量图
plt.plot(heights, potential_energies, label='Potential Energy (J)')
plt.plot(heights, kinetic_energies, label='Kinetic Energy (J)')
plt.xlabel('Height (m)')
plt.ylabel('Energy (Joules)')
plt.title('Potential and Kinetic Energy vs. Height')
plt.legend()
plt.grid(True)
plt.show()


# 单位转换函数
def convert_units(value, from_unit, to_unit):
    conversion_factors = {
        'kg_to_g': 1000,
        'g_to_kg': 0.001,
        'm_to_cm': 100,
        'cm_to_m': 0.01,
        'l_to_ml': 1000,
        'ml_to_l': 0.001
    }

    conversion_key = f"{from_unit}_to_{to_unit}"

    if conversion_key in conversion_factors:
        return value * conversion_factors[conversion_key]
    else:
        raise ValueError("Invalid conversion units.")


# 示例转换
mass_in_kg = 2
mass_in_g = convert_units(mass_in_kg, 'kg', 'g')
print(f"{mass_in_kg} kg = {mass_in_g} g")

# 误差分析：使用标准偏差来分析一组测量数据的准确度
measurements = [2.5, 2.6, 2.55, 2.45, 2.65]  # 示例测量数据
mean = np.mean(measurements)
std_dev = np.std(measurements)

print(f"Mean: {mean}, Standard Deviation: {std_dev}")

# 可视化测量数据的误差
plt.hist(measurements, bins=5, alpha=0.7, color='blue', edgecolor='black')
plt.axvline(mean, color='red', linestyle='dashed', linewidth=2, label=f'Mean = {mean}')
plt.axvline(mean - std_dev, color='green', linestyle='dashed', linewidth=2, label=f'Standard Deviation = ±{std_dev}')
plt.axvline(mean + std_dev, color='green', linestyle='dashed', linewidth=2)
plt.legend()
plt.title('Measurement Distribution and Error Analysis')
plt.xlabel('Measurement (kg)')
plt.ylabel('Frequency')
plt.show()
