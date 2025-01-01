import numpy as np
import matplotlib.pyplot as plt

class UniversalMotionModel:
    def __init__(self, initial_position=(0, 0), initial_velocity=(0, 0), acceleration=(0, 0), time_end=10, time_step=0.1):
        self.x0, self.y0 = initial_position  # 初始位置 x0, y0
        self.v0x, self.v0y = initial_velocity  # 初始速度 vx0, vy0
        self.ax, self.ay = acceleration  # 加速度 ax, ay
        self.t_end = time_end  # 总时间
        self.dt = time_step  # 时间步长
        
        # 时间数组
        self.time = np.arange(0, self.t_end, self.dt)
        self.position_x = np.zeros(len(self.time))  # x方向位置数组
        self.position_y = np.zeros(len(self.time))  # y方向位置数组
        self.velocity_x = np.zeros(len(self.time))  # x方向速度数组
        self.velocity_y = np.zeros(len(self.time))  # y方向速度数组

    def calculate_trajectory(self):
        # 初始条件
        self.position_x[0] = self.x0
        self.position_y[0] = self.y0
        self.velocity_x[0] = self.v0x
        self.velocity_y[0] = self.v0y

        # 根据输入的加速度和初始条件，计算质点的运动轨迹
        for i in range(1, len(self.time)):
            # 计算速度
            self.velocity_x[i] = self.velocity_x[i-1] + self.ax * self.dt
            self.velocity_y[i] = self.velocity_y[i-1] + self.ay * self.dt

            # 计算位置
            self.position_x[i] = self.position_x[i-1] + self.velocity_x[i-1] * self.dt + 0.5 * self.ax * self.dt**2
            self.position_y[i] = self.position_y[i-1] + self.velocity_y[i-1] * self.dt + 0.5 * self.ay * self.dt**2

    def plot_motion(self):
        # 绘制运动轨迹
        plt.plot(self.time, self.position_x, label="Position X")
        plt.plot(self.time, self.position_y, label="Position Y")
        plt.title("Universal Motion")
        plt.xlabel("Time (s)")
        plt.ylabel("Position (m)")
        plt.legend()
        plt.grid(True)
        plt.show()

# 匀速运动函数
def uniform_motion(initial_position=(0, 0), initial_velocity=(0, 0), time_end=10, time_step=0.1):
    motion = UniversalMotionModel(initial_position, initial_velocity, acceleration=(0, 0), time_end=time_end, time_step=time_step)
    motion.calculate_trajectory()
    return motion

# 匀加速运动函数
def uniformly_accelerated_motion(initial_position=(0, 0), initial_velocity=(0, 0), acceleration=(0, 0), time_end=10, time_step=0.1):
    motion = UniversalMotionModel(initial_position, initial_velocity, acceleration=acceleration, time_end=time_end, time_step=time_step)
    motion.calculate_trajectory()
    return motion

# 自由落体运动函数
def free_fall_motion(initial_position=(0, 0), initial_velocity=(0, 0), time_end=10, time_step=0.1):
    # 自由落体运动只考虑竖直方向加速度为重力加速度
    motion = UniversalMotionModel(initial_position, initial_velocity, acceleration=(0, -9.8), time_end=time_end, time_step=time_step)
    motion.calculate_trajectory()
    return motion

# 抛体运动函数
def projectile_motion(initial_position=(0, 0), initial_velocity=(0, 0), time_end=10, time_step=0.1):
    # 抛体运动水平和竖直方向的加速度
    motion = UniversalMotionModel(initial_position, initial_velocity, acceleration=(0, -9.8), time_end=time_end, time_step=time_step)
    motion.calculate_trajectory()
    return motion

# 示例：匀速运动
motion_uniform = uniform_motion(initial_position=(0, 0), initial_velocity=(2, 0), time_end=10, time_step=0.1)
motion_uniform.plot_motion()

# 示例：匀加速运动
motion_uniform_accel = uniformly_accelerated_motion(initial_position=(0, 0), initial_velocity=(0, 0), acceleration=(2, 3), time_end=10, time_step=0.1)
motion_uniform_accel.plot_motion()

# 示例：自由落体
motion_fall = free_fall_motion(initial_position=(0, 100), initial_velocity=(0, 0), time_end=10, time_step=0.1)
motion_fall.plot_motion()

# 示例：抛体运动
motion_projectile = projectile_motion(initial_position=(0, 0), initial_velocity=(10, 20), time_end=5, time_step=0.1)
motion_projectile.plot_motion()
