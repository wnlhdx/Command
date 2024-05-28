import scipy.integrate as spi
import numpy as np

# 物理参数
u = 0.0  # 初始速度 (m/s)
a = 9.8  # 加速度 (m/s^2) - 在地球表面，这大约等于重力加速度

# 时间点
t = np.linspace(0, 10, 100)  # 从0到10秒，分成100个时间点

# 微分方程 dx/dt = v, dv/dt = a
def motion_equations(state, t):
    x, v = state
    dxdt = v
    dvdt = a
    return [dxdt, dvdt]

# 初始状态 [x0, v0]
initial_state = [0, u]

# 使用SciPy的积分函数求解微分方程
result = spi.odeint(motion_equations, initial_state, t)

# 提取结果
x = result[:, 0]  # 位置
v = result[:, 1]  # 速度

# 打印结果
for i in range(len(t)):
    print(f"时间: {t[i]:.2f} s, 位置: {x[i]:.2f} m, 速度: {v[i]:.2f} m/s")

# 如果需要，可以绘制图表
import matplotlib.pyplot as plt

plt.figure(figsize=(10, 4))
plt.subplot(1, 2, 1)
plt.plot(t, x, 'b')
plt.title('位置随时间变化')
plt.xlabel('时间 (s)')
plt.ylabel('位置 (m)')

plt.subplot(1, 2, 2)
plt.plot(t, v, 'r')
plt.title('速度随时间变化')
plt.xlabel('时间 (s)')
plt.ylabel('速度 (m/s)')

plt.tight_layout()
plt.show()