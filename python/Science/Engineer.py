import numpy as np
from scipy.constants import g
from sympy import symbols, Eq, solve

# 定义木块的尺寸
L, W, H = 1, 1, 1  # 假设木块的尺寸为1米

# 定义外力
F = 1000  # 垂直于底面的力，单位：牛顿
P = 500   # 水平力，单位：牛顿
Q = 200   # 垂直于侧面的力，单位：牛顿

# 木块的质量，假设为1千克
m = 1

# 重力加速度，单位：米/秒^2
g = 9.81

# 计算重力
G = m * g

# 定义力的矢量
F_vector = np.array([0, 0, F])  # 垂直力
P_vector = np.array([P, 0, 0])  # 水平力
Q_vector = np.array([0, 0, Q])  # 垂直力

# 计算质心位置
center_of_mass = np.array([L / 2, W / 2, H / 2])

# 计算每个力对质心的力矩
moment_F = np.cross(F_vector, center_of_mass)
moment_P = np.cross(P_vector, center_of_mass)
moment_Q = np.cross(Q_vector, center_of_mass)

# 计算总力矩
total_moment = moment_F + moment_P + moment_Q

# 检查总力矩是否为零，如果为零，则木块处于静止状态
if np.linalg.norm(total_moment) == 0:
    print("木块处于静止状态。")
else:
    print("木块不处于静止状态。")

# 定义符号变量
x, y, Fx, Fy = symbols('x y Fx Fy')

# 定义力的矢量
force_vector = [Fx, Fy]

# 假设木块的质心位于其几何中心
center_of_mass = [L / 2, W / 2]

# 计算每个力对质心的力矩
moment_F = np.cross(force_vector, center_of_mass)

# 检查总力矩是否为零，如果为零，则木块处于静止状态
total_moment = moment_F.dot(center_of_mass)
equation = Eq(total_moment, 0)

# 求解方程
solution = solve(equation, Fx)

print(f"水平力 Fx 的值：{solution[0]}")