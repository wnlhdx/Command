import pyasl

# 创建一个时间序列
t = np.linspace(0, 10, 100)  # 从0到10天，分成100个时间点

# 初始参数
earth_pos = np.array([0.0, 0.0, 0.0])  # 地球初始位置 (单位：天文单位)
sun_pos = np.array([0.0, 0.0, 0.0])  # 太阳初始位置 (单位：天文单位)
moon_pos = np.array([0.0, 0.0, 0.0])  # 月球初始位置 (单位：天文单位)

# 使用PyAstronomy库模拟天体运动
earth_pos = pyasl.KeplerianElements.from_planet_mass_radius(earth_pos, earth_pos)
sun_pos = pyasl.KeplerianElements.from_planet_mass_radius(sun_pos, sun_pos)
moon_pos = pyasl.KeplerianElements.from_planet_mass_radius(moon_pos, moon_pos)

# 计算天体在时间t的位置
for i in range(len(t)):
    earth_pos = pyasl.KeplerianElements.integrate_orbit(earth_pos, sun_pos, t[i])
    moon_pos = pyasl.KeplerianElements.integrate_orbit(moon_pos, earth_pos, t[i])

    # 打印天体在时间t的位置
    print(f"时间: {t[i]:.2f} 天, 地球位置: {earth_pos[0]:.2f}, {earth_pos[1]:.2f}, {earth_pos[2]:.2f} 天文单位, 月球位置: {moon_pos[0]:.2f}, {moon_pos[1]:.2f}, {moon_pos[2]:.2f} 天文单位")

# 如果需要，可以绘制图表
import matplotlib.pyplot as plt

plt.figure(figsize=(10, 4))
plt.subplot(1, 2, 1)
plt.plot(t, earth_pos[:, 0], 'b-', label='地球位置 (x)')
plt.plot(t, earth_pos[:, 1], 'g-', label='地球位置 (y)')
plt.plot(t, earth_pos[:, 2], 'r-', label='地球位置 (z)')
plt.title('地球位置随时间变化')
plt.xlabel('时间 (天)')
plt.ylabel('位置 (天文单位)')
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(t, moon_pos[:, 0], 'b-', label='月球位置 (x)')
plt.plot(t, moon_pos[:, 1], 'g-', label='月球位置 (y)')
plt.plot(t, moon_pos[:, 2], 'r-', label='月球位置 (z)')
plt.title('月球位置随时间变化')
plt.xlabel('时间 (天)')
plt.ylabel('位置 (天文单位)')
plt.legend()

plt.tight_layout()
plt.show()
